#include "Components/UFlashlightComponent.h"

#include "Components/LightComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UFlashlightComponent::UFlashlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	bIsOn = false;
	BatteryLevel = 100.f;
	DrainPerSecond = 0.5f;
	LowBatteryThreshold = 15.f;
	BlackoutChancePerSecond = 0.35f;

	DefaultIntensity = -1.f;
	BlackoutTimeRemaining = 0.f;
	bLowBatteryNotified = false;
}

void UFlashlightComponent::BeginPlay()
{
	Super::BeginPlay();

	// Свет не создаём сами: либо назначен в Blueprint, либо ищем у владельца
	if (!AttachedLight && GetOwner())
	{
		AttachedLight = GetOwner()->FindComponentByClass<ULightComponent>();
	}

	if (AttachedLight)
	{
		DefaultIntensity = AttachedLight->Intensity;
	}

	ApplyLightState();
}

void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const AActor* Owner = GetOwner();
	const bool bHasAuthority = Owner && Owner->HasAuthority();

	// Разряд батареи считает только сервер, значение приходит клиентам репликацией
	if (bIsOn && bHasAuthority)
	{
		const float OldLevel = BatteryLevel;
		BatteryLevel = FMath::Max(0.f, BatteryLevel - DrainPerSecond * DeltaTime);

		if (!bLowBatteryNotified && BatteryLevel < LowBatteryThreshold)
		{
			bLowBatteryNotified = true;
			OnBatteryLow.Broadcast();
		}

		if (BatteryLevel <= 0.f && OldLevel > 0.f)
		{
			OnBatteryEmpty.Broadcast();
			TurnOff(); // батарея села — выключаемся принудительно
		}
	}

	UpdateFlicker(DeltaTime);
}

void UFlashlightComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFlashlightComponent, bIsOn);
	DOREPLIFETIME(UFlashlightComponent, BatteryLevel);
}

void UFlashlightComponent::TurnOn()
{
	// С пустой батареей включиться нельзя — сначала Recharge()
	if (bIsOn || BatteryLevel <= 0.f)
	{
		return;
	}

	bIsOn = true;
	ApplyLightState();
	OnFlashlightToggled.Broadcast(true);
}

void UFlashlightComponent::TurnOff()
{
	if (!bIsOn)
	{
		return;
	}

	bIsOn = false;
	BlackoutTimeRemaining = 0.f;
	ApplyLightState();
	OnFlashlightToggled.Broadcast(false);
}

void UFlashlightComponent::Toggle()
{
	if (bIsOn)
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}

void UFlashlightComponent::Recharge(float Amount)
{
	BatteryLevel = FMath::Clamp(BatteryLevel + Amount, 0.f, 100.f);

	if (BatteryLevel >= LowBatteryThreshold)
	{
		bLowBatteryNotified = false; // позволит OnBatteryLow сработать снова
	}
}

void UFlashlightComponent::OnRep_IsOn()
{
	// Пришло новое состояние с сервера — обновляем свет на клиенте
	ApplyLightState();
	OnFlashlightToggled.Broadcast(bIsOn);
}

void UFlashlightComponent::ApplyLightState()
{
	if (!AttachedLight)
	{
		return;
	}

	AttachedLight->SetVisibility(bIsOn);

	if (bIsOn && DefaultIntensity >= 0.f)
	{
		AttachedLight->SetIntensity(DefaultIntensity);
	}
}

void UFlashlightComponent::UpdateFlicker(float DeltaTime)
{
	if (!AttachedLight || !bIsOn)
	{
		return;
	}

	// Заряд в норме — горим ровно
	if (BatteryLevel >= LowBatteryThreshold)
	{
		if (DefaultIntensity >= 0.f)
		{
			AttachedLight->SetIntensity(DefaultIntensity);
		}
		BlackoutTimeRemaining = 0.f;
		return;
	}

	// Идёт случайное кратковременное отключение
	if (BlackoutTimeRemaining > 0.f)
	{
		BlackoutTimeRemaining -= DeltaTime;
		AttachedLight->SetIntensity(BlackoutTimeRemaining > 0.f ? 0.f : DefaultIntensity);
		return;
	}

	// Шанс начать отключение на 0.2-0.5 сек
	if (FMath::FRand() < BlackoutChancePerSecond * DeltaTime)
	{
		BlackoutTimeRemaining = FMath::FRandRange(0.2f, 0.5f);
		AttachedLight->SetIntensity(0.f);
		return;
	}

	// Нервное дрожание интенсивности
	const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	const float Flicker = 0.55f + 0.45f * FMath::Abs(FMath::Sin(Time * 25.f)) * FMath::FRandRange(0.6f, 1.f);
	AttachedLight->SetIntensity(DefaultIntensity * Flicker);
}
