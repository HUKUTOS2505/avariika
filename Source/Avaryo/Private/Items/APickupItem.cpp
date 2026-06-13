#include "Items/APickupItem.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicatingMovement(true); // чтобы клиенты видели падение предмета при сбросе

	// Меш — корень: при сбросе физика двигает весь актор
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Радиус подбора. Абсолютный масштаб — не сжимается вместе с мешем
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	TriggerSphere->SetupAttachment(MeshComponent);
	TriggerSphere->SetUsingAbsoluteScale(true);
	TriggerSphere->SetSphereRadius(120.f);
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Конус струи огнетушителя: меш задаётся в Blueprint, по умолчанию пуст и скрыт
	SprayCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SprayCone"));
	SprayCone->SetupAttachment(MeshComponent);
	SprayCone->SetUsingAbsoluteScale(true);
	SprayCone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SprayCone->SetVisibility(false);

	DisplayName = FText::FromString(TEXT("Предмет"));
	ItemSize = EItemSize::Light;
	ItemEffect = EItemEffect::None;
	EffectMagnitude = 0.f;
	Charges = -1;
	UseCastTime = 0.f;
	CastSpeedMultiplier = 1.f;
	HoldOffset = FVector(60.f, 30.f, -25.f);
	HoldRotation = FRotator::ZeroRotator;
	CarryOffset = FVector(45.f, -45.f, -40.f);
	RotationSpeed = 45.f;
	bSpraying = false;
	bToggledOn = false;
	ToggleNoiseAccum = 0.f;
}

void APickupItem::SetToggledOn(bool bNewOn)
{
	bToggledOn = bNewOn;
	ToggleNoiseAccum = 0.f;
}

void APickupItem::SetSpraying(bool bNewSpraying)
{
	bSpraying = bNewSpraying;
	if (SprayCone)
	{
		SprayCone->SetVisibility(bSpraying);
	}
}

void APickupItem::OnRep_Spraying()
{
	if (SprayCone)
	{
		SprayCone->SetVisibility(bSpraying);
	}
}

void APickupItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickupItem, Charges);
	DOREPLIFETIME(APickupItem, bSpraying);
	DOREPLIFETIME(APickupItem, bToggledOn);
}

void APickupItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Декоративное вращение, пока предмет лежит свободно.
	// Крутим только на сервере — клиентам позиция приходит репликацией движения.
	if (HasAuthority() && RotationSpeed != 0.f && !GetAttachParentActor() && !MeshComponent->IsSimulatingPhysics())
	{
		AddActorLocalRotation(FRotator(0.f, RotationSpeed * DeltaSeconds, 0.f));
	}

	// Включённая рация шипит эфиром — слышно даже из кармана (задел под монстра)
	if (HasAuthority() && bToggledOn && ItemEffect == EItemEffect::Radio)
	{
		ToggleNoiseAccum += DeltaSeconds;
		if (ToggleNoiseAccum >= 2.f)
		{
			ToggleNoiseAccum = 0.f;
			MakeNoise(0.8f, Cast<APawn>(GetOwner()), GetActorLocation());
		}
	}
}
