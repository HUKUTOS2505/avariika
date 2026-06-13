#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VitalsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWounded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRevived);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSanitaryIncident);

/**
 * Шкалы игрока по концепту: здоровье, паника, выносливость, "позыв в туалет".
 * Всю динамику считает сервер, значения реплицируются.
 * При HP = 0 игрок не умирает, а становится раненым (ползает, ждёт помощи).
 */
UCLASS(ClassGroup=(Avaryo), meta=(BlueprintSpawnableComponent))
class AVARYO_API UVitalsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVitalsComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// -- API (вызывать на сервере) --

	UFUNCTION(BlueprintCallable, Category="Vitals")
	void ApplyDamage(float Amount);

	/** Лечение. Раненый с HP выше порога встаёт на ноги. */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category="Vitals")
	void AddPanic(float Amount);

	UFUNCTION(BlueprintCallable, Category="Vitals")
	void ReducePanic(float Amount);

	/** Восстановить выносливость (кофе/термос). */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void RestoreStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category="Vitals")
	void SetSprinting(bool bNewSprinting);

	/** Сходить в туалет полностью (шкала в ноль). */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void RelieveBladder();

	/** Частично облегчиться (мини-игра биотуалета). */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void DrainBladder(float Amount);

	/** Добавить к шкале туалета (диуретик: кофе/термос). Клампится в [0;100]. */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void AddBladder(float Amount);

	/** Начать перекур: SmokingDuration секунд паника плавно снижается. */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void StartSmoking();

	UFUNCTION(BlueprintPure, Category="Vitals")
	bool IsSmoking() const { return SmokingRemaining > 0.f; }

	/** Добавить «амбре» (запах от газа, порошка, дыма, инцидента). Зажато 0..100. */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void AddSmell(float Amount);

	/** Дев-режим: жёстко выставить шкалу по имени (health/panic/stamina/bladder/smell). Только сервер. */
	void DebugSetVital(FName Which, float Value);

	/** Вся динамика витала — серверная. Без овнера (юнит-тест) считаем авторитетным, иначе сверяем овнера.
	 * Защищает реплицируемые шкалы от клиентских вызовов (кооп: рассинхрон/чит). */
	bool IsVitalAuthority() const;

	// -- Геттеры --

	UFUNCTION(BlueprintPure, Category="Vitals") float GetHealth() const { return Health; }
	UFUNCTION(BlueprintPure, Category="Vitals") float GetPanic() const { return Panic; }
	UFUNCTION(BlueprintPure, Category="Vitals") float GetStamina() const { return Stamina; }
	UFUNCTION(BlueprintPure, Category="Vitals") float GetBladder() const { return Bladder; }
	UFUNCTION(BlueprintPure, Category="Vitals") bool IsWounded() const { return bWounded; }
	UFUNCTION(BlueprintPure, Category="Vitals") bool IsSprinting() const { return bSprinting; }
	UFUNCTION(BlueprintPure, Category="Vitals") bool IsPanicking() const { return Panic >= PanicThreshold; }
	UFUNCTION(BlueprintPure, Category="Vitals") bool IsIncidentSlowed() const { return IncidentSlowRemaining > 0.f; }
	UFUNCTION(BlueprintPure, Category="Vitals") bool IsSoiled() const { return bSoiled; }
	UFUNCTION(BlueprintPure, Category="Vitals") float GetSmell() const { return Smell; }
	UFUNCTION(BlueprintPure, Category="Vitals") bool IsSmelly() const { return Smell >= SmellThreshold; }

	// -- События --

	UPROPERTY(BlueprintAssignable, Category="Vitals")
	FOnWounded OnWounded;

	UPROPERTY(BlueprintAssignable, Category="Vitals")
	FOnRevived OnRevived;

	/** "Санитарный инцидент" — шкала туалета дошла до 100. */
	UPROPERTY(BlueprintAssignable, Category="Vitals")
	FOnSanitaryIncident OnSanitaryIncident;

	// -- Настройки --

	/** Паника: рост в темноте (фонарь выключен), %/сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float PanicRiseInDarkPerSecond;

	/** Паника: спад при включённом фонаре, %/сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float PanicFallInLightPerSecond;

	/** Паника: рост в одиночестве, %/сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float PanicRiseAlonePerSecond;

	/** Паника: спад рядом с тиммейтом, %/сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float PanicFallNearTeammatePerSecond;

	/** Радиус "тиммейт рядом", см. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float TeammateRadius;

	/** Паника: дополнительный рост, пока ранен, %/сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float PanicRiseWoundedPerSecond;

	/** Порог "паникует": трясущиеся руки, ускоренный туалет и т.д. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float PanicThreshold;

	/** Радиус заражения страхом: паникёр накручивает панику соседям, см. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float FearContagionRadius;

	/** Сколько паники/сек добавляет паникёр каждому соседу в радиусе. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float FearContagionPerSecond;

	/** Минимальная яркость источника, чтобы он считался «успокаивающим светом» (отсекает тусклые/тревожные лампы). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Panic")
	float CalmLightMinIntensity;

	/** Выносливость: расход при беге, %/сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Stamina")
	float StaminaDrainPerSecond;

	/** Множитель расхода, если несёшь тяжёлый предмет. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Stamina")
	float StaminaHeavyMultiplier;

	/** Выносливость: восстановление, %/сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Stamina")
	float StaminaRegenPerSecond;

	/** Туалет: базовый рост, %/сек (0.2 = ~8 минут до инцидента). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Bladder")
	float BladderRisePerSecond;

	/** Множитель роста туалетной шкалы при панике. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Bladder")
	float BladderPanicMultiplier;

	/** Длительность замедления после инцидента, сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Bladder")
	float IncidentSlowDuration;

	/** Скачок паники при инциденте. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Bladder")
	float IncidentPanicSpike;

	/** HP, выше которого раненый встаёт на ноги. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Health")
	float WoundedReviveThreshold;

	/** Длительность перекура, сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smoking")
	float SmokingDuration;

	/** Сколько паники снимает каждая секунда курения. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smoking")
	float SmokingPanicPerSecond;

	// --- Запах / амбре (§16) ---

	/** Порог, выше которого монтёр «воняет» (тиммейтов рядом подташнивает). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float SmellThreshold;

	/** Скорость выветривания запаха, ед/сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float SmellDecayPerSecond;

	/** Прирост запаха за секунду курения. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float SmellSmokingPerSecond;

	/** Прирост запаха за секунду паники (пот). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float SmellPanicPerSecond;

	/** Скачок запаха при санитарном инциденте. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float SmellIncidentJump;

	/** Сколько паники в секунду добавляет вонючий монтёр тиммейтам рядом. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float SmellTeammatePanicPerSecond;

	/** Радиус, в котором чувствуется амбре, см. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float SmellRadius;

	/** Как часто кашляешь, провонявшись (газ/химия/пот), сек. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float CoughInterval;

	/** Паника за кашель (мелочь — но выдаёт шумом). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vitals|Smell")
	float CoughPanic;

protected:
	/** Стоит ли монтёр в досягаемости достаточно яркого «успокаивающего» света (не тревожно-красного). */
	bool IsLitByNearbyLight(const AActor* OwnerChar) const;

	float CoughAccum = 0.f;     // таймер кашля
	float LightScanAccum = 0.f; // троттл сканера успокаивающего света
	bool bInLightCached = false; // последний результат сканера света
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Vitals")
	float Health;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Vitals")
	float Panic;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Vitals")
	float Stamina;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Vitals")
	float Bladder;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Vitals")
	bool bWounded;

	/** Статус "испачкан" после инцидента (пока снимается только перезапуском). */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Vitals")
	bool bSoiled;

	UPROPERTY(Replicated)
	float IncidentSlowRemaining;

	UPROPERTY(Replicated)
	bool bSprinting;

	/** Сколько секунд осталось курить. */
	UPROPERTY(Replicated)
	float SmokingRemaining;

	/** Уровень запаха 0..100. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Vitals")
	float Smell;
};
