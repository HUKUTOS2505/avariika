#include "Game/MenuGameMode.h"

#include "UI/MenuHUD.h"

AMenuPlayerController::AMenuPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;       // нужно, чтобы AHUD::NotifyHitBoxClick срабатывал
	bEnableMouseOverEvents = true;   // подсветка кнопок при наведении
}

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Курсор + игровой ввод (чтобы клики дошли до хитбоксов HUD, а UMG-настройки тоже работали)
	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

AMenuGameMode::AMenuGameMode()
{
	HUDClass = AMenuHUD::StaticClass();
	PlayerControllerClass = AMenuPlayerController::StaticClass();
	DefaultPawnClass = nullptr; // в меню пешка не нужна
}
