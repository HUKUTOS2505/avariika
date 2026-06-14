#include "UI/MenuHUD.h"

#include "Game/AvariikaOnlineSubsystem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
	const FLinearColor MenuBG(0.02f, 0.02f, 0.03f, 1.f);
	const FLinearColor Orange(1.f, 0.55f, 0.15f, 1.f);
	const FLinearColor PanelIdle(0.06f, 0.06f, 0.07f, 0.92f);
	const FLinearColor PanelHover(0.22f, 0.11f, 0.02f, 0.95f);
	const FLinearColor Grey(0.7f, 0.7f, 0.72f, 1.f);
}

UAvariikaOnlineSubsystem* AMenuHUD::GetOnline() const
{
	if (const UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			return GI->GetSubsystem<UAvariikaOnlineSubsystem>();
		}
	}
	return nullptr;
}

void AMenuHUD::DrawButton(const FString& Label, float CenterX, float Y, float Width, float Height, FName BoxName, UFont* Font)
{
	const bool bHover = (HoveredBox == BoxName);
	const float X = CenterX - Width * 0.5f;

	DrawRect(bHover ? PanelHover : PanelIdle, X, Y, Width, Height);
	// оранжевая рамка
	DrawRect(Orange, X, Y, Width, 2.f);
	DrawRect(Orange, X, Y + Height - 2.f, Width, 2.f);
	DrawRect(Orange, X, Y, 2.f, Height);
	DrawRect(Orange, X + Width - 2.f, Y, 2.f, Height);

	float TW = 0.f, TH = 0.f;
	GetTextSize(Label, TW, TH, Font, 1.2f);
	DrawText(Label, bHover ? FLinearColor::White : Orange, CenterX - TW * 0.5f, Y + (Height - TH) * 0.5f, Font, 1.2f);

	AddHitBox(FVector2D(X, Y), FVector2D(Width, Height), BoxName, true);
}

void AMenuHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas)
	{
		return;
	}

	const float SX = Canvas->SizeX;
	const float SY = Canvas->SizeY;

	UFont* Big = GEngine ? GEngine->GetLargeFont() : nullptr;
	UFont* Med = GEngine ? GEngine->GetMediumFont() : nullptr;

	// Фон
	DrawRect(MenuBG, 0.f, 0.f, SX, SY);
	// Оранжевая полоса-акцент сверху
	DrawRect(Orange, 0.f, 0.f, SX, 4.f);

	// Заголовок
	const FString Title = TEXT("АВАРИЙКА");
	float TW = 0.f, TH = 0.f;
	GetTextSize(Title, TW, TH, Big, 3.0f);
	DrawText(Title, Orange, SX * 0.5f - TW * 0.5f, SY * 0.16f, Big, 3.0f);

	const FString Sub = TEXT("аварийная бригада");
	GetTextSize(Sub, TW, TH, Med, 1.1f);
	DrawText(Sub, Grey, SX * 0.5f - TW * 0.5f, SY * 0.16f + 64.f, Med, 1.1f);

	const float CX = SX * 0.5f;
	const float BW = 380.f, BH = 58.f, Gap = 16.f;
	float Y = SY * 0.42f;

	if (Screen == EMenuScreen::Main)
	{
		DrawButton(TEXT("Создать игру"), CX, Y, BW, BH, TEXT("host"), Med);     Y += BH + Gap;
		DrawButton(TEXT("Найти игру"),   CX, Y, BW, BH, TEXT("find"), Med);     Y += BH + Gap;
		DrawButton(TEXT("Настройки"),    CX, Y, BW, BH, TEXT("settings"), Med); Y += BH + Gap;
		DrawButton(TEXT("Выход"),        CX, Y, BW, BH, TEXT("quit"), Med);
	}
	else // Browse
	{
		const FString Hdr = TEXT("Поиск игр в сети");
		GetTextSize(Hdr, TW, TH, Med, 1.3f);
		DrawText(Hdr, Orange, CX - TW * 0.5f, Y - 50.f, Med, 1.3f);

		UAvariikaOnlineSubsystem* Online = GetOnline();
		const int32 Num = Online ? Online->GetNumFoundSessions() : 0;

		// поиск считаем «идущим» ~6 c после запуска
		if (bSearching && GetWorld() && (GetWorld()->GetRealTimeSeconds() - SearchStartTime) > 6.0)
		{
			bSearching = false;
		}

		if (Num <= 0)
		{
			const FString Msg = bSearching ? TEXT("Поиск...") : TEXT("Игр не найдено");
			GetTextSize(Msg, TW, TH, Med, 1.1f);
			DrawText(Msg, Grey, CX - TW * 0.5f, Y + 8.f, Med, 1.1f);
			Y += 50.f;
		}
		else
		{
			for (int32 i = 0; i < Num; ++i)
			{
				const FString Label = FString::Printf(TEXT("Игра %d   [Подключиться]"), i + 1);
				DrawButton(Label, CX, Y, BW, BH, FName(*FString::Printf(TEXT("join_%d"), i)), Med);
				Y += BH + Gap;
			}
		}

		Y += Gap;
		DrawButton(TEXT("Обновить"), CX, Y, BW, BH, TEXT("refresh"), Med); Y += BH + Gap;
		DrawButton(TEXT("Назад"),    CX, Y, BW, BH, TEXT("back"), Med);
	}

	// Подсказка снизу
	const FString Hint = TEXT("v0.1 — кооп LAN/EOS");
	GetTextSize(Hint, TW, TH, Med, 0.9f);
	DrawText(Hint, FLinearColor(0.4f, 0.4f, 0.42f, 1.f), SX - TW - 16.f, SY - TH - 12.f, Med, 0.9f);
}

void AMenuHUD::NotifyHitBoxClick(FName BoxName)
{
	Super::NotifyHitBoxClick(BoxName);

	UAvariikaOnlineSubsystem* Online = GetOnline();

	if (BoxName == TEXT("host"))
	{
		if (Online) { Online->HostGame(); }
	}
	else if (BoxName == TEXT("find") || BoxName == TEXT("refresh"))
	{
		Screen = EMenuScreen::Browse;
		if (Online)
		{
			Online->FindGames();
			bSearching = true;
			SearchStartTime = GetWorld() ? GetWorld()->GetRealTimeSeconds() : 0.0;
		}
	}
	else if (BoxName == TEXT("settings"))
	{
		OpenSettings();
	}
	else if (BoxName == TEXT("quit"))
	{
		UKismetSystemLibrary::QuitGame(this, GetOwningPlayerController(), EQuitPreference::Quit, false);
	}
	else if (BoxName == TEXT("back"))
	{
		Screen = EMenuScreen::Main;
	}
	else if (BoxName.ToString().StartsWith(TEXT("join_")))
	{
		const int32 Index = FCString::Atoi(*BoxName.ToString().RightChop(5));
		if (Online) { Online->JoinGameByIndex(Index); }
	}
}

void AMenuHUD::NotifyHitBoxBeginCursorOver(FName BoxName)
{
	Super::NotifyHitBoxBeginCursorOver(BoxName);
	HoveredBox = BoxName;
}

void AMenuHUD::NotifyHitBoxEndCursorOver(FName BoxName)
{
	Super::NotifyHitBoxEndCursorOver(BoxName);
	if (HoveredBox == BoxName)
	{
		HoveredBox = NAME_None;
	}
}

void AMenuHUD::OpenSettings()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	UClass* WidgetClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/EasyOptionsMenu/Core/WBP_EasyOptionsMenuMain.WBP_EasyOptionsMenuMain_C"));
	if (!WidgetClass)
	{
		return;
	}

	if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass))
	{
		Widget->AddToViewport(100);
	}
}
