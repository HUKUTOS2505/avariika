#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "MenuGameMode.generated.h"

/**
 * Map-scoped controller for the authored Main Menu shell.
 * Cursor/input ownership is finalized by UAvMainMenuInputRouter after root creation.
 */
UCLASS()
class AVARYO_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMenuPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual bool InputKey(const FInputKeyEventArgs& Params) override;
};

/**
 * Map-scoped GameMode. It does not replace global project defaults and spawns no pawn.
 */
UCLASS()
class AVARYO_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMenuGameMode();
};
