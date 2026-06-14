#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "MenuGameMode.generated.h"

/**
 * Контроллер главного меню: показывает курсор и пускает клики в HUD-хитбоксы.
 * Пешка не нужна — меню рисуется на Canvas (AMenuHUD).
 */
UCLASS()
class AVARYO_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMenuPlayerController();

protected:
	virtual void BeginPlay() override;
};

/**
 * Гейммод стартового экрана. Своя пешка не спавнится — это просто меню.
 * Ставится в WorldSettings карты L_MainMenu. На «Создать игру» подсистема
 * сессий уезжает на Lvl_FirstPerson, где подхватывается BP_AvaryoGameMode.
 */
UCLASS()
class AVARYO_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMenuGameMode();
};
