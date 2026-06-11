#include "Components/VitalsComponent.h"

#include "AvaryoCharacter.h"
#include "Components/UFlashlightComponent.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

UVitalsComponent::UVitalsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// Стартовые 50/50 — чтобы сразу тестировать аптечку и сигареты.
	// Перед релизом вернуть Health=100, Panic=0.
	Health = 50.f;
	Panic = 50.f;
	Stamina = 100.f;
	Bladder = 0.f;
	bWounded = false;
	bSoiled = false;
	IncidentSlowRemaining = 0.f;
	bSprinting = false;

	PanicRiseInDarkPerSecond = 1.5f;
	PanicFallInLightPerSecond = 2.f;
	PanicRiseAlonePerSecond = 0.5f;
	PanicFallNearTeammatePerSecond = 1.f;
	TeammateRadius = 1000.f;
	PanicRiseWoundedPerSecond = 3.f;
	PanicThreshold = 70.f;

	StaminaDrainPerSecond = 12.f;
	StaminaHeavyMultiplier = 1.6f;
	StaminaRegenPerSecond = 10.f;

	BladderRisePerSecond = 0.2f;
	BladderPanicMultiplier = 2.f;
	IncidentSlowDuration = 15.f;
	IncidentPanicSpike = 25.f;

	WoundedReviveThreshold = 25.f;

	SmokingDuration = 15.f;
	SmokingPanicPerSecond = 2.f;
	SmokingRemaining = 0.f;
}

void UVitalsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UVitalsComponent, Health);
	DOREPLIFETIME(UVitalsComponent, Panic);
	DOREPLIFETIME(UVitalsComponent, Stamina);
	DOREPLIFETIME(UVitalsComponent, Bladder);
	DOREPLIFETIME(UVitalsComponent, bWounded);
	DOREPLIFETIME(UVitalsComponent, bSoiled);
	DOREPLIFETIME(UVitalsComponent, IncidentSlowRemaining);
	DOREPLIFETIME(UVitalsComponent, bSprinting);
	DOREPLIFETIME(UVitalsComponent, SmokingRemaining);
}

void UVitalsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AAvaryoCharacter* Char = Cast<AAvaryoCharacter>(GetOwner());
	if (!Char || !Char->HasAuthority())
	{
		return; // вся динамика — на сервере
	}

	// --- Паника ---
	float PanicDelta = 0.f;

	const bool bLit = Char->FlashlightComponent && Char->FlashlightComponent->IsOn();
	PanicDelta += bLit ? -PanicFallInLightPerSecond : PanicRiseInDarkPerSecond;

	bool bTeammateNear = false;
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (*It != Char && FVector::DistSquared(It->GetActorLocation(), Char->GetActorLocation()) < FMath::Square(TeammateRadius))
		{
			bTeammateNear = true;
			break;
		}
	}
	PanicDelta += bTeammateNear ? -PanicFallNearTeammatePerSecond : PanicRiseAlonePerSecond;

	if (bWounded)
	{
		PanicDelta += PanicRiseWoundedPerSecond;
	}

	// Перекур глушит страх: пассивный рост паники не действует,
	// и каждая секунда курения снимает SmokingPanicPerSecond очков
	if (SmokingRemaining > 0.f)
	{
		SmokingRemaining = FMath::Max(0.f, SmokingRemaining - DeltaTime);
		PanicDelta = -SmokingPanicPerSecond;
	}
	else
	{
		// Совместный перекур: курящий рядом тиммейт успокаивает и тебя (вполовину слабее).
		// Бригада сбивается в курилку — и дружно шумит на радость будущему монстру
		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			if (*It != Char && It->VitalsComponent && It->VitalsComponent->IsSmoking()
				&& FVector::DistSquared(It->GetActorLocation(), Char->GetActorLocation()) < FMath::Square(250.f))
			{
				PanicDelta = FMath::Min(PanicDelta, -SmokingPanicPerSecond * 0.5f);
				break;
			}
		}
	}

	Panic = FMath::Clamp(Panic + PanicDelta * DeltaTime, 0.f, 100.f);

	// --- Выносливость ---
	const bool bMoving = Char->GetVelocity().SizeSquared2D() > 100.f;
	if (bSprinting && bMoving && !bWounded)
	{
		const float Drain = StaminaDrainPerSecond * (Char->IsCarryingHeavy() ? StaminaHeavyMultiplier : 1.f);
		Stamina -= Drain * DeltaTime;
		if (Stamina <= 0.f)
		{
			Stamina = 0.f;
			bSprinting = false; // выдохся
		}
	}
	else
	{
		Stamina = FMath::Min(100.f, Stamina + StaminaRegenPerSecond * DeltaTime);
	}

	// --- Туалет ---
	const float BladderRise = BladderRisePerSecond * (IsPanicking() ? BladderPanicMultiplier : 1.f);
	Bladder += BladderRise * DeltaTime;
	if (Bladder >= 100.f)
	{
		// Санитарный инцидент: громко, стыдно, попадёт в отчёт
		Bladder = 0.f;
		bSoiled = true;
		IncidentSlowRemaining = IncidentSlowDuration;
		Panic = FMath::Min(100.f, Panic + IncidentPanicSpike);
		Char->MakeNoise(1.f, Char, Char->GetActorLocation()); // очень громко и стыдно
		OnSanitaryIncident.Broadcast();
	}

	if (IncidentSlowRemaining > 0.f)
	{
		IncidentSlowRemaining = FMath::Max(0.f, IncidentSlowRemaining - DeltaTime);
	}
}

void UVitalsComponent::ApplyDamage(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}

	Health = FMath::Max(0.f, Health - Amount);
	Panic = FMath::Min(100.f, Panic + Amount * 0.5f); // боль пугает

	if (Health <= 0.f && !bWounded)
	{
		bWounded = true;
		bSprinting = false;
		OnWounded.Broadcast();
	}
}

void UVitalsComponent::Heal(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}

	Health = FMath::Clamp(Health + Amount, 0.f, 100.f);

	if (bWounded && Health >= WoundedReviveThreshold)
	{
		bWounded = false;
		OnRevived.Broadcast();
	}
}

void UVitalsComponent::AddPanic(float Amount)
{
	Panic = FMath::Clamp(Panic + Amount, 0.f, 100.f);
}

void UVitalsComponent::ReducePanic(float Amount)
{
	Panic = FMath::Clamp(Panic - Amount, 0.f, 100.f);
}

void UVitalsComponent::SetSprinting(bool bNewSprinting)
{
	// Нельзя бежать раненым или совсем без сил
	bSprinting = bNewSprinting && !bWounded && Stamina > 5.f;
}

void UVitalsComponent::RelieveBladder()
{
	Bladder = 0.f;
}

void UVitalsComponent::StartSmoking()
{
	SmokingRemaining = SmokingDuration;
}
