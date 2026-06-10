#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AToilet.generated.h"

class AAvaryoCharacter;
class UStaticMeshComponent;
class UTextRenderComponent;

/**
 * Биотуалет: E — облегчиться, шкала туалета обнуляется, «санитарный инцидент»
 * отменяется честным путём. Процесс слышно (задел под монстра-слухача).
 */
UCLASS()
class AVARYO_API AToilet : public AActor
{
	GENERATED_BODY()

public:
	AToilet();

	virtual void Tick(float DeltaSeconds) override;

	/** Кабинка (куб-заглушка; меш можно заменить в Blueprint). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Toilet")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Табличка над кабинкой. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Toilet")
	TObjectPtr<UTextRenderComponent> Label;

	/** Есть ли игроку смысл пользоваться (шкала туалета не пустая, не ранен). */
	UFUNCTION(BlueprintPure, Category="Toilet")
	bool CanUseBy(const AAvaryoCharacter* Who) const;

	/** Облегчиться. Только сервер. */
	void UseBy(AAvaryoCharacter* Who);
};
