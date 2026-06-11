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
	PrimaryActorTick.bCanEverTick = true; // прогресс на сервере + поворот таблички
	bReplicates = true;

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

	UseDuration = 3.f;
	UseProgress = 0.f;
}

void AToilet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AToilet, Occupant);
	DOREPLIFETIME(AToilet, UseProgress);
}

void AToilet::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Сервер: тикаем «процесс»
	if (HasAuthority() && Occupant)
	{
		// Срыв: ранен/нечего делать (CanUseBy), пошёл, отошёл
		const bool bMoved = Occupant->GetVelocity().SizeSquared2D() > 2500.f; // > 50 см/с — отошёл
		const bool bNear = FVector::DistSquared(Occupant->GetActorLocation(), GetActorLocation()) <= FMath::Square(350.f);
		if (!CanUseBy(Occupant) || bMoved || !bNear)
		{
			EndUseBy(Occupant);
		}
		else
		{
			UseProgress = FMath::Min(UseProgress + DeltaSeconds / FMath::Max(UseDuration, 0.1f), 1.f);
			if (UseProgress >= 1.f)
			{
				AAvaryoCharacter* Done = Occupant;
				Occupant = nullptr;
				UseProgress = 0.f;
				Done->VitalsComponent->RelieveBladder();
				if (ARunState* Run = ARunState::Get(GetWorld()))
				{
					Run->AddToiletVisit(Done); // дисциплина — в «Акт»
				}
				// Слышно. Конечно слышно.
				MakeNoise(0.7f, Done, GetActorLocation());
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
	UseProgress = 0.f;
	return true;
}

void AToilet::EndUseBy(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Occupant == Who)
	{
		Occupant = nullptr;
		UseProgress = 0.f; // прерванный процесс не засчитывается
	}
}
