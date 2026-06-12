#include "World/ARepairable.h"

#include "AvaryoCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "GameFramework/PlayerController.h"
#include "Items/APickupItem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/AvaryoCameraShakes.h"

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
	LastShownPercent = -1;

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
}

void ARepairable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
			CursorPhase += DeltaSeconds * MinigameCursorSpeed * MinigameSpeedMult * (1.f + PanicHardenScale * RepairerPanic01());
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
		if (IsLeakingGas())
		{
			for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
			{
				if (!It->VitalsComponent
					|| FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) > FMath::Square(GasRadius))
				{
					continue;
				}
				It->VitalsComponent->AddSmell(8.f * DeltaSeconds); // провонял газом
				if (ExplosionCooldown <= 0.f && It->VitalsComponent->IsSmoking())
				{
					ExplodeGas(*It);
					break;
				}
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

	// Нет нужного инструмента, но можно колхозить — крудовый ремонт удержанием E (без мини-игры)
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
		if (IsMinigameRepair() && !bBotching && Who)
		{
			Who->SetInteractionLocked(false);
		}
		bStarterPulling = false;
		StarterTension = 0.f;
		bBotching = false;
		Repairer = nullptr; // прогресс сохраняется — можно дочинить позже
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

void ARepairable::TryHitBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || Repairer != Who || !Who)
	{
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

	const float EffGreenHalf = FMath::Max(MinigameGreenHalfWidth * (1.f - 0.5f * PanicHardenScale * RepairerPanic01()), 0.02f);
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

	// Паника сужает окно рывка к центру (трясущиеся руки)
	const float WinCenter = (StarterWindowStart + StarterWindowEnd) * 0.5f;
	const float WinHalf = (StarterWindowEnd - StarterWindowStart) * 0.5f * (1.f - 0.5f * PanicHardenScale * RepairerPanic01());
	if (Tension >= WinCenter - WinHalf && Tension <= WinCenter + WinHalf)
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
	}
	else
	{
		RepairProgress = FMath::Min(RepairProgress + ValveTurnAmount, 1.f);
		MakeNoise(0.4f, Who, GetActorLocation());
		if (RepairProgress >= 1.f)
		{
			FinishRepair(Who);
			return;
		}
	}
	ValveCooldown = ValveMinInterval; // ритм отсчитывается от любого тыка, даже сорванного
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
	RefreshStatusVisual();
}

void ARepairable::ExplodeGas(AAvaryoCharacter* Culprit)
{
	ExplosionCooldown = 10.f;
	RepairProgress = 0.f; // взрыв сжёг всю проделанную работу

	// Урон по радиусу (через TakeDamage дойдёт до шкал) + скачок паники у всех рядом
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), GasRadius,
		nullptr, {}, this, Culprit ? Culprit->GetController() : nullptr, true);
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (It->VitalsComponent
			&& FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= FMath::Square(GasRadius * 1.5f))
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
}

void ARepairable::OnRep_Broken()
{
	RefreshStatusVisual();
}

void ARepairable::RefreshStatusVisual()
{
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
