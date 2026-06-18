#include "World/ADoor.h"

#include "AvaryoCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	SetRootComponent(Hinge);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Hinge);
	// Полотно смещено от петли по Y на полудлину → вращается вокруг края (петли).
	DoorMesh->SetRelativeLocation(FVector(0.f, LeafOffsetY, 100.f));
	DoorMesh->SetRelativeScale3D(FVector(0.1f, 1.0f, 2.0f)); // тонкая дверь-заглушка (куб)
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorMesh->SetCollisionResponseToAllChannels(ECR_Block); // полотно перекрывает проход, пока закрыто
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded()) { DoorMesh->SetStaticMesh(CubeMesh.Object); }

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	Zone->SetupAttachment(Hinge);
	Zone->SetRelativeLocation(FVector(0.f, LeafOffsetY, 100.f));
	Zone->SetBoxExtent(FVector(80.f, 90.f, 110.f));
	Zone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Zone->SetCollisionResponseToAllChannels(ECR_Overlap);

	// Обычная бытовая дверь (нормальный звук). В библиотеке нет идеального «дверь открылась» — ближайший household.
	static ConstructorHelpers::FObjectFinder<USoundBase> Snd(TEXT("/Game/Audio/Lib/door_impact/Ghosthack-SF_Household_Cabinet_Cupboard_Slide_Door_Close_01.Ghosthack-SF_Household_Cabinet_Cupboard_Slide_Door_Close_01"));
	if (Snd.Succeeded()) { OpenSound = Snd.Object; }
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bOpen);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	// Применить смещение полотна/зоны под текущую LeafOffsetY (могли поменять при размещении).
	if (DoorMesh) { FVector L = DoorMesh->GetRelativeLocation(); L.Y = LeafOffsetY; DoorMesh->SetRelativeLocation(L); }
	if (Zone) { FVector L = Zone->GetRelativeLocation(); L.Y = LeafOffsetY; Zone->SetRelativeLocation(L); }
	CurrentAngle = bOpen ? OpenAngle : 0.f;
	// Вращаем ПОЛОТНО (а не корень-петлю): корень = трансформ актора, и его поворот
	// затирал бы поворот размещения двери в уровне. Полотно/зону крутим относительно петли.
	const FRotator Swing(0.f, CurrentAngle, 0.f);
	if (DoorMesh) { DoorMesh->SetRelativeRotation(Swing); }
	if (Zone) { Zone->SetRelativeRotation(Swing); }
}

void ADoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const float Target = bOpen ? OpenAngle : 0.f;
	if (!FMath::IsNearlyEqual(CurrentAngle, Target, 0.05f))
	{
		CurrentAngle = FMath::FInterpTo(CurrentAngle, Target, DeltaSeconds, OpenSpeed);
		const FRotator Swing(0.f, CurrentAngle, 0.f);
		if (DoorMesh) { DoorMesh->SetRelativeRotation(Swing); }
		if (Zone) { Zone->SetRelativeRotation(Swing); }
	}
}

void ADoor::ToggleBy(AAvaryoCharacter* /*Who*/)
{
	if (!HasAuthority()) { return; }
	bOpen = !bOpen;
	if (OpenSound) { UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation()); }
	// сервер тоже сразу двигает (клиенты — через OnRep + Tick)
}

void ADoor::OnRep_Open()
{
	if (OpenSound) { UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation()); }
}
