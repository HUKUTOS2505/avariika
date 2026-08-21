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

public:
	/** Esc переключает внутриигровое пауза-меню (Продолжить/Настройки/Выйти в меню). */
	void TogglePauseMenu();
	/** Закрыть пауза-меню и вернуть игровой ввод (зовётся кнопкой «Продолжить» с HUD). */
	void ClosePauseMenu();
	bool IsPauseMenuOpen() const { return bPauseMenuOpen; }

	/** Открыть/закрыть экран кастомизации (одежда/волосы) — дев-команда AvCustomize. */
	void ToggleCustomize();
	UFUNCTION(Exec)
	void AvCustomize();
	bool IsCustomizeOpen() const { return bCustomizeOpen; }

protected:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(APawn* P) override;
	virtual bool InputKey(const FInputKeyEventArgs& Params) override;
	virtual void SetupInputComponent() override;

	/** Принудительно: игровой ввод, курсор скрыт (только для локального контроллера). */
	void ForceGameInput();

	void OnEscapePressed();
	void OnCustomizePressed();
	void OpenCustomizationForNewGame();
	void LookYaw(float Value);
	void LookPitch(float Value);
	/** Режим ввода под меню (курсор+UI-клики) или игру. */
	void SetMenuInputMode(bool bMenu);

	bool bPauseMenuOpen = false;
	bool bCustomizeOpen = false;
	FTimerHandle NewGameCustomizationTimer;
	int32 NewGameCustomizationAttempts = 0;
};
