#include "World/AFoamPatch.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AFoamPatch::AFoamPatch()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.5f; // только отсчёт времени жизни — не покадрово
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CylinderMesh.Object); // плоский диск-лужа
	}
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // не мешает ходьбе
	MeshComponent->SetCastShadow(false);

	SlipRadius = 150.f;
	Lifetime = 25.f;
	ElapsedLife = 0.f;
}

void AFoamPatch::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Диск делаем плоским и по радиусу скольжения (цилиндр в BasicShapes — 100 см диаметр, 100 высота)
	MeshComponent->SetRelativeScale3D(FVector(SlipRadius / 50.f, SlipRadius / 50.f, 0.04f));

	if (!HasAuthority())
	{
		return;
	}

	ElapsedLife += DeltaSeconds;
	if (ElapsedLife >= Lifetime)
	{
		Destroy(); // высохла
	}
}
