#include "Components/VitalsComponent.h"

#include "AvaryoCharacter.h"
#include "Components/UFlashlightComponent.h"
#include "Game/ARunState.h"
#include "Components/PointLightComponent.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UObjectIterator.h"
#include "HAL/IConsoleManager.h"

// Тумблер отключения паники для тестов: `Av.NoPanic 0` вернёт нормальную панику.
// По умолчанию 1 (паника ВЫКЛ), пока доводим механики/окружение. Вернуть 0 перед релизом.
static TAutoConsoleVariable<int32> CVarAvNoPanic(
	TEXT("Av.NoPanic"), 1,
	TEXT("1 = не накапливать панику (тест), 0 = нормальная паника."));

UVitalsComponent::UVitalsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// Витали — медленные шкалы; тикаем 10 Гц вместо 60. Вся математика на DeltaTime,
	// значения не меняются, но пер-фреймовые обходы мира (соседи/курящие/вонь/паника) реже в 6 раз.
	PrimaryComponentTick.TickInterval = 0.1f;
	SetIsReplicatedByDefault(true);

	// Стартовые 50/50 — чтобы сразу тестировать аптечку и сигареты.
	// Перед релизом вернуть Health=100, Panic=0.
	Health = 50.f;
	Panic = 50.f;
	Stamina = 100.f;
	Bladder = 0.f;
	bWounded = false;
	bUnconscious = false;
	WoundedBleedOut = 0.f;
	bSoiled = false;
	IncidentSlowRemaining = 0.f;
	WetRemaining = 0.f;
	BurnRemaining = 0.f;
	bSprinting = false;

	PanicRiseInDarkPerSecond = 1.5f;
	PanicFallInLightPerSecond = 2.f;
	PanicRiseAlonePerSecond = 0.5f;
	PanicFallNearTeammatePerSecond = 1.f;
	TeammateRadius = 1000.f;
	PanicRiseWoundedPerSecond = 3.f;
	PanicThreshold = 70.f;
	FearContagionRadius = 400.f;
	FearContagionPerSecond = 1.5f;
	CalmLightMinIntensity = 1000.f;
	CoughInterval = 4.f;
	CoughPanic = 1.f;

	HiccupBladderThreshold = 70.f;
	HiccupInterval = 3.5f;
	WindedDuration = 3.5f;

	StaminaDrainPerSecond = 12.f;
	StaminaHeavyMultiplier = 1.6f;
	StaminaRegenPerSecond = 10.f;

	BladderRisePerSecond = 0.2f;
	BladderPanicMultiplier = 2.f;
	IncidentSlowDuration = 15.f;
	IncidentPanicSpike = 25.f;
	WetDuration = 6.f;
	WetPanicPerSecond = 1.5f;
	BurnDuration = 6.f;
	BurnDamagePerSecond = 6.f;
	BurnPanicPerSecond = 8.f;

	WoundedReviveThreshold = 25.f;

	SmokingDuration = 15.f;
	SmokingPanicPerSecond = 2.f;
	SmokingRemaining = 0.f;

	Smell = 0.f;
	SmellThreshold = 50.f;
	SmellDecayPerSecond = 1.5f;
	SmellSmokingPerSecond = 5.f;
	SmellPanicPerSecond = 1.f;
	SmellIncidentJump = 70.f;
	SmellTeammatePanicPerSecond = 2.f;
	SmellRadius = 350.f;
}

void UVitalsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UVitalsComponent, Health);
	DOREPLIFETIME(UVitalsComponent, Panic);
	DOREPLIFETIME(UVitalsComponent, Stamina);
	DOREPLIFETIME(UVitalsComponent, Bladder);
	DOREPLIFETIME(UVitalsComponent, bWounded);
	DOREPLIFETIME(UVitalsComponent, bUnconscious);
	DOREPLIFETIME(UVitalsComponent, WoundedBleedOut);
	DOREPLIFETIME(UVitalsComponent, ExhaustStunRemaining);
	DOREPLIFETIME(UVitalsComponent, bSoiled);
	DOREPLIFETIME(UVitalsComponent, IncidentSlowRemaining);
	DOREPLIFETIME(UVitalsComponent, WetRemaining);
	DOREPLIFETIME(UVitalsComponent, BurnRemaining);
	DOREPLIFETIME(UVitalsComponent, bSprinting);
	DOREPLIFETIME(UVitalsComponent, SmokingRemaining);
	DOREPLIFETIME(UVitalsComponent, Smell);
}

void UVitalsComponent::MakeWet(float Seconds)
{
	if (!IsVitalAuthority()) { return; }
	const float Dur = (Seconds > 0.f) ? Seconds : WetDuration;
	WetRemaining = FMath::Max(WetRemaining, Dur);
}

void UVitalsComponent::Ignite(float Seconds)
{
	if (!IsVitalAuthority()) { return; }
	if (WetRemaining > 0.f) { return; } // мокрый/только из воды — не загорается
	const bool bWasBurning = BurnRemaining > 0.f;
	const float Dur = (Seconds > 0.f) ? Seconds : BurnDuration;
	BurnRemaining = FMath::Max(BurnRemaining, Dur);
	// Диспетчер реагирует на НОВОЕ возгорание монтёра (не на повторный поджиг уже горящего)
	if (!bWasBurning && BurnRemaining > 0.f)
	{
		if (AAvaryoCharacter* Char = Cast<AAvaryoCharacter>(GetOwner()))
		{
			if (ARunState* Run = ARunState::Get(GetWorld()))
			{
				Run->NotifyBurning(Char);
			}
		}
	}
}

void UVitalsComponent::Extinguish()
{
	if (!IsVitalAuthority()) { return; }
	BurnRemaining = 0.f;
}

void UVitalsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AAvaryoCharacter* Char = Cast<AAvaryoCharacter>(GetOwner());
	if (!Char || !Char->HasAuthority())
	{
		return; // вся динамика — на сервере
	}

	// База (хаб) — безопасная зона: паники тут быть не должно (нет аварий/монстра).
	bool bSafeZone = false;
	if (const ARunState* Run = ARunState::Get(GetWorld()))
	{
		bSafeZone = Run->IsHubMode();
	}

	// --- Паника ---
	float PanicDelta = 0.f;

	bool bLit = Char->FlashlightComponent && Char->FlashlightComponent->IsOn();
	if (!bLit)
	{
		// Любой достаточно яркий свет рядом (прожектор, лампы здания, чужой фонарь) тоже успокаивает.
		// Сканируем не каждый кадр — дорого.
		LightScanAccum += DeltaTime;
		if (LightScanAccum >= 0.3f)
		{
			LightScanAccum = 0.f;
			bInLightCached = IsLitByNearbyLight(Char);
		}
		bLit = bInLightCached;
	}
	// Свет успокаивает всех; страх темноты не вешаем на раненого — ему и так идёт +PanicRiseWounded
	if (bLit)
	{
		PanicDelta += -PanicFallInLightPerSecond;
	}
	else if (!bWounded)
	{
		PanicDelta += PanicRiseInDarkPerSecond;
	}

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

	// Истечение в стадии Ранен → отруб (Без сознания), если не подняли/не подлечился вовремя.
	if (bWounded && !bUnconscious && WoundedBleedOut > 0.f)
	{
		WoundedBleedOut = FMath::Max(0.f, WoundedBleedOut - DeltaTime);
		if (WoundedBleedOut <= 0.f) { bUnconscious = true; }
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

	if (bSafeZone || CVarAvNoPanic.GetValueOnGameThread() != 0)
	{
		// База/тест-режим: паника не растёт и плавно гаснет к нулю
		Panic = FMath::Max(0.f, Panic - 25.f * DeltaTime);
	}
	else
	{
		Panic = FMath::Clamp(Panic + PanicDelta * DeltaTime, 0.f, 100.f);
	}

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
			WindedRemaining = WindedDuration; // отдышка пошла
			ExhaustStunRemaining = ExhaustStunTime; // и стан-стаггер: секунду еле плетёшься
		}
	}
	else
	{
		Stamina = FMath::Min(100.f, Stamina + StaminaRegenPerSecond * DeltaTime);
	}

	// Стан изнурения гаснет (еле плетёшься первую секунду после выдоха).
	if (ExhaustStunRemaining > 0.f)
	{
		ExhaustStunRemaining = FMath::Max(0.f, ExhaustStunRemaining - DeltaTime);
	}

	// Отдышка: выдохшись в ноль, монтёр пару секунд шумно дышит — тактически выдаёт позицию
	if (WindedRemaining > 0.f)
	{
		WindedRemaining = FMath::Max(0.f, WindedRemaining - DeltaTime);
		WindedNoiseAccum += DeltaTime;
		if (WindedNoiseAccum >= 0.6f)
		{
			WindedNoiseAccum = 0.f;
			Char->MakeNoise(0.7f, Char, Char->GetActorLocation());
			Char->RegisterSelfNoise(0.7f);
		}
	}

	// --- Туалет --- (в хабе/safe-zone пузырь НЕ копится: отдыхаешь, инцидентов там нет — как и паники)
	if (!bSafeZone)
	{
		const float BladderRise = BladderRisePerSecond * (IsPanicking() ? BladderPanicMultiplier : 1.f);
		Bladder += BladderRise * DeltaTime;
		if (Bladder >= 100.f)
		{
			// Санитарный инцидент: громко, стыдно, попадёт в отчёт
			Bladder = 0.f;
			bSoiled = true;
			IncidentSlowRemaining = IncidentSlowDuration;
			if (CVarAvNoPanic.GetValueOnGameThread() == 0) { Panic = FMath::Min(100.f, Panic + IncidentPanicSpike); } // уважает тест-тумблер
			Smell = FMath::Min(100.f, Smell + SmellIncidentJump); // и амбре теперь надолго (снимается визитом в туалет — см. RelieveBladder)
			Char->MakeNoise(1.f, Char, Char->GetActorLocation()); // очень громко и стыдно
			Char->RegisterSelfNoise(1.f);
			OnSanitaryIncident.Broadcast();
		}
	}

	if (IncidentSlowRemaining > 0.f)
	{
		IncidentSlowRemaining = FMath::Max(0.f, IncidentSlowRemaining - DeltaTime);
	}

	if (WetRemaining > 0.f)
	{
		WetRemaining = FMath::Max(0.f, WetRemaining - DeltaTime);
		if (!bSafeZone && CVarAvNoPanic.GetValueOnGameThread() == 0)
		{
			Panic = FMath::Min(100.f, Panic + WetPanicPerSecond * DeltaTime); // мокро и зябко → лёгкая паника (уважает NoPanic/hub)
		}
	}

	// --- Горит ---
	if (BurnRemaining > 0.f)
	{
		if (WetRemaining > 0.f)
		{
			// Вода/мокрота тушит огонь — гаснет быстро (×6)
			BurnRemaining = FMath::Max(0.f, BurnRemaining - DeltaTime * 6.f);
		}
		else
		{
			BurnRemaining = FMath::Max(0.f, BurnRemaining - DeltaTime); // тлеет/догорает по времени
			// Огонь НЕ добивает лежачего — окно подъёма (revive) сохраняется (CODE_AUDIT3 #1):
			// ApplyDamage НЕ гейтит wounded, а на Health==0 любой DoT-тик мгновенно эскалировал бы в отруб.
			if (!bWounded)
			{
				ApplyDamage(BurnDamagePerSecond * DeltaTime);                      // DoT (gated bInvulnerable; на раненом не зовём)
				if (!bSafeZone && CVarAvNoPanic.GetValueOnGameThread() == 0)
				{
					Panic = FMath::Min(100.f, Panic + BurnPanicPerSecond * DeltaTime); // мечешься в панике (уважает NoPanic/hub — CODE_AUDIT3 #7)
				}
				Smell = FMath::Min(100.f, Smell + 4.f * DeltaTime);                // палёным воняет
				BurnNoiseAccum += DeltaTime;
				if (BurnNoiseAccum >= 0.5f)
				{
					BurnNoiseAccum = 0.f;
					Char->MakeNoise(0.7f, Char, Char->GetActorLocation()); // кричишь/мечешься — слышно
					Char->RegisterSelfNoise(0.7f);
				}
			}
		}
	}

	// --- Запах / амбре ---
	float SmellDelta = -SmellDecayPerSecond;        // постепенно выветривается
	if (SmokingRemaining > 0.f) SmellDelta += SmellSmokingPerSecond; // дым липнет
	if (IsPanicking())          SmellDelta += SmellPanicPerSecond;   // пот от страха
	if (bSoiled)                SmellDelta = FMath::Max(SmellDelta, 0.f); // испачкан — не выветривается сам
	Smell = FMath::Clamp(Smell + SmellDelta * DeltaTime, 0.f, 100.f);

	// Вонючий монтёр травит тиммейтов рядом — у тех растёт паника
	if (IsSmelly())
	{
		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			if (*It != Char && It->VitalsComponent
				&& FVector::DistSquared(It->GetActorLocation(), Char->GetActorLocation()) < FMath::Square(SmellRadius))
			{
				It->VitalsComponent->AddPanic(SmellTeammatePanicPerSecond * DeltaTime);
			}
		}
	}

	// Кашель: провонявшись газом/химией, монтёр кашляет — шум выдаёт позицию (задел под монстра)
	if (IsSmelly())
	{
		CoughAccum += DeltaTime;
		if (CoughAccum >= CoughInterval)
		{
			CoughAccum = 0.f;
			Char->MakeNoise(0.5f, Char, Char->GetActorLocation());
			Char->RegisterSelfNoise(0.5f);
			AddPanic(CoughPanic);
		}
	}
	else
	{
		CoughAccum = 0.f;
	}

	// Нервная икота при переполненном пузыре: «пора в туалет»-звоночек, тихий шум + крошечная паника
	if (Bladder > HiccupBladderThreshold && !bWounded)
	{
		HiccupAccum += DeltaTime;
		if (HiccupAccum >= HiccupInterval)
		{
			HiccupAccum = 0.f;
			Char->MakeNoise(0.3f, Char, Char->GetActorLocation());
			Char->RegisterSelfNoise(0.3f);
			AddPanic(0.5f);
		}
	}
	else
	{
		HiccupAccum = 0.f;
	}

	// Групповая паника (§18): паникёр заражает страхом соседей. Спокойный тиммейт рядом
	// всё ещё успокаивает (выше), а вот трясущийся — наоборот накручивает.
	if (IsPanicking())
	{
		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			if (*It != Char && It->VitalsComponent
				&& FVector::DistSquared(It->GetActorLocation(), Char->GetActorLocation()) < FMath::Square(FearContagionRadius))
			{
				It->VitalsComponent->AddPanic(FearContagionPerSecond * DeltaTime);
			}
		}
	}
}

bool UVitalsComponent::IsVitalAuthority() const
{
	const AActor* OwnerActor = GetOwner();
	return !OwnerActor || OwnerActor->HasAuthority();
}

bool UVitalsComponent::IsLitByNearbyLight(const AActor* OwnerChar) const
{
	const UWorld* World = GetWorld();
	if (!World || !OwnerChar)
	{
		return false;
	}
	const FVector P = OwnerChar->GetActorLocation();

	// UPointLightComponent ловит и точечные, и прожекторные (USpotLightComponent — его подкласс).
	for (TObjectIterator<UPointLightComponent> It; It; ++It)
	{
		const UPointLightComponent* L = *It;
		if (!IsValid(L) || L->GetWorld() != World || !L->IsVisible() || L->Intensity < CalmLightMinIntensity)
		{
			continue;
		}
		const FLinearColor C = L->GetLightColor();
		if (C.R > 1.5f * FMath::Max(C.G, C.B)) // тревожно-красная лампа (авария) не успокаивает
		{
			continue;
		}
		if (FVector::DistSquared(L->GetComponentLocation(), P) <= FMath::Square(L->AttenuationRadius))
		{
			return true;
		}
	}
	return false;
}

void UVitalsComponent::AddSmell(float Amount)
{
	if (!IsVitalAuthority()) { return; }
	Smell = FMath::Clamp(Smell + Amount, 0.f, 100.f);
}

void UVitalsComponent::DebugSetVital(FName Which, float Value)
{
	const float V = FMath::Clamp(Value, 0.f, 100.f);
	if (Which == TEXT("health"))       { Health = V; if ((bWounded || bUnconscious) && Health >= WoundedReviveThreshold) { bWounded = false; bUnconscious = false; WoundedBleedOut = 0.f; OnRevived.Broadcast(); } }
	else if (Which == TEXT("panic"))   { Panic = V; }
	else if (Which == TEXT("stamina")) { Stamina = V; }
	else if (Which == TEXT("bladder")) { Bladder = V; } // 100 → инцидент сработает на ближайшем тике
	else if (Which == TEXT("smell"))   { Smell = V; if (V < SmellThreshold) { bSoiled = false; } } // дев-сброс «испачкан»
}

void UVitalsComponent::ApplyDamage(float Amount)
{
	if (!IsVitalAuthority() || Amount <= 0.f || bInvulnerable)
	{
		return;
	}

	Health = FMath::Max(0.f, Health - Amount);
	if (CVarAvNoPanic.GetValueOnGameThread() == 0)
	{
		Panic = FMath::Min(100.f, Panic + Amount * 0.5f); // боль пугает (уважает тест-тумблер Av.NoPanic)
	}

	if (Health <= 0.f)
	{
		if (!bWounded)
		{
			bWounded = true;
			bSprinting = false;
			WoundedBleedOut = WoundedBleedOutTime; // окно: подняться/подлечиться/дотащить
			if (WoundedBleedOutTime <= 0.f) { bUnconscious = true; } // нет окна → сразу отруб (без софт-лока)
			OnWounded.Broadcast();
		}
		else if (!bUnconscious)
		{
			bUnconscious = true; // добили лежачего → отруб
		}
	}
}

void UVitalsComponent::Heal(float Amount)
{
	if (!IsVitalAuthority() || Amount <= 0.f)
	{
		return;
	}

	Health = FMath::Clamp(Health + Amount, 0.f, 100.f);

	if ((bWounded || bUnconscious) && Health >= WoundedReviveThreshold)
	{
		bWounded = false;
		bUnconscious = false;
		WoundedBleedOut = 0.f;
		OnRevived.Broadcast();
	}
}

void UVitalsComponent::AddPanic(float Amount)
{
	if (!IsVitalAuthority()) { return; }
	if (CVarAvNoPanic.GetValueOnGameThread() != 0) { return; } // тест: паника выключена
	// База (хаб) — безопасная зона: паника не добавляется ничем
	if (const ARunState* Run = ARunState::Get(GetWorld()))
	{
		if (Run->IsHubMode()) { return; }
	}
	Panic = FMath::Clamp(Panic + Amount, 0.f, 100.f);
}

void UVitalsComponent::RestoreStamina(float Amount)
{
	if (!IsVitalAuthority()) { return; }
	Stamina = FMath::Clamp(Stamina + Amount, 0.f, 100.f);
}

void UVitalsComponent::ReducePanic(float Amount)
{
	if (!IsVitalAuthority()) { return; }
	Panic = FMath::Clamp(Panic - Amount, 0.f, 100.f);
}

void UVitalsComponent::SetSprinting(bool bNewSprinting)
{
	// Нельзя бежать раненым или совсем без сил
	bSprinting = bNewSprinting && !bWounded && Stamina > 5.f;
}

void UVitalsComponent::RelieveBladder()
{
	if (!IsVitalAuthority()) { return; }
	Bladder = 0.f;
	// Полный визит в биотуалет = заодно привёл себя в порядок: снимаем «испачкан» и сбиваем амбре ниже порога.
	// (иначе bSoiled висел весь забег — вечный кашель/вонь/аура паники без возможности отмыться.)
	if (bSoiled)
	{
		bSoiled = false;
		Smell = FMath::Min(Smell, SmellThreshold - 5.f);
	}
}

void UVitalsComponent::DrainBladder(float Amount)
{
	if (!IsVitalAuthority()) { return; }
	Bladder = FMath::Max(0.f, Bladder - Amount);
}

void UVitalsComponent::AddBladder(float Amount)
{
	if (!IsVitalAuthority()) { return; }
	Bladder = FMath::Clamp(Bladder + Amount, 0.f, 100.f);
}

void UVitalsComponent::StartSmoking()
{
	if (!IsVitalAuthority()) { return; }
	SmokingRemaining = SmokingDuration;
}

void UVitalsComponent::StopSmoking()
{
	if (!IsVitalAuthority()) { return; }
	SmokingRemaining = 0.f; // потушил — больше не «открытый огонь» рядом с газом
}
