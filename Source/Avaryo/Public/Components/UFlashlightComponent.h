#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UFlashlightComponent.generated.h"

class ULightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlashlightToggled, bool, bNewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBatteryLow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBatteryEmpty);

/**
 * Налобный фонарик с батареей, репликацией и мерцанием при низком заряде.
 * Свет здесь не создаётся: назначьте AttachedLight в Blueprint,
 * либо компонент сам найдёт первый ULightComponent у владельца в BeginPlay.
 */
UCLASS(ClassGroup=(Avaryo), meta=(BlueprintSpawnableComponent))
class AVARYO_API UFlashlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlashlightComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Включить фонарь (не сработает при пустой батарее). Вызывать на сервере. */
	UFUNCTION(BlueprintCallable, Category="Flashlight")
	void TurnOn();

	UFUNCTION(BlueprintCallable, Category="Flashlight")
	void TurnOff();

	UFUNCTION(BlueprintCallable, Category="Flashlight")
	void Toggle();

	/** Восстановить заряд на Amount процентов (итог зажат в 0-100). */
	UFUNCTION(BlueprintCallable, Category="Flashlight")
	void Recharge(float Amount);

	/** Пометить фонарь дешёвым: моргает даже при полном заряде. Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Flashlight")
	void SetCheapUnit(bool bNewCheap);

	UFUNCTION(BlueprintPure, Category="Flashlight")
	bool IsCheapUnit() const { return bCheapUnit; }

	UFUNCTION(BlueprintPure, Category="Flashlight")
	bool IsOn() const { return bIsOn; }

	UFUNCTION(BlueprintPure, Category="Flashlight")
	float GetBatteryLevel() const { return BatteryLevel; }

	/** Срабатывает при включении/выключении (на сервере сразу, на клиентах через OnRep). */
	UPROPERTY(BlueprintAssignable, Category="Flashlight")
	FOnFlashlightToggled OnFlashlightToggled;

	/** Батарея опустилась ниже LowBatteryThreshold (только на сервере). */
	UPROPERTY(BlueprintAssignable, Category="Flashlight")
	FOnBatteryLow OnBatteryLow;

	/** Батарея полностью села (только на сервере). */
	UPROPERTY(BlueprintAssignable, Category="Flashlight")
	FOnBatteryEmpty OnBatteryEmpty;

	/** Компонент света. Назначается в Blueprint или ищется у владельца в BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flashlight")
	TObjectPtr<ULightComponent> AttachedLight;

	/** Скорость разряда, % в секунду. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flashlight", meta=(ClampMin="0.0"))
	float DrainPerSecond;

	/** Порог низкого заряда (начало мерцания), %. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flashlight", meta=(ClampMin="0.0", ClampMax="100.0"))
	float LowBatteryThreshold;

	/** Вероятность случайного отключения в секунду при низком заряде. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flashlight", meta=(ClampMin="0.0", ClampMax="1.0"))
	float BlackoutChancePerSecond;

	/** Вероятность короткого моргания в секунду у дешёвого фонаря при нормальном заряде. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flashlight", meta=(ClampMin="0.0", ClampMax="1.0"))
	float CheapGlitchChancePerSecond;

protected:
	/** Дешёвый комплект: моргает даже при полном заряде (косяк оборудования, §18). Реплицируется для клиентского мерцания. */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Flashlight")
	bool bCheapUnit;

	/** Текущее состояние, реплицируется. */
	UPROPERTY(ReplicatedUsing=OnRep_IsOn, BlueprintReadOnly, Category="Flashlight")
	bool bIsOn;

	/** Заряд 0-100, реплицируется. Разряд считает только сервер. */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Flashlight", meta=(ClampMin="0.0", ClampMax="100.0"))
	float BatteryLevel;

	UFUNCTION()
	void OnRep_IsOn();

	/** Применить bIsOn к компоненту света (видимость + интенсивность). */
	void ApplyLightState();

	/** Мерцание и случайные отключения при низком заряде. Чистая косметика, выполняется на всех машинах. */
	void UpdateFlicker(float DeltaTime);

private:
	float DefaultIntensity;       // интенсивность света по умолчанию (запоминается в BeginPlay)
	float BlackoutTimeRemaining;  // сколько ещё длится случайное отключение
	bool bLowBatteryNotified;     // чтобы OnBatteryLow сработал один раз на разряд
};
