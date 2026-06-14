#include "World/ARepairable.h"

#include "AvaryoCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Items/APickupItem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "UI/AvaryoCameraShakes.h"
#include "UObject/ConstructorHelpers.h"

ARepairable::ARepairable()
{
	PrimaryActorTick.bCanEverTick = true; // прогресс на сервере + табличка на клиентах
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // чтобы трейс из камеры его видел

	StatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StatusText"));
	StatusText->SetupAttachment(MeshComponent);
	StatusText->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	StatusText->SetHorizontalAlignment(EHTA_Center);
	StatusText->SetWorldSize(28.f);

	// Аварийная лампа: в ночной темноте сломанный объект видно по красной пульсации
	AlarmLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("AlarmLight"));
	AlarmLight->SetupAttachment(MeshComponent);
	AlarmLight->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	AlarmLight->SetUsingAbsoluteScale(true); // масштаб меша не должен раздувать радиус света
	AlarmLight->SetLightColor(FColor(255, 40, 20));
	AlarmLight->SetIntensity(3000.f);
	AlarmLight->SetAttenuationRadius(700.f);
	AlarmLight->SetCastShadows(false); // дёшево: лампочек несколько, тени не нужны

	DisplayName = FText::FromString(TEXT("Объект"));
	RepairDuration = 8.f;
	RequiredTool = NAME_None;
	RepairRange = 350.f;
	bLeaksGasWhenBroken = false;
	GasRadius = 450.f;
	ExplosionDamage = 45.f;
	bBroken = true;
	RepairProgress = 0.f;
	NoiseAccum = 0.f;
	ExplosionCooldown = 0.f;
	GasSuppressedTime = 0.f;
	GasLeakElapsed = 0.f;
	GasSpreadPerSecond = 0.05f; // +5%/с — за ~20 с до максимума
	GasSpreadMaxScale = 2.0f;
	CurrentGasRadius = GasRadius;
	LastShownPercent = -1;

	// Звуки по умолчанию (можно переопределить в Blueprint)
	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSnd(TEXT("/Game/Audio/SFX/Hazard/Hazard_ExplosionGas_1.Hazard_ExplosionGas_1"));
	if (ExplosionSnd.Succeeded()) { ExplosionSound = ExplosionSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> RepairSnd(TEXT("/Game/Audio/SFX/RepairDone.RepairDone"));
	if (RepairSnd.Succeeded()) { RepairDoneSound = RepairSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> HitSnd(TEXT("/Game/Audio/SFX/Cues/SC_MinigameHit.SC_MinigameHit")); // случайный из 5
	if (HitSnd.Succeeded()) { MinigameHitSound = HitSnd.Object; }

	// VFX по умолчанию (из бесплатных паков; переопределяемы в Blueprint)
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExpFX(TEXT("/Game/NiagaraExamples/FX_Explosions/NS_Explosion.NS_Explosion"));
	if (ExpFX.Succeeded()) { ExplosionFX = ExpFX.Object; }
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SpkFX(TEXT("/Game/NiagaraExamples/FX_Sparks/NS_Spark_Burst.NS_Spark_Burst"));
	if (SpkFX.Succeeded()) { SparkFX = SpkFX.Object; }
	// Утечка газа = лёгкая струйка пара + шипение (НЕ густой дым «как горит»)
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> GasFX(TEXT("/Game/NiagaraExamples/Utilities/SpriteGeneration/SmokePuffLight/NS_SmokePuffLight.NS_SmokePuffLight"));
	if (GasFX.Succeeded()) { GasLeakFX = GasFX.Object; }
	// Звук утечки газа ВЫКЛ по просьбе (нынешний — «свист-свист»); вернём с нормальным газ-эффектом
	// static ConstructorHelpers::FObjectFinder<USoundBase> HissSnd(TEXT("/Game/Audio/SFX/GasHiss.GasHiss"));
	// if (HissSnd.Succeeded()) { GasHissSound = HissSnd.Object; }
	// Залив бензина (генератор) — настоящий бульк-луп (Magic Water glug)
	static ConstructorHelpers::FObjectFinder<USoundBase> FuelSnd(TEXT("/Game/Audio/SFX/Repair/Repair_FuelFill_Loop.Repair_FuelFill_Loop"));
	if (FuelSnd.Succeeded()) { FuelFillSound = FuelSnd.Object; }

	GasHissComp = CreateDefaultSubobject<UAudioComponent>(TEXT("GasHissAudio"));
	GasHissComp->SetupAttachment(MeshComponent);
	GasHissComp->bAutoActivate = false;
	GasHissComp->SetVolumeMultiplier(0.55f);
	if (GasHissSound) { GasHissComp->SetSound(GasHissSound); }
	// Затухание по расстоянию: вблизи слышно, вдали — еле-еле шипение
	GasHissComp->bOverrideAttenuation = true;
	GasHissComp->AttenuationOverrides.bAttenuate = true;
	GasHissComp->AttenuationOverrides.bSpatialize = true;
	GasHissComp->AttenuationOverrides.AttenuationShape = EAttenuationShape::Sphere;
	GasHissComp->AttenuationOverrides.AttenuationShapeExtents = FVector(150.f, 0.f, 0.f); // радиус полной громкости ~1.5 м
	GasHissComp->AttenuationOverrides.FalloffDistance = 2500.f; // дальше плавно гаснет до тишины (~26 м)

	// Установка расходника — металлический клик «вставлено» (Nut Driver), а не «уронил»
	static ConstructorHelpers::FObjectFinder<USoundBase> InsSnd(TEXT("/Game/Audio/SFX/Repair/SC_Insert.SC_Insert")); // случайный из 2
	if (InsSnd.Succeeded()) { InsertSound = InsSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> FillSnd(TEXT("/Game/Survival_SFX/Craft/Crafting_wood_item_1.Crafting_wood_item_1"));
	if (FillSnd.Succeeded()) { FillLoopSound = FillSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> WeldSnd(TEXT("/Game/Audio/SFX/WeldBuzz.WeldBuzz"));
	if (WeldSnd.Succeeded()) { WeldLoopSound = WeldSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> EngSnd(TEXT("/Game/Audio/SFX/EngineStart.EngineStart"));
	if (EngSnd.Succeeded()) { EngineStartSound = EngSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> ShortSnd(TEXT("/Game/Audio/SFX/ElectricZap.ElectricZap"));
	if (ShortSnd.Succeeded()) { ShortCircuitSound = ShortSnd.Object; }
	// Вентиль: трещотка ключа на тык + срыв резьбы (случайный вариант, чтобы не «долбило одно»)
	static ConstructorHelpers::FObjectFinder<USoundBase> ValveSnd(TEXT("/Game/Audio/SFX/Repair/SC_ValveRatchet.SC_ValveRatchet")); // случайный из 3
	if (ValveSnd.Succeeded()) { ValveTurnSound = ValveSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> SlipSnd(TEXT("/Game/Audio/SFX/Repair/SC_ValveStrip.SC_ValveStrip")); // случайный из 2
	if (SlipSnd.Succeeded()) { ValveSlipSound = SlipSnd.Object; }
	// Стартер: натяжение шнура (луп) + холостой ход двигателя после запуска (луп)
	static ConstructorHelpers::FObjectFinder<USoundBase> PullSnd(TEXT("/Game/Audio/SFX/Repair/Repair_GenPull_Loop.Repair_GenPull_Loop"));
	if (PullSnd.Succeeded()) { StarterPullLoopSound = PullSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> IdleSnd(TEXT("/Game/Audio/SFX/Repair/Repair_EngineIdle_Loop.Repair_EngineIdle_Loop"));
	if (IdleSnd.Succeeded()) { EngineIdleSound = IdleSnd.Object; }
	// Колхоз: возня подручным (Tire Changing Machine) — луп через FillAudioComp
	static ConstructorHelpers::FObjectFinder<USoundBase> BotchSnd(TEXT("/Game/Audio/SFX/Repair/Repair_JuryRig_Loop.Repair_JuryRig_Loop"));
	if (BotchSnd.Succeeded()) { BotchLoopSound = BotchSnd.Object; }
	// Низкий рокот-хвост взрыва (earthquake LFE) — слой под бабах
	static ConstructorHelpers::FObjectFinder<USoundBase> RumbleSnd(TEXT("/Game/Audio/SFX/Hazard/Hazard_Rumble_1.Hazard_Rumble_1"));
	if (RumbleSnd.Succeeded()) { ExplosionRumbleSound = RumbleSnd.Object; }

	// Луп заливки/прокладки/натяга — компонент, гоняется в Tick по состоянию
	FillAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FillAudio"));
	FillAudioComp->SetupAttachment(MeshComponent);
	FillAudioComp->bAutoActivate = false;
	if (FillLoopSound) { FillAudioComp->SetSound(FillLoopSound); }

	// Холостой ход генератора: отдельный луп, играет пока объект-стартер починен (3D-затухание).
	EngineIdleComp = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineIdleAudio"));
	EngineIdleComp->SetupAttachment(MeshComponent);
	EngineIdleComp->bAutoActivate = false;
	EngineIdleComp->SetVolumeMultiplier(0.5f);
	if (EngineIdleSound) { EngineIdleComp->SetSound(EngineIdleSound); }
	EngineIdleComp->bOverrideAttenuation = true;
	EngineIdleComp->AttenuationOverrides.bAttenuate = true;
	EngineIdleComp->AttenuationOverrides.bSpatialize = true;
	EngineIdleComp->AttenuationOverrides.AttenuationShape = EAttenuationShape::Sphere;
	EngineIdleComp->AttenuationOverrides.AttenuationShapeExtents = FVector(300.f, 0.f, 0.f); // полный объём ~3 м
	EngineIdleComp->AttenuationOverrides.FalloffDistance = 3000.f;                            // дальше гаснет

	MinigameType = ERepairMinigameType::None;
	HitsToRepair = 4;
	MinigameCursorSpeed = 0.9f;
	MinigameGreenHalfWidth = 0.07f;
	PanicHardenScale = 0.6f;
	ShockDamage = 15.f;
	ShockAoEDamage = 25.f;
	MissesBeforeLockout = 3;
	LockoutDuration = 60.f;
	CursorPos = 0.f;
	GreenCenter = 0.5f;
	MissCount = 0;
	LockoutRemaining = 0.f;
	CursorPhase = 0.f;
	MinigameSpeedMult = 1.f;

	ValveTurnAmount = 0.12f;
	ValveMinInterval = 0.7f;
	ValveSlipPenalty = 0.2f;
	ValveCooldown = 0.f;

	StarterChargeTime = 1.6f;
	StarterWindowStart = 0.7f;
	StarterWindowEnd = 0.9f;
	StarterPullsToFix = 3;
	StarterKickDamage = 5.f;
	StarterKickPanic = 5.f;
	StarterGraceTension = 0.15f;
	bStarterPulling = false;
	StarterTension = 0.f;

	bAllowBotch = true;
	BotchDurationMultiplier = 2.0f;
	BotchMishapChancePerSecond = 0.25f;
	BotchMishapProgressLoss = 0.15f;
	BotchMishapDamage = 6.f;
	BotchMishapPanic = 6.f;
	bBotching = false;

	PrereqIndex = 0;
	PrereqProgress = 0.f;
	bDoingPrereqHold = false;
	bPrereqAutoFilling = false;
	bDoingPrereqMinigame = false;
}

void ARepairable::BeginPlay()
{
	Super::BeginPlay();
	RefreshStatusVisual();
}

void ARepairable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARepairable, bBroken);
	DOREPLIFETIME(ARepairable, RepairProgress);
	DOREPLIFETIME(ARepairable, Repairer);
	DOREPLIFETIME(ARepairable, CursorPos);
	DOREPLIFETIME(ARepairable, GreenCenter);
	DOREPLIFETIME(ARepairable, MissCount);
	DOREPLIFETIME(ARepairable, LockoutRemaining);
	DOREPLIFETIME(ARepairable, ValveCooldown);
	DOREPLIFETIME(ARepairable, bStarterPulling);
	DOREPLIFETIME(ARepairable, StarterTension);
	DOREPLIFETIME(ARepairable, bBotching);
	DOREPLIFETIME(ARepairable, PrereqIndex);
	DOREPLIFETIME(ARepairable, PrereqProgress);
	DOREPLIFETIME(ARepairable, bDoingPrereqHold);
	DOREPLIFETIME(ARepairable, bPrereqAutoFilling);
	DOREPLIFETIME(ARepairable, bDoingPrereqMinigame);
}

bool ARepairable::GetCurrentStage(FRepairStage& OutStage) const
{
	if (PrereqIndex >= 0 && PrereqIndex < PrereqStages.Num())
	{
		OutStage = PrereqStages[PrereqIndex];
		return true;
	}
	return false;
}

bool ARepairable::NeedsInsertNow() const
{
	FRepairStage S;
	return bBroken && GetCurrentStage(S) && S.Kind == ERepairStageKind::InsertItem;
}

bool ARepairable::IsAutoFillStageNow() const
{
	FRepairStage S;
	return bBroken && GetCurrentStage(S) && S.Kind == ERepairStageKind::AutoFill;
}

bool ARepairable::IsMinigameStageNow() const
{
	FRepairStage S;
	return bBroken && GetCurrentStage(S) && S.Kind == ERepairStageKind::Minigame;
}

bool ARepairable::TryInsertBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || !Who || !bBroken)
	{
		return false;
	}
	FRepairStage S;
	if (!GetCurrentStage(S) || S.Kind != ERepairStageKind::InsertItem)
	{
		return false;
	}
	if (FVector::DistSquared(Who->GetActorLocation(), GetActorLocation()) > FMath::Square(RepairRange))
	{
		return false;
	}
	APickupItem* Held = Who->GetHeldItem();
	if (!Held || Held->ToolTag != S.ItemTag)
	{
		return false; // нужен правильный расходник в руках
	}
	// Потратить предмет (кабель/канистра/предохранитель)
	Who->ConsumeHeldItemCharge();
	PrereqIndex++;
	PrereqProgress = 0.f;
	MakeNoise(0.6f, Who, GetActorLocation());
	if (InsertSound) // установка — у всех
	{
		MulticastSound(InsertSound, GetActorLocation(), 1.f);
	}
	RefreshStatusVisual();
	return true;
}

void ARepairable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Луп заливки/прокладки (AutoFill) или сварки (удержание с инструментом) — на всех машинах
	if (FillAudioComp)
	{
		USoundBase* WantLoop = nullptr;
		if (bPrereqAutoFilling)
		{
			// Генератор (стартер) = залив бензина → бульканье; остальное (кабель) → прокладка
			WantLoop = (MinigameType == ERepairMinigameType::Starter && FuelFillSound) ? FuelFillSound : FillLoopSound;
		}
		else if (bDoingPrereqHold && !bBotching) { WantLoop = WeldLoopSound; } // сварка (колхоз руками — без дуги)
		else if (bStarterPulling && StarterPullLoopSound) { WantLoop = StarterPullLoopSound; } // натяг шнура стартера
		else if (bBotching && BotchLoopSound) { WantLoop = BotchLoopSound; } // колхоз: возня подручным
		if (WantLoop)
		{
			if (FillAudioComp->Sound != WantLoop) { FillAudioComp->SetSound(WantLoop); FillAudioComp->Play(); }
			else if (!FillAudioComp->IsPlaying()) { FillAudioComp->Play(); }
		}
		else if (FillAudioComp->IsPlaying())
		{
			FillAudioComp->Stop();
		}
	}

	// Холостой ход генератора: на всех машинах по реплицируемому bBroken — гудит, пока стартер-объект починен.
	if (EngineIdleComp && EngineIdleSound)
	{
		const bool bWantIdle = (MinigameType == ERepairMinigameType::Starter) && !bBroken;
		if (bWantIdle && !EngineIdleComp->IsPlaying()) { EngineIdleComp->Play(); }
		else if (!bWantIdle && EngineIdleComp->IsPlaying()) { EngineIdleComp->Stop(); }
	}

	// Сервер: блокировка после замыкания тает
	if (HasAuthority() && LockoutRemaining > 0.f)
	{
		LockoutRemaining = FMath::Max(LockoutRemaining - DeltaSeconds, 0.f);
	}

	// Сервер: тикаем починку
	if (HasAuthority() && Repairer)
	{
		if (!CanContinueRepair())
		{
			EndRepairBy(Repairer);
		}
		else if (bDoingPrereqHold || bPrereqAutoFilling)
		{
			// Hold-этап (держать E) ИЛИ AutoFill (полоска сама ползёт после нажатия E)
			FRepairStage S;
			if (GetCurrentStage(S))
			{
				// Паника растягивает заливку/удержание (трясущиеся руки) — как и курсорные мини-игры
				const float PanicSlow = 1.f + PanicHardenScale * RepairerPanic01();
				PrereqProgress = FMath::Min(PrereqProgress + DeltaSeconds / FMath::Max(S.Duration * PanicSlow, 0.1f), 1.f);
				NoiseAccum += DeltaSeconds;
				if (NoiseAccum >= 1.f)
				{
					NoiseAccum = 0.f;
					MakeNoise(1.f, Repairer, GetActorLocation());
				}
				if (PrereqProgress >= 1.f)
				{
					if (bPrereqAutoFilling && Repairer)
					{
						Repairer->ConsumeHeldItemCharge(); // кабель/расходник потрачен после установки
					}
					PrereqIndex++;
					PrereqProgress = 0.f;
					bDoingPrereqHold = false;
					bPrereqAutoFilling = false;
					if (Repairer)
					{
						Repairer->SetInteractionLocked(false);
					}
					Repairer = nullptr; // этап пройден — игрок заново жмёт E для следующего шага
					RefreshStatusVisual();
				}
			}
			else
			{
				bDoingPrereqHold = false;
				bPrereqAutoFilling = false;
				EndRepairBy(Repairer);
			}
		}
		else if (bDoingPrereqMinigame)
		{
			// Prereq-мини-игра (заварка/починка руками): курсор бегает, попадания/откат — в TryHitBy
			CursorPhase += DeltaSeconds * MinigameCursorSpeed * MinigameSpeedMult * (1.f + PanicHardenScale * RepairerPanic01()) / FMath::Max(0.25f, RepairerToolQuality());
			const float Saw = FMath::Fmod(CursorPhase, 2.f);
			CursorPos = Saw <= 1.f ? Saw : 2.f - Saw;
		}
		else if (bBotching)
		{
			// Колхоз: держим E, прогресс ползёт медленно, периодически всё идёт наперекосяк
			const float BotchDur = FMath::Max(RepairDuration * BotchDurationMultiplier, 0.1f);
			RepairProgress = FMath::Min(RepairProgress + DeltaSeconds / BotchDur, 1.f);

			if (FMath::FRand() < BotchMishapChancePerSecond * DeltaSeconds)
			{
				// Соскочило/искрануло/сорвало — часть работы насмарку, по рукам и громко
				RepairProgress = FMath::Max(RepairProgress - BotchMishapProgressLoss, 0.f);
				Repairer->TakeDamage(BotchMishapDamage, FDamageEvent(), nullptr, this);
				if (Repairer->VitalsComponent)
				{
					Repairer->VitalsComponent->AddPanic(BotchMishapPanic);
				}
				MakeNoise(1.f, Repairer, GetActorLocation());
			}

			NoiseAccum += DeltaSeconds;
			if (NoiseAccum >= 0.8f)
			{
				NoiseAccum = 0.f;
				MakeNoise(1.2f, Repairer, GetActorLocation()); // колхоз шумнее обычной починки
			}

			if (RepairProgress >= 1.f)
			{
				FinishRepair(Repairer);
			}
		}
		else if (MinigameType == ERepairMinigameType::Cursor)
		{
			// Мини-игра: курсор бегает, прогресс растёт только попаданиями (TryHitBy)
			CursorPhase += DeltaSeconds * MinigameCursorSpeed * MinigameSpeedMult * (1.f + PanicHardenScale * RepairerPanic01()) / FMath::Max(0.25f, RepairerToolQuality());
			const float Saw = FMath::Fmod(CursorPhase, 2.f);
			CursorPos = Saw <= 1.f ? Saw : 2.f - Saw;
		}
		else if (MinigameType == ERepairMinigameType::Valve)
		{
			// Вентиль: тает «кулдаун ритма» — HUD показывает, когда безопасно тыкать
			ValveCooldown = FMath::Max(ValveCooldown - DeltaSeconds, 0.f);
		}
		else if (MinigameType == ERepairMinigameType::Starter)
		{
			// Стартер: при зажатом E натяжение растёт; дотянул до упора — обратный удар
			if (bStarterPulling)
			{
				StarterTension = FMath::Min(StarterTension + DeltaSeconds / FMath::Max(StarterChargeTime, 0.1f), 1.f);
				if (StarterTension >= 1.f)
				{
					StarterKickback(Repairer);
				}
			}
		}
		else
		{
			RepairProgress = FMath::Min(RepairProgress + DeltaSeconds / FMath::Max(RepairDuration, 0.1f), 1.f);

			// Стук/сварка слышны — монстр-слухач это оценит
			NoiseAccum += DeltaSeconds;
			if (NoiseAccum >= 1.f)
			{
				NoiseAccum = 0.f;
				MakeNoise(1.f, Repairer, GetActorLocation());
			}

			if (RepairProgress >= 1.f)
			{
				FinishRepair(Repairer);
			}
		}
	}

	// Сервер: газовая утечка — открытый огонь (перекур) в облаке = взрыв
	if (HasAuthority())
	{
		ExplosionCooldown = FMath::Max(ExplosionCooldown - DeltaSeconds, 0.f);
		GasSuppressedTime = FMath::Max(GasSuppressedTime - DeltaSeconds, 0.f);
		if (IsLeakingGas())
		{
			// Облако растёт, пока не перекрыли
			GasLeakElapsed += DeltaSeconds;
			CurrentGasRadius = GasRadius * FMath::Min(1.f + GasSpreadPerSecond * GasLeakElapsed, GasSpreadMaxScale);
			for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
			{
				if (!It->VitalsComponent
					|| FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) > FMath::Square(CurrentGasRadius))
				{
					continue;
				}
				It->VitalsComponent->AddSmell(8.f * DeltaSeconds); // провонял газом
				// Пока облако сбито пеной — поджечь нельзя (огнетушитель спасает от взрыва)
				if (GasSuppressedTime <= 0.f && ExplosionCooldown <= 0.f && It->VitalsComponent->IsSmoking())
				{
					ExplodeGas(*It);
					break;
				}
			}
		}
		else
		{
			GasLeakElapsed = 0.f; // перекрыли — облако опадает
			CurrentGasRadius = GasRadius;
		}
	}

	// Все машины: красная пульсация аварийной лампы, пока сломан
	if (AlarmLight)
	{
		const bool bLightOn = bBroken;
		if (AlarmLight->IsVisible() != bLightOn)
		{
			AlarmLight->SetVisibility(bLightOn);
		}
		if (bLightOn)
		{
			const float Pulse = 0.55f + 0.45f * FMath::Sin(GetWorld()->GetTimeSeconds() * 4.f + GetUniqueID() % 7);
			AlarmLight->SetIntensity(3000.f * Pulse);
		}
	}

	// Все машины: обновляем процент на табличке и поворачиваем её к местной камере
	RefreshStatusVisual();
	if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
	{
		if (PC->PlayerCameraManager && StatusText)
		{
			const FVector ToCamera = PC->PlayerCameraManager->GetCameraLocation() - StatusText->GetComponentLocation();
			StatusText->SetWorldRotation(ToCamera.Rotation());
		}
	}
}

bool ARepairable::CanBeRepairedBy(const AAvaryoCharacter* Who) const
{
	if (!Who || !bBroken)
	{
		return false;
	}
	if (LockoutRemaining > 0.f)
	{
		return false; // короткое замыкание — щиток остывает
	}
	if (Repairer && Repairer != Who)
	{
		return false; // объект уже кто-то чинит
	}
	if (Who->VitalsComponent && Who->VitalsComponent->IsWounded())
	{
		return false; // раненый не работник
	}
	if (RequiredTool != NAME_None)
	{
		const APickupItem* Held = Who->GetHeldItem();
		if (!Held || Held->ToolTag != RequiredTool)
		{
			return false; // нужен правильный инструмент в руках
		}
	}
	return true;
}

bool ARepairable::CanBotchBy(const AAvaryoCharacter* Who) const
{
	if (!Who || !bBroken || !bAllowBotch || RequiredTool == NAME_None)
	{
		return false; // колхозят только то, что вообще требует инструмент
	}
	if (LockoutRemaining > 0.f || (Repairer && Repairer != Who))
	{
		return false;
	}
	if (Who->VitalsComponent && Who->VitalsComponent->IsWounded())
	{
		return false;
	}
	// Колхоз именно тогда, когда нужного инструмента в руках НЕТ (иначе это обычная починка)
	const APickupItem* Held = Who->GetHeldItem();
	return !Held || Held->ToolTag != RequiredTool;
}

bool ARepairable::CanContinueRepair() const
{
	const bool bInRange = FVector::DistSquared(Repairer->GetActorLocation(), GetActorLocation()) <= FMath::Square(RepairRange);
	if (bDoingPrereqHold || bPrereqAutoFilling || bDoingPrereqMinigame)
	{
		if (!bInRange)
		{
			return false;
		}
		if (Repairer->VitalsComponent && Repairer->VitalsComponent->IsWounded())
		{
			return false;
		}
		FRepairStage S;
		if (!GetCurrentStage(S))
		{
			return false;
		}
		// если этап требует предмета — он должен оставаться в руках
		const bool bNeedsItem = (S.Kind == ERepairStageKind::HoldTool
			|| S.Kind == ERepairStageKind::AutoFill
			|| (S.Kind == ERepairStageKind::Minigame && !S.ItemTag.IsNone()));
		if (bNeedsItem)
		{
			const APickupItem* Held = Repairer->GetHeldItem();
			if (!Held || Held->ToolTag != S.ItemTag)
			{
				return false;
			}
		}
		return true;
	}
	if (bBotching)
	{
		return CanBotchBy(Repairer) && bInRange;
	}
	return CanBeRepairedBy(Repairer) && bInRange;
}

bool ARepairable::BeginRepairBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || !Who)
	{
		return false;
	}
	if (FVector::DistSquared(Who->GetActorLocation(), GetActorLocation()) > FMath::Square(RepairRange))
	{
		return false;
	}

	// --- Подготовительные этапы (заварить / починить руками / залить) ДО основной починки ---
	if (!ArePrereqsDone())
	{
		FRepairStage S;
		GetCurrentStage(S);
		if (S.Kind == ERepairStageKind::InsertItem)
		{
			return false; // мгновенная вставка — отдельным нажатием E (TryInsertBy)
		}
		if (LockoutRemaining > 0.f || (Repairer && Repairer != Who))
		{
			return false;
		}
		if (Who->VitalsComponent && Who->VitalsComponent->IsWounded())
		{
			return false;
		}
		// нужен предмет в руках: инструмент (HoldTool / Minigame с ItemTag) или расходник (AutoFill)
		const bool bNeedsItem = (S.Kind == ERepairStageKind::HoldTool
			|| S.Kind == ERepairStageKind::AutoFill
			|| (S.Kind == ERepairStageKind::Minigame && !S.ItemTag.IsNone()));
		if (bNeedsItem)
		{
			const APickupItem* Held = Who->GetHeldItem();
			if (!Held || Held->ToolTag != S.ItemTag)
			{
				return false; // нужен предмет этапа в руках (сварочник / кабель / ...)
			}
		}
		Repairer = Who;
		NoiseAccum = 0.f;
		bBotching = false;
		bDoingPrereqHold = (S.Kind == ERepairStageKind::HoldHand || S.Kind == ERepairStageKind::HoldTool);
		bPrereqAutoFilling = (S.Kind == ERepairStageKind::AutoFill);
		bDoingPrereqMinigame = (S.Kind == ERepairStageKind::Minigame);
		if (bDoingPrereqMinigame)
		{
			CursorPhase = 0.f;
			CursorPos = 0.f;
			MissCount = 0;
			MinigameSpeedMult = 1.f;
			GreenCenter = FMath::FRandRange(0.1f, 0.9f);
		}
		Who->SetInteractionLocked(true);
		return true;
	}

	// Нет нужного инструмента, но можно колхозить — крудовый ремонт удержанием E (без мини-игры)
	bDoingPrereqHold = false;
	bPrereqAutoFilling = false;
	bDoingPrereqMinigame = false;
	const bool bProper = CanBeRepairedBy(Who);
	if (!bProper)
	{
		if (!CanBotchBy(Who))
		{
			return false;
		}
		Repairer = Who;
		NoiseAccum = 0.f;
		bBotching = true;
		return true; // колхоз не блокирует движение и не запускает мини-игру
	}

	Repairer = Who;
	NoiseAccum = 0.f;
	bBotching = false;

	if (IsMinigameRepair())
	{
		// Мини-игра: фиксируем ремонтника на месте, сбрасываем состояние режима
		CursorPhase = 0.f;
		CursorPos = 0.f;
		MinigameSpeedMult = 1.f;
		MissCount = 0;
		GreenCenter = FMath::FRandRange(0.1f, 0.9f);
		ValveCooldown = 0.f; // первый тык вентиля — бесплатный
		StarterTension = 0.f;
		// Стартер: E уже зажат этим самым нажатием — первый рывок пошёл
		bStarterPulling = MinigameType == ERepairMinigameType::Starter;
		Who->SetInteractionLocked(true);
	}
	return true;
}

void ARepairable::EndRepairBy(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Repairer == Who)
	{
		if (Who && (bDoingPrereqHold || bPrereqAutoFilling || bDoingPrereqMinigame || (IsMinigameRepair() && !bBotching)))
		{
			Who->SetInteractionLocked(false);
		}
		bStarterPulling = false;
		StarterTension = 0.f;
		bBotching = false;
		bDoingPrereqHold = false;
		bPrereqAutoFilling = false;
		bDoingPrereqMinigame = false;
		Repairer = nullptr; // прогресс сохраняется — можно дочинить позже (этапы тоже)
	}
}

void ARepairable::SuppressGas(float Duration)
{
	if (HasAuthority() && IsLeakingGas())
	{
		GasSuppressedTime = FMath::Max(GasSuppressedTime, Duration);
	}
}

float ARepairable::RepairerPanic01() const
{
	if (Repairer && Repairer->VitalsComponent)
	{
		return FMath::Clamp(Repairer->VitalsComponent->GetPanic() / 100.f, 0.f, 1.f);
	}
	return 0.f;
}

float ARepairable::RepairerToolQuality() const
{
	if (!Repairer)
	{
		return 1.f;
	}

	FRepairStage Stage;
	const bool bHaveStage = GetCurrentStage(Stage);

	// Этап «руками» (мини-игра без инструмента) — посторонний предмет в руках не влияет
	if (bDoingPrereqMinigame && bHaveStage && Stage.ItemTag.IsNone())
	{
		return 1.f;
	}

	// База: качество держимого инструмента (дешёвый комплект / per-instance)
	float Q = 1.f;
	if (const APickupItem* Held = Repairer->GetHeldItem())
	{
		Q = FMath::Clamp(Held->ToolQualityScale, 0.25f, 3.f);
	}

	// Апгрейд магазина для инструмента текущего этапа (по тегу этапа / типу мини-игры)
	FName ToolCat = NAME_None;
	if (bDoingPrereqMinigame && bHaveStage && !Stage.ItemTag.IsNone())
	{
		ToolCat = Stage.ItemTag; // напр. 'Welder'
	}
	else if (MinigameType == ERepairMinigameType::Cursor)
	{
		ToolCat = FName(TEXT("Tester")); // щиток настраивается тестером
	}
	if (!ToolCat.IsNone())
	{
		// Уровень апгрейда берём из ARunState (реплицируется) — чтобы зона совпадала у клиента,
		// а не из host-only леджера (у клиента он пустой → рассинхрон зоны → удар током).
		if (const ARunState* Run = ARunState::Get(GetWorld()))
		{
			Q *= 1.f + 0.2f * FMath::Max(0, Run->GetEquipmentLevelRep(ToolCat) - 1);
		}
	}

	return FMath::Clamp(Q, 0.25f, 3.f);
}

float ARepairable::GetEffectiveGreenHalf() const
{
	return FMath::Max(MinigameGreenHalfWidth * (1.f - 0.5f * PanicHardenScale * RepairerPanic01()) * RepairerToolQuality(), 0.02f);
}

void ARepairable::GetEffectiveStarterWindow(float& OutStart, float& OutEnd) const
{
	const float Center = (StarterWindowStart + StarterWindowEnd) * 0.5f;
	const float Half = (StarterWindowEnd - StarterWindowStart) * 0.5f
		* (1.f - 0.5f * PanicHardenScale * RepairerPanic01()) * RepairerToolQuality();
	OutStart = Center - Half;
	OutEnd = Center + Half;
}

void ARepairable::TryHitBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || Repairer != Who || !Who)
	{
		return;
	}

	// Тактильный «тык» мини-игры — у всех. Стартер: тык в начале не к месту (звук натяга — луп).
	// Вентиль: свой звук трещотки/срыва играем в HandleValveTurn (по исходу), а не общий «тык».
	if (MinigameHitSound
		&& MinigameType != ERepairMinigameType::Starter
		&& MinigameType != ERepairMinigameType::Valve)
	{
		MulticastSound(MinigameHitSound, GetActorLocation(), 0.5f);
	}

	// Prereq-мини-игра (заварка / починка руками): курсор в зелёной зоне = прогресс этапа, промах = откат
	if (bDoingPrereqMinigame)
	{
		const float EffGreenHalf = GetEffectiveGreenHalf();
		if (FMath::Abs(CursorPos - GreenCenter) <= EffGreenHalf)
		{
			PrereqProgress = FMath::Min(PrereqProgress + 1.f / FMath::Max(HitsToRepair, 1), 1.f);
			GreenCenter = FMath::FRandRange(0.1f, 0.9f);
			MakeNoise(0.5f, Who, GetActorLocation());
			if (PrereqProgress >= 1.f)
			{
				PrereqIndex++;
				PrereqProgress = 0.f;
				bDoingPrereqMinigame = false;
				if (Repairer)
				{
					Repairer->SetInteractionLocked(false);
				}
				Repairer = nullptr; // этап пройден — игрок жмёт E для следующего
				RefreshStatusVisual();
			}
		}
		else
		{
			// Промах — «поломка»: часть прогресса этапа сгорает
			PrereqProgress = FMath::Max(PrereqProgress - ValveSlipPenalty, 0.f);
			MissCount++;
			MakeNoise(0.8f, Who, GetActorLocation());
		}
		return;
	}

	switch (MinigameType)
	{
	case ERepairMinigameType::Valve:
		HandleValveTurn(Who);
		return;

	case ERepairMinigameType::Starter:
		// Новое нажатие E — начали тянуть шнур заново
		if (!bStarterPulling)
		{
			bStarterPulling = true;
			StarterTension = 0.f;
		}
		return;

	case ERepairMinigameType::Cursor:
		break; // ниже

	default:
		return;
	}

	const float EffGreenHalf = GetEffectiveGreenHalf();
	if (FMath::Abs(CursorPos - GreenCenter) <= EffGreenHalf)
	{
		// Попадание: ещё один контакт прозвонен
		RepairProgress = FMath::Min(RepairProgress + 1.f / FMath::Max(HitsToRepair, 1), 1.f);
		MakeNoise(0.5f, Who, GetActorLocation());

		if (RepairProgress >= 1.f)
		{
			FinishRepair(Who);
			return;
		}
	}
	else
	{
		// Промах: бьёт током; серия промахов — короткое замыкание
		++MissCount;
		Who->TakeDamage(ShockDamage, FDamageEvent(), nullptr, this);
		if (Who->VitalsComponent)
		{
			Who->VitalsComponent->AddPanic(8.f);
		}
		MakeNoise(0.8f, Who, GetActorLocation());

		if (MissCount >= MissesBeforeLockout)
		{
			ShortCircuit(Who);
			return;
		}
	}

	// Идём дальше: зелёная зона хаотично переезжает, курсор ускоряется
	GreenCenter = FMath::FRandRange(0.1f, 0.9f);
	MinigameSpeedMult = FMath::Min(MinigameSpeedMult + 0.1f, 1.6f);
}

void ARepairable::TryReleaseBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || Repairer != Who || !Who
		|| MinigameType != ERepairMinigameType::Starter || !bStarterPulling)
	{
		return;
	}

	bStarterPulling = false;
	const float Tension = StarterTension;
	StarterTension = 0.f;

	if (Tension < StarterGraceTension)
	{
		return; // едва взялся и отпустил — просто перехват, без наказания
	}

	// Паника сужает окно рывка к центру (трясущиеся руки), хороший инструмент — расширяет
	float WinStart, WinEnd;
	GetEffectiveStarterWindow(WinStart, WinEnd);
	if (Tension >= WinStart && Tension <= WinEnd)
	{
		// Рывок удался: движок чихнул и провернулся
		RepairProgress = FMath::Min(RepairProgress + 1.f / FMath::Max(StarterPullsToFix, 1), 1.f);
		MakeNoise(0.6f, Who, GetActorLocation());
		if (RepairProgress >= 1.f)
		{
			FinishRepair(Who);
		}
	}
	else
	{
		StarterKickback(Who); // отпустил рано — шнур хлестнул обратно
	}
}

void ARepairable::HandleValveTurn(AAvaryoCharacter* Who)
{
	if (ValveCooldown > 0.f)
	{
		// Засуетился — резьба сорвалась, вентиль провернулся назад с громким шипением
		RepairProgress = FMath::Max(RepairProgress - ValveSlipPenalty, 0.f);
		MakeNoise(1.f, Who, GetActorLocation());
		if (ValveSlipSound) { MulticastSound(ValveSlipSound, GetActorLocation(), 1.f); } // срыв резьбы — у всех
	}
	else
	{
		RepairProgress = FMath::Min(RepairProgress + ValveTurnAmount, 1.f);
		MakeNoise(0.4f, Who, GetActorLocation());
		if (ValveTurnSound) { MulticastSound(ValveTurnSound, GetActorLocation(), 0.8f); } // трещотка — у всех
		if (RepairProgress >= 1.f)
		{
			FinishRepair(Who);
			return;
		}
	}
	ValveCooldown = ValveMinInterval / FMath::Max(0.25f, RepairerToolQuality()); // хороший ключ — мягче ритм
}

void ARepairable::StarterKickback(AAvaryoCharacter* Who)
{
	bStarterPulling = false;
	StarterTension = 0.f;

	Who->TakeDamage(StarterKickDamage, FDamageEvent(), nullptr, this);
	if (Who->VitalsComponent)
	{
		Who->VitalsComponent->AddPanic(StarterKickPanic);
	}
	MakeNoise(0.8f, Who, GetActorLocation());
}

void ARepairable::FinishRepair(AAvaryoCharacter* Who)
{
	const bool bWasBotch = bBotching;

	if (IsMinigameRepair() && !bWasBotch && Who)
	{
		Who->SetInteractionLocked(false);
	}
	bStarterPulling = false;
	StarterTension = 0.f;
	bBotching = false;
	Repairer = nullptr;
	bBroken = false;
	RefreshStatusVisual(); // на листен-сервере OnRep не придёт

	if (bWasBotch)
	{
		// Кустарно, но «работает». Громко, и в акт отдельной строкой со штрафом.
		MakeNoise(1.5f, Who, GetActorLocation());
		if (ARunState* Run = ARunState::Get(GetWorld()))
		{
			Run->AddBotchedRepair(Who);
		}
	}

	// Генератор (стартер) → «завёлся» (двигатель); остальные → общий «починили». У всех.
	USoundBase* DoneSnd = (MinigameType == ERepairMinigameType::Starter && EngineStartSound)
		? EngineStartSound : RepairDoneSound;
	if (DoneSnd)
	{
		MulticastSound(DoneSnd, GetActorLocation(), 1.f);
	}

	OnRepairFinished.Broadcast(this, Who);
}

void ARepairable::ShortCircuit(AAvaryoCharacter* Culprit)
{
	// Дуга бьёт всех рядом — стоять у щитка во время ремонта плохая идея
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= FMath::Square(350.f))
		{
			It->TakeDamage(ShockAoEDamage, FDamageEvent(), nullptr, this);
			if (It->VitalsComponent)
			{
				It->VitalsComponent->AddPanic(15.f);
			}
		}
	}
	MakeNoise(1.5f, Culprit, GetActorLocation());
	if (SparkFX) // мелкие искры дуги — у всех
	{
		MulticastSparkFX(GetActorLocation() + FVector(0, 0, 60.f));
	}
	if (ShortCircuitSound) // электро-разряд — у всех
	{
		MulticastSound(ShortCircuitSound, GetActorLocation() + FVector(0, 0, 60.f), 1.f);
	}

	LockoutRemaining = LockoutDuration;
	EndRepairBy(Culprit); // выкидывает из мини-игры и снимает блокировку ввода
	RefreshStatusVisual();

	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->NotifyShortCircuit(Culprit); // диспетчер прокомментирует
	}
}

void ARepairable::SetBroken(bool bNewBroken)
{
	if (!HasAuthority() || bBroken == bNewBroken)
	{
		return;
	}
	bBroken = bNewBroken;
	RepairProgress = 0.f;
	Repairer = nullptr;
	bBotching = false;
	bDoingPrereqHold = false;
	bPrereqAutoFilling = false;
	bDoingPrereqMinigame = false;
	if (bNewBroken)
	{
		PrereqIndex = 0;       // сломали заново — этапы с нуля
		PrereqProgress = 0.f;
	}
	RefreshStatusVisual();
}

void ARepairable::ExplodeGas(AAvaryoCharacter* Culprit)
{
	ExplosionCooldown = 10.f;
	RepairProgress = 0.f; // взрыв сжёг всю проделанную работу
	GasLeakElapsed = 0.f; // облако вспыхнуло — копится заново

	// Разросшееся облако = больше радиус взрыва
	const float BlastRadius = CurrentGasRadius > 0.f ? CurrentGasRadius : GasRadius;
	// Урон по радиусу (через TakeDamage дойдёт до шкал) + скачок паники у всех рядом
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), BlastRadius,
		nullptr, {}, this, Culprit ? Culprit->GetController() : nullptr, true);
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (It->VitalsComponent
			&& FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= FMath::Square(BlastRadius * 1.5f))
		{
			It->VitalsComponent->AddPanic(30.f);
			It->FumbleHeavy(); // взрывом вышибает сварочник из рук
		}
	}

	// Громче этого ночью не бывает
	MakeNoise(2.f, Culprit, GetActorLocation());
	MulticastExplosionShake();

	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->NotifyGasExplosion(Culprit); // диспетчер уже в курсе
	}
}

void ARepairable::MulticastExplosionShake_Implementation()
{
	UGameplayStatics::PlayWorldCameraShake(this, UExplosionCameraShake::StaticClass(),
		GetActorLocation(), GasRadius * 0.5f, GasRadius * 2.5f);

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	if (ExplosionRumbleSound) // низкий рокот-хвост поверх бабаха
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionRumbleSound, GetActorLocation(), 0.9f);
	}
	if (ExplosionFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation());
	}
}

void ARepairable::MulticastSound_Implementation(USoundBase* Sound, FVector Loc, float Vol)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, Loc, Vol);
	}
}

void ARepairable::MulticastSparkFX_Implementation(FVector Loc)
{
	if (SparkFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SparkFX, Loc, GetActorRotation(), FVector(SparkScale));
	}
}

void ARepairable::OnRep_Broken()
{
	RefreshStatusVisual();
}

void ARepairable::RefreshStatusVisual()
{
	// Газовое облако: висит на трубе пока сломана+травит, гаснет при починке (на всех машинах).
	const bool bLeakingNow = bBroken && bLeaksGasWhenBroken;
	if (bLeakingNow && GasLeakFX && !GasFXComp)
	{
		GasFXComp = UNiagaraFunctionLibrary::SpawnSystemAttached(GasLeakFX, MeshComponent, NAME_None,
			GasFXOffset, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
		if (GasFXComp) { GasFXComp->SetRelativeScale3D(FVector(GasFXScale)); }
	}
	else if (!bLeakingNow && GasFXComp)
	{
		GasFXComp->Deactivate();
		GasFXComp->DestroyComponent();
		GasFXComp = nullptr;
	}
	// Шипение газа — синхронно с облаком
	if (GasHissComp)
	{
		if (bLeakingNow && !GasHissComp->IsPlaying()) { GasHissComp->Play(); }
		else if (!bLeakingNow && GasHissComp->IsPlaying()) { GasHissComp->Stop(); }
	}

	if (!StatusText)
	{
		return;
	}

	const int32 Percent = FMath::RoundToInt(RepairProgress * 100.f);
	// Код состояния для защиты от перерисовки: 101 — починено, 1000+N — блокировка N секунд
	int32 ShownPercent = bBroken ? Percent : 101;
	if (bBroken && LockoutRemaining > 0.f)
	{
		ShownPercent = 1000 + FMath::CeilToInt(LockoutRemaining);
	}
	else if (bBroken && !ArePrereqsDone())
	{
		ShownPercent = 2000 + PrereqIndex; // показываем текущий этап
	}
	if (ShownPercent == LastShownPercent)
	{
		return; // текст не менялся
	}
	LastShownPercent = ShownPercent;

	if (!bBroken)
	{
		StatusText->SetText(FText::Format(NSLOCTEXT("Repair", "StatusOk", "{0} — ОК"), DisplayName));
		StatusText->SetTextRenderColor(FColor(80, 220, 80));
	}
	else if (LockoutRemaining > 0.f)
	{
		StatusText->SetText(FText::Format(NSLOCTEXT("Repair", "StatusLockout", "{0} — ЗАМКНУЛО ({1} с)"),
			DisplayName, FMath::CeilToInt(LockoutRemaining)));
		StatusText->SetTextRenderColor(FColor(255, 60, 0)); // тревожный, не как обычная поломка
	}
	else if (!ArePrereqsDone())
	{
		FRepairStage S;
		GetCurrentStage(S);
		const FText Step = S.Label.IsEmpty() ? NSLOCTEXT("Repair", "StepGeneric", "подготовка") : S.Label;
		StatusText->SetText(FText::Format(NSLOCTEXT("Repair", "StatusStage", "{0}: {1} ({2}/{3})"),
			DisplayName, Step, FText::AsNumber(PrereqIndex + 1), FText::AsNumber(PrereqStages.Num())));
		StatusText->SetTextRenderColor(FColor(255, 200, 0)); // жёлтый — этап подготовки
	}
	else if (Percent > 0)
	{
		StatusText->SetText(FText::Format(NSLOCTEXT("Repair", "StatusProgress", "{0} — СЛОМАНО ({1}%)"), DisplayName, Percent));
		StatusText->SetTextRenderColor(FColor(255, 140, 0)); // оранжевый акцент проекта
	}
	else
	{
		StatusText->SetText(FText::Format(NSLOCTEXT("Repair", "StatusBroken", "{0} — СЛОМАНО"), DisplayName));
		StatusText->SetTextRenderColor(FColor(230, 60, 60));
	}
}
