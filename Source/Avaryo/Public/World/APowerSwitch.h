#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APowerSwitch.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UBoxComponent;
class UPointLightComponent;
class USoundBase;
class AAvaryoCharacter;

/**
 * Рубильник (каскад «Прорыв воды» 2.1, шаг 5). Стоит на СУХОМ месте у щитка.
 * E переключает питание объекта. Пока питание ВКЛ — залитая зона под током (бьёт
 * без диэлектрика). Опустил рубильник (ВЫКЛ) → вода больше не бьёт → можно лезть чинить.
 * Это «порядок-наказание»: обесточь ПЕРЕД тем, как заходить в воду.
 * Свет: красный = под напряжением (опасно), зелёный = обесточено (безопасно).
 */
UCLASS()
class AVARYO_API APowerSwitch : public AActor
{
	GENERATED_BODY()

public:
	APowerSwitch();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PowerSwitch")
	TObjectPtr<UBoxComponent> Zone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PowerSwitch")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PowerSwitch")
	TObjectPtr<UTextRenderComponent> Label;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PowerSwitch")
	TObjectPtr<UPointLightComponent> StatusLight;

	/** Звук щелчка рубильника. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PowerSwitch")
	TObjectPtr<USoundBase> ClickSound;

	/** Питание включено? (true — зона под током). */
	UFUNCTION(BlueprintPure, Category="PowerSwitch")
	bool IsPowerOn() const { return bPowerOn; }

	/** Игрок нажал E — переключить питание. Только сервер. */
	void ToggleBy(AAvaryoCharacter* Who);

protected:
	/** Питание объекта. true — под напряжением (залитая зона бьёт током). */
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Power, BlueprintReadOnly, Category="PowerSwitch")
	bool bPowerOn = true;

	UFUNCTION()
	void OnRep_Power();

	/** Раздать состояние питания во все залитые зоны (под током / безопасно). Только сервер. */
	void ApplyToFloods();

	/** Обновить табличку + цвет лампы по состоянию питания. */
	void RefreshVisual();

	float LastToggleTime = 0.f; // дебаунс E (анти-спам питания/звука/ре-электрификации воды)
};
