#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFoamPatch.generated.h"

class UStaticMeshComponent;

/**
 * Лужа пены из огнетушителя (§18 «косяки/хаос»): распыляя порошок, монтёр заливает
 * пол скользкой пеной. Кто на неё наступает — теряет сцепление и катится (логика
 * скольжения живёт в AAvaryoCharacter, патч — это маркер + визуал + время жизни).
 * Кооп-комедия: «кто залил пол — на том и проедутся всей бригадой».
 */
UCLASS()
class AVARYO_API AFoamPatch : public AActor
{
	GENERATED_BODY()

public:
	AFoamPatch();

	virtual void Tick(float DeltaSeconds) override;

	/** Плоский диск-заглушка (видимая лужа). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Foam")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Радиус скольжения, см. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Foam")
	float SlipRadius;

	/** Сколько живёт лужа, сек (потом высыхает). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Foam")
	float Lifetime;

protected:
	float ElapsedLife;
};
