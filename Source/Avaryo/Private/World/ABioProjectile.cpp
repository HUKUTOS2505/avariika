#include "World/ABioProjectile.h"

#include "AvaryoCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "UObject/ConstructorHelpers.h"

ABioProjectile::ABioProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicatingMovement(true); // клиенты видят полёт и место падения

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BioMesh(TEXT("/Game/Avariika/Meshes/SM_BioBlob/SM_BioBlob.SM_BioBlob"));
	if (BioMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(BioMesh.Object); // реальный био-комок (~14 см)
	}
	MeshComponent->SetRelativeScale3D(FVector(0.75f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // о монтёров не отскакивает — шлёпает
	MeshComponent->SetNotifyRigidBodyCollision(true);
	// Физику включаем только на сервере (BeginPlay) — клиентам приходит реплицированное движение

	ThrowSpeed = 1200.f;
	HitRadius = 90.f;
	SplatRadius = 300.f;
	SplatSmell = 70.f;
	SplatPanic = 25.f;
	LingerTime = 5.f;
	MaxAirborneTime = 8.f;

	bSplatted = false;
	SettledTime = 0.f;
	LingerRemaining = 0.f;
	StinkNoiseAccum = 0.f;
	AirborneTime = 0.f;
}

void ABioProjectile::BeginPlay()
{
	Super::BeginPlay();
	// Симулирует только сервер; клиенты — кинематик + реплицированное движение (без дёрганья)
	if (MeshComponent)
	{
		MeshComponent->SetSimulatePhysics(HasAuthority());
	}
}

void ABioProjectile::Launch(const FVector& Direction)
{
	if (MeshComponent && MeshComponent->IsSimulatingPhysics())
	{
		MeshComponent->SetPhysicsLinearVelocity(Direction.GetSafeNormal() * ThrowSpeed);
	}
}

void ABioProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	// После шлепка — «амбре-зона»: травим запахом и периодически воняем (шум для монстра)
	if (bSplatted)
	{
		LingerRemaining -= DeltaSeconds;
		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			if (It->VitalsComponent
				&& FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= FMath::Square(SplatRadius))
			{
				It->VitalsComponent->AddSmell(20.f * DeltaSeconds);
			}
		}
		StinkNoiseAccum += DeltaSeconds;
		if (StinkNoiseAccum >= 1.5f)
		{
			StinkNoiseAccum = 0.f;
			MakeNoise(0.5f, nullptr, GetActorLocation()); // запах «зовёт» — задел под монстра
		}
		if (LingerRemaining <= 0.f)
		{
			Destroy();
		}
		return;
	}

	// Предохранитель: снаряд, который не осел и не попал (катится по лестнице/застрял в углу),
	// иначе тикал бы вечно, перебирая всех монтёров каждый кадр. Через MaxAirborneTime — форс-шлепок.
	AirborneTime += DeltaSeconds;
	if (AirborneTime >= MaxAirborneTime)
	{
		Splat(nullptr);
		return;
	}

	// Прямое попадание в монтёра
	const float HitRadiusSq = FMath::Square(HitRadius);
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (*It != GetInstigator()
			&& FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= HitRadiusSq)
		{
			Splat(*It);
			return;
		}
	}

	// Снаряд успокоился (упал и лежит) — шлёпок о землю
	const float Speed = MeshComponent ? MeshComponent->GetPhysicsLinearVelocity().Size() : 0.f;
	if (Speed < 40.f)
	{
		SettledTime += DeltaSeconds;
		if (SettledTime >= 0.4f)
		{
			Splat(nullptr);
		}
	}
	else
	{
		SettledTime = 0.f;
	}
}

void ABioProjectile::Splat(AAvaryoCharacter* DirectHit)
{
	bSplatted = true;
	LingerRemaining = LingerTime;

	if (MeshComponent)
	{
		MeshComponent->SetSimulatePhysics(false);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Прямой жертве — по полной
	if (DirectHit && DirectHit->VitalsComponent)
	{
		DirectHit->VitalsComponent->AddSmell(SplatSmell);
		DirectHit->VitalsComponent->AddPanic(SplatPanic);
	}

	// Всем в радиусе — паника от вида и запаха
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (*It == DirectHit || !It->VitalsComponent)
		{
			continue;
		}
		if (FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= FMath::Square(SplatRadius))
		{
			It->VitalsComponent->AddPanic(SplatPanic * 0.5f);
			It->VitalsComponent->AddSmell(SplatSmell * 0.4f);
		}
	}

	MakeNoise(0.8f, GetInstigator<APawn>(), GetActorLocation());

	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->NotifyBioSplat(DirectHit);
	}
}
