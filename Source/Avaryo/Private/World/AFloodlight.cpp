#include "World/AFloodlight.h"

#include "AvaryoCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

AFloodlight::AFloodlight()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.25f; // успокоение и гул — не покадрово
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FloodMesh(TEXT("/Game/Avariika/Meshes/SM_Floodlight/SM_Floodlight.SM_Floodlight"));
	if (FloodMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(FloodMesh.Object); // реальная модель прожектора на треноге (~130 см)
	}
	MeshComponent->SetRelativeScale3D(FVector(1.0f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	Light = CreateDefaultSubobject<USpotLightComponent>(TEXT("Light"));
	Light->SetupAttachment(MeshComponent);
	Light->SetRelativeLocation(FVector(12.f, 0.f, 105.f)); // у головы прожектора, чуть вперёд
	Light->SetRelativeRotation(FRotator(-22.f, 0.f, 0.f)); // конус вперёд и чуть вниз — туда, куда «смотрит»
	Light->SetUsingAbsoluteScale(true);
	Light->SetLightColor(FColor(255, 240, 210)); // тёплый рабочий свет
	Light->SetIntensity(40000.f);                // у spot-света нужно больше, чем у point
	Light->SetAttenuationRadius(2200.f);
	Light->SetInnerConeAngle(26.f);
	Light->SetOuterConeAngle(46.f);
	Light->SetCastShadows(true);

	// Гул лампы: 3D-луп, авто-старт на всех машинах (компонент на реплицируемом акторе)
	HumAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("HumAudio"));
	HumAudio->SetupAttachment(MeshComponent);
	HumAudio->SetRelativeLocation(FVector(0.f, 0.f, 105.f)); // у головы прожектора
	HumAudio->bAutoActivate = true;
	HumAudio->SetVolumeMultiplier(0.4f);
	static ConstructorHelpers::FObjectFinder<USoundBase> HumSnd(TEXT("/Game/Audio/SFX/Hazard/Hazard_LampHum_Loop.Hazard_LampHum_Loop"));
	if (HumSnd.Succeeded())
	{
		HumSound = HumSnd.Object;
		HumAudio->SetSound(HumSnd.Object);
	}
	HumAudio->bOverrideAttenuation = true;
	HumAudio->AttenuationOverrides.bAttenuate = true;
	HumAudio->AttenuationOverrides.bSpatialize = true;
	HumAudio->AttenuationOverrides.AttenuationShape = EAttenuationShape::Sphere;
	HumAudio->AttenuationOverrides.AttenuationShapeExtents = FVector(250.f, 0.f, 0.f);
	HumAudio->AttenuationOverrides.FalloffDistance = 2000.f;

	CalmRadius = 700.f;
	CalmPerSecond = 4.f;
	NoiseInterval = 3.f;
	NoiseLoudness = 0.5f;
	NoiseAccum = 0.f;
}

void AFloodlight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	// Свет успокаивает: всем монтёрам в радиусе тихо снимаем панику
	const float CalmRadiusSq = FMath::Square(CalmRadius);
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (It->VitalsComponent
			&& FVector::DistSquared(It->GetActorLocation(), GetActorLocation()) <= CalmRadiusSq)
		{
			It->VitalsComponent->ReducePanic(CalmPerSecond * DeltaSeconds);
		}
	}

	// ...но гудит и выдаёт позицию — задел под монстра-слухача
	NoiseAccum += DeltaSeconds;
	if (NoiseAccum >= NoiseInterval)
	{
		NoiseAccum = 0.f;
		MakeNoise(NoiseLoudness, nullptr, GetActorLocation());
	}
}
