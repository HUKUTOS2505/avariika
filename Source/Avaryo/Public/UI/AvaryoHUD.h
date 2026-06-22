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

	/** Внутриигровое пауза-меню (Esc). Управляется из AAvaryoPlayerController. */
	void SetPauseMenu(bool bOpen) { bPauseMenuOpen = bOpen; if (!bOpen) { HoveredBox = NAME_None; } }
	bool IsPauseMenuOpen() const { return bPauseMenuOpen; }

	// Клики/наведение по кнопкам пауза-меню (Canvas-хитбоксы AHUD)
	virtual void NotifyHitBoxClick(FName BoxName) override;
	virtual void NotifyHitBoxBeginCursorOver(FName BoxName) override;
	virtual void NotifyHitBoxEndCursorOver(FName BoxName) override;

protected:
	bool bShopOpen = false;

	/** Модальный экран снаряжения: уровни/цены апгрейдов + баланс (читает леджер). */
	void DrawShop();

	/** Внутриигровое пауза-меню (Продолжить/Настройки/Выйти в меню) на Canvas. */
	void DrawPauseMenu();
	/** Открыть оверлей настроек (EasyOptionsMenu) поверх паузы. */
	void OpenSettings();

	bool bPauseMenuOpen = false;
	FName HoveredBox = NAME_None;
	TWeakObjectPtr<class UUserWidget> SettingsWidget;

private:
	// Кэш статичных акторов миникарты (AExitZone/AToilet не двигаются) — обновляем ≤1 Гц,
	// а не сканируем мир TActorIterator каждый DrawHUD-кадр (CODE_AUDIT3 #9).
	TArray<FVector> CachedExitZoneLocs;
	TArray<FVector> CachedToiletLocs;
	float MinimapCacheStamp = -1000.f;
	void RefreshMinimapCache();
};
