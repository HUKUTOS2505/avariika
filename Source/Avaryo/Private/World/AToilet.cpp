#include "World/AToilet.h"

#include "AvaryoCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "Game/ARunState.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

AToilet::AToilet()
{
	PrimaryActorTick.bCanEverTick = true; // курсор мини-игры + поворот таблички
	bReplicates = true;
	SetNetUpdateFrequency(30.f); // курсор должен идти плавно у клиентов

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // трейс из камеры

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(MeshComponent);
	Label->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetWorldSize(26.f);
	Label->SetText(NSLOCTEXT("Toilet", "Label", "Биотуалет"));
	Label->SetTextRenderColor(FColor(120, 200, 255));

	CursorSpeed = 0.8f;
	GreenHalfWidth = 0.07f;
	YellowHalfWidth = 0.09f;
	GreenDrain = 30.f;
	YellowDrain = 12.f;
	MissDrain = 2.f;
	PassiveDrainPerSecond = 2.f;

	CursorPos = 0.f;
	GreenCenter = 0.2f;
	YellowCenter = 0.7f;
	CursorPhase = 0.f;
	SpeedMultiplier = 1.f;
}

void AToilet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AToilet, Occupant);
	DOREPLIFETIME(AToilet, CursorPos);
	DOREPLIFETIME(AToilet, GreenCenter);
	DOREPLIFETIME(AToilet, YellowCenter);
}

void AToilet::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Сервер: курсор бегает, шкала медленно уходит сама.
	// Двигаться сидя нельзя (ввод заблокирован) — срыв только ранением или G
	if (HasAuthority() && Occupant)
	{
		UVitalsComponent* Vitals = Occupant->VitalsComponent;
		if (!Vitals || Vitals->IsWounded())
		{
			EndUseBy(Occupant); // процесс сорван
		}
		else
		{
			// Пинг-понг курсора (треугольная волна из фазы)
			CursorPhase += DeltaSeconds * CursorSpeed * SpeedMultiplier;
			const float Saw = FMath::Fmod(CursorPhase, 2.f);
			CursorPos = Saw <= 1.f ? Saw : 2.f - Saw;

			Vitals->DrainBladder(PassiveDrainPerSecond * DeltaSeconds);
			if (Vitals->GetBladder() <= 0.5f)
			{
				FinishSession(Occupant);
			}
		}
	}

	// Табличка смотрит на местную камеру
	if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
	{
		if (PC->PlayerCameraManager && Label)
		{
			const FVector ToCamera = PC->PlayerCameraManager->GetCameraLocation() - Label->GetComponentLocation();
			Label->SetWorldRotation(ToCamera.Rotation());
		}
	}
}

bool AToilet::CanUseBy(const AAvaryoCharacter* Who) const
{
	if (Occupant && Occupant != Who)
	{
		return false; // занято
	}
	return Who && Who->VitalsComponent
		&& !Who->VitalsComponent->IsWounded()
		&& Who->VitalsComponent->GetBladder() > 5.f;
}

bool AToilet::BeginUseBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || !CanUseBy(Who) || Occupant)
	{
		return false;
	}
	if (FVector::DistSquared(Who->GetActorLocation(), GetActorLocation()) > FMath::Square(350.f))
	{
		return false;
	}
	Occupant = Who;
	CursorPhase = 0.f;
	CursorPos = 0.f;
	SpeedMultiplier = 1.f;
	GreenCenter = 0.2f;  // стартовые позиции зон — как договорились
	YellowCenter = 0.7f;

	// Сесть: телепорт на куб, разворот на 180°, движение и камера блокируются
	Who->SetInteractionLocked(true);
	const float SeatZ = GetActorScale3D().Z * 50.f + 92.f; // верх куба + полувысота капсулы
	Who->SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, SeatZ), false, nullptr, ETeleportType::TeleportPhysics);
	const float SeatYaw = Who->GetActorRotation().Yaw + 180.f;
	Who->SetActorRotation(FRotator(0.f, SeatYaw, 0.f));
	Who->ClientSetControlYaw(SeatYaw);
	return true;
}

void AToilet::TryHitBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || Occupant != Who || !Who->VitalsComponent)
	{
		return;
	}

	if (FMath::Abs(CursorPos - GreenCenter) <= GreenHalfWidth)
	{
		// Зелёная: дело спорится, почти бесшумно
		Who->VitalsComponent->DrainBladder(GreenDrain);
		MakeNoise(0.2f, Who, GetActorLocation());
	}
	else if (FMath::Abs(CursorPos - YellowCenter) <= YellowHalfWidth)
	{
		Who->VitalsComponent->DrainBladder(YellowDrain);
		MakeNoise(0.4f, Who, GetActorLocation());
	}
	else
	{
		// Мимо: толку чуть, а конфуз слышен на всю карту
		Who->VitalsComponent->DrainBladder(MissDrain);
		MakeNoise(0.9f, Who, GetActorLocation());
	}

	if (Who->VitalsComponent->GetBladder() <= 0.5f)
	{
		FinishSession(Who);
		return;
	}

	// Идём дальше: зоны переезжают, курсор ускоряется (до x1.7)
	RerollZones();
	SpeedMultiplier = FMath::Min(SpeedMultiplier + 0.12f, 1.7f);
}

void AToilet::EndUseBy(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Occupant == Who)
	{
		Occupant = nullptr; // недоделанное остаётся в шкале — приходи ещё
		if (Who)
		{
			Who->SetInteractionLocked(false);
		}
	}
}

void AToilet::FinishSession(AAvaryoCharacter* Who)
{
	Occupant = nullptr;
	Who->SetInteractionLocked(false);
	Who->VitalsComponent->RelieveBladder();
	Who->VitalsComponent->ReducePanic(12.f); // облегчение: короткий «ааа», сброс паники
	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->AddToiletVisit(Who); // дисциплина — в «Акт»
	}
	MakeNoise(0.6f, Who, GetActorLocation()); // финальный аккорд
}

void AToilet::RerollZones()
{
	GreenCenter = FMath::FRandRange(0.1f, 0.9f);
	// Жёлтая — отдельно, не налезая на зелёную
	const float MinGap = GreenHalfWidth + YellowHalfWidth + 0.04f;
	for (int32 Attempt = 0; Attempt < 16; ++Attempt)
	{
		YellowCenter = FMath::FRandRange(0.1f, 0.9f);
		if (FMath::Abs(YellowCenter - GreenCenter) > MinGap)
		{
			break;
		}
	}
}
