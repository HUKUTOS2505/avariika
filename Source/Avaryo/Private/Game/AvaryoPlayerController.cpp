#include "Game/AvaryoPlayerController.h"

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
