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

	UFUNCTION(BlueprintCallable, Category="Vitals")
	void SetSprinting(bool bNewSprinting);

	/** Сходить в туалет (на будущее — туалеты на картах). */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void RelieveBladder();

	/** Начать перекур: SmokingDuration секунд паника плавно снижается. */
	UFUNCTION(BlueprintCallable, Category="Vitals")
	void StartSmoking();

	UFUNCTION(BlueprintPure, Category="Vitals")
	bool IsSmoking() const { return SmokingRemaining > 0.f; }

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

protected:
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
};
