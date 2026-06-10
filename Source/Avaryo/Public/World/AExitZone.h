#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AExitZone.generated.h"

class UBoxComponent;
class UTextRenderComponent;

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

	virtual void Tick(float DeltaSeconds) override;

	/** Объём зоны. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ExitZone")
	TObjectPtr<UBoxComponent> Zone;

	/** Табличка "ГАЗель — выход" над зоной. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ExitZone")
	TObjectPtr<UTextRenderComponent> Label;

	/** Вся ли команда сейчас в зоне (для подсказки на HUD). */
	UFUNCTION(BlueprintPure, Category="ExitZone")
	bool IsTeamInside() const { return bTeamInside; }

protected:
	/** Кэш последней проверки (сервер считает, не реплицируется — HUD сервера хватает). */
	bool bTeamInside;
};
