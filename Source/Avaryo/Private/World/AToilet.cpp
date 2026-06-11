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
	YellowHalfWidth = 0.18f;
	GreenDrain = 30.f;
	YellowDrain = 12.f;
	MissDrain = 2.f;
	PassiveDrainPerSecond = 2.f;

	CursorPos = 0.f;
	GreenCenter = 0.5f;
	CursorPhase = 0.f;
	SpeedMultiplier = 1.f;
}

void AToilet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AToilet, Occupant);
	DOREPLIFETIME(AToilet, CursorPos);
	DOREPLIFETIME(AToilet, GreenCenter);
}

void AToilet::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Сервер: курсор бегает, шкала медленно уходит сама
	if (HasAuthority() && Occupant)
	{
		UVitalsComponent* Vitals = Occupant->VitalsComponent;
		const bool bMoved = Occupant->GetVelocity().SizeSquared2D() > 2500.f; // > 50 см/с — встал
		const bool bNear = FVector::DistSquared(Occupant->GetActorLocation(), GetActorLocation()) <= FMath::Square(350.f);
		const bool bWounded = Vitals && Vitals->IsWounded();
		if (!Vitals || bMoved || !bNear || bWounded)
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
	RerollGreenZone();
	return true;
}

void AToilet::TryHitBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || Occupant != Who || !Who->VitalsComponent)
	{
		return;
	}

	const float Dist = FMath::Abs(CursorPos - GreenCenter);
	if (Dist <= GreenHalfWidth)
	{
		// Зелёная: дело спорится, почти бесшумно
		Who->VitalsComponent->DrainBladder(GreenDrain);
		MakeNoise(0.2f, Who, GetActorLocation());
	}
	else if (Dist <= YellowHalfWidth)
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

	// Идём дальше: зона переезжает, курсор ускоряется (до x1.7)
	RerollGreenZone();
	SpeedMultiplier = FMath::Min(SpeedMultiplier + 0.12f, 1.7f);
}

void AToilet::EndUseBy(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Occupant == Who)
	{
		Occupant = nullptr; // недоделанное остаётся в шкале — приходи ещё
	}
}

void AToilet::FinishSession(AAvaryoCharacter* Who)
{
	Occupant = nullptr;
	Who->VitalsComponent->RelieveBladder();
	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->AddToiletVisit(Who); // дисциплина — в «Акт»
	}
	MakeNoise(0.6f, Who, GetActorLocation()); // финальный аккорд
}

void AToilet::RerollGreenZone()
{
	GreenCenter = FMath::FRandRange(0.15f, 0.85f);
}
