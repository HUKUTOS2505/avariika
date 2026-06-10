#include "World/AToilet.h"

#include "AvaryoCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AToilet::AToilet()
{
	PrimaryActorTick.bCanEverTick = true; // только поворот таблички к камере
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
}

void AToilet::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
	return Who && Who->VitalsComponent
		&& !Who->VitalsComponent->IsWounded()
		&& Who->VitalsComponent->GetBladder() > 5.f;
}

void AToilet::UseBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || !CanUseBy(Who))
	{
		return;
	}

	Who->VitalsComponent->RelieveBladder();

	// Слышно. Конечно слышно.
	MakeNoise(0.7f, Who, GetActorLocation());
}
