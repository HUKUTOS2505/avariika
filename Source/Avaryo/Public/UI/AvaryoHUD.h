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

	/** Открыть/закрыть экран магазина (дев-команда AvShop). Локально на клиенте. */
	void ToggleShop() { bShopOpen = !bShopOpen; }
	bool IsShopOpen() const { return bShopOpen; }

protected:
	bool bShopOpen = false;

	/** Модальный экран снаряжения: уровни/цены апгрейдов + баланс (читает леджер). */
	void DrawShop();

private:
	// Кэш статичных акторов миникарты (AExitZone/AToilet не двигаются) — обновляем ≤1 Гц,
	// а не сканируем мир TActorIterator каждый DrawHUD-кадр (CODE_AUDIT3 #9).
	TArray<FVector> CachedExitZoneLocs;
	TArray<FVector> CachedToiletLocs;
	float MinimapCacheStamp = -1000.f;
	void RefreshMinimapCache();
};
