#include "Components/UFlashlightComponent.h"

#include "AvaryoCharacter.h"
#include "Components/LightComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UFlashlightComponent::UFlashlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	bIsOn = false;
	BatteryLevel = 100.f;
	DrainPerSecond = 0.5f;
	LowBatteryThreshold = 15.f;
	BlackoutChancePerSecond = 0.35f;
	CheapGlitchChancePerSecond = 0.12f;
	bCheapUnit = false;
	DeadBatteryFright = 12.f;

	DefaultIntensity = -1.f;
	BeamIntensity = 3500.f; // cd: видимый луч, но не выбивает тёмную комнату в белое (было 14000 = белый экран). 0 = брать из BP
	BlackoutTimeRemaining = 0.f;
	bLowBatteryNotified = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> ClickSnd(TEXT("/Game/Survival_SFX/User_Interface/Button_press_1.Button_press_1"));
	if (ClickSnd.Succeeded()) { ClickSound = ClickSnd.Object; }
}

void UFlashlightComponent::DebugSetBattery(float Pct)
{
	const float OldLevel = BatteryLevel;
	BatteryLevel = FMath::Clamp(Pct, 0.f, 100.f);
	if (BatteryLevel >= LowBatteryThreshold) { bLowBatteryNotified = false; }

	// Дев-команда `AvBattery 0`: сразу показать испуг от севшей батареи (паника + шум),
	// не дожидаясь медленного разряда — иначе тест неудобный.
	if (BatteryLevel <= 0.f && OldLevel > 0.f && bIsOn)
	{
		OnBatteryEmpty.Broadcast();
		TriggerDeadBatteryFright();
	}
}

void UFlashlightComponent::TriggerDeadBatteryFright()
{
	TurnOff(); // батарея села — выключаемся принудительно

	// Внезапная темнота пугает: скачок паники + испуганный вздох (выдаёт позицию)
	if (AAvaryoCharacter* Char = Cast<AAvaryoCharacter>(GetOwner()))
	{
		if (Char->VitalsComponent) { Char->VitalsComponent->AddPanic(DeadBatteryFright); }
		Char->MakeNoise(0.5f, Char, Char->GetActorLocation());
		Char->RegisterSelfNoise(0.5f);
	}
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
			TriggerDeadBatteryFright();
		}
	}

	UpdateFlicker(DeltaTime);
}

void UFlashlightComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFlashlightComponent, bIsOn);
	DOREPLIFETIME(UFlashlightComponent, BatteryLevel);
	DOREPLIFETIME(UFlashlightComponent, bCheapUnit);
}

void UFlashlightComponent::SetCheapUnit(bool bNewCheap)
{
	bCheapUnit = bNewCheap;
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
	// Щелчок тумблера на каждом переключении (первый вызов из BeginPlay пропускаем)
	if (bClickReady && ClickSound)
	{
		if (const AActor* Owner = GetOwner())
		{
			UGameplayStatics::PlaySoundAtLocation(this, ClickSound, Owner->GetActorLocation());
		}
	}
	bClickReady = true;

	if (!AttachedLight)
	{
		return;
	}

	AttachedLight->SetVisibility(bIsOn);

	if (bIsOn)
	{
		AttachedLight->SetIntensity(OnIntensity());
	}
}

void UFlashlightComponent::ForceBlackout(float Duration)
{
	// Берём максимум — повторный скачок не укорачивает темноту
	ForcedBlackoutRemaining = FMath::Max(ForcedBlackoutRemaining, Duration);
}

void UFlashlightComponent::UpdateFlicker(float DeltaTime)
{
	if (!AttachedLight || !bIsOn)
	{
		return;
	}

	// Мерцание — чистая косметика; на выделенном сервере луч не рендерится → не жжём RNG/Sin/SetIntensity (CODE_AUDIT3 #18)
	const UWorld* W = GetWorld();
	if (W && W->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	// Принудительное отключение (перегрузка/скачок) — приоритетнее заряда и дешёвого юнита
	if (ForcedBlackoutRemaining > 0.f)
	{
		ForcedBlackoutRemaining -= DeltaTime;
		AttachedLight->SetIntensity(0.f);
		return;
	}

	// Заряд в норме
	if (BatteryLevel >= LowBatteryThreshold)
	{
		// Дешёвый фонарь моргает и при полном заряде — но редко (косяк оборудования)
		if (bCheapUnit)
		{
			if (BlackoutTimeRemaining > 0.f)
			{
				BlackoutTimeRemaining -= DeltaTime;
				AttachedLight->SetIntensity(BlackoutTimeRemaining > 0.f ? 0.f : OnIntensity());
				return;
			}
			if (FMath::FRand() < CheapGlitchChancePerSecond * DeltaTime)
			{
				BlackoutTimeRemaining = FMath::FRandRange(0.08f, 0.22f); // короткий «щёлк» темноты
				AttachedLight->SetIntensity(0.f);
				return;
			}
		}

		// Иначе горим ровно
		AttachedLight->SetIntensity(OnIntensity());
		BlackoutTimeRemaining = 0.f;
		return;
	}

	// Идёт случайное кратковременное отключение
	if (BlackoutTimeRemaining > 0.f)
	{
		BlackoutTimeRemaining -= DeltaTime;
		AttachedLight->SetIntensity(BlackoutTimeRemaining > 0.f ? 0.f : OnIntensity());
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
	AttachedLight->SetIntensity(OnIntensity() * Flicker);
}
