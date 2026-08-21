#include "UI/AvaryoHUD.h"

#include "AvariikaLoc.h"
#include "AvaryoCharacter.h"
#include "Components/UFlashlightComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/GameInstance.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "Game/AvaryoPlayerController.h"
#include "Game/AvariikaOnlineSubsystem.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "UI/AvCharacterCustomizationRootWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Items/APickupItem.h"
#include "World/ACallBoard.h"
#include "World/AToolCase.h"
#include "World/AExitZone.h"
#include "World/ARepairable.h"
#include "World/AToilet.h"

namespace AvaryoHUDStyle
{
	const FLinearColor PanelBG(0.03f, 0.035f, 0.06f, 0.94f);   // С‚С‘РјРЅР°СЏ РїР°РЅРµР»СЊ
	const FLinearColor CellBG(0.07f, 0.08f, 0.12f, 0.95f);     // СЏС‡РµР№РєР° СЃР»РѕС‚Р°
	const FLinearColor Accent(0.95f, 0.45f, 0.04f, 1.f);       // Р°РІР°СЂРёР№РЅС‹Р№ РѕСЂР°РЅР¶РµРІС‹Р№
	const FLinearColor AccentDim(0.6f, 0.27f, 0.02f, 1.f);     // Р°РєС†РµРЅС‚РЅР°СЏ РєСЂРѕРјРєР°
	const FLinearColor BoxBG(0.04f, 0.05f, 0.09f, 0.88f);      // РїР»Р°С€РєРё РїРѕ С†РµРЅС‚СЂСѓ
	const FLinearColor TextMain(0.96f, 0.96f, 0.98f, 1.f);
	const FLinearColor TextDim(0.62f, 0.64f, 0.72f, 1.f);
	const FLinearColor BarFill(0.2f, 0.55f, 0.95f, 1.f);       // СЃРёРЅРёР№ РїСЂРѕРіСЂРµСЃСЃ РїСЂРёРјРµРЅРµРЅРёСЏ
	const FLinearColor BarBG(0.f, 0.f, 0.f, 0.55f);
}

AAvaryoHUD::AAvaryoHUD()
{
	CharacterCustomizationWidgetClass = TSoftClassPtr<UUserWidget>(
		FSoftObjectPath(TEXT(
			"/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2."
			"WBP_CharacterCustomizationRoot_V2_C")));
}

void AAvaryoHUD::SetCustomize(bool bOpen)
{
	UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeInput] HUD SetCustomize(%s), existing widget=%s"),
		bOpen ? TEXT("open") : TEXT("closed"),
		CustomizeWidget.IsValid() ? TEXT("valid") : TEXT("none"));
	bCustomizeOpen = bOpen;
	HoveredBox = NAME_None;

	if (!bOpen)
	{
		if (CustomizeWidget.IsValid())
		{
			CustomizeWidget->RemoveFromParent();
			CustomizeWidget.Reset();
		}
		return;
	}

	if (CustomizeWidget.IsValid())
	{
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	UClass* WidgetClass = CharacterCustomizationWidgetClass.LoadSynchronous();
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvCustomizeInput] Character customization widget class not found"));
		return;
	}

	if (UAvCharacterCustomizationRootWidget* Widget = CreateWidget<UAvCharacterCustomizationRootWidget>(PC, WidgetClass))
	{
		CustomizeWidget = Widget;
		Widget->SetVisibility(ESlateVisibility::Visible);
		Widget->AddToViewport(100);
		Widget->RefreshFullscreenLayout();
		UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeInput] Customize widget created and added to screen, class=%s"),
			*GetNameSafe(WidgetClass));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AvCustomizeInput] CreateWidget failed, class=%s"), *GetNameSafe(WidgetClass));
	}
}

bool AAvaryoHUD::TriggerCustomizeRandomAppearance()
{
	if (UAvCharacterCustomizationRootWidget* Widget = Cast<UAvCharacterCustomizationRootWidget>(CustomizeWidget.Get()))
	{
		Widget->TriggerRandomAppearance();
		return true;
	}

	return false;
}

bool AAvaryoHUD::ToggleCustomizeAppearanceInspector()
{
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (UAvCharacterCustomizationRootWidget* Widget = Cast<UAvCharacterCustomizationRootWidget>(CustomizeWidget.Get()))
	{
		Widget->ToggleAppearanceInspector();
		return true;
	}
#endif
	return false;
}

void AAvaryoHUD::DrawPauseMenu()
{
	if (!Canvas) { return; }
	UFont* Font = GEngine ? GEngine->GetMediumFont() : nullptr;
	UFont* Big = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font) { Font = Big; }
	if (!Font) { return; }

	const float SX = Canvas->SizeX;
	const float SY = Canvas->SizeY;
	const FLinearColor Orange(1.f, 0.55f, 0.15f, 1.f);
	const FLinearColor PanelIdle(0.06f, 0.06f, 0.07f, 0.92f);
	const FLinearColor PanelHover(0.22f, 0.11f, 0.02f, 0.95f);

	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.7f), 0.f, 0.f, SX, SY); // Р·Р°С‚РµРјРЅРµРЅРёРµ СЃС†РµРЅС‹
	DrawRect(Orange, 0.f, 0.f, SX, 4.f);                            // РѕСЂР°РЅР¶РµРІР°СЏ РєСЂРѕРјРєР°

	const FString Title = FAvLoc::S(TEXT("ui_001"));
	float TW = 0.f, TH = 0.f;
	UFont* TitleFont = Big ? Big : Font;
	GetTextSize(Title, TW, TH, TitleFont, 2.4f);
	DrawText(Title, Orange, SX * 0.5f - TW * 0.5f, SY * 0.22f, TitleFont, 2.4f);

	const float CX = SX * 0.5f, BW = 380.f, BH = 56.f, Gap = 16.f;
	float Y = SY * 0.42f;
	auto Btn = [&](const FString& Label, FName Box)
	{
		const bool bHover = (HoveredBox == Box);
		const float X = CX - BW * 0.5f;
		DrawRect(bHover ? PanelHover : PanelIdle, X, Y, BW, BH);
		DrawRect(Orange, X, Y, BW, 2.f);
		DrawRect(Orange, X, Y + BH - 2.f, BW, 2.f);
		DrawRect(Orange, X, Y, 2.f, BH);
		DrawRect(Orange, X + BW - 2.f, Y, 2.f, BH);
		float w = 0.f, h = 0.f;
		GetTextSize(Label, w, h, Font, 1.2f);
		DrawText(Label, bHover ? FLinearColor::White : Orange, CX - w * 0.5f, Y + (BH - h) * 0.5f, Font, 1.2f);
		AddHitBox(FVector2D(X, Y), FVector2D(BW, BH), Box, true);
		Y += BH + Gap;
	};
	Btn(FAvLoc::S(TEXT("ui_002")),        TEXT("pause_resume"));
	Btn(FAvLoc::S(TEXT("ui_003")),      TEXT("pause_settings"));
	Btn(FAvLoc::S(TEXT("ui_004")), TEXT("pause_leave"));
}

void AAvaryoHUD::NotifyHitBoxClick(FName BoxName)
{
	Super::NotifyHitBoxClick(BoxName);

	if (BoxName == TEXT("pause_resume"))
	{
		if (AAvaryoPlayerController* PC = Cast<AAvaryoPlayerController>(GetOwningPlayerController()))
		{
			PC->ClosePauseMenu();
		}
	}
	else if (BoxName == TEXT("pause_settings"))
	{
		OpenSettings();
	}
	else if (BoxName == TEXT("pause_leave"))
	{
		if (UWorld* W = GetWorld())
		{
			if (UGameInstance* GI = W->GetGameInstance())
			{
				if (UAvariikaOnlineSubsystem* O = GI->GetSubsystem<UAvariikaOnlineSubsystem>())
				{
					O->LeaveGame(); // Р·Р°РєСЂС‹С‚СЊ СЃРµСЃСЃРёСЋ
				}
			}
		}
		if (APlayerController* PC = GetOwningPlayerController())
		{
			PC->ClientTravel(TEXT("/Game/Avariika/Maps/MainMenu/L_MainMenu"), ETravelType::TRAVEL_Absolute);
		}
	}
}

void AAvaryoHUD::NotifyHitBoxBeginCursorOver(FName BoxName)
{
	Super::NotifyHitBoxBeginCursorOver(BoxName);
	HoveredBox = BoxName;
}

void AAvaryoHUD::NotifyHitBoxEndCursorOver(FName BoxName)
{
	Super::NotifyHitBoxEndCursorOver(BoxName);
	if (HoveredBox == BoxName)
	{
		HoveredBox = NAME_None;
	}
}

void AAvaryoHUD::OpenSettings()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}
	if (SettingsWidget.IsValid() && SettingsWidget->IsInViewport())
	{
		return; // СѓР¶Рµ РѕС‚РєСЂС‹С‚Рѕ вЂ” Р±РµР· РґСѓР±Р»РµР№ РІ РІСЊСЋРїРѕСЂС‚Рµ
	}
	UClass* WidgetClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/EasyOptionsMenu/Core/WBP_EasyOptionsMenuMain.WBP_EasyOptionsMenuMain_C"));
	if (!WidgetClass)
	{
		return;
	}
	if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass))
	{
		Widget->AddToViewport(120);
		SettingsWidget = Widget;
	}
}

void AAvaryoHUD::RefreshMinimapCache()
{
	UWorld* W = GetWorld();
	if (!W)
	{
		return;
	}
	const float Now = W->GetTimeSeconds();
	if (Now - MinimapCacheStamp < 1.0f)
	{
		return; // СЃС‚Р°С‚РёС‡РЅС‹Рµ Р°РєС‚РѕСЂС‹ вЂ” РѕР±РЅРѕРІР»СЏРµРј РЅРµ С‡Р°С‰Рµ 1 Р“С† РІРјРµСЃС‚Рѕ TActorIterator РєР°Р¶РґС‹Р№ РєР°РґСЂ (CODE_AUDIT3 #9)
	}
	MinimapCacheStamp = Now;
	CachedExitZoneLocs.Reset();
	for (TActorIterator<AExitZone> It(W); It; ++It)
	{
		CachedExitZoneLocs.Add(It->GetActorLocation());
	}
	CachedToiletLocs.Reset();
	for (TActorIterator<AToilet> It(W); It; ++It)
	{
		CachedToiletLocs.Add(It->GetActorLocation());
	}
}

void AAvaryoHUD::DrawShop()
{
	using namespace AvaryoHUDStyle;
	if (!Canvas)
	{
		return;
	}
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font)
	{
		return;
	}

	// Р§РёС‚Р°РµРј РёР· ARunState (СЂРµРїР»РёС†РёСЂСѓРµС‚СЃСЏ), Р° РЅРµ РёР· host-only Р»РµРґР¶РµСЂР° вЂ” РёРЅР°С‡Рµ Сѓ РєР»РёРµРЅС‚Р° Р±Р°Р»Р°РЅСЃ/СѓСЂРѕРІРЅРё 0
	const ARunState* Run = ARunState::Get(GetWorld());
	const int32 Balance = Run ? Run->GetCompanyBalanceLive() : 0;

	const float SizeX = Canvas->SizeX;
	const float SizeY = Canvas->SizeY;
	const float W = FMath::Min(660.f, SizeX - 80.f);
	const float H = 360.f;
	const float PX = (SizeX - W) * 0.5f;
	float TY = FMath::Max(40.f, (SizeY - H) * 0.5f);

	DrawRect(PanelBG, PX, TY, W, H);
	DrawRect(Accent, PX, TY, W, 4.f);
	TY += 16.f;

	DrawText(FAvLoc::S(TEXT("ui_005")), Accent, PX + 22.f, TY, Font, 1.3f);
	TY += 36.f;
	DrawText(FString::Printf(TEXT("%s: %d в‚Ѕ"), *FAvLoc::S(TEXT("ui_006")), Balance), TextMain, PX + 22.f, TY, Font, 1.0f);
	TY += 32.f;

	struct FShopRow { const TCHAR* Cat; const TCHAR* Name; const TCHAR* NameEn; int32 MaxLevel; };
	static const FShopRow Rows[] = {
		{ TEXT("Welder"),       TEXT("РЎРІР°СЂРѕС‡РЅРёРє"),    TEXT("Welder"),       3 },
		{ TEXT("Tester"),       TEXT("РўРµСЃС‚РµСЂ"),       TEXT("Tester"),       3 },
		{ TEXT("Flashlight"),   TEXT("Р¤РѕРЅР°СЂСЊ"),       TEXT("Flashlight"),   4 },
		{ TEXT("Extinguisher"), TEXT("РћРіРЅРµС‚СѓС€РёС‚РµР»СЊ"), TEXT("Extinguisher"), 3 },
		{ TEXT("Radio"),        TEXT("Р Р°С†РёСЏ"),        TEXT("Radio"),        3 },
	};
	for (const FShopRow& R : Rows)
	{
		const int32 Lvl = Run ? Run->GetEquipmentLevelRep(FName(R.Cat)) : 1;
		const TCHAR* RName = FAvLoc::IsEnglish() ? R.NameEn : R.Name;
		FString Line;
		FLinearColor Col;
		if (Lvl >= R.MaxLevel)
		{
			Line = FString::Printf(TEXT("%s вЂ” %s %d  (%s)"), RName, *FAvLoc::S(TEXT("ui_007")), Lvl, *FAvLoc::S(TEXT("ui_008")));
			Col = TextDim;
		}
		else
		{
			const int32 Price = 3000 * Lvl;
			Line = FString::Printf(TEXT("%s вЂ” %s %d в†’ %d:  %d в‚Ѕ   [AvUpgrade %s]"), RName, *FAvLoc::S(TEXT("ui_007")), Lvl, Lvl + 1, Price, R.Cat);
			Col = (Balance >= Price) ? TextMain : FLinearColor(0.85f, 0.42f, 0.3f);
		}
		DrawText(Line, Col, PX + 26.f, TY, Font, 0.95f);
		TY += 28.f;
	}
	TY += 12.f;
	DrawText(FAvLoc::S(TEXT("ui_009")), TextDim, PX + 22.f, TY, Font, 0.85f);
}

void AAvaryoHUD::DrawHUD()
{
	Super::DrawHUD();

	// РљСЌС€ РЅР° РєР°РґСЂ: ARunState::Get вЂ” TActorIterator-СЃРєР°РЅ РјРёСЂР°; СЂР°РЅСЊС€Рµ РґС‘СЂРіР°Р»Рё 4Г— Р·Р° РѕРґРёРЅ DrawHUD.
	ARunState* Run = ARunState::Get(GetWorld());

	using namespace AvaryoHUDStyle;

	AAvaryoCharacter* Character = Cast<AAvaryoCharacter>(GetOwningPawn());
	if (!Character || !Canvas)
	{
		return;
	}

	if (bCustomizeOpen)
	{
		return;
	}

	// РњРѕРґР°Р»СЊРЅС‹Р№ СЌРєСЂР°РЅ РјР°РіР°Р·РёРЅР° (AvShop) вЂ” СЂРёСЃСѓРµРј РµРіРѕ Рё Р±РѕР»СЊС€Рµ РЅРёС‡РµРіРѕ
	if (bShopOpen)
	{
		DrawShop();
		return;
	}

	// Р’РЅСѓС‚СЂРёРёРіСЂРѕРІРѕРµ РїР°СѓР·Р°-РјРµРЅСЋ (Esc) вЂ” РѕРІРµСЂР»РµР№ РїРѕРІРµСЂС… РёРіСЂС‹, РѕСЃС‚Р°Р»СЊРЅРѕР№ HUD РЅРµ СЂРёСЃСѓРµРј
	if (bPauseMenuOpen)
	{
		DrawPauseMenu();
		return;
	}

	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font)
	{
		return;
	}

	UVitalsComponent* Vitals = Character->VitalsComponent;
	const float SizeX = Canvas->SizeX;
	const float SizeY = Canvas->SizeY;

	// ---------- Р’РёРЅСЊРµС‚РєР° РїР°РЅРёРєРё (С…РѕСЂСЂРѕСЂ-Р°С‚РјРѕСЃС„РµСЂР°, РїСѓР»СЊСЃРёСЂСѓРµС‚ РєР°Рє СЃРµСЂРґС†РµР±РёРµРЅРёРµ) ----------
	if (Vitals && !Character->IsMonitorOpen())
	{
		const float P = Vitals->GetPanic();
		if (P > 45.f)
		{
			const float Intensity = FMath::GetMappedRangeValueClamped(FVector2D(45.f, 100.f), FVector2D(0.f, 1.f), P);
			const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
			const float Beat = 0.6f + 0.4f * FMath::Sin(Time * (4.f + 6.f * Intensity)); // С‡РµРј РїР°РЅРёС‡РЅРµРµ вЂ” С‚РµРј С‡Р°С‰Рµ
			const float A = 0.5f * Intensity * Beat;
			const FLinearColor Edge(0.35f, 0.f, 0.f, A); // С‚С‘РјРЅРѕ-РєСЂР°СЃРЅС‹Р№ (Р±РµР· С„РёРѕР»РµС‚РѕРІРѕРіРѕ)
			const float BandX = SizeX * 0.16f;
			const float BandY = SizeY * 0.16f;
			DrawRect(Edge, 0.f, 0.f, SizeX, BandY);           // РІРµСЂС…
			DrawRect(Edge, 0.f, SizeY - BandY, SizeX, BandY); // РЅРёР·
			DrawRect(Edge, 0.f, 0.f, BandX, SizeY);           // Р»РµРІРѕ
			DrawRect(Edge, SizeX - BandX, 0.f, BandX, SizeY); // РїСЂР°РІРѕ
		}
	}

	// ---------- Р—РґРѕСЂРѕРІСЊРµ РґРёРµРіРµС‚РёС‡РµСЃРєРё: РІСЃРїС‹С€РєР° РЅР° СѓСЂРѕРЅ + СЃРµСЂРґС†РµР±РёРµРЅРёРµ-РїСѓР»СЊСЃ РЅР° РЅРёР·РєРѕРј HP ----------
	{
		const float DF = Character->GetDamageFlash01();
		if (DF > 0.f)
		{
			DrawRect(FLinearColor(0.65f, 0.f, 0.f, 0.55f * DF), 0.f, 0.f, SizeX, SizeY); // РІСЃРїС‹С€РєР° Р±РѕР»Рё РЅР° Р»СЋР±РѕР№ СѓСЂРѕРЅ
		}
		if (Vitals && !Vitals->IsWounded() && !Character->IsMonitorOpen())
		{
			const float HP = Vitals->GetHealth();
			const float Thr = 35.f; // СЃРѕРІРїР°РґР°РµС‚ СЃ HeartbeatHealthThreshold
			if (HP < Thr)
			{
				const float Sev = FMath::Clamp(1.f - HP / Thr, 0.f, 1.f);
				const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
				const float Beat = FMath::Pow(FMath::Abs(FMath::Sin(Time * (3.0f + 4.0f * Sev))), 4.f); // СЂРµР·РєРёР№ С‚РѕР»С‡РѕРє СЃРµСЂРґС†Р°
				const float A = 0.5f * Sev * Beat;
				const FLinearColor Edge(0.7f, 0.f, 0.f, A);
				const float BandX = SizeX * 0.18f;
				const float BandY = SizeY * 0.18f;
				DrawRect(Edge, 0.f, 0.f, SizeX, BandY);
				DrawRect(Edge, 0.f, SizeY - BandY, SizeX, BandY);
				DrawRect(Edge, 0.f, 0.f, BandX, SizeY);
				DrawRect(Edge, SizeX - BandX, 0.f, BandX, SizeY);
			}
		}
	}

	// ---------- РђРјР±СЂРµ: РїСЂРѕРІРѕРЅСЏР» (В§16) в†’ Р±РѕР»РµР·РЅРµРЅРЅРѕ-Р·РµР»С‘РЅС‹Р№ С‚РёРЅС‚ СЌРєСЂР°РЅР° (Р·РІСѓРє РјСѓС… вЂ” РІ СЃР°СѓРЅРґ-РїСЂРѕС…РѕРґРµ) ----------
	if (Vitals && Vitals->IsSmelly() && !Character->IsMonitorOpen())
	{
		const float Sm = FMath::Clamp((Vitals->GetSmell() - 50.f) / 50.f, 0.f, 1.f); // 50в†’100 = 0в†’1
		const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		const float Wobble = 0.85f + 0.15f * FMath::Sin(Time * 2.3f); // Р»С‘РіРєРѕРµ В«РґСѓСЂРЅРѕС‚РЅРѕРµВ» РєРѕР»С‹С…Р°РЅРёРµ
		DrawRect(FLinearColor(0.15f, 0.35f, 0.1f, (0.10f + 0.15f * Sm) * Wobble), 0.f, 0.f, SizeX, SizeY);
	}

	// ---------- РЎР»РµРїСЏС‰Р°СЏ РґСѓРіР°: РІР°СЂРёС‚ Р±РµР· СЃРІР°СЂРѕС‡РЅРѕР№ РјР°СЃРєРё (arc eye) ----------
	// РҐРѕР»РѕРґРЅС‹Р№ Р±РµР»Рѕ-РіРѕР»СѓР±РѕР№ СЃС‚СЂРѕР±РѕСЃРєРѕРї РґСѓРіРё Р·Р°Р»РёРІР°РµС‚ СЌРєСЂР°РЅ вЂ” РІР°СЂРёС‚СЊ РІСЃР»РµРїСѓСЋ С‚СЂСѓРґРЅРѕ,
	// СЌС‚Рѕ Рё РµСЃС‚СЊ РґР°РІР»РµРЅРёРµ В«РЅР°РґРµРЅСЊ РјР°СЃРєСѓВ». РџР°СЂР°Р»Р»РµР»СЊРЅРѕ РєРѕРїРёС‚СЃСЏ РїР°РЅРёРєР° (СЃРј. Character::Tick).
	if (Character->IsWelding() && !Character->HasWeldingMask())
	{
		const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		const float Flick = 0.55f + 0.45f * FMath::Abs(FMath::Sin(Time * 37.f)); // СЂРІР°РЅРѕРµ РјРµСЂС†Р°РЅРёРµ РґСѓРіРё
		const FLinearColor Arc(0.78f, 0.86f, 1.0f, 0.58f * Flick);
		DrawRect(Arc, 0.f, 0.f, SizeX, SizeY);
		DrawText(FAvLoc::S(TEXT("ui_010")), FLinearColor(0.05f, 0.05f, 0.08f),
			SizeX * 0.5f - 150.f, SizeY * 0.30f, Font, 1.0f);
	}

	// ---------- Р—Р°СЂСЏРґРєР° Р±СЂРѕСЃРєР° (СѓРґРµСЂР¶Р°РЅРёРµ G): РїРѕР»РѕСЃРєР° СЃРёР»С‹ Сѓ С†РµРЅС‚СЂР° ----------
	if (Character->IsChargingThrow())
	{
		const float A = Character->GetThrowChargeAlpha();
		const float BW = 200.f, BH = 12.f;
		const float BX = (SizeX - BW) * 0.5f, BY = SizeY * 0.60f;
		DrawRect(BarBG, BX, BY, BW, BH);
		DrawRect(A >= 1.f ? FLinearColor(0.95f, 0.85f, 0.2f) : Accent, BX, BY, BW * A, BH);
		DrawText(A >= 1.f ? FAvLoc::S(TEXT("ui_011")) : FAvLoc::S(TEXT("ui_012")),
			TextMain, BX, BY - 18.f, Font, 0.85f);
	}

	// ---------- РњРѕРЅРёС‚РѕСЂ РѕРїРµСЂР°С‚РѕСЂР° (Tab РІ Р·РѕРЅРµ Р“РђР—РµР»Рё) ----------
	if (Character->IsMonitorOpen())
	{
		DrawRect(FLinearColor(0.01f, 0.012f, 0.02f, 0.97f), 0.f, 0.f, SizeX, SizeY);
		DrawText(FAvLoc::S(TEXT("ui_013")), AvaryoHUDStyle::Accent, 30.f, 24.f, Font, 1.5f);

		// РџР»РёС‚РєРё 16:9 РІ РґРІРµ РєРѕР»РѕРЅРєРё
		const float TileW = FMath::Min(SizeX * 0.42f, 560.f);
		const float TileH = TileW * 9.f / 16.f;
		const float PadX = (SizeX - TileW * 2.f) / 3.f;
		float TileX = PadX;
		float TileY = 80.f;
		int32 Column = 0;

		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			AAvaryoCharacter* Crew = *It;

			DrawRect(FLinearColor(0.f, 0.f, 0.f, 1.f), TileX - 2.f, TileY - 2.f, TileW + 4.f, TileH + 4.f);
			if (UTexture* CamImage = Crew->GetChestCamTarget())
			{
				DrawTexture(CamImage, TileX, TileY, TileW, TileH, 0.f, 0.f, 1.f, 1.f);
			}

			// РџРѕРґРїРёСЃСЊ: РёРјСЏ + СЃРѕСЃС‚РѕСЏРЅРёРµ
			FString Name = Crew->GetPlayerState() ? Crew->GetPlayerState()->GetPlayerName() : FAvLoc::S(TEXT("ui_014"));
			if (Crew == Character)
			{
				Name += FAvLoc::S(TEXT("ui_015"));
			}
			if (Crew->VitalsComponent)
			{
				if (Crew->VitalsComponent->IsUnconscious())
				{
					Name += FAvLoc::S(TEXT("ui_016"));
				}
				else if (Crew->VitalsComponent->IsWounded())
				{
					Name += FAvLoc::S(TEXT("ui_017"))
						+ FString::FromInt(FMath::CeilToInt(Crew->VitalsComponent->GetBleedOutSeconds()))
						+ FAvLoc::S(TEXT("ui_018"));
				}
			}
			DrawText(Name, AvaryoHUDStyle::TextMain, TileX + 8.f, TileY + 6.f, Font, 1.0f);

			// РњРёРЅРё-С€РєР°Р»С‹ HP Рё РїР°РЅРёРєРё РїРѕРґ РїР»РёС‚РєРѕР№
			if (UVitalsComponent* CrewVitals = Crew->VitalsComponent)
			{
				const float BarY = TileY + TileH + 6.f;
				DrawRect(AvaryoHUDStyle::BarBG, TileX, BarY, TileW * 0.48f, 8.f);
				DrawRect(FLinearColor(0.8f, 0.12f, 0.12f), TileX, BarY, TileW * 0.48f * FMath::Clamp(CrewVitals->GetHealth(), 0.f, 100.f) / 100.f, 8.f);
				DrawRect(AvaryoHUDStyle::BarBG, TileX + TileW * 0.52f, BarY, TileW * 0.48f, 8.f);
				DrawRect(FLinearColor(0.25f, 0.7f, 0.85f), TileX + TileW * 0.52f, BarY, TileW * 0.48f * FMath::Clamp(CrewVitals->GetPanic(), 0.f, 100.f) / 100.f, 8.f);
			}

			if (++Column % 2 == 0)
			{
				TileX = PadX;
				TileY += TileH + 44.f;
			}
			else
			{
				TileX += TileW + PadX;
			}
		}
		return; // РјРѕРЅРёС‚РѕСЂ РїРµСЂРµРєСЂС‹РІР°РµС‚ РѕР±С‹С‡РЅС‹Р№ HUD
	}

	// РўРµРєСЃС‚ РїРѕ С†РµРЅС‚СЂСѓ (РїРѕ X) РЅР° Р·Р°РґР°РЅРЅРѕР№ РІС‹СЃРѕС‚Рµ
	auto DrawCentered = [&](const FString& Text, const FLinearColor& Color, float Y, float Scale)
	{
		float W = 0.f, H = 0.f;
		GetTextSize(Text, W, H, Font, Scale);
		DrawText(Text, Color, (SizeX - W) * 0.5f, Y, Font, Scale);
		return H;
	};

	// ---------- РҐРђР‘: РєР°СЃСЃР°/СЂРµРїСѓС‚Р°С†РёСЏ/РєРІРѕС‚Р° РєРѕРЅС‚РѕСЂС‹ (РїРµС‚Р»СЏ В«Р·Р°СЂР°Р±Р°С‚С‹РІР°РµРј РЅР° Р°РїРіСЂРµР№РґВ» РІРёРґРЅР° РЅР° Р±Р°Р·Рµ, PLAYER_APPEAL #3) ----------
	// РўРѕР»СЊРєРѕ РІ С…Р°Р±Рµ вЂ” РІРѕ РІСЂРµРјСЏ РјРёСЃСЃРёРё HUD РґРµСЂР¶РёРј РґРёРµРіРµС‚РёС‡РЅРѕ-С‡РёСЃС‚С‹Рј.
	if (Run && Run->IsHubMode() && !Character->IsMonitorOpen())
	{
		const FString HubLine = FString::Printf(TEXT("РљРћРќРўРћР Рђ В«РђРІР°СЂРёР№РєР°В»    В·    РљР°СЃСЃР°: %d в‚Ѕ    В·    Р РµРїСѓС‚Р°С†РёСЏ: %s"),
			Run->GetCompanyBalanceLive(), *ARunState::ReputationTitle(Run->GetReputation()));
		DrawCentered(HubLine, Accent, 26.f, 1.2f);
		if (Run->IsQuotaActive())
		{
			const FString QLine = FString::Printf(TEXT("РљРІРѕС‚Р° РґРёСЃРїРµС‚С‡РµСЂР°: %d / %d в‚Ѕ    (СЃСЂРѕРє: СЃРјРµРЅР° в„– %d)"),
				Run->GetQuotaPaid(), Run->GetQuotaTarget(), Run->GetQuotaDeadlineShift());
			DrawCentered(QLine, TextDim, 50.f, 1.0f);
		}
	}

	// РџР»Р°С€РєР°-РїРѕРґСЃРєР°Р·РєР° РїРѕ С†РµРЅС‚СЂСѓ СЃ РѕСЂР°РЅР¶РµРІРѕР№ РєСЂРѕРјРєРѕР№ ("[E] РџРѕРґРЅСЏС‚СЊ ...")
	auto DrawPromptBox = [&](const FString& Prompt)
	{
		float W = 0.f, H = 0.f;
		GetTextSize(Prompt, W, H, Font, 1.3f);

		const float BoxW = W + 36.f, BoxH = H + 16.f;
		const float BoxX = (SizeX - BoxW) * 0.5f;
		const float BoxY = SizeY * 0.58f;

		DrawRect(BoxBG, BoxX, BoxY, BoxW, BoxH);
		DrawRect(Accent, BoxX, BoxY, BoxW, 3.f); // РѕСЂР°РЅР¶РµРІР°СЏ РєСЂРѕРјРєР° СЃРІРµСЂС…Сѓ
		DrawText(Prompt, TextMain, BoxX + 18.f, BoxY + 8.f, Font, 1.3f);
	};

	// РџРѕР»РѕСЃР° РїСЂРѕРіСЂРµСЃСЃР° СЃ РїРѕРґРїРёСЃСЊСЋ РїРѕ С†РµРЅС‚СЂСѓ СЌРєСЂР°РЅР° (РїСЂРёРјРµРЅРµРЅРёРµ, СЂРµРјРѕРЅС‚)
	auto DrawCastBar = [&](const FString& Label, float Progress, const FLinearColor& Fill)
	{
		const float BarW = 340.f, BarH = 20.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		const float BoxY = SizeY * 0.46f;

		float LabelW = 0.f, LabelH = 0.f;
		GetTextSize(Label, LabelW, LabelH, Font, 1.1f);

		DrawRect(BoxBG, BoxX - 12.f, BoxY - LabelH - 10.f, BarW + 24.f, LabelH + BarH + 22.f);
		DrawText(Label, TextMain, BoxX, BoxY - LabelH - 4.f, Font, 1.1f);
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(Fill, BoxX, BoxY + 2.f, BarW * FMath::Clamp(Progress, 0.f, 1.f), BarH);
	};

	// ---------- РЁРєР°Р»С‹ (СЃР»РµРІР° СЃРІРµСЂС…Сѓ) ----------
	if (Vitals)
	{
		auto DrawBar = [&](const FString& Label, float Value, const FLinearColor& Color, float Y)
		{
			const float X = 30.f, W = 180.f, H = 14.f;
			DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.72f), X, Y, W, H);
			DrawRect(Color, X, Y, W * FMath::Clamp(Value, 0.f, 100.f) / 100.f, H);
			DrawRect(AccentDim, X, Y, W, 1.f);
			DrawRect(AccentDim, X, Y + H - 1.f, W, 1.f);
			DrawRect(AccentDim, X, Y, 1.f, H);
			DrawRect(AccentDim, X + W - 1.f, Y, 1.f, H);
			DrawText(Label, TextMain, X + W + 8.f, Y - 2.f, Font, 0.95f);
		};

		float Y = 30.f;
		DrawBar(FAvLoc::S(TEXT("ui_019")),  Vitals->GetHealth(),  FLinearColor(0.8f, 0.12f, 0.12f), Y); Y += 22.f;
		DrawBar(FAvLoc::S(TEXT("ui_020")), Vitals->GetStamina(), FLinearColor(0.2f, 0.7f, 0.25f),  Y); Y += 22.f;
		DrawBar(FAvLoc::S(TEXT("ui_021")),   Vitals->GetPanic(),   FLinearColor(0.25f, 0.7f, 0.85f), Y); Y += 22.f;
		DrawBar(FAvLoc::S(TEXT("ui_022")), Vitals->GetBladder(), FLinearColor(0.8f, 0.6f, 0.12f),  Y); Y += 22.f;
		DrawBar(FAvLoc::S(TEXT("ui_023")),  Vitals->GetSmell(),   FLinearColor(0.5f, 0.45f, 0.12f), Y); Y += 22.f;

		// РЁСѓРј: РЅР°СЃРєРѕР»СЊРєРѕ С‚С‹ СЃРµР№С‡Р°СЃ СЃР»С‹С€РµРЅ вЂ” Р·Р°РґРµР» РїРѕРґ РјРѕРЅСЃС‚СЂР°-СЃР»СѓС…Р°С‡Р°.
		// Р”РІРёР¶РµРЅРёРµ (Р±РµРі/РїСЂРёСЃРµРґ) + СЃРѕР±С‹С‚РёСЏ MakeNoise (РєР°С€РµР»СЊ, РёРєРѕС‚Р°, РѕС‚РґС‹С€РєР°, Р±СЂРѕСЃРѕРє, РїРѕС‡РёРЅРєР°вЂ¦).
		float Noise01 = FMath::Clamp(Character->GetVelocity().Size2D() / 750.f, 0.f, 1.f);
		if (Character->bIsCrouched)
		{
			Noise01 *= 0.4f;
		}
		if (APickupItem* HeldNoisy = Character->GetHeldItem())
		{
			if (HeldNoisy->IsSpraying())
			{
				Noise01 = 1.f;
			}
		}
		Noise01 = FMath::Max(Noise01, Character->GetSelfNoise01()); // РІСЃРїР»РµСЃРєРё РѕС‚ СЃРѕР±С‹С‚РёР№ С€СѓРјР°
		DrawBar(FAvLoc::S(TEXT("ui_024")), Noise01 * 100.f, FLinearColor(1.f, 0.55f, 0.2f), Y);

		// Р‘Р°С‚Р°СЂРµСЏ РЅР°Р»РѕР±РЅРѕРіРѕ С„РѕРЅР°СЂСЏ: СЏСЂРєРѕ-Р¶С‘Р»С‚Р°СЏ РєРѕРіРґР° РІРєР»СЋС‡С‘РЅ, С‚СѓСЃРєР»Р°СЏ РєРѕРіРґР° РІС‹РєР»СЋС‡РµРЅ
		if (UFlashlightComponent* Flashlight = Character->FlashlightComponent)
		{
			Y += 22.f;
			const bool bOn = Flashlight->IsOn();
			DrawBar(bOn ? FAvLoc::S(TEXT("ui_025")) : FAvLoc::S(TEXT("ui_026")),
				Flashlight->GetBatteryLevel(),
				bOn ? FLinearColor(0.95f, 0.85f, 0.25f) : FLinearColor(0.45f, 0.42f, 0.2f), Y);
		}

		// Р“Р°Р·РѕР°РЅР°Р»РёР·Р°С‚РѕСЂ: РґРµСЂР¶РёС€СЊ РїСЂРёР±РѕСЂ вЂ” РІРёРґРёС€СЊ СѓСЂРѕРІРµРЅСЊ РіР°Р·Р° СЂСЏРґРѕРј (Р·РµР»С‘РЅС‹Р№в†’РєСЂР°СЃРЅС‹Р№).
		// РќР°Р№С‚Рё СѓС‚РµС‡РєСѓ Рё РїРѕРЅСЏС‚СЊ, СЂР°Р·РІРµСЏР»Рё Р»Рё РіР°Р· РїРµРЅРѕР№ РїРµСЂРµРґ СЃРІР°СЂРєРѕР№.
		if (Character->IsHoldingGasDetector())
		{
			Y += 22.f;
			const float Gas = FMath::Clamp(Character->GetGasReading(), 0.f, 1.f);
			const FLinearColor GasColor = FMath::Lerp(FLinearColor(0.2f, 0.8f, 0.3f), FLinearColor(0.9f, 0.15f, 0.1f), Gas);
			DrawBar(Gas > 0.6f ? FAvLoc::S(TEXT("ui_027")) : (Gas > 0.05f ? FAvLoc::S(TEXT("ui_028")) : FAvLoc::S(TEXT("ui_029"))),
				Gas * 100.f, GasColor, Y);
		}
	}

	// ---------- Р Р°С†РёСЏ РґРёСЃРїРµС‚С‡РµСЂР° (СЃРІРµСЂС…Сѓ РїРѕ С†РµРЅС‚СЂСѓ, РїР»Р°С€РєРё РіР°СЃРЅСѓС‚ СЃР°РјРё) ----------
	if (Run)
	{
		const float Now = GetWorld()->GetTimeSeconds();

		float LineY = 16.f;
		for (const FDispatcherLine& Line : Run->GetDispatcherLines())
		{
			const float Age = Now - Line.ReceivedAt;
			if (Age > 9.f)
			{
				continue; // РѕС‚РіРѕРІРѕСЂРёР» вЂ” РїР»Р°С€РєР° РїРѕРіР°СЃР»Р°
			}
			const float Alpha = Age > 7.f ? 1.f - (Age - 7.f) / 2.f : 1.f; // РїРѕСЃР»РµРґРЅРёРµ 2 СЃ С‚Р°СЋС‚

			const FString Prefix = Line.Speaker + TEXT(": ");
			float PrefixW = 0.f, PrefixH = 0.f;
			GetTextSize(Prefix, PrefixW, PrefixH, Font, 1.05f);
			float TextW = 0.f, TextH = 0.f;
			GetTextSize(Line.Text, TextW, TextH, Font, 1.05f);
			const float BoxW = PrefixW + TextW + 28.f;
			const float BoxX = (SizeX - BoxW) * 0.5f;

			FLinearColor BG = BoxBG;        BG.A *= Alpha;
			FLinearColor Edge = Accent;     Edge.A *= Alpha;
			FLinearColor NameC = Accent;    NameC.A *= Alpha;
			FLinearColor TextC = TextMain;  TextC.A *= Alpha;

			DrawRect(BG, BoxX, LineY, BoxW, TextH + 14.f);
			DrawRect(Edge, BoxX, LineY, 4.f, TextH + 14.f); // РѕСЂР°РЅР¶РµРІР°СЏ РєСЂРѕРјРєР° СЃР»РµРІР° вЂ” В«СЌС„РёСЂВ»
			DrawText(Prefix, NameC, BoxX + 14.f, LineY + 7.f, Font, 1.05f);
			DrawText(Line.Text, TextC, BoxX + 14.f + PrefixW, LineY + 7.f, Font, 1.05f);
			LineY += TextH + 20.f;
		}
	}

	// ---------- Р—Р°Р±РµРі: Р·Р°РґР°С‡Рё, С‚Р°Р№РјРµСЂ, С„Р°Р·Р° (СЃРїСЂР°РІР° СЃРІРµСЂС…Сѓ) ----------
	if (Run)
	{
		const int32 Elapsed = FMath::FloorToInt(Run->GetElapsedSeconds());
		const FString Header = FString::Printf(TEXT("%s %d/%d   %02d:%02d"),
			*FAvLoc::S(TEXT("ui_030")),
			Run->GetRepairedCount(), Run->GetTotalObjectives(), Elapsed / 60, Elapsed % 60);

		float HeaderW = 0.f, HeaderH = 0.f;
		GetTextSize(Header, HeaderW, HeaderH, Font, 1.15f);

		const float PanelW = 250.f;
		const float RowH = 20.f;
		const float ListX = SizeX - PanelW - 24.f;
		float ListY = 30.f;

		DrawRect(BoxBG, ListX - 12.f, ListY - 8.f, PanelW + 24.f, HeaderH + Run->GetObjectives().Num() * RowH + 24.f);
		DrawRect(Accent, ListX - 12.f, ListY - 8.f, PanelW + 24.f, 3.f);
		DrawText(Header, Accent, ListX, ListY, Font, 1.15f);
		ListY += HeaderH + 8.f;

		for (const ARepairable* Objective : Run->GetObjectives())
		{
			if (!Objective)
			{
				continue;
			}
			FString Row;
			FLinearColor RowColor;
			if (!Objective->IsBroken())
			{
				Row = FString::Printf(TEXT("[+] %s"), *Objective->DisplayName.ToString());
				RowColor = FLinearColor(0.3f, 0.8f, 0.3f);
			}
			else if (Objective->GetRepairProgress() > 0.f)
			{
				Row = FString::Printf(TEXT("[~] %s (%d%%)"), *Objective->DisplayName.ToString(),
					FMath::RoundToInt(Objective->GetRepairProgress() * 100.f));
				RowColor = Accent;
			}
			else
			{
				Row = FString::Printf(TEXT("[-] %s"), *Objective->DisplayName.ToString());
				RowColor = TextDim;
			}
			if (Objective->IsOnFire())
			{
				Row = FString::Printf(TEXT("[Р“РћР РРў!] %s"), *Objective->DisplayName.ToString());
				RowColor = FLinearColor(1.0f, 0.3f, 0.1f); // РѕРіРЅРµРЅРЅРѕ-РєСЂР°СЃРЅС‹Р№
			}
			DrawText(Row, RowColor, ListX, ListY, Font, 0.95f);
			ListY += RowH;
		}

		// Р¤РёРЅР°Р»: В«РђРєС‚ РІС‹РїРѕР»РЅРµРЅРЅС‹С… СЂР°Р±РѕС‚В» вЂ” РјРµРјРЅС‹Р№ РёС‚РѕРіРѕРІС‹Р№ РѕС‚С‡С‘С‚
		if (Run->GetPhase() == ERunPhase::Won || Run->GetPhase() == ERunPhase::Lost)
		{
			const bool bWon = Run->GetPhase() == ERunPhase::Won;
			const TArray<FPlayerRunStats>& AllStats = Run->GetPlayerStats();

			// РњР°РєСЃРёРјСѓРјС‹ РґР»СЏ СЂР°Р·РґР°С‡Рё Р·РІР°РЅРёР№
			int32 MaxRepairs = 0, MaxWounded = 0, MaxRevives = 0, MaxDrags = 0, MaxBotched = 0;
			int32 MaxShoved = 0, MaxTripped = 0;
			int32 MaxExplosions = 0, MaxShorts = 0, MaxThrows = 0, MaxCoffees = 0, MaxLoot = 0;
			float MaxPanic = 0.f, MaxSmell = 0.f, MaxSmoke = 0.f;
			for (const FPlayerRunStats& S : AllStats)
			{
				MaxLoot    = FMath::Max(MaxLoot,    S.LootValue);
				MaxRepairs = FMath::Max(MaxRepairs, S.Repairs);
				MaxWounded = FMath::Max(MaxWounded, S.TimesWounded);
				MaxRevives = FMath::Max(MaxRevives, S.Revives);
				MaxDrags   = FMath::Max(MaxDrags,   S.Drags);
				MaxBotched = FMath::Max(MaxBotched, S.BotchedRepairs);
				MaxShoved  = FMath::Max(MaxShoved,  S.ShovedOthers);
				MaxTripped = FMath::Max(MaxTripped, S.TimesTripped);
				MaxExplosions = FMath::Max(MaxExplosions, S.ExplosionsCaused);
				MaxShorts  = FMath::Max(MaxShorts,  S.ShortsCaused);
				MaxThrows  = FMath::Max(MaxThrows,  S.Throws);
				MaxCoffees = FMath::Max(MaxCoffees, S.Coffees);
				MaxPanic   = FMath::Max(MaxPanic,   S.PanicSeconds);
				MaxSmell   = FMath::Max(MaxSmell,   S.SmellSeconds);
				MaxSmoke   = FMath::Max(MaxSmoke,   S.SmokeSeconds);
			}

			const float ReportW = FMath::Min(820.f, SizeX - 80.f);
			const float ReportRowH = 26.f;
			const float ReportH = 260.f + AllStats.Num() * ReportRowH * 4.f;
			const float PX = (SizeX - ReportW) * 0.5f;
			float PY = FMath::Max(40.f, (SizeY - ReportH) * 0.5f);

			DrawRect(PanelBG, PX, PY, ReportW, ReportH);
			DrawRect(Accent, PX, PY, ReportW, 4.f);

			float TY = PY + 18.f;
			TY += DrawCentered(FString::Printf(TEXT("РђРљРў Р’Р«РџРћР›РќР•РќРќР«РҐ Р РђР‘РћРў в„– %03d"), Run->GetShiftNumber()),
				Accent, TY, 1.6f) + 6.f;
			TY += DrawCentered(bWon
				? FString::Printf(TEXT("РћР±СЉРµРєС‚ СЃРґР°РЅ. Р’СЂРµРјСЏ: %02d:%02d. Р—Р°РєР°Р·С‡РёРє РЅРµРґРѕРІРѕР»РµРЅ, РЅРѕ РїРѕРґРїРёСЃР°Р»."), Elapsed / 60, Elapsed % 60)
				: TEXT("РћР±СЉРµРєС‚ РќР• СЃРґР°РЅ: РІСЃСЏ Р±СЂРёРіР°РґР° РІС‹РІРµРґРµРЅР° РёР· СЃС‚СЂРѕСЏ. РђРєС‚ РїРѕРґРїРёСЃР°РЅ Р·Р°РґРЅРёРј С‡РёСЃР»РѕРј."),
				bWon ? FLinearColor(0.3f, 0.9f, 0.3f) : FLinearColor(0.95f, 0.25f, 0.25f), TY, 1.1f) + 8.f;

			// В«РЎРІРѕРґРєР°В» вЂ” Р°РІС‚Рѕ-Р·Р°РіРѕР»РѕРІРѕРє РїСЂРѕ СЃР°РјРѕРµ Р·Р°РїРѕРјРёРЅР°СЋС‰РµРµСЃСЏ (РїСЂРёРјР°РЅРєР° РґР»СЏ СЃРєСЂРёРЅР°/РєР»РёРїР°, PLAYER_APPEAL #1/#2).
			// РџСЂРёРѕСЂРёС‚РµС‚ РѕС‚ СЃР°РјРѕРіРѕ Р·СЂРµР»РёС‰РЅРѕРіРѕ Рє Р±СѓРґРЅРёС‡РЅРѕРјСѓ; С‡РёС‚Р°РµС‚ СѓР¶Рµ РїРѕСЃС‡РёС‚Р°РЅРЅС‹Рµ РјР°РєСЃРёРјСѓРјС‹ РїРѕ Р±СЂРёРіР°РґРµ.
			FString Headline;
			if (!bWon)                       Headline = TEXT("вљ° РЎРІРѕРґРєР°: Р±СЂРёРіР°РґР° РѕСЃС‚Р°Р»Р°СЃСЊ РЅР° РѕР±СЉРµРєС‚Рµ. РќР°РІСЃРµРіРґР°.");
			else if (MaxExplosions > 0)      Headline = TEXT("рџ”Ґ РЎРІРѕРґРєР°: РѕР±СЉРµРєС‚ С‡СѓС‚СЊ РЅРµ СЃРїР°Р»РёР»Рё РґРѕС‚Р»Р°.");
			else if (MaxShorts > 0)          Headline = TEXT("вљЎ РЎРІРѕРґРєР°: СѓСЃС‚СЂРѕРёР»Рё РёР»Р»СЋРјРёРЅР°С†РёСЋ РЅР° С‰РёС‚РєРµ.");
			else if (MaxLoot >= 2000)        Headline = TEXT("рџ’° РЎРІРѕРґРєР°: РІС‹РЅРµСЃР»Рё РёР· РґРѕРјР° Р±РѕР»СЊС€Рµ, С‡РµРј РїРѕС‡РёРЅРёР»Рё.");
			else if (MaxSmell > 5.f)         Headline = TEXT("рџ¤ў РЎРІРѕРґРєР°: РєС‚Рѕ-С‚Рѕ РїСЂРѕРІРѕРЅСЏР» РІСЃСЋ СЃРјРµРЅСѓ.");
			else if (MaxWounded == 0 && MaxBotched == 0) Headline = TEXT("вњ… РЎРІРѕРґРєР°: С‡РёСЃС‚Рѕ СЃРґР°Р»Рё, Р±РµР· РїРѕС‚РµСЂСЊ. Р РµРґРєРѕСЃС‚СЊ!");
			else                             Headline = TEXT("рџ›  РЎРІРѕРґРєР°: СЃРјРµРЅР° РєР°Рє СЃРјРµРЅР°. Р‘С‹РІР°Р»Рѕ Рё С…СѓР¶Рµ.");
			TY += DrawCentered(Headline, TextMain, TY, 1.15f) + 12.f;

			int32 PlayerIndex = 0;
			for (const FPlayerRunStats& S : AllStats)
			{
				if (!IsValid(S.Character)) { continue; } // ghost-Р·Р°РїРёСЃСЊ РѕС‚РєР»СЋС‡РёРІС€РµРіРѕСЃСЏ РЅРµ СЂРёСЃСѓРµРј вЂ” СЃС‚СЂРѕРєРё СЃС…РѕРґСЏС‚СЃСЏ СЃ ShiftNet (CODE_AUDIT3 #8 follow-up)
				++PlayerIndex;
				FString Name = FString::Printf(TEXT("РњРѕРЅС‚С‘СЂ в„–%d"), PlayerIndex);
				if (S.Character && S.Character->GetPlayerState())
				{
					Name = S.Character->GetPlayerState()->GetPlayerName();
				}

				// Р—РІР°РЅРёРµ: РёРЅС†РёРґРµРЅС‚С‹ РІРЅРµ РєРѕРЅРєСѓСЂРµРЅС†РёРё, РґР°Р»СЊС€Рµ вЂ” РїРѕ Р»СѓС‡С€РµРјСѓ РїРѕРєР°Р·Р°С‚РµР»СЋ
				FString Title;
				if (S.Incidents > 0)                                   Title = TEXT("Р‘РёРѕР»РѕРіРёС‡РµСЃРєР°СЏ СѓРіСЂРѕР·Р°");
				else if (S.ExplosionsCaused > 0 && S.ExplosionsCaused == MaxExplosions) Title = TEXT("РџРѕР¶Р°СЂРЅС‹Р№ РЅР°РѕР±РѕСЂРѕС‚");
				else if (S.ShortsCaused > 0 && S.ShortsCaused == MaxShorts) Title = TEXT("Р­Р»РµРєС‚СЂРёРє Р­РґРёСЃРѕРЅ");
				else if (S.BotchedRepairs > 0 && S.BotchedRepairs == MaxBotched) Title = TEXT("РќР°СЂРѕРґРЅС‹Р№ СѓРјРµР»РµС†");
				else if (S.SmellSeconds > 5.f && S.SmellSeconds >= MaxSmell) Title = TEXT("РђРјР±СЂРµ СЃРјРµРЅС‹");
				else if (S.SmokeSeconds > 8.f && S.SmokeSeconds >= MaxSmoke) Title = TEXT("Р“Р»Р°РІРЅС‹Р№ РєСѓСЂРёР»СЊС‰РёРє");
				else if (S.LootValue >= 2000 && S.LootValue == MaxLoot) Title = TEXT("Р‘Р°СЂС‹РіР° СЃРјРµРЅС‹");
				else if (S.ToiletVisits >= 2)                          Title = TEXT("Р”РёСЃС†РёРїР»РёРЅРёСЂРѕРІР°РЅРЅС‹Р№ РјРѕС‡РµРІРѕР№ РїСѓР·С‹СЂСЊ");
				else if (S.Repairs > 0 && S.Repairs == MaxRepairs)     Title = TEXT("Р Р°Р±РѕС‚РЅРёРє РјРµСЃСЏС†Р°");
				else if (S.Revives > 0 && S.Revives == MaxRevives)     Title = TEXT("РџРѕР»РµРІРѕР№ РјРµРґРёРє");
				else if (S.Drags > 0 && S.Drags == MaxDrags)           Title = TEXT("Р­РІР°РєСѓР°С‚РѕСЂ");
				else if (S.ShovedOthers > 0 && S.ShovedOthers == MaxShoved) Title = TEXT("Р“СЂРѕР·Р° РєРѕР»Р»РµРєС‚РёРІР°");
				else if (S.Throws > 1 && S.Throws == MaxThrows)        Title = TEXT("РњРµС‚Р°С‚РµР»СЊ СЃРЅР°СЂСЏРґРѕРІ");
				else if (S.TimesTripped > 1 && S.TimesTripped == MaxTripped) Title = TEXT("РЎРїРѕС‚С‹РєР°С‡ СЃРјРµРЅС‹");
				else if (S.TimesWounded > 0 && S.TimesWounded == MaxWounded) Title = TEXT("Р“Р»Р°РІРЅС‹Р№ РїРѕСЃС‚СЂР°РґР°РІС€РёР№");
				else if (S.PanicSeconds > 1.f && S.PanicSeconds >= MaxPanic) Title = TEXT("РџР°РЅРёРєС‘СЂ СЃРјРµРЅС‹");
				else if (S.Coffees >= 2 && S.Coffees == MaxCoffees)    Title = TEXT("РљРѕС„РµРјР°РЅ СЃРјРµРЅС‹");
				else                                                   Title = TEXT("РџСЂРѕСЃС‚Рѕ РїСЂРёСЃСѓС‚СЃС‚РІРѕРІР°Р»");

				// Р‘СѓС…РіР°Р»С‚РµСЂРёСЏ: РїСЂРµРјРёРё Рё С€С‚СЂР°С„С‹ (РµРґРёРЅР°СЏ С„РѕСЂРјСѓР»Р° СЃ СЃРµСЂРІРµСЂРѕРј)
				const int32 Balance = ARunState::ComputePlayerBalance(S);

				const FString Row1 = FString::Printf(TEXT("%s вЂ” В«%sВ»"), *Name, *Title);
				// Р”Р»РёРЅРЅСѓСЋ СЃС‚Р°С‚РёСЃС‚РёРєСѓ РґРµР»РёРј РЅР° РґРІРµ СЃС‚СЂРѕРєРё, С‡С‚РѕР±С‹ РЅРµ РІС‹Р»РµР·Р°Р»Р° Р·Р° РїР°РЅРµР»СЊ
				const FString Row2 = FString::Printf(TEXT("РїРѕС‡РёРЅРєРё: %d   РєРѕР»С…РѕР·: %d   РїРѕРґСЉС‘РјС‹: %d   СЌРІР°РєСѓР°С†РёРё: %d   С‚СѓР°Р»РµС‚: %d"),
					S.Repairs, S.BotchedRepairs, S.Revives, S.Drags, S.ToiletVisits);
				const FString Row3 = FString::Printf(TEXT("СЂР°РЅРµРЅРёСЏ: %d   РёРЅС†РёРґРµРЅС‚С‹: %d   РїР°РЅРёРєР°: %d СЃ   РІРѕРЅСЏР»: %d СЃ   РёС‚РѕРі: %s%d в‚Ѕ"),
					S.TimesWounded, S.Incidents, FMath::RoundToInt(S.PanicSeconds), FMath::RoundToInt(S.SmellSeconds),
					Balance >= 0 ? TEXT("+") : TEXT(""), Balance);

				const FString Row4 = FString::Printf(TEXT("С‚РѕР»РєРЅСѓР»: %d   СЃРїРѕС‚РєРЅСѓР»СЃСЏ: %d   РєР°С‚Р°Р»СЃСЏ РїРѕ РїРµРЅРµ: %d СЃ   РІС‹РЅРµСЃ: %d в‚Ѕ"),
					S.ShovedOthers, S.TimesTripped, FMath::RoundToInt(S.SlipSeconds), S.LootValue);

				DrawText(Row1, TextMain, PX + 28.f, TY, Font, 1.1f);
				TY += ReportRowH;
				DrawText(Row2, TextDim, PX + 28.f, TY, Font, 0.92f);
				TY += ReportRowH;
				DrawText(Row3, Balance >= 0 ? TextDim : FLinearColor(0.95f, 0.45f, 0.3f), PX + 28.f, TY, Font, 0.92f);
				TY += ReportRowH;
				DrawText(Row4, TextDim, PX + 28.f, TY, Font, 0.92f);
				TY += ReportRowH;
			}

			TY += 8.f;
			const int32 ShiftNet = Run->GetShiftNet();
			const FLinearColor PosC(0.3f, 0.9f, 0.3f);
			const FLinearColor NegC(0.95f, 0.45f, 0.3f);

			const FString TotalLine = ShiftNet >= 0
				? FString::Printf(TEXT("РС‚РѕРі СЃРјРµРЅС‹: +%d в‚Ѕ"), ShiftNet)
				: FString::Printf(TEXT("РС‚РѕРі СЃРјРµРЅС‹: %d в‚Ѕ (РІ РјРёРЅСѓСЃ)"), ShiftNet);
			TY += DrawCentered(TotalLine, ShiftNet >= 0 ? PosC : NegC, TY, 1.25f) + 6.f;

			// Р‘Р°Р»Р°РЅСЃ РєРѕРЅС‚РѕСЂС‹: Р±С‹Р»Рѕ в†’ СЃС‚Р°Р»Рѕ (В§19, РєРѕРїРёС‚СЃСЏ РјРµР¶РґСѓ СЃРјРµРЅР°РјРё)
			const int32 BalBefore = Run->GetCompanyBalanceStart();
			const int32 BalAfter = BalBefore + ShiftNet;
			const FString BalLine = FString::Printf(TEXT("РљР°СЃСЃР° РєРѕРЅС‚РѕСЂС‹: %d в‚Ѕ  в†’  %d в‚Ѕ"), BalBefore, BalAfter);
			TY += DrawCentered(BalLine, BalAfter >= 0 ? TextMain : NegC, TY, 1.1f) + 4.f;

			// Р РµРїСѓС‚Р°С†РёСЏ РєРѕРЅС‚РѕСЂС‹ вЂ” РІР»РёСЏРµС‚ РЅР° РєР°С‡РµСЃС‚РІРѕ РІС‹РґР°РІР°РµРјРѕРіРѕ РєРѕРјРїР»РµРєС‚Р° РІ СЃР»РµРґСѓСЋС‰РёС… СЃРјРµРЅР°С…
			const FString RepLine = FString::Printf(TEXT("Р РµРїСѓС‚Р°С†РёСЏ: %s"), *ARunState::ReputationTitle(Run->GetReputation()));
			TY += DrawCentered(RepLine, TextDim, TY, 1.0f) + 6.f;

			// РљР°СЂСЊРµСЂР° РєРѕРЅС‚РѕСЂС‹ Р·Р° РІСЃС‘ РІСЂРµРјСЏ (СЂРµРїР»РёС†РёСЂСѓРµС‚СЃСЏ РёР· ARunState вЂ” Сѓ РєР»РёРµРЅС‚Р° С‚РѕР¶Рµ РІРµСЂРЅРѕ)
			const FString CareerLine = FString::Printf(TEXT("РљР°СЂСЊРµСЂР°: РїРѕС‡РёРЅРѕРє РІСЃРµРіРѕ %d В· РґРѕРјРѕРІ СЃРїР°Р»РёР»Рё %d В· РёРЅС†РёРґРµРЅС‚РѕРІ %d"),
				Run->GetCareerRepairs(), Run->GetCareerBlownUp(), Run->GetCareerIncidents());
			TY += DrawCentered(CareerLine, TextDim, TY, 0.85f) + 8.f;

			// РљРІРѕС‚Р° РґРёСЃРїРµС‚С‡РµСЂР° (game-over РєСЂСЋС‡РѕРє) вЂ” РїРѕРєР°Р·С‹РІР°РµРј С‚РѕР»СЊРєРѕ РєРѕРіРґР° РІРєР»СЋС‡РµРЅР°
			if (Run->IsQuotaFailed())
			{
				TY += DrawCentered(TEXT("РљР’РћРўРђ РџР РћР’РђР›Р•РќРђ вЂ” РєРѕРЅС‚РѕСЂР° Р·Р°РєСЂС‹С‚Р°"), NegC, TY, 1.15f) + 4.f;
				DrawCentered(TEXT("[R] РќР°С‡Р°С‚СЊ РєР°СЂСЊРµСЂСѓ Р·Р°РЅРѕРІРѕ"), TextDim, TY, 1.05f);
			}
			else if (Run->IsQuotaActive())
			{
				const FString QLine = FString::Printf(TEXT("РљРІРѕС‚Р°: %d / %d в‚Ѕ   СЃСЂРѕРє вЂ” СЃРјРµРЅР° в„–%d"),
					Run->GetQuotaPaid(), Run->GetQuotaTarget(), Run->GetQuotaDeadlineShift());
				const bool bMet = Run->GetQuotaPaid() >= Run->GetQuotaTarget();
				TY += DrawCentered(QLine, bMet ? PosC : TextMain, TY, 1.05f) + 8.f;
				DrawCentered(TEXT("[R] РЎР»РµРґСѓСЋС‰Р°СЏ СЃРјРµРЅР°"), TextDim, TY, 1.05f);
			}
			else
			{
				DrawCentered(TEXT("[R] РЎР»РµРґСѓСЋС‰Р°СЏ СЃРјРµРЅР°"), TextDim, TY, 1.05f);
			}
		}
		else if (Run->AreAllObjectivesComplete() && Run->GetTotalObjectives() > 0)
		{
			DrawCentered(FAvLoc::S(TEXT("ui_031")), Accent, SizeY * 0.34f, 1.3f);
		}
	}

	// ---------- РњРёРЅРёРєР°СЂС‚Р° (СЃР»РµРІР°, РїРѕРґ С€РєР°Р»Р°РјРё) ----------
	{
		const float MapSize = 150.f;
		const float MapX = 30.f, MapY = 175.f;
		const float WorldHalf = 2100.f; // РїРѕР» РєР°СЂС‚С‹ В±2000 + Р·Р°РїР°СЃ

		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), MapX, MapY, MapSize, MapSize);
		DrawRect(AccentDim, MapX, MapY, MapSize, 2.f);
		DrawRect(AccentDim, MapX, MapY + MapSize - 2.f, MapSize, 2.f);
		DrawRect(AccentDim, MapX, MapY, 2.f, MapSize);
		DrawRect(AccentDim, MapX + MapSize - 2.f, MapY, 2.f, MapSize);

		// РњРёСЂ в†’ РјРёРЅРёРєР°СЂС‚Р°: СЃРµРІРµСЂ (X+) СЃРІРµСЂС…Сѓ, РІРѕСЃС‚РѕРє (Y+) СЃРїСЂР°РІР°
		auto WorldToMap = [&](const FVector& W, float& OutX, float& OutY)
		{
			OutX = MapX + FMath::Clamp((W.Y + WorldHalf) / (2.f * WorldHalf), 0.02f, 0.98f) * MapSize;
			OutY = MapY + FMath::Clamp((WorldHalf - W.X) / (2.f * WorldHalf), 0.02f, 0.98f) * MapSize;
		};
		auto DrawDot = [&](const FVector& W, const FLinearColor& Color, float Half)
		{
			float X = 0.f, Y = 0.f;
			WorldToMap(W, X, Y);
			DrawRect(Color, X - Half, Y - Half, Half * 2.f, Half * 2.f);
		};

		// Р—РѕРЅР° Р“РђР—РµР»Рё Рё Р±РёРѕС‚СѓР°Р»РµС‚С‹ вЂ” РёР· РєСЌС€Р° (СЃС‚Р°С‚РёС‡РЅС‹Рµ Р°РєС‚РѕСЂС‹, РЅРµ СЃРєР°РЅРёСЂСѓРµРј РјРёСЂ РєР°Р¶РґС‹Р№ РєР°РґСЂ, CODE_AUDIT3 #9)
		RefreshMinimapCache();
		for (const FVector& L : CachedExitZoneLocs)
		{
			DrawDot(L, Accent, 5.f);
		}
		for (const FVector& L : CachedToiletLocs)
		{
			DrawDot(L, FLinearColor(0.45f, 0.75f, 1.f), 3.f);
		}

		// Р—Р°РґР°С‡Рё: СЃР»РѕРјР°РЅРЅС‹Рµ РєСЂР°СЃРЅС‹Рј, РїРѕС‡РёРЅРµРЅРЅС‹Рµ Р·РµР»С‘РЅС‹Рј
		if (Run)
		{
			for (const ARepairable* Objective : Run->GetObjectives())
			{
				if (Objective)
				{
					DrawDot(Objective->GetActorLocation(),
						Objective->IsBroken() ? FLinearColor(0.95f, 0.2f, 0.2f) : FLinearColor(0.3f, 0.85f, 0.3f), 3.f);
				}
			}
		}

		// Р‘СЂРёРіР°РґР°: С‚РёРјРјРµР№С‚С‹ Р±РµР»С‹Рј (СЂР°РЅРµРЅС‹Рµ РєСЂР°СЃРЅС‹Рј), СЏ вЂ” РѕСЂР°РЅР¶РµРІС‹Р№ СЃ РЅР°РїСЂР°РІР»РµРЅРёРµРј РІР·РіР»СЏРґР°
		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			if (*It == Character)
			{
				continue;
			}
			const bool bCrewWounded = It->VitalsComponent && It->VitalsComponent->IsWounded();
			DrawDot(It->GetActorLocation(), bCrewWounded ? FLinearColor(1.f, 0.3f, 0.3f) : TextMain, 2.5f);
		}

		float SelfX = 0.f, SelfY = 0.f;
		WorldToMap(Character->GetActorLocation(), SelfX, SelfY);
		DrawRect(Accent, SelfX - 3.f, SelfY - 3.f, 6.f, 6.f);
		const FVector Forward = Character->GetControlRotation().Vector();
		Draw2DLine(FMath::RoundToInt(SelfX), FMath::RoundToInt(SelfY),
			FMath::RoundToInt(SelfX + Forward.Y * 10.f), FMath::RoundToInt(SelfY - Forward.X * 10.f),
			Accent.ToFColor(true));
	}

	// ---------- Р‘Р°РЅРЅРµСЂ СЂР°РЅРµРЅРёСЏ + РѕР±СЂР°С‚РЅС‹Р№ РѕС‚СЃС‡С‘С‚ bleed-out (РєРѕРѕРї-СЃРїР°СЃРµРЅРёРµ Р±РѕР»СЊС€Рµ РЅРµ РІСЃР»РµРїСѓСЋ) ----------
	if (Vitals && Vitals->IsUnconscious())
	{
		DrawCentered(FAvLoc::S(TEXT("ui_032")), FLinearColor(0.95f, 0.2f, 0.2f), SizeY * 0.4f, 1.6f);
	}
	else if (Vitals && Vitals->IsWounded())
	{
		const int32 Secs = FMath::CeilToInt(Vitals->GetBleedOutSeconds());
		DrawCentered(FAvLoc::S(TEXT("ui_033"))
			+ FString::FromInt(Secs)
			+ FAvLoc::S(TEXT("ui_034")),
			FLinearColor::Red, SizeY * 0.4f, 1.6f);
		// РџРѕР»РѕСЃРєР° РѕРєРЅР° РїРѕРґСЉС‘РјР°: РёСЃС‚РµРєР°РµС‚ РїРѕ РјРµСЂРµ РїСЂРёР±Р»РёР¶РµРЅРёСЏ Рє РѕС‚СЂСѓР±Сѓ
		const float BarW = FMath::Min(420.f, SizeX * 0.4f);
		const float BarX = (SizeX - BarW) * 0.5f;
		const float BarY = SizeY * 0.4f + 30.f;
		DrawRect(AvaryoHUDStyle::BarBG, BarX, BarY, BarW, 10.f);
		DrawRect(FLinearColor(0.95f, 0.2f, 0.2f), BarX, BarY, BarW * Vitals->GetBleedOut01(), 10.f);
	}

	// ---------- РџСЂРёРјРµРЅРµРЅРёРµ РїСЂРµРґРјРµС‚Р° (РїРѕ С†РµРЅС‚СЂСѓ, РєР°Рє РІ СЂРµС„РµСЂРµРЅСЃРµ) ----------
	if (Character->IsUsingItem())
	{
		APickupItem* Casting = Character->GetHeldItem();
		const int32 Percent = FMath::RoundToInt(Character->GetUseProgress() * 100.f);
		DrawCastBar(FString::Printf(TEXT("РџСЂРёРјРµРЅРµРЅРёРµ: %s  %d%%"),
			Casting ? *Casting->DisplayName.ToString() : TEXT("..."), Percent),
			Character->GetUseProgress(), BarFill);
	}
	// ---------- РњРёРЅРё-РёРіСЂР° Р±РёРѕС‚СѓР°Р»РµС‚Р° ----------
	else if (Character->IsUsingToilet() && Character->GetCurrentToilet() && Vitals)
	{
		AToilet* T = Character->GetCurrentToilet();
		const float BarW = 340.f, BarH = 18.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		float BoxY = SizeY * 0.42f;

		const FString Label = FString::Printf(TEXT("%s %d%%"), *FAvLoc::S(TEXT("ui_035")), FMath::RoundToInt(Vitals->GetBladder()));
		float LabelW = 0.f, LabelH = 0.f;
		GetTextSize(Label, LabelW, LabelH, Font, 1.1f);

		// РџР»Р°С€РєР° РЅР° РѕР±Рµ РїРѕР»РѕСЃРєРё Рё РїРѕРґСЃРєР°Р·РєСѓ
		DrawRect(BoxBG, BoxX - 12.f, BoxY - LabelH - 10.f, BarW + 24.f, LabelH + BarH * 2.f + 56.f);
		DrawText(Label, TextMain, BoxX, BoxY - LabelH - 4.f, Font, 1.1f);

		// Р’РµСЂС…РЅСЏСЏ РїРѕР»РѕСЃРєР°: СЃРєРѕР»СЊРєРѕ В«СЃРѕРґРµСЂР¶РёРјРѕРіРѕВ» РѕСЃС‚Р°Р»РѕСЃСЊ
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(FLinearColor(0.65f, 0.45f, 0.15f), BoxX, BoxY + 2.f, BarW * FMath::Clamp(Vitals->GetBladder(), 0.f, 100.f) / 100.f, BarH);

		// РќРёР¶РЅСЏСЏ РїРѕР»РѕСЃРєР°: РјРёРЅРё-РёРіСЂР° вЂ” РєСЂР°СЃРЅС‹Р№ С„РѕРЅ, РѕС‚РґРµР»СЊРЅС‹Рµ Р¶С‘Р»С‚Р°СЏ Рё Р·РµР»С‘РЅР°СЏ Р·РѕРЅС‹, Р±РµР»С‹Р№ РєСѓСЂСЃРѕСЂ
		BoxY += BarH + 12.f;
		DrawRect(FLinearColor(0.55f, 0.12f, 0.1f), BoxX, BoxY + 2.f, BarW, BarH);
		const float Yellow = T->GetYellowCenter();
		const float YellowL = FMath::Clamp(Yellow - T->YellowHalfWidth, 0.f, 1.f);
		const float YellowR = FMath::Clamp(Yellow + T->YellowHalfWidth, 0.f, 1.f);
		DrawRect(FLinearColor(0.8f, 0.7f, 0.15f), BoxX + YellowL * BarW, BoxY + 2.f, (YellowR - YellowL) * BarW, BarH);
		const float Green = T->GetGreenCenter();
		const float GreenL = FMath::Clamp(Green - T->GreenHalfWidth, 0.f, 1.f);
		const float GreenR = FMath::Clamp(Green + T->GreenHalfWidth, 0.f, 1.f);
		DrawRect(FLinearColor(0.25f, 0.8f, 0.25f), BoxX + GreenL * BarW, BoxY + 2.f, (GreenR - GreenL) * BarW, BarH);
		DrawRect(TextMain, BoxX + T->GetCursorPos() * BarW - 2.f, BoxY - 2.f, 4.f, BarH + 8.f);

		DrawText(FAvLoc::S(TEXT("ui_036")), TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
	}
	// ---------- РњРёРЅРё-РёРіСЂС‹ РїРѕС‡РёРЅРєРё (С‰РёС‚РѕРє/РІРµРЅС‚РёР»СЊ/СЃС‚Р°СЂС‚РµСЂ) вЂ” С‚РѕР»СЊРєРѕ РєРѕРіРґР° РІСЃРµ РїРѕРґРіРѕС‚РѕРІРёС‚РµР»СЊРЅС‹Рµ СЌС‚Р°РїС‹ РїСЂРѕР№РґРµРЅС‹ ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable()
		&& Character->GetCurrentRepairable()->IsMinigameRepair() && Character->GetCurrentRepairable()->ArePrereqsDone()
		&& !Character->GetCurrentRepairable()->IsBotching())
	{
		ARepairable* R = Character->GetCurrentRepairable();
		const float BarW = 340.f, BarH = 18.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		float BoxY = SizeY * 0.42f;

		FString Label;
		switch (R->GetMinigameType())
		{
		case ERepairMinigameType::Valve:
			Label = FString::Printf(TEXT("Р’РµРЅС‚РёР»СЊ: %s  %d%%"),
				*R->DisplayName.ToString(), FMath::RoundToInt(R->GetRepairProgress() * 100.f));
			break;
		case ERepairMinigameType::Starter:
			Label = FString::Printf(TEXT("РЎС‚Р°СЂС‚РµСЂ: %s  %d%%"),
				*R->DisplayName.ToString(), FMath::RoundToInt(R->GetRepairProgress() * 100.f));
			break;
		default:
			Label = FString::Printf(TEXT("Р РµРјРѕРЅС‚: %s  %d%%   РїСЂРѕРјР°С…Рё %d/%d"),
				*R->DisplayName.ToString(), FMath::RoundToInt(R->GetRepairProgress() * 100.f),
				R->GetMissCount(), R->MissesBeforeLockout);
			break;
		}
		float LabelW = 0.f, LabelH = 0.f;
		GetTextSize(Label, LabelW, LabelH, Font, 1.1f);

		DrawRect(BoxBG, BoxX - 12.f, BoxY - LabelH - 10.f, BarW + 24.f, LabelH + BarH * 2.f + 56.f);
		DrawText(Label, TextMain, BoxX, BoxY - LabelH - 4.f, Font, 1.1f);

		// Р’РµСЂС…РЅСЏСЏ РїРѕР»РѕСЃРєР°: РїСЂРѕРіСЂРµСЃСЃ РїРѕС‡РёРЅРєРё
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(Accent, BoxX, BoxY + 2.f, BarW * R->GetRepairProgress(), BarH);

		// РќРёР¶РЅСЏСЏ РїРѕР»РѕСЃРєР° вЂ” СЃРІРѕСЏ РЅР° РєР°Р¶РґС‹Р№ СЂРµР¶РёРј
		BoxY += BarH + 12.f;
		if (R->GetMinigameType() == ERepairMinigameType::Valve)
		{
			// Р РёС‚Рј: РїРѕР»РѕСЃРєР° РЅР°РїРѕР»РЅСЏРµС‚СЃСЏ Рє В«Р±РµР·РѕРїР°СЃРЅРѕ С‚С‹РєР°С‚СЊВ»; С‚С‹Рє РґРѕ Р·РµР»С‘РЅРѕРіРѕ = СЃСЂС‹РІ СЂРµР·СЊР±С‹
			const float Ready = 1.f - FMath::Clamp(R->GetValveCooldown() / FMath::Max(R->ValveMinInterval, 0.01f), 0.f, 1.f);
			DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
			DrawRect(Ready >= 1.f ? FLinearColor(0.25f, 0.8f, 0.25f) : FLinearColor(0.55f, 0.12f, 0.1f),
				BoxX, BoxY + 2.f, BarW * Ready, BarH);
			DrawText(FAvLoc::S(TEXT("ui_037")),
				TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
		}
		else if (R->GetMinigameType() == ERepairMinigameType::Starter)
		{
			// РќР°С‚СЏР¶РµРЅРёРµ С€РЅСѓСЂР°: Р·РµР»С‘РЅРѕРµ РѕРєРЅРѕ, Р±РµР»Р°СЏ РїРѕР»РѕСЃР° СЂР°СЃС‚С‘С‚ РїРѕРєР° РґРµСЂР¶РёС€СЊ E
			DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
			float WStart, WEnd;
			R->GetEffectiveStarterWindow(WStart, WEnd); // РѕРєРЅРѕ РєР°Рє Сѓ СЃРµСЂРІРµСЂР° (РїР°РЅРёРєР°+РёРЅСЃС‚СЂСѓРјРµРЅС‚)
			const float WinL = FMath::Clamp(WStart, 0.f, 1.f);
			const float WinR = FMath::Clamp(WEnd, 0.f, 1.f);
			DrawRect(FLinearColor(0.25f, 0.8f, 0.25f), BoxX + WinL * BarW, BoxY + 2.f, (WinR - WinL) * BarW, BarH);
			const float Tension = R->GetStarterTension();
			DrawRect(FLinearColor(0.9f, 0.9f, 0.9f), BoxX, BoxY + 4.f, BarW * Tension, BarH - 4.f);
			DrawRect(TextMain, BoxX + Tension * BarW - 2.f, BoxY - 2.f, 4.f, BarH + 8.f);
			DrawText(R->IsStarterPulling()
				? FAvLoc::S(TEXT("ui_038"))
				: FAvLoc::S(TEXT("ui_039")),
				TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
		}
		else
		{
			// Р©РёС‚РѕРє: РєСЂР°СЃРЅС‹Р№ С„РѕРЅ, С…Р°РѕС‚РёС‡РЅР°СЏ Р·РµР»С‘РЅР°СЏ Р·РѕРЅР°, Р±РµР»С‹Р№ РєСѓСЂСЃРѕСЂ
			DrawRect(FLinearColor(0.55f, 0.12f, 0.1f), BoxX, BoxY + 2.f, BarW, BarH);
			const float Green = R->GetGreenCenter();
			const float HalfW = R->GetEffectiveGreenHalf(); // С‚Р° Р¶Рµ Р·РѕРЅР°, С‡С‚Рѕ РїСЂРёРЅРёРјР°РµС‚ СЃРµСЂРІРµСЂ (РїР°РЅРёРєР°+РёРЅСЃС‚СЂСѓРјРµРЅС‚)
			const float GreenL = FMath::Clamp(Green - HalfW, 0.f, 1.f);
			const float GreenR = FMath::Clamp(Green + HalfW, 0.f, 1.f);
			DrawRect(FLinearColor(0.25f, 0.8f, 0.25f), BoxX + GreenL * BarW, BoxY + 2.f, (GreenR - GreenL) * BarW, BarH);
			DrawRect(TextMain, BoxX + R->GetCursorPos() * BarW - 2.f, BoxY - 2.f, 4.f, BarH + 8.f);
			DrawText(FAvLoc::S(TEXT("ui_040")), TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
		}
	}
	// ---------- РџРѕРґРіРѕС‚РѕРІРёС‚РµР»СЊРЅР°СЏ РјРёРЅРё-РёРіСЂР° (Р·Р°РІР°СЂРєР° / РїРѕС‡РёРЅРєР° СЂСѓРєР°РјРё вЂ” РєСѓСЂСЃРѕСЂ СЃ РѕС‚РєР°С‚РѕРј) ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable()
		&& Character->GetCurrentRepairable()->IsDoingPrereqMinigame())
	{
		ARepairable* R = Character->GetCurrentRepairable();
		FRepairStage St;
		R->GetCurrentStage(St);
		const FString Label = FString::Printf(TEXT("%s  %d%%"),
			St.Label.IsEmpty() ? TEXT("Р­С‚Р°Рї") : *St.Label.ToString(), FMath::RoundToInt(R->GetPrereqProgress() * 100.f));
		const float BarW = 340.f, BarH = 18.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		float BoxY = SizeY * 0.42f;
		float LW = 0.f, LH = 0.f;
		GetTextSize(Label, LW, LH, Font, 1.1f);
		DrawRect(BoxBG, BoxX - 12.f, BoxY - LH - 10.f, BarW + 24.f, LH + BarH * 2.f + 56.f);
		DrawText(Label, TextMain, BoxX, BoxY - LH - 4.f, Font, 1.1f);
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(Accent, BoxX, BoxY + 2.f, BarW * R->GetPrereqProgress(), BarH);
		BoxY += BarH + 12.f;
		DrawRect(FLinearColor(0.55f, 0.12f, 0.1f), BoxX, BoxY + 2.f, BarW, BarH);
		const float Green = R->GetGreenCenter();
		const float HalfW = R->GetEffectiveGreenHalf(); // СЃРѕРІРїР°РґР°РµС‚ СЃ СЃРµСЂРІРµСЂРЅРѕР№ Р·РѕРЅРѕР№ (РїР°РЅРёРєР°+РёРЅСЃС‚СЂСѓРјРµРЅС‚)
		const float GreenL = FMath::Clamp(Green - HalfW, 0.f, 1.f);
		const float GreenR = FMath::Clamp(Green + HalfW, 0.f, 1.f);
		DrawRect(FLinearColor(0.25f, 0.8f, 0.25f), BoxX + GreenL * BarW, BoxY + 2.f, (GreenR - GreenL) * BarW, BarH);
		DrawRect(TextMain, BoxX + R->GetCursorPos() * BarW - 2.f, BoxY - 2.f, 4.f, BarH + 8.f);
		DrawText(FAvLoc::S(TEXT("ui_041")), TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
	}
	// ---------- РџРѕРґРіРѕС‚РѕРІРёС‚РµР»СЊРЅС‹Р№ СЌС‚Р°Рї: РґРµСЂР¶Р°С‚СЊ E (РїРѕС‡РёРЅРєР° СЂСѓРєР°РјРё) РёР»Рё AutoFill (СѓСЃС‚Р°РЅРѕРІРєР° РєР°Р±РµР»СЏ) ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable()
		&& !Character->GetCurrentRepairable()->ArePrereqsDone())
	{
		ARepairable* Rep = Character->GetCurrentRepairable();
		FRepairStage St;
		const FString StepName = (Rep->GetCurrentStage(St) && !St.Label.IsEmpty()) ? St.Label.ToString() : TEXT("Подготовка");
		DrawCastBar(Rep->IsAutoFilling() ? FString::Printf(TEXT("%s (установка...)"), *StepName) : StepName,
			Rep->GetPrereqProgress(), Accent);
	}
	// ---------- РћСЃРЅРѕРІРЅР°СЏ РїРѕС‡РёРЅРєР° (РґРµСЂР¶РёС‚ E Сѓ РѕР±СЉРµРєС‚Р°) ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable())
	{
		ARepairable* Repairing = Character->GetCurrentRepairable();
		const int32 Percent = FMath::RoundToInt(Repairing->GetRepairProgress() * 100.f);
		DrawCastBar(FString::Printf(TEXT("Ремонт: %s  %d%%"), *Repairing->DisplayName.ToString(), Percent),
			Repairing->GetRepairProgress(), Accent);
	}
	// ---------- РџРѕРґСЃРєР°Р·РєР° РїРѕРґР±РѕСЂР° (РїР»Р°С€РєР° РєР°Рє РІ СЂРµС„РµСЂРµРЅСЃРµ) ----------
	else if (ACallBoard* Board = Character->GetFocusedCallBoard())
		{
			const TArray<FCallListing>& BC = Board->GetCalls();
			const int32 Sel = Board->GetSelectedIndex();
			const FString Title = BC.IsValidIndex(Sel) ? BC[Sel].Title : FString(TEXT("заявка"));
			DrawPromptBox(FString::Printf(TEXT("[E] Взять заявку: %s"), *Title));
		}
		else if (Character->GetFocusedToolCase())
		{
			DrawPromptBox(FAvLoc::S(TEXT("ui_042")));
		}
		else if (APickupItem* Focused = Character->GetFocusedItem())
	{
		DrawPromptBox(FString::Printf(TEXT("[E] Поднять %s"), *Focused->DisplayName.ToString()));
	}
	// ---------- РџРѕРґСЃРєР°Р·РєР° РїРѕС‡РёРЅРєРё ----------
	else if (ARepairable* FocusedRep = Character->GetFocusedRepairable())
	{
		FString Prompt;
		if (FocusedRep->IsOnFire())
		{
			Prompt = FString::Printf(TEXT("%s — ГОРИТ! Потуши огнетушителем"), *FocusedRep->DisplayName.ToString());
		}
		else if (FocusedRep->GetLockoutRemaining() > 0.f)
		{
			Prompt = FString::Printf(TEXT("%s замкнуло — подождите %d с"),
				*FocusedRep->DisplayName.ToString(), FMath::CeilToInt(FocusedRep->GetLockoutRemaining()));
		}
		else if (!FocusedRep->ArePrereqsDone())
		{
			// РџРѕРґРіРѕС‚РѕРІРёС‚РµР»СЊРЅС‹Р№ СЌС‚Р°Рї: РїРѕРґСЃРєР°Р·С‹РІР°РµРј, С‡С‚Рѕ РґРµР»Р°С‚СЊ (Р·Р°РІР°СЂРёС‚СЊ / РїРѕРґРєР»СЋС‡РёС‚СЊ РєР°Р±РµР»СЊ / РІСЃС‚Р°РІРёС‚СЊ...)
			FRepairStage St;
			FocusedRep->GetCurrentStage(St);
			const FString Step = St.Label.IsEmpty() ? TEXT("Подготовка") : St.Label.ToString();
			const APickupItem* Held = Character->GetHeldItem();
			const bool bNeedItem = (St.Kind == ERepairStageKind::HoldTool
				|| St.Kind == ERepairStageKind::InsertItem
				|| St.Kind == ERepairStageKind::AutoFill
				|| (St.Kind == ERepairStageKind::Minigame && !St.ItemTag.IsNone()));
			const bool bHasItem = Held && Held->ToolTag == St.ItemTag;
			if (bNeedItem && !bHasItem)
			{
				Prompt = FString::Printf(TEXT("%s — нужен в руках: %s"), *Step, *St.ItemTag.ToString());
			}
			else if (St.Kind == ERepairStageKind::Minigame)
			{
				Prompt = FString::Printf(TEXT("[E] %s (мини-игра)"), *Step);
			}
			else if (St.Kind == ERepairStageKind::HoldHand || St.Kind == ERepairStageKind::HoldTool)
			{
				Prompt = FString::Printf(TEXT("[E] %s (держать)"), *Step);
			}
			else
			{
				Prompt = FString::Printf(TEXT("[E] %s"), *Step); // InsertItem / AutoFill
			}
		}
		else if (FocusedRep->CanBeRepairedBy(Character))
		{
			if (FocusedRep->GetMinigameType() == ERepairMinigameType::Valve)
			{
				Prompt = FString::Printf(TEXT("[E] Закрутить вентиль: %s (жми размеренно!)"), *FocusedRep->DisplayName.ToString());
			}
			else if (FocusedRep->GetMinigameType() == ERepairMinigameType::Starter)
			{
				Prompt = FString::Printf(TEXT("[E] Заводить %s (держать и отпустить вовремя)"), *FocusedRep->DisplayName.ToString());
			}
			else if (FocusedRep->IsMinigameRepair())
			{
				Prompt = FString::Printf(TEXT("[E] Чинить %s (мини-игра, нужен тестер)"), *FocusedRep->DisplayName.ToString());
			}
			else
			{
				Prompt = FocusedRep->GetRepairProgress() > 0.f
					? FString::Printf(TEXT("[E] Дочинить %s (%d%%)"), *FocusedRep->DisplayName.ToString(),
						FMath::RoundToInt(FocusedRep->GetRepairProgress() * 100.f))
					: FString::Printf(TEXT("[E] Чинить %s (держать)"), *FocusedRep->DisplayName.ToString());
			}
		}
		else if (FocusedRep->IsBeingRepaired())
		{
			Prompt = FString::Printf(TEXT("%s уже чинят"), *FocusedRep->DisplayName.ToString());
		}
		else
		{
			Prompt = FString::Printf(TEXT("Для «%s» нужен инструмент: %s"),
				*FocusedRep->DisplayName.ToString(), *FocusedRep->RequiredTool.ToString());
		}
		DrawPromptBox(Prompt);
	}
	// ---------- РџРѕРґСЃРєР°Р·РєР° Р±РёРѕС‚СѓР°Р»РµС‚Р° ----------
	else if (AToilet* Toilet = Character->GetFocusedToilet())
	{
		FString ToiletPrompt;
		if (Toilet->IsOccupied() && Toilet->GetOccupant() != Character)
		{
			ToiletPrompt = FAvLoc::S(TEXT("ui_043"));
		}
		else if (Toilet->CanUseBy(Character))
		{
			ToiletPrompt = FAvLoc::S(TEXT("ui_044"));
		}
		else
		{
			ToiletPrompt = FAvLoc::S(TEXT("ui_045"));
		}
		DrawPromptBox(ToiletPrompt);
	}
	// ---------- РџРѕРґСЃРєР°Р·РєР° РґСЂР°РіР° СЂР°РЅРµРЅРѕРіРѕ ----------
	else if (Character->GetFocusedWounded() && !Character->IsDragging())
	{
		DrawPromptBox(Character->IsCarryingHeavy()
			? FAvLoc::S(TEXT("ui_046"))
			: FAvLoc::S(TEXT("ui_047")));
	}

	// ---------- РџРѕРґСЃРєР°Р·РєРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРёСЏ / РїРµСЂРµРґР°С‡Рё ----------
	if (!Character->IsUsingItem())
	{
		if (Character->IsOffering())
		{
			APickupItem* Held = Character->GetHeldItem();
			const FString OfferText = FAvLoc::S(TEXT("ui_048"))
				+ (Held ? Held->DisplayName.ToString() : FString(TEXT("...")))
				+ FAvLoc::S(TEXT("ui_049"));
			DrawCentered(OfferText, Accent, SizeY * 0.62f, 1.15f);
		}
		else if (APickupItem* Held = Character->GetHeldItem())
		{
			FString Hint;
			if (Held->ItemEffect == EItemEffect::Extinguish)
			{
				Hint = FAvLoc::S(TEXT("ui_050"));
			}
			else if (Held->ItemEffect == EItemEffect::Radio)
			{
				Hint = Held->IsToggledOn()
					? FAvLoc::S(TEXT("ui_051"))
					: FAvLoc::S(TEXT("ui_052"));
			}
			else if (Held->ItemEffect == EItemEffect::DeployTrap)
			{
				Hint = FAvLoc::S(TEXT("ui_053"));
			}
			else if (Held->ItemEffect == EItemEffect::ThrowBio)
			{
				Hint = FAvLoc::S(TEXT("ui_054"));
			}
			else if (Held->ItemEffect == EItemEffect::DeployLight)
			{
				Hint = FAvLoc::S(TEXT("ui_055"));
			}
			else if (Held->ItemEffect != EItemEffect::None)
			{
				Hint = FAvLoc::S(TEXT("ui_056"));
			}
			else
			{
				Hint = FAvLoc::S(TEXT("ui_057"));
			}
			DrawCentered(Hint, TextDim, SizeY * 0.66f, 0.95f);
		}
	}

	// ---------- РќРёР¶РЅСЏСЏ РїР°РЅРµР»СЊ РёРЅРІРµРЅС‚Р°СЂСЏ (РєР°Рє РІ СЂРµС„РµСЂРµРЅСЃРµ) ----------
	const float PanelH = 50.f;
	const float PanelY = SizeY - PanelH;
	DrawRect(PanelBG, 0.f, PanelY, SizeX, PanelH);
	DrawRect(AccentDim, 0.f, PanelY, SizeX, 2.f); // С‚РѕРЅРєР°СЏ Р°РєС†РµРЅС‚РЅР°СЏ РєСЂРѕРјРєР°

	const float CellW = SizeX / AAvaryoCharacter::NumSlots;
	for (int32 SlotIndex = 0; SlotIndex < AAvaryoCharacter::NumSlots; ++SlotIndex)
	{
		APickupItem* Item = Character->GetItemInSlot(SlotIndex);
		const bool bActive = Character->GetActiveSlot() == SlotIndex;
		const float CellX = SlotIndex * CellW;

		// Р¤РѕРЅ РєР°Р¶РґРѕР№ СЏС‡РµР№РєРё (РѕС‚РґРµР»СЏРµС‚ СЃР»РѕС‚С‹); Р°РєС‚РёРІРЅР°СЏ вЂ” РѕСЂР°РЅР¶РµРІРѕР№ Р РђРњРљРћР™, РЅРµ СЃРїР»РѕС€РЅРѕР№ Р·Р°Р»РёРІРєРѕР№.
		DrawRect(CellBG, CellX + 4.f, PanelY + 4.f, CellW - 8.f, PanelH - 8.f);
		if (bActive)
		{
			const float fx = CellX + 4.f, fy = PanelY + 4.f, fw = CellW - 8.f, fh = PanelH - 8.f;
			DrawRect(Accent, fx, fy, fw, 2.f);
			DrawRect(Accent, fx, fy + fh - 2.f, fw, 2.f);
			DrawRect(Accent, fx, fy, 2.f, fh);
			DrawRect(Accent, fx + fw - 2.f, fy, 2.f, fh);
		}

		const int32 SlotNum = SlotIndex == 0 ? 1 : SlotIndex + 1;
		if (Item && Item->Icon)
		{
			// РРєРѕРЅРєР° РїСЂРµРґРјРµС‚Р° РїРѕ С†РµРЅС‚СЂСѓ СЏС‡РµР№РєРё; РёРЅРґРµРєСЃ/Р·Р°СЂСЏРґС‹ вЂ” РјРµР»РєРёРј РїРѕ СѓРіР»Р°Рј.
			const float Pad = 6.f;
			const float IconSz = FMath::Min(CellW - Pad * 2.f, PanelH - Pad * 2.f);
			const float IconX = CellX + (CellW - IconSz) * 0.5f;
			const float IconY = PanelY + (PanelH - IconSz) * 0.5f;
			DrawTexture(Item->Icon, IconX, IconY, IconSz, IconSz, 0.f, 0.f, 1.f, 1.f,
				bActive ? FLinearColor::White : FLinearColor(0.65f, 0.65f, 0.65f));
			DrawText(FString::Printf(TEXT("%d"), SlotNum), bActive ? TextMain : TextDim, CellX + 6.f, PanelY + 4.f, Font, 0.8f);
			if (Item->Charges >= 0)
			{
				DrawText(FString::Printf(TEXT("%d"), Item->Charges), TextDim, CellX + CellW - 24.f, PanelY + 4.f, Font, 0.8f);
			}
			else if (Item->IsToggledOn())
			{
				DrawText(FAvLoc::S(TEXT("ui_058")), Accent, CellX + CellW - 38.f, PanelY + 4.f, Font, 0.7f);
			}
		}
		else
		{
			FString ItemName;
			if (Item)
			{
				ItemName = Item->DisplayName.ToString();
				if (Item->Charges >= 0)
				{
					ItemName += FString::Printf(TEXT(" (%d)"), Item->Charges);
				}
				if (Item->IsToggledOn())
				{
					ItemName += FAvLoc::S(TEXT("ui_059"));
				}
			}
			else
			{
				ItemName = SlotIndex == 0 ? FAvLoc::S(TEXT("ui_060")) : FAvLoc::S(TEXT("ui_061"));
			}

			const FString Label = SlotIndex == 0
				? FString::Printf(TEXT("[1] %s: %s"), *FAvLoc::S(TEXT("ui_062")), *ItemName)
				: FString::Printf(TEXT("[%d] %s"), SlotIndex + 1, *ItemName);

			float W = 0.f, H = 0.f;
			const float Scale = 1.0f;
			GetTextSize(Label, W, H, Font, Scale);
			DrawText(Label, bActive ? TextMain : TextDim, CellX + (CellW - W) * 0.5f, PanelY + (PanelH - H) * 0.5f, Font, Scale);
		}
	}

	// ---------- РЎС‚Р°С‚СѓСЃС‹ (СЃРїСЂР°РІР°, РЅР°Рґ РїР°РЅРµР»СЊСЋ) ----------
	if (Vitals)
	{
		TArray<FString> Statuses;
		if (Vitals->IsUnconscious())      Statuses.Add(FAvLoc::S(TEXT("ui_063")));
		else if (Vitals->IsWounded())     Statuses.Add(FAvLoc::S(TEXT("ui_064")));
		if (Vitals->IsSmoking())          Statuses.Add(FAvLoc::S(TEXT("ui_065")));
		if (Vitals->IsPanicking())        Statuses.Add(FAvLoc::S(TEXT("ui_066")));
		if (Vitals->GetBladder() > 70.f)  Statuses.Add(FAvLoc::S(TEXT("ui_067")));
		if (Vitals->GetStamina() < 20.f)  Statuses.Add(FAvLoc::S(TEXT("ui_068")));
		if (Character->IsCarryingHeavy()) Statuses.Add(FAvLoc::S(TEXT("ui_069")));
		if (Character->IsDragging())      Statuses.Add(FAvLoc::S(TEXT("ui_070")));
		if (Character->GetDraggedBy())    Statuses.Add(FAvLoc::S(TEXT("ui_071")));
		if (Vitals->IsIncidentSlowed())   Statuses.Add(FAvLoc::S(TEXT("ui_072")));
		else if (Vitals->IsSoiled())      Statuses.Add(FAvLoc::S(TEXT("ui_073")));
		if (Vitals->IsWet())              Statuses.Add(FAvLoc::S(TEXT("ui_074")));
		if (Vitals->IsBurning())          Statuses.Add(FAvLoc::S(TEXT("ui_075")));
		if (Vitals->IsSmelly())           Statuses.Add(FAvLoc::S(TEXT("ui_076")));

		// Р’ РіР°Р·РѕРІРѕРј РѕР±Р»Р°РєРµ вЂ” РЅРµ РєСѓСЂРёС‚СЊ! (Р±РµСЂС‘Рј РёР· РєСЌС€Р° Р·Р°РґР°С‡ RunState, Р±РµР· РѕР±С…РѕРґР° РІСЃРµС… Р°РєС‚РѕСЂРѕРІ РєР°Р¶РґС‹Р№ РєР°РґСЂ)
		if (ARunState* GasRun = Run)
		{
			for (const ARepairable* Obj : GasRun->GetObjectives())
			{
				if (Obj && Obj->IsLeakingGas()
					&& FVector::DistSquared(Character->GetActorLocation(), Obj->GetActorLocation()) <= FMath::Square(Obj->GetCurrentGasRadius())) // РІС‹СЂРѕСЃС€РµРµ РѕР±Р»Р°РєРѕ, РЅРµ СЃС‚Р°С‚РёРє (CODE_AUDIT3 #4)
				{
					Statuses.Add(FAvLoc::S(TEXT("ui_077")));
					break;
				}
			}
		}

		float StatusY = PanelY - Statuses.Num() * 22.f - 12.f;
		for (const FString& Status : Statuses)
		{
			float W = 0.f, H = 0.f;
			GetTextSize(Status, W, H, Font, 1.0f);
			DrawText(Status, FLinearColor(1.f, 0.55f, 0.2f), SizeX - W - 30.f, StatusY, Font, 1.0f);
			StatusY += 22.f;
		}
	}
}
