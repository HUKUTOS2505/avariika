#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AvaryoHUD.generated.h"

/**
 * Простой HUD: подсказка "[E] Подобрать: ..." и список слотов инвентаря.
 * Рисуем через Canvas, без UMG — позже заменим на нормальные виджеты.
 */
UCLASS()
class AVARYO_API AAvaryoHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
