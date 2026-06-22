#include "Game/AvaryoPlayerController.h"

#include "UI/AvaryoHUD.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"

void AAvaryoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ForceGameInput();
}

void AAvaryoPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	// После ServerTravel/повторного владения пешкой ещё раз закрепляем игровой ввод
	ForceGameInput();
}

void AAvaryoPlayerController::ForceGameInput()
{
	if (!IsLocalController())
	{
		return;
	}
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void AAvaryoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Esc открывает/закрывает внутриигровое меню. BindKey работает рядом с Enhanced Input (Esc не занят экшеном).
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AAvaryoPlayerController::OnEscapePressed);
	}
}

void AAvaryoPlayerController::OnEscapePressed()
{
	TogglePauseMenu();
}

void AAvaryoPlayerController::TogglePauseMenu()
{
	if (!IsLocalController())
	{
		return;
	}
	bPauseMenuOpen = !bPauseMenuOpen;
	if (AAvaryoHUD* H = Cast<AAvaryoHUD>(GetHUD()))
	{
		H->SetPauseMenu(bPauseMenuOpen);
	}
	SetMenuInputMode(bPauseMenuOpen);
	// Одиночная игра — ставим реальную паузу; в кооп сервер не остановить, меню = оверлей.
	if (GetNetMode() == NM_Standalone)
	{
		SetPause(bPauseMenuOpen);
	}
}

void AAvaryoPlayerController::ClosePauseMenu()
{
	if (!IsLocalController() || !bPauseMenuOpen)
	{
		return;
	}
	bPauseMenuOpen = false;
	if (AAvaryoHUD* H = Cast<AAvaryoHUD>(GetHUD()))
	{
		H->SetPauseMenu(false);
	}
	SetMenuInputMode(false);
	if (GetNetMode() == NM_Standalone)
	{
		SetPause(false);
	}
}

void AAvaryoPlayerController::SetMenuInputMode(bool bMenu)
{
	if (!IsLocalController())
	{
		return;
	}
	if (bMenu)
	{
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		SetInputMode(Mode);
	}
	else
	{
		ForceGameInput();
	}
}
