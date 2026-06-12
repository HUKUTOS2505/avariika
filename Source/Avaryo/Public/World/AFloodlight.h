#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFloodlight.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;

/**
 * Переносной прожектор (§18 «предметы-ловушки»): монтёр ставит его из слота.
 * Светит на зону и гасит панику всем рядом (свет успокаивает) — но гудит и
 * периодически шумит, выдавая позицию будущему монстру. Удобно, но «против команды».
 */
UCLASS()
class AVARYO_API AFloodlight : public AActor
{
	GENERATED_BODY()

public:
	AFloodlight();

	virtual void Tick(float DeltaSeconds) override;

	/** Корпус-тренога (заглушка). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floodlight")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Источник света. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floodlight")
	TObjectPtr<UPointLightComponent> Light;

	/** Радиус, в котором прожектор гасит панику, см. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floodlight")
	float CalmRadius;

	/** Сколько паники в секунду снимает стоящим рядом. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floodlight")
	float CalmPerSecond;

	/** Интервал гудящего шума, сек (выдаёт позицию). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floodlight")
	float NoiseInterval;

	/** Громкость гудения. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floodlight")
	float NoiseLoudness;

protected:
	float NoiseAccum;
};
