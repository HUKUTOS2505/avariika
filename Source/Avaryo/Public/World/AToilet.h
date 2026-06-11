#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AToilet.generated.h"

class AAvaryoCharacter;
class UStaticMeshComponent;
class UTextRenderComponent;

/**
 * Биотуалет: держать E три секунды — «процесс» идёт, движение его срывает.
 * По завершении шкала туалета обнуляется, визит уходит в статистику «Акта».
 * Процесс слышно (задел под монстра-слухача).
 */
UCLASS()
class AVARYO_API AToilet : public AActor
{
	GENERATED_BODY()

public:
	AToilet();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Кабинка (куб-заглушка; меш можно заменить в Blueprint). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Toilet")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Табличка над кабинкой. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Toilet")
	TObjectPtr<UTextRenderComponent> Label;

	/** Сколько секунд держать E. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Toilet")
	float UseDuration;

	/** Есть ли игроку смысл пользоваться (шкала туалета не пустая, не ранен, свободно). */
	UFUNCTION(BlueprintPure, Category="Toilet")
	bool CanUseBy(const AAvaryoCharacter* Who) const;

	/** Начать процесс (зажал E). Только сервер. */
	bool BeginUseBy(AAvaryoCharacter* Who);

	/** Прервать процесс (отпустил E / ушёл). Прогресс сбрасывается. Только сервер. */
	void EndUseBy(AAvaryoCharacter* Who);

	UFUNCTION(BlueprintPure, Category="Toilet") bool IsOccupied() const { return Occupant != nullptr; }
	UFUNCTION(BlueprintPure, Category="Toilet") AAvaryoCharacter* GetOccupant() const { return Occupant; }
	UFUNCTION(BlueprintPure, Category="Toilet") float GetUseProgress() const { return UseProgress; }

protected:
	/** Кто сейчас «в процессе» (держит E). */
	UPROPERTY(Replicated)
	TObjectPtr<AAvaryoCharacter> Occupant;

	/** Прогресс 0..1, реплицируется для HUD. */
	UPROPERTY(Replicated)
	float UseProgress;
};
