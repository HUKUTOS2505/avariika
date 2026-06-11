#include "World/ARepairable.h"

#include "AvaryoCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Items/APickupItem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
}

void ARepairable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Сервер: тикаем починку
	if (HasAuthority() && Repairer)
	{
		if (!CanContinueRepair())
		{
			EndRepairBy(Repairer);
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
				AAvaryoCharacter* FinishedBy = Repairer;
				Repairer = nullptr;
				bBroken = false;
				RefreshStatusVisual(); // на листен-сервере OnRep не придёт
				OnRepairFinished.Broadcast(this, FinishedBy);
			}
		}
	}

	// Сервер: газовая утечка — открытый огонь (перекур) в облаке = взрыв
	if (HasAuthority())
	{
		ExplosionCooldown = FMath::Max(ExplosionCooldown - DeltaSeconds, 0.f);
		if (IsLeakingGas() && ExplosionCooldown <= 0.f)
		{
			for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
			{
				if (It->VitalsComponent && It->VitalsComponent->IsSmoking()
					&& FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= FMath::Square(GasRadius))
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

bool ARepairable::CanContinueRepair() const
{
	return CanBeRepairedBy(Repairer)
		&& FVector::DistSquared(Repairer->GetActorLocation(), GetActorLocation()) <= FMath::Square(RepairRange);
}

bool ARepairable::BeginRepairBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || !CanBeRepairedBy(Who))
	{
		return false;
	}
	if (FVector::DistSquared(Who->GetActorLocation(), GetActorLocation()) > FMath::Square(RepairRange))
	{
		return false;
	}

	Repairer = Who;
	NoiseAccum = 0.f;
	return true;
}

void ARepairable::EndRepairBy(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Repairer == Who)
	{
		Repairer = nullptr; // прогресс сохраняется — можно дочинить позже
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
		}
	}

	// Громче этого ночью не бывает
	MakeNoise(2.f, Culprit, GetActorLocation());
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
	const int32 ShownPercent = bBroken ? Percent : 101; // 101 — маркер "починено"
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
