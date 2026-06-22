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

	// Диск делаем плоским и по радиусу скольжения (цилиндр в BasicShapes — 100 см диаметр, 100 высота).
	// Значение константно — переставляем только при изменении, не дёргаем transform каждый тик (CODE_AUDIT3 #7).
	const FVector DesiredScale(SlipRadius / 50.f, SlipRadius / 50.f, 0.04f);
	if (!MeshComponent->GetRelativeScale3D().Equals(DesiredScale))
	{
		MeshComponent->SetRelativeScale3D(DesiredScale);
	}

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
