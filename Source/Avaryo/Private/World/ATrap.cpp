#include "World/ATrap.h"

#include "AvaryoCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/AvaryoCameraShakes.h"
#include "UObject/ConstructorHelpers.h"

ATrap::ATrap()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f; // частить незачем — зона срабатывания большая
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TrapMesh(TEXT("/Game/Avariika/Meshes/SM_Trap/SM_Trap.SM_Trap"));
	if (TrapMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(TrapMesh.Object); // реальная развёрнутая растяжка (~20 см)
	}
	MeshComponent->SetRelativeScale3D(FVector(0.565f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	TriggerSphere->SetupAttachment(MeshComponent);
	TriggerSphere->SetUsingAbsoluteScale(true);
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // детект делаем перебором, не оверлапом

	IndicatorLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Indicator"));
	IndicatorLight->SetupAttachment(MeshComponent);
	IndicatorLight->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
	IndicatorLight->SetUsingAbsoluteScale(true);
	IndicatorLight->SetLightColor(FColor(255, 140, 0)); // оранжевый акцент проекта
	IndicatorLight->SetIntensity(0.f);
	IndicatorLight->SetAttenuationRadius(400.f);
	IndicatorLight->SetCastShadows(false);

	ArmDelay = 2.0f;
	TriggerRadius = 250.f;
	NoiseLoudness = 1.5f;
	PanicAmount = 25.f;
	PlacerGraceTime = 3.0f;

	bArmed = false;
	bTriggered = false;
	ArmedAtTime = 0.f;
}

void ATrap::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerSphere)
	{
		TriggerSphere->SetSphereRadius(TriggerRadius);
	}

	if (HasAuthority())
	{
		// Взвод с задержкой — поставивший успевает отойти, и нельзя подорвать в упор сразу
		FTimerHandle ArmTimer;
		GetWorldTimerManager().SetTimer(ArmTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			bArmed = true;
			ArmedAtTime = GetWorld()->GetTimeSeconds();
		}), FMath::Max(ArmDelay, 0.05f), false);
	}
}

void ATrap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATrap, bArmed);
}

void ATrap::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Все машины: индикатор мигает, пока ловушка взведена и не сработала
	if (IndicatorLight)
	{
		if (bArmed && !bTriggered)
		{
			const float Blink = FMath::Square(FMath::Sin(GetWorld()->GetTimeSeconds() * 6.f));
			IndicatorLight->SetIntensity(200.f + 1200.f * Blink);
		}
		else if (!bArmed)
		{
			IndicatorLight->SetIntensity(0.f);
		}
	}

	// Сервер: ищем, кто наступил в зону
	if (!HasAuthority() || !bArmed || bTriggered)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const bool bGraceActive = (Now - ArmedAtTime) < PlacerGraceTime;
	const AActor* Placer = GetInstigator();
	const float RadiusSq = FMath::Square(TriggerRadius);

	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) > RadiusSq)
		{
			continue;
		}
		if (bGraceActive && *It == Placer)
		{
			continue; // поставивший ещё под защитой — даём отойти
		}
		Spring(*It);
		break;
	}
}

void ATrap::Spring(AAvaryoCharacter* TriggeredBy)
{
	bTriggered = true;

	// Грохот на всю карту — задел под монстра-слухача
	MakeNoise(NoiseLoudness, TriggeredBy, GetActorLocation());

	// Все рядом подпрыгивают от неожиданности
	const float PanicRadiusSq = FMath::Square(TriggerRadius * 1.5f);
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (It->VitalsComponent
			&& FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= PanicRadiusSq)
		{
			It->VitalsComponent->AddPanic(PanicAmount);
		}
	}

	MulticastFlash();

	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->NotifyTrapTriggered(TriggeredBy);
	}

	SetLifeSpan(0.3f); // дать вспышке отыграть и убрать ловушку
}

void ATrap::MulticastFlash_Implementation()
{
	if (IndicatorLight)
	{
		IndicatorLight->SetLightColor(FColor(255, 255, 255));
		IndicatorLight->SetIntensity(8000.f);
	}
	UGameplayStatics::PlayWorldCameraShake(this, UExplosionCameraShake::StaticClass(),
		GetActorLocation(), TriggerRadius * 0.5f, TriggerRadius * 3.f);
}
