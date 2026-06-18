#include "World/ARepairable.h"

#include "AvaryoCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/GameInstance.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Items/APickupItem.h"
#include "World/APowerSwitch.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "UI/AvaryoCameraShakes.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"

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
	// КИРИЛЛИЦА: дефолтный шрифт TextRender (RobotoDistanceField, offline) без кириллицы → прямоугольники.
	// Берём РАНТАЙМ-шрифт Roboto (рендерит любые глифы, как HUD).
	static ConstructorHelpers::FObjectFinder<UFont> CyrFont(TEXT("/Engine/EngineFonts/Roboto.Roboto"));
	if (CyrFont.Succeeded()) { StatusText->SetFont(CyrFont.Object); }

	// Аварийная лампа: в ночной темноте сломанный объект видно по красной пульсации
	AlarmLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("AlarmLight"));
	AlarmLight->SetupAttachment(MeshComponent);
	AlarmLight->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	AlarmLight->SetUsingAbsoluteScale(true); // масштаб меша не должен раздувать радиус света
	AlarmLight->SetLightColor(FColor(255, 40, 20));
	AlarmLight->SetIntensity(3000.f);
	AlarmLight->SetAttenuationRadius(700.f);
	AlarmLight->SetCastShadows(false); // дёшево: лампочек несколько, тени не нужны

	// Лужа разлива — ДЕКАЛЬ: проецируется вниз на пол (повторяет пол, без клиппинга/парения).
	FloodDecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("FloodDecal"));
	FloodDecalComp->SetupAttachment(MeshComponent);
	FloodDecalComp->SetUsingAbsoluteRotation(true); // проекция строго вниз, не зависит от поворота трубы
	FloodDecalComp->SetUsingAbsoluteScale(true);    // масштаб трубы не раздувает лужу
	FloodDecalComp->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // смотрит вниз → проецирует на пол
	FloodDecalComp->SetHiddenInGame(true);          // видна только пока затоплено (Tick, все клиенты)

	DisplayName = FText::FromString(TEXT("Объект"));
	RepairDuration = 8.f;
	RequiredTool = NAME_None;
	RepairRange = 350.f;
	bLeaksGasWhenBroken = false;
	GasRadius = 450.f; // зона запаха/взрыва (газ НЕ наносит HP-урон — бытовой газ не токсичен)
	ExplosionDamage = 45.f;
	bBroken = true;
	RepairProgress = 0.f;
	NoiseAccum = 0.f;
	ExplosionCooldown = 0.f;
	GasSuppressedTime = 0.f;
	GasLeakElapsed = 0.f;
	GasCheckAccum = 0.f;
	GasSpreadPerSecond = 0.05f; // +5%/с — за ~20 с до максимума
	GasSpreadMaxScale = 2.0f;
	GasDisperseRate = 3.0f; // под пеной облако рассеивается ~втрое быстрее, чем копилось
	CurrentGasRadius = GasRadius;
	LastShownPercent = -1;

	// Вода/потоп (каскад 2.1) — по умолчанию выключено, включается на трубе-источнике.
	bFloodsWhenBroken = false;
	FloodRadius = 500.f;
	FloodSpreadPerSecond = 0.06f; // +6%/с
	FloodSpreadMaxScale = 2.2f;
	bFloodElectrified = true;      // вода добралась до проводки — зона под током, пока не обесточат
	FloodShockDamage = 18.f;
	FloodShockInterval = 3.0f; // раз в 3 секунды (по фидбеку — ритмичный удар, не каждую секунду)
	FloodElapsed = 0.f;
	FloodCheckAccum = 0.f;
	FloodShockCooldown = 0.f;
	CurrentFloodRadius = FloodRadius;

	// Электрика / «живой провод» (план: выключить рубильник → починить проводку)
	bLiveWireWhenBroken = false;
	LiveWireShockDamage = 18.f;
	LiveWireShockInterval = 3.0f; // раз в 3 секунды (единый ритм удара током)
	LiveWirePanic = 22.f;
	LiveWireShockCooldown = 0.f;
	bElectricallyPowered = true; // по умолчанию запитано; рубильник снимает

	// Звуки по умолчанию (можно переопределить в Blueprint)
	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSnd(TEXT("/Game/Audio/SFX/Hazard/Hazard_ExplosionGas_1.Hazard_ExplosionGas_1"));
	if (ExplosionSnd.Succeeded()) { ExplosionSound = ExplosionSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> RepairSnd(TEXT("/Game/Audio/SFX/RepairDone.RepairDone"));
	if (RepairSnd.Succeeded()) { RepairDoneSound = RepairSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> HitSnd(TEXT("/Game/Audio/SFX/Cues/SC_MinigameHit.SC_MinigameHit")); // случайный из 5
	if (HitSnd.Succeeded()) { MinigameHitSound = HitSnd.Object; }

	// VFX взрыва: новый пак NiagaraExplosion01 (наземный взрыв со вспышками-молниями), с фолбэком
	// на движковый пример (пак тяжёлый/локальный, на свежем клоне его нет).
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExpFXNew(TEXT("/Game/NiagaraExplosion01/Niagaras/Ground/N_ExplosionGround_001.N_ExplosionGround_001"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExpFXOld(TEXT("/Game/NiagaraExamples/FX_Explosions/NS_Explosion.NS_Explosion"));
	if (ExpFXNew.Succeeded()) { ExplosionFX = ExpFXNew.Object; }
	else if (ExpFXOld.Succeeded()) { ExplosionFX = ExpFXOld.Object; }
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SpkFX(TEXT("/Game/NiagaraExamples/FX_Sparks/NS_Spark_Burst.NS_Spark_Burst"));
	if (SpkFX.Succeeded()) { SparkFX = SpkFX.Object; }
	// Утечка газа = ТОКСИЧНАЯ туча (кислотный дым). Фолбэк на лёгкий дымок — пак локальный (gitignore),
	// на свежем клоне его нет (как у взрыва выше).
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> GasFXNew(TEXT("/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_7_acid.NS_Smoke_7_acid"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> GasFXOld(TEXT("/Game/NiagaraExamples/Utilities/SpriteGeneration/SmokePuffLight/NS_SmokePuffLight.NS_SmokePuffLight"));
	if (GasFXNew.Succeeded()) { GasLeakFX = GasFXNew.Object; }
	else if (GasFXOld.Succeeded()) { GasLeakFX = GasFXOld.Object; }
	// Струя воды из прорванной трубы (напорная). Фолбэк — галерейный всплеск; оба пака локальные.
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> WaterFXNew(TEXT("/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_WaterHose_SingleProjection.NS_WaterHose_SingleProjection"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> WaterFXOld(TEXT("/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Water/NS_Water_1.NS_Water_1"));
	if (WaterFXNew.Succeeded()) { WaterSprayFX = WaterFXNew.Object; }
	else if (WaterFXOld.Succeeded()) { WaterSprayFX = WaterFXOld.Object; }
	// Материал лужи (декаль-домен): водяная лужа на полу. Фолбэк — асфальтовая лужа (точно декаль). Паки локальные.
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PuddleMat(TEXT("/Game/IndustrialFactory/Decals/Puddle_01/m_Puddle_01_01.m_Puddle_01_01"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PuddleMatAlt(TEXT("/Game/ResidentialHouses/Materials/Decals/Puddles/MI_AsphaltPuddle01.MI_AsphaltPuddle01"));
	if (PuddleMat.Succeeded()) { FloodDecalMaterial = PuddleMat.Object; }
	else if (PuddleMatAlt.Succeeded()) { FloodDecalMaterial = PuddleMatAlt.Object; }
	if (FloodDecalComp)
	{
		FloodDecalComp->DecalSize = FVector(FloodDecalDepth, 256.f, 256.f);
		if (FloodDecalMaterial) { FloodDecalComp->SetDecalMaterial(FloodDecalMaterial); }
	}
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
	// Короткий «пзык» для частых ударов током (вода/живой провод). Фолбэк — длинный КЗ-зап. EditAnywhere → легко сменить.
	static ConstructorHelpers::FObjectFinder<USoundBase> ZapSnd(TEXT("/Game/Audio/Lib/electrical/Ghosthack-ME_Magic_Fire_Weld_Machine_Inventor_Multiple_Short_Electrode_Sparkling_Hits.Ghosthack-ME_Magic_Fire_Weld_Machine_Inventor_Multiple_Short_Electrode_Sparkling_Hits"));
	if (ZapSnd.Succeeded()) { ShockZapSound = ZapSnd.Object; }
	else if (ShortCircuitSound) { ShockZapSound = ShortCircuitSound; }
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
	bGeneratorShortsIfPanelLive = false; // выкл по умолчанию (ноль влияния на существующий контент)
	GeneratorPanelScanRadius = 1500.f;
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
	DOREPLIFETIME(ARepairable, bElectricallyPowered);
	DOREPLIFETIME(ARepairable, bFloodElectrified);   // зона под током — для клиентского HUD
	DOREPLIFETIME(ARepairable, CurrentGasRadius);     // рост газ-облака — для HUD/газодетектора
	DOREPLIFETIME(ARepairable, CurrentFloodRadius);   // рост разлива — для HUD
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
			// Пена огнетушителя не только не даёт поджечь — она РАЗВЕИВАЕТ облако:
			// пока сбито пеной, газ не копится, а рассеивается (реальный способ «убрать газ»).
			if (GasSuppressedTime > 0.f)
			{
				GasLeakElapsed = FMath::Max(0.f, GasLeakElapsed - DeltaSeconds * GasDisperseRate);
			}
			else
			{
				GasLeakElapsed += DeltaSeconds; // иначе облако растёт, пока не перекрыли/не развеяли
			}
			CurrentGasRadius = GasRadius * FMath::Min(1.f + GasSpreadPerSecond * GasLeakElapsed, GasSpreadMaxScale);
			// Обход игроков — не каждый кадр (дорого при нескольких трубах). Запах копим по накопленному dt,
			// поджиг проверяем 5 Гц (взрыв и так на кулдауне). Курсор-мини-игра тикает полным Tick — не задета.
			GasCheckAccum += DeltaSeconds;
			if (GasCheckAccum >= 0.2f)
			{
				const float GasDt = GasCheckAccum;
				GasCheckAccum = 0.f;
				for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
				{
					if (!It->VitalsComponent
						|| FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) > FMath::Square(CurrentGasRadius))
					{
						continue;
					}
					// Дыхательная опасность: ПРОТИВОГАЗ (ToolTag GasMask) фильтрует воздух — полностью спасает.
					// Без маски в облаке: надышался (запах → кашель в VitalsComponent), удушье (паника) и токсичный урон.
					if (!It->HasGasMask())
					{
						// Бытовой газ НЕ токсичен — здоровью не вредит (опасность = ВЗРЫВ: искра/сварка/курение).
						// Без маски только провонял + дискомфорт (запах меркаптана) → лёгкая паника. Маска фильтрует → спокоен.
						It->VitalsComponent->AddSmell(8.f * GasDt);
						It->VitalsComponent->AddPanic(8.f * GasDt);
					}
					// Открытый огонь рядом поджигает облако: курение ИЛИ электро-дуга сварки. Пена (огнетушитель) спасает.
					if (GasSuppressedTime <= 0.f && ExplosionCooldown <= 0.f && (It->VitalsComponent->IsSmoking() || It->IsWelding()))
					{
						ExplodeGas(*It);
						break;
					}
				}
			}
		}
		else
		{
			GasLeakElapsed = 0.f; // перекрыли — облако опадает
			CurrentGasRadius = GasRadius;
		}

		// ----- Вода / потоп (каскад 2.1): разлив растёт; под напряжением — бьёт током -----
		FloodShockCooldown = FMath::Max(FloodShockCooldown - DeltaSeconds, 0.f);
		if (IsFlooding())
		{
			FloodElapsed += DeltaSeconds;
			CurrentFloodRadius = FloodRadius * FMath::Min(1.f + FloodSpreadPerSecond * FloodElapsed, FloodSpreadMaxScale);
			FloodCheckAccum += DeltaSeconds;
			if (FloodCheckAccum >= 0.2f)
			{
				FloodCheckAccum = 0.f;
				// Один разряд на зону за интервал (не по каждому игроку), бьёт того, кто без диэлектрика.
				// Кулдаун проверяем ВНУТРИ цикла (не снимок) — иначе в кооп бьёт всех безсапожных за один тик.
				for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
				{
					// В ВОДЕ = в радиусе по ГОРИЗОНТАЛИ (XY, разлив вокруг) И в пределах уровня воды по ВЫСОТЕ
					// (вода поднимается со временем). Стоящего ВЫШЕ (на платформе) не бьёт — раньше была 3D-сфера.
					if (!It->VitalsComponent) { continue; }
					{
						const FVector P = It->GetActorLocation(), Cn = GetActorLocation();
						const float WaterZ = Cn.Z + FloodDecalZOffset;
						const float ReachUp = FloodReachUp + FMath::Min(FloodRisePerSec * FloodElapsed, FloodRiseMax);
						const bool bInWater = (FMath::Square(P.X - Cn.X) + FMath::Square(P.Y - Cn.Y)) <= FMath::Square(CurrentFloodRadius)
							&& P.Z <= WaterZ + ReachUp && P.Z >= WaterZ - 150.f;
						if (!bInWater) { continue; }
					}
					It->VitalsComponent->MakeWet(-1.f); // стоит в воде → промок (сапоги от мокроты не спасают)
					if (bFloodElectrified && FloodShockCooldown <= 0.f && !It->HasRubberBoots())
					{
						It->TakeDamage(FloodShockDamage, FDamageEvent(), nullptr, this);
						It->VitalsComponent->AddPanic(20.f);
						FloodShockCooldown = FloodShockInterval;
						MulticastSparkFX(It->GetActorLocation()); // телеграф: искры по воде
						if (ShockZapSound) { MulticastSound(ShockZapSound, It->GetActorLocation(), 0.9f); } // короткий разряд
					}
				}
			}
		}
		else
		{
			FloodElapsed = 0.f;
			CurrentFloodRadius = FloodRadius;
		}

		// ----- Электрика: «живой провод» — пока под напряжением, бьёт током рядом стоящих -----
		// (сухой контакт: сапоги НЕ спасают — единственный способ обезопасить - срубить рубильник)
		LiveWireShockCooldown = FMath::Max(LiveWireShockCooldown - DeltaSeconds, 0.f);
		if (IsLiveWireHot() && LiveWireShockCooldown <= 0.f)
		{
			const float HotRadius = FMath::Max(RepairRange * 1.15f, 160.f);
			for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
			{
				if (!It->VitalsComponent
					|| FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) > FMath::Square(HotRadius))
				{
					continue;
				}
				It->TakeDamage(LiveWireShockDamage, FDamageEvent(), nullptr, this);
				It->VitalsComponent->AddPanic(LiveWirePanic);
				LiveWireShockCooldown = LiveWireShockInterval;
				MulticastSparkFX(GetActorLocation()); // искры на проводе
				if (ShockZapSound) { MulticastSound(ShockZapSound, GetActorLocation(), 0.9f); } // короткий разряд
				break; // один разряд на интервал
			}
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

	// Все машины: растущая лужа-ДЕКАЛЬ. Footprint = CurrentFloodRadius (реплицируется) → видимая вода ТОЧНО = зоне удара.
	if (FloodDecalComp)
	{
		const bool bFloodVis = IsFlooding();
		FloodDecalComp->SetHiddenInGame(!bFloodVis);
		if (bFloodVis)
		{
			const FVector DC = GetActorLocation();
			FloodDecalComp->SetWorldRotation(FRotator(FloodDecalPitch, 0.f, 0.f)); // проекция вниз (тюнится FloodDecalPitch)
			FloodDecalComp->SetWorldLocation(FVector(DC.X, DC.Y, DC.Z + FloodDecalZOffset)); // опустить на пол
			const float S = CurrentFloodRadius / 256.f; // base DecalSize.Y/Z=256 -> footprint = радиусу
			FloodDecalComp->SetWorldScale3D(FVector(1.f, S, S));
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
	if (IsLiveWireHot())
	{
		return false; // провод под напряжением — сначала обесточь рубильником
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
	if (IsLiveWireHot())
	{
		return false; // под напряжением — даже колхозить нельзя, пока не обесточат
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
	if (IsLiveWireHot())
	{
		return false; // если питание вернули посреди починки — провод снова под током, нельзя
	}
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
	// Генератор + рядом ещё ЗАПИТАННЫЙ щиток → короткое замыкание ВМЕСТО успеха.
	// До сброса bBroken (ниже) — генератор остаётся сломан: сперва обесточь щиток, потом заводи.
	if (bGeneratorShortsIfPanelLive && MinigameType == ERepairMinigameType::Starter
		&& !bBotching && HasLivePanelNearby())
	{
		ShortCircuit(Who);    // дуга по рядом стоящим + лок-аут + диспетчер прокомментирует
		RepairProgress = 0.f; // КЗ сжигает прогресс — стартер проходить заново (а не доделать 1 рывком)
		return;               // НЕ завершаем — bBroken остаётся true
	}

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

bool ARepairable::HasLivePanelNearby() const
{
	for (TActorIterator<APowerSwitch> It(GetWorld()); It; ++It)
	{
		if (It->IsPowerOn()
			&& FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= FMath::Square(GeneratorPanelScanRadius))
		{
			return true;
		}
	}
	return false;
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

void ARepairable::TriggerReTrip()
{
	if (!HasAuthority())
	{
		return;
	}
	SetBroken(true); // снова обесточились (OnRep_Broken оживит аварийные лампы)
	MulticastSparkFX(GetActorLocation()); // искры у всех
	if (ShortCircuitSound)
	{
		MulticastSound(ShortCircuitSound, GetActorLocation(), 1.0f); // звук замыкания
	}
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

	// Струя воды: висит на трубе пока затоплено (мирроринг газового облака).
	const bool bFloodingNow = IsFlooding();
	if (bFloodingNow && WaterSprayFX && !WaterSprayComp)
	{
		WaterSprayComp = UNiagaraFunctionLibrary::SpawnSystemAttached(WaterSprayFX, MeshComponent, NAME_None,
			WaterSprayOffset, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
		if (WaterSprayComp) { WaterSprayComp->SetRelativeScale3D(FVector(WaterSprayScale)); }
	}
	else if (!bFloodingNow && WaterSprayComp)
	{
		WaterSprayComp->Deactivate();
		WaterSprayComp->DestroyComponent();
		WaterSprayComp = nullptr;
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
	else if (bBroken && IsLiveWireHot())
	{
		ShownPercent = 3000; // под напряжением — блокирующее состояние
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
	else if (IsLiveWireHot())
	{
		StatusText->SetText(FText::Format(NSLOCTEXT("Repair", "StatusLiveWire", "{0} — ПОД НАПРЯЖЕНИЕМ!\nсними рубильник"), DisplayName));
		StatusText->SetTextRenderColor(FColor(255, 230, 0)); // электро-жёлтый
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
