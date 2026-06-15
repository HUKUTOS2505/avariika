#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AExitZone.generated.h"

class UBoxComponent;
class UTextRenderComponent;
class UPointLightComponent;
class USoundBase;

/**
 * Зона выхода у Газели. Когда все задачи починены и ВСЯ команда (включая
 * раненых — тащите их сюда) стоит в зоне, RunState объявляет победу.
 */
UCLASS()
class AVARYO_API AExitZone : public AActor
{
	GENERATED_BODY()

public:
	AExitZone();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Объём зоны. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ExitZone")
	TObjectPtr<UBoxComponent> Zone;

	/** Табличка "ГАЗель — выход" над зоной. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ExitZone")
	TObjectPtr<UTextRenderComponent> Label;

	/** Маяк готовности: тусклый, пока не всё починено; загорается зелёным — «к ГАЗели, на базу». */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ExitZone")
	TObjectPtr<UPointLightComponent> Beacon;

	/** Звук «готово к выезду» в момент, когда всё починено и маяк загорелся. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ExitZone")
	TObjectPtr<USoundBase> ReadySound;

	/** Вся ли команда сейчас в зоне (для подсказки на HUD). */
	UFUNCTION(BlueprintPure, Category="ExitZone")
	bool IsTeamInside() const { return bTeamInside; }

	/** Всё ли починено — маяк горит, можно возвращаться на базу (для HUD). */
	UFUNCTION(BlueprintPure, Category="ExitZone")
	bool IsReadyToLeave() const { return bReadyToLeave; }

protected:
	/** Вся ли команда в зоне. Сервер считает в Tick, реплицируется → клиентский HUD тоже видит. */
	UPROPERTY(Replicated)
	bool bTeamInside;

	/** Всё починено — зона активна, маяк зелёный. Реплицируется для HUD/визуала у клиента. */
	UPROPERTY(ReplicatedUsing=OnRep_Ready)
	bool bReadyToLeave;

	/** Обновить маяк/табличку по готовности (вызывается на сервере и у клиента через OnRep). */
	void RefreshBeacon();

	UFUNCTION()
	void OnRep_Ready();
};
