#include "Game/MenuGameMode.h"

#include "InputKeyEventArgs.h"
#include "UI/MenuHUD.h"

AMenuPlayerController::AMenuPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Initial safe mode; the project-owned router takes authoritative ownership once the root exists.
	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

bool AMenuPlayerController::InputKey(const FInputKeyEventArgs& Params)
{
	if (AMenuHUD* MenuHUD = Cast<AMenuHUD>(GetHUD()))
	{
		if (MenuHUD->HandleAuthenticMSPInput(Params.Key, Params.Event, Params.AmountDepressed))
		{
			return true;
		}
	}
	return Super::InputKey(Params);
}

AMenuGameMode::AMenuGameMode()
{
	HUDClass = AMenuHUD::StaticClass();
	PlayerControllerClass = AMenuPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
}
