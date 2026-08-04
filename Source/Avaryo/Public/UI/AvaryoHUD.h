#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AvaryoHUD.generated.h"

class UUserWidget;

UCLASS()
class AVARYO_API AAvaryoHUD : public AHUD
{
	GENERATED_BODY()

public:
	AAvaryoHUD();

	virtual void DrawHUD() override;

	void ToggleShop() { bShopOpen = !bShopOpen; }
	bool IsShopOpen() const { return bShopOpen; }

	void SetPauseMenu(bool bOpen) { bPauseMenuOpen = bOpen; if (!bOpen) { HoveredBox = NAME_None; } }
	bool IsPauseMenuOpen() const { return bPauseMenuOpen; }

	void SetCustomize(bool bOpen);
	bool IsCustomizeOpen() const { return bCustomizeOpen; }

	UFUNCTION()
	bool TriggerCustomizeRandomAppearance();

	/** Routes the reliable PlayerController F9 input to the development-only transient inspector. */
	bool ToggleCustomizeAppearanceInspector();

	virtual void NotifyHitBoxClick(FName BoxName) override;
	virtual void NotifyHitBoxBeginCursorOver(FName BoxName) override;
	virtual void NotifyHitBoxEndCursorOver(FName BoxName) override;

protected:
	void DrawShop();
	void DrawPauseMenu();
	void OpenSettings();

	bool bShopOpen = false;
	bool bPauseMenuOpen = false;
	FName HoveredBox = NAME_None;
	TWeakObjectPtr<UUserWidget> SettingsWidget;

	bool bCustomizeOpen = false;
	TWeakObjectPtr<UUserWidget> CustomizeWidget;

	/** Cook-visible soft reference; loaded only after startup localization registration has completed. */
	UPROPERTY()
	TSoftClassPtr<UUserWidget> CharacterCustomizationWidgetClass;

private:
	TArray<FVector> CachedExitZoneLocs;
	TArray<FVector> CachedToiletLocs;
	float MinimapCacheStamp = -1000.f;
	void RefreshMinimapCache();
};
