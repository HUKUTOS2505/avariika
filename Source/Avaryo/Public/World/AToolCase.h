#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AToolCase.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UBoxComponent;
class USoundBase;
class AAvaryoCharacter;

/**
 * «Ящик инструмента» в хабе. Часть петли из CONCEPT.md: заявка → СОБРАТЬ
 * ИНСТРУМЕНТ в ящик → загрузить в машину → выезд. Игрок жмёт E у ящика →
 * кит «собран» (UDispatchSubsystem::SetKitLoaded), диспетчер реагирует.
 * Пока это флавор-шаг (выезд не блокирует) — гейтить выезд можно позже.
 */
UCLASS()
class AVARYO_API AToolCase : public AActor
{
	GENERATED_BODY()

public:
	AToolCase();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ToolCase")
	TObjectPtr<UStaticMeshComponent> Case;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ToolCase")
	TObjectPtr<UTextRenderComponent> Label;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ToolCase")
	TObjectPtr<UBoxComponent> Zone;

	/** Звук защёлки/сбора ящика. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToolCase")
	TObjectPtr<USoundBase> LoadSound;

	/** Собран ли ящик (для подсказки HUD). */
	UFUNCTION(BlueprintPure, Category="ToolCase")
	bool IsLoaded() const { return bLoaded; }

	/** Игрок нажал E у ящика — собрать кит. Только сервер. */
	void UseBy(AAvaryoCharacter* Who);

protected:
	bool bLoaded = false;

	void RefreshLabel();
};
