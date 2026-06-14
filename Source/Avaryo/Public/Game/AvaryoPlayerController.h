#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AvaryoPlayerController.generated.h"

/**
 * Игровой контроллер. Базовый класс под BP_FirstPersonPlayerController (репарент):
 * BP оставляет свою настройку Enhanced Input (обзор/движение), а C++ ГАРАНТИРУЕТ
 * чистый игровой ввод — захват мыши + скрытый курсор. Нужно, потому что вход в игру
 * через меню (ServerTravel) мог оставить режим курсора/UI, и тогда не работали
 * ни обзор мышью, ни клавиши (E «починка» и т.п.).
 */
UCLASS()
class AVARYO_API AAvaryoPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(APawn* P) override;

	/** Принудительно: игровой ввод, курсор скрыт (только для локального контроллера). */
	void ForceGameInput();
};
