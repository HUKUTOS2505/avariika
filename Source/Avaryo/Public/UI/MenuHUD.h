#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MenuHUD.generated.h"

class UAvariikaOnlineSubsystem;
class UFont;

/**
 * Главное меню «Аварийки» — рисуется на Canvas в нашем стиле (тёмный фон,
 * оранжевый акцент, без фиолетового). Кнопки — нативные хитбоксы AHUD
 * (AddHitBox + NotifyHitBoxClick), без UMG/Blueprint. Завязано на нашу
 * кооп-подсистему: Создать/Найти/Подключиться, Настройки (Easy Options), Выход.
 */
UCLASS()
class AVARYO_API AMenuHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	virtual void NotifyHitBoxClick(FName BoxName) override;
	virtual void NotifyHitBoxBeginCursorOver(FName BoxName) override;
	virtual void NotifyHitBoxEndCursorOver(FName BoxName) override;

protected:
	enum class EMenuScreen : uint8 { Main, Browse };
	EMenuScreen Screen = EMenuScreen::Main;

	FName HoveredBox = NAME_None;
	bool bSearching = false;
	double SearchStartTime = 0.0;

	UAvariikaOnlineSubsystem* GetOnline() const;
	void OpenSettings();

	/** Рисует кнопку-панель с оранжевой рамкой + регистрирует хитбокс. */
	void DrawButton(const FString& Label, float CenterX, float Y, float Width, float Height, FName BoxName, UFont* Font);
};
