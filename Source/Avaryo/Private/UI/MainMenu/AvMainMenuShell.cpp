#include "UI/MainMenu/AvMainMenuShell.h"

#include "Algo/Unique.h"
#include "AvariikaLoc.h"
#include "AudioDevice.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "GenericPlatform/GenericApplication.h"
#include "Game/AvariikaSaveGame.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Internationalization/Internationalization.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Styling/CoreStyle.h"
#include "Misc/Paths.h"

namespace AvMainMenu
{
	constexpr int32 ContinueAction = 0;
	constexpr int32 NewGameAction = 1;
	constexpr int32 MultiplayerAction = 2;
	constexpr int32 SettingsAction = 3;
	constexpr int32 CreditsAction = 4;
	constexpr int32 ExitAction = 5;
	constexpr int32 SettingsApplyAction = 200;
	constexpr int32 SettingsRevertAction = 201;
	constexpr int32 SettingsBackAction = 202;
	constexpr int32 CreditsBackAction = 210;
	constexpr int32 ModalYesAction = 300;
	constexpr int32 ModalNoAction = 301;
	constexpr int32 SettingAdjustmentBase = 1000;

	const FLinearColor Background(0.008f, 0.011f, 0.014f, 1.f);
	const FLinearColor Panel(0.025f, 0.031f, 0.038f, 0.97f);
	const FLinearColor Idle(0.045f, 0.052f, 0.061f, 1.f);
	const FLinearColor Hover(0.18f, 0.075f, 0.015f, 1.f);
	const FLinearColor Focus(0.34f, 0.125f, 0.02f, 1.f);
	const FLinearColor Pressed(0.58f, 0.22f, 0.025f, 1.f);
	const FLinearColor Disabled(0.025f, 0.028f, 0.032f, 0.86f);
	const FLinearColor Orange(1.f, 0.36f, 0.055f, 1.f);
	const FLinearColor OrangeSoft(1.f, 0.54f, 0.22f, 1.f);
	const FLinearColor Text(0.91f, 0.93f, 0.95f, 1.f);
	const FLinearColor Muted(0.46f, 0.5f, 0.54f, 1.f);
	const FLinearColor DisabledText(0.28f, 0.31f, 0.34f, 1.f);

	FSlateBrush Brush(const FLinearColor& Color)
	{
		FSlateBrush Result;
		Result.DrawAs = ESlateBrushDrawType::Box;
		Result.TintColor = FSlateColor(Color);
		Result.Margin = FMargin(0.f);
		return Result;
	}

	FButtonStyle ButtonStyle(const FLinearColor& Normal, const FLinearColor& Hovered,
		const FLinearColor& InPressed, const FLinearColor& InDisabled)
	{
		FButtonStyle Style;
		Style.SetNormal(Brush(Normal));
		Style.SetHovered(Brush(Hovered));
		Style.SetPressed(Brush(InPressed));
		Style.SetDisabled(Brush(InDisabled));
		Style.SetNormalPadding(FMargin(18.f, 10.f));
		Style.SetPressedPadding(FMargin(18.f, 11.f, 18.f, 9.f));
		return Style;
	}

	FSlateFontInfo Font(const int32 Size, const bool bBold = false)
	{
		return FCoreStyle::GetDefaultFontStyle(bBold ? TEXT("Bold") : TEXT("Regular"), Size);
	}

	void ConfigureText(UTextBlock* TextBlock, const int32 Size, const FLinearColor& Color,
		const bool bBold = false)
	{
		if (!TextBlock)
		{
			return;
		}
		TextBlock->SetFont(Font(Size, bBold));
		TextBlock->SetColorAndOpacity(FSlateColor(Color));
		TextBlock->SetAutoWrapText(true);
	}

	FString WindowModeKey(const int32 Mode)
	{
		switch (static_cast<EWindowMode::Type>(Mode))
		{
		case EWindowMode::Fullscreen: return TEXT("MainMenu.Settings.WindowMode.Fullscreen");
		case EWindowMode::Windowed: return TEXT("MainMenu.Settings.WindowMode.Windowed");
		default: return TEXT("MainMenu.Settings.WindowMode.Borderless");
		}
	}

	FString QualityKey(const int32 Quality)
	{
		switch (FMath::Clamp(Quality, 0, 4))
		{
		case 0: return TEXT("MainMenu.Settings.Quality.Low");
		case 1: return TEXT("MainMenu.Settings.Quality.Medium");
		case 2: return TEXT("MainMenu.Settings.Quality.High");
		case 3: return TEXT("MainMenu.Settings.Quality.Epic");
		default: return TEXT("MainMenu.Settings.Quality.Custom");
		}
	}

	constexpr float DefaultDisplayGamma = 2.2f;

	const TArray<float>& FrameRateLimits()
	{
		static const TArray<float> Values = {0.f, 30.f, 60.f, 90.f, 120.f, 144.f, 165.f, 240.f};
		return Values;
	}

	void SetRuntimeCVar(const TCHAR* Name, const int32 Value)
	{
		if (IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			Variable->Set(Value, ECVF_SetByGameSetting);
		}
	}

	int32 GetRuntimeCVar(const TCHAR* Name, const int32 Fallback)
	{
		if (const IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			return Variable->GetInt();
		}
		return Fallback;
	}
}

bool FAvMainMenuSettingsSnapshot::EqualsDisplay(const FAvMainMenuSettingsSnapshot& Other) const
{
	return Resolution == Other.Resolution && WindowMode == Other.WindowMode;
}

bool FAvMainMenuSettingsSnapshot::Equals(const FAvMainMenuSettingsSnapshot& Other) const
{
	return EqualsDisplay(Other) &&
		AspectRatio == Other.AspectRatio &&
		bVSync == Other.bVSync &&
		FMath::IsNearlyEqual(FrameRateLimit, Other.FrameRateLimit) &&
		FMath::IsNearlyEqual(Brightness, Other.Brightness) &&
		ColorDeficiencyType == Other.ColorDeficiencyType &&
		ColorDeficiencyStrength == Other.ColorDeficiencyStrength &&
		OverallQuality == Other.OverallQuality &&
		GlobalIlluminationQuality == Other.GlobalIlluminationQuality &&
		ShadowQuality == Other.ShadowQuality &&
		AntiAliasingQuality == Other.AntiAliasingQuality &&
		ViewDistanceQuality == Other.ViewDistanceQuality &&
		TextureQuality == Other.TextureQuality &&
		EffectsQuality == Other.EffectsQuality &&
		ReflectionQuality == Other.ReflectionQuality &&
		FoliageQuality == Other.FoliageQuality &&
		ShadingQuality == Other.ShadingQuality &&
		PostProcessQuality == Other.PostProcessQuality &&
		GlobalIlluminationMethod == Other.GlobalIlluminationMethod &&
		ReflectionMethod == Other.ReflectionMethod &&
		AntiAliasingMethod == Other.AntiAliasingMethod &&
		bMotionBlur == Other.bMotionBlur &&
		bLensFlares == Other.bLensFlares &&
		bBloom == Other.bBloom &&
		bAmbientOcclusion == Other.bAmbientOcclusion &&
		FMath::IsNearlyEqual(ResolutionScale, Other.ResolutionScale) &&
		FMath::IsNearlyEqual(MasterVolume, Other.MasterVolume) &&
		FMath::IsNearlyEqual(SFXVolume, Other.SFXVolume) &&
		FMath::IsNearlyEqual(MusicVolume, Other.MusicVolume) &&
		FMath::IsNearlyEqual(VoiceVolume, Other.VoiceVolume) &&
		Culture == Other.Culture;
}

bool UAvMainMenuSavePresenceAdapter::Refresh()
{
	bHasValidSave = false;
	ValidationReason = TEXT("MissingSlot");
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("AvariikaCompany"), 0))
	{
		return false;
	}

	const UAvariikaSaveGame* Save = Cast<UAvariikaSaveGame>(
		UGameplayStatics::LoadGameFromSlot(TEXT("AvariikaCompany"), 0));
	if (!Save)
	{
		ValidationReason = TEXT("InvalidClass");
		return false;
	}

	const bool bHasValidActiveRecord = Save->CharacterRecords.ContainsByPredicate(
		[Save](const FAvCharacterRecord& Record)
		{
			return !Record.CharacterId.IsNone() &&
				(Record.CharacterId == Save->ActiveCharacterId || Record.bIsActive);
		});
	const bool bLegacyValid = Save->CharacterRecords.IsEmpty() && Save->bHasSavedWorkerAppearance;
	const bool bCompanyFieldsValid = Save->ShiftNumber >= 1 && Save->QuotaWindowShifts >= 0;
	bHasValidSave = bCompanyFieldsValid && (bHasValidActiveRecord || bLegacyValid);
	ValidationReason = bHasValidSave ? TEXT("Valid") : TEXT("InvalidPayload");
	return bHasValidSave;
}

void UAvMainMenuSettingsAdapter::Initialize(UWorld* InWorld)
{
	World = InWorld;
	LoadSupportedResolutions();

	if (UGameUserSettings* Native = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		Original.Resolution = Native->GetScreenResolution();
		Original.WindowMode = static_cast<int32>(Native->GetFullscreenMode());
		Original.bVSync = Native->IsVSyncEnabled();
		Original.FrameRateLimit = Native->GetFrameRateLimit();
		Original.GlobalIlluminationQuality = FMath::Clamp(Native->GetGlobalIlluminationQuality(), 0, 3);
		Original.ShadowQuality = FMath::Clamp(Native->GetShadowQuality(), 0, 3);
		Original.AntiAliasingQuality = FMath::Clamp(Native->GetAntiAliasingQuality(), 0, 3);
		Original.ViewDistanceQuality = FMath::Clamp(Native->GetViewDistanceQuality(), 0, 3);
		Original.TextureQuality = FMath::Clamp(Native->GetTextureQuality(), 0, 3);
		Original.EffectsQuality = FMath::Clamp(Native->GetVisualEffectQuality(), 0, 3);
		Original.ReflectionQuality = FMath::Clamp(Native->GetReflectionQuality(), 0, 3);
		Original.FoliageQuality = FMath::Clamp(Native->GetFoliageQuality(), 0, 3);
		Original.ShadingQuality = FMath::Clamp(Native->GetShadingQuality(), 0, 3);
		Original.PostProcessQuality = FMath::Clamp(Native->GetPostProcessingQuality(), 0, 3);
		float CurrentScaleNormalized = 1.f;
		float MinimumScale = 50.f;
		float MaximumScale = 100.f;
		Native->GetResolutionScaleInformationEx(
			CurrentScaleNormalized, Original.ResolutionScale, MinimumScale, MaximumScale);
		const int32 NativeOverall = Native->GetOverallScalabilityLevel();
		Original.OverallQuality = NativeOverall >= 0 && NativeOverall <= 3 ? NativeOverall : 4;
	}
	Original.GlobalIlluminationMethod =
		FMath::Clamp(AvMainMenu::GetRuntimeCVar(TEXT("r.DynamicGlobalIlluminationMethod"), 1), 0, 1);
	Original.ReflectionMethod =
		FMath::Clamp(AvMainMenu::GetRuntimeCVar(TEXT("r.ReflectionMethod"), 1), 0, 2);
	Original.AntiAliasingMethod =
		AvMainMenu::GetRuntimeCVar(TEXT("r.AntiAliasingMethod"), 4);
	if (Original.AntiAliasingMethod < 0 || Original.AntiAliasingMethod > 4 ||
		Original.AntiAliasingMethod == 3)
	{
		Original.AntiAliasingMethod = 4;
	}
	Original.bMotionBlur = AvMainMenu::GetRuntimeCVar(TEXT("r.MotionBlurQuality"), 0) > 0;
	Original.bLensFlares = AvMainMenu::GetRuntimeCVar(TEXT("r.LensFlareQuality"), 1) > 0;
	Original.bBloom = AvMainMenu::GetRuntimeCVar(TEXT("r.BloomQuality"), 5) > 0;
	Original.bAmbientOcclusion = AvMainMenu::GetRuntimeCVar(TEXT("r.AmbientOcclusionLevels"), 3) != 0;
	Original.Culture = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();
	if (Original.Culture != TEXT("en"))
	{
		Original.Culture = TEXT("ru");
	}
	LoadProjectSettingsSave();
	if (!SupportedResolutions.Contains(Original.Resolution) && !SupportedResolutions.IsEmpty())
	{
		Original.Resolution = SupportedResolutions[
			FMath::Clamp(FindResolutionIndex(Original.Resolution), 0, SupportedResolutions.Num() - 1)];
	}
	Pending = Original;
	BeforeDisplayPreview = Original;
}

void UAvMainMenuSettingsAdapter::LoadSupportedResolutions()
{
	SupportedResolutions.Reset();
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);
	SupportedResolutions.RemoveAll([](const FIntPoint& Point)
	{
		return Point.X < 1280 || Point.Y < 720;
	});
	SupportedResolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		return A.X == B.X ? A.Y < B.Y : A.X < B.X;
	});
	SupportedResolutions.SetNum(Algo::Unique(SupportedResolutions));
	if (SupportedResolutions.IsEmpty())
	{
		SupportedResolutions = {
			FIntPoint(1280, 720), FIntPoint(1920, 1080), FIntPoint(2560, 1440),
			FIntPoint(3440, 1440), FIntPoint(3840, 2160)
		};
	}
}

void UAvMainMenuSettingsAdapter::LoadProjectSettingsSave()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("AvariikaUserSettings"), 0))
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenuSettings] No settings save exists yet; first Apply will create %s."),
			*FPaths::Combine(
				FPaths::ProjectSavedDir(), TEXT("SaveGames/AvariikaUserSettings.sav")));
		return;
	}
	const UAvariikaUserSettingsSaveGame* Save = Cast<UAvariikaUserSettingsSaveGame>(
		UGameplayStatics::LoadGameFromSlot(TEXT("AvariikaUserSettings"), 0));
	if (!Save || Save->MenuSettingsSchemaVersion < 1)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenuSettings] Existing settings save could not be read as UAvariikaUserSettingsSaveGame or has an unsupported schema."));
		return;
	}

	Original.MasterVolume = FMath::Clamp(Save->MasterVolume, 0.f, 1.f);
	Original.SFXVolume = FMath::Clamp(Save->SFXVolume, 0.f, 1.f);
	Original.MusicVolume = FMath::Clamp(Save->MusicVolume, 0.f, 1.f);
	Original.VoiceVolume = FMath::Clamp(Save->VoiceVolume, 0.f, 1.f);
	if (Save->Culture == TEXT("ru") || Save->Culture == TEXT("en"))
	{
		Original.Culture = Save->Culture;
	}
	if (Save->ResolutionX >= 1280 && Save->ResolutionY >= 720)
	{
		Original.Resolution = FIntPoint(Save->ResolutionX, Save->ResolutionY);
	}
	if (Save->WindowMode >= 0 && Save->WindowMode <= static_cast<int32>(EWindowMode::Windowed))
	{
		Original.WindowMode = Save->WindowMode;
	}
	Original.AspectRatio = FMath::Clamp(Save->AspectRatio, 0, 4);
	Original.bVSync = Save->bVSync;
	Original.FrameRateLimit = FMath::Max(0.f, Save->FrameRateLimit);
	Original.Brightness = FMath::Clamp(Save->Brightness, 0.5f, 1.5f);
	Original.ColorDeficiencyType = FMath::Clamp(Save->ColorDeficiencyType, 0, 3);
	Original.ColorDeficiencyStrength = FMath::Clamp(Save->ColorDeficiencyStrength, 0, 10);
	if (Save->OverallQuality >= 0 && Save->OverallQuality <= 4)
	{
		Original.OverallQuality = Save->OverallQuality;
	}
	if (Save->MenuSettingsSchemaVersion >= 2)
	{
		Original.GlobalIlluminationQuality = FMath::Clamp(Save->GlobalIlluminationQuality, 0, 3);
		Original.ShadowQuality = FMath::Clamp(Save->ShadowQuality, 0, 3);
		Original.AntiAliasingQuality = FMath::Clamp(Save->AntiAliasingQuality, 0, 3);
		Original.ViewDistanceQuality = FMath::Clamp(Save->ViewDistanceQuality, 0, 3);
		Original.TextureQuality = FMath::Clamp(Save->TextureQuality, 0, 3);
		Original.EffectsQuality = FMath::Clamp(Save->EffectsQuality, 0, 3);
		Original.ReflectionQuality = FMath::Clamp(Save->ReflectionQuality, 0, 3);
		Original.FoliageQuality = FMath::Clamp(Save->FoliageQuality, 0, 3);
		Original.ShadingQuality = FMath::Clamp(Save->ShadingQuality, 0, 3);
		Original.PostProcessQuality = FMath::Clamp(Save->PostProcessQuality, 0, 3);
		Original.GlobalIlluminationMethod = FMath::Clamp(Save->GlobalIlluminationMethod, 0, 1);
		Original.ReflectionMethod = FMath::Clamp(Save->ReflectionMethod, 0, 2);
		Original.AntiAliasingMethod =
			Save->AntiAliasingMethod == 3 ? 4 : FMath::Clamp(Save->AntiAliasingMethod, 0, 4);
		Original.bMotionBlur = Save->bMotionBlur;
		Original.bLensFlares = Save->bLensFlares;
		Original.bBloom = Save->bBloom;
		Original.bAmbientOcclusion = Save->bAmbientOcclusion;
		Original.ResolutionScale = FMath::Clamp(Save->ResolutionScale, 50.f, 100.f);
	}
}

int32 UAvMainMenuSettingsAdapter::FindResolutionIndex(const FIntPoint& Resolution) const
{
	const int32 Exact = SupportedResolutions.IndexOfByKey(Resolution);
	if (Exact != INDEX_NONE)
	{
		return Exact;
	}
	int32 BestIndex = 0;
	int64 BestDistance = MAX_int64;
	for (int32 Index = 0; Index < SupportedResolutions.Num(); ++Index)
	{
		const int64 Distance = FMath::Abs(static_cast<int64>(SupportedResolutions[Index].X) - Resolution.X) +
			FMath::Abs(static_cast<int64>(SupportedResolutions[Index].Y) - Resolution.Y);
		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			BestIndex = Index;
		}
	}
	return BestIndex;
}

void UAvMainMenuSettingsAdapter::Adjust(const EAvMainMenuSettingRow Row, const int32 Delta)
{
	if (Delta == 0)
	{
		return;
	}
	if (Row == EAvMainMenuSettingRow::Resolution)
	{
		SetPendingIndex(Row, GetPendingIndex(Row) + FMath::Sign(Delta));
		return;
	}
	if (Row == EAvMainMenuSettingRow::MasterVolume ||
		Row == EAvMainMenuSettingRow::SFXVolume ||
		Row == EAvMainMenuSettingRow::MusicVolume ||
		Row == EAvMainMenuSettingRow::VoiceVolume)
	{
		SetPendingScalar(Row, GetPendingScalar(Row) + Delta * 5.f);
		return;
	}
	SetPendingIndex(Row, GetPendingIndex(Row) + FMath::Sign(Delta));
}

TArray<FIntPoint> UAvMainMenuSettingsAdapter::GetFilteredResolutions() const
{
	if (Pending.AspectRatio == 0)
	{
		return SupportedResolutions;
	}

	static const float Ratios[] = {0.f, 16.f / 9.f, 16.f / 10.f, 21.f / 9.f, 4.f / 3.f};
	const float Target = Ratios[FMath::Clamp(Pending.AspectRatio, 0, 4)];
	TArray<FIntPoint> Result = SupportedResolutions.FilterByPredicate([Target](const FIntPoint& Point)
	{
		return Point.Y > 0 && FMath::Abs(static_cast<float>(Point.X) / Point.Y - Target) <= 0.035f;
	});
	return Result.IsEmpty() ? SupportedResolutions : Result;
}

bool UAvMainMenuSettingsAdapter::IsSupportedResolution(const FIntPoint& Resolution) const
{
	return SupportedResolutions.Contains(Resolution);
}

void UAvMainMenuSettingsAdapter::ApplyQualityPreset(const int32 Quality)
{
	const int32 Clamped = FMath::Clamp(Quality, 0, 3);
	Pending.OverallQuality = Clamped;
	Pending.GlobalIlluminationQuality = Clamped;
	Pending.ShadowQuality = Clamped;
	Pending.AntiAliasingQuality = Clamped;
	Pending.ViewDistanceQuality = Clamped;
	Pending.TextureQuality = Clamped;
	Pending.EffectsQuality = Clamped;
	Pending.ReflectionQuality = Clamped;
	Pending.FoliageQuality = Clamped;
	Pending.ShadingQuality = Clamped;
	Pending.PostProcessQuality = Clamped;
}

void UAvMainMenuSettingsAdapter::MarkQualityCustom()
{
	Pending.OverallQuality = 4;
}

void UAvMainMenuSettingsAdapter::SetPendingIndex(
	const EAvMainMenuSettingRow Row, const int32 Value)
{
	switch (Row)
	{
	case EAvMainMenuSettingRow::Resolution:
	{
		const TArray<FIntPoint> Filtered = GetFilteredResolutions();
		if (!Filtered.IsEmpty())
		{
			Pending.Resolution = Filtered[FMath::Clamp(Value, 0, Filtered.Num() - 1)];
		}
		break;
	}
	case EAvMainMenuSettingRow::WindowMode:
		Pending.WindowMode = FMath::Clamp(Value, 0, 2);
		break;
	case EAvMainMenuSettingRow::AspectRatio:
		Pending.AspectRatio = FMath::Clamp(Value, 0, 4);
	{
		const TArray<FIntPoint> Filtered = GetFilteredResolutions();
		if (!Filtered.Contains(Pending.Resolution) && !Filtered.IsEmpty())
		{
			int32 Best = 0;
			int64 BestDistance = MAX_int64;
			for (int32 Index = 0; Index < Filtered.Num(); ++Index)
			{
				const int64 Distance =
					FMath::Abs(static_cast<int64>(Filtered[Index].X) - Pending.Resolution.X) +
					FMath::Abs(static_cast<int64>(Filtered[Index].Y) - Pending.Resolution.Y);
				if (Distance < BestDistance)
				{
					BestDistance = Distance;
					Best = Index;
				}
			}
			Pending.Resolution = Filtered[Best];
		}
	}
		break;
	case EAvMainMenuSettingRow::VSync:
		Pending.bVSync = Value != 0;
		break;
	case EAvMainMenuSettingRow::FrameRateLimit:
		Pending.FrameRateLimit =
			AvMainMenu::FrameRateLimits()[FMath::Clamp(Value, 0, AvMainMenu::FrameRateLimits().Num() - 1)];
		break;
	case EAvMainMenuSettingRow::ColorDeficiencyType:
	{
		const int32 Clamped = FMath::Clamp(Value, 0, 3);
		if (Pending.ColorDeficiencyType != Clamped)
		{
			Pending.ColorDeficiencyType = Clamped;
			UWidgetBlueprintLibrary::SetColorVisionDeficiencyType(
				static_cast<EColorVisionDeficiency>(Pending.ColorDeficiencyType),
				Pending.ColorDeficiencyStrength, true, false);
		}
		break;
	}
	case EAvMainMenuSettingRow::OverallQuality:
		if (Value >= 0 && Value <= 3)
		{
			ApplyQualityPreset(Value);
		}
		else if (Value == 4)
		{
			// Custom preserves the currently selected child settings. It is a
			// real pending value, not a display-only vendor option.
			MarkQualityCustom();
		}
		break;
	case EAvMainMenuSettingRow::GlobalIlluminationQuality:
		Pending.GlobalIlluminationQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::ShadowQuality:
		Pending.ShadowQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::AntiAliasingQuality:
		Pending.AntiAliasingQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::ViewDistanceQuality:
		Pending.ViewDistanceQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::TextureQuality:
		Pending.TextureQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::EffectsQuality:
		Pending.EffectsQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::ReflectionQuality:
		Pending.ReflectionQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::FoliageQuality:
		Pending.FoliageQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::ShadingQuality:
		Pending.ShadingQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::PostProcessQuality:
		Pending.PostProcessQuality = FMath::Clamp(Value, 0, 3); MarkQualityCustom(); break;
	case EAvMainMenuSettingRow::GlobalIlluminationMethod:
		Pending.GlobalIlluminationMethod = FMath::Clamp(Value, 0, 1);
		break;
	case EAvMainMenuSettingRow::ReflectionMethod:
		Pending.ReflectionMethod = FMath::Clamp(Value, 0, 2);
		break;
	case EAvMainMenuSettingRow::MotionBlur:
		Pending.bMotionBlur = Value != 0;
		break;
	case EAvMainMenuSettingRow::LensFlares:
		Pending.bLensFlares = Value != 0;
		break;
	case EAvMainMenuSettingRow::Bloom:
		Pending.bBloom = Value != 0;
		break;
	case EAvMainMenuSettingRow::AmbientOcclusion:
		Pending.bAmbientOcclusion = Value != 0;
		break;
	case EAvMainMenuSettingRow::AntiAliasingMethod:
	{
		static const int32 Methods[] = {0, 1, 2, 4};
		Pending.AntiAliasingMethod = Methods[FMath::Clamp(Value, 0, 3)];
		break;
	}
	case EAvMainMenuSettingRow::Language:
	{
		const FString Culture = Value == 1 ? TEXT("en") : TEXT("ru");
		if (Pending.Culture != Culture)
		{
			Pending.Culture = Culture;
			ApplyCulture(Pending.Culture);
			UE_LOG(LogTemp, Log,
				TEXT("[AvMainMenuSettings] Runtime culture changed to %s; persistence is pending Apply."),
				*Pending.Culture);
		}
		break;
	}
	default:
		break;
	}
}

void UAvMainMenuSettingsAdapter::SetPendingScalar(
	const EAvMainMenuSettingRow Row, const float Value)
{
	switch (Row)
	{
	case EAvMainMenuSettingRow::Brightness:
	{
		const float Clamped = FMath::Clamp(Value / 100.f, 0.5f, 1.5f);
		if (!FMath::IsNearlyEqual(Pending.Brightness, Clamped))
		{
			Pending.Brightness = Clamped;
			if (GEngine)
			{
				GEngine->DisplayGamma = AvMainMenu::DefaultDisplayGamma * Pending.Brightness;
			}
		}
		break;
	}
	case EAvMainMenuSettingRow::ColorDeficiencyStrength:
	{
		const int32 Clamped = FMath::Clamp(FMath::RoundToInt(Value), 0, 10);
		if (Pending.ColorDeficiencyStrength != Clamped)
		{
			Pending.ColorDeficiencyStrength = Clamped;
			UWidgetBlueprintLibrary::SetColorVisionDeficiencyType(
				static_cast<EColorVisionDeficiency>(Pending.ColorDeficiencyType),
				Pending.ColorDeficiencyStrength, true, false);
		}
		break;
	}
	case EAvMainMenuSettingRow::ResolutionScale:
		Pending.ResolutionScale = FMath::Clamp(Value, 50.f, 100.f);
		break;
	case EAvMainMenuSettingRow::MasterVolume:
	{
		const float Clamped = FMath::Clamp(Value / 100.f, 0.f, 1.f);
		if (!FMath::IsNearlyEqual(Pending.MasterVolume, Clamped))
		{
			Pending.MasterVolume = Clamped;
			ApplyAudioRuntimeState();
		}
		break;
	}
	case EAvMainMenuSettingRow::SFXVolume:
	{
		const float Clamped = FMath::Clamp(Value / 100.f, 0.f, 1.f);
		if (!FMath::IsNearlyEqual(Pending.SFXVolume, Clamped))
		{
			Pending.SFXVolume = Clamped;
			ApplyAudioRuntimeState();
		}
		break;
	}
	case EAvMainMenuSettingRow::MusicVolume:
	{
		const float Clamped = FMath::Clamp(Value / 100.f, 0.f, 1.f);
		if (!FMath::IsNearlyEqual(Pending.MusicVolume, Clamped))
		{
			Pending.MusicVolume = Clamped;
			ApplyAudioRuntimeState();
		}
		break;
	}
	case EAvMainMenuSettingRow::VoiceVolume:
	{
		const float Clamped = FMath::Clamp(Value / 100.f, 0.f, 1.f);
		if (!FMath::IsNearlyEqual(Pending.VoiceVolume, Clamped))
		{
			Pending.VoiceVolume = Clamped;
			ApplyAudioRuntimeState();
		}
		break;
	}
	default:
		break;
	}
}

int32 UAvMainMenuSettingsAdapter::GetPendingIndex(const EAvMainMenuSettingRow Row) const
{
	switch (Row)
	{
	case EAvMainMenuSettingRow::Resolution:
		return GetFilteredResolutions().IndexOfByKey(Pending.Resolution);
	case EAvMainMenuSettingRow::WindowMode: return Pending.WindowMode;
	case EAvMainMenuSettingRow::AspectRatio: return Pending.AspectRatio;
	case EAvMainMenuSettingRow::VSync: return Pending.bVSync ? 1 : 0;
	case EAvMainMenuSettingRow::FrameRateLimit:
	{
		const int32 Found = AvMainMenu::FrameRateLimits().IndexOfByPredicate([this](const float Candidate)
		{
			return FMath::IsNearlyEqual(Candidate, Pending.FrameRateLimit, 0.5f);
		});
		return Found == INDEX_NONE ? 0 : Found;
	}
	case EAvMainMenuSettingRow::ColorDeficiencyType: return Pending.ColorDeficiencyType;
	case EAvMainMenuSettingRow::OverallQuality: return Pending.OverallQuality;
	case EAvMainMenuSettingRow::GlobalIlluminationQuality: return Pending.GlobalIlluminationQuality;
	case EAvMainMenuSettingRow::ShadowQuality: return Pending.ShadowQuality;
	case EAvMainMenuSettingRow::AntiAliasingQuality: return Pending.AntiAliasingQuality;
	case EAvMainMenuSettingRow::ViewDistanceQuality: return Pending.ViewDistanceQuality;
	case EAvMainMenuSettingRow::TextureQuality: return Pending.TextureQuality;
	case EAvMainMenuSettingRow::EffectsQuality: return Pending.EffectsQuality;
	case EAvMainMenuSettingRow::ReflectionQuality: return Pending.ReflectionQuality;
	case EAvMainMenuSettingRow::FoliageQuality: return Pending.FoliageQuality;
	case EAvMainMenuSettingRow::ShadingQuality: return Pending.ShadingQuality;
	case EAvMainMenuSettingRow::PostProcessQuality: return Pending.PostProcessQuality;
	case EAvMainMenuSettingRow::GlobalIlluminationMethod: return Pending.GlobalIlluminationMethod;
	case EAvMainMenuSettingRow::ReflectionMethod: return Pending.ReflectionMethod;
	case EAvMainMenuSettingRow::MotionBlur: return Pending.bMotionBlur ? 1 : 0;
	case EAvMainMenuSettingRow::LensFlares: return Pending.bLensFlares ? 1 : 0;
	case EAvMainMenuSettingRow::Bloom: return Pending.bBloom ? 1 : 0;
	case EAvMainMenuSettingRow::AmbientOcclusion: return Pending.bAmbientOcclusion ? 1 : 0;
	case EAvMainMenuSettingRow::AntiAliasingMethod:
		return Pending.AntiAliasingMethod == 4 ? 3 : FMath::Clamp(Pending.AntiAliasingMethod, 0, 2);
	case EAvMainMenuSettingRow::Language: return Pending.Culture == TEXT("en") ? 1 : 0;
	default: return 0;
	}
}

int32 UAvMainMenuSettingsAdapter::GetOriginalIndex(
	const EAvMainMenuSettingRow Row) const
{
	switch (Row)
	{
	case EAvMainMenuSettingRow::Resolution:
		return SupportedResolutions.IndexOfByKey(Original.Resolution);
	case EAvMainMenuSettingRow::WindowMode: return Original.WindowMode;
	case EAvMainMenuSettingRow::AspectRatio: return Original.AspectRatio;
	case EAvMainMenuSettingRow::VSync: return Original.bVSync ? 1 : 0;
	case EAvMainMenuSettingRow::FrameRateLimit:
	{
		const int32 Found = AvMainMenu::FrameRateLimits().IndexOfByPredicate(
			[this](const float Candidate)
			{
				return FMath::IsNearlyEqual(
					Candidate, Original.FrameRateLimit, 0.5f);
			});
		return Found == INDEX_NONE ? 0 : Found;
	}
	case EAvMainMenuSettingRow::ColorDeficiencyType:
		return Original.ColorDeficiencyType;
	case EAvMainMenuSettingRow::OverallQuality:
		return Original.OverallQuality;
	case EAvMainMenuSettingRow::GlobalIlluminationQuality:
		return Original.GlobalIlluminationQuality;
	case EAvMainMenuSettingRow::ShadowQuality:
		return Original.ShadowQuality;
	case EAvMainMenuSettingRow::AntiAliasingQuality:
		return Original.AntiAliasingQuality;
	case EAvMainMenuSettingRow::ViewDistanceQuality:
		return Original.ViewDistanceQuality;
	case EAvMainMenuSettingRow::TextureQuality:
		return Original.TextureQuality;
	case EAvMainMenuSettingRow::EffectsQuality:
		return Original.EffectsQuality;
	case EAvMainMenuSettingRow::ReflectionQuality:
		return Original.ReflectionQuality;
	case EAvMainMenuSettingRow::FoliageQuality:
		return Original.FoliageQuality;
	case EAvMainMenuSettingRow::ShadingQuality:
		return Original.ShadingQuality;
	case EAvMainMenuSettingRow::PostProcessQuality:
		return Original.PostProcessQuality;
	case EAvMainMenuSettingRow::GlobalIlluminationMethod:
		return Original.GlobalIlluminationMethod;
	case EAvMainMenuSettingRow::ReflectionMethod:
		return Original.ReflectionMethod;
	case EAvMainMenuSettingRow::MotionBlur:
		return Original.bMotionBlur ? 1 : 0;
	case EAvMainMenuSettingRow::LensFlares:
		return Original.bLensFlares ? 1 : 0;
	case EAvMainMenuSettingRow::Bloom:
		return Original.bBloom ? 1 : 0;
	case EAvMainMenuSettingRow::AmbientOcclusion:
		return Original.bAmbientOcclusion ? 1 : 0;
	case EAvMainMenuSettingRow::AntiAliasingMethod:
		return Original.AntiAliasingMethod == 4 ?
			3 : FMath::Clamp(Original.AntiAliasingMethod, 0, 2);
	case EAvMainMenuSettingRow::Language:
		return Original.Culture == TEXT("en") ? 1 : 0;
	default: return 0;
	}
}

float UAvMainMenuSettingsAdapter::GetPendingScalar(const EAvMainMenuSettingRow Row) const
{
	switch (Row)
	{
	case EAvMainMenuSettingRow::Brightness: return Pending.Brightness * 100.f;
	case EAvMainMenuSettingRow::ColorDeficiencyStrength:
		return static_cast<float>(Pending.ColorDeficiencyStrength);
	case EAvMainMenuSettingRow::ResolutionScale: return Pending.ResolutionScale;
	case EAvMainMenuSettingRow::MasterVolume: return Pending.MasterVolume * 100.f;
	case EAvMainMenuSettingRow::SFXVolume: return Pending.SFXVolume * 100.f;
	case EAvMainMenuSettingRow::MusicVolume: return Pending.MusicVolume * 100.f;
	case EAvMainMenuSettingRow::VoiceVolume: return Pending.VoiceVolume * 100.f;
	default: return 0.f;
	}
}

float UAvMainMenuSettingsAdapter::GetOriginalScalar(
	const EAvMainMenuSettingRow Row) const
{
	switch (Row)
	{
	case EAvMainMenuSettingRow::Brightness:
		return Original.Brightness * 100.f;
	case EAvMainMenuSettingRow::ColorDeficiencyStrength:
		return static_cast<float>(Original.ColorDeficiencyStrength);
	case EAvMainMenuSettingRow::ResolutionScale:
		return Original.ResolutionScale;
	case EAvMainMenuSettingRow::MasterVolume:
		return Original.MasterVolume * 100.f;
	case EAvMainMenuSettingRow::SFXVolume:
		return Original.SFXVolume * 100.f;
	case EAvMainMenuSettingRow::MusicVolume:
		return Original.MusicVolume * 100.f;
	case EAvMainMenuSettingRow::VoiceVolume:
		return Original.VoiceVolume * 100.f;
	default: return 0.f;
	}
}

TArray<FText> UAvMainMenuSettingsAdapter::GetOptions(const EAvMainMenuSettingRow Row) const
{
	auto Texts = [](std::initializer_list<const TCHAR*> Keys)
	{
		TArray<FText> Result;
		for (const TCHAR* Key : Keys)
		{
			Result.Add(FAvLoc::Text(Key));
		}
		return Result;
	};

	switch (Row)
	{
	case EAvMainMenuSettingRow::Resolution:
	{
		TArray<FText> Result;
		for (const FIntPoint& Point : GetFilteredResolutions())
		{
			Result.Add(FText::FromString(FString::Printf(TEXT("%d x %d"), Point.X, Point.Y)));
		}
		return Result;
	}
	case EAvMainMenuSettingRow::WindowMode:
		return Texts({TEXT("MainMenu.Settings.WindowMode.Fullscreen"),
			TEXT("MainMenu.Settings.WindowMode.Borderless"),
			TEXT("MainMenu.Settings.WindowMode.Windowed")});
	case EAvMainMenuSettingRow::AspectRatio:
		return Texts({TEXT("MainMenu.Settings.Aspect.Any"), TEXT("MainMenu.Settings.Aspect.16x9"),
			TEXT("MainMenu.Settings.Aspect.16x10"), TEXT("MainMenu.Settings.Aspect.21x9"),
			TEXT("MainMenu.Settings.Aspect.4x3")});
	case EAvMainMenuSettingRow::VSync:
	case EAvMainMenuSettingRow::MotionBlur:
	case EAvMainMenuSettingRow::LensFlares:
	case EAvMainMenuSettingRow::Bloom:
	case EAvMainMenuSettingRow::AmbientOcclusion:
		return Texts({TEXT("MainMenu.Common.Off"), TEXT("MainMenu.Common.On")});
	case EAvMainMenuSettingRow::FrameRateLimit:
		return {FAvLoc::Text(TEXT("MainMenu.MSP.Value.Unlocked")), FText::FromString(TEXT("30")),
			FText::FromString(TEXT("60")), FText::FromString(TEXT("90")),
			FText::FromString(TEXT("120")), FText::FromString(TEXT("144")),
			FText::FromString(TEXT("165")), FText::FromString(TEXT("240"))};
	case EAvMainMenuSettingRow::OverallQuality:
		return Texts({TEXT("MainMenu.Settings.Quality.Low"), TEXT("MainMenu.Settings.Quality.Medium"),
			TEXT("MainMenu.Settings.Quality.High"), TEXT("MainMenu.Settings.Quality.Epic"),
			TEXT("MainMenu.Settings.Quality.Custom")});
	case EAvMainMenuSettingRow::GlobalIlluminationQuality:
	case EAvMainMenuSettingRow::ShadowQuality:
	case EAvMainMenuSettingRow::AntiAliasingQuality:
	case EAvMainMenuSettingRow::ViewDistanceQuality:
	case EAvMainMenuSettingRow::TextureQuality:
	case EAvMainMenuSettingRow::EffectsQuality:
	case EAvMainMenuSettingRow::ReflectionQuality:
	case EAvMainMenuSettingRow::FoliageQuality:
	case EAvMainMenuSettingRow::ShadingQuality:
	case EAvMainMenuSettingRow::PostProcessQuality:
		return Texts({TEXT("MainMenu.Settings.Quality.Low"), TEXT("MainMenu.Settings.Quality.Medium"),
			TEXT("MainMenu.Settings.Quality.High"), TEXT("MainMenu.Settings.Quality.Epic")});
	case EAvMainMenuSettingRow::ColorDeficiencyType:
		return Texts({TEXT("MainMenu.Settings.Color.Normal"),
			TEXT("MainMenu.Settings.Color.Deuteranope"),
			TEXT("MainMenu.Settings.Color.Protanope"),
			TEXT("MainMenu.Settings.Color.Tritanope")});
	case EAvMainMenuSettingRow::GlobalIlluminationMethod:
		return Texts({TEXT("MainMenu.Settings.Renderer.Disabled"), TEXT("MainMenu.Settings.Renderer.Lumen")});
	case EAvMainMenuSettingRow::ReflectionMethod:
		return Texts({TEXT("MainMenu.Settings.Renderer.Disabled"), TEXT("MainMenu.Settings.Renderer.Lumen"),
			TEXT("MainMenu.Settings.Renderer.ScreenSpace")});
	case EAvMainMenuSettingRow::AntiAliasingMethod:
		return Texts({TEXT("MainMenu.Settings.AA.None"), TEXT("MainMenu.Settings.AA.FXAA"),
			TEXT("MainMenu.Settings.AA.TAA"), TEXT("MainMenu.Settings.AA.TSR")});
	case EAvMainMenuSettingRow::Language:
		return Texts({TEXT("MainMenu.Settings.Language.Russian"),
			TEXT("MainMenu.Settings.Language.English")});
	default:
		return {};
	}
}

FText UAvMainMenuSettingsAdapter::GetValueText(const EAvMainMenuSettingRow Row) const
{
	switch (Row)
	{
	case EAvMainMenuSettingRow::Resolution:
		return FText::FromString(FString::Printf(TEXT("%d x %d"), Pending.Resolution.X, Pending.Resolution.Y));
	case EAvMainMenuSettingRow::WindowMode:
		return FAvLoc::Text(AvMainMenu::WindowModeKey(Pending.WindowMode));
	case EAvMainMenuSettingRow::VSync:
		return FAvLoc::Text(Pending.bVSync ? TEXT("MainMenu.Common.On") : TEXT("MainMenu.Common.Off"));
	case EAvMainMenuSettingRow::OverallQuality:
		return FAvLoc::Text(AvMainMenu::QualityKey(Pending.OverallQuality));
	case EAvMainMenuSettingRow::MasterVolume:
		return FText::AsPercent(Pending.MasterVolume);
	case EAvMainMenuSettingRow::SFXVolume:
		return FText::AsPercent(Pending.SFXVolume);
	case EAvMainMenuSettingRow::MusicVolume:
		return FText::AsPercent(Pending.MusicVolume);
	case EAvMainMenuSettingRow::VoiceVolume:
		return FText::AsPercent(Pending.VoiceVolume);
	case EAvMainMenuSettingRow::Language:
		return FAvLoc::Text(Pending.Culture == TEXT("en") ?
			TEXT("MainMenu.Settings.Language.English") : TEXT("MainMenu.Settings.Language.Russian"));
	default:
	{
		const TArray<FText> Options = GetOptions(Row);
		const int32 PendingIndex = GetPendingIndex(Row);
		return Options.IsValidIndex(PendingIndex) ? Options[PendingIndex] : FText::GetEmpty();
	}
	}
}

bool UAvMainMenuSettingsAdapter::IsPIEWorld() const
{
	return World && World->WorldType == EWorldType::PIE;
}

void UAvMainMenuSettingsAdapter::ApplyCulture(const FString& Culture) const
{
	// Use the supported runtime culture API. AvariikaUserSettings remains the
	// only persistent source of truth, so the engine config mirror is disabled.
	if (!UKismetInternationalizationLibrary::SetCurrentCulture(Culture, false))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenuSettings] Could not change runtime culture to %s."),
			*Culture);
	}
}

void UAvMainMenuSettingsAdapter::ApplyAudioRuntimeState()
{
	if (!World)
	{
		return;
	}

	USoundMix* Mix = LoadObject<USoundMix>(nullptr,
		TEXT("/Game/Avariika/Audio/Mixes/SM_Avariika_Settings.SM_Avariika_Settings"));
	USoundClass* MasterClass = LoadObject<USoundClass>(nullptr,
		TEXT("/Game/Avariika/Audio/Classes/SC_Avariika_Master.SC_Avariika_Master"));
	USoundClass* SFXClass = LoadObject<USoundClass>(nullptr,
		TEXT("/Game/Avariika/Audio/Classes/SC_Avariika_SFX.SC_Avariika_SFX"));
	USoundClass* MusicClass = LoadObject<USoundClass>(nullptr,
		TEXT("/Game/Avariika/Audio/Classes/SC_Avariika_Music.SC_Avariika_Music"));
	USoundClass* VoiceClass = LoadObject<USoundClass>(nullptr,
		TEXT("/Game/Avariika/Audio/Classes/SC_Avariika_Voice.SC_Avariika_Voice"));
	bDedicatedAudioChannelsReady = Mix && MasterClass && SFXClass && MusicClass && VoiceClass;

	if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
	{
		AudioDevice->SetTransientPrimaryVolume(Pending.MasterVolume);
	}
	if (!bDedicatedAudioChannelsReady)
	{
		return;
	}

	if (!bAudioMixPushed)
	{
		UGameplayStatics::PushSoundMixModifier(World, Mix);
		bAudioMixPushed = true;
	}
	// SoundWave/SoundCue routing is authored in project assets. Runtime only
	// changes SoundMix overrides; mutating cooked USoundBase objects here breaks
	// Disregard-for-GC assumptions in packaged builds.
	// Master volume already applies safely to every routed and unrouted sound
	// through the audio device primary volume above. Adding a second recursive
	// override for the root SoundClass made the active mix recursively visit
	// the same child classes that have dedicated overrides below.
	UGameplayStatics::SetSoundMixClassOverride(
		World, Mix, SFXClass, Pending.SFXVolume, 1.f, 0.05f, false);
	UGameplayStatics::SetSoundMixClassOverride(
		World, Mix, MusicClass, Pending.MusicVolume, 1.f, 0.05f, false);
	UGameplayStatics::SetSoundMixClassOverride(
		World, Mix, VoiceClass, Pending.VoiceVolume, 1.f, 0.05f, false);
}

void UAvMainMenuSettingsAdapter::ApplyNonDisplayRuntimeState()
{
	if (UGameUserSettings* Native = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		Native->SetVSyncEnabled(Pending.bVSync);
		Native->SetFrameRateLimit(Pending.FrameRateLimit);
		Native->SetGlobalIlluminationQuality(Pending.GlobalIlluminationQuality);
		Native->SetShadowQuality(Pending.ShadowQuality);
		Native->SetAntiAliasingQuality(Pending.AntiAliasingQuality);
		Native->SetViewDistanceQuality(Pending.ViewDistanceQuality);
		Native->SetTextureQuality(Pending.TextureQuality);
		Native->SetVisualEffectQuality(Pending.EffectsQuality);
		Native->SetReflectionQuality(Pending.ReflectionQuality);
		Native->SetFoliageQuality(Pending.FoliageQuality);
		Native->SetShadingQuality(Pending.ShadingQuality);
		Native->SetPostProcessingQuality(Pending.PostProcessQuality);
		Native->SetResolutionScaleValueEx(Pending.ResolutionScale);
		Native->ApplyNonResolutionSettings();
	}

	AvMainMenu::SetRuntimeCVar(TEXT("r.AntiAliasingMethod"), Pending.AntiAliasingMethod);
	AvMainMenu::SetRuntimeCVar(TEXT("r.MotionBlurQuality"), Pending.bMotionBlur ? 4 : 0);
	AvMainMenu::SetRuntimeCVar(TEXT("r.LensFlareQuality"), Pending.bLensFlares ? 1 : 0);
	AvMainMenu::SetRuntimeCVar(TEXT("r.BloomQuality"), Pending.bBloom ? 5 : 0);
	AvMainMenu::SetRuntimeCVar(TEXT("r.AmbientOcclusionLevels"), Pending.bAmbientOcclusion ? 3 : 0);

	if (GEngine)
	{
		GEngine->DisplayGamma = AvMainMenu::DefaultDisplayGamma * Pending.Brightness;
	}
	UWidgetBlueprintLibrary::SetColorVisionDeficiencyType(
		static_cast<EColorVisionDeficiency>(Pending.ColorDeficiencyType),
		Pending.ColorDeficiencyStrength, true, false);
	ApplyCulture(Pending.Culture);
	ApplyAudioRuntimeState();
}

void UAvMainMenuSettingsAdapter::ApplyNativePreview()
{
	UGameUserSettings* Native = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	if (!Native)
	{
		return;
	}
	Native->SetScreenResolution(Pending.Resolution);
	Native->SetFullscreenMode(static_cast<EWindowMode::Type>(Pending.WindowMode));
	Native->ApplyResolutionSettings(false);
	ApplyNonDisplayRuntimeState();
}

bool UAvMainMenuSettingsAdapter::BeginApply(bool& bOutNeedsDisplayConfirmation)
{
	bOutNeedsDisplayConfirmation = IsDisplayChanged();
	BeforeDisplayPreview = Original;
	bLastApplyWasPIESafe = IsPIEWorld();
	ApplyNonDisplayRuntimeState();
	if (!bLastApplyWasPIESafe)
	{
		if (bOutNeedsDisplayConfirmation)
		{
			ApplyNativePreview();
		}
		bDisplayPreviewApplied = bOutNeedsDisplayConfirmation;
	}
	else
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenuSettings] PIE-safe apply: resolution/window unchanged; graphics/audio/language applied."));
	}
	return true;
}

void UAvMainMenuSettingsAdapter::ConfirmApply()
{
	if (!IsPIEWorld())
	{
		if (UGameUserSettings* Native = GEngine ? GEngine->GetGameUserSettings() : nullptr)
		{
			Native->ConfirmVideoMode();
			Native->SaveSettings();
		}
	}
	SaveProjectSettings();
	Original = Pending;
	BeforeDisplayPreview = Original;
	bDisplayPreviewApplied = false;
}

void UAvMainMenuSettingsAdapter::ApplyPersistedRuntimeState()
{
	Pending = Original;
	BeforeDisplayPreview = Original;
	if (!IsPIEWorld())
	{
		ApplyNativePreview();
	}
	else
	{
		ApplyNonDisplayRuntimeState();
	}
}

void UAvMainMenuSettingsAdapter::CaptureNativeStateAndCommit()
{
	if (UGameUserSettings* Native = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		Pending.Resolution = Native->GetScreenResolution();
		Pending.WindowMode = static_cast<int32>(Native->GetFullscreenMode());
		Pending.bVSync = Native->IsVSyncEnabled();
		Pending.FrameRateLimit = Native->GetFrameRateLimit();
	}
	Pending.Culture = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();
	if (Pending.Culture != TEXT("en"))
	{
		Pending.Culture = TEXT("ru");
	}
	SaveProjectSettings();
	Original = Pending;
	BeforeDisplayPreview = Original;
	bDisplayPreviewApplied = false;
}

void UAvMainMenuSettingsAdapter::RevertDisplayPreview()
{
	if (bDisplayPreviewApplied && !IsPIEWorld())
	{
		if (UGameUserSettings* Native = GEngine ? GEngine->GetGameUserSettings() : nullptr)
		{
			Native->RevertVideoMode();
			Native->SetVSyncEnabled(BeforeDisplayPreview.bVSync);
			Native->ApplyResolutionSettings(false);
		}
	}
	Pending = BeforeDisplayPreview;
	ApplyNonDisplayRuntimeState();
	bDisplayPreviewApplied = false;
}

void UAvMainMenuSettingsAdapter::RevertPending()
{
	RevertDisplayPreview();
	Pending = Original;
	ApplyNonDisplayRuntimeState();
}

void UAvMainMenuSettingsAdapter::ResetToDefaults()
{
	FAvMainMenuSettingsSnapshot Defaults;
	FDisplayMetrics Metrics;
	FDisplayMetrics::RebuildDisplayMetrics(Metrics);
	Defaults.Resolution = FIntPoint(Metrics.PrimaryDisplayWidth, Metrics.PrimaryDisplayHeight);
	if (!SupportedResolutions.Contains(Defaults.Resolution))
	{
		Defaults.Resolution = SupportedResolutions[
			FMath::Clamp(FindResolutionIndex(Defaults.Resolution), 0, SupportedResolutions.Num() - 1)];
	}
	Defaults.WindowMode = static_cast<int32>(EWindowMode::WindowedFullscreen);
	Defaults.FrameRateLimit = 60.f;
	Defaults.Culture = TEXT("ru");
	Pending = Defaults;
	ApplyNonDisplayRuntimeState();
}

bool UAvMainMenuSettingsAdapter::IsDisplayChanged() const
{
	return !Pending.EqualsDisplay(Original);
}

void UAvMainMenuSettingsAdapter::SaveProjectSettings()
{
	UAvariikaUserSettingsSaveGame* Save = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("AvariikaUserSettings"), 0))
	{
		Save = Cast<UAvariikaUserSettingsSaveGame>(
			UGameplayStatics::LoadGameFromSlot(TEXT("AvariikaUserSettings"), 0));
		if (!Save)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvMainMenuSettings] Existing settings save has the wrong class and will not be overwritten."));
			return;
		}
	}
	if (!Save)
	{
		Save = Cast<UAvariikaUserSettingsSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UAvariikaUserSettingsSaveGame::StaticClass()));
	}
	if (!Save)
	{
		return;
	}
	Save->MenuSettingsSchemaVersion = 2;
	Save->ResolutionX = Pending.Resolution.X;
	Save->ResolutionY = Pending.Resolution.Y;
	Save->WindowMode = Pending.WindowMode;
	Save->AspectRatio = Pending.AspectRatio;
	Save->bVSync = Pending.bVSync;
	Save->FrameRateLimit = Pending.FrameRateLimit;
	Save->Brightness = Pending.Brightness;
	Save->ColorDeficiencyType = Pending.ColorDeficiencyType;
	Save->ColorDeficiencyStrength = Pending.ColorDeficiencyStrength;
	Save->OverallQuality = Pending.OverallQuality;
	Save->GlobalIlluminationQuality = Pending.GlobalIlluminationQuality;
	Save->ShadowQuality = Pending.ShadowQuality;
	Save->AntiAliasingQuality = Pending.AntiAliasingQuality;
	Save->ViewDistanceQuality = Pending.ViewDistanceQuality;
	Save->TextureQuality = Pending.TextureQuality;
	Save->EffectsQuality = Pending.EffectsQuality;
	Save->ReflectionQuality = Pending.ReflectionQuality;
	Save->FoliageQuality = Pending.FoliageQuality;
	Save->ShadingQuality = Pending.ShadingQuality;
	Save->PostProcessQuality = Pending.PostProcessQuality;
	Save->GlobalIlluminationMethod = Pending.GlobalIlluminationMethod;
	Save->ReflectionMethod = Pending.ReflectionMethod;
	Save->AntiAliasingMethod = Pending.AntiAliasingMethod;
	Save->bMotionBlur = Pending.bMotionBlur;
	Save->bLensFlares = Pending.bLensFlares;
	Save->bBloom = Pending.bBloom;
	Save->bAmbientOcclusion = Pending.bAmbientOcclusion;
	Save->ResolutionScale = Pending.ResolutionScale;
	Save->MasterVolume = Pending.MasterVolume;
	Save->SFXVolume = Pending.SFXVolume;
	Save->MusicVolume = Pending.MusicVolume;
	Save->VoiceVolume = Pending.VoiceVolume;
	Save->Culture = Pending.Culture;
	if (UGameplayStatics::SaveGameToSlot(Save, TEXT("AvariikaUserSettings"), 0))
	{
		++SaveCommitCount;
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenuSettings] Saved AvariikaUserSettings path=%s commit=%d culture=%s master=%.2f sfx=%.2f music=%.2f voice=%.2f fps=%.0f vsync=%s overall=%d."),
			*FPaths::Combine(
				FPaths::ProjectSavedDir(), TEXT("SaveGames/AvariikaUserSettings.sav")),
			SaveCommitCount, *Pending.Culture, Pending.MasterVolume, Pending.SFXVolume,
			Pending.MusicVolume, Pending.VoiceVolume, Pending.FrameRateLimit,
			Pending.bVSync ? TEXT("true") : TEXT("false"), Pending.OverallQuality);
	}
	else
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenuSettings] Failed to save AvariikaUserSettings."));
	}
}

void UAvMainMenuInputRouter::Activate(APlayerController* InController, UWidget* InitialFocus)
{
	Deactivate();
	Controller = InController;
	if (!Controller)
	{
		return;
	}
	Controller->bShowMouseCursor = true;
	Controller->bEnableClickEvents = true;
	Controller->bEnableMouseOverEvents = true;
	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	if (InitialFocus)
	{
		Mode.SetWidgetToFocus(InitialFocus->TakeWidget());
	}
	Controller->SetInputMode(Mode);

	if (ULocalPlayer* LocalPlayer = Controller->GetLocalPlayer())
	{
		EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	}
	VendorMenuContext = LoadObject<UInputMappingContext>(nullptr,
		TEXT("/Game/ThirdParty/MenuSystemPro/Input/Menu/IMC_Menu.IMC_Menu"));
	bVendorMenuContextLoaded = VendorMenuContext != nullptr;
	if (EnhancedSubsystem && VendorMenuContext)
	{
		EnhancedSubsystem->AddMappingContext(VendorMenuContext, 900);
	}
	bActive = true;
}

void UAvMainMenuInputRouter::Deactivate()
{
	if (EnhancedSubsystem && VendorMenuContext)
	{
		EnhancedSubsystem->RemoveMappingContext(VendorMenuContext);
	}
	EnhancedSubsystem = nullptr;
	VendorMenuContext = nullptr;
	Controller = nullptr;
	bActive = false;
	bVendorMenuContextLoaded = false;
}

bool UAvMainMenuInputRouter::IsBackKey(const FKey& Key)
{
	return Key == EKeys::Escape || Key == EKeys::BackSpace || Key == EKeys::RightMouseButton ||
		Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Virtual_Gamepad_Back.GetVirtualKey();
}

bool UAvMainMenuInputRouter::IsAcceptKey(const FKey& Key)
{
	return Key == EKeys::Enter || Key == EKeys::SpaceBar ||
		Key == EKeys::Gamepad_FaceButton_Bottom || Key == EKeys::Virtual_Gamepad_Accept.GetVirtualKey();
}

int32 UAvMainMenuInputRouter::GetVerticalDirection(const FKey& Key)
{
	if (Key == EKeys::Up || Key == EKeys::W || Key == EKeys::Gamepad_DPad_Up)
	{
		return -1;
	}
	if (Key == EKeys::Down || Key == EKeys::S || Key == EKeys::Gamepad_DPad_Down)
	{
		return 1;
	}
	return 0;
}

int32 UAvMainMenuInputRouter::GetHorizontalDirection(const FKey& Key)
{
	if (Key == EKeys::Left || Key == EKeys::A || Key == EKeys::Gamepad_DPad_Left)
	{
		return -1;
	}
	if (Key == EKeys::Right || Key == EKeys::D || Key == EKeys::Gamepad_DPad_Right)
	{
		return 1;
	}
	return 0;
}

TSharedRef<SWidget> UAvMainMenuActionButtonWidget::RebuildWidget()
{
	BuildTree();
	return Super::RebuildWidget();
}

void UAvMainMenuActionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	BuildTree();
	RefreshText();
	RefreshStyle();
}

void UAvMainMenuActionButtonWidget::BuildTree()
{
	if (WidgetTree->RootWidget)
	{
		return;
	}
	USizeBox* Size = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ButtonSize"));
	Size->SetMinDesiredHeight(50.f);
	Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Button"));
	Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Label"));
	AvMainMenu::ConfigureText(Label, 19, AvMainMenu::Text, true);
	Label->SetJustification(ETextJustify::Left);
	Button->AddChild(Label);
	Size->AddChild(Button);
	WidgetTree->RootWidget = Size;
	Button->OnClicked.AddDynamic(this, &UAvMainMenuActionButtonWidget::HandleClicked);
	Button->OnHovered.AddDynamic(this, &UAvMainMenuActionButtonWidget::HandleHovered);
	Button->OnUnhovered.AddDynamic(this, &UAvMainMenuActionButtonWidget::HandleUnhovered);
}

void UAvMainMenuActionButtonWidget::Configure(const int32 InActionId, const TCHAR* InTextKey,
	const bool bInEnabled)
{
	ActionId = InActionId;
	TextKey = InTextKey;
	SetEnabledState(bInEnabled);
	RefreshText();
}

void UAvMainMenuActionButtonWidget::SetEnabledState(const bool bInEnabled)
{
	bActionEnabled = bInEnabled;
	SetIsEnabled(bActionEnabled);
	RefreshStyle();
}

void UAvMainMenuActionButtonWidget::RefreshText()
{
	if (Label && !TextKey.IsEmpty())
	{
		Label->SetText(FAvLoc::Text(TextKey));
	}
}

FReply UAvMainMenuActionButtonWidget::NativeOnFocusReceived(const FGeometry& InGeometry,
	const FFocusEvent& InFocusEvent)
{
	bFocused = true;
	RefreshStyle();
	return FReply::Handled();
}

void UAvMainMenuActionButtonWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	bFocused = false;
	RefreshStyle();
	Super::NativeOnFocusLost(InFocusEvent);
}

void UAvMainMenuActionButtonWidget::HandleClicked()
{
	if (bActionEnabled)
	{
		OnActivated.Broadcast(ActionId);
	}
}

void UAvMainMenuActionButtonWidget::HandleHovered()
{
	bHovered = true;
	if (bActionEnabled)
	{
		SetKeyboardFocus();
	}
	RefreshStyle();
}

void UAvMainMenuActionButtonWidget::HandleUnhovered()
{
	bHovered = false;
	RefreshStyle();
}

void UAvMainMenuActionButtonWidget::RefreshStyle()
{
	if (!Button || !Label)
	{
		return;
	}
	const FLinearColor Normal = bActionEnabled ?
		(bFocused ? AvMainMenu::Focus : AvMainMenu::Idle) : AvMainMenu::Disabled;
	Button->SetStyle(AvMainMenu::ButtonStyle(Normal, AvMainMenu::Hover,
		AvMainMenu::Pressed, AvMainMenu::Disabled));
	Label->SetColorAndOpacity(FSlateColor(bActionEnabled ?
		(bFocused || bHovered ? AvMainMenu::OrangeSoft : AvMainMenu::Text) : AvMainMenu::DisabledText));
}

TSharedRef<SWidget> UAvMainMenuSettingRowWidget::RebuildWidget()
{
	BuildTree();
	return Super::RebuildWidget();
}

void UAvMainMenuSettingRowWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	BuildTree();
	RefreshLabel();
	RefreshStyle();
}

void UAvMainMenuSettingRowWidget::BuildTree()
{
	if (WidgetTree->RootWidget)
	{
		return;
	}
	Frame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Frame"));
	Frame->SetPadding(FMargin(12.f, 5.f));
	UHorizontalBox* RowBox = WidgetTree->ConstructWidget<UHorizontalBox>();
	Label = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(Label, 15, AvMainMenu::Text, true);
	UHorizontalBoxSlot* LabelSlot = RowBox->AddChildToHorizontalBox(Label);
	LabelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	LabelSlot->SetVerticalAlignment(VAlign_Center);
	LeftButton = WidgetTree->ConstructWidget<UButton>();
	LeftButton->SetStyle(AvMainMenu::ButtonStyle(AvMainMenu::Idle, AvMainMenu::Hover,
		AvMainMenu::Pressed, AvMainMenu::Disabled));
	UTextBlock* LeftText = WidgetTree->ConstructWidget<UTextBlock>();
	LeftText->SetText(FText::FromString(TEXT("<")));
	AvMainMenu::ConfigureText(LeftText, 20, AvMainMenu::Orange, true);
	LeftButton->AddChild(LeftText);
	RowBox->AddChildToHorizontalBox(LeftButton)->SetPadding(FMargin(4.f, 0.f));
	ValueText = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(ValueText, 15, AvMainMenu::OrangeSoft, true);
	ValueText->SetJustification(ETextJustify::Center);
	USizeBox* ValueSize = WidgetTree->ConstructWidget<USizeBox>();
	ValueSize->SetWidthOverride(170.f);
	ValueSize->AddChild(ValueText);
	RowBox->AddChildToHorizontalBox(ValueSize)->SetVerticalAlignment(VAlign_Center);
	RightButton = WidgetTree->ConstructWidget<UButton>();
	RightButton->SetStyle(AvMainMenu::ButtonStyle(AvMainMenu::Idle, AvMainMenu::Hover,
		AvMainMenu::Pressed, AvMainMenu::Disabled));
	UTextBlock* RightText = WidgetTree->ConstructWidget<UTextBlock>();
	RightText->SetText(FText::FromString(TEXT(">")));
	AvMainMenu::ConfigureText(RightText, 20, AvMainMenu::Orange, true);
	RightButton->AddChild(RightText);
	RowBox->AddChildToHorizontalBox(RightButton)->SetPadding(FMargin(4.f, 0.f));
	Frame->SetContent(RowBox);
	WidgetTree->RootWidget = Frame;
	LeftButton->OnClicked.AddDynamic(this, &UAvMainMenuSettingRowWidget::HandleLeft);
	RightButton->OnClicked.AddDynamic(this, &UAvMainMenuSettingRowWidget::HandleRight);
	LeftButton->OnHovered.AddDynamic(this, &UAvMainMenuSettingRowWidget::HandleHover);
	RightButton->OnHovered.AddDynamic(this, &UAvMainMenuSettingRowWidget::HandleHover);
	LeftButton->OnUnhovered.AddDynamic(this, &UAvMainMenuSettingRowWidget::HandleUnhover);
	RightButton->OnUnhovered.AddDynamic(this, &UAvMainMenuSettingRowWidget::HandleUnhover);
}

void UAvMainMenuSettingRowWidget::Configure(const EAvMainMenuSettingRow InRow,
	const TCHAR* InLabelKey, const bool bInEnabled)
{
	Row = InRow;
	LabelKey = InLabelKey;
	bRowEnabled = bInEnabled;
	SetIsEnabled(bRowEnabled);
	RefreshLabel();
	RefreshStyle();
}

void UAvMainMenuSettingRowWidget::SetValue(const FText& Value)
{
	if (ValueText)
	{
		ValueText->SetText(Value);
	}
}

void UAvMainMenuSettingRowWidget::RefreshLabel()
{
	if (Label && !LabelKey.IsEmpty())
	{
		Label->SetText(FAvLoc::Text(LabelKey));
	}
}

FReply UAvMainMenuSettingRowWidget::NativeOnFocusReceived(const FGeometry& InGeometry,
	const FFocusEvent& InFocusEvent)
{
	bFocused = true;
	RefreshStyle();
	return FReply::Handled();
}

void UAvMainMenuSettingRowWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	bFocused = false;
	RefreshStyle();
	Super::NativeOnFocusLost(InFocusEvent);
}

void UAvMainMenuSettingRowWidget::HandleLeft()
{
	if (bRowEnabled)
	{
		OnActivated.Broadcast(AvMainMenu::SettingAdjustmentBase + static_cast<int32>(Row) * 2);
	}
}

void UAvMainMenuSettingRowWidget::HandleRight()
{
	if (bRowEnabled)
	{
		OnActivated.Broadcast(AvMainMenu::SettingAdjustmentBase + static_cast<int32>(Row) * 2 + 1);
	}
}

void UAvMainMenuSettingRowWidget::HandleHover()
{
	bHovered = true;
	if (bRowEnabled)
	{
		SetKeyboardFocus();
	}
	RefreshStyle();
}

void UAvMainMenuSettingRowWidget::HandleUnhover()
{
	bHovered = false;
	RefreshStyle();
}

void UAvMainMenuSettingRowWidget::RefreshStyle()
{
	if (!Frame)
	{
		return;
	}
	Frame->SetBrushColor(!bRowEnabled ? AvMainMenu::Disabled :
		(bFocused ? AvMainMenu::Focus : (bHovered ? AvMainMenu::Hover : AvMainMenu::Idle)));
	if (Label)
	{
		Label->SetColorAndOpacity(FSlateColor(bRowEnabled ? AvMainMenu::Text : AvMainMenu::DisabledText));
	}
}

TSharedRef<SWidget> UAvMainMenuHomeWidget::RebuildWidget()
{
	BuildTree();
	return Super::RebuildWidget();
}

void UAvMainMenuHomeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildTree();
}

void UAvMainMenuHomeWidget::BuildTree()
{
	if (WidgetTree->RootWidget)
	{
		return;
	}
	ButtonList = WidgetTree->ConstructWidget<UVerticalBox>();
	WidgetTree->RootWidget = ButtonList;
}

void UAvMainMenuHomeWidget::InitializeHome(UAvMainMenuRootWidget* InRoot,
	const bool bContinueEnabled)
{
	Root = InRoot;
	BuildTree();
	if (!Buttons.IsEmpty())
	{
		Buttons[0]->SetEnabledState(bContinueEnabled);
		return;
	}
	struct FButtonDef { int32 Id; const TCHAR* Key; bool bEnabled; };
	const FButtonDef Definitions[] = {
		{AvMainMenu::ContinueAction, TEXT("MainMenu.Continue"), bContinueEnabled},
		{AvMainMenu::NewGameAction, TEXT("MainMenu.NewGame"), true},
		{AvMainMenu::MultiplayerAction, TEXT("MainMenu.Multiplayer"), false},
		{AvMainMenu::SettingsAction, TEXT("MainMenu.Settings"), true},
		{AvMainMenu::CreditsAction, TEXT("MainMenu.Credits"), true},
		{AvMainMenu::ExitAction, TEXT("MainMenu.Exit"), true}
	};
	for (const FButtonDef& Definition : Definitions)
	{
		UAvMainMenuActionButtonWidget* Entry = CreateWidget<UAvMainMenuActionButtonWidget>(
			GetOwningPlayer(), UAvMainMenuActionButtonWidget::StaticClass());
		Entry->Configure(Definition.Id, Definition.Key, Definition.bEnabled);
		Entry->OnActivated.AddDynamic(Root, &UAvMainMenuRootWidget::HandleAction);
		UVerticalBoxSlot* ButtonSlot = ButtonList->AddChildToVerticalBox(Entry);
		ButtonSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 9.f));
		Buttons.Add(Entry);
		if (Definition.Id == AvMainMenu::MultiplayerAction)
		{
			ComingSoonLabel = WidgetTree->ConstructWidget<UTextBlock>();
			ComingSoonLabel->SetText(FAvLoc::Text(TEXT("MainMenu.ComingSoon")));
			AvMainMenu::ConfigureText(ComingSoonLabel, 12, AvMainMenu::Muted);
			ComingSoonLabel->SetJustification(ETextJustify::Right);
			ButtonList->AddChildToVerticalBox(ComingSoonLabel)->SetPadding(FMargin(0.f, -4.f, 18.f, 9.f));
		}
	}
}

void UAvMainMenuHomeWidget::RefreshText()
{
	for (UAvMainMenuActionButtonWidget* ButtonWidget : Buttons)
	{
		if (ButtonWidget)
		{
			ButtonWidget->RefreshText();
		}
	}
	if (ComingSoonLabel)
	{
		ComingSoonLabel->SetText(FAvLoc::Text(TEXT("MainMenu.ComingSoon")));
	}
}

TSharedRef<SWidget> UAvMainMenuConfirmDialogWidget::RebuildWidget()
{
	BuildTree();
	return Super::RebuildWidget();
}

void UAvMainMenuConfirmDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildTree();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UAvMainMenuConfirmDialogWidget::InitializeDialog()
{
	BuildTree();
}

void UAvMainMenuConfirmDialogWidget::BuildTree()
{
	if (WidgetTree->RootWidget)
	{
		return;
	}
	UOverlay* RootOverlay = WidgetTree->ConstructWidget<UOverlay>();
	UBorder* Dim = WidgetTree->ConstructWidget<UBorder>();
	Dim->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.76f));
	RootOverlay->AddChildToOverlay(Dim);
	USizeBox* ModalSize = WidgetTree->ConstructWidget<USizeBox>();
	ModalSize->SetWidthOverride(570.f);
	UBorder* ModalFrame = WidgetTree->ConstructWidget<UBorder>();
	ModalFrame->SetBrushColor(AvMainMenu::Panel);
	ModalFrame->SetPadding(FMargin(28.f));
	UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>();
	TitleText = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(TitleText, 26, AvMainMenu::Orange, true);
	Box->AddChildToVerticalBox(TitleText)->SetPadding(FMargin(0.f, 0.f, 0.f, 14.f));
	BodyText = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(BodyText, 17, AvMainMenu::Text);
	Box->AddChildToVerticalBox(BodyText)->SetPadding(FMargin(0.f, 0.f, 0.f, 22.f));
	UHorizontalBox* Actions = WidgetTree->ConstructWidget<UHorizontalBox>();
	YesButton = WidgetTree->ConstructWidget<UAvMainMenuActionButtonWidget>(
		UAvMainMenuActionButtonWidget::StaticClass(), TEXT("YesButton"));
	YesButton->Configure(AvMainMenu::ModalYesAction, TEXT("MainMenu.Common.Yes"));
	NoButton = WidgetTree->ConstructWidget<UAvMainMenuActionButtonWidget>(
		UAvMainMenuActionButtonWidget::StaticClass(), TEXT("NoButton"));
	NoButton->Configure(AvMainMenu::ModalNoAction, TEXT("MainMenu.Common.No"));
	UHorizontalBoxSlot* YesSlot = Actions->AddChildToHorizontalBox(YesButton);
	YesSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	YesSlot->SetPadding(FMargin(0.f, 0.f, 8.f, 0.f));
	UHorizontalBoxSlot* NoSlot = Actions->AddChildToHorizontalBox(NoButton);
	NoSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	NoSlot->SetPadding(FMargin(8.f, 0.f, 0.f, 0.f));
	Box->AddChildToVerticalBox(Actions);
	ModalFrame->SetContent(Box);
	ModalSize->AddChild(ModalFrame);
	UOverlaySlot* ModalSlot = RootOverlay->AddChildToOverlay(ModalSize);
	ModalSlot->SetHorizontalAlignment(HAlign_Center);
	ModalSlot->SetVerticalAlignment(VAlign_Center);
	WidgetTree->RootWidget = RootOverlay;
}

void UAvMainMenuConfirmDialogWidget::ShowDialog(const FText& Title, const FText& Body,
	const bool bShowYes)
{
	CurrentTitle = Title;
	CurrentBody = Body;
	if (TitleText) TitleText->SetText(CurrentTitle);
	if (BodyText) BodyText->SetText(CurrentBody);
	if (YesButton) YesButton->SetVisibility(bShowYes ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	SetVisibility(ESlateVisibility::Visible);
}

void UAvMainMenuConfirmDialogWidget::HideDialog()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UAvMainMenuConfirmDialogWidget::RefreshText()
{
	if (YesButton) YesButton->RefreshText();
	if (NoButton) NoButton->RefreshText();
}

TSharedRef<SWidget> UAvMainMenuSettingsHostWidget::RebuildWidget()
{
	BuildTree();
	return Super::RebuildWidget();
}

void UAvMainMenuSettingsHostWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildTree();
}

void UAvMainMenuSettingsHostWidget::BuildTree()
{
	if (WidgetTree->RootWidget)
	{
		return;
	}
	UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>();
	Heading = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(Heading, 25, AvMainMenu::Orange, true);
	RootBox->AddChildToVerticalBox(Heading)->SetPadding(FMargin(0.f, 0.f, 0.f, 10.f));
	Scroll = WidgetTree->ConstructWidget<UScrollBox>();
	Scroll->SetAnimateWheelScrolling(true);
	Scroll->SetScrollBarVisibility(ESlateVisibility::Hidden);
	USizeBox* ScrollViewport = WidgetTree->ConstructWidget<USizeBox>();
	ScrollViewport->SetMinDesiredHeight(350.f);
	ScrollViewport->AddChild(Scroll);
	UVerticalBoxSlot* ScrollSlot = RootBox->AddChildToVerticalBox(ScrollViewport);
	ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	AudioNote = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(AudioNote, 11, AvMainMenu::Muted);
	RootBox->AddChildToVerticalBox(AudioNote)->SetPadding(FMargin(0.f, 4.f, 0.f, 8.f));
	UHorizontalBox* Actions = WidgetTree->ConstructWidget<UHorizontalBox>();
	ApplyButton = WidgetTree->ConstructWidget<UAvMainMenuActionButtonWidget>(
		UAvMainMenuActionButtonWidget::StaticClass(), TEXT("ApplyButton"));
	ApplyButton->Configure(AvMainMenu::SettingsApplyAction, TEXT("MainMenu.Settings.Apply"));
	RevertButton = WidgetTree->ConstructWidget<UAvMainMenuActionButtonWidget>(
		UAvMainMenuActionButtonWidget::StaticClass(), TEXT("RevertButton"));
	RevertButton->Configure(AvMainMenu::SettingsRevertAction, TEXT("MainMenu.Settings.Revert"));
	BackButton = WidgetTree->ConstructWidget<UAvMainMenuActionButtonWidget>(
		UAvMainMenuActionButtonWidget::StaticClass(), TEXT("BackButton"));
	BackButton->Configure(AvMainMenu::SettingsBackAction, TEXT("MainMenu.Common.Back"));
	for (UAvMainMenuActionButtonWidget* ButtonWidget : {ApplyButton, RevertButton})
	{
		UHorizontalBoxSlot* ActionSlot = Actions->AddChildToHorizontalBox(ButtonWidget);
		ActionSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		ActionSlot->SetPadding(FMargin(3.f));
	}
	RootBox->AddChildToVerticalBox(Actions);
	RootBox->AddChildToVerticalBox(BackButton)->SetPadding(FMargin(3.f, 4.f, 3.f, 0.f));
	WidgetTree->RootWidget = RootBox;
}

void UAvMainMenuSettingsHostWidget::InitializeSettings(UAvMainMenuRootWidget* InRoot,
	UAvMainMenuSettingsAdapter* InAdapter)
{
	Root = InRoot;
	Adapter = InAdapter;
	BuildTree();
	if (Rows.IsEmpty())
	{
		struct FRowDef { EAvMainMenuSettingRow Row; const TCHAR* Key; bool bEnabled; };
		const FRowDef Definitions[] = {
			{EAvMainMenuSettingRow::Resolution, TEXT("MainMenu.Settings.Resolution"), true},
			{EAvMainMenuSettingRow::WindowMode, TEXT("MainMenu.Settings.WindowMode"), true},
			{EAvMainMenuSettingRow::VSync, TEXT("MainMenu.Settings.VSync"), true},
			{EAvMainMenuSettingRow::OverallQuality, TEXT("MainMenu.Settings.OverallQuality"), true},
			{EAvMainMenuSettingRow::MasterVolume, TEXT("MainMenu.Settings.Master"), true},
			{EAvMainMenuSettingRow::SFXVolume, TEXT("MainMenu.Settings.SFX"), false},
			{EAvMainMenuSettingRow::MusicVolume, TEXT("MainMenu.Settings.Music"), false},
			{EAvMainMenuSettingRow::VoiceVolume, TEXT("MainMenu.Settings.Voice"), false},
			{EAvMainMenuSettingRow::Language, TEXT("MainMenu.Settings.Language"), true},
			{EAvMainMenuSettingRow::Controls, TEXT("MainMenu.Settings.Controls"), false}
		};
		for (const FRowDef& Definition : Definitions)
		{
			UAvMainMenuSettingRowWidget* Entry = CreateWidget<UAvMainMenuSettingRowWidget>(GetOwningPlayer());
			Entry->Configure(Definition.Row, Definition.Key, Definition.bEnabled);
			Entry->OnActivated.AddDynamic(Root, &UAvMainMenuRootWidget::HandleAction);
			Scroll->AddChild(Entry);
			Rows.Add(Entry);
		}
		ApplyButton->OnActivated.AddDynamic(Root, &UAvMainMenuRootWidget::HandleAction);
		RevertButton->OnActivated.AddDynamic(Root, &UAvMainMenuRootWidget::HandleAction);
		BackButton->OnActivated.AddDynamic(Root, &UAvMainMenuRootWidget::HandleAction);
	}
	RefreshAll();
}

void UAvMainMenuSettingsHostWidget::RefreshAll()
{
	if (Heading) Heading->SetText(FAvLoc::Text(TEXT("MainMenu.Settings.Title")));
	if (AudioNote) AudioNote->SetText(FAvLoc::Text(TEXT("MainMenu.Settings.AudioRoutingDeferred")));
	for (UAvMainMenuSettingRowWidget* RowWidget : Rows)
	{
		if (RowWidget)
		{
			RowWidget->RefreshLabel();
			if (Adapter) RowWidget->SetValue(Adapter->GetValueText(RowWidget->GetRow()));
		}
	}
	for (UAvMainMenuActionButtonWidget* ButtonWidget : {ApplyButton, RevertButton, BackButton})
	{
		if (ButtonWidget) ButtonWidget->RefreshText();
	}
}

TSharedRef<SWidget> UAvMainMenuCreditsWidget::RebuildWidget()
{
	BuildTree();
	return Super::RebuildWidget();
}

void UAvMainMenuCreditsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildTree();
	RefreshText();
}

void UAvMainMenuCreditsWidget::InitializeCredits()
{
	BuildTree();
}

void UAvMainMenuCreditsWidget::BuildTree()
{
	if (WidgetTree->RootWidget)
	{
		return;
	}
	UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>();
	Heading = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(Heading, 27, AvMainMenu::Orange, true);
	RootBox->AddChildToVerticalBox(Heading)->SetPadding(FMargin(0.f, 0.f, 0.f, 18.f));
	Body = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(Body, 17, AvMainMenu::Text);
	UVerticalBoxSlot* BodySlot = RootBox->AddChildToVerticalBox(Body);
	BodySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BackButton = WidgetTree->ConstructWidget<UAvMainMenuActionButtonWidget>(
		UAvMainMenuActionButtonWidget::StaticClass(), TEXT("BackButton"));
	BackButton->Configure(AvMainMenu::CreditsBackAction, TEXT("MainMenu.Common.Back"));
	RootBox->AddChildToVerticalBox(BackButton);
	WidgetTree->RootWidget = RootBox;
}

void UAvMainMenuCreditsWidget::RefreshText()
{
	if (Heading) Heading->SetText(FAvLoc::Text(TEXT("MainMenu.Credits.Title")));
	if (Body) Body->SetText(FAvLoc::Text(TEXT("MainMenu.Credits.Placeholder")));
	if (BackButton) BackButton->RefreshText();
}

UClass* UAvMainMenuRootWidget::ResolveWidgetClass(const TCHAR* Path, UClass* Fallback) const
{
	if (UClass* Loaded = LoadClass<UUserWidget>(nullptr, Path))
	{
		return Loaded;
	}
	return Fallback;
}

TSharedRef<SWidget> UAvMainMenuRootWidget::RebuildWidget()
{
	BuildTree();
	return Super::RebuildWidget();
}

void UAvMainMenuRootWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	BuildTree();
	Coordinator = NewObject<UAvMainMenuCoordinator>(this);
	InputRouter = NewObject<UAvMainMenuInputRouter>(this);
	SettingsAdapter = NewObject<UAvMainMenuSettingsAdapter>(this);
	SavePresenceAdapter = NewObject<UAvMainMenuSavePresenceAdapter>(this);
	SettingsAdapter->Initialize(GetWorld());
	bContinueEnabled = SavePresenceAdapter->Refresh();
	CreateScreens();
	RefreshLocalizedText();
	ShowScreen(EAvMainMenuScreen::Home, AvMainMenu::ContinueAction);
	InputRouter->Activate(GetOwningPlayer(), FocusWidgets.IsValidIndex(FocusIndex) ? FocusWidgets[FocusIndex] : this);
	ApplyFocus();
	UE_LOG(LogTemp, Log,
		TEXT("[AvMainMenu] Authored root active. Continue=%s VendorIMC=%s Culture=%s"),
		bContinueEnabled ? TEXT("true") : TEXT("false"),
		InputRouter->HasVendorMenuContext() ? TEXT("loaded") : TEXT("missing"),
		*FInternationalization::Get().GetCurrentCulture()->GetName());
}

void UAvMainMenuRootWidget::NativeDestruct()
{
	if (SettingsAdapter)
	{
		SettingsAdapter->RevertPending();
	}
	if (InputRouter)
	{
		InputRouter->Deactivate();
	}
	Super::NativeDestruct();
}

void UAvMainMenuRootWidget::BuildTree()
{
	if (WidgetTree->RootWidget)
	{
		return;
	}
	UOverlay* RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
	UBorder* Background = WidgetTree->ConstructWidget<UBorder>();
	Background->SetBrushColor(AvMainMenu::Background);
	UOverlaySlot* BackgroundSlot = RootOverlay->AddChildToOverlay(Background);
	BackgroundSlot->SetHorizontalAlignment(HAlign_Fill);
	BackgroundSlot->SetVerticalAlignment(VAlign_Fill);

	UBorder* TopAccent = WidgetTree->ConstructWidget<UBorder>();
	TopAccent->SetBrushColor(AvMainMenu::Orange);
	USizeBox* TopAccentSize = WidgetTree->ConstructWidget<USizeBox>();
	TopAccentSize->SetHeightOverride(4.f);
	TopAccentSize->AddChild(TopAccent);
	UOverlaySlot* TopAccentSlot = RootOverlay->AddChildToOverlay(TopAccentSize);
	TopAccentSlot->SetHorizontalAlignment(HAlign_Fill);
	TopAccentSlot->SetVerticalAlignment(VAlign_Top);

	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>();
	PanelSize->SetWidthOverride(500.f);
	UBorder* PanelFrame = WidgetTree->ConstructWidget<UBorder>();
	PanelFrame->SetBrushColor(AvMainMenu::Panel);
	PanelFrame->SetPadding(FMargin(28.f, 24.f));
	UVerticalBox* PanelContent = WidgetTree->ConstructWidget<UVerticalBox>();
	BrandTitle = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(BrandTitle, 42, AvMainMenu::Orange, true);
	PanelContent->AddChildToVerticalBox(BrandTitle);
	BrandSubtitle = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(BrandSubtitle, 14, AvMainMenu::Muted);
	PanelContent->AddChildToVerticalBox(BrandSubtitle)->SetPadding(FMargin(0.f, 0.f, 0.f, 18.f));
	UBorder* Divider = WidgetTree->ConstructWidget<UBorder>();
	Divider->SetBrushColor(AvMainMenu::Orange);
	USizeBox* DividerSize = WidgetTree->ConstructWidget<USizeBox>();
	DividerSize->SetHeightOverride(2.f);
	DividerSize->AddChild(Divider);
	PanelContent->AddChildToVerticalBox(DividerSize)->SetPadding(FMargin(0.f, 0.f, 0.f, 18.f));
	ScreenSwitcher = WidgetTree->ConstructWidget<UWidgetSwitcher>();
	UVerticalBoxSlot* ScreenSlot = PanelContent->AddChildToVerticalBox(ScreenSwitcher);
	ScreenSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	FooterHint = WidgetTree->ConstructWidget<UTextBlock>();
	AvMainMenu::ConfigureText(FooterHint, 11, AvMainMenu::Muted);
	PanelContent->AddChildToVerticalBox(FooterHint)->SetPadding(FMargin(0.f, 14.f, 0.f, 0.f));
	PanelFrame->SetContent(PanelContent);
	PanelSize->AddChild(PanelFrame);
	UOverlaySlot* PanelSlot = RootOverlay->AddChildToOverlay(PanelSize);
	PanelSlot->SetPadding(FMargin(48.f, 42.f, 0.f, 42.f));
	PanelSlot->SetHorizontalAlignment(HAlign_Left);
	PanelSlot->SetVerticalAlignment(VAlign_Fill);

	UBorder* FutureSceneLine = WidgetTree->ConstructWidget<UBorder>();
	FutureSceneLine->SetBrushColor(FLinearColor(1.f, 0.36f, 0.055f, 0.16f));
	USizeBox* FutureSceneLineSize = WidgetTree->ConstructWidget<USizeBox>();
	FutureSceneLineSize->SetWidthOverride(2.f);
	FutureSceneLineSize->AddChild(FutureSceneLine);
	UOverlaySlot* LineSlot = RootOverlay->AddChildToOverlay(FutureSceneLineSize);
	LineSlot->SetPadding(FMargin(0.f, 72.f, 12.f, 72.f));
	LineSlot->SetHorizontalAlignment(HAlign_Right);
	LineSlot->SetVerticalAlignment(VAlign_Fill);

	ModalDialog = WidgetTree->ConstructWidget<UAvMainMenuConfirmDialogWidget>(
		ResolveWidgetClass(TEXT("/Game/Avariika/UI/MainMenu/WBP_AvMainMenuConfirmDialog.WBP_AvMainMenuConfirmDialog_C"),
			UAvMainMenuConfirmDialogWidget::StaticClass()), TEXT("ModalDialog"));
	UOverlaySlot* ModalSlot = RootOverlay->AddChildToOverlay(ModalDialog);
	ModalSlot->SetHorizontalAlignment(HAlign_Fill);
	ModalSlot->SetVerticalAlignment(VAlign_Fill);
	WidgetTree->RootWidget = RootOverlay;
}

void UAvMainMenuRootWidget::CreateScreens()
{
	if (Home)
	{
		return;
	}
	Home = CreateWidget<UAvMainMenuHomeWidget>(GetOwningPlayer(),
		ResolveWidgetClass(TEXT("/Game/Avariika/UI/MainMenu/WBP_AvMainMenuHome.WBP_AvMainMenuHome_C"),
			UAvMainMenuHomeWidget::StaticClass()));
	Settings = CreateWidget<UAvMainMenuSettingsHostWidget>(GetOwningPlayer(),
		ResolveWidgetClass(TEXT("/Game/Avariika/UI/MainMenu/WBP_AvMainMenuSettingsHost.WBP_AvMainMenuSettingsHost_C"),
			UAvMainMenuSettingsHostWidget::StaticClass()));
	Credits = CreateWidget<UAvMainMenuCreditsWidget>(GetOwningPlayer(),
		ResolveWidgetClass(TEXT("/Game/Avariika/UI/MainMenu/WBP_AvMainMenuCredits.WBP_AvMainMenuCredits_C"),
			UAvMainMenuCreditsWidget::StaticClass()));
	ScreenSwitcher->AddChild(Home);
	ScreenSwitcher->AddChild(Settings);
	ScreenSwitcher->AddChild(Credits);
	Home->InitializeHome(this, bContinueEnabled);
	Settings->InitializeSettings(this, SettingsAdapter);
	Credits->InitializeCredits();
	ModalDialog->InitializeDialog();
	Credits->GetBackButton()->OnActivated.AddDynamic(this, &UAvMainMenuRootWidget::HandleAction);
	ModalDialog->GetYesButton()->OnActivated.AddDynamic(this, &UAvMainMenuRootWidget::HandleAction);
	ModalDialog->GetNoButton()->OnActivated.AddDynamic(this, &UAvMainMenuRootWidget::HandleAction);
}

void UAvMainMenuRootWidget::RefreshLocalizedText()
{
	if (BrandTitle) BrandTitle->SetText(FAvLoc::Text(TEXT("MainMenu.Title")));
	if (BrandSubtitle) BrandSubtitle->SetText(FAvLoc::Text(TEXT("MainMenu.Subtitle")));
	if (FooterHint) FooterHint->SetText(FAvLoc::Text(TEXT("MainMenu.InputHint")));
	if (Home) Home->RefreshText();
	if (Settings) Settings->RefreshAll();
	if (Credits) Credits->RefreshText();
	if (ModalDialog) ModalDialog->RefreshText();
}

void UAvMainMenuRootWidget::ShowScreen(const EAvMainMenuScreen Screen, const int32 RestoreAction)
{
	if (!Coordinator || !ScreenSwitcher)
	{
		return;
	}
	Coordinator->SetScreen(Screen);
	ScreenSwitcher->SetActiveWidgetIndex(static_cast<int32>(Screen));
	RebuildFocusList(RestoreAction);
	ApplyFocus();
}

void UAvMainMenuRootWidget::RebuildFocusList(const int32 PreferredAction)
{
	FocusWidgets.Reset();
	FocusActionIds.Reset();
	if (Coordinator->HasModal())
	{
		if (ModalDialog->GetYesButton()->GetVisibility() != ESlateVisibility::Collapsed)
		{
			FocusWidgets.Add(ModalDialog->GetYesButton());
			FocusActionIds.Add(AvMainMenu::ModalYesAction);
		}
		FocusWidgets.Add(ModalDialog->GetNoButton());
		FocusActionIds.Add(AvMainMenu::ModalNoAction);
	}
	else if (Coordinator->GetScreen() == EAvMainMenuScreen::Home)
	{
		for (UAvMainMenuActionButtonWidget* ButtonWidget : Home->GetButtons())
		{
			if (ButtonWidget && ButtonWidget->IsActionEnabled())
			{
				FocusWidgets.Add(ButtonWidget);
				FocusActionIds.Add(ButtonWidget->GetActionId());
			}
		}
	}
	else if (Coordinator->GetScreen() == EAvMainMenuScreen::Settings)
	{
		for (UAvMainMenuSettingRowWidget* RowWidget : Settings->GetRows())
		{
			if (RowWidget && RowWidget->IsRowEnabled())
			{
				FocusWidgets.Add(RowWidget);
				FocusActionIds.Add(100 + static_cast<int32>(RowWidget->GetRow()));
			}
		}
		for (UAvMainMenuActionButtonWidget* ButtonWidget : {
			Settings->GetApplyButton(), Settings->GetRevertButton(), Settings->GetBackButton()})
		{
			FocusWidgets.Add(ButtonWidget);
			FocusActionIds.Add(ButtonWidget->GetActionId());
		}
	}
	else
	{
		FocusWidgets.Add(Credits->GetBackButton());
		FocusActionIds.Add(AvMainMenu::CreditsBackAction);
	}
	FocusIndex = 0;
	if (PreferredAction != INDEX_NONE)
	{
		const int32 PreferredIndex = FocusActionIds.IndexOfByKey(PreferredAction);
		if (PreferredIndex != INDEX_NONE)
		{
			FocusIndex = PreferredIndex;
		}
	}
}

void UAvMainMenuRootWidget::ApplyFocus()
{
	if (FocusWidgets.IsValidIndex(FocusIndex) && FocusWidgets[FocusIndex])
	{
		FocusWidgets[FocusIndex]->SetKeyboardFocus();
	}
}

void UAvMainMenuRootWidget::SyncFocusIndexFromSlateFocus()
{
	for (int32 Index = 0; Index < FocusWidgets.Num(); ++Index)
	{
		if (FocusWidgets[Index] && FocusWidgets[Index]->HasKeyboardFocus())
		{
			FocusIndex = Index;
			return;
		}
	}
}

void UAvMainMenuRootWidget::MoveFocus(const int32 Direction)
{
	if (Direction == 0 || FocusWidgets.IsEmpty())
	{
		return;
	}
	SyncFocusIndexFromSlateFocus();
	FocusIndex = (FocusIndex + FMath::Sign(Direction) + FocusWidgets.Num()) % FocusWidgets.Num();
	ApplyFocus();
}

void UAvMainMenuRootWidget::AdjustFocusedSetting(const int32 Direction)
{
	if (!Coordinator || Coordinator->HasModal() ||
		Coordinator->GetScreen() != EAvMainMenuScreen::Settings ||
		!FocusActionIds.IsValidIndex(FocusIndex))
	{
		return;
	}
	SyncFocusIndexFromSlateFocus();
	const int32 Action = FocusActionIds[FocusIndex];
	if (Action >= 100 && Action < 100 + static_cast<int32>(EAvMainMenuSettingRow::Controls) + 1)
	{
		HandleSettingAdjustment(static_cast<EAvMainMenuSettingRow>(Action - 100), Direction);
	}
}

void UAvMainMenuRootWidget::AcceptFocused()
{
	SyncFocusIndexFromSlateFocus();
	if (!FocusActionIds.IsValidIndex(FocusIndex))
	{
		return;
	}
	const int32 Action = FocusActionIds[FocusIndex];
	if (Action >= 100 && Action < 100 + static_cast<int32>(EAvMainMenuSettingRow::Controls) + 1)
	{
		HandleSettingAdjustment(static_cast<EAvMainMenuSettingRow>(Action - 100), 1);
	}
	else
	{
		HandleAction(Action);
	}
}

FReply UAvMainMenuRootWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (UAvMainMenuInputRouter::IsBackKey(Key))
	{
		DevBack();
		return FReply::Handled();
	}
	if (const int32 Vertical = UAvMainMenuInputRouter::GetVerticalDirection(Key))
	{
		MoveFocus(Vertical);
		return FReply::Handled();
	}
	if (const int32 Horizontal = UAvMainMenuInputRouter::GetHorizontalDirection(Key))
	{
		AdjustFocusedSetting(Horizontal);
		return FReply::Handled();
	}
	if (UAvMainMenuInputRouter::IsAcceptKey(Key))
	{
		AcceptFocused();
		return FReply::Handled();
	}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UAvMainMenuRootWidget::NativeOnAnalogValueChanged(const FGeometry& InGeometry,
	const FAnalogInputEvent& InAnalogEvent)
{
	const float Now = GetWorld() ? GetWorld()->GetRealTimeSeconds() : 0.f;
	if (Now - LastAnalogNavigationTime < 0.18f)
	{
		return FReply::Handled();
	}
	const FKey Key = InAnalogEvent.GetKey();
	const float Value = InAnalogEvent.GetAnalogValue();
	if (Key == EKeys::Gamepad_LeftY && FMath::Abs(Value) > 0.55f)
	{
		MoveFocus(Value > 0.f ? -1 : 1);
		LastAnalogNavigationTime = Now;
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_LeftX && FMath::Abs(Value) > 0.55f)
	{
		AdjustFocusedSetting(Value > 0.f ? 1 : -1);
		LastAnalogNavigationTime = Now;
		return FReply::Handled();
	}
	return Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);
}

void UAvMainMenuRootWidget::HandleSettingAdjustment(const EAvMainMenuSettingRow Row,
	const int32 Direction)
{
	if (!SettingsAdapter)
	{
		return;
	}
	SettingsAdapter->Adjust(Row, Direction);
	Settings->RefreshAll();
	if (Row == EAvMainMenuSettingRow::Language)
	{
		RefreshLocalizedText();
	}
}

void UAvMainMenuRootWidget::HandleAction(const int32 ActionId)
{
	if (ActionId >= AvMainMenu::SettingAdjustmentBase)
	{
		const int32 Encoded = ActionId - AvMainMenu::SettingAdjustmentBase;
		HandleSettingAdjustment(static_cast<EAvMainMenuSettingRow>(Encoded / 2),
			(Encoded % 2) == 0 ? -1 : 1);
		return;
	}
	if (ActionId == AvMainMenu::ModalYesAction || ActionId == AvMainMenu::ModalNoAction)
	{
		ConfirmModal(ActionId == AvMainMenu::ModalYesAction);
		return;
	}
	if (ActionId == AvMainMenu::ContinueAction && bContinueEnabled)
	{
		OpenModal(EAvMainMenuModal::ContinueInfo);
	}
	else if (ActionId == AvMainMenu::NewGameAction)
	{
		if (SavePresenceAdapter->Refresh())
		{
			OpenModal(EAvMainMenuModal::NewGame);
		}
		else
		{
			SetLastSafeAction(TEXT("NewGameFutureHook"));
			OpenModal(EAvMainMenuModal::ContinueInfo);
		}
	}
	else if (ActionId == AvMainMenu::SettingsAction)
	{
		ShowScreen(EAvMainMenuScreen::Settings);
	}
	else if (ActionId == AvMainMenu::CreditsAction)
	{
		ShowScreen(EAvMainMenuScreen::Credits);
	}
	else if (ActionId == AvMainMenu::ExitAction)
	{
		OpenModal(EAvMainMenuModal::Exit);
	}
	else if (ActionId == AvMainMenu::SettingsApplyAction)
	{
		bool bNeedsConfirmation = false;
		SettingsAdapter->BeginApply(bNeedsConfirmation);
		if (bNeedsConfirmation)
		{
			OpenModal(EAvMainMenuModal::DisplayConfirm);
		}
		else
		{
			SettingsAdapter->ConfirmApply();
			SetLastSafeAction(TEXT("SettingsApplied"));
		}
	}
	else if (ActionId == AvMainMenu::SettingsRevertAction)
	{
		SettingsAdapter->RevertPending();
		RefreshLocalizedText();
		Settings->RefreshAll();
	}
	else if (ActionId == AvMainMenu::SettingsBackAction)
	{
		SettingsAdapter->RevertPending();
		RefreshLocalizedText();
		ShowScreen(EAvMainMenuScreen::Home, AvMainMenu::SettingsAction);
	}
	else if (ActionId == AvMainMenu::CreditsBackAction)
	{
		ShowScreen(EAvMainMenuScreen::Home, AvMainMenu::CreditsAction);
	}
}

void UAvMainMenuRootWidget::OpenModal(const EAvMainMenuModal Modal)
{
	if (!Coordinator || Coordinator->HasModal())
	{
		return;
	}
	ModalReturnAction = FocusActionIds.IsValidIndex(FocusIndex) ? FocusActionIds[FocusIndex] : INDEX_NONE;
	Coordinator->SetModal(Modal);
	FText Title;
	FText Body;
	bool bShowYes = true;
	switch (Modal)
	{
	case EAvMainMenuModal::NewGame:
		Title = FAvLoc::Text(TEXT("MainMenu.Confirm.NewGame.Title"));
		Body = FAvLoc::Text(TEXT("MainMenu.Confirm.NewGame.Body"));
		break;
	case EAvMainMenuModal::Exit:
		Title = FAvLoc::Text(TEXT("MainMenu.Confirm.Exit.Title"));
		Body = FAvLoc::Text(TEXT("MainMenu.Confirm.Exit.Body"));
		break;
	case EAvMainMenuModal::DisplayConfirm:
		Title = FAvLoc::Text(TEXT("MainMenu.Confirm.Display.Title"));
		Body = FAvLoc::Text(TEXT("MainMenu.Confirm.Display.Body"));
		break;
	default:
		Title = FAvLoc::Text(TEXT("MainMenu.Info.Title"));
		Body = FAvLoc::Text(TEXT("MainMenu.Info.FutureRoute"));
		bShowYes = false;
		break;
	}
	ModalDialog->ShowDialog(Title, Body, bShowYes);
	RebuildFocusList(bShowYes ? AvMainMenu::ModalNoAction : AvMainMenu::ModalNoAction);
	ApplyFocus();
}

void UAvMainMenuRootWidget::CloseModal(const bool bRestoreFocus)
{
	if (!Coordinator || !Coordinator->HasModal())
	{
		return;
	}
	Coordinator->SetModal(EAvMainMenuModal::None);
	ModalDialog->HideDialog();
	RebuildFocusList(bRestoreFocus ? ModalReturnAction : INDEX_NONE);
	ApplyFocus();
}

bool UAvMainMenuRootWidget::IsPIE() const
{
	return GetWorld() && GetWorld()->WorldType == EWorldType::PIE;
}

void UAvMainMenuRootWidget::ConfirmModal(const bool bYes)
{
	if (!Coordinator || !Coordinator->HasModal())
	{
		return;
	}
	const EAvMainMenuModal Modal = Coordinator->GetModal();
	if (!bYes)
	{
		if (Modal == EAvMainMenuModal::DisplayConfirm)
		{
			SettingsAdapter->RevertDisplayPreview();
			Settings->RefreshAll();
			RefreshLocalizedText();
		}
		SetLastSafeAction(TEXT("ModalCancelled"));
		CloseModal();
		return;
	}
	if (Modal == EAvMainMenuModal::DisplayConfirm)
	{
		SettingsAdapter->ConfirmApply();
		SetLastSafeAction(TEXT("DisplayConfirmed"));
	}
	else if (Modal == EAvMainMenuModal::NewGame)
	{
		SetLastSafeAction(TEXT("NewGameFutureHook_NoMutation"));
		UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] New Game future hook accepted; no SaveGame mutation."));
	}
	else if (Modal == EAvMainMenuModal::Exit)
	{
		if (IsPIE())
		{
			SetLastSafeAction(TEXT("ExitPIESafe"));
			UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] Exit accepted in PIE; Editor remains open."));
		}
		else
		{
			SetLastSafeAction(TEXT("ExitRequested"));
			UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
		}
	}
	CloseModal();
}

void UAvMainMenuRootWidget::SetLastSafeAction(const TCHAR* Action)
{
	LastSafeAction = Action;
}

void UAvMainMenuRootWidget::DevNavigate(const int32 Direction)
{
	MoveFocus(Direction);
}

void UAvMainMenuRootWidget::DevAdjust(const int32 Direction)
{
	AdjustFocusedSetting(Direction);
}

void UAvMainMenuRootWidget::DevAccept()
{
	AcceptFocused();
}

void UAvMainMenuRootWidget::DevBack()
{
	if (Coordinator->HasModal())
	{
		ConfirmModal(false);
	}
	else if (Coordinator->GetScreen() == EAvMainMenuScreen::Settings)
	{
		SettingsAdapter->RevertPending();
		RefreshLocalizedText();
		ShowScreen(EAvMainMenuScreen::Home, AvMainMenu::SettingsAction);
	}
	else if (Coordinator->GetScreen() == EAvMainMenuScreen::Credits)
	{
		ShowScreen(EAvMainMenuScreen::Home, AvMainMenu::CreditsAction);
	}
	else
	{
		SetLastSafeAction(TEXT("BackIgnoredAtRoot"));
	}
}

void UAvMainMenuRootWidget::DevActivateHomeAction(const int32 ActionId)
{
	HandleAction(ActionId);
}

void UAvMainMenuRootWidget::DevConfirmModal(const bool bYes)
{
	ConfirmModal(bYes);
}

void UAvMainMenuRootWidget::DevSetCulture(const FString& Culture)
{
	if (Culture == TEXT("ru") || Culture == TEXT("en"))
	{
		FInternationalization::Get().SetCurrentCulture(Culture);
		RefreshLocalizedText();
	}
}

FString UAvMainMenuRootWidget::GetDiagnosticState() const
{
	const FVector2D Size = GetCachedGeometry().GetLocalSize();
	const int32 FocusedAction = FocusActionIds.IsValidIndex(FocusIndex) ? FocusActionIds[FocusIndex] : INDEX_NONE;
	return FString::Printf(
		TEXT("{\"screen\":%d,\"modal\":%d,\"focused_action\":%d,\"continue_enabled\":%s,")
		TEXT("\"vendor_imc\":%s,\"culture\":\"%s\",\"root_width\":%.2f,\"root_height\":%.2f,")
		TEXT("\"settings_commits\":%d,\"last_safe_action\":\"%s\"}"),
		Coordinator ? static_cast<int32>(Coordinator->GetScreen()) : -1,
		Coordinator ? static_cast<int32>(Coordinator->GetModal()) : -1,
		FocusedAction,
		bContinueEnabled ? TEXT("true") : TEXT("false"),
		InputRouter && InputRouter->HasVendorMenuContext() ? TEXT("true") : TEXT("false"),
		*FInternationalization::Get().GetCurrentCulture()->GetName(),
		Size.X, Size.Y,
		SettingsAdapter ? SettingsAdapter->GetSaveCommitCount() : 0,
		*LastSafeAction);
}
