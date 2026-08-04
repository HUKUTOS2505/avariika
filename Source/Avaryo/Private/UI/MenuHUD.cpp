#include "UI/MenuHUD.h"

#include "AvariikaLoc.h"
#include "Application/SlateApplicationBase.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ActorComponent.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/PanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/RichTextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Game/AvariikaOnlineSubsystem.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "HAL/FileManager.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "UObject/StructOnScope.h"
#include "Styling/CoreStyle.h"
#include "TimerManager.h"
#include "UI/MainMenu/AvMainMenuShell.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"

namespace
{
	const FLinearColor MenuBG(0.02f, 0.02f, 0.03f, 1.f);
	const FLinearColor Orange(1.f, 0.55f, 0.15f, 1.f);
	const FLinearColor PanelIdle(0.06f, 0.06f, 0.07f, 0.92f);
	const FLinearColor PanelHover(0.22f, 0.11f, 0.02f, 0.95f);
	const FLinearColor Grey(0.7f, 0.7f, 0.72f, 1.f);

	FString NormalizeMSPText(FString Value)
	{
		Value.ReplaceInline(TEXT("\r"), TEXT(" "));
		Value.ReplaceInline(TEXT("\n"), TEXT(" "));
		Value.TrimStartAndEndInline();
		while (Value.Contains(TEXT("  ")))
		{
			Value.ReplaceInline(TEXT("  "), TEXT(" "));
		}
		return Value.ToUpper();
	}

	const TMap<FString, FString>& MSPSettingKeys()
	{
		static const TMap<FString, FString> Keys = {
			{TEXT("WINDOW MODE"), TEXT("MainMenu.MSP.Video.WindowMode")},
			// Hidden vendor HDR is authored as the initial description owner.
			// Redirect its source copy to the first supported Screen setting so
			// a delayed vendor description update cannot reintroduce English.
			{TEXT("HDR"), TEXT("MainMenu.MSP.Video.WindowMode")},
			{TEXT("ENABLE HDR OUTPUT"), TEXT("MainMenu.MSP.Video.WindowMode")},
			{TEXT("ASPECT RATIO"), TEXT("MainMenu.MSP.Video.AspectRatio")},
			{TEXT("DISPLAY RESOLUTION"), TEXT("MainMenu.MSP.Video.Resolution")},
			{TEXT("BRIGHTNESS"), TEXT("MainMenu.MSP.Video.Brightness")},
			{TEXT("LOCK FPS"), TEXT("MainMenu.MSP.Video.FPSLock")},
			{TEXT("VSYNC"), TEXT("MainMenu.MSP.Video.VSync")},
			{TEXT("COLOR DEFICIENCY TYPE"), TEXT("MainMenu.MSP.Video.ColorDeficiencyType")},
			{TEXT("COLOR DEFICIENCY STRENGTH"), TEXT("MainMenu.MSP.Video.ColorDeficiencyStrength")},
			{TEXT("QUALITY PRESET"), TEXT("MainMenu.MSP.Graphics.QualityPreset")},
			{TEXT("GLOBAL ILLUMINATION"), TEXT("MainMenu.MSP.Graphics.GlobalIllumination")},
			{TEXT("SHADOWS"), TEXT("MainMenu.MSP.Graphics.Shadows")},
			{TEXT("ANTI ALIASING"), TEXT("MainMenu.MSP.Graphics.AntiAliasing")},
			{TEXT("ANTI ALIASING QUALITY"), TEXT("MainMenu.MSP.Graphics.AntiAliasing")},
			{TEXT("VIEW DISTANCE"), TEXT("MainMenu.MSP.Graphics.ViewDistance")},
			{TEXT("TEXTURES"), TEXT("MainMenu.MSP.Graphics.Textures")},
			{TEXT("EFFECTS"), TEXT("MainMenu.MSP.Graphics.Effects")},
			{TEXT("REFLECTIONS"), TEXT("MainMenu.MSP.Graphics.Reflections")},
			{TEXT("FOLIAGE"), TEXT("MainMenu.MSP.Graphics.Foliage")},
			{TEXT("SHADING"), TEXT("MainMenu.MSP.Graphics.Shading")},
			{TEXT("SHADING QUALITY"), TEXT("MainMenu.MSP.Graphics.Shading")},
			{TEXT("POST PROCESSING"), TEXT("MainMenu.MSP.Graphics.PostProcessing")},
			{TEXT("GLOBAL ILLUMINATION METHOD"), TEXT("MainMenu.MSP.Graphics.GlobalIlluminationMethod")},
			{TEXT("REFLECTION METHOD"), TEXT("MainMenu.MSP.Graphics.ReflectionMethod")},
			{TEXT("MOTION BLUR"), TEXT("MainMenu.MSP.Graphics.MotionBlur")},
			{TEXT("LENS FLARES"), TEXT("MainMenu.MSP.Graphics.LensFlares")},
			{TEXT("BLOOM"), TEXT("MainMenu.MSP.Graphics.Bloom")},
			{TEXT("AMBIENT OCCLUSION"), TEXT("MainMenu.MSP.Graphics.AmbientOcclusion")},
			{TEXT("RESOLUTION SCALE"), TEXT("MainMenu.MSP.Graphics.ResolutionScale")},
			{TEXT("UPSCALE QUALITY"), TEXT("MainMenu.MSP.Graphics.UpscaleQuality")},
			{TEXT("ANTI ALIASING METHOD"), TEXT("MainMenu.MSP.Graphics.AntiAliasingMethod")},
			{TEXT("TEMPORAL UPSAMPLING"), TEXT("MainMenu.MSP.Graphics.TemporalUpsampling")},
			{TEXT("TEMPORAL ANTI ALIASING UPSAMPLING"), TEXT("MainMenu.MSP.Graphics.TemporalUpsampling")},
			{TEXT("MASTER"), TEXT("MainMenu.MSP.Audio.Master")},
			{TEXT("MASTER VOLUME"), TEXT("MainMenu.MSP.Audio.Master")},
			{TEXT("SOUND EFFECTS"), TEXT("MainMenu.MSP.Audio.SFX")},
			{TEXT("SFX"), TEXT("MainMenu.MSP.Audio.SFX")},
			{TEXT("SFX VOLUME"), TEXT("MainMenu.MSP.Audio.SFX")},
			{TEXT("MUSIC"), TEXT("MainMenu.MSP.Audio.Music")},
			{TEXT("MUSIC VOLUME"), TEXT("MainMenu.MSP.Audio.Music")},
			{TEXT("VOICE"), TEXT("MainMenu.MSP.Audio.Voice")},
			{TEXT("VOICE VOLUME"), TEXT("MainMenu.MSP.Audio.Voice")},
			{TEXT("OUTPUT DEVICE"), TEXT("MainMenu.MSP.Audio.OutputDevice")},
		};
		return Keys;
	}

	const TMap<FString, FString>& MSPLiteralKeys()
	{
		static TMap<FString, FString> Keys = []
		{
			TMap<FString, FString> Result = MSPSettingKeys();
			const TMap<FString, FString> AdditionalKeys = {
				{TEXT("DISPLAY"), TEXT("MainMenu.MSP.Category.Display")},
				{TEXT("PERFORMANCE"), TEXT("MainMenu.MSP.Category.Performance")},
				{TEXT("COLORS"), TEXT("MainMenu.MSP.Category.Colors")},
				{TEXT("QUALITY"), TEXT("MainMenu.MSP.Category.Quality")},
				{TEXT("ADVANCED"), TEXT("MainMenu.MSP.Category.Advanced")},
				{TEXT("RESOLUTION"), TEXT("MainMenu.MSP.Category.Resolution")},
				{TEXT("GENERAL"), TEXT("MainMenu.Settings.Language")},
				{TEXT("GAMEPLAY"), TEXT("MainMenu.Settings.Language")},
				{TEXT("AUDIO"), TEXT("MainMenu.MSP.Tab.Audio")},
				{TEXT("AUDIO VOLUME"), TEXT("MainMenu.MSP.Tab.Audio")},
				{TEXT("VIDEO"), TEXT("MainMenu.MSP.Tab.Video")},
				{TEXT("CONTROLS"), TEXT("MainMenu.MSP.Tab.Controls")},
				{TEXT("GRAPHICS"), TEXT("MainMenu.MSP.Tab.Graphics")},
				{TEXT("LANGUAGE"), TEXT("MainMenu.Settings.Language")},
				{TEXT("APPLY"), TEXT("MainMenu.MSP.Common.Apply")},
				{TEXT("RESET SETTINGS"), TEXT("MainMenu.MSP.Common.ResetSettings")},
				{TEXT("BACK"), TEXT("MainMenu.Common.Back")},
				{TEXT("YES"), TEXT("MainMenu.Common.Yes")},
				{TEXT("NO"), TEXT("MainMenu.Common.No")},
				{TEXT("ON"), TEXT("MainMenu.Common.On")},
				{TEXT("OFF"), TEXT("MainMenu.Common.Off")},
				{TEXT("FULLSCREEN"), TEXT("MainMenu.Settings.WindowMode.Fullscreen")},
				{TEXT("WINDOWED"), TEXT("MainMenu.Settings.WindowMode.Windowed")},
				{TEXT("BORDERLESS"), TEXT("MainMenu.Settings.WindowMode.Borderless")},
				{TEXT("LOW"), TEXT("MainMenu.Settings.Quality.Low")},
				{TEXT("MEDIUM"), TEXT("MainMenu.Settings.Quality.Medium")},
				{TEXT("HIGH"), TEXT("MainMenu.Settings.Quality.High")},
				{TEXT("EPIC"), TEXT("MainMenu.Settings.Quality.Epic")},
				{TEXT("CINEMATIC"), TEXT("MainMenu.Settings.Quality.Cinematic")},
				{TEXT("ANY"), TEXT("MainMenu.MSP.Value.Default")},
				{TEXT("VERY LOW"), TEXT("MainMenu.MSP.Value.VeryLow")},
				{TEXT("FAR"), TEXT("MainMenu.MSP.Value.Far")},
				{TEXT("SCREEN SPACE"), TEXT("MainMenu.MSP.Value.ScreenSpace")},
				{TEXT("UNLOCKED"), TEXT("MainMenu.MSP.Value.Unlocked")},
				{TEXT("DEFAULT"), TEXT("MainMenu.MSP.Value.Default")},
				{TEXT("SYSTEM DEFAULT"), TEXT("MainMenu.MSP.Audio.SystemDefault")},
				{TEXT("ONE"), TEXT("MainMenu.MSP.Audio.SystemDefault")},
				{TEXT("EXIT GAME"), TEXT("MainMenu.Confirm.Exit.Title")},
				{TEXT("ARE YOU SURE YOU WANT TO QUIT THE GAME?"), TEXT("MainMenu.Confirm.Exit.Body")},
			};
			Result.Append(AdditionalKeys);
			return Result;
		}();
		return Keys;
	}

	FString NamedMSPTextKey(const FString& OwnerName, const FString& LeafName)
	{
		if (LeafName == TEXT("GameTitleText")) return TEXT("MainMenu.Title");
		if (OwnerName.Contains(TEXT("ContinueTitleScreenButton"))) return TEXT("MainMenu.Continue");
		if (OwnerName.Contains(TEXT("PlayTitleScreenButton"))) return TEXT("MainMenu.NewGame");
		if (OwnerName.Contains(TEXT("MultiplayerTitleScreenButton"))) return TEXT("MainMenu.Multiplayer");
		if (OwnerName.Contains(TEXT("OptionsTitleScreenButton"))) return TEXT("MainMenu.Settings");
		if (OwnerName.Contains(TEXT("ExtrasTitleScreenButton"))) return TEXT("MainMenu.Credits");
		if (OwnerName.Contains(TEXT("QuitTitleScreenButton"))) return TEXT("MainMenu.Exit");
		if (OwnerName.Contains(TEXT("GameplayLargeTabButton"))) return TEXT("MainMenu.MSP.Tab.Display");
		if (OwnerName.Contains(TEXT("AudioLargeTabButton"))) return TEXT("MainMenu.MSP.Tab.Graphics");
		if (OwnerName.Contains(TEXT("VideoLargeTabButton"))) return TEXT("MainMenu.MSP.Tab.Audio");
		if (OwnerName.Contains(TEXT("ControlsLargeTabButton"))) return TEXT("MainMenu.Settings.Language");
		if (OwnerName.Contains(TEXT("DisplaySmallTabButton"))) return TEXT("MainMenu.MSP.Tab.Display");
		if (OwnerName.Contains(TEXT("GraphicsSmallTabButton"))) return TEXT("MainMenu.MSP.Tab.Graphics");
		if (OwnerName.Contains(TEXT("GeneralSmallTabButton"))) return TEXT("MainMenu.Settings.Language");
		if (OwnerName.Contains(TEXT("VoiceChatSmallTabButton"))) return TEXT("MainMenu.MSP.Tab.VoiceChat");
		if (OwnerName.Contains(TEXT("DecisionButton_Yes"))) return TEXT("MainMenu.Common.Yes");
		if (OwnerName.Contains(TEXT("DecisionButton_No"))) return TEXT("MainMenu.Common.No");
		return FString();
	}

	bool IsInsideMSPLanguageMenu(const UObject* Object)
	{
		for (const UObject* Outer = Object; Outer; Outer = Outer->GetOuter())
		{
			if (Outer->GetClass()->GetPathName().Contains(TEXT("WBP_AvMSP_Language_C")))
			{
				return true;
			}
		}
		return false;
	}

	bool IsInsideMSPNamedWidget(const UObject* Object, const TCHAR* NameFragment)
	{
		for (const UObject* Outer = Object; Outer; Outer = Outer->GetOuter())
		{
			if (Outer->GetName().Contains(NameFragment))
			{
				return true;
			}
		}
		return false;
	}

	const TArray<FName>& MSPHomeButtonOrder()
	{
		static const TArray<FName> Order = {
			TEXT("ContinueTitleScreenButton"),
			TEXT("PlayTitleScreenButton"),
			TEXT("MultiplayerTitleScreenButton"),
			TEXT("OptionsTitleScreenButton"),
			TEXT("ExtrasTitleScreenButton"),
			TEXT("QuitTitleScreenButton"),
		};
		return Order;
	}

	double ReadMSPNumericProperty(const UObject* Object, const FName PropertyName, const double Fallback)
	{
		if (!Object)
		{
			return Fallback;
		}
		const FNumericProperty* Property = FindFProperty<FNumericProperty>(Object->GetClass(), PropertyName);
		if (!Property)
		{
			return Fallback;
		}
		const void* Value = Property->ContainerPtrToValuePtr<void>(Object);
		return Property->IsFloatingPoint() ?
			Property->GetFloatingPointPropertyValue(Value) :
			static_cast<double>(Property->GetSignedIntPropertyValue(Value));
	}

	FString ReadMSPTextProperty(const UObject* Object, const FName PropertyName)
	{
		const FTextProperty* Property = Object ?
			FindFProperty<FTextProperty>(Object->GetClass(), PropertyName) : nullptr;
		if (!Property)
		{
			return FString();
		}
		return Property->GetPropertyValue_InContainer(Object).ToString();
	}

	bool WriteMSPTextProperty(
		UObject* Object, const FName PropertyName, const FText& Value)
	{
		FTextProperty* Property = Object ?
			FindFProperty<FTextProperty>(Object->GetClass(), PropertyName) : nullptr;
		if (!Property)
		{
			return false;
		}
		Property->SetPropertyValue_InContainer(Object, Value);
		return true;
	}

	FName ReadMSPNameProperty(const UObject* Object, const FName PropertyName)
	{
		const FNameProperty* Property = Object ?
			FindFProperty<FNameProperty>(Object->GetClass(), PropertyName) : nullptr;
		return Property ? Property->GetPropertyValue_InContainer(Object) : NAME_None;
	}

	bool WriteMSPNumericProperty(UObject* Object, const FName PropertyName, const double Value)
	{
		if (!Object)
		{
			return false;
		}
		FNumericProperty* Property = FindFProperty<FNumericProperty>(Object->GetClass(), PropertyName);
		if (!Property)
		{
			return false;
		}
		void* Storage = Property->ContainerPtrToValuePtr<void>(Object);
		if (Property->IsFloatingPoint())
		{
			Property->SetFloatingPointPropertyValue(Storage, Value);
		}
		else
		{
			Property->SetIntPropertyValue(Storage, static_cast<int64>(FMath::RoundToDouble(Value)));
		}
		return true;
	}

	int64 FindMSPEnumValue(const UEnum* Enum, const FString& ValueName)
	{
		if (!Enum)
		{
			return INDEX_NONE;
		}
		for (int32 Index = 0; Index < Enum->NumEnums(); ++Index)
		{
			const FString Candidate = Enum->GetNameStringByIndex(Index);
			const FString DisplayName = Enum->GetDisplayNameTextByIndex(Index).ToString();
			if (Candidate.Equals(ValueName, ESearchCase::IgnoreCase) ||
				Candidate.EndsWith(TEXT("::") + ValueName, ESearchCase::IgnoreCase) ||
				DisplayName.Equals(ValueName, ESearchCase::IgnoreCase))
			{
				return Enum->GetValueByIndex(Index);
			}
		}
		return INDEX_NONE;
	}

	bool WriteMSPEnumProperty(
		UObject* Object, const FName PropertyName, const FString& ValueName)
	{
		FProperty* Property = Object ?
			FindFProperty<FProperty>(Object->GetClass(), PropertyName) : nullptr;
		if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			const int64 Value = FindMSPEnumValue(EnumProperty->GetEnum(), ValueName);
			if (Value != INDEX_NONE)
			{
				EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(
					EnumProperty->ContainerPtrToValuePtr<void>(Object), Value);
				return true;
			}
		}
		if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			const int64 Value = FindMSPEnumValue(ByteProperty->Enum, ValueName);
			if (Value != INDEX_NONE)
			{
				ByteProperty->SetPropertyValue_InContainer(
					Object, static_cast<uint8>(Value));
				return true;
			}
		}
		return false;
	}

	bool WriteMSPObjectProperty(
		UObject* Object, const FName PropertyName, UObject* Value)
	{
		FObjectPropertyBase* Property = Object ?
			FindFProperty<FObjectPropertyBase>(Object->GetClass(), PropertyName) : nullptr;
		if (!Property || (Value && !Value->IsA(Property->PropertyClass)))
		{
			return false;
		}
		Property->SetObjectPropertyValue_InContainer(Object, Value);
		return true;
	}

	UObject* ReadMSPObjectProperty(
		UObject* Object, const FName PropertyName)
	{
		FObjectPropertyBase* Property = Object ?
			FindFProperty<FObjectPropertyBase>(Object->GetClass(), PropertyName) : nullptr;
		return Property ?
			Property->GetObjectPropertyValue_InContainer(Object) : nullptr;
	}

	bool InvokeMSPEnumFunction(
		UObject* Object, const FName FunctionName, const FString& ValueName)
	{
		UFunction* Function = Object ? Object->FindFunction(FunctionName) : nullptr;
		if (!Function)
		{
			return false;
		}
		FStructOnScope Parameters(Function);
		bool bSetValue = false;
		for (TFieldIterator<FProperty> It(Function);
			It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
		{
			FProperty* Property = *It;
			if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}
			void* Storage =
				Property->ContainerPtrToValuePtr<void>(Parameters.GetStructMemory());
			if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
			{
				const int64 Value =
					FindMSPEnumValue(EnumProperty->GetEnum(), ValueName);
				if (Value != INDEX_NONE)
				{
					EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(Storage, Value);
					bSetValue = true;
				}
			}
			else if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
			{
				const int64 Value = FindMSPEnumValue(ByteProperty->Enum, ValueName);
				if (Value != INDEX_NONE)
				{
					ByteProperty->SetPropertyValue(Storage, static_cast<uint8>(Value));
					bSetValue = true;
				}
			}
		}
		if (bSetValue)
		{
			Object->ProcessEvent(Function, Parameters.GetStructMemory());
		}
		return bSetValue;
	}

	bool InvokeMSPNoParameterFunction(UObject* Object, const FName FunctionName)
	{
		UFunction* Function = Object ? Object->FindFunction(FunctionName) : nullptr;
		if (!Function)
		{
			return false;
		}
		for (TFieldIterator<FProperty> It(Function);
			It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
		{
			if (!It->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				return false;
			}
		}
		FStructOnScope Parameters(Function);
		Object->ProcessEvent(Function, Parameters.GetStructMemory());
		return true;
	}

	bool WriteMSPLinearColorProperty(
		UObject* Object, const FName PropertyName, const FLinearColor& Value)
	{
		if (!Object)
		{
			return false;
		}
		FStructProperty* Property = FindFProperty<FStructProperty>(Object->GetClass(), PropertyName);
		if (!Property || Property->Struct != TBaseStructure<FLinearColor>::Get())
		{
			return false;
		}
		*Property->ContainerPtrToValuePtr<FLinearColor>(Object) = Value;
		return true;
	}

	bool WriteMSPOptions(UUserWidget* Widget, const TArray<FText>& Options)
	{
		FArrayProperty* Property = Widget ?
			FindFProperty<FArrayProperty>(Widget->GetClass(), TEXT("OptionsArray")) : nullptr;
		FTextProperty* Inner = Property ? CastField<FTextProperty>(Property->Inner) : nullptr;
		if (!Property || !Inner)
		{
			return false;
		}
		FScriptArrayHelper Helper(Property, Property->ContainerPtrToValuePtr<void>(Widget));
		Helper.EmptyValues();
		for (const FText& Option : Options)
		{
			const int32 Index = Helper.AddValue();
			Inner->SetPropertyValue(Helper.GetRawPtr(Index), Option);
		}
		WriteMSPNumericProperty(Widget, TEXT("MaxSelectableIndex"), FMath::Max(0, Options.Num() - 1));
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(
			Widget->GetWidgetFromName(TEXT("ComboBoxString"))))
		{
			ComboBox->ClearOptions();
			for (const FText& Option : Options)
			{
				ComboBox->AddOption(Option.ToString());
			}
		}
		return true;
	}

	bool InvokeMSPNumericFunction(UUserWidget* Widget, const FName FunctionName, const double Value)
	{
		UFunction* Function = Widget ? Widget->FindFunction(FunctionName) : nullptr;
		if (!Function)
		{
			return false;
		}
		FStructOnScope Parameters(Function);
		bool bSetNumeric = false;
		for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
		{
			FProperty* Property = *It;
			if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}
			void* Storage = Property->ContainerPtrToValuePtr<void>(Parameters.GetStructMemory());
			if (FNumericProperty* Numeric = CastField<FNumericProperty>(Property))
			{
				if (Numeric->IsFloatingPoint())
				{
					Numeric->SetFloatingPointPropertyValue(Storage, Value);
				}
				else
				{
					Numeric->SetIntPropertyValue(Storage, static_cast<int64>(FMath::RoundToDouble(Value)));
				}
				bSetNumeric = true;
			}
			else if (FBoolProperty* Bool = CastField<FBoolProperty>(Property))
			{
				const FName Name = Property->GetFName();
				Bool->SetPropertyValue(
					Storage,
					Name == TEXT("bOptionChangeIgnore") ||
					Name == TEXT("bOptionChangedIgnore"));
			}
		}
		if (!bSetNumeric)
		{
			return false;
		}
		Widget->ProcessEvent(Function, Parameters.GetStructMemory());
		return true;
	}

	bool InvokeMSPOptionIndex(UUserWidget* Widget, const int32 Index)
	{
		WriteMSPNumericProperty(Widget, TEXT("ActiveOptionIndex"), Index);
		WriteMSPNumericProperty(Widget, TEXT("DefaultOptionIndex"), Index);
		if (UComboBoxString* ComboBox = Widget ? Cast<UComboBoxString>(
			Widget->GetWidgetFromName(TEXT("ComboBoxString"))) : nullptr)
		{
			ComboBox->SetSelectedIndex(Index);
		}
		// Do not call vendor SetActiveOption while synchronizing. Besides
		// broadcasting OnOptionChanged, that function also fakes a focus event,
		// which caused the first row to start highlighted and entered the
		// WBP_ComboBoxBase IsUserFocusOnSelf Accessed-None path.
		return Widget != nullptr;
	}

	bool InvokeMSPSliderValue(UUserWidget* Widget, const double Value)
	{
		if (!Widget)
		{
			return false;
		}
		const double Minimum = ReadMSPNumericProperty(Widget, TEXT("SliderMinValue"), 0.0);
		const double Maximum = ReadMSPNumericProperty(Widget, TEXT("SliderMaxValue"), 100.0);
		const float Clamped = static_cast<float>(
			FMath::Clamp(Value, Minimum, Maximum));
		WriteMSPNumericProperty(Widget, TEXT("CurrentValue"), Clamped);
		if (USlider* Slider = Cast<USlider>(
			Widget->GetWidgetFromName(TEXT("Slider"))))
		{
			Slider->SetValue(Clamped);
		}
		else if (USlider* SliderHandle = Cast<USlider>(
			Widget->GetWidgetFromName(TEXT("SliderHandle"))))
		{
			SliderHandle->SetValue(Clamped);
		}
		if (UUserWidget* ValueEditor = Cast<UUserWidget>(
			Widget->GetWidgetFromName(TEXT("SliderValueEditText"))))
		{
			if (UEditableText* Editable = Cast<UEditableText>(
				ValueEditor->GetWidgetFromName(TEXT("EditableText"))))
			{
				Editable->SetText(FText::AsNumber(FMath::RoundToInt(Clamped)));
			}
		}
		return true;
	}

	void RefreshMSPSliderLabel(UUserWidget* Widget, const double Value)
	{
		if (!Widget)
		{
			return;
		}
		if (UUserWidget* ValueEditor = Cast<UUserWidget>(
			Widget->GetWidgetFromName(TEXT("SliderValueEditText"))))
		{
			if (UEditableText* Editable = Cast<UEditableText>(
				ValueEditor->GetWidgetFromName(TEXT("EditableText"))))
			{
				Editable->SetText(FText::AsNumber(FMath::RoundToInt(Value)));
			}
		}
	}

	void RefreshMSPOptionLabel(UUserWidget* Widget, const FText& Value)
	{
		if (!Widget)
		{
			return;
		}
		if (UTextBlock* PickedOption = Cast<UTextBlock>(
			Widget->GetWidgetFromName(TEXT("PickedOptionText"))))
		{
			PickedOption->SetText(Value);
		}
	}

	int32 OverrideMSPStructFonts(
		const UStruct* Struct, void* Container, const FSlateFontInfo& Font, const int32 Depth = 0)
	{
		if (!Struct || !Container || Depth > 3)
		{
			return 0;
		}

		int32 OverrideCount = 0;
		for (TFieldIterator<FProperty> It(Struct); It; ++It)
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(*It);
			if (!StructProperty)
			{
				continue;
			}
			void* Value = StructProperty->ContainerPtrToValuePtr<void>(Container);
			if (StructProperty->Struct == TBaseStructure<FSlateFontInfo>::Get())
			{
				*static_cast<FSlateFontInfo*>(Value) = Font;
				++OverrideCount;
				continue;
			}
			OverrideCount += OverrideMSPStructFonts(
				StructProperty->Struct, Value, Font, Depth + 1);
		}
		return OverrideCount;
	}

	int32 OverrideMSPStyleFont(
		UObject* Object, const FName PropertyName, const FSlateFontInfo& Font)
	{
		FStructProperty* StyleProperty =
			Object ? FindFProperty<FStructProperty>(Object->GetClass(), PropertyName) : nullptr;
		if (!StyleProperty)
		{
			return 0;
		}
		void* Value = StyleProperty->ContainerPtrToValuePtr<void>(Object);
		if (StyleProperty->Struct == TBaseStructure<FSlateFontInfo>::Get())
		{
			*static_cast<FSlateFontInfo*>(Value) = Font;
			return 1;
		}
		return OverrideMSPStructFonts(
			StyleProperty->Struct,
			Value,
			Font);
	}

	FString DescribeMSPFont(const FSlateFontInfo& Font)
	{
		return FString::Printf(
			TEXT("FontObject=%s TypefaceFontName=%s FontMaterial=%s OutlineMaterial=%s Size=%.1f"),
			Font.FontObject ? *Font.FontObject->GetPathName() : TEXT("<none>"),
			*Font.TypefaceFontName.ToString(),
			Font.FontMaterial ? *Font.FontMaterial->GetPathName() : TEXT("<none>"),
			Font.OutlineSettings.OutlineMaterial ?
				*Font.OutlineSettings.OutlineMaterial->GetPathName() : TEXT("<none>"),
			Font.Size);
	}

	FSlateFontInfo MakeAuthenticMSPBodyFont(const int32 FontSize)
	{
		if (UObject* BodyFontObject = LoadObject<UObject>(
			nullptr,
			TEXT("/Game/Avariika/UI/Fonts/AvSettingsBodyComposite.AvSettingsBodyComposite")))
		{
			// Construct a clean asset-backed font info. Mixing a CoreStyle
			// shared composite pointer with a different FontObject works for a
			// direct TextBlock, but the transient pointer is intentionally lost
			// when MSP copies FFontStyle through Blueprint.
			return FSlateFontInfo(
				BodyFontObject, FontSize, TEXT("Regular"));
		}
		return FCoreStyle::GetDefaultFontStyle(
			TEXT("Regular"), FontSize);
	}

	bool FindMSPSettingRow(const FString& Name, EAvMainMenuSettingRow& OutRow)
	{
		static const TMap<FString, EAvMainMenuSettingRow> Rows = {
			{TEXT("WindowModeOptionsPicker"), EAvMainMenuSettingRow::WindowMode},
			{TEXT("AspectRatioOptionsPicker"), EAvMainMenuSettingRow::AspectRatio},
			{TEXT("DisplayResolutionsComboBox"), EAvMainMenuSettingRow::Resolution},
			{TEXT("BrightnessOptionsSlider"), EAvMainMenuSettingRow::Brightness},
			{TEXT("LockFPSOptionsPicker"), EAvMainMenuSettingRow::FrameRateLimit},
			{TEXT("VSyncOptionsPicker"), EAvMainMenuSettingRow::VSync},
			{TEXT("ColorDeficiencyTypeOptionsPicker"), EAvMainMenuSettingRow::ColorDeficiencyType},
			{TEXT("ColorDeficiencyStrengthOptionsSlider"), EAvMainMenuSettingRow::ColorDeficiencyStrength},
			{TEXT("PresetOptionsPicker"), EAvMainMenuSettingRow::OverallQuality},
			{TEXT("GlobalIlluminationOptionsPicker"), EAvMainMenuSettingRow::GlobalIlluminationQuality},
			{TEXT("ShadowsOptionsPicker"), EAvMainMenuSettingRow::ShadowQuality},
			{TEXT("AntiAliasingQualityOptionsPicker"), EAvMainMenuSettingRow::AntiAliasingQuality},
			{TEXT("ViewDistanceOptionsPicker"), EAvMainMenuSettingRow::ViewDistanceQuality},
			{TEXT("TexturesOptionsPicker"), EAvMainMenuSettingRow::TextureQuality},
			{TEXT("EffectsOptionsPicker"), EAvMainMenuSettingRow::EffectsQuality},
			{TEXT("ReflectionsOptionsPicker"), EAvMainMenuSettingRow::ReflectionQuality},
			{TEXT("FoliageOptionsPicker"), EAvMainMenuSettingRow::FoliageQuality},
			{TEXT("ShadingOptionsPicker"), EAvMainMenuSettingRow::ShadingQuality},
			{TEXT("PostProcessingOptionsPicker"), EAvMainMenuSettingRow::PostProcessQuality},
			{TEXT("GlobalIlluminationMethodOptionsPicker"), EAvMainMenuSettingRow::GlobalIlluminationMethod},
			{TEXT("ReflectionMethodOptionsPicker"), EAvMainMenuSettingRow::ReflectionMethod},
			{TEXT("MotionBlurOptionsPicker"), EAvMainMenuSettingRow::MotionBlur},
			{TEXT("LensFlaresOptionsPicker"), EAvMainMenuSettingRow::LensFlares},
			{TEXT("BloomOptionsPicker"), EAvMainMenuSettingRow::Bloom},
			{TEXT("AmbientOcclusionOptionsPicker"), EAvMainMenuSettingRow::AmbientOcclusion},
			{TEXT("ResolutionScaleOptionsSlider"), EAvMainMenuSettingRow::ResolutionScale},
			{TEXT("AntiAliasingMethodOptionPicker"), EAvMainMenuSettingRow::AntiAliasingMethod},
			{TEXT("MasterSlider"), EAvMainMenuSettingRow::MasterVolume},
			{TEXT("SFXSlider"), EAvMainMenuSettingRow::SFXVolume},
			{TEXT("MusicSlider"), EAvMainMenuSettingRow::MusicVolume},
			{TEXT("VoiceSlider"), EAvMainMenuSettingRow::VoiceVolume},
		};
		for (const TPair<FString, EAvMainMenuSettingRow>& Pair : Rows)
		{
			if (Name.Contains(Pair.Key))
			{
				OutRow = Pair.Value;
				return true;
			}
		}
		return false;
	}

	bool IsMSPSliderRow(const EAvMainMenuSettingRow Row)
	{
		return Row == EAvMainMenuSettingRow::Brightness ||
			Row == EAvMainMenuSettingRow::ColorDeficiencyStrength ||
			Row == EAvMainMenuSettingRow::ResolutionScale ||
			Row == EAvMainMenuSettingRow::MasterVolume ||
			Row == EAvMainMenuSettingRow::SFXVolume ||
			Row == EAvMainMenuSettingRow::MusicVolume ||
			Row == EAvMainMenuSettingRow::VoiceVolume;
	}

	const TCHAR* MSPDescriptionKeyForRow(const EAvMainMenuSettingRow Row)
	{
		switch (Row)
		{
		case EAvMainMenuSettingRow::WindowMode:
			return TEXT("MainMenu.MSP.Video.WindowMode");
		case EAvMainMenuSettingRow::AspectRatio:
			return TEXT("MainMenu.MSP.Video.AspectRatio");
		case EAvMainMenuSettingRow::Resolution:
			return TEXT("MainMenu.MSP.Video.Resolution");
		case EAvMainMenuSettingRow::Brightness:
			return TEXT("MainMenu.MSP.Video.Brightness");
		case EAvMainMenuSettingRow::FrameRateLimit:
			return TEXT("MainMenu.MSP.Video.FPSLock");
		case EAvMainMenuSettingRow::VSync:
			return TEXT("MainMenu.MSP.Video.VSync");
		case EAvMainMenuSettingRow::ColorDeficiencyType:
			return TEXT("MainMenu.MSP.Video.ColorDeficiencyType");
		case EAvMainMenuSettingRow::ColorDeficiencyStrength:
			return TEXT("MainMenu.MSP.Video.ColorDeficiencyStrength");
		case EAvMainMenuSettingRow::OverallQuality:
			return TEXT("MainMenu.MSP.Graphics.QualityPreset");
		case EAvMainMenuSettingRow::GlobalIlluminationQuality:
			return TEXT("MainMenu.MSP.Graphics.GlobalIllumination");
		case EAvMainMenuSettingRow::ShadowQuality:
			return TEXT("MainMenu.MSP.Graphics.Shadows");
		case EAvMainMenuSettingRow::AntiAliasingQuality:
			return TEXT("MainMenu.MSP.Graphics.AntiAliasing");
		case EAvMainMenuSettingRow::ViewDistanceQuality:
			return TEXT("MainMenu.MSP.Graphics.ViewDistance");
		case EAvMainMenuSettingRow::TextureQuality:
			return TEXT("MainMenu.MSP.Graphics.Textures");
		case EAvMainMenuSettingRow::EffectsQuality:
			return TEXT("MainMenu.MSP.Graphics.Effects");
		case EAvMainMenuSettingRow::ReflectionQuality:
			return TEXT("MainMenu.MSP.Graphics.Reflections");
		case EAvMainMenuSettingRow::FoliageQuality:
			return TEXT("MainMenu.MSP.Graphics.Foliage");
		case EAvMainMenuSettingRow::ShadingQuality:
			return TEXT("MainMenu.MSP.Graphics.Shading");
		case EAvMainMenuSettingRow::PostProcessQuality:
			return TEXT("MainMenu.MSP.Graphics.PostProcessing");
		case EAvMainMenuSettingRow::GlobalIlluminationMethod:
			return TEXT("MainMenu.MSP.Graphics.GlobalIlluminationMethod");
		case EAvMainMenuSettingRow::ReflectionMethod:
			return TEXT("MainMenu.MSP.Graphics.ReflectionMethod");
		case EAvMainMenuSettingRow::MotionBlur:
			return TEXT("MainMenu.MSP.Graphics.MotionBlur");
		case EAvMainMenuSettingRow::LensFlares:
			return TEXT("MainMenu.MSP.Graphics.LensFlares");
		case EAvMainMenuSettingRow::Bloom:
			return TEXT("MainMenu.MSP.Graphics.Bloom");
		case EAvMainMenuSettingRow::AmbientOcclusion:
			return TEXT("MainMenu.MSP.Graphics.AmbientOcclusion");
		case EAvMainMenuSettingRow::ResolutionScale:
			return TEXT("MainMenu.MSP.Graphics.ResolutionScale");
		case EAvMainMenuSettingRow::AntiAliasingMethod:
			return TEXT("MainMenu.MSP.Graphics.AntiAliasingMethod");
		case EAvMainMenuSettingRow::MasterVolume:
			return TEXT("MainMenu.MSP.Audio.Master");
		case EAvMainMenuSettingRow::SFXVolume:
			return TEXT("MainMenu.MSP.Audio.SFX");
		case EAvMainMenuSettingRow::MusicVolume:
			return TEXT("MainMenu.MSP.Audio.Music");
		case EAvMainMenuSettingRow::VoiceVolume:
			return TEXT("MainMenu.MSP.Audio.Voice");
		case EAvMainMenuSettingRow::Language:
			return TEXT("MainMenu.Settings.Language");
		default:
			return nullptr;
		}
	}
}

void UAvMSPSettingEventProxy::Initialize(
	AMenuHUD* InOwner, UUserWidget* InSettingWidget)
{
	Owner = InOwner;
	SettingWidget = InSettingWidget;
}

void UAvMSPSettingEventProxy::HandlePreviousClicked()
{
	if (Owner && SettingWidget)
	{
		Owner->AdjustMSPSettingOption(SettingWidget, -1);
	}
}

void UAvMSPSettingEventProxy::HandleNextClicked()
{
	if (Owner && SettingWidget)
	{
		Owner->AdjustMSPSettingOption(SettingWidget, 1);
	}
}

void UAvMSPSettingEventProxy::HandleSliderChanged(const float Value)
{
	if (Owner && SettingWidget)
	{
		Owner->CommitMSPSettingScalar(SettingWidget, Value);
	}
}

void UAvMSPSettingEventProxy::HandleComboSelectionChanged(
	FString SelectedItem, const ESelectInfo::Type SelectionType)
{
	if (Owner && SettingWidget && SelectionType != ESelectInfo::Direct)
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(
			SettingWidget->GetWidgetFromName(TEXT("ComboBoxString"))))
		{
			Owner->CommitMSPSettingIndex(
				SettingWidget, ComboBox->GetSelectedIndex());
		}
	}
}

void UAvMSPSettingEventProxy::HandleRowHovered()
{
	if (Owner && SettingWidget)
	{
		Owner->HandleMSPSettingRowHovered(SettingWidget);
	}
}

void UAvMSPSettingEventProxy::HandleRowUnhovered()
{
	if (Owner && SettingWidget)
	{
		Owner->HandleMSPSettingRowUnhovered(SettingWidget);
	}
}

void UAvMSPSettingEventProxy::HandleRowPressed()
{
	if (Owner && SettingWidget)
	{
		Owner->HandleMSPSettingRowPressed(SettingWidget);
	}
}

void UAvMSPSettingEventProxy::HandleRowReleased()
{
	if (Owner && SettingWidget)
	{
		Owner->HandleMSPSettingRowReleased(SettingWidget);
	}
}

void UAvMSPSettingEventProxy::HandleRowFocused()
{
	if (Owner && SettingWidget)
	{
		Owner->HandleMSPSettingRowFocused(SettingWidget);
	}
}

void UAvMSPSettingEventProxy::HandleRowFocusLost()
{
	if (Owner && SettingWidget)
	{
		Owner->HandleMSPSettingRowFocusLost(SettingWidget);
	}
}

bool AMenuHUD::IsAuthoredShellWorld() const
{
	const UWorld* World = GetWorld();
	return World && World->GetOutermost()->GetName().Contains(TEXT("/Avariika/Maps/MainMenu/"));
}

void AMenuHUD::BeginPlay()
{
	Super::BeginPlay();
	bMSPSettingsInteractionDiagnostics =
		FParse::Param(FCommandLine::Get(), TEXT("AvSettingsInteractionDiagnostics"));
	bAuthenticMSPMap = IsAuthoredShellWorld();
	if (!bAuthenticMSPMap)
	{
		return;
	}

	if (!MountAuthenticMSP())
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenu] Authentic MSP mount failed. The deprecated provisional shell remains inactive."));
	}
}

void AMenuHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AuthenticMSPBridgeTimer);
		GetWorld()->GetTimerManager().ClearTimer(MSPDisplayConfirmationTimer);
	}
	RemoveLegacyMSPJsonMirror();
	AuthenticMSPRoot.Reset();
	MSPMainSwitcher = nullptr;
	MSPSettingsPageSwitcher = nullptr;
	MSPHomeMenuWidget = nullptr;
	StableMSPSettingsRoot = nullptr;
	StableMSPScreenPage = nullptr;
	StableMSPGraphicsPage = nullptr;
	StableMSPAudioPage = nullptr;
	StableMSPLanguagePage = nullptr;
	if (ActiveMSPOverlay)
	{
		ActiveMSPOverlay->RemoveFromParent();
		ActiveMSPOverlay = nullptr;
	}
	ActiveMSPOverlayKind = EMSPOverlay::None;
	bActiveMSPOverlayOwnedByRouter = false;
	MSPOverlayYesButton.Reset();
	MSPOverlayNoButton.Reset();
	BoundMSPButtons.Reset();
	MSPHomeButtons.Reset();
	MSPHoveredHomeButton.Reset();
	MSPPressedHomeButton.Reset();
	MSPHomeFocusIndex = INDEX_NONE;
	PendingMSPHomeFocusOwner = NAME_None;
	InitializedMSPBackgroundWidgets.Reset();
	StyledMSPWidgets.Reset();
	MSPApplyButtons.Reset();
	MSPApplyWidgets.Reset();
	MSPSettingsMenuWidgets.Reset();
	MSPLanguageMenuWidgets.Reset();
	BoundMSPLanguageTabWidgets.Reset();
	BoundStableMSPSettingsTabWidgets.Reset();
	MSPLanguageTabButtons.Reset();
	ResponsiveMSPWidgets.Reset();
	InitializedMSPSettingWidgets.Reset();
	InitializedMSPBridgeWidgets.Reset();
	BoundMSPSettingInputButtons.Reset();
	BoundMSPSettingInputSliders.Reset();
	BoundMSPSettingInputComboBoxes.Reset();
	MSPSettingEventProxies.Reset();
	BoundMSPMouseModalityButtons.Reset();
	DiagnosticMSPButtons.Reset();
	DiagnosticMSPSliders.Reset();
	MSPSettingRows.Reset();
	MSPSettingVisualStates.Reset();
	PressedMSPSettingRows.Reset();
	BoundMSPSettingVisualRows.Reset();
	LoggedMSPOptionIndicatorRows.Reset();
	LoggedMSPSettingHoverTargets.Reset();
	MSPLastCapturedIndices.Reset();
	MSPLastCapturedScalars.Reset();
	MSPApplyButton.Reset();
	MSPResetButton.Reset();
	MSPBackButton.Reset();
	MSPLanguageButton.Reset();
	MSPLanguageReturnFocusButton.Reset();
	MSPApplyWidget.Reset();
	MSPFooterBarWidget.Reset();
	MSPSettingsMenuWidget.Reset();
	MSPSettingsReturnFocusButton.Reset();
	MSPLanguagePreviousSubMenuClassPath.Empty();
	ActiveMSPVisualSubMenuClassPath.Empty();
	ActiveMSPDescriptionKeys.Reset();
	LastMSPSettingsInvariantSignature.Empty();
	MSPFontSetCounts.Reset();
	bMSPSettingsOpen = false;
	bMSPLanguageOpen = false;
	bSynchronizingSettingsUI = false;
	bMSPSliderDragging = false;
	bMSPMouseInputActive = false;
	bLoggedMSPHomeInitialNoSelection = false;
	bFocusMSPSettingsAfterLanguageBack = false;
	bReturnHomeAfterMSPApply = false;
	bStableMSPSettingsWarmupStarted = false;
	bStableMSPSettingsReady = false;
	bStableMSPInitialRowFontPassComplete = false;
	bOpenStableMSPSettingsWhenReady = false;
	bLoggedStableMSPSettingsDump = false;
	if (AuthenticMSPActor && !AuthenticMSPActor->IsActorBeingDestroyed())
	{
		AuthenticMSPActor->Destroy();
	}
	AuthenticMSPActor = nullptr;
	if (AuthenticMSPControllerComponent && bOwnsAuthenticMSPControllerComponent)
	{
		AuthenticMSPControllerComponent->DestroyComponent();
	}
	AuthenticMSPControllerComponent = nullptr;
	bOwnsAuthenticMSPControllerComponent = false;
	SettingsAdapter = nullptr;
	SavePresenceAdapter = nullptr;
	if (MainMenuRoot)
	{
		MainMenuRoot->RemoveFromParent();
		MainMenuRoot = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

UUserWidget* AMenuHUD::GetActiveMSPSubMenu() const
{
	if (!AuthenticMSPRoot.IsValid())
	{
		return nullptr;
	}
	if (const UBorder* SubMenu = Cast<UBorder>(
		AuthenticMSPRoot->GetWidgetFromName(TEXT("SubMenu"))))
	{
		if (const UWidgetSwitcher* Switcher =
			Cast<UWidgetSwitcher>(SubMenu->GetContent()))
		{
			return Cast<UUserWidget>(Switcher->GetActiveWidget());
		}
		return Cast<UUserWidget>(SubMenu->GetContent());
	}
	return nullptr;
}

bool AMenuHUD::IsInActiveMSPSubMenu(const UWidget* Widget) const
{
	const UUserWidget* ActiveSubMenu = GetActiveMSPSubMenu();
	if (!Widget || !ActiveSubMenu)
	{
		return false;
	}
	if (Widget == ActiveSubMenu)
	{
		return true;
	}
	for (const UObject* Outer = Widget->GetOuter(); Outer; Outer = Outer->GetOuter())
	{
		if (Outer == ActiveSubMenu)
		{
			return true;
		}
	}
	return false;
}

bool AMenuHUD::MountAuthenticMSP()
{
	UWorld* World = GetWorld();
	APlayerController* PC = GetOwningPlayerController();
	if (!World || !PC)
	{
		return false;
	}

	RemoveLegacyMSPJsonMirror();
	SavePresenceAdapter = NewObject<UAvMainMenuSavePresenceAdapter>(this);
	SettingsAdapter = NewObject<UAvMainMenuSettingsAdapter>(this);
	SettingsAdapter->Initialize(World);
	SettingsAdapter->ApplyPersistedRuntimeState();
	const bool bContinueEnabled = SavePresenceAdapter->Refresh();

	UClass* ControllerComponentClass = LoadClass<UActorComponent>(nullptr,
		TEXT("/Game/Avariika/Blueprints/MainMenu/BP_AvMSP_MenuControllerComponent.BP_AvMSP_MenuControllerComponent_C"));
	UClass* ActorClass = LoadClass<AActor>(nullptr,
		TEXT("/Game/Avariika/Blueprints/MainMenu/BP_AvMSP_MenuSystemActor.BP_AvMSP_MenuSystemActor_C"));
	if (!ControllerComponentClass || !ActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenu] Authentic MSP classes are missing. Actor=%s Controller=%s"),
			ActorClass ? TEXT("ok") : TEXT("missing"),
			ControllerComponentClass ? TEXT("ok") : TEXT("missing"));
		return false;
	}

	TArray<UActorComponent*> ExistingComponents;
	PC->GetComponents(ExistingComponents);
	for (UActorComponent* ExistingComponent : ExistingComponents)
	{
		if (IsValid(ExistingComponent) && ExistingComponent->IsA(ControllerComponentClass))
		{
			AuthenticMSPControllerComponent = ExistingComponent;
			break;
		}
	}
	if (!AuthenticMSPControllerComponent)
	{
		AuthenticMSPControllerComponent = NewObject<UActorComponent>(
			PC, ControllerComponentClass, TEXT("AvMSP_MenuControllerComponent"));
		bOwnsAuthenticMSPControllerComponent = AuthenticMSPControllerComponent != nullptr;
	}
	if (!AuthenticMSPControllerComponent)
	{
		return false;
	}
	if (bOwnsAuthenticMSPControllerComponent)
	{
		PC->AddInstanceComponent(AuthenticMSPControllerComponent);
	}
	// The authentic widgets query this component while their Construct events run.
	// Register and activate it before the menu actor creates the root widget.
	if (!AuthenticMSPControllerComponent->IsRegistered())
	{
		AuthenticMSPControllerComponent->RegisterComponent();
	}
	AuthenticMSPControllerComponent->Activate(true);

	const FTransform SpawnTransform = FTransform::Identity;
	AuthenticMSPActor = World->SpawnActorDeferred<AActor>(ActorClass, SpawnTransform, PC, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!AuthenticMSPActor)
	{
		if (bOwnsAuthenticMSPControllerComponent)
		{
			AuthenticMSPControllerComponent->DestroyComponent();
		}
		AuthenticMSPControllerComponent = nullptr;
		bOwnsAuthenticMSPControllerComponent = false;
		return false;
	}
	if (FBoolProperty* MainMenuProperty = FindFProperty<FBoolProperty>(
		AuthenticMSPActor->GetClass(), TEXT("bLevelIsMainMenu")))
	{
		MainMenuProperty->SetPropertyValue_InContainer(AuthenticMSPActor, true);
	}
	UGameplayStatics::FinishSpawningActor(AuthenticMSPActor, SpawnTransform);

	World->GetTimerManager().SetTimer(AuthenticMSPBridgeTimer, this,
		&AMenuHUD::RefreshAuthenticMSPBridge, 0.10f, true, 0.05f);
	UE_LOG(LogTemp, Log,
		TEXT("[AvMainMenu] Authentic MSP actor mounted. Continue=%s ProvisionalShell=false"),
		bContinueEnabled ? TEXT("true") : TEXT("false"));
	return true;
}

UButton* AMenuHUD::FindFirstButton(UUserWidget* Widget) const
{
	if (!Widget || !Widget->WidgetTree)
	{
		return nullptr;
	}
	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		if (UButton* Button = Cast<UButton>(Child))
		{
			return Button;
		}
	}
	return nullptr;
}

USlider* AMenuHUD::FindFirstSlider(UUserWidget* Widget) const
{
	if (!Widget || !Widget->WidgetTree)
	{
		return nullptr;
	}
	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		if (USlider* Slider = Cast<USlider>(Child))
		{
			return Slider;
		}
	}
	return nullptr;
}

void AMenuHUD::LocalizeAuthenticMSPWidget(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}
	const bool bLanguageMenu = IsInsideMSPLanguageMenu(Widget);
	if (bLanguageMenu)
	{
		ActiveMSPDescriptionKeys.FindOrAdd(Widget) = TEXT("MainMenu.Settings.Language");
	}
	else
	{
		const FString SourceOptionName = NormalizeMSPText(
			ReadMSPTextProperty(Widget, TEXT("OptionName")));
		if (const FString* SettingKey = MSPSettingKeys().Find(SourceOptionName))
		{
			ActiveMSPDescriptionKeys.FindOrAdd(Widget) = *SettingKey;
		}
	}

	// MSP restores these source properties when its authored hover/focus state
	// changes. Localize the state data once, rather than continuously repairing
	// the rendered text after each interaction.
	if (const FString* ActiveKey = ActiveMSPDescriptionKeys.Find(Widget))
	{
		WriteMSPTextProperty(Widget, TEXT("OptionName"), FAvLoc::Text(*ActiveKey));
		WriteMSPTextProperty(
			Widget, TEXT("OptionDescription"), FAvLoc::Text(*ActiveKey + TEXT(".Description")));
	}

	if (UTextBlock* SelectedName = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("SelectedOptionName"))))
	{
		if (!bLanguageMenu)
		{
			const FString Normalized = NormalizeMSPText(SelectedName->GetText().ToString());
			if (const FString* SettingKey = MSPSettingKeys().Find(Normalized))
			{
				const bool bHiddenOutputDevice =
					*SettingKey == TEXT("MainMenu.MSP.Audio.OutputDevice");
				ActiveMSPDescriptionKeys.FindOrAdd(Widget) = bHiddenOutputDevice ?
					TEXT("MainMenu.MSP.Audio.Master") : *SettingKey;
			}
		}
		if (const FString* ActiveKey = ActiveMSPDescriptionKeys.Find(Widget))
		{
			SelectedName->SetText(FAvLoc::Text(*ActiveKey));
		}
	}

	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		const UUserWidget* OwnerWidget = Child->GetTypedOuter<UUserWidget>();
		const FString OwnerName = OwnerWidget ? OwnerWidget->GetName() : Widget->GetName();
		FString Key = NamedMSPTextKey(OwnerName, Child->GetName());
		if (OwnerName.Contains(TEXT("DecisionButton_Yes")))
		{
			if (ActiveMSPOverlayKind == EMSPOverlay::DisplayConfirm)
			{
				Key = TEXT("MainMenu.Confirm.Display.Keep");
			}
			else if (ActiveMSPOverlayKind == EMSPOverlay::ResetConfirm)
			{
				Key = TEXT("MainMenu.Settings.Revert");
			}
			else if (ActiveMSPOverlayKind == EMSPOverlay::UnsavedConfirm)
			{
				Key = TEXT("MainMenu.MSP.Common.Apply");
			}
		}
		else if (OwnerName.Contains(TEXT("DecisionButton_No")))
		{
			if (ActiveMSPOverlayKind == EMSPOverlay::DisplayConfirm)
			{
				Key = TEXT("MainMenu.Confirm.Display.Revert");
			}
			else if (ActiveMSPOverlayKind == EMSPOverlay::ResetConfirm)
			{
				Key = TEXT("MainMenu.Common.Cancel");
			}
			else if (ActiveMSPOverlayKind == EMSPOverlay::UnsavedConfirm)
			{
				Key = TEXT("MainMenu.Settings.Discard");
			}
		}
		if (bLanguageMenu &&
			((OwnerName.Contains(TEXT("OutputDeviceOptionsPicker")) &&
				Child->GetName() == TEXT("OptionNameText")) ||
				Child->GetName() == TEXT("VolumeHeaderText_1")))
		{
			Key = TEXT("MainMenu.Settings.Language");
		}
		if (UTextBlock* Text = Cast<UTextBlock>(Child))
		{
			if (Key.IsEmpty())
			{
				if (const FString* LiteralKey = MSPLiteralKeys().Find(NormalizeMSPText(Text->GetText().ToString())))
				{
					Key = *LiteralKey;
				}
			}
			if (IsInsideMSPNamedWidget(Text, TEXT("ControlsLargeTabButton")) ||
				IsInsideMSPNamedWidget(Text, TEXT("GeneralSmallTabButton")))
			{
				Key = TEXT("MainMenu.Settings.Language");
			}
			if (!Key.IsEmpty())
			{
				Text->SetText(FAvLoc::Text(Key));
			}
		}
		else if (URichTextBlock* RichText = Cast<URichTextBlock>(Child))
		{
			if (RichText->GetName().StartsWith(TEXT("SelectedOptionDescription")))
			{
				if (const FString* ActiveKey = ActiveMSPDescriptionKeys.Find(Widget))
				{
					RichText->SetText(FAvLoc::Text(*ActiveKey + TEXT(".Description")));
				}
			}
			else if (const FString* LiteralKey = MSPLiteralKeys().Find(
				NormalizeMSPText(RichText->GetText().ToString())))
			{
				RichText->SetText(FAvLoc::Text(*LiteralKey));
			}
		}
	}
}

void AMenuHUD::ApplyAuthenticMSPBackground(UUserWidget* Widget)
{
	if (!Widget || InitializedMSPBackgroundWidgets.Contains(Widget))
	{
		return;
	}

	UBorder* Background = Cast<UBorder>(Widget->GetWidgetFromName(TEXT("Background")));
	if (!Background)
	{
		return;
	}

	UMaterialInterface* BackgroundMaterial = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/MenuSystemPro/Blueprints/UI/Materials/MI_MenuBackground.MI_MenuBackground"));
	if (!BackgroundMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenu] Authentic MSP background material is missing."));
		return;
	}

	Background->SetBrushFromMaterial(BackgroundMaterial);
	Background->SetBrushColor(FLinearColor::White);
	InitializedMSPBackgroundWidgets.Add(Widget);
}

void AMenuHUD::ApplyAuthenticMSPFontPolicy(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}
	const bool bNeedsCyrillicFallback = FInternationalization::Get().GetCurrentCulture()
		->GetTwoLetterISOLanguageName() == TEXT("ru");
	const uint8* SettingRow = MSPSettingRows.Find(Widget);
	const FString WidgetName = Widget->GetName();
	const bool bSettingsTab =
		WidgetName.Contains(TEXT("GameplayLargeTabButton")) ||
		WidgetName.Contains(TEXT("AudioLargeTabButton")) ||
		WidgetName.Contains(TEXT("VideoLargeTabButton")) ||
		WidgetName.Contains(TEXT("ControlsLargeTabButton")) ||
		WidgetName.Contains(TEXT("GeneralSmallTabButton"));
	const bool bLanguageSetting = SettingRow &&
		static_cast<EAvMainMenuSettingRow>(*SettingRow) ==
			EAvMainMenuSettingRow::Language;

	// Stable role classification: a registered setting widget owns exactly one
	// OptionNameText (RowLabel) and, where applicable, PickedOptionText
	// (ValueText). The decision never depends on localized text content.
	int32 LabelStyleOverrideCount = 0;
	int32 ValueStyleOverrideCount = 0;
	if (SettingRow)
	{
		int32 LabelFontSize = 22;
		if (const UTextBlock* RowLabel = Cast<UTextBlock>(
			Widget->GetWidgetFromName(TEXT("OptionNameText"))))
		{
			LabelFontSize = RowLabel->GetFont().Size;
		}
		const FSlateFontInfo BodyFont =
			MakeAuthenticMSPBodyFont(LabelFontSize);
		// TextFontStyle is itself an FSlateFontInfo in the MSP setting rows.
		// The old helper skipped that root value and only searched nested
		// structs, allowing delayed authored setup to restore the icon face.
		LabelStyleOverrideCount =
			OverrideMSPStyleFont(Widget, TEXT("TextFontStyle"), BodyFont);

		if (bNeedsCyrillicFallback || bLanguageSetting)
		{
			int32 ValueFontSize = LabelFontSize;
			if (const UTextBlock* ValueText = Cast<UTextBlock>(
				Widget->GetWidgetFromName(TEXT("PickedOptionText"))))
			{
				ValueFontSize = ValueText->GetFont().Size;
			}
			const FSlateFontInfo ValueFont =
				MakeAuthenticMSPBodyFont(ValueFontSize);
			ValueStyleOverrideCount += OverrideMSPStyleFont(
				Widget, TEXT("TextValueFontStyle"), ValueFont);
			ValueStyleOverrideCount += OverrideMSPStyleFont(
				Widget, TEXT("ValueFontStyle"), ValueFont);
		}
	}
	if (bSettingsTab)
	{
		int32 FontSize = 22;
		if (const UTextBlock* ButtonText = Cast<UTextBlock>(
			Widget->GetWidgetFromName(TEXT("ButtonText"))))
		{
			FontSize = ButtonText->GetFont().Size;
		}
		const FSlateFontInfo BodyFont =
			MakeAuthenticMSPBodyFont(FontSize);
		OverrideMSPStyleFont(Widget, TEXT("TextFontStyle"), BodyFont);
	}

	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		if (UTextBlock* Text = Cast<UTextBlock>(Child))
		{
			const FName TextName = Text->GetFName();
			const bool bRowLabel =
				SettingRow && TextName == TEXT("OptionNameText");
			const bool bValueText =
				SettingRow && TextName == TEXT("PickedOptionText");
			const bool bSectionHeading =
				!SettingRow && TextName == TEXT("HeadlineTitleText");
			const bool bTabText = TextName == TEXT("ButtonText");
			if (!bRowLabel && !bValueText && !bSectionHeading && !bTabText)
			{
				// IconGlyph and all unrelated text roles retain their authored
				// font, material and glyph mapping.
				continue;
			}
			FSlateFontInfo& Original = OriginalMSPTextFonts.FindOrAdd(Text, Text->GetFont());
			const FSlateFontInfo Before = Text->GetFont();
			const TCHAR* TextRole = bRowLabel ? TEXT("RowLabel") :
				bValueText ? TEXT("ValueText") :
				bSectionHeading ? TEXT("SectionHeading") :
				TEXT("TabText");
			const TCHAR* Source = TEXT("ApplyAuthenticMSPFontPolicy");
			if (bRowLabel)
			{
				// RowLabel always uses the body face. This makes both EN and RU
				// deterministic and prevents a later culture pass from restoring
				// the authored icon/font-role mismatch.
				Text->SetFont(MakeAuthenticMSPBodyFont(Original.Size));
			}
			else if ((bValueText && (bNeedsCyrillicFallback || bLanguageSetting)) ||
				(bSectionHeading && bNeedsCyrillicFallback) ||
				(bTabText && bSettingsTab))
			{
				Text->SetFont(MakeAuthenticMSPBodyFont(Original.Size));
			}
			else
			{
				Text->SetFont(Original);
			}
			const int32 SetFontCount = ++MSPFontSetCounts.FindOrAdd(Text);
			if (bMSPSettingsInteractionDiagnostics &&
				(bRowLabel || bValueText || bSectionHeading ||
					(bTabText && bSettingsTab)))
			{
				UE_LOG(LogTemp, Log,
					TEXT("[AvSettingsFontDiag] object=%s class=%s outer=%s role=%s text=\"%s\" before={%s} after={%s} SetFontCalls=%d source=%s labelStyleOverrides=%d valueStyleOverrides=%d"),
					*Text->GetName(),
					*Text->GetClass()->GetPathName(),
					Text->GetOuter() ? *Text->GetOuter()->GetPathName() : TEXT("<none>"),
					TextRole,
					*Text->GetText().ToString(),
					*DescribeMSPFont(Before),
					*DescribeMSPFont(Text->GetFont()),
					SetFontCount,
					Source,
					LabelStyleOverrideCount,
					ValueStyleOverrideCount);
			}
		}
	}
}

void AMenuHUD::InitializeAuthenticMSPSettingWidget(UUserWidget* Widget)
{
	if (!Widget || !SettingsAdapter || InitializedMSPSettingWidgets.Contains(Widget))
	{
		return;
	}

	EAvMainMenuSettingRow Row;
	if (IsInsideMSPLanguageMenu(Widget) &&
		Widget->GetName().Contains(TEXT("OutputDeviceOptionsPicker")))
	{
		Row = EAvMainMenuSettingRow::Language;
	}
	else if (!FindMSPSettingRow(Widget->GetName(), Row))
	{
		return;
	}

	MSPSettingRows.Add(Widget, static_cast<uint8>(Row));
	if (const TCHAR* DescriptionKey = MSPDescriptionKeyForRow(Row))
	{
		ActiveMSPDescriptionKeys.FindOrAdd(Widget) = DescriptionKey;
	}
	Widget->SetIsEnabled(true);
	if (IsMSPSliderRow(Row))
	{
		float Minimum = 0.f;
		float Maximum = 100.f;
		float Step = 5.f;
		if (Row == EAvMainMenuSettingRow::Brightness)
		{
			Minimum = 50.f;
			Maximum = 150.f;
		}
		else if (Row == EAvMainMenuSettingRow::ColorDeficiencyStrength)
		{
			Maximum = 10.f;
			Step = 1.f;
		}
		else if (Row == EAvMainMenuSettingRow::ResolutionScale)
		{
			Minimum = 50.f;
		}
		WriteMSPNumericProperty(Widget, TEXT("SliderMinValue"), Minimum);
		WriteMSPNumericProperty(Widget, TEXT("SliderMaxValue"), Maximum);
		WriteMSPNumericProperty(Widget, TEXT("SliderStepSize"), Step);
		WriteMSPNumericProperty(Widget, TEXT("MouseSliderStepSize"), Step);
		WriteMSPNumericProperty(Widget, TEXT("ValueDecimalPlaces"), 0);
		WriteMSPNumericProperty(Widget, TEXT("bSetSettings"), 0);
		if (USlider* Slider = FindFirstSlider(Widget))
		{
			Slider->SetMinValue(Minimum);
			Slider->SetMaxValue(Maximum);
			Slider->SetStepSize(Step);
		}
		const float Current = SettingsAdapter->GetPendingScalar(Row);
		WriteMSPNumericProperty(Widget, TEXT("SliderDefaultValue"), Current);
		WriteMSPNumericProperty(Widget, TEXT("LoadedValue"), Current);
		InvokeMSPSliderValue(Widget, Current);
		MSPLastCapturedScalars.FindOrAdd(Widget) = Current;
	}
	else
	{
		WriteMSPOptions(Widget, SettingsAdapter->GetOptions(Row));
		const int32 Index = FMath::Max(0, SettingsAdapter->GetPendingIndex(Row));
		WriteMSPNumericProperty(Widget, TEXT("LoadedValue"), Index);
		InvokeMSPOptionIndex(Widget, Index);
		RefreshMSPOptionLabel(Widget, SettingsAdapter->GetValueText(Row));
		MSPLastCapturedIndices.FindOrAdd(Widget) = Index;
	}
	InitializedMSPSettingWidgets.Add(Widget);
	if (bMSPSettingsInteractionDiagnostics)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] SettingInitialized row=%d widget=%s class=%s identity=%p visibility=%s enabled=%s"),
			static_cast<int32>(Row),
			*Widget->GetPathName(),
			*Widget->GetClass()->GetPathName(),
			Widget,
			*StaticEnum<ESlateVisibility>()->GetNameStringByValue(
				static_cast<int64>(Widget->GetVisibility())),
			Widget->GetIsEnabled() ? TEXT("true") : TEXT("false"));
	}
}

void AMenuHUD::BindMSPSettingRowVisualState(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree || !MSPSettingRows.Contains(Widget))
	{
		return;
	}

	UAvMSPSettingEventProxy* EventProxy = nullptr;
	for (UAvMSPSettingEventProxy* ExistingProxy : MSPSettingEventProxies)
	{
		if (ExistingProxy && ExistingProxy->SettingWidget == Widget)
		{
			EventProxy = ExistingProxy;
			break;
		}
	}
	if (!EventProxy)
	{
		return;
	}

	const FLinearColor Transparent(1.f, 1.f, 1.f, 0.f);
	const FSlateColor WhiteForeground(FLinearColor::White);
	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	int32 HoverTargets = 0;
	for (UWidget* Child : Widgets)
	{
		if (UButton* Button = Cast<UButton>(Child))
		{
			const bool bRootHoverTarget =
				Button->GetFName() == TEXT("HoverButton");
			Button->OnHovered.RemoveDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowHovered);
			Button->OnUnhovered.RemoveDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowUnhovered);
			Button->OnPressed.RemoveDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowPressed);
			Button->OnReleased.RemoveDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowReleased);
			if (bRootHoverTarget)
			{
				Button->OnHovered.AddUniqueDynamic(
					EventProxy, &UAvMSPSettingEventProxy::HandleRowHovered);
				Button->OnUnhovered.AddUniqueDynamic(
					EventProxy, &UAvMSPSettingEventProxy::HandleRowUnhovered);
			}
			Button->OnPressed.AddUniqueDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowPressed);
			Button->OnReleased.AddUniqueDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowReleased);
			Button->OnReceivedFocus.BindUObject(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowFocused);
			Button->OnLostFocus.BindUObject(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowFocusLost);

			// Background state is rendered by the two authored row borders.
			// Keep every input button brush transparent so hovering an arrow
			// cannot highlight only the right-hand control segment.
			FButtonStyle Style = Button->GetStyle();
			Style.Normal.TintColor = FSlateColor(Transparent);
			Style.Hovered.TintColor = FSlateColor(Transparent);
			Style.Pressed.TintColor = FSlateColor(Transparent);
			Style.Disabled.TintColor = FSlateColor(Transparent);
			Style.SetNormalForeground(WhiteForeground);
			Style.SetHoveredForeground(WhiteForeground);
			Style.SetPressedForeground(WhiteForeground);
			Style.SetDisabledForeground(FSlateColor(
				FLinearColor(0.45f, 0.47f, 0.50f, 1.f)));
			Button->SetStyle(Style);

			if (bRootHoverTarget)
			{
				// This authored button spans the complete row. It had been made
				// SelfHitTestInvisible by the project bridge, which made its
				// native hover path unreachable over labels and empty space.
				Button->SetVisibility(ESlateVisibility::Visible);
				PRAGMA_DISABLE_DEPRECATION_WARNINGS
				Button->IsFocusable = false;
				PRAGMA_ENABLE_DEPRECATION_WARNINGS
			}
			else if (Button->GetFName() == TEXT("PreviousOptionButton") ||
				Button->GetFName() == TEXT("NextOptionButton"))
			{
				PRAGMA_DISABLE_DEPRECATION_WARNINGS
				Button->IsFocusable = true;
				PRAGMA_ENABLE_DEPRECATION_WARNINGS
			}
			++HoverTargets;
		}
		else if (USlider* Slider = Cast<USlider>(Child))
		{
			Slider->OnMouseCaptureBegin.RemoveDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowPressed);
			Slider->OnMouseCaptureEnd.RemoveDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowReleased);
			Slider->OnControllerCaptureBegin.RemoveDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowPressed);
			Slider->OnControllerCaptureEnd.RemoveDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowReleased);
			Slider->OnMouseCaptureBegin.AddUniqueDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowPressed);
			Slider->OnMouseCaptureEnd.AddUniqueDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowReleased);
			Slider->OnControllerCaptureBegin.AddUniqueDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowPressed);
			Slider->OnControllerCaptureEnd.AddUniqueDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleRowReleased);
		}
	}

	const bool bFirstBinding = !BoundMSPSettingVisualRows.Contains(Widget);
	BoundMSPSettingVisualRows.Add(Widget);
	RefreshMSPSettingRowVisualState(
		Widget, bFirstBinding ? TEXT("InitialProjectBinding") :
			TEXT("IdempotentProjectRebind"), true);
	if (bMSPSettingsInteractionDiagnostics && bFirstBinding)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsHoverDiag] ProjectBinding row=%s class=%s identity=%p hoverTargets=%d bindCount=1 vendorHoverHandlers=0 tickCorrection=false timerRetry=false"),
			*Widget->GetPathName(), *Widget->GetClass()->GetPathName(),
			Widget, HoverTargets);
	}
}

void AMenuHUD::RefreshMSPSettingRowVisualState(
	UUserWidget* Widget, const TCHAR* Source, const bool bForce)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}

	const ESlateVisibility Visibility = Widget->GetVisibility();
	const bool bDisabled = !Widget->GetIsEnabled() ||
		Visibility == ESlateVisibility::Collapsed ||
		Visibility == ESlateVisibility::Hidden;
	bool bHovered = Widget->IsHovered();
	bool bFocused = Widget->HasAnyUserFocus();
	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		bHovered = bHovered || Child->IsHovered();
		bFocused = bFocused || Child->HasAnyUserFocus();
	}

	EAvSettingsVisualState NewState = EAvSettingsVisualState::Normal;
	if (bDisabled)
	{
		NewState = EAvSettingsVisualState::Disabled;
	}
	else if (PressedMSPSettingRows.Contains(Widget))
	{
		NewState = EAvSettingsVisualState::Pressed;
	}
	else if (bHovered)
	{
		// Mouse modality deliberately outranks keyboard focus.
		NewState = EAvSettingsVisualState::Hovered;
	}
	else if (bFocused)
	{
		NewState = EAvSettingsVisualState::Focused;
	}

	const EAvSettingsVisualState* Previous =
		MSPSettingVisualStates.Find(Widget);
	if (!bForce && Previous && *Previous == NewState)
	{
		return;
	}
	ApplyMSPSettingRowVisualState(Widget, NewState, Source);
}

void AMenuHUD::ApplyMSPSettingRowVisualState(
	UUserWidget* Widget, const EAvSettingsVisualState State,
	const TCHAR* Source)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}

	const FLinearColor NormalBackground(0.115f, 0.128f, 0.145f, 1.f);
	const FLinearColor HoverBackground(0.22f, 0.095f, 0.025f, 1.f);
	const FLinearColor FocusBackground(0.38f, 0.15f, 0.03f, 1.f);
	const FLinearColor PressedBackground(0.52f, 0.18f, 0.02f, 1.f);
	const FLinearColor DisabledBackground(0.065f, 0.072f, 0.082f, 0.92f);
	const FLinearColor WhiteText(0.96f, 0.97f, 0.98f, 1.f);
	const FLinearColor DisabledText(0.43f, 0.45f, 0.48f, 1.f);
	const FLinearColor MutedArrow(0.43f, 0.46f, 0.50f, 1.f);
	const FLinearColor OrangeArrow(1.f, 0.42f, 0.055f, 1.f);
	const FLinearColor BrightOrange(1.f, 0.49f, 0.10f, 1.f);
	const FLinearColor Track(0.18f, 0.20f, 0.23f, 1.f);

	FLinearColor Background = NormalBackground;
	FLinearColor Arrow = MutedArrow;
	FLinearColor SliderAccent(0.92f, 0.34f, 0.04f, 1.f);
	FLinearColor Text = WhiteText;
	const TCHAR* StateName = TEXT("Normal");
	switch (State)
	{
	case EAvSettingsVisualState::Hovered:
		Background = HoverBackground;
		Arrow = OrangeArrow;
		SliderAccent = BrightOrange;
		StateName = TEXT("Hovered");
		break;
	case EAvSettingsVisualState::Focused:
		Background = FocusBackground;
		Arrow = BrightOrange;
		SliderAccent = BrightOrange;
		StateName = TEXT("Focused");
		break;
	case EAvSettingsVisualState::Pressed:
		Background = PressedBackground;
		Arrow = BrightOrange;
		SliderAccent = BrightOrange;
		StateName = TEXT("Pressed");
		break;
	case EAvSettingsVisualState::Disabled:
		Background = DisabledBackground;
		Arrow = DisabledText;
		SliderAccent = DisabledText;
		Text = DisabledText;
		StateName = TEXT("Disabled");
		break;
	default:
		break;
	}

	// Some authored MSP rows (notably the production Language picker) bind
	// their brushes and foregrounds back to Blueprint color properties every
	// frame.  Update those existing state sources together with the rendered
	// widgets so a later binding evaluation cannot restore the vendor gray/red
	// palette after this project-owned state transition.
	for (const FName Property : {
		TEXT("LeftBackgroundColor"), TEXT("RightBackgroundColor"),
		TEXT("BarBackgroundColor"), TEXT("LeftColor"), TEXT("RightColor"),
		TEXT("ButtonColor"), TEXT("BackgroundColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, Background);
	}
	for (const FName Property : {
		TEXT("TextColor"), TEXT("TextHighlightColor"), TEXT("TextActiveColor"),
		TEXT("ValueColor"), TEXT("ValueHighlightColor"), TEXT("ValueActiveColor"),
		TEXT("ValueTextColor"), TEXT("ValueTextHighlightColor"),
		TEXT("ValueTextActiveColor"), TEXT("InnerTextColor"),
		TEXT("InnerTextHighlightColor"), TEXT("InnerTextActiveColor"),
		TEXT("ActiveOptionTextColor"), TEXT("ActiveOptionTextHighlightColor"),
		TEXT("ActiveOptionTextActiveColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, Text);
	}
	for (const FName Property : {
		TEXT("PreviousNextColor"), TEXT("PreviousNextHighlightColor"),
		TEXT("PreviousNextActiveColor"), TEXT("SubButtonColor"),
		TEXT("SubButtonHighlightColor"), TEXT("SubButtonActiveColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, Arrow);
	}
	for (const FName Property : {
		TEXT("BarColor"), TEXT("BarHighlightColor"), TEXT("BarActiveColor"),
		TEXT("HandleColor"), TEXT("HandleHighlightColor"),
		TEXT("HandleActiveColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, SliderAccent);
	}

	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		const FString Name = Child->GetName();
		if (UBorder* Border = Cast<UBorder>(Child))
		{
			if (Name.Contains(TEXT("LeftBorder")) ||
				Name.Contains(TEXT("RightBorder")))
			{
				Border->SetBrushColor(Background);
			}
			else if (Name.Contains(TEXT("BarBackground")))
			{
				Border->SetBrushColor(Track);
			}
		}
		else if (UTextBlock* TextBlock = Cast<UTextBlock>(Child))
		{
			// Labels and numeric/value text deliberately share the same white
			// policy in every interactive state.
			TextBlock->SetColorAndOpacity(FSlateColor(Text));
		}
		else if (UEditableText* EditableText = Cast<UEditableText>(Child))
		{
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
			FEditableTextStyle EditableStyle = EditableText->WidgetStyle;
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
			EditableStyle.SetColorAndOpacity(FSlateColor(Text));
			EditableText->SetWidgetStyle(EditableStyle);
		}
		else if (UButton* Button = Cast<UButton>(Child))
		{
			if (Name.Contains(TEXT("Previous")) ||
				Name.Contains(TEXT("Next")))
			{
				Button->SetColorAndOpacity(Arrow);
			}
		}
		else if (UImage* Image = Cast<UImage>(Child))
		{
			if (!Name.Contains(TEXT("Separator")) &&
				!Name.Contains(TEXT("Seperator")))
			{
				Image->SetColorAndOpacity(Arrow);
			}
		}
		else if (UProgressBar* Progress = Cast<UProgressBar>(Child))
		{
			if (Progress->GetFName() != TEXT("SliderProgressBar"))
			{
				Progress->SetFillColorAndOpacity(SliderAccent);
			}
		}
		else if (USlider* Slider = Cast<USlider>(Child))
		{
			FSliderStyle Style = Slider->GetWidgetStyle();
			Style.NormalBarImage.DrawAs =
				ESlateBrushDrawType::NoDrawType;
			Style.HoveredBarImage.DrawAs =
				ESlateBrushDrawType::NoDrawType;
			Style.DisabledBarImage.DrawAs =
				ESlateBrushDrawType::NoDrawType;
			Style.NormalBarImage.TintColor =
				FSlateColor(FLinearColor::Transparent);
			Style.HoveredBarImage.TintColor =
				FSlateColor(FLinearColor::Transparent);
			Style.DisabledBarImage.TintColor =
				FSlateColor(FLinearColor::Transparent);
			Slider->SetWidgetStyle(Style);
			Slider->SetSliderBarColor(FLinearColor::Transparent);
			Slider->SetSliderHandleColor(SliderAccent);
		}
	}
	UpdateMSPSliderVisual(Widget, Track, SliderAccent);
	UpdateMSPOptionIndicators(Widget, State, SliderAccent);

	const EAvSettingsVisualState Previous =
		MSPSettingVisualStates.FindRef(Widget);
	MSPSettingVisualStates.FindOrAdd(Widget) = State;
	Widget->InvalidateLayoutAndVolatility();
	if (bMSPSettingsInteractionDiagnostics && Previous != State)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsHoverDiag] StateTransition row=%s identity=%p state=%s source=%s valueText=white geometryChanged=false"),
			*Widget->GetPathName(), Widget, StateName, Source);
	}
}

void AMenuHUD::UpdateMSPSliderVisual(
	UUserWidget* Widget, const FLinearColor& TrackColor,
	const FLinearColor& FillColor)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}
	USlider* Slider = FindFirstSlider(Widget);
	UProgressBar* FillBar = Cast<UProgressBar>(
		Widget->GetWidgetFromName(TEXT("SliderProgressBar")));
	if (!Slider || !FillBar)
	{
		return;
	}
	FillBar->SetVisibility(ESlateVisibility::HitTestInvisible);

	auto MakeBarBrush = [](const FLinearColor& Color)
	{
		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		Brush.ImageSize = FVector2D(12.f, 6.f);
		Brush.TintColor = FSlateColor(Color);
		Brush.OutlineSettings.CornerRadii =
			FVector4(3.f, 3.f, 3.f, 3.f);
		Brush.OutlineSettings.RoundingType =
			ESlateBrushRoundingType::FixedRadius;
		return Brush;
	};
	FProgressBarStyle ProgressStyle = FillBar->GetWidgetStyle();
	ProgressStyle.SetBackgroundImage(MakeBarBrush(TrackColor));
	ProgressStyle.SetFillImage(MakeBarBrush(FillColor));
	FSlateBrush EmptyBrush;
	EmptyBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	ProgressStyle.SetMarqueeImage(EmptyBrush);
	ProgressStyle.SetEnableFillAnimation(false);
	FillBar->SetWidgetStyle(ProgressStyle);
	FillBar->SetBarFillType(EProgressBarFillType::LeftToRight);
	FillBar->SetBarFillStyle(EProgressBarFillStyle::Scale);
	FillBar->SetBorderPadding(FVector2D::ZeroVector);
	FillBar->SetFillColorAndOpacity(FLinearColor::White);

	const float Minimum = Slider->GetMinValue();
	const float Maximum = Slider->GetMaxValue();
	const float Percent = Maximum > Minimum ?
		FMath::Clamp(
			(Slider->GetValue() - Minimum) / (Maximum - Minimum),
			0.f, 1.f) :
		0.f;
	FillBar->SetPercent(Percent);
}

void AMenuHUD::UpdateMSPOptionIndicators(
	UUserWidget* Widget, const EAvSettingsVisualState State,
	const FLinearColor& AccentColor)
{
	if (!Widget || !SettingsAdapter)
	{
		return;
	}
	const uint8* RowValue = MSPSettingRows.Find(Widget);
	UScrollBox* Indicators = Cast<UScrollBox>(
		Widget->GetWidgetFromName(TEXT("SubButtonsScrollBox")));
	if (!RowValue || !Indicators)
	{
		return;
	}
	const EAvMainMenuSettingRow Row =
		static_cast<EAvMainMenuSettingRow>(*RowValue);
	const int32 ActiveIndex = SettingsAdapter->GetPendingIndex(Row);
	const int32 OptionCount = SettingsAdapter->GetOptions(Row).Num();
	const FLinearColor InactiveColor =
		State == EAvSettingsVisualState::Disabled ?
			FLinearColor(0.24f, 0.25f, 0.27f, 0.72f) :
			FLinearColor(0.48f, 0.50f, 0.53f, 1.f);
	Indicators->SetVisibility(ESlateVisibility::HitTestInvisible);

	TArray<UWidget*> IndicatorDescendants;
	UWidgetTree::GetChildWidgets(Indicators, IndicatorDescendants);
	TArray<UUserWidget*> IndicatorWidgets;
	for (UWidget* Descendant : IndicatorDescendants)
	{
		UUserWidget* Indicator = Cast<UUserWidget>(Descendant);
		if (Indicator && Indicator->GetClass()->GetPathName().Contains(
			TEXT("WBP_OptionsPickerSubButton")))
		{
			IndicatorWidgets.Add(Indicator);
		}
	}
	for (int32 Index = 0; Index < IndicatorWidgets.Num(); ++Index)
	{
		UUserWidget* Indicator = IndicatorWidgets[Index];
		const bool bSupportedOption = Index < OptionCount;
		Indicator->SetVisibility(bSupportedOption ?
			ESlateVisibility::HitTestInvisible :
			ESlateVisibility::Collapsed);
		if (!bSupportedOption)
		{
			continue;
		}
		const FLinearColor IndicatorColor =
			Index == ActiveIndex ? AccentColor : InactiveColor;
		WriteMSPLinearColorProperty(
			Indicator, TEXT("SubButtonColor"), IndicatorColor);
		WriteMSPLinearColorProperty(
			Indicator, TEXT("SubButtonHighlightColor"), IndicatorColor);
		WriteMSPLinearColorProperty(
			Indicator, TEXT("SubButtonActiveColor"), IndicatorColor);
		WriteMSPLinearColorProperty(
			Indicator, TEXT("SubButtonDisabledColor"), InactiveColor);
		WriteMSPLinearColorProperty(
			Indicator, TEXT("NormalColor"), IndicatorColor);
		WriteMSPLinearColorProperty(
			Indicator, TEXT("HighlightColor"), IndicatorColor);
		WriteMSPLinearColorProperty(
			Indicator, TEXT("ActiveColor"), IndicatorColor);
		WriteMSPLinearColorProperty(
			Indicator, TEXT("DisabledColor"), InactiveColor);
		if (FBoolProperty* ActiveProperty =
			FindFProperty<FBoolProperty>(
				Indicator->GetClass(), TEXT("bIsActiveIndex")))
		{
			ActiveProperty->SetPropertyValue_InContainer(
				Indicator, Index == ActiveIndex);
		}
		WriteMSPEnumProperty(
			Indicator, TEXT("WidgetStyleState"),
			Index == ActiveIndex ? TEXT("Active") : TEXT("Normal"));
		InvokeMSPEnumFunction(
			Indicator, TEXT("SetSubButtonStyle"),
			Index == ActiveIndex ? TEXT("Active") : TEXT("Normal"));
		InvokeMSPNoParameterFunction(
			Indicator, TEXT("SetSubButtonStyle"));
		if (UBorder* Border = Cast<UBorder>(
			Indicator->GetWidgetFromName(TEXT("ButtonBorder"))))
		{
			Border->SetBrushColor(IndicatorColor);
		}
		if (UButton* Button = Cast<UButton>(
			Indicator->GetWidgetFromName(TEXT("SubButton"))))
		{
			FButtonStyle ButtonStyle = Button->GetStyle();
			ButtonStyle.Normal.TintColor =
				FSlateColor(FLinearColor::Transparent);
			ButtonStyle.Hovered.TintColor =
				FSlateColor(FLinearColor::Transparent);
			ButtonStyle.Pressed.TintColor =
				FSlateColor(FLinearColor::Transparent);
			Button->SetStyle(ButtonStyle);
		}
	}
	if (bMSPSettingsInteractionDiagnostics &&
		!LoggedMSPOptionIndicatorRows.Contains(Widget))
	{
		LoggedMSPOptionIndicatorRows.Add(Widget);
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsPolish] OptionIndicators row=%s directChildren=%d authoredIndicators=%d visibleIndicators=%d activeIndex=%d sourceProperties=project-owned"),
			*Widget->GetPathName(), Indicators->GetChildrenCount(),
			IndicatorWidgets.Num(), FMath::Min(IndicatorWidgets.Num(), OptionCount),
			ActiveIndex);
	}
}

void AMenuHUD::UpdateMSPSettingDescription(
	UUserWidget* Widget, const TCHAR* Source)
{
	if (!Widget)
	{
		return;
	}
	const uint8* RowValue = MSPSettingRows.Find(Widget);
	if (!RowValue)
	{
		return;
	}
	const EAvMainMenuSettingRow Row =
		static_cast<EAvMainMenuSettingRow>(*RowValue);
	const TCHAR* DescriptionKey = MSPDescriptionKeyForRow(Row);
	if (!DescriptionKey)
	{
		return;
	}

	UUserWidget* DescriptionOwner = StableMSPGraphicsPage;
	if (Row <= EAvMainMenuSettingRow::ColorDeficiencyStrength)
	{
		DescriptionOwner = StableMSPScreenPage;
	}
	else if (Row >= EAvMainMenuSettingRow::MasterVolume &&
		Row <= EAvMainMenuSettingRow::VoiceVolume)
	{
		DescriptionOwner = StableMSPAudioPage;
	}
	else if (Row == EAvMainMenuSettingRow::Language)
	{
		DescriptionOwner = StableMSPLanguagePage;
	}
	if (!DescriptionOwner)
	{
		return;
	}

	if (UTextBlock* Name = Cast<UTextBlock>(
		DescriptionOwner->GetWidgetFromName(TEXT("SelectedOptionName"))))
	{
		Name->SetText(FAvLoc::Text(DescriptionKey));
	}
	for (const FName DescriptionName : {
		TEXT("SelectedOptionDescription"),
		TEXT("SelectedOptionDescription_1")})
	{
		if (URichTextBlock* Description = Cast<URichTextBlock>(
			DescriptionOwner->GetWidgetFromName(DescriptionName)))
		{
			Description->SetText(FAvLoc::Text(
				FString(DescriptionKey) + TEXT(".Description")));
		}
	}
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsPolish] Description row=%s key=%s source=%s page=%s"),
		*Widget->GetPathName(), DescriptionKey, Source,
		*DescriptionOwner->GetPathName());
}

void AMenuHUD::HandleMSPSettingRowHovered(UUserWidget* Widget)
{
	bMSPMouseInputActive = true;
	LogMSPSettingRowHoverTarget(Widget, TEXT("MouseEnter"));
	UpdateMSPSettingDescription(Widget, TEXT("MouseEnter"));
	RefreshMSPSettingRowVisualState(Widget, TEXT("MouseEnter"));
}

void AMenuHUD::HandleMSPSettingRowUnhovered(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}

	EAvSettingsVisualState State = EAvSettingsVisualState::Normal;
	if (!Widget->GetIsEnabled() ||
		Widget->GetVisibility() == ESlateVisibility::Collapsed ||
		Widget->GetVisibility() == ESlateVisibility::Hidden)
	{
		State = EAvSettingsVisualState::Disabled;
	}
	else if (PressedMSPSettingRows.Contains(Widget))
	{
		State = EAvSettingsVisualState::Pressed;
	}
	else
	{
		bool bFocused = Widget->HasAnyUserFocus();
		TArray<UWidget*> Widgets;
		Widget->WidgetTree->GetAllWidgets(Widgets);
		for (UWidget* Child : Widgets)
		{
			bFocused = bFocused || Child->HasAnyUserFocus();
		}
		if (bFocused)
		{
			State = EAvSettingsVisualState::Focused;
		}
	}
	ApplyMSPSettingRowVisualState(Widget, State, TEXT("MouseLeave"));
}

void AMenuHUD::HandleMSPSettingRowPressed(UUserWidget* Widget)
{
	bMSPMouseInputActive = true;
	PressedMSPSettingRows.Add(Widget);
	UpdateMSPSettingDescription(Widget, TEXT("MouseDown"));
	RefreshMSPSettingRowVisualState(Widget, TEXT("MouseDown"));
}

void AMenuHUD::HandleMSPSettingRowReleased(UUserWidget* Widget)
{
	PressedMSPSettingRows.Remove(Widget);
	RefreshMSPSettingRowVisualState(Widget, TEXT("MouseUp"));
}

void AMenuHUD::HandleMSPSettingRowFocused(UUserWidget* Widget)
{
	UpdateMSPSettingDescription(Widget, TEXT("KeyboardFocus"));
	RefreshMSPSettingRowVisualState(Widget, TEXT("KeyboardFocus"));
}

void AMenuHUD::HandleMSPSettingRowFocusLost(UUserWidget* Widget)
{
	RefreshMSPSettingRowVisualState(Widget, TEXT("KeyboardFocusLost"));
}

void AMenuHUD::LogMSPSettingRowHoverTarget(
	UUserWidget* Widget, const TCHAR* EventName)
{
	if (!bMSPSettingsInteractionDiagnostics || !Widget ||
		!Widget->WidgetTree ||
		LoggedMSPSettingHoverTargets.Contains(Widget))
	{
		return;
	}
	LoggedMSPSettingHoverTargets.Add(Widget);

	UWidget* ActualTarget = Widget;
	UButton* HoverButton = nullptr;
	FString StructuralWidgets;
	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		if (Child->IsHovered())
		{
			ActualTarget = Child;
		}
		if (UButton* Button = Cast<UButton>(Child);
			Button && Button->GetFName() == TEXT("HoverButton"))
		{
			HoverButton = Button;
		}
		if (Cast<UButton>(Child) || Cast<UBorder>(Child) ||
			Cast<UOverlay>(Child) || Cast<USizeBox>(Child) ||
			Cast<UImage>(Child) || Cast<USlider>(Child))
		{
			if (!StructuralWidgets.IsEmpty())
			{
				StructuralWidgets += TEXT("|");
			}
			StructuralWidgets += FString::Printf(
				TEXT("%s:%s"), *Child->GetClass()->GetName(),
				*Child->GetName());
		}
	}

	const auto VisibilityName = [](const UWidget* Candidate)
	{
		return StaticEnum<ESlateVisibility>()->GetNameStringByValue(
			static_cast<int64>(Candidate->GetVisibility()));
	};
	const bool bHitTestable = ActualTarget->GetVisibility() ==
		ESlateVisibility::Visible;
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsHoverDiag] %s actualWidget=%s object=%s class=%s outer=%s parentRow=%s rowClass=%s rowHovered=%s rootHoverButton=%s rootHovered=%s visibility=%s enabled=%s hitTestable=%s projectStyle=true vendorStyleAfterProject=false structures={%s}"),
		EventName,
		*ActualTarget->GetPathName(), *ActualTarget->GetName(),
		*ActualTarget->GetClass()->GetPathName(),
		*GetNameSafe(ActualTarget->GetOuter()),
		*Widget->GetPathName(), *Widget->GetClass()->GetPathName(),
		Widget->IsHovered() ? TEXT("true") : TEXT("false"),
		*GetNameSafe(HoverButton),
		HoverButton && HoverButton->IsHovered() ? TEXT("true") : TEXT("false"),
		*VisibilityName(ActualTarget),
		ActualTarget->GetIsEnabled() ? TEXT("true") : TEXT("false"),
		bHitTestable ? TEXT("true") : TEXT("false"),
		*StructuralWidgets);
}

void AMenuHUD::BindAuthenticMSPSettingInput(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree || !MSPSettingRows.Contains(Widget))
	{
		return;
	}

	UAvMSPSettingEventProxy* EventProxy = nullptr;
	for (UAvMSPSettingEventProxy* ExistingProxy : MSPSettingEventProxies)
	{
		if (ExistingProxy && ExistingProxy->SettingWidget == Widget)
		{
			EventProxy = ExistingProxy;
			break;
		}
	}
	if (!EventProxy)
	{
		EventProxy = NewObject<UAvMSPSettingEventProxy>(this);
		EventProxy->Initialize(this, Widget);
		MSPSettingEventProxies.Add(EventProxy);
	}

	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		if (UButton* Button = Cast<UButton>(Child))
		{
			const FName ButtonName = Button->GetFName();
			Button->OnHovered.Clear();
			Button->OnUnhovered.Clear();
			Button->OnPressed.Clear();
			Button->OnReleased.Clear();
			Button->OnReceivedFocus.Unbind();
			Button->OnLostFocus.Unbind();
			if (ButtonName == TEXT("HoverButton"))
			{
				Button->OnClicked.Clear();
				Button->SetVisibility(ESlateVisibility::Visible);
				PRAGMA_DISABLE_DEPRECATION_WARNINGS
				Button->IsFocusable = false;
				PRAGMA_ENABLE_DEPRECATION_WARNINGS
			}
			else if (ButtonName == TEXT("PreviousOptionButton"))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddUniqueDynamic(
					EventProxy,
					&UAvMSPSettingEventProxy::HandlePreviousClicked);
			}
			else if (ButtonName == TEXT("NextOptionButton"))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddUniqueDynamic(
					EventProxy,
					&UAvMSPSettingEventProxy::HandleNextClicked);
			}
			BoundMSPSettingInputButtons.Add(Button);
		}
		else if (USlider* Slider = Cast<USlider>(Child))
		{
			Slider->OnValueChanged.Clear();
			Slider->OnMouseCaptureBegin.Clear();
			Slider->OnMouseCaptureEnd.Clear();
			Slider->OnValueChanged.AddUniqueDynamic(
				EventProxy, &UAvMSPSettingEventProxy::HandleSliderChanged);
			Slider->OnMouseCaptureBegin.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPSettingSliderCaptureBegin);
			Slider->OnMouseCaptureEnd.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPSettingSliderCaptureEnd);
			BoundMSPSettingInputSliders.Add(Slider);
		}
		else if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Child))
		{
			ComboBox->OnSelectionChanged.Clear();
			ComboBox->OnSelectionChanged.AddUniqueDynamic(
				EventProxy,
				&UAvMSPSettingEventProxy::HandleComboSelectionChanged);
			BoundMSPSettingInputComboBoxes.Add(ComboBox);
		}
	}
	BindMSPSettingRowVisualState(Widget);
}

void AMenuHUD::AdjustMSPSettingOption(
	UUserWidget* Widget, const int32 Delta)
{
	if (!SettingsAdapter || bSynchronizingSettingsUI || !bMSPSettingsOpen ||
		!IsValid(Widget) || !IsInActiveMSPSubMenu(Widget) || Delta == 0)
	{
		return;
	}
	const uint8* RowValue = MSPSettingRows.Find(Widget);
	if (!RowValue)
	{
		return;
	}
	const EAvMainMenuSettingRow Row =
		static_cast<EAvMainMenuSettingRow>(*RowValue);
	const int32 OptionCount = SettingsAdapter->GetOptions(Row).Num();
	if (OptionCount <= 0)
	{
		return;
	}
	const int32 Current = SettingsAdapter->GetPendingIndex(Row);
	const int32 Next = (Current + FMath::Sign(Delta) + OptionCount) %
		OptionCount;
	CommitMSPSettingIndex(Widget, Next);
}

void AMenuHUD::CommitMSPSettingIndex(
	UUserWidget* Widget, const int32 Index)
{
	if (!SettingsAdapter || bSynchronizingSettingsUI || !bMSPSettingsOpen ||
		!IsValid(Widget) || !IsInActiveMSPSubMenu(Widget))
	{
		return;
	}
	const uint8* RowValue = MSPSettingRows.Find(Widget);
	if (!RowValue)
	{
		return;
	}
	const EAvMainMenuSettingRow Row =
		static_cast<EAvMainMenuSettingRow>(*RowValue);
	const int32 Before = SettingsAdapter->GetPendingIndex(Row);
	const int32 Saved = SettingsAdapter->GetOriginalIndex(Row);
	SettingsAdapter->SetPendingIndex(Row, Index);
	if (Row == EAvMainMenuSettingRow::Language)
	{
		RefreshAuthenticMSPLocalization();
		RefreshStableMSPSettingsVisualState(MSPSettingsActiveCategory);
	}
	SyncAuthenticMSPSettingsFromAdapter();
	RefreshMSPSettingRowVisualState(
		Widget, TEXT("ValueChanged"), true);
	UpdateMSPSettingDescription(Widget, TEXT("ValueChangedFinal"));
	RefreshMSPApplyState();
	if (bMSPSettingsInteractionDiagnostics)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] DirectIndexCommit row=%d previous=%d new=%d pendingValue=%d savedValue=%d HasPendingChanges=%s ApplyButtonCount=%d ApplyEnabled=%s widget=%s"),
			static_cast<int32>(Row), Before, Index,
			SettingsAdapter->GetPendingIndex(Row),
			Saved,
			SettingsAdapter->HasPendingChanges() ? TEXT("true") : TEXT("false"),
			MSPApplyButtons.Num(),
			MSPApplyButton.IsValid() && MSPApplyButton->GetIsEnabled() ?
				TEXT("true") : TEXT("false"),
			*Widget->GetPathName());
	}
}

void AMenuHUD::CommitMSPSettingScalar(
	UUserWidget* Widget, const float Value)
{
	if (!SettingsAdapter || bSynchronizingSettingsUI || !bMSPSettingsOpen ||
		!IsValid(Widget) || !IsInActiveMSPSubMenu(Widget))
	{
		return;
	}
	const uint8* RowValue = MSPSettingRows.Find(Widget);
	if (!RowValue)
	{
		return;
	}
	const EAvMainMenuSettingRow Row =
		static_cast<EAvMainMenuSettingRow>(*RowValue);
	const float Before = SettingsAdapter->GetPendingScalar(Row);
	const float Saved = SettingsAdapter->GetOriginalScalar(Row);
	SettingsAdapter->SetPendingScalar(Row, Value);
	WriteMSPNumericProperty(Widget, TEXT("CurrentValue"), Value);
	RefreshMSPSliderLabel(Widget, Value);
	MSPLastCapturedScalars.FindOrAdd(Widget) = Value;
	RefreshMSPSettingRowVisualState(
		Widget, TEXT("SliderValueChanged"), true);
	UpdateMSPSettingDescription(Widget, TEXT("SliderValueChangedFinal"));
	RefreshMSPApplyState();
	if (bMSPSettingsInteractionDiagnostics)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] DirectScalarCommit row=%d previous=%.3f new=%.3f pendingValue=%.3f savedValue=%.3f HasPendingChanges=%s ApplyButtonCount=%d ApplyEnabled=%s widget=%s"),
			static_cast<int32>(Row), Before, Value,
			SettingsAdapter->GetPendingScalar(Row),
			Saved,
			SettingsAdapter->HasPendingChanges() ? TEXT("true") : TEXT("false"),
			MSPApplyButtons.Num(),
			MSPApplyButton.IsValid() && MSPApplyButton->GetIsEnabled() ?
				TEXT("true") : TEXT("false"),
			*Widget->GetPathName());
	}
}

void AMenuHUD::BindMSPMouseModality(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}
	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		if (UButton* Button = Cast<UButton>(Child);
			Button && !BoundMSPMouseModalityButtons.Contains(Button))
		{
			Button->OnHovered.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPMouseModalityActivated);
			BoundMSPMouseModalityButtons.Add(Button);
		}
	}
}

void AMenuHUD::HandleMSPSettingSliderCaptureBegin()
{
	bMSPMouseInputActive = true;
	bMSPSliderDragging = true;
}

void AMenuHUD::HandleMSPSettingSliderCaptureEnd()
{
	bMSPSliderDragging = false;
}

void AMenuHUD::HandleMSPMouseModalityActivated()
{
	bMSPMouseInputActive = true;
}

void AMenuHUD::CaptureAuthenticMSPSettings(UUserWidget* TargetWidget)
{
	if (!SettingsAdapter || bSynchronizingSettingsUI)
	{
		return;
	}
	++MSPSettingsDiagnosticCaptureCalls;

	TArray<TPair<EAvMainMenuSettingRow, int32>> ChangedIndices;
	TArray<TPair<EAvMainMenuSettingRow, float>> ChangedScalars;
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		UUserWidget* Widget = Pair.Key.Get();
		if (!IsValid(Widget) || Widget->GetWorld() != GetWorld())
		{
			continue;
		}
		if (TargetWidget && Widget != TargetWidget)
		{
			continue;
		}
		if (!IsInActiveMSPSubMenu(Widget) ||
			Widget->GetVisibility() == ESlateVisibility::Collapsed ||
			Widget->GetVisibility() == ESlateVisibility::Hidden)
		{
			continue;
		}
		const EAvMainMenuSettingRow Row = static_cast<EAvMainMenuSettingRow>(Pair.Value);
		if (IsMSPSliderRow(Row))
		{
			const float Current = static_cast<float>(ReadMSPNumericProperty(
				Widget, TEXT("CurrentValue"), SettingsAdapter->GetPendingScalar(Row)));
			RefreshMSPSliderLabel(Widget, Current);
			const float Previous = MSPLastCapturedScalars.FindRef(Widget);
			if (!FMath::IsNearlyEqual(Current, Previous, 0.01f))
			{
				MSPLastCapturedScalars.FindOrAdd(Widget) = Current;
				ChangedScalars.Emplace(Row, Current);
			}
		}
		else
		{
			const int32 Current = static_cast<int32>(ReadMSPNumericProperty(
				Widget, TEXT("ActiveOptionIndex"), SettingsAdapter->GetPendingIndex(Row)));
			const int32 Previous = MSPLastCapturedIndices.FindRef(Widget);
			if (Current != Previous)
			{
				MSPLastCapturedIndices.FindOrAdd(Widget) = Current;
				ChangedIndices.Emplace(Row, Current);
			}
		}
	}

	bool bAspectChanged = false;
	bool bPresetChanged = false;
	bool bIndividualQualityChanged = false;
	bool bLanguageChanged = false;
	for (const TPair<EAvMainMenuSettingRow, int32>& Change : ChangedIndices)
	{
		if (Change.Key == EAvMainMenuSettingRow::AspectRatio)
		{
			SettingsAdapter->SetPendingIndex(Change.Key, Change.Value);
			bAspectChanged = true;
		}
		else if (Change.Key == EAvMainMenuSettingRow::OverallQuality)
		{
			SettingsAdapter->SetPendingIndex(Change.Key, Change.Value);
			bPresetChanged = true;
		}
	}

	auto IsIndividualQuality = [](const EAvMainMenuSettingRow Row)
	{
		return Row == EAvMainMenuSettingRow::GlobalIlluminationQuality ||
			Row == EAvMainMenuSettingRow::ShadowQuality ||
			Row == EAvMainMenuSettingRow::AntiAliasingQuality ||
			Row == EAvMainMenuSettingRow::ViewDistanceQuality ||
			Row == EAvMainMenuSettingRow::TextureQuality ||
			Row == EAvMainMenuSettingRow::EffectsQuality ||
			Row == EAvMainMenuSettingRow::ReflectionQuality ||
			Row == EAvMainMenuSettingRow::FoliageQuality ||
			Row == EAvMainMenuSettingRow::ShadingQuality ||
			Row == EAvMainMenuSettingRow::PostProcessQuality;
	};
	for (const TPair<EAvMainMenuSettingRow, int32>& Change : ChangedIndices)
	{
		if (Change.Key == EAvMainMenuSettingRow::AspectRatio ||
			Change.Key == EAvMainMenuSettingRow::OverallQuality ||
			(bPresetChanged && IsIndividualQuality(Change.Key)))
		{
			continue;
		}
		if (Change.Key == EAvMainMenuSettingRow::Language && Change.Value == 0)
		{
			// Culture switching updates localized text synchronously. Apply the
			// Cyrillic-capable font before SetCurrentCulture so Language labels
			// never spend a frame in the authored Kanit face, which has no
			// Cyrillic glyphs.
			for (TObjectIterator<UTextBlock> It; It; ++It)
			{
				UTextBlock* Text = *It;
				if (!IsValid(Text) || Text->GetWorld() != GetWorld())
				{
					continue;
				}
				const FName TextName = Text->GetFName();
				const bool bLanguageScreenText =
					IsInsideMSPLanguageMenu(Text) &&
					(TextName == TEXT("OptionNameText") ||
						TextName == TEXT("PickedOptionText") ||
						TextName == TEXT("HeadlineTitleText"));
				const bool bLanguageTabText =
					TextName == TEXT("ButtonText") &&
					(IsInsideMSPNamedWidget(Text, TEXT("GeneralSmallTabButton")) ||
						IsInsideMSPNamedWidget(Text, TEXT("ControlsLargeTabButton")));
				if (bLanguageScreenText || bLanguageTabText)
				{
					Text->SetFont(FCoreStyle::GetDefaultFontStyle(
						TEXT("Regular"), Text->GetFont().Size));
				}
			}
		}
		SettingsAdapter->SetPendingIndex(Change.Key, Change.Value);
		bLanguageChanged = bLanguageChanged ||
			Change.Key == EAvMainMenuSettingRow::Language;
		bIndividualQualityChanged = bIndividualQualityChanged || IsIndividualQuality(Change.Key);
	}
	for (const TPair<EAvMainMenuSettingRow, float>& Change : ChangedScalars)
	{
		SettingsAdapter->SetPendingScalar(Change.Key, Change.Value);
	}
	MSPSettingsDiagnosticPendingChanges += ChangedIndices.Num() + ChangedScalars.Num();
	if (bMSPSettingsInteractionDiagnostics &&
		(!ChangedIndices.IsEmpty() || !ChangedScalars.IsEmpty()))
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] PendingValueChanged count=%d indices=%d scalars=%d captureCall=%d"),
			ChangedIndices.Num() + ChangedScalars.Num(),
			ChangedIndices.Num(),
			ChangedScalars.Num(),
			MSPSettingsDiagnosticCaptureCalls);
	}
	if (bLanguageChanged)
	{
		RefreshAuthenticMSPLocalization();
	}
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		if (Pair.Key.IsValid() && IsInActiveMSPSubMenu(Pair.Key.Get()) &&
			static_cast<EAvMainMenuSettingRow>(Pair.Value) ==
				EAvMainMenuSettingRow::Language)
		{
			RefreshMSPOptionLabel(
				Pair.Key.Get(), SettingsAdapter->GetValueText(EAvMainMenuSettingRow::Language));
		}
	}
	if (bPresetChanged)
	{
		for (const EAvMainMenuSettingRow Row : {
			EAvMainMenuSettingRow::OverallQuality,
			EAvMainMenuSettingRow::GlobalIlluminationQuality,
			EAvMainMenuSettingRow::ShadowQuality,
			EAvMainMenuSettingRow::AntiAliasingQuality,
			EAvMainMenuSettingRow::ViewDistanceQuality,
			EAvMainMenuSettingRow::TextureQuality,
			EAvMainMenuSettingRow::EffectsQuality,
			EAvMainMenuSettingRow::ReflectionQuality,
			EAvMainMenuSettingRow::FoliageQuality,
			EAvMainMenuSettingRow::ShadingQuality,
			EAvMainMenuSettingRow::PostProcessQuality})
		{
			SyncAuthenticMSPSettingRow(static_cast<uint8>(Row));
		}
		RefreshMSPApplyState();
		return;
	}
	if (bIndividualQualityChanged)
	{
		SyncAuthenticMSPSettingRow(
			static_cast<uint8>(EAvMainMenuSettingRow::OverallQuality));
		RefreshMSPApplyState();
		return;
	}

	// Aspect-ratio changes alter the authoritative resolution list. Update only
	// when that control changed so dropdown interaction is never overwritten.
	if (!bAspectChanged)
	{
		if (!ChangedIndices.IsEmpty() || !ChangedScalars.IsEmpty())
		{
			RefreshMSPApplyState();
		}
		return;
	}
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		if (Pair.Value != static_cast<uint8>(EAvMainMenuSettingRow::Resolution) ||
			!Pair.Key.IsValid() || !IsInActiveMSPSubMenu(Pair.Key.Get()))
		{
			continue;
		}
		if (UUserWidget* ResolutionWidget = Pair.Key.Get())
		{
			WriteMSPOptions(
				ResolutionWidget, SettingsAdapter->GetOptions(EAvMainMenuSettingRow::Resolution));
			InvokeMSPOptionIndex(ResolutionWidget,
				FMath::Max(0, SettingsAdapter->GetPendingIndex(EAvMainMenuSettingRow::Resolution)));
			MSPLastCapturedIndices.FindOrAdd(ResolutionWidget) =
				FMath::Max(0, SettingsAdapter->GetPendingIndex(EAvMainMenuSettingRow::Resolution));
		}
	}
	RefreshMSPApplyState();
}

void AMenuHUD::SyncAuthenticMSPSettingsFromAdapter()
{
	if (!SettingsAdapter || bSynchronizingSettingsUI || bMSPSliderDragging)
	{
		return;
	}
	TGuardValue<bool> SynchronizingGuard(bSynchronizingSettingsUI, true);
	++MSPSettingsDiagnosticSyncCalls;
	if (bMSPSettingsInteractionDiagnostics)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] AdapterToWidgetSync call=%d rows=%d"),
			MSPSettingsDiagnosticSyncCalls,
			MSPSettingRows.Num());
	}

	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		UUserWidget* Widget = Pair.Key.Get();
		if (!IsValid(Widget) || Widget->GetWorld() != GetWorld())
		{
			continue;
		}
		if (!IsInActiveMSPSubMenu(Widget) ||
			Widget->GetVisibility() == ESlateVisibility::Collapsed ||
			Widget->GetVisibility() == ESlateVisibility::Hidden)
		{
			continue;
		}

		const EAvMainMenuSettingRow Row = static_cast<EAvMainMenuSettingRow>(Pair.Value);
		if (IsMSPSliderRow(Row))
		{
			const float Current = SettingsAdapter->GetPendingScalar(Row);
			WriteMSPNumericProperty(Widget, TEXT("SliderDefaultValue"), Current);
			WriteMSPNumericProperty(Widget, TEXT("LoadedValue"), Current);
			InvokeMSPSliderValue(Widget, Current);
			MSPLastCapturedScalars.FindOrAdd(Widget) = Current;
		}
		else
		{
			WriteMSPOptions(Widget, SettingsAdapter->GetOptions(Row));
			const int32 Index = FMath::Max(0, SettingsAdapter->GetPendingIndex(Row));
			WriteMSPNumericProperty(Widget, TEXT("LoadedValue"), Index);
			InvokeMSPOptionIndex(Widget, Index);
			RefreshMSPOptionLabel(Widget, SettingsAdapter->GetValueText(Row));
			MSPLastCapturedIndices.FindOrAdd(Widget) = Index;
		}
		RefreshMSPSettingRowVisualState(
			Widget, TEXT("AdapterSync"), true);
	}
	RefreshMSPApplyState();
}

void AMenuHUD::SyncAuthenticMSPSettingRow(const uint8 RowValue)
{
	if (!SettingsAdapter || bSynchronizingSettingsUI || bMSPSliderDragging)
	{
		return;
	}
	TGuardValue<bool> SynchronizingGuard(bSynchronizingSettingsUI, true);
	const EAvMainMenuSettingRow TargetRow =
		static_cast<EAvMainMenuSettingRow>(RowValue);
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		UUserWidget* Widget = Pair.Key.Get();
		if (!IsValid(Widget) || Pair.Value != RowValue ||
			!IsInActiveMSPSubMenu(Widget) ||
			Widget->GetVisibility() == ESlateVisibility::Collapsed ||
			Widget->GetVisibility() == ESlateVisibility::Hidden)
		{
			continue;
		}
		if (IsMSPSliderRow(TargetRow))
		{
			const float Current = SettingsAdapter->GetPendingScalar(TargetRow);
			WriteMSPNumericProperty(Widget, TEXT("SliderDefaultValue"), Current);
			WriteMSPNumericProperty(Widget, TEXT("LoadedValue"), Current);
			InvokeMSPSliderValue(Widget, Current);
			MSPLastCapturedScalars.FindOrAdd(Widget) = Current;
		}
		else
		{
			WriteMSPOptions(Widget, SettingsAdapter->GetOptions(TargetRow));
			const int32 Index = FMath::Max(0, SettingsAdapter->GetPendingIndex(TargetRow));
			WriteMSPNumericProperty(Widget, TEXT("LoadedValue"), Index);
			InvokeMSPOptionIndex(Widget, Index);
			RefreshMSPOptionLabel(Widget, SettingsAdapter->GetValueText(TargetRow));
			MSPLastCapturedIndices.FindOrAdd(Widget) = Index;
		}
		RefreshMSPSettingRowVisualState(
			Widget, TEXT("AdapterRowSync"), true);
	}
}

void AMenuHUD::RefreshAuthenticMSPLocalization()
{
	for (const TWeakObjectPtr<UUserWidget>& Widget : InitializedMSPBridgeWidgets)
	{
		if (Widget.IsValid() && Widget->GetWorld() == GetWorld())
		{
			LocalizeAuthenticMSPWidget(Widget.Get());
			ApplyAuthenticMSPFontPolicy(Widget.Get());
		}
	}
}

void AMenuHUD::RefreshMSPApplyState()
{
	const bool bHasChanges = SettingsAdapter && SettingsAdapter->HasPendingChanges();
	for (const TWeakObjectPtr<UUserWidget>& ApplyWidget : MSPApplyWidgets)
	{
		if (!ApplyWidget.IsValid())
		{
			continue;
		}
		// Set the outer widget first: its authored enabled-state callback writes
		// a button style, so the production button style below must be last.
		ApplyWidget->SetIsEnabled(bHasChanges);
		const FLinearColor ApplyNormal = bHasChanges ?
			FLinearColor(0.68f, 0.20f, 0.018f, 1.f) :
			FLinearColor(0.025f, 0.03f, 0.035f, 0.75f);
		const FLinearColor ApplyHighlight = bHasChanges ?
			FLinearColor(1.f, 0.40f, 0.045f, 1.f) :
			FLinearColor(0.038f, 0.047f, 0.057f, 0.9f);
		const FLinearColor ApplyText = bHasChanges ?
			FLinearColor(0.96f, 0.97f, 0.98f, 1.f) :
			FLinearColor(0.42f, 0.45f, 0.49f, 1.f);
		for (const FName Property : {TEXT("ButtonColor"), TEXT("BackgroundColor")})
		{
			WriteMSPLinearColorProperty(ApplyWidget.Get(), Property, ApplyNormal);
		}
		for (const FName Property : {
			TEXT("ButtonHighlightColor"), TEXT("BackgroundHighlightColor"),
			TEXT("ButtonActiveColor"), TEXT("BackgroundActiveColor")})
		{
			WriteMSPLinearColorProperty(ApplyWidget.Get(), Property, ApplyHighlight);
		}
		for (const FName Property : {
			TEXT("TextColor"), TEXT("TextHighlightColor"), TEXT("TextActiveColor")})
		{
			WriteMSPLinearColorProperty(ApplyWidget.Get(), Property, ApplyText);
		}
		InvokeMSPNoParameterFunction(ApplyWidget.Get(), TEXT("ApplyWidgetStyleNormal"));
		if (UTextBlock* ButtonText = Cast<UTextBlock>(
			ApplyWidget->GetWidgetFromName(TEXT("ButtonText"))))
		{
			ButtonText->SetText(FAvLoc::Text(TEXT("MainMenu.MSP.Common.Apply")));
			ButtonText->SetColorAndOpacity(FSlateColor(ApplyText));
		}
	}
	int32 ValidApplyButtons = 0;
	for (const TWeakObjectPtr<UButton>& ApplyButton : MSPApplyButtons)
	{
		if (!ApplyButton.IsValid())
		{
			continue;
		}
		++ValidApplyButtons;
		ApplyButton->SetIsEnabled(bHasChanges);
		FButtonStyle Style = ApplyButton->GetStyle();
		Style.Normal.TintColor = FSlateColor(bHasChanges ?
			FLinearColor(0.68f, 0.20f, 0.018f, 1.f) :
			FLinearColor(0.025f, 0.03f, 0.035f, 0.75f));
		Style.Hovered.TintColor = FSlateColor(
			bHasChanges ? FLinearColor(1.f, 0.38f, 0.055f, 1.f) :
			FLinearColor(0.038f, 0.047f, 0.057f, 0.9f));
		ApplyButton->SetStyle(Style);
	}
	if (bMSPSettingsInteractionDiagnostics)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] ApplyState pending=%s buttons=%d widgets=%d"),
			bHasChanges ? TEXT("true") : TEXT("false"),
			ValidApplyButtons, MSPApplyWidgets.Num());
	}
}

void AMenuHUD::ApplyAuthenticMSPStyle(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}

	const FLinearColor Graphite(0.012f, 0.016f, 0.021f, 0.98f);
	const FLinearColor Panel(0.025f, 0.031f, 0.038f, 0.98f);
	const FLinearColor Row(0.115f, 0.128f, 0.145f, 1.f);
	const FLinearColor RowHover(0.82f, 0.23f, 0.018f, 1.f);
	const FLinearColor Amber(1.f, 0.40f, 0.045f, 1.f);
	const FLinearColor AmberBright(1.f, 0.48f, 0.10f, 1.f);
	const FLinearColor Divider(0.18f, 0.20f, 0.22f, 0.9f);
	const FLinearColor Light(0.96f, 0.97f, 0.98f, 1.f);
	const FLinearColor Muted(0.62f, 0.65f, 0.69f, 1.f);
	const FLinearColor Disabled(0.075f, 0.083f, 0.095f, 0.92f);

	for (const FName Property : {TEXT("LeftBackgroundColor"), TEXT("RightBackgroundColor"),
		TEXT("BarBackgroundColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, Row);
	}
	for (const FName Property : {TEXT("LeftColor"), TEXT("RightColor"),
		TEXT("ButtonColor"), TEXT("BackgroundColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, Row);
	}
	for (const FName Property : {TEXT("LeftHighlightColor"), TEXT("RightHighlightColor"),
		TEXT("BarBackgroundHighlightColor"), TEXT("ButtonHighlightColor"),
		TEXT("BackgroundHighlightColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, RowHover);
	}
	for (const FName Property : {TEXT("LeftActiveColor"), TEXT("RightActiveColor"),
		TEXT("BarBackgroundActiveColor"), TEXT("ButtonActiveColor"),
		TEXT("BackgroundActiveColor"), TEXT("ActiveColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, Amber);
	}
	for (const FName Property : {TEXT("LeftDisabledColor"), TEXT("RightDisabledColor"),
		TEXT("BarBackgroundDisabledColor"), TEXT("ButtonDisabledColor"),
		TEXT("BackgroundDisabledColor"), TEXT("DisabledColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, Disabled);
	}
	for (const FName Property : {TEXT("BarColor"), TEXT("BarHighlightColor"),
		TEXT("BarActiveColor"), TEXT("HandleColor"), TEXT("HandleHighlightColor"),
		TEXT("HandleActiveColor"), TEXT("PreviousNextColor"),
		TEXT("PreviousNextHighlightColor"), TEXT("PreviousNextActiveColor"),
		TEXT("SubButtonActiveColor"), TEXT("SubButtonHighlightColor"),
		})
	{
		WriteMSPLinearColorProperty(Widget, Property, AmberBright);
	}
	WriteMSPLinearColorProperty(Widget, TEXT("HighlightColor"), RowHover);
	WriteMSPLinearColorProperty(Widget, TEXT("SubButtonColor"), Muted);
	for (const FName Property : {TEXT("TextColor"), TEXT("TextHighlightColor"),
		TEXT("TextActiveColor"), TEXT("TextDisabledColor"), TEXT("ValueColor"),
		TEXT("ValueHighlightColor"), TEXT("ValueActiveColor"), TEXT("ValueDisabledColor"),
		TEXT("ValueTextColor"), TEXT("ValueTextHighlightColor"),
		TEXT("ValueTextActiveColor"), TEXT("ValueTextDisabledColor"),
		TEXT("InnerTextColor"), TEXT("InnerTextHighlightColor"),
		TEXT("InnerTextActiveColor"), TEXT("InnerTextDisabledColor"),
		TEXT("ActiveOptionTextColor"), TEXT("ActiveOptionTextHighlightColor"),
		TEXT("ActiveOptionTextActiveColor"), TEXT("ActiveOptionTextDisabledColor")})
	{
		WriteMSPLinearColorProperty(Widget, Property, Light);
	}
	if (IsInsideMSPLanguageMenu(Widget) &&
		Widget->GetName().Contains(TEXT("OutputDeviceOptionsPicker")))
	{
		InvokeMSPNoParameterFunction(Widget, TEXT("ApplyWidgetStyleNormal"));
	}

	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		const FString Name = Child->GetName();
		if (UBorder* Border = Cast<UBorder>(Child))
		{
			if (Name.Contains(TEXT("Seperator")) || Name.Contains(TEXT("Separator")))
			{
				Border->SetBrushColor(Divider);
			}
			else if (Name.Contains(TEXT("MenuBorder")) || Name.Contains(TEXT("Description")) ||
				Name == TEXT("Border") || Name.Contains(TEXT("OuterBorder")))
			{
				Border->SetBrushColor(Graphite);
			}
			else if (Name.Contains(TEXT("LeftBorder")) || Name.Contains(TEXT("RightBorder")))
			{
				Border->SetBrushColor(Row);
			}
		}
		else if (UButton* Button = Cast<UButton>(Child))
		{
			FButtonStyle Style = Button->GetStyle();
			Style.Normal.TintColor = FSlateColor(Row);
			Style.Hovered.TintColor = FSlateColor(RowHover);
			Style.Pressed.TintColor = FSlateColor(Amber);
			Style.Disabled.TintColor = FSlateColor(Disabled);
			Button->SetStyle(Style);
		}
		else if (UTextBlock* Text = Cast<UTextBlock>(Child))
		{
			if (Name == TEXT("GameTitleText"))
			{
				// This is the one authored game title, not a decorative duplicate.
				// Keep it cook-safe by using text and font already owned by the
				// production widget instead of mounting another logo layer.
				Text->SetText(FAvLoc::Text(TEXT("MainMenu.Title")));
				Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
				Text->SetRenderOpacity(1.f);
				Text->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				Text->SetColorAndOpacity(FSlateColor(
					Name.Contains(TEXT("Description")) ? Muted : Light));
			}
			Text->SetAutoWrapText(Name.Contains(TEXT("Description")));
		}
		else if (URichTextBlock* Rich = Cast<URichTextBlock>(Child))
		{
			Rich->SetAutoWrapText(true);
		}
		else if (UImage* Image = Cast<UImage>(Child))
		{
			if (Name.Contains(TEXT("Arrow")) || Name.Contains(TEXT("ButtonImage")) ||
				Name.Contains(TEXT("Seperator")))
			{
				Image->SetColorAndOpacity(AmberBright);
			}
			else if (Name == TEXT("SelectedOptionImage"))
			{
				Image->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else if (UProgressBar* Progress = Cast<UProgressBar>(Child))
		{
			Progress->SetFillColorAndOpacity(AmberBright);
		}
		else if (USlider* Slider = Cast<USlider>(Child))
		{
			Slider->SetSliderBarColor(Panel);
			Slider->SetSliderHandleColor(AmberBright);
		}
	}
	if (MSPSettingRows.Contains(Widget))
	{
		// MSP's PreConstruct already applied the vendor data asset before the
		// project bridge replaces its source colors. Re-apply Normal exactly
		// once so the first rendered frame and every later hover/active state
		// use the same white value-text policy.
		InvokeMSPNoParameterFunction(Widget, TEXT("ApplyWidgetStyleNormal"));
		// Vendor Normal is the last authored style writer. Restore the
		// project-owned transparent hit targets and row state after it.
		BindMSPSettingRowVisualState(Widget);
	}

	FString DefaultDescriptionKey;
	const FString ClassPath = Widget->GetClass()->GetPathName();
	if (ClassPath.Contains(TEXT("WBP_DisplaySettings_Silence_C")))
	{
		DefaultDescriptionKey = TEXT("MainMenu.MSP.Video.WindowMode");
	}
	else if (ClassPath.Contains(TEXT("WBP_AvMSP_Graphics_C")))
	{
		DefaultDescriptionKey = TEXT("MainMenu.MSP.Graphics.QualityPreset");
	}
	else if (ClassPath.Contains(TEXT("WBP_AudioSettings_Silence_C")))
	{
		DefaultDescriptionKey = TEXT("MainMenu.MSP.Audio.Master");
	}
	else if (ClassPath.Contains(TEXT("WBP_AvMSP_Language_C")))
	{
		DefaultDescriptionKey = TEXT("MainMenu.Settings.Language");
	}
	if (!DefaultDescriptionKey.IsEmpty())
	{
		if (UTextBlock* Name = Cast<UTextBlock>(
			Widget->GetWidgetFromName(TEXT("SelectedOptionName"))))
		{
			Name->SetText(FAvLoc::Text(DefaultDescriptionKey));
		}
		for (const FName DescriptionName : {
			TEXT("SelectedOptionDescription"), TEXT("SelectedOptionDescription_1")})
		{
			if (URichTextBlock* Description = Cast<URichTextBlock>(
				Widget->GetWidgetFromName(DescriptionName)))
			{
				Description->SetText(FAvLoc::Text(
					DefaultDescriptionKey + TEXT(".Description")));
			}
		}
	}
	StyledMSPWidgets.Add(Widget);
}

void AMenuHUD::ApplyAuthenticMSPResponsiveLayout(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree || ResponsiveMSPWidgets.Contains(Widget))
	{
		return;
	}
	const FString ClassPath = Widget->GetClass()->GetPathName();
	const bool bSettingsLayout =
		ClassPath.Contains(TEXT("WBP_AvMSP_Settings_C")) ||
		ClassPath.Contains(TEXT("WBP_DisplaySettings_Silence_C")) ||
		ClassPath.Contains(TEXT("WBP_AvMSP_Graphics_C")) ||
		ClassPath.Contains(TEXT("WBP_AudioSettings_Silence_C")) ||
		ClassPath.Contains(TEXT("WBP_AvMSP_Language_C"));
	const bool bTopLevelSettingsTabs =
		ClassPath.Contains(TEXT("WBP_AvMSP_Settings_C"));
	const bool bLeafSettingsPage =
		ClassPath.Contains(TEXT("WBP_DisplaySettings_Silence_C")) ||
		ClassPath.Contains(TEXT("WBP_AvMSP_Graphics_C")) ||
		ClassPath.Contains(TEXT("WBP_AudioSettings_Silence_C")) ||
		ClassPath.Contains(TEXT("WBP_AvMSP_Language_C"));
	const bool bObsoleteInnerSettingsTabs =
		ClassPath.Contains(TEXT("WBP_AvMSP_VideoTabs_C")) ||
		ClassPath.Contains(TEXT("WBP_AvMSP_AudioTabs_C"));

	if (bObsoleteInnerSettingsTabs)
	{
		Widget->SetVisibility(ESlateVisibility::Collapsed);
		ResponsiveMSPWidgets.Add(Widget);
		return;
	}

	if (bSettingsLayout)
	{
		if (UWidget* Content = Widget->GetWidgetFromName(TEXT("HorizontalBox_1"));
			Content)
		{
			if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Content->Slot))
			{
				Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
				Slot->SetOffsets(FMargin(32.f, 122.f, 32.f, 96.f));
			}
		}
	}
	if (bLeafSettingsPage)
	{
		// The Silence settings base reserves a large image slot even when the
		// selected option has no artwork, which renders as a white placeholder.
		// Collapse its container once; description events may still update the
		// child image without being able to reopen or resize the panel.
		if (UWidget* DescriptionImageContainer =
			Widget->GetWidgetFromName(TEXT("SizeBox_0")))
		{
			DescriptionImageContainer->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (bTopLevelSettingsTabs)
	{
		if (UWidget* Tabs = Widget->GetWidgetFromName(TEXT("Border")); Tabs)
		{
			if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Tabs->Slot))
			{
				Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 0.f));
				Slot->SetOffsets(FMargin(
					32.f, 32.f, 32.f, 58.f));
			}
		}
	}
	if (MSPSettingRows.Contains(Widget))
	{
		for (const FName SizeBoxName : {TEXT("OptionsSizeBox"), TEXT("SliderSizeBox")})
		{
			if (USizeBox* RowSize = Cast<USizeBox>(Widget->GetWidgetFromName(SizeBoxName)))
			{
				RowSize->SetHeightOverride(52.f);
			}
		}
		if (UWidget* Spacer = Widget->GetWidgetFromName(TEXT("Spacer_0")))
		{
			Spacer->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (UWidget* Spacer = Widget->GetWidgetFromName(TEXT("Spacer_3")))
		{
			Spacer->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (UWidget* Left = Widget->GetWidgetFromName(TEXT("LeftBorder")))
		{
			if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Left->Slot))
			{
				FSlateChildSize Size(ESlateSizeRule::Fill);
				Size.Value = 0.64f;
				Slot->SetSize(Size);
			}
		}
		if (UWidget* Right = Widget->GetWidgetFromName(TEXT("RightBorder")))
		{
			if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Right->Slot))
			{
				FSlateChildSize Size(ESlateSizeRule::Fill);
				Size.Value = 0.36f;
				Slot->SetSize(Size);
			}
		}
	}
	if (UWidget* MenuBorder = Widget->GetWidgetFromName(TEXT("MenuBorder")))
	{
		if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(MenuBorder->Slot))
		{
			FSlateChildSize Size(ESlateSizeRule::Fill);
			Size.Value = 1.42f;
			Slot->SetSize(Size);
			Slot->SetPadding(FMargin(0.f, 0.f, 8.f, 0.f));
		}
	}
	if (UWidget* DescriptionBorder = Widget->GetWidgetFromName(TEXT("DescriptionBorder")))
	{
		if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(DescriptionBorder->Slot))
		{
			FSlateChildSize Size(ESlateSizeRule::Fill);
			Size.Value = 0.72f;
			Slot->SetSize(Size);
			Slot->SetPadding(FMargin(8.f, 0.f, 0.f, 0.f));
		}
	}
	if (UScrollBox* Scroll = Cast<UScrollBox>(Widget->GetWidgetFromName(TEXT("MenuScrollBox"))))
	{
		Scroll->SetScrollBarVisibility(ESlateVisibility::Visible);
		Scroll->SetScrollbarThickness(FVector2D(5.f, 5.f));
		Scroll->SetScrollbarPadding(FMargin(8.f, 0.f, 0.f, 0.f));
	}
	ResponsiveMSPWidgets.Add(Widget);
}

FString AMenuHUD::DescribeHoveredMSPSettingsControl() const
{
	auto Describe = [](const UWidget* Control)
	{
		const FString Visibility = StaticEnum<ESlateVisibility>()->GetNameStringByValue(
			static_cast<int64>(Control->GetVisibility()));
		return FString::Printf(
			TEXT("name=%s path=%s class=%s identity=%p visibility=%s enabled=%s hovered=%s focused=%s geometry=%.1fx%.1f"),
			*Control->GetName(),
			*Control->GetPathName(),
			*Control->GetClass()->GetPathName(),
			Control,
			*Visibility,
			Control->GetIsEnabled() ? TEXT("true") : TEXT("false"),
			Control->IsHovered() ? TEXT("true") : TEXT("false"),
			Control->HasAnyUserFocus() ? TEXT("true") : TEXT("false"),
			Control->GetCachedGeometry().GetLocalSize().X,
			Control->GetCachedGeometry().GetLocalSize().Y);
	};

	for (const TWeakObjectPtr<UButton>& Button : DiagnosticMSPButtons)
	{
		if (Button.IsValid() && Button->IsHovered())
		{
			return Describe(Button.Get());
		}
	}
	for (const TWeakObjectPtr<USlider>& Slider : DiagnosticMSPSliders)
	{
		if (Slider.IsValid() && Slider->IsHovered())
		{
			return Describe(Slider.Get());
		}
	}
	return TEXT("name=<none>");
}

void AMenuHUD::BindMSPSettingsInteractionDiagnostics(UUserWidget* Widget)
{
	if (!bMSPSettingsInteractionDiagnostics || !Widget || !Widget->WidgetTree)
	{
		return;
	}

	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		if (UButton* Button = Cast<UButton>(Child);
			Button && !DiagnosticMSPButtons.Contains(Button))
		{
			Button->OnHovered.AddUniqueDynamic(this, &AMenuHUD::HandleMSPDiagnosticHovered);
			Button->OnUnhovered.AddUniqueDynamic(this, &AMenuHUD::HandleMSPDiagnosticUnhovered);
			Button->OnPressed.AddUniqueDynamic(this, &AMenuHUD::HandleMSPDiagnosticPressed);
			Button->OnReleased.AddUniqueDynamic(this, &AMenuHUD::HandleMSPDiagnosticReleased);
			Button->OnClicked.AddUniqueDynamic(this, &AMenuHUD::HandleMSPDiagnosticClicked);
			DiagnosticMSPButtons.Add(Button);
			UE_LOG(LogTemp, Log,
				TEXT("[AvSettingsInteractionDiag] DelegateBinding control=%s identity=%p bindCount=1"),
				*Button->GetPathName(),
				Button);
		}
		else if (USlider* Slider = Cast<USlider>(Child);
			Slider && !DiagnosticMSPSliders.Contains(Slider))
		{
			Slider->OnValueChanged.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPDiagnosticSliderChanged);
			Slider->OnMouseCaptureBegin.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPDiagnosticSliderCaptureBegin);
			Slider->OnMouseCaptureEnd.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPDiagnosticSliderCaptureEnd);
			DiagnosticMSPSliders.Add(Slider);
			UE_LOG(LogTemp, Log,
				TEXT("[AvSettingsInteractionDiag] DelegateBinding control=%s identity=%p bindCount=1"),
				*Slider->GetPathName(),
				Slider);
		}
	}
}

void AMenuHUD::LogMSPSettingsInteractionSnapshot()
{
	if (!bMSPSettingsInteractionDiagnostics)
	{
		return;
	}

	FString Focused = TEXT("name=<none>");
	for (const TWeakObjectPtr<UButton>& Button : DiagnosticMSPButtons)
	{
		if (Button.IsValid() && Button->HasAnyUserFocus())
		{
			Focused = Button->GetPathName();
			break;
		}
	}
	if (Focused == TEXT("name=<none>"))
	{
		for (const TWeakObjectPtr<USlider>& Slider : DiagnosticMSPSliders)
		{
			if (Slider.IsValid() && Slider->HasAnyUserFocus())
			{
				Focused = Slider->GetPathName();
				break;
			}
		}
	}
	if (Focused != LastMSPSettingsDiagnosticFocus)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] FocusTransition old=%s new=%s hovered={%s}"),
			*LastMSPSettingsDiagnosticFocus,
			*Focused,
			*DescribeHoveredMSPSettingsControl());
		LastMSPSettingsDiagnosticFocus = Focused;
	}

	const double Now = FPlatformTime::Seconds();
	if (LastMSPSettingsDiagnosticSnapshotTime >= 0.0 &&
		Now - LastMSPSettingsDiagnosticSnapshotTime < 1.0)
	{
		return;
	}
	LastMSPSettingsDiagnosticSnapshotTime = Now;

	int32 SettingsInstances = 0;
	int32 SelfVisibleSettingsInstances = 0;
	for (const TWeakObjectPtr<UUserWidget>& Widget : MSPSettingsMenuWidgets)
	{
		if (Widget.IsValid())
		{
			++SettingsInstances;
			SelfVisibleSettingsInstances += Widget->IsVisible() ? 1 : 0;
		}
	}
	TMap<uint8, int32> RowInstanceCounts;
	int32 ActiveRowInstances = 0;
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		UUserWidget* RowWidget = Pair.Key.Get();
		if (IsValid(RowWidget) &&
			IsInActiveMSPSubMenu(RowWidget) &&
			RowWidget->GetVisibility() != ESlateVisibility::Collapsed &&
			RowWidget->GetVisibility() != ESlateVisibility::Hidden)
		{
			++ActiveRowInstances;
			++RowInstanceCounts.FindOrAdd(Pair.Value);
		}
	}
	int32 DuplicateRowKinds = 0;
	for (const TPair<uint8, int32>& Pair : RowInstanceCounts)
	{
		DuplicateRowKinds += Pair.Value > 1 ? 1 : 0;
	}
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] Snapshot bridgePass=%d settingsInstances=%d selfVisibleSettings=%d rows=%d activeRows=%d duplicateActiveRowKinds=%d captureCalls=%d syncCalls=%d focusCalls=%d pendingChanges=%d hovered={%s}"),
		MSPSettingsDiagnosticBridgePasses,
		SettingsInstances,
		SelfVisibleSettingsInstances,
		MSPSettingRows.Num(),
		ActiveRowInstances,
		DuplicateRowKinds,
		MSPSettingsDiagnosticCaptureCalls,
		MSPSettingsDiagnosticSyncCalls,
		MSPSettingsDiagnosticFocusCalls,
		MSPSettingsDiagnosticPendingChanges,
		*DescribeHoveredMSPSettingsControl());
}

void AMenuHUD::ApplyAuthenticMSPSupportPolicy(UUserWidget* Widget)
{
	if (!Widget || !Widget->WidgetTree)
	{
		return;
	}
	auto ApplyPolicy = [this](UWidget* Candidate)
	{
		if (!Candidate)
		{
			return;
		}
		const FString Name = Candidate->GetName();
		const bool bLanguageMenu = IsInsideMSPLanguageMenu(Candidate);
		if (bLanguageMenu)
		{
			if (IsInsideMSPNamedWidget(Candidate, TEXT("OutputDeviceOptionsPicker")))
			{
				if (UImage* Image = Cast<UImage>(Candidate))
				{
					Image->SetColorAndOpacity(FLinearColor(1.f, 0.42f, 0.06f, 1.f));
				}
			}
			if (Name.Contains(TEXT("MasterSlider")) || Name.Contains(TEXT("SFXSlider")) ||
				Name.Contains(TEXT("MusicSlider")) || Name.Contains(TEXT("VoiceSlider")) ||
				Name.Contains(TEXT("SeperatorHBox_Volume")))
			{
				Candidate->SetVisibility(ESlateVisibility::Collapsed);
				return;
			}
			if (Name == TEXT("SubButtonsScrollBox"))
			{
				Candidate->SetVisibility(ESlateVisibility::Collapsed);
				return;
			}
			if (Name.Contains(TEXT("OutputDeviceOptionsPicker")))
			{
				Candidate->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				Candidate->SetIsEnabled(true);
				return;
			}
		}
		if (Name.Contains(TEXT("NvidiaDLSS")) || Name.Contains(TEXT("FSR2")) ||
			Name.Contains(TEXT("VoiceChat")) || Name.Contains(TEXT("HDR")) ||
			Name.Contains(TEXT("Benchmark")) || Name.Contains(TEXT("UpscaleQuality")) ||
			Name.Contains(TEXT("TemporalUpsampling")) ||
			Name.Contains(TEXT("GlobalIlluminationMethodOptionsPicker")) ||
			Name.Contains(TEXT("ReflectionMethodOptionsPicker")))
		{
			Candidate->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (Name.Contains(TEXT("OutputDevice")) ||
			Name.Contains(TEXT("SeperatorHBox_General")))
		{
			Candidate->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (Name.Contains(TEXT("SFXSlider")) || Name.Contains(TEXT("MusicSlider")) ||
			Name.Contains(TEXT("VoiceSlider")) || Name.Contains(TEXT("SFXVolume")) ||
			Name.Contains(TEXT("MusicVolume")) || Name.Contains(TEXT("VoiceVolume")))
		{
			const bool bSupported = SettingsAdapter && SettingsAdapter->HasDedicatedAudioChannels();
			Candidate->SetVisibility(
				bSupported ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
			Candidate->SetIsEnabled(bSupported);
		}
	};
	ApplyPolicy(Widget);
	TArray<UWidget*> Widgets;
	Widget->WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Child : Widgets)
	{
		ApplyPolicy(Child);
	}

	// The vendor Display page points both selection references at a hidden HDR
	// control. Once that control is collapsed its delayed selection callback can
	// still replace the description with the English HDR copy. Point every leaf
	// page at its first supported project row before asking the authored widget
	// to establish its one initial selection.
	const FString ClassPath = Widget->GetClass()->GetPathName();
	FName DefaultRowName = NAME_None;
	if (ClassPath.Contains(TEXT("WBP_DisplaySettings_Silence_C")))
	{
		DefaultRowName = TEXT("WindowModeOptionsPicker");
	}
	else if (ClassPath.Contains(TEXT("WBP_AvMSP_Graphics_C")))
	{
		DefaultRowName = TEXT("PresetOptionsPicker");
	}
	else if (ClassPath.Contains(TEXT("WBP_AudioSettings_Silence_C")))
	{
		DefaultRowName = TEXT("MasterSlider");
	}
	else if (ClassPath.Contains(TEXT("WBP_AvMSP_Language_C")))
	{
		DefaultRowName = TEXT("OutputDeviceOptionsPicker");
	}
	if (!DefaultRowName.IsNone())
	{
		if (UWidget* DefaultRow = Widget->GetWidgetFromName(DefaultRowName))
		{
			WriteMSPObjectProperty(
				Widget, TEXT("DefaultSelectedUserWidget"), DefaultRow);
			WriteMSPObjectProperty(
				Widget, TEXT("LastSelectedUserWidget"), DefaultRow);
			InvokeMSPNoParameterFunction(
				Widget, TEXT("SetActiveDefaultOrLastSelectedWidget"));
			InvokeMSPNoParameterFunction(
				Widget, TEXT("HighlightDefaultOrLastSelectedWidget"));
		}
	}
}

void AMenuHUD::InitializeAuthenticMSPWidgetNow(UUserWidget* Widget)
{
	if (!IsValid(Widget) || InitializedMSPBridgeWidgets.Contains(Widget))
	{
		return;
	}

	LocalizeAuthenticMSPWidget(Widget);
	ApplyAuthenticMSPSupportPolicy(Widget);
	InitializeAuthenticMSPSettingWidget(Widget);
	BindAuthenticMSPSettingInput(Widget);
	ApplyAuthenticMSPStyle(Widget);
	// Setting rows are reparented once while the four production pages are
	// assembled behind the hidden stable switcher. Their vendor PreConstruct
	// restores config fonts during that final reparent, so the one creation-time
	// role pass is deliberately deferred until the stable hierarchy exists.
	if (!MSPSettingRows.Contains(Widget) ||
		!bStableMSPSettingsWarmupStarted || bStableMSPSettingsReady)
	{
		ApplyAuthenticMSPFontPolicy(Widget);
	}
	ApplyAuthenticMSPResponsiveLayout(Widget);
	InitializedMSPBridgeWidgets.Add(Widget);
	if (bMSPSettingsInteractionDiagnostics)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] BridgeInitialized widget=%s class=%s identity=%p pass=%d"),
			*Widget->GetPathName(),
			*Widget->GetClass()->GetPathName(),
			Widget,
			MSPSettingsDiagnosticBridgePasses);
	}
}

void AMenuHUD::RefreshAuthenticMSPBridge()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	++MSPSettingsDiagnosticBridgePasses;
	if (MSPSettingsDiagnosticBridgePasses > 200)
	{
		World->GetTimerManager().ClearTimer(AuthenticMSPBridgeTimer);
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenu] MSP bridge stopped after its bounded initialization window before reaching a stable runtime."));
		return;
	}

	int32 RootCount = 0;
	bool bVisibleSettingsMenu = false;
	bool bVisibleLanguageMenu = false;
	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Widget = *It;
		if (!IsValid(Widget) || Widget->GetWorld() != World)
		{
			continue;
		}
		const FString ClassPath = Widget->GetClass()->GetPathName();
		if (!ClassPath.Contains(TEXT("/Game/MenuSystemPro/")) &&
			!ClassPath.Contains(TEXT("/Game/Avariika/UI/MainMenu/MSP/")))
		{
			continue;
		}
		if (ClassPath.Contains(TEXT("WBP_AvMSP_MainMenuRoot_C")))
		{
			++RootCount;
			AuthenticMSPRoot = Widget;
			ApplyAuthenticMSPBackground(Widget);
		}
		else if (ClassPath.Contains(TEXT("WBP_AvMSP_Settings_C")))
		{
			MSPSettingsMenuWidgets.Add(Widget);
			if (Widget->IsVisible() || !MSPSettingsMenuWidget.IsValid())
			{
				MSPSettingsMenuWidget = Widget;
			}
			bVisibleSettingsMenu = bVisibleSettingsMenu || Widget->IsVisible();
		}
		else if (ClassPath.Contains(TEXT("WBP_AvMSP_Language_C")))
		{
			MSPLanguageMenuWidgets.Add(Widget);
			bVisibleLanguageMenu = bVisibleLanguageMenu || Widget->IsVisible();
		}
		else if (ClassPath.Contains(TEXT("WBP_MenuFooterBar_Silence_C")) &&
			Widget->IsVisible())
		{
			MSPFooterBarWidget = Widget;
		}

		if (!InitializedMSPBridgeWidgets.Contains(Widget))
		{
			InitializeAuthenticMSPWidgetNow(Widget);
		}

		const FString WidgetName = Widget->GetName();
		if (WidgetName.Contains(TEXT("ControlsLargeTabButton")))
		{
			BindMSPLanguageTab(Widget);
			BindMSPMouseModality(Widget);
			BindMSPSettingsInteractionDiagnostics(Widget);
			continue;
		}
		if (UButton* Button = FindFirstButton(Widget); Button && !BoundMSPButtons.Contains(Button))
		{
			const FName InputAction = ReadMSPNameProperty(Widget, TEXT("PlayerMappableOptionsName"));
			const FString OptionName = NormalizeMSPText(
				ReadMSPTextProperty(Widget, TEXT("OptionName")));
			if (InputAction == TEXT("IA_Benchmark") || OptionName == TEXT("BENCHMARK"))
			{
				Button->SetIsEnabled(false);
				Widget->SetIsEnabled(false);
				Widget->SetVisibility(ESlateVisibility::Collapsed);
				BoundMSPButtons.Add(Button);
			}
			else if (InputAction == TEXT("IA_Apply") || OptionName == TEXT("APPLY"))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddDynamic(this, &AMenuHUD::HandleMSPApplyClicked);
				MSPApplyButton = Button;
				MSPApplyWidget = Widget;
				MSPApplyButtons.Add(Button);
				MSPApplyWidgets.Add(Widget);
				BoundMSPButtons.Add(Button);
			}
			else if (InputAction == TEXT("IA_Reset") ||
				OptionName == TEXT("RESET SETTINGS"))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddDynamic(this, &AMenuHUD::HandleMSPResetClicked);
				MSPResetButton = Button;
				BoundMSPButtons.Add(Button);
			}
			else if (InputAction == TEXT("IA_Back") || OptionName == TEXT("BACK"))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddDynamic(this, &AMenuHUD::HandleMSPBackClicked);
				MSPBackButton = Button;
				BoundMSPButtons.Add(Button);
			}
			else if (WidgetName.Contains(TEXT("ContinueTitleScreenButton")))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddDynamic(this, &AMenuHUD::HandleMSPContinueClicked);
				Button->SetIsEnabled(SavePresenceAdapter && SavePresenceAdapter->Refresh());
				RegisterMSPHomeButton(TEXT("ContinueTitleScreenButton"), Button);
				BoundMSPButtons.Add(Button);
			}
			else if (WidgetName.Contains(TEXT("PlayTitleScreenButton")))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddDynamic(this, &AMenuHUD::HandleMSPNewGameClicked);
				RegisterMSPHomeButton(TEXT("PlayTitleScreenButton"), Button);
				BoundMSPButtons.Add(Button);
			}
			else if (WidgetName.Contains(TEXT("MultiplayerTitleScreenButton")))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddDynamic(this, &AMenuHUD::HandleMSPMultiplayerClicked);
				RegisterMSPHomeButton(TEXT("MultiplayerTitleScreenButton"), Button);
				BoundMSPButtons.Add(Button);
			}
			else if (WidgetName.Contains(TEXT("OptionsTitleScreenButton")))
			{
				// The authored Home route creates a fresh Settings tree. Replace it
				// with the single stable runtime router before any click can fire.
				Button->OnClicked.Clear();
				Button->OnClicked.AddUniqueDynamic(this, &AMenuHUD::HandleMSPSettingsClicked);
				RegisterMSPHomeButton(TEXT("OptionsTitleScreenButton"), Button);
				BoundMSPButtons.Add(Button);
			}
			else if (WidgetName.Contains(TEXT("ExtrasTitleScreenButton")))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddDynamic(this, &AMenuHUD::HandleMSPCreditsClicked);
				RegisterMSPHomeButton(TEXT("ExtrasTitleScreenButton"), Button);
				BoundMSPButtons.Add(Button);
			}
			else if (WidgetName.Contains(TEXT("QuitTitleScreenButton")))
			{
				Button->OnClicked.Clear();
				Button->OnClicked.AddDynamic(this, &AMenuHUD::HandleMSPExitClicked);
				RegisterMSPHomeButton(TEXT("QuitTitleScreenButton"), Button);
				BoundMSPButtons.Add(Button);
			}
		}
		BindMSPMouseModality(Widget);
		BindMSPSettingsInteractionDiagnostics(Widget);
	}
	EnsureStableMSPSettingsRuntime();
	ValidateStableMSPSettingsInvariant();
	if (!bVisibleSettingsMenu)
	{
		ActiveMSPVisualSubMenuClassPath.Empty();
	}
	if (bFocusMSPSettingsAfterLanguageBack && bVisibleSettingsMenu && !bVisibleLanguageMenu)
	{
		if (!bMSPMouseInputActive &&
			MSPLanguageReturnFocusButton.IsValid() &&
			MSPLanguageReturnFocusButton->GetIsEnabled())
		{
			FocusMSPButton(MSPLanguageReturnFocusButton.Get());
		}
		else if (!bMSPMouseInputActive)
		{
			FocusFirstVisibleMSPSettingsButton();
		}
		bFocusMSPSettingsAfterLanguageBack = false;
		MSPLanguageReturnFocusButton.Reset();
	}
	if ((ActiveMSPOverlayKind == EMSPOverlay::ComingSoon ||
		ActiveMSPOverlayKind == EMSPOverlay::ExitConfirm) &&
		MSPOverlayNoButton.IsValid() &&
		!MSPOverlayNoButton->HasAnyUserFocus() &&
		(!MSPOverlayYesButton.IsValid() || !MSPOverlayYesButton->HasAnyUserFocus()))
	{
		bMSPOverlayYesFocused = false;
		FocusMSPButton(MSPOverlayNoButton.Get());
	}
	ConfigureMSPHomeNavigation();
	if (!bMSPSettingsOpen)
	{
		RefreshMSPHomeFocus();
	}
	LogMSPSettingsInteractionSnapshot();
	if (RootCount > 1 && !bReportedDuplicateMSPRoot)
	{
		bReportedDuplicateMSPRoot = true;
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenu] Authentic MSP duplicate root count: %d"), RootCount);
	}
	RemoveLegacyMSPJsonMirror();
}

void AMenuHUD::RegisterMSPHomeButton(const FName OwnerName, UButton* Button)
{
	if (Button)
	{
		MSPHomeButtons.FindOrAdd(OwnerName) = Button;
		Button->OnHovered.AddUniqueDynamic(
			this, &AMenuHUD::HandleMSPHomeButtonHovered);
		Button->OnUnhovered.AddUniqueDynamic(
			this, &AMenuHUD::HandleMSPHomeButtonUnhovered);
		Button->OnPressed.AddUniqueDynamic(
			this, &AMenuHUD::HandleMSPHomeButtonPressed);
		Button->OnReleased.AddUniqueDynamic(
			this, &AMenuHUD::HandleMSPHomeButtonReleased);
	}
}

void AMenuHUD::ConfigureMSPHomeNavigation()
{
	TArray<UButton*> AvailableButtons;
	for (const FName OwnerName : MSPHomeButtonOrder())
	{
		if (const TWeakObjectPtr<UButton>* Candidate = MSPHomeButtons.Find(OwnerName);
			Candidate && Candidate->IsValid() && Candidate->Get()->IsVisible() &&
			Candidate->Get()->GetIsEnabled())
		{
			AvailableButtons.Add(Candidate->Get());
		}
	}

	if (AvailableButtons.Num() < 2)
	{
		return;
	}

	for (int32 Index = 0; Index < AvailableButtons.Num(); ++Index)
	{
		UButton* Button = AvailableButtons[Index];
		UButton* Previous = AvailableButtons[(Index - 1 + AvailableButtons.Num()) % AvailableButtons.Num()];
		UButton* Next = AvailableButtons[(Index + 1) % AvailableButtons.Num()];
		Button->SetNavigationRuleExplicit(EUINavigation::Up, Previous);
		Button->SetNavigationRuleExplicit(EUINavigation::Down, Next);
	}
}

void AMenuHUD::RefreshMSPHomeButtonVisualStates(const TCHAR* Source)
{
	int32 HoveredCount = 0;
	for (const TPair<FName, TWeakObjectPtr<UButton>>& Pair : MSPHomeButtons)
	{
		UButton* Button = Pair.Value.Get();
		if (!Button)
		{
			continue;
		}

		EAvSettingsVisualState State = EAvSettingsVisualState::Normal;
		if (!Button->GetIsEnabled() || !Button->IsVisible())
		{
			State = EAvSettingsVisualState::Disabled;
		}
		else if (MSPPressedHomeButton.Get() == Button && Button->IsPressed())
		{
			State = EAvSettingsVisualState::Pressed;
		}
		else if (bMSPMouseInputActive &&
			MSPHoveredHomeButton.Get() == Button && Button->IsHovered())
		{
			State = EAvSettingsVisualState::Hovered;
		}
		else if (!bMSPMouseInputActive && Button->HasAnyUserFocus())
		{
			State = EAvSettingsVisualState::Focused;
		}
		if (State == EAvSettingsVisualState::Hovered ||
			State == EAvSettingsVisualState::Focused ||
			State == EAvSettingsVisualState::Pressed)
		{
			++HoveredCount;
		}
		ApplyMSPHomeButtonVisualState(Button, State, Source);
	}

	UE_LOG(LogTemp, Log,
		TEXT("[AvMainMenuHoverExclusive] source=%s modality=%s hovered=%s pressed=%s highlightedCount=%d"),
		Source,
		bMSPMouseInputActive ? TEXT("Mouse") : TEXT("Keyboard"),
		MSPHoveredHomeButton.IsValid() ?
			*MSPHoveredHomeButton->GetPathName() : TEXT("<none>"),
		MSPPressedHomeButton.IsValid() ?
			*MSPPressedHomeButton->GetPathName() : TEXT("<none>"),
		HoveredCount);
}

void AMenuHUD::ApplyMSPHomeButtonVisualState(
	UButton* Button, const EAvSettingsVisualState State, const TCHAR* Source)
{
	if (!Button)
	{
		return;
	}

	const FLinearColor Normal(0.025f, 0.031f, 0.038f, 0.96f);
	const FLinearColor Hovered(1.f, 0.40f, 0.045f, 1.f);
	const FLinearColor Focused(0.86f, 0.31f, 0.035f, 1.f);
	const FLinearColor Pressed(0.68f, 0.20f, 0.018f, 1.f);
	const FLinearColor Disabled(0.025f, 0.028f, 0.032f, 0.86f);
	FLinearColor StateColor = Normal;
	const TCHAR* StateName = TEXT("Normal");
	switch (State)
	{
	case EAvSettingsVisualState::Hovered:
		StateColor = Hovered;
		StateName = TEXT("Hovered");
		break;
	case EAvSettingsVisualState::Focused:
		StateColor = Focused;
		StateName = TEXT("Focused");
		break;
	case EAvSettingsVisualState::Pressed:
		StateColor = Pressed;
		StateName = TEXT("Pressed");
		break;
	case EAvSettingsVisualState::Disabled:
		StateColor = Disabled;
		StateName = TEXT("Disabled");
		break;
	default:
		break;
	}

	bool bAppliedOwnerState = false;
	FVector2D OwnerAbsoluteBefore = FVector2D::ZeroVector;
	FVector2D OwnerAbsoluteAfter = FVector2D::ZeroVector;
	FButtonStyle StableButtonStyle = Button->GetStyle();
	if (UUserWidget* OwnerWidget = Button->GetTypedOuter<UUserWidget>())
	{
		OwnerWidget->StopAllAnimations();
		InvokeMSPNoParameterFunction(
			OwnerWidget, TEXT("ApplyWidgetStyleNormal"));
		OwnerWidget->StopAllAnimations();

		TArray<UWidget*> GeometryWidgets;
		GeometryWidgets.Add(OwnerWidget);
		if (OwnerWidget->WidgetTree)
		{
			OwnerWidget->WidgetTree->GetAllWidgets(GeometryWidgets);
		}
		TMap<TWeakObjectPtr<UWidget>, FWidgetTransform> StableTransforms;
		TMap<TWeakObjectPtr<UWidget>, FVector2D> StablePivots;
		for (UWidget* GeometryWidget : GeometryWidgets)
		{
			if (GeometryWidget)
			{
				StableTransforms.Add(
					GeometryWidget, GeometryWidget->GetRenderTransform());
				StablePivots.Add(
					GeometryWidget, GeometryWidget->GetRenderTransformPivot());
			}
		}
		StableButtonStyle = Button->GetStyle();
		OwnerAbsoluteBefore =
			OwnerWidget->GetCachedGeometry().GetAbsolutePosition();

		FName StyleFunction = TEXT("ApplyWidgetStyleNormal");
		if (State == EAvSettingsVisualState::Hovered ||
			State == EAvSettingsVisualState::Focused)
		{
			StyleFunction = TEXT("ApplyWidgetStyleHighlight");
		}
		else if (State == EAvSettingsVisualState::Pressed)
		{
			StyleFunction = TEXT("ApplyWidgetStylePressed");
		}
		bAppliedOwnerState =
			InvokeMSPNoParameterFunction(OwnerWidget, StyleFunction);
		if (!bAppliedOwnerState &&
			(State == EAvSettingsVisualState::Hovered ||
				State == EAvSettingsVisualState::Focused))
		{
			bAppliedOwnerState = InvokeMSPNoParameterFunction(
				OwnerWidget, TEXT("ApplyWidgetStyleActive"));
		}
		OwnerWidget->StopAllAnimations();
		for (const TPair<TWeakObjectPtr<UWidget>, FWidgetTransform>& Pair :
			StableTransforms)
		{
			if (Pair.Key.IsValid())
			{
				Pair.Key->SetRenderTransform(Pair.Value);
				if (const FVector2D* Pivot = StablePivots.Find(Pair.Key))
				{
					Pair.Key->SetRenderTransformPivot(*Pivot);
				}
			}
		}
		OwnerWidget->InvalidateLayoutAndVolatility();
		OwnerAbsoluteAfter =
			OwnerWidget->GetCachedGeometry().GetAbsolutePosition();
	}

	FButtonStyle Style = Button->GetStyle();
	Style.Normal.TintColor = FSlateColor(StateColor);
	Style.Hovered.TintColor = FSlateColor(StateColor);
	Style.Pressed.TintColor = FSlateColor(
		State == EAvSettingsVisualState::Disabled ? Disabled : Pressed);
	Style.Disabled.TintColor = FSlateColor(Disabled);
	Style.NormalPadding = StableButtonStyle.NormalPadding;
	Style.PressedPadding = StableButtonStyle.NormalPadding;
	Style.SetNormalForeground(FSlateColor(FLinearColor::White));
	Style.SetHoveredForeground(FSlateColor(FLinearColor::White));
	Style.SetPressedForeground(FSlateColor(FLinearColor::White));
	Button->SetStyle(Style);

	if (bMSPSettingsInteractionDiagnostics)
	{
		UE_LOG(LogTemp, Verbose,
			TEXT("[AvMainMenuHoverExclusive] state=%s source=%s button=%s focused=%s hovered=%s ownerStateApplied=%s absoluteBefore=(%.2f,%.2f) absoluteAfter=(%.2f,%.2f)"),
			StateName, Source, *Button->GetPathName(),
			Button->HasAnyUserFocus() ? TEXT("true") : TEXT("false"),
			Button->IsHovered() ? TEXT("true") : TEXT("false"),
			bAppliedOwnerState ? TEXT("true") : TEXT("false"),
			OwnerAbsoluteBefore.X, OwnerAbsoluteBefore.Y,
			OwnerAbsoluteAfter.X, OwnerAbsoluteAfter.Y);
	}
}

void AMenuHUD::HandleMSPHomeButtonHovered()
{
	UButton* NewHovered = nullptr;
	FName NewOwner = NAME_None;
	for (const TPair<FName, TWeakObjectPtr<UButton>>& Pair : MSPHomeButtons)
	{
		if (Pair.Value.IsValid() && Pair.Value->IsHovered())
		{
			NewHovered = Pair.Value.Get();
			NewOwner = Pair.Key;
			break;
		}
	}
	if (!NewHovered)
	{
		return;
	}

	const FString Previous = MSPHoveredHomeButton.IsValid() ?
		MSPHoveredHomeButton->GetPathName() : TEXT("<none>");
	MSPHoveredHomeButton = NewHovered;
	MSPPressedHomeButton.Reset();
	bMSPMouseInputActive = true;
	const int32 HoveredIndex = MSPHomeButtonOrder().IndexOfByKey(NewOwner);
	if (HoveredIndex != INDEX_NONE)
	{
		MSPHomeFocusIndex = HoveredIndex;
	}
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Mouse);
	}
	RefreshMSPHomeButtonVisualStates(TEXT("MouseEnter"));
	UE_LOG(LogTemp, Log,
		TEXT("[AvMainMenuHoverExclusive] transition=%s -> %s owner=%s"),
		*Previous, *NewHovered->GetPathName(), *NewOwner.ToString());
}

void AMenuHUD::HandleMSPHomeButtonUnhovered()
{
	UButton* CurrentHovered = nullptr;
	for (const TPair<FName, TWeakObjectPtr<UButton>>& Pair : MSPHomeButtons)
	{
		if (Pair.Value.IsValid() && Pair.Value->IsHovered())
		{
			CurrentHovered = Pair.Value.Get();
			break;
		}
	}
	MSPHoveredHomeButton = CurrentHovered;
	MSPPressedHomeButton.Reset();
	RefreshMSPHomeButtonVisualStates(TEXT("MouseLeave"));
}

void AMenuHUD::HandleMSPHomeButtonPressed()
{
	for (const TPair<FName, TWeakObjectPtr<UButton>>& Pair : MSPHomeButtons)
	{
		if (Pair.Value.IsValid() && Pair.Value->IsPressed())
		{
			MSPPressedHomeButton = Pair.Value;
			break;
		}
	}
	RefreshMSPHomeButtonVisualStates(TEXT("MouseDown"));
}

void AMenuHUD::HandleMSPHomeButtonReleased()
{
	MSPPressedHomeButton.Reset();
	RefreshMSPHomeButtonVisualStates(TEXT("MouseUp"));
}

void AMenuHUD::RefreshMSPHomeFocus()
{
	if (ActiveMSPOverlayKind != EMSPOverlay::None)
	{
		return;
	}

	const TArray<FName>& Order = MSPHomeButtonOrder();
	for (int32 Index = 0; Index < Order.Num(); ++Index)
	{
		if (const TWeakObjectPtr<UButton>* Candidate = MSPHomeButtons.Find(Order[Index]);
			Candidate && Candidate->IsValid() && Candidate->Get()->HasAnyUserFocus())
		{
			MSPHomeFocusIndex = Index;
			return;
		}
	}

	if (!PendingMSPHomeFocusOwner.IsNone())
	{
		if (const TWeakObjectPtr<UButton>* Pending = MSPHomeButtons.Find(PendingMSPHomeFocusOwner);
			Pending && Pending->IsValid() && Pending->Get()->IsVisible() && Pending->Get()->GetIsEnabled())
		{
			const int32 Index = Order.IndexOfByKey(PendingMSPHomeFocusOwner);
			MSPHomeFocusIndex = Index;
			const FName RestoredOwner = PendingMSPHomeFocusOwner;
			PendingMSPHomeFocusOwner = NAME_None;
			FocusMSPButton(Pending->Get());
			UE_LOG(LogTemp, Verbose, TEXT("[AvMainMenu] Restored MSP focus to %s."), *RestoredOwner.ToString());
			return;
		}
	}

	if (!bLoggedMSPHomeInitialNoSelection)
	{
		bLoggedMSPHomeInitialNoSelection = true;
		MSPHomeFocusIndex = INDEX_NONE;
		MSPHoveredHomeButton.Reset();
		MSPPressedHomeButton.Reset();
		RefreshMSPHomeButtonVisualStates(TEXT("InitialNoSelection"));
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenuState] Initial state resolved with no selected or focused home item."));
	}
}

void AMenuHUD::FocusMSPButton(UButton* Button)
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC || !Button || !Button->IsVisible() || !Button->GetIsEnabled())
	{
		return;
	}
	if (bMSPSettingsOpen && bMSPMouseInputActive)
	{
		return;
	}
	++MSPSettingsDiagnosticFocusCalls;
	if (bMSPSettingsInteractionDiagnostics)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsInteractionDiag] SetKeyboardFocus call=%d target=%s identity=%p hovered=%s settingsOpen=%s"),
			MSPSettingsDiagnosticFocusCalls,
			*Button->GetPathName(),
			Button,
			Button->IsHovered() ? TEXT("true") : TEXT("false"),
			bMSPSettingsOpen ? TEXT("true") : TEXT("false"));
	}

	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	Mode.SetWidgetToFocus(Button->TakeWidget());
	PC->SetInputMode(Mode);
	PC->bShowMouseCursor = true;
	Button->SetKeyboardFocus();
	for (const TPair<FName, TWeakObjectPtr<UButton>>& Pair : MSPHomeButtons)
	{
		if (Pair.Value.Get() == Button)
		{
			MSPHoveredHomeButton.Reset();
			MSPPressedHomeButton.Reset();
			RefreshMSPHomeButtonVisualStates(TEXT("KeyboardFocus"));
			break;
		}
	}
}

void AMenuHUD::HandleMSPDiagnosticHovered()
{
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] HoverEnter %s"),
		*DescribeHoveredMSPSettingsControl());
}

void AMenuHUD::HandleMSPDiagnosticUnhovered()
{
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] HoverLeave previous=%s"),
		*LastMSPSettingsDiagnosticFocus);
}

void AMenuHUD::HandleMSPDiagnosticPressed()
{
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] MouseDown %s"),
		*DescribeHoveredMSPSettingsControl());
}

void AMenuHUD::HandleMSPDiagnosticReleased()
{
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] MouseUp %s"),
		*DescribeHoveredMSPSettingsControl());
}

void AMenuHUD::HandleMSPDiagnosticClicked()
{
	int32 BoundObjectCount = 0;
	for (const TWeakObjectPtr<UButton>& Button : DiagnosticMSPButtons)
	{
		if (!Button.IsValid() || !Button->IsHovered())
		{
			continue;
		}
		const TArray<UObject*> BoundObjects = Button->OnClicked.GetAllObjects();
		BoundObjectCount = BoundObjects.Num();
		break;
	}
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] Click %s boundObjects=%d"),
		*DescribeHoveredMSPSettingsControl(),
		BoundObjectCount);
}

void AMenuHUD::HandleMSPDiagnosticSliderChanged(const float Value)
{
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] SliderValueChanged value=%.3f %s"),
		Value,
		*DescribeHoveredMSPSettingsControl());
}

void AMenuHUD::HandleMSPDiagnosticSliderCaptureBegin()
{
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] SliderCaptureBegin %s"),
		*DescribeHoveredMSPSettingsControl());
}

void AMenuHUD::HandleMSPDiagnosticSliderCaptureEnd()
{
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsInteractionDiag] SliderCaptureEnd %s"),
		*DescribeHoveredMSPSettingsControl());
}

void AMenuHUD::MoveMSPHomeFocus(const int32 Direction)
{
	const TArray<FName>& Order = MSPHomeButtonOrder();
	if (Order.IsEmpty())
	{
		return;
	}

	int32 Index = MSPHomeFocusIndex;
	if (!Order.IsValidIndex(Index))
	{
		Index = Direction > 0 ? -1 : 0;
	}
	for (int32 Attempt = 0; Attempt < Order.Num(); ++Attempt)
	{
		Index = (Index + Direction + Order.Num()) % Order.Num();
		if (const TWeakObjectPtr<UButton>* Candidate = MSPHomeButtons.Find(Order[Index]);
			Candidate && Candidate->IsValid() && Candidate->Get()->IsVisible() &&
			Candidate->Get()->GetIsEnabled())
		{
			MSPHomeFocusIndex = Index;
			FocusMSPButton(Candidate->Get());
			return;
		}
	}
}

void AMenuHUD::ActivateFocusedMSPHomeButton()
{
	const TArray<FName>& Order = MSPHomeButtonOrder();
	if (!Order.IsValidIndex(MSPHomeFocusIndex))
	{
		RefreshMSPHomeFocus();
	}
	if (Order.IsValidIndex(MSPHomeFocusIndex))
	{
		if (const TWeakObjectPtr<UButton>* Candidate = MSPHomeButtons.Find(Order[MSPHomeFocusIndex]);
			Candidate && Candidate->IsValid() && Candidate->Get()->IsVisible() &&
			Candidate->Get()->GetIsEnabled())
		{
			Candidate->Get()->OnClicked.Broadcast();
		}
	}
}

void AMenuHUD::FocusFirstVisibleMSPSettingsButton()
{
	if (bMSPLanguageOpen)
	{
		for (TObjectIterator<UUserWidget> It; It; ++It)
		{
			UUserWidget* Widget = *It;
			if (IsValid(Widget) && Widget->GetWorld() == GetWorld() &&
				IsInsideMSPLanguageMenu(Widget) &&
				Widget->GetName().Contains(TEXT("OutputDeviceOptionsPicker")))
			{
				if (UButton* Button = FindFirstButton(Widget);
					Button && Button->IsVisible() && Button->GetIsEnabled())
				{
					FocusMSPButton(Button);
					return;
				}
			}
		}
	}

	static const TArray<FString> PreferredOwners = {
		TEXT("OutputDeviceOptionsPicker"),
		TEXT("GameplayLargeTabButton"),
		TEXT("AudioLargeTabButton"),
		TEXT("VideoLargeTabButton"),
		TEXT("ControlsLargeTabButton"),
	};
	for (const FString& PreferredOwner : PreferredOwners)
	{
		for (TObjectIterator<UUserWidget> It; It; ++It)
		{
			UUserWidget* Widget = *It;
			if (IsValid(Widget) && Widget->GetWorld() == GetWorld() &&
				Widget->GetName().Contains(PreferredOwner))
			{
				if (UButton* Button = FindFirstButton(Widget);
					Button && Button->IsVisible() && Button->GetIsEnabled())
				{
					FocusMSPButton(Button);
					return;
				}
			}
		}
	}
}

bool AMenuHUD::HandleAuthenticMSPInput(const FKey& Key, const EInputEvent Event, const float AnalogValue)
{
	if (!bAuthenticMSPMap || Event == IE_Released)
	{
		return false;
	}

	const bool bConfirm = Key == EKeys::Enter || Key == EKeys::SpaceBar ||
		Key == EKeys::Gamepad_FaceButton_Bottom;
	const bool bBack = Key == EKeys::Escape || Key == EKeys::BackSpace ||
		Key == EKeys::Gamepad_FaceButton_Right;
	const bool bLeft = Key == EKeys::Left || Key == EKeys::A || Key == EKeys::Gamepad_DPad_Left;
	const bool bRight = Key == EKeys::Right || Key == EKeys::D || Key == EKeys::Gamepad_DPad_Right;
	const bool bUp = Key == EKeys::Up || Key == EKeys::W || Key == EKeys::Gamepad_DPad_Up;
	const bool bDown = Key == EKeys::Down || Key == EKeys::S || Key == EKeys::Gamepad_DPad_Down;
	if (!bMSPSettingsOpen && Event == IE_Axis &&
		(Key == EKeys::MouseX || Key == EKeys::MouseY) &&
		FMath::Abs(AnalogValue) > KINDA_SMALL_NUMBER)
	{
		for (const TPair<FName, TWeakObjectPtr<UButton>>& Pair : MSPHomeButtons)
		{
			if (Pair.Value.IsValid() && Pair.Value->IsHovered())
			{
				if (!bMSPMouseInputActive ||
					MSPHoveredHomeButton.Get() != Pair.Value.Get())
				{
					HandleMSPHomeButtonHovered();
				}
				break;
			}
		}
	}
	if (bMSPSettingsOpen && Event == IE_Pressed &&
		!Key.IsMouseButton() && !Key.IsGamepadKey())
	{
		bMSPMouseInputActive = false;
	}
	else if (!bMSPSettingsOpen && Event == IE_Pressed &&
		!Key.IsMouseButton())
	{
		bMSPMouseInputActive = false;
	}

	if (ActiveMSPOverlayKind != EMSPOverlay::None)
	{
		if (ActiveMSPOverlayKind == EMSPOverlay::Credits && !bActiveMSPOverlayOwnedByRouter)
		{
			// Authored credits own their any-key/Back transition and animation.
			return false;
		}
		if (bBack)
		{
			if (ActiveMSPOverlayKind == EMSPOverlay::UnsavedConfirm)
			{
				CloseMSPOverlay(NAME_None);
				return true;
			}
			HandleMSPOverlayNoClicked();
			return true;
		}
		if ((ActiveMSPOverlayKind == EMSPOverlay::ExitConfirm ||
			ActiveMSPOverlayKind == EMSPOverlay::DisplayConfirm ||
			ActiveMSPOverlayKind == EMSPOverlay::ResetConfirm ||
			ActiveMSPOverlayKind == EMSPOverlay::UnsavedConfirm) &&
			(bLeft || bRight))
		{
			bMSPOverlayYesFocused = !bMSPOverlayYesFocused;
			FocusMSPButton(bMSPOverlayYesFocused ? MSPOverlayYesButton.Get() : MSPOverlayNoButton.Get());
			return true;
		}
		if (bConfirm)
		{
			if (bMSPOverlayYesFocused && MSPOverlayYesButton.IsValid())
			{
				HandleMSPOverlayYesClicked();
			}
			else
			{
				HandleMSPOverlayNoClicked();
			}
			return true;
		}
		return bUp || bDown || bLeft || bRight ||
			(Key == EKeys::Gamepad_LeftY && Event == IE_Axis);
	}

	// Once MSP owns a Settings screen, its authored navigation handles arrows,
	// Tab, D-pad, stick, dropdowns and sliders. The project router only owns
	// Back so unsaved changes cannot be discarded silently.
	if (bMSPSettingsOpen)
	{
		if (bBack)
		{
			InvokeMSPSettingsBack();
			return true;
		}
		return false;
	}

	bool bAnalogUp = false;
	bool bAnalogDown = false;
	if (Key == EKeys::Gamepad_LeftY && Event == IE_Axis && FMath::Abs(AnalogValue) >= 0.55f)
	{
		const double Now = FPlatformTime::Seconds();
		if (LastMSPAnalogNavigationTime < 0.0 || Now - LastMSPAnalogNavigationTime >= 0.22)
		{
			LastMSPAnalogNavigationTime = Now;
			bAnalogUp = AnalogValue > 0.0f;
			bAnalogDown = AnalogValue < 0.0f;
		}
	}
	else if (Key == EKeys::Gamepad_LeftY && Event == IE_Axis)
	{
		LastMSPAnalogNavigationTime = -1.0;
		return true;
	}

	if (bUp || bAnalogUp)
	{
		MoveMSPHomeFocus(-1);
		return true;
	}
	if (bDown || bAnalogDown)
	{
		MoveMSPHomeFocus(1);
		return true;
	}
	if (bConfirm)
	{
		ActivateFocusedMSPHomeButton();
		return true;
	}
	return false;
}

void AMenuHUD::HandleMSPContinueClicked()
{
	if (SavePresenceAdapter && SavePresenceAdapter->Refresh())
	{
		UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] Continue validated the active company save and is opening gameplay."));
		TravelToGameplay(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AvMainMenu] Continue rejected because no valid company save exists."));
	}
}

void AMenuHUD::HandleMSPNewGameClicked()
{
	UE_LOG(LogTemp, Log,
		TEXT("[AvMainMenu] New Game is opening the existing gameplay/customization flow without deleting SaveGame."));
	TravelToGameplay(true);
}

void AMenuHUD::HandleMSPMultiplayerClicked()
{
	PendingMSPHomeFocusOwner = TEXT("MultiplayerTitleScreenButton");
	OpenMSPComingSoon();
}

void AMenuHUD::EnsureStableMSPSettingsRuntime()
{
	if (bStableMSPSettingsWarmupStarted || !AuthenticMSPRoot.IsValid() ||
		!AuthenticMSPRoot->WidgetTree)
	{
		return;
	}

	UBorder* MainLayer = Cast<UBorder>(
		AuthenticMSPRoot->GetWidgetFromName(TEXT("Main")));
	UBorder* SubMenuLayer = Cast<UBorder>(
		AuthenticMSPRoot->GetWidgetFromName(TEXT("SubMenu")));
	UUserWidget* Home = MainLayer ? Cast<UUserWidget>(MainLayer->GetContent()) : nullptr;
	if (!MainLayer || !SubMenuLayer || !Home ||
		!Home->GetClass()->GetPathName().Contains(TEXT("WBP_AvMSP_Home_C")))
	{
		return;
	}

	UClass* SettingsClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/Avariika/UI/MainMenu/MSP/WBP_AvMSP_Settings.WBP_AvMSP_Settings_C"));
	APlayerController* PC = GetOwningPlayerController();
	if (!SettingsClass || !PC)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] Stable Settings warmup could not load its production root."));
		return;
	}

	MSPHomeMenuWidget = Home;
	MSPMainSwitcher = AuthenticMSPRoot->WidgetTree->ConstructWidget<UWidgetSwitcher>(
		UWidgetSwitcher::StaticClass(), TEXT("AvStableSettingsMainSwitcher"));
	if (!MSPMainSwitcher)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] Could not construct the stable Main WidgetSwitcher."));
		return;
	}

	Home->RemoveFromParent();
	MSPMainSwitcher->AddChild(Home);
	MainLayer->SetContent(MSPMainSwitcher);
	Home->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	StableMSPSettingsRoot = CreateWidget<UUserWidget>(
		PC, SettingsClass, TEXT("AvStableSettingsRoot"));
	if (!StableMSPSettingsRoot)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] Could not construct the stable Settings root."));
		return;
	}
	StableMSPSettingsRoot->SetVisibility(ESlateVisibility::Collapsed);
	MSPMainSwitcher->AddChild(StableMSPSettingsRoot);
	MSPMainSwitcher->SetActiveWidgetIndex(0);

	// The four authored leaf screens are warmed behind a collapsed layer. This
	// lets their Construct/transition work finish and lets the project bridge
	// initialize their final state before any of them can be rendered.
	SubMenuLayer->SetVisibility(ESlateVisibility::Collapsed);
	bStableMSPSettingsWarmupStarted = true;
	MSPSettingsWarmupCategory = 0;
	MSPSettingsWarmupStableFrames = 0;
	MSPSettingsWarmupAttempts = 0;
	++MSPSettingsWarmupGeneration;
	const int32 Generation = MSPSettingsWarmupGeneration;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this, Generation]
			{
				PollStableMSPSettingsWarmup(Generation);
			}));
	}
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsPageSwap] Warmup started. root=%s identity=%p mainSwitcher=%p"),
		*StableMSPSettingsRoot->GetPathName(), StableMSPSettingsRoot.Get(),
		MSPMainSwitcher.Get());
}

void AMenuHUD::PollStableMSPSettingsWarmup(const int32 Generation)
{
	if (Generation != MSPSettingsWarmupGeneration)
	{
		++MSPSettingsStaleCallbacks;
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] StaleTransitionCallback generation=%d current=%d"),
			Generation, MSPSettingsWarmupGeneration);
		return;
	}
	if (!bStableMSPSettingsWarmupStarted || bStableMSPSettingsReady ||
		!StableMSPSettingsRoot || !AuthenticMSPRoot.IsValid())
	{
		return;
	}
	if (++MSPSettingsWarmupAttempts > 600)
	{
		bStableMSPSettingsWarmupStarted = false;
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] Bounded warmup failed after %d game frames; no retry loop remains active."),
			MSPSettingsWarmupAttempts);
		return;
	}

	static const TCHAR* PageClassFragments[] = {
		TEXT("WBP_DisplaySettings_Silence_C"),
		TEXT("WBP_AvMSP_Graphics_C"),
		TEXT("WBP_AudioSettings_Silence_C"),
		TEXT("WBP_AvMSP_Language_C")
	};
	UUserWidget* ActivePage = GetActiveMSPSubMenu();
	const bool bExpectedPage =
		ActivePage && ActivePage->GetClass()->GetPathName().Contains(
			PageClassFragments[MSPSettingsWarmupCategory]);
	if (bExpectedPage)
	{
		// Eight stable game frames cover the authored 0.1 s delayed callbacks.
		// Any target replacement resets the counter instead of racing it.
		++MSPSettingsWarmupStableFrames;
		for (TObjectIterator<UUserWidget> It; It; ++It)
		{
			UUserWidget* Widget = *It;
			if (!IsValid(Widget) || Widget->GetWorld() != GetWorld())
			{
				continue;
			}
			const FString ClassPath = Widget->GetClass()->GetPathName();
			if (ClassPath.Contains(TEXT("/Game/MenuSystemPro/")) ||
				ClassPath.Contains(TEXT("/Game/Avariika/UI/MainMenu/MSP/")))
			{
				InitializeAuthenticMSPWidgetNow(Widget);
			}
		}
		ActivePage->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		MSPSettingsWarmupStableFrames = 0;
	}

	if (bExpectedPage && MSPSettingsWarmupStableFrames >= 8)
	{
		switch (MSPSettingsWarmupCategory)
		{
		case 0: StableMSPScreenPage = ActivePage; break;
		case 1: StableMSPGraphicsPage = ActivePage; break;
		case 2: StableMSPAudioPage = ActivePage; break;
		case 3: StableMSPLanguagePage = ActivePage; break;
		default: break;
		}
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsPageSwap] Production page cached category=%d object=%s class=%s identity=%p visibility=Collapsed"),
			MSPSettingsWarmupCategory, *ActivePage->GetPathName(),
			*ActivePage->GetClass()->GetPathName(), ActivePage);

		if (MSPSettingsWarmupCategory < 3)
		{
			++MSPSettingsWarmupCategory;
			MSPSettingsWarmupStableFrames = 0;
			static const TCHAR* TargetPaths[] = {
				TEXT(""),
				TEXT("/Game/Avariika/UI/MainMenu/MSP/WBP_AvMSP_Graphics.WBP_AvMSP_Graphics_C"),
				TEXT("/Game/MenuSystemPro/ExampleContent/Designs/Design_Silence/Menus/Settings/WBP_AudioSettings_Silence.WBP_AudioSettings_Silence_C"),
				TEXT("/Game/Avariika/UI/MainMenu/MSP/WBP_AvMSP_Language.WBP_AvMSP_Language_C")
			};
			if (!TransitionMSPMenu(StableMSPSettingsRoot,
				TargetPaths[MSPSettingsWarmupCategory], TEXT("SubMenu")))
			{
				UE_LOG(LogTemp, Error,
					TEXT("[AvSettingsPageSwap] Warmup transition failed category=%d."),
					MSPSettingsWarmupCategory);
				return;
			}
			UE_LOG(LogTemp, Log,
				TEXT("[AvSettingsPageSwap] SetActive authored warmup destination category=%d generation=%d"),
				MSPSettingsWarmupCategory, Generation);
		}
		else
		{
			FinalizeStableMSPSettingsRuntime();
			return;
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this, Generation]
			{
				PollStableMSPSettingsWarmup(Generation);
			}));
	}
}

void AMenuHUD::FinalizeStableMSPSettingsRuntime()
{
	if (!AuthenticMSPRoot.IsValid() || !AuthenticMSPRoot->WidgetTree ||
		!StableMSPScreenPage || !StableMSPGraphicsPage ||
		!StableMSPAudioPage || !StableMSPLanguagePage)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] Cannot finalize: one or more production destinations are missing."));
		return;
	}
	UBorder* SubMenuLayer = Cast<UBorder>(
		AuthenticMSPRoot->GetWidgetFromName(TEXT("SubMenu")));
	if (!SubMenuLayer)
	{
		return;
	}

	MSPSettingsPageSwitcher =
		AuthenticMSPRoot->WidgetTree->ConstructWidget<UWidgetSwitcher>(
			UWidgetSwitcher::StaticClass(), TEXT("AvStableSettingsPageSwitcher"));
	if (!MSPSettingsPageSwitcher)
	{
		return;
	}
	UUserWidget* Pages[] = {
		StableMSPScreenPage,
		StableMSPGraphicsPage,
		StableMSPAudioPage,
		StableMSPLanguagePage
	};
	if (UWidget* ExistingContent = SubMenuLayer->GetContent())
	{
		ExistingContent->RemoveFromParent();
	}
	for (UUserWidget* Page : Pages)
	{
		Page->RemoveFromParent();
		Page->SetVisibility(ESlateVisibility::Collapsed);
		MSPSettingsPageSwitcher->AddChild(Page);
	}
	SubMenuLayer->SetContent(MSPSettingsPageSwitcher);
	MSPSettingsPageSwitcher->SetActiveWidgetIndex(0);
	SubMenuLayer->SetVisibility(ESlateVisibility::Collapsed);
	MSPSettingsActiveCategory = 0;

	// Adding the four already-created pages to the stable switcher rebuilds
	// their Slate hierarchy. MSP runs its authored PreConstruct style pass
	// during that rebuild and restores TextFontStyle from StyleData. Let that
	// one rebuild finish before applying the project role policy.
	const int32 Generation = MSPSettingsWarmupGeneration;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this, Generation]
			{
				CompleteStableMSPSettingsRuntimeAfterReparent(Generation);
			}));
	}
}

void AMenuHUD::CompleteStableMSPSettingsRuntimeAfterReparent(
	int32 Generation)
{
	if (Generation != MSPSettingsWarmupGeneration ||
		!AuthenticMSPRoot.IsValid() || !MSPSettingsPageSwitcher ||
		!StableMSPScreenPage || !StableMSPGraphicsPage ||
		!StableMSPAudioPage || !StableMSPLanguagePage)
	{
		++MSPSettingsStaleCallbacks;
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] Reparent completion rejected generation=%d currentGeneration=%d."),
			Generation, MSPSettingsWarmupGeneration);
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsFontDiag] StableHierarchyReady generation=%d rowFontPassDeferredUntilFirstReveal=true"),
		Generation);

	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Widget = *It;
		if (IsValid(Widget) && Widget->GetWorld() == GetWorld())
		{
			BindStableMSPSettingsTabs(Widget);
			const FString ClassPath = Widget->GetClass()->GetPathName();
			if (ClassPath.Contains(TEXT("/Game/MenuSystemPro/")) ||
				ClassPath.Contains(TEXT("/Game/Avariika/UI/MainMenu/MSP/")))
			{
				ApplyAuthenticMSPStyle(Widget);
				ApplyAuthenticMSPFontPolicy(Widget);
			}
		}
	}

	// Reparenting invokes the vendor PreConstruct one final time. Re-initialize
	// every production row after that point so the adapter is the sole data and
	// style writer before the first visible frame. This is especially important
	// for Language, whose authored control is based on the vendor output-device
	// selector and can otherwise retain a hardware name.
	TArray<UUserWidget*> StableRows;
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		if (UUserWidget* RowWidget = Pair.Key.Get();
			IsValid(RowWidget) && RowWidget->GetWorld() == GetWorld())
		{
			StableRows.Add(RowWidget);
		}
	}
	for (UUserWidget* RowWidget : StableRows)
	{
		InitializedMSPSettingWidgets.Remove(RowWidget);
		InitializeAuthenticMSPSettingWidget(RowWidget);
		BindAuthenticMSPSettingInput(RowWidget);
		if (RowWidget->WidgetTree)
		{
			TArray<UWidget*> RowChildren;
			RowWidget->WidgetTree->GetAllWidgets(RowChildren);
			for (UWidget* Child : RowChildren)
			{
				if (UButton* Button = Cast<UButton>(Child))
				{
					BoundMSPMouseModalityButtons.Remove(Button);
					DiagnosticMSPButtons.Remove(Button);
				}
				else if (USlider* Slider = Cast<USlider>(Child))
				{
					DiagnosticMSPSliders.Remove(Slider);
				}
			}
		}
		BindMSPMouseModality(RowWidget);
		BindMSPSettingsInteractionDiagnostics(RowWidget);
		ApplyAuthenticMSPSupportPolicy(RowWidget);
		ApplyAuthenticMSPStyle(RowWidget);
		ApplyAuthenticMSPFontPolicy(RowWidget);
	}

	// The vendor Display screen binds AspectRatio.OnOptionChanged to
	// SetupDisplayResolutionMenu. The project adapter already owns the
	// authoritative filtered resolution list and updates the resolution row in
	// CaptureAuthenticMSPSettings. Keeping both handlers makes every adapter
	// sync run the vendor setup again; that setup calls
	// OverrideWidgetStyleFromConfig and restores Kanit on the combo row label.
	// Remove only the vendor screen destination, preserving the row's own
	// handlers and the project input binding.
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		UUserWidget* AspectRatioWidget = Pair.Key.Get();
		if (!IsValid(AspectRatioWidget) ||
			static_cast<EAvMainMenuSettingRow>(Pair.Value) !=
				EAvMainMenuSettingRow::AspectRatio)
		{
			continue;
		}
		FMulticastDelegateProperty* OptionChangedProperty =
			FindFProperty<FMulticastDelegateProperty>(
				AspectRatioWidget->GetClass(), TEXT("OnOptionChanged"));
		FMulticastScriptDelegate* OptionChangedDelegate = OptionChangedProperty ?
			OptionChangedProperty->ContainerPtrToValuePtr<FMulticastScriptDelegate>(
				AspectRatioWidget) : nullptr;
		if (!OptionChangedDelegate)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvSettingsFontDiag] AspectRatio row has no OnOptionChanged delegate: %s"),
				*AspectRatioWidget->GetPathName());
			continue;
		}
		int32 VendorHandlersBefore = 0;
		for (UObject* BoundObject : OptionChangedDelegate->GetAllObjects())
		{
			VendorHandlersBefore += BoundObject == StableMSPScreenPage ? 1 : 0;
		}
		OptionChangedDelegate->RemoveAll(StableMSPScreenPage);
		int32 VendorHandlersAfter = 0;
		for (UObject* BoundObject : OptionChangedDelegate->GetAllObjects())
		{
			VendorHandlersAfter += BoundObject == StableMSPScreenPage ? 1 : 0;
		}
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsFontDiag] AspectRatioVendorResolutionRefresh disconnected row=%s vendorHandlersBefore=%d vendorHandlersAfter=%d"),
			*AspectRatioWidget->GetPathName(),
			VendorHandlersBefore,
			VendorHandlersAfter);
	}
	bStableMSPSettingsReady = true;
	bStableMSPSettingsWarmupStarted = false;
	bStableMSPInitialRowFontPassComplete = true;
	RefreshStableMSPSettingsVisualState(0);
	SetMSPSettingsFooterVisible(false);
	BindStableMSPSettingsFooter(TEXT("AfterFinalVendorPreConstruct"));
	ApplyFinalMSPHomeStyle(TEXT("AfterFinalVendorPreConstruct"));
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AuthenticMSPBridgeTimer);
	}
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsPageSwap] Stable runtime ready. SettingsRootInstances=1 ScreenPages=1 GraphicsPages=1 AudioPages=1 LanguagePages=1 RowsFinalized=%d DuplicateBindings=%d StaleTransitionCallbacks=%d BridgePasses=%d"),
		StableRows.Num(), MSPSettingsDuplicateBindings,
		MSPSettingsStaleCallbacks, MSPSettingsDiagnosticBridgePasses);
	ValidateStableMSPSettingsInvariant();
	if (bOpenStableMSPSettingsWhenReady)
	{
		bOpenStableMSPSettingsWhenReady = false;
		OpenStableMSPSettings();
	}
}

void AMenuHUD::ApplyStableMSPSettingsRowFontsAfterFirstReveal()
{
	if (bStableMSPInitialRowFontPassComplete)
	{
		return;
	}

	int32 RowFontPasses = 0;
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair :
		MSPSettingRows)
	{
		UUserWidget* RowWidget = Pair.Key.Get();
		if (!IsValid(RowWidget) || RowWidget->GetWorld() != GetWorld())
		{
			continue;
		}
		// Finish MSP's authored normal state first. ComboBox carries an internal
		// FFontStyle copy that otherwise becomes the last writer and restores
		// Kanit after the project role assignment.
		InvokeMSPNoParameterFunction(
			RowWidget, TEXT("ApplyWidgetStyleNormal"));
		// Update both the backing FFontStyle and the visible TextBlock last.
		// Future hover/focus states now read the corrected backing style.
		ApplyAuthenticMSPStyle(RowWidget);
		ApplyAuthenticMSPFontPolicy(RowWidget);
		UTextBlock* RowLabel = Cast<UTextBlock>(
			RowWidget->GetWidgetFromName(TEXT("OptionNameText")));
		if (RowLabel)
		{
			RowLabel->InvalidateLayoutAndVolatility();
		}
		RowWidget->InvalidateLayoutAndVolatility();
		++RowFontPasses;
	}
	if (StableMSPSettingsRoot)
	{
		StableMSPSettingsRoot->InvalidateLayoutAndVolatility();
		StableMSPSettingsRoot->ForceLayoutPrepass();
	}
	bStableMSPInitialRowFontPassComplete = true;
	if (bMSPSettingsInteractionDiagnostics)
	{
		LogStableMSPResolutionFontIdentity(
			TEXT("ApplyStableMSPSettingsRowFontsAfterFirstReveal"));
		if (GetWorld())
		{
			TWeakObjectPtr<AMenuHUD> WeakThis(this);
			FTimerHandle ResolutionFontAuditTimer;
			GetWorld()->GetTimerManager().SetTimer(
				ResolutionFontAuditTimer,
				[WeakThis]()
				{
					if (AMenuHUD* HUD = WeakThis.Get())
					{
						HUD->LogStableMSPResolutionFontIdentity(
							TEXT("FirstRevealPlus500ms"));
					}
				},
				0.5f,
				false);
		}
	}
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsFontDiag] FirstRevealRowFontPass rows=%d source=ApplyStableMSPSettingsRowFontsAfterFirstReveal"),
		RowFontPasses);
}

void AMenuHUD::LogStableMSPResolutionFontIdentity(const TCHAR* Source)
{
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		UUserWidget* RowWidget = Pair.Key.Get();
		if (!IsValid(RowWidget) ||
			!RowWidget->GetName().Contains(TEXT("DisplayResolutionsComboBox")))
		{
			continue;
		}

		UTextBlock* NamedLabel = Cast<UTextBlock>(
			RowWidget->GetWidgetFromName(TEXT("OptionNameText")));
		UTextBlock* AuthoredLabel = Cast<UTextBlock>(
			ReadMSPObjectProperty(RowWidget, TEXT("OptionNameTextRef")));
		UComboBoxString* ComboBox = Cast<UComboBoxString>(
			RowWidget->GetWidgetFromName(TEXT("ComboBoxString")));
		const auto GeometryDescription = [](const UWidget* Widget)
		{
			if (!Widget)
			{
				return FString(TEXT("<none>"));
			}
			const FGeometry& Geometry = Widget->GetCachedGeometry();
			const FVector2D Position = Geometry.GetAbsolutePosition();
			const FVector2D Size = Geometry.GetLocalSize();
			return FString::Printf(
				TEXT("pos=(%.1f,%.1f) size=(%.1f,%.1f)"),
				Position.X, Position.Y, Size.X, Size.Y);
		};
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsFontIdentity] source=%s row=%s named=%p authoredRef=%p same=%s namedPath=%s authoredPath=%s namedText=\"%s\" namedFont={%s} authoredFont={%s} namedGeometry={%s} authoredGeometry={%s} combo=%p comboPath=%s selected=\"%s\" comboFont={%s} comboGeometry={%s} customGenerator=%s"),
			Source,
			*RowWidget->GetPathName(),
			NamedLabel,
			AuthoredLabel,
			NamedLabel == AuthoredLabel ? TEXT("true") : TEXT("false"),
			NamedLabel ? *NamedLabel->GetPathName() : TEXT("<none>"),
			AuthoredLabel ? *AuthoredLabel->GetPathName() : TEXT("<none>"),
			NamedLabel ? *NamedLabel->GetText().ToString() : TEXT("<none>"),
			NamedLabel ? *DescribeMSPFont(NamedLabel->GetFont()) : TEXT("<none>"),
			AuthoredLabel ? *DescribeMSPFont(AuthoredLabel->GetFont()) : TEXT("<none>"),
			*GeometryDescription(NamedLabel),
			*GeometryDescription(AuthoredLabel),
			ComboBox,
			ComboBox ? *ComboBox->GetPathName() : TEXT("<none>"),
			ComboBox ? *ComboBox->GetSelectedOption() : TEXT("<none>"),
			ComboBox ? *DescribeMSPFont(ComboBox->GetFont()) : TEXT("<none>"),
			*GeometryDescription(ComboBox),
			ComboBox && ComboBox->OnGenerateWidgetEvent.IsBound() ?
				TEXT("true") : TEXT("false"));
		return;
	}
	UE_LOG(LogTemp, Warning,
		TEXT("[AvSettingsFontIdentity] source=%s resolutionRow=<not-found>"),
		Source);
}

void AMenuHUD::BindStableMSPSettingsTabs(UUserWidget* Widget)
{
	if (!Widget || !StableMSPSettingsRoot)
	{
		return;
	}
	const FString WidgetName = Widget->GetName();
	UFunction* HandlerFunction = nullptr;
	if (WidgetName == TEXT("GameplayLargeTabButton"))
	{
		HandlerFunction = FindFunction(
			GET_FUNCTION_NAME_CHECKED(AMenuHUD, HandleMSPScreenTabClicked));
	}
	else if (WidgetName == TEXT("AudioLargeTabButton"))
	{
		HandlerFunction = FindFunction(
			GET_FUNCTION_NAME_CHECKED(AMenuHUD, HandleMSPGraphicsTabClicked));
	}
	else if (WidgetName == TEXT("VideoLargeTabButton"))
	{
		HandlerFunction = FindFunction(
			GET_FUNCTION_NAME_CHECKED(AMenuHUD, HandleMSPAudioTabClicked));
	}
	else if (WidgetName == TEXT("ControlsLargeTabButton"))
	{
		HandlerFunction = FindFunction(
			GET_FUNCTION_NAME_CHECKED(AMenuHUD, HandleMSPLanguageClicked));
	}
	if (!HandlerFunction || BoundStableMSPSettingsTabWidgets.Contains(Widget))
	{
		return;
	}

	FMulticastDelegateProperty* ClickedProperty =
		FindFProperty<FMulticastDelegateProperty>(
			Widget->GetClass(), TEXT("OnWidgetClicked"));
	FMulticastScriptDelegate* ClickedDelegate = ClickedProperty ?
		ClickedProperty->ContainerPtrToValuePtr<FMulticastScriptDelegate>(Widget) : nullptr;
	if (!ClickedDelegate)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] Tab has no OnWidgetClicked delegate: %s"),
			*Widget->GetPathName());
		return;
	}

	// Remove the authored Settings destination only. The tab's internal click
	// handler remains and is allowed to update visuals/sound, never pages.
	ClickedDelegate->RemoveAll(StableMSPSettingsRoot);
	FScriptDelegate Handler;
	Handler.BindUFunction(this, HandlerFunction->GetFName());
	ClickedDelegate->Remove(Handler);
	ClickedDelegate->AddUnique(Handler);

	const TArray<UObject*> BoundObjects = ClickedDelegate->GetAllObjects();
	int32 HUDHandlerCount = 0;
	for (UObject* Object : BoundObjects)
	{
		HUDHandlerCount += Object == this ? 1 : 0;
	}
	if (HUDHandlerCount != 1)
	{
		++MSPSettingsDuplicateBindings;
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwap] DuplicateBinding tab=%s hudHandlers=%d totalObjects=%d"),
			*Widget->GetPathName(), HUDHandlerCount, BoundObjects.Num());
	}
	BoundStableMSPSettingsTabWidgets.Add(Widget);
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsPageSwap] TabBound tab=%s handler=%s hudHandlers=%d totalObjects=%d"),
		*Widget->GetPathName(), *HandlerFunction->GetName(),
		HUDHandlerCount, BoundObjects.Num());
}

void AMenuHUD::BindStableMSPSettingsFooter(const TCHAR* Source)
{
	if (!GetWorld())
	{
		return;
	}

	TArray<UUserWidget*> Footers;
	UUserWidget* SelectedFooter = nullptr;
	int32 SelectedButtonCount = -1;
	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Candidate = *It;
		if (!IsValid(Candidate) || Candidate->GetWorld() != GetWorld() ||
			!Candidate->GetClass()->GetPathName().Contains(
				TEXT("WBP_MenuFooterBar_Silence_C")))
		{
			continue;
		}
		Footers.Add(Candidate);
		const UHorizontalBox* Buttons = Cast<UHorizontalBox>(
			Candidate->GetWidgetFromName(TEXT("ButtonsHBox")));
		const int32 ButtonCount = Buttons ? Buttons->GetChildrenCount() : 0;
		const bool bPrefer = !SelectedFooter ||
			(Candidate->IsVisible() && !SelectedFooter->IsVisible()) ||
			(Candidate->IsVisible() == SelectedFooter->IsVisible() &&
				ButtonCount > SelectedButtonCount);
		if (bPrefer)
		{
			SelectedFooter = Candidate;
			SelectedButtonCount = ButtonCount;
		}
	}
	if (!SelectedFooter)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvFooterBinding] source=%s footerInstances=0 result=missing"),
			Source);
		return;
	}

	MSPFooterBarWidget = SelectedFooter;
	UHorizontalBox* Buttons = Cast<UHorizontalBox>(
		SelectedFooter->GetWidgetFromName(TEXT("ButtonsHBox")));
	if (!Buttons)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvFooterBinding] source=%s footer=%s class=%s outer=%s footerInstances=%d ButtonsHBox=missing"),
			Source,
			*SelectedFooter->GetPathName(),
			*SelectedFooter->GetClass()->GetPathName(),
			SelectedFooter->GetOuter() ?
				*SelectedFooter->GetOuter()->GetPathName() : TEXT("<none>"),
			Footers.Num());
		return;
	}

	++MSPFooterBindingPasses;
	MSPApplyButtons.Reset();
	MSPApplyWidgets.Reset();
	MSPApplyButton.Reset();
	MSPResetButton.Reset();
	MSPBackButton.Reset();
	MSPApplyWidget.Reset();

	const int32 FooterButtonCount = Buttons->GetChildrenCount();
	for (int32 Index = 0; Index < FooterButtonCount; ++Index)
	{
		UUserWidget* FooterButtonWidget =
			Cast<UUserWidget>(Buttons->GetChildAt(Index));
		if (!FooterButtonWidget)
		{
			continue;
		}
		UButton* Button = Cast<UButton>(
			FooterButtonWidget->GetWidgetFromName(TEXT("NavigationKeyButton")));
		if (!Button)
		{
			Button = FindFirstButton(FooterButtonWidget);
		}
		if (!Button)
		{
			continue;
		}

		const FName InputAction = ReadMSPNameProperty(
			FooterButtonWidget, TEXT("PlayerMappableOptionsName"));
		const FString OptionName = NormalizeMSPText(
			ReadMSPTextProperty(FooterButtonWidget, TEXT("OptionName")));
		UTextBlock* ButtonText = Cast<UTextBlock>(
			FooterButtonWidget->GetWidgetFromName(TEXT("ButtonText")));
		const FString VisibleText = ButtonText ?
			NormalizeMSPText(ButtonText->GetText().ToString()) : FString();

		enum class EFooterRole : uint8 { Unknown, Apply, Reset, Back };
		EFooterRole FooterRole = EFooterRole::Unknown;
		if (InputAction == TEXT("IA_Apply") || OptionName == TEXT("APPLY") ||
			VisibleText == TEXT("APPLY") || VisibleText == TEXT("ПРИМЕНИТЬ"))
		{
			FooterRole = EFooterRole::Apply;
		}
		else if (InputAction == TEXT("IA_Reset") ||
			OptionName == TEXT("RESET SETTINGS") ||
			VisibleText == TEXT("RESET SETTINGS") ||
			VisibleText == TEXT("СБРОСИТЬ НАСТРОЙКИ"))
		{
			FooterRole = EFooterRole::Reset;
		}
		else if (InputAction == TEXT("IA_Back") || OptionName == TEXT("BACK") ||
			VisibleText == TEXT("BACK") || VisibleText == TEXT("НАЗАД"))
		{
			FooterRole = EFooterRole::Back;
		}
		else if (FooterButtonCount == 3)
		{
			FooterRole = Index == 0 ? EFooterRole::Apply :
				Index == 1 ? EFooterRole::Reset : EFooterRole::Back;
		}

		const TCHAR* RoleName = TEXT("Unknown");
		const TCHAR* HandlerName = TEXT("<none>");
		Button->OnClicked.Clear();
		switch (FooterRole)
		{
		case EFooterRole::Apply:
			RoleName = TEXT("Apply");
			HandlerName = TEXT("HandleMSPApplyClicked");
			Button->OnClicked.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPApplyClicked);
			MSPApplyButton = Button;
			MSPApplyWidget = FooterButtonWidget;
			MSPApplyButtons.Add(Button);
			MSPApplyWidgets.Add(FooterButtonWidget);
			if (ButtonText)
			{
				ButtonText->SetText(
					FAvLoc::Text(TEXT("MainMenu.MSP.Common.Apply")));
			}
			break;
		case EFooterRole::Reset:
			RoleName = TEXT("Reset");
			HandlerName = TEXT("HandleMSPResetClicked");
			Button->OnClicked.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPResetClicked);
			MSPResetButton = Button;
			if (ButtonText)
			{
				ButtonText->SetText(
					FAvLoc::Text(TEXT("MainMenu.MSP.Common.ResetSettings")));
			}
			break;
		case EFooterRole::Back:
			RoleName = TEXT("Back");
			HandlerName = TEXT("HandleMSPBackClicked");
			Button->OnClicked.AddUniqueDynamic(
				this, &AMenuHUD::HandleMSPBackClicked);
			MSPBackButton = Button;
			if (ButtonText)
			{
				ButtonText->SetText(
					FAvLoc::Text(TEXT("MainMenu.Common.Back")));
			}
			break;
		default:
			break;
		}

		const ESlateVisibility Visibility = Button->GetVisibility();
		const bool bHitTestable =
			Visibility == ESlateVisibility::Visible;
		UE_LOG(LogTemp, Log,
			TEXT("[AvFooterBinding] source=%s pass=%d footerInstances=%d footerButtonIndex=%d role=%s widget=%s class=%s outer=%s button=%s visibility=%s enabled=%s hitTestable=%s handlers=%d handler=%s cachedSlate=%s inputAction=%s option=\"%s\""),
			Source, MSPFooterBindingPasses, Footers.Num(), Index, RoleName,
			*FooterButtonWidget->GetPathName(),
			*FooterButtonWidget->GetClass()->GetPathName(),
			FooterButtonWidget->GetOuter() ?
				*FooterButtonWidget->GetOuter()->GetPathName() : TEXT("<none>"),
			*Button->GetPathName(),
			*StaticEnum<ESlateVisibility>()->GetNameStringByValue(
				static_cast<int64>(Visibility)),
			Button->GetIsEnabled() ? TEXT("true") : TEXT("false"),
			bHitTestable ? TEXT("true") : TEXT("false"),
			Button->OnClicked.GetAllObjects().Num(),
			HandlerName,
			FooterButtonWidget->GetCachedWidget().IsValid() ?
				TEXT("true") : TEXT("false"),
			*InputAction.ToString(), *OptionName);
	}
	RefreshMSPApplyState();
}

void AMenuHUD::ApplyFinalMSPHomeStyle(const TCHAR* Source)
{
	if (!MSPHomeMenuWidget)
	{
		return;
	}
	// The authored intro animation leaves GameTitleText at its dark starting
	// tint after the production widget has already completed Construct. Stop
	// that one-shot animation before applying the final project-owned home
	// style; button hover is delegate/style driven and is unaffected.
	MSPHomeMenuWidget->StopAllAnimations();

	const FLinearColor Normal(0.025f, 0.031f, 0.038f, 0.96f);
	const FLinearColor Hovered(1.f, 0.40f, 0.045f, 1.f);
	const FLinearColor Pressed(0.68f, 0.20f, 0.018f, 1.f);
	const FLinearColor Disabled(0.025f, 0.028f, 0.032f, 0.86f);
	for (const TPair<FName, TWeakObjectPtr<UButton>>& Pair : MSPHomeButtons)
	{
		UButton* Button = Pair.Value.Get();
		if (!Button)
		{
			continue;
		}
		UUserWidget* OwnerWidget = Button->GetTypedOuter<UUserWidget>();
		if (OwnerWidget)
		{
			for (const FName Property : {
				TEXT("ButtonColor"), TEXT("BackgroundColor")})
			{
				WriteMSPLinearColorProperty(OwnerWidget, Property, Normal);
			}
			for (const FName Property : {
				TEXT("ButtonHighlightColor"), TEXT("BackgroundHighlightColor"),
				TEXT("HighlightColor"), TEXT("ButtonActiveColor"),
				TEXT("BackgroundActiveColor"), TEXT("ActiveColor")})
			{
				WriteMSPLinearColorProperty(OwnerWidget, Property, Hovered);
			}
			for (const FName Property : {
				TEXT("ButtonPressedColor"), TEXT("BackgroundPressedColor"),
				TEXT("PressedColor")})
			{
				WriteMSPLinearColorProperty(OwnerWidget, Property, Pressed);
			}
			for (const FName Property : {
				TEXT("TextColor"), TEXT("TextHighlightColor"),
				TEXT("TextActiveColor")})
			{
				WriteMSPLinearColorProperty(
					OwnerWidget, Property, FLinearColor::White);
			}
			InvokeMSPNoParameterFunction(
				OwnerWidget, TEXT("ApplyWidgetStyleNormal"));
		}

		FButtonStyle Style = Button->GetStyle();
		Style.Normal.TintColor = FSlateColor(Normal);
		Style.Hovered.TintColor = FSlateColor(Hovered);
		Style.Pressed.TintColor = FSlateColor(Pressed);
		Style.Disabled.TintColor = FSlateColor(Disabled);
		Style.SetNormalForeground(FSlateColor(FLinearColor::White));
		Style.SetHoveredForeground(FSlateColor(FLinearColor::White));
		Style.SetPressedForeground(FSlateColor(FLinearColor::White));
		Button->SetStyle(Style);
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenuStyle] source=%s owner=%s button=%s normal=%s hover=orange pressed=dark-orange"),
			Source, *Pair.Key.ToString(), *Button->GetPathName(),
			*Normal.ToString());
	}

	if (UTextBlock* GameTitle = Cast<UTextBlock>(
		MSPHomeMenuWidget->GetWidgetFromName(TEXT("GameTitleText"))))
	{
		const bool bHadColorBinding =
			GameTitle->ColorAndOpacityDelegate.IsBound();
		GameTitle->ColorAndOpacityDelegate.Unbind();
		GameTitle->SetText(FAvLoc::Text(TEXT("MainMenu.Title")));
		GameTitle->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		GameTitle->SetOpacity(1.f);
		GameTitle->SetFont(MakeAuthenticMSPBodyFont(
			FMath::RoundToInt(GameTitle->GetFont().Size)));
		GameTitle->SetShadowColorAndOpacity(FLinearColor::Transparent);
		FString ParentOpacities;
		for (UWidget* Parent = GameTitle->GetParent();
			Parent; Parent = Parent->GetParent())
		{
			if (!ParentOpacities.IsEmpty())
			{
				ParentOpacities += TEXT("|");
			}
			ParentOpacities += FString::Printf(
				TEXT("%s:%.3f"), *Parent->GetName(),
				Parent->GetRenderOpacity());
			Parent->SetRenderOpacity(1.f);
		}
		int32 TitleZOrder = INDEX_NONE;
		if (UCanvasPanelSlot* TitleSlot =
			Cast<UCanvasPanelSlot>(GameTitle->Slot))
		{
			TitleSlot->SetZOrder(100);
			TitleZOrder = TitleSlot->GetZOrder();
		}
		GameTitle->SetRenderOpacity(1.f);
		GameTitle->SetVisibility(ESlateVisibility::HitTestInvisible);
		GameTitle->InvalidateLayoutAndVolatility();
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenuBrand] source=%s object=%s text=\"%s\" visibility=%s opacity=%.2f color=white colorBindingRemoved=%s fontMaterial=none parentOpacityBefore={%s} zOrder=%d"),
			Source, *GameTitle->GetPathName(),
			*GameTitle->GetText().ToString(),
			*StaticEnum<ESlateVisibility>()->GetNameStringByValue(
				static_cast<int64>(GameTitle->GetVisibility())),
			GameTitle->GetRenderOpacity(),
			bHadColorBinding ? TEXT("true") : TEXT("false"),
			*ParentOpacities, TitleZOrder);
	}
}

void AMenuHUD::RefreshStableMSPSettingsVisualState(const uint8 Category)
{
	if (Category > 3)
	{
		return;
	}

	static const FName TabNames[] = {
		TEXT("GameplayLargeTabButton"),
		TEXT("AudioLargeTabButton"),
		TEXT("VideoLargeTabButton"),
		TEXT("ControlsLargeTabButton")
	};
	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Tab = *It;
		if (!IsValid(Tab) || Tab->GetWorld() != GetWorld())
		{
			continue;
		}
		int32 TabIndex = INDEX_NONE;
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(TabNames); ++Index)
		{
			if (Tab->GetFName() == TabNames[Index])
			{
				TabIndex = Index;
				break;
			}
		}
		if (TabIndex == INDEX_NONE)
		{
			continue;
		}
		const TCHAR* State = TabIndex == Category ? TEXT("Active") : TEXT("Normal");
		WriteMSPEnumProperty(Tab, TEXT("FallbackState"), State);
		InvokeMSPEnumFunction(Tab, TEXT("SetWidgetState"), State);
		InvokeMSPNoParameterFunction(
			Tab,
			TabIndex == Category ?
				TEXT("ApplyWidgetStyleActive") : TEXT("ApplyWidgetStyleNormal"));
	}

	UUserWidget* Pages[] = {
		StableMSPScreenPage,
		StableMSPGraphicsPage,
		StableMSPAudioPage,
		StableMSPLanguagePage
	};
	static const TCHAR* DescriptionKeys[] = {
		TEXT("MainMenu.MSP.Video.WindowMode"),
		TEXT("MainMenu.MSP.Graphics.QualityPreset"),
		TEXT("MainMenu.MSP.Audio.Master"),
		TEXT("MainMenu.Settings.Language")
	};
	UUserWidget* ActivePage = Pages[Category];
	if (!ActivePage)
	{
		return;
	}
	ActiveMSPVisualSubMenuClassPath = ActivePage->GetClass()->GetPathName();
	if (UTextBlock* Name = Cast<UTextBlock>(
		ActivePage->GetWidgetFromName(TEXT("SelectedOptionName"))))
	{
		Name->SetText(FAvLoc::Text(DescriptionKeys[Category]));
	}
	for (const FName DescriptionName : {
		TEXT("SelectedOptionDescription"),
		TEXT("SelectedOptionDescription_1")})
	{
		if (URichTextBlock* Description = Cast<URichTextBlock>(
			ActivePage->GetWidgetFromName(DescriptionName)))
		{
			Description->SetText(FAvLoc::Text(
				FString(DescriptionKeys[Category]) + TEXT(".Description")));
		}
	}
	if (UWidget* DescriptionImageContainer =
		ActivePage->GetWidgetFromName(TEXT("SizeBox_0")))
	{
		DescriptionImageContainer->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMenuHUD::SetMSPSettingsFooterVisible(const bool bVisible)
{
	TArray<UUserWidget*> Footers;
	UUserWidget* StableFooter = nullptr;
	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Candidate = *It;
		if (!IsValid(Candidate) || Candidate->GetWorld() != GetWorld() ||
			!Candidate->GetClass()->GetPathName().Contains(
				TEXT("WBP_MenuFooterBar_Silence_C")))
		{
			continue;
		}
		Footers.Add(Candidate);
		for (UObject* Outer = Candidate; Outer; Outer = Outer->GetOuter())
		{
			if (Outer == StableMSPSettingsRoot)
			{
				StableFooter = Candidate;
				break;
			}
		}
	}
	if (!StableFooter && MSPFooterBarWidget.IsValid() &&
		Footers.Contains(MSPFooterBarWidget.Get()))
	{
		StableFooter = MSPFooterBarWidget.Get();
	}
	if (!StableFooter && !Footers.IsEmpty())
	{
		StableFooter = Footers[0];
	}
	MSPFooterBarWidget = StableFooter;
	for (UUserWidget* Footer : Footers)
	{
		Footer->SetVisibility(
			bVisible && Footer == StableFooter ?
				ESlateVisibility::SelfHitTestInvisible :
				ESlateVisibility::Collapsed);
	}
}

void AMenuHUD::RequestStableMSPSettingsCategory(const uint8 Category)
{
	ActivateStableMSPSettingsCategory(Category, true);
}

void AMenuHUD::ActivateStableMSPSettingsCategory(
	const uint8 Category, const bool bCountAsClick)
{
	if (!bStableMSPSettingsReady || !MSPSettingsPageSwitcher || Category > 3)
	{
		return;
	}
	if (bCountAsClick)
	{
		++MSPSettingsTabClickSerial;
		++MSPSettingsLogicalTransitions;
	}
	UUserWidget* Pages[] = {
		StableMSPScreenPage,
		StableMSPGraphicsPage,
		StableMSPAudioPage,
		StableMSPLanguagePage
	};
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Pages[Index]->SetVisibility(
			Index == Category ? ESlateVisibility::SelfHitTestInvisible :
				ESlateVisibility::Collapsed);
	}
	MSPSettingsPageSwitcher->SetActiveWidgetIndex(Category);
	MSPSettingsActiveCategory = Category;
	bMSPLanguageOpen = Category == 3;
	RefreshStableMSPSettingsVisualState(Category);
	SyncAuthenticMSPSettingsFromAdapter();
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsPageSwap] TabClick serial=%d category=%d logicalTransitionsThisClick=1 SetActiveWidgetIndex=%d visiblePage=%s identity=%p hoverChangedPage=false focusChangedPage=false"),
		MSPSettingsTabClickSerial, Category, Category,
		*Pages[Category]->GetPathName(), Pages[Category]);
	ValidateStableMSPSettingsInvariant();
}

void AMenuHUD::OpenStableMSPSettings()
{
	if (!bStableMSPSettingsReady || !MSPMainSwitcher ||
		!StableMSPSettingsRoot || !MSPHomeMenuWidget)
	{
		bOpenStableMSPSettingsWhenReady = true;
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsPageSwap] Settings open queued until the one-time hidden warmup completes."));
		return;
	}
	UBorder* SubMenuLayer = AuthenticMSPRoot.IsValid() ?
		Cast<UBorder>(AuthenticMSPRoot->GetWidgetFromName(TEXT("SubMenu"))) : nullptr;
	// The vendor rows finish their authored PreConstruct while the stable tree
	// is hidden. Reassert the production delegates at the actual open event,
	// after that lifecycle has completed, so visual-only vendor handlers cannot
	// bypass the settings adapter or its pending-change state.
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
	{
		if (UUserWidget* RowWidget = Pair.Key.Get();
			IsValid(RowWidget) && RowWidget->GetWorld() == GetWorld())
		{
			BindAuthenticMSPSettingInput(RowWidget);
		}
	}
	bMSPSettingsOpen = true;
	MSPSettingsActiveCategory = 0;
	ActivateStableMSPSettingsCategory(0, false);
	MSPHomeMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	StableMSPSettingsRoot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MSPMainSwitcher->SetActiveWidgetIndex(1);
	SetMSPSettingsFooterVisible(true);
	BindStableMSPSettingsFooter(TEXT("OpenSettingsVisible"));
	if (SubMenuLayer)
	{
		SubMenuLayer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	bReturnHomeAfterMSPApply = false;
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsPageSwap] OpenSettings reusedRoot=true root=%s identity=%p mainIndex=1 category=%d"),
		*StableMSPSettingsRoot->GetPathName(), StableMSPSettingsRoot.Get(),
		MSPSettingsActiveCategory);
	if (!bLoggedStableMSPSettingsDump)
	{
		LogStableMSPSettingsWidgetDump();
		bLoggedStableMSPSettingsDump = true;
	}
	ValidateStableMSPSettingsInvariant();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this]
			{
				if (!bMSPSettingsOpen || !bStableMSPSettingsReady)
				{
					return;
				}
				SyncAuthenticMSPSettingsFromAdapter();
				BindStableMSPSettingsFooter(TEXT("OpenSettingsNextTick"));
				RefreshMSPApplyState();
				// All authored setup and the initial adapter sync are complete.
				// Apply the idempotent role policy once, as the final writer.
				ApplyStableMSPSettingsRowFontsAfterFirstReveal();
				if (!bMSPMouseInputActive)
				{
					FocusFirstVisibleMSPSettingsButton();
				}
			}));
	}
}

void AMenuHUD::CloseStableMSPSettings()
{
	if (!bStableMSPSettingsReady || !MSPMainSwitcher)
	{
		return;
	}
	UUserWidget* Pages[] = {
		StableMSPScreenPage,
		StableMSPGraphicsPage,
		StableMSPAudioPage,
		StableMSPLanguagePage
	};
	for (UUserWidget* Page : Pages)
	{
		Page->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (AuthenticMSPRoot.IsValid())
	{
		if (UBorder* SubMenuLayer = Cast<UBorder>(
			AuthenticMSPRoot->GetWidgetFromName(TEXT("SubMenu"))))
		{
			SubMenuLayer->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	StableMSPSettingsRoot->SetVisibility(ESlateVisibility::Collapsed);
	MSPHomeMenuWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MSPMainSwitcher->SetActiveWidgetIndex(0);
	SetMSPSettingsFooterVisible(false);
	bMSPSettingsOpen = false;
	bMSPLanguageOpen = false;
	bLoggedStableMSPSettingsDump = false;
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsPageSwap] CloseSettings reusedRoot=true removedDelegates=0 timers=0 animations=0 mainIndex=0"));
	ApplyFinalMSPHomeStyle(TEXT("ReturnFromSettings"));
	ValidateStableMSPSettingsInvariant();
	RefreshMSPHomeFocus();
}

void AMenuHUD::LogStableMSPSettingsWidgetDump()
{
	if (!bMSPSettingsInteractionDiagnostics || !GetWorld())
	{
		return;
	}
	UE_LOG(LogTemp, Log,
		TEXT("[AvSettingsPageSwapDump] BEGIN owningRoot=%s identity=%p owningPlayer=%s mainSwitcherIndex=%d pageSwitcherIndex=%d"),
		AuthenticMSPRoot.IsValid() ? *AuthenticMSPRoot->GetPathName() : TEXT("<none>"),
		AuthenticMSPRoot.Get(),
		GetOwningPlayerController() ? *GetOwningPlayerController()->GetPathName() : TEXT("<none>"),
		MSPMainSwitcher ? MSPMainSwitcher->GetActiveWidgetIndex() : INDEX_NONE,
		MSPSettingsPageSwitcher ? MSPSettingsPageSwitcher->GetActiveWidgetIndex() : INDEX_NONE);

	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Widget = *It;
		if (!IsValid(Widget) || Widget->GetWorld() != GetWorld())
		{
			continue;
		}
		const FString ClassPath = Widget->GetClass()->GetPathName();
		const FString ObjectPath = Widget->GetPathName();
		const bool bRelated =
			ClassPath.Contains(TEXT("WBP_AvMSP_Settings_C")) ||
			ClassPath.Contains(TEXT("WBP_AvMSP_VideoTabs_C")) ||
			ClassPath.Contains(TEXT("WBP_AvMSP_AudioTabs_C")) ||
			ClassPath.Contains(TEXT("WBP_AvMSP_Graphics_C")) ||
			ClassPath.Contains(TEXT("WBP_DisplaySettings_Silence_C")) ||
			ClassPath.Contains(TEXT("WBP_AudioSettings_Silence_C")) ||
			ClassPath.Contains(TEXT("WBP_AvMSP_Language_C")) ||
			ObjectPath.Contains(TEXT("LargeTabButton"));
		if (!bRelated)
		{
			continue;
		}

		FString Hierarchy = Widget->GetName();
		for (UWidget* Parent = Widget->GetParent(); Parent; Parent = Parent->GetParent())
		{
			Hierarchy += TEXT(" <- ") + Parent->GetName();
		}
		const UPanelSlot* Slot = Widget->Slot;
		int32 ZOrder = INDEX_NONE;
		if (const UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
		{
			ZOrder = CanvasSlot->GetZOrder();
		}
		const ESlateVisibility Visibility = Widget->GetVisibility();
		const bool bHitTestable =
			Visibility == ESlateVisibility::Visible ||
			Visibility == ESlateVisibility::SelfHitTestInvisible;
		const bool bProduction =
			Widget == StableMSPSettingsRoot ||
			Widget == StableMSPScreenPage ||
			Widget == StableMSPGraphicsPage ||
			Widget == StableMSPAudioPage ||
			Widget == StableMSPLanguagePage;
		const bool bProjectClass =
			ClassPath.Contains(TEXT("/Game/Avariika/UI/MainMenu/MSP/"));
		const UWidgetSwitcher* ParentSwitcher =
			Cast<UWidgetSwitcher>(Widget->GetParent());
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsPageSwapDump] object=%s class=%s outer=%s owningRoot=%s owningPlayer=%s identity=%p visibility=%s renderOpacity=%.3f enabled=%s hitTestable=%s zOrder=%d slot=%s parentHierarchy=\"%s\" parentSwitcherIndex=%d animationPlaying=%s role=%s source=%s"),
			*ObjectPath,
			*ClassPath,
			Widget->GetOuter() ? *Widget->GetOuter()->GetPathName() : TEXT("<none>"),
			AuthenticMSPRoot.IsValid() ? *AuthenticMSPRoot->GetPathName() : TEXT("<none>"),
			Widget->GetOwningPlayer() ? *Widget->GetOwningPlayer()->GetPathName() : TEXT("<none>"),
			Widget,
			*UEnum::GetValueAsString(Visibility),
			Widget->GetRenderOpacity(),
			Widget->GetIsEnabled() ? TEXT("true") : TEXT("false"),
			bHitTestable ? TEXT("true") : TEXT("false"),
			ZOrder,
			Slot ? *Slot->GetClass()->GetPathName() : TEXT("<none>"),
			*Hierarchy,
			ParentSwitcher ? ParentSwitcher->GetActiveWidgetIndex() : INDEX_NONE,
			Widget->IsAnyAnimationPlaying() ? TEXT("true") : TEXT("false"),
			bProduction ? TEXT("production-destination") : TEXT("non-production"),
			bProjectClass ? TEXT("project-wrapper") : TEXT("vendor-original"));
	}
	for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair :
		MSPSettingRows)
	{
		UUserWidget* RowWidget = Pair.Key.Get();
		if (!IsValid(RowWidget) || RowWidget->GetWorld() != GetWorld())
		{
			continue;
		}
		UTextBlock* RowLabel = Cast<UTextBlock>(
			RowWidget->GetWidgetFromName(TEXT("OptionNameText")));
		UTextBlock* ValueText = Cast<UTextBlock>(
			RowWidget->GetWidgetFromName(TEXT("PickedOptionText")));
		if (RowLabel)
		{
			UE_LOG(LogTemp, Log,
				TEXT("[AvSettingsFontVerify] object=%s class=%s outer=%s owner=%s role=RowLabel text=\"%s\" font={%s} SetFontCalls=%d source=CompleteStableMSPSettingsRuntimeAfterReparent"),
				*RowLabel->GetName(),
				*RowLabel->GetClass()->GetPathName(),
				RowLabel->GetOuter() ?
					*RowLabel->GetOuter()->GetPathName() : TEXT("<none>"),
				*RowWidget->GetPathName(),
				*RowLabel->GetText().ToString(),
				*DescribeMSPFont(RowLabel->GetFont()),
				MSPFontSetCounts.FindRef(RowLabel));
		}
		if (ValueText)
		{
			UE_LOG(LogTemp, Log,
				TEXT("[AvSettingsFontVerify] object=%s class=%s outer=%s owner=%s role=ValueText text=\"%s\" font={%s} SetFontCalls=%d"),
				*ValueText->GetName(),
				*ValueText->GetClass()->GetPathName(),
				ValueText->GetOuter() ?
					*ValueText->GetOuter()->GetPathName() : TEXT("<none>"),
				*RowWidget->GetPathName(),
				*ValueText->GetText().ToString(),
				*DescribeMSPFont(ValueText->GetFont()),
				MSPFontSetCounts.FindRef(ValueText));
		}
	}
	UUserWidget* Comparators[] = {
		StableMSPSettingsRoot,
		StableMSPScreenPage,
		StableMSPGraphicsPage,
		StableMSPAudioPage,
		StableMSPLanguagePage
	};
	for (UUserWidget* ComparatorOwner : Comparators)
	{
		if (!IsValid(ComparatorOwner))
		{
			continue;
		}
		const FName ComparatorNames[] = {
			TEXT("HeadlineTitleText"),
			TEXT("ButtonText")
		};
		for (const FName ComparatorName : ComparatorNames)
		{
			if (UTextBlock* Comparator = Cast<UTextBlock>(
				ComparatorOwner->GetWidgetFromName(ComparatorName)))
			{
				UE_LOG(LogTemp, Log,
					TEXT("[AvSettingsFontVerify] object=%s class=%s outer=%s owner=%s role=%s text=\"%s\" font={%s} SetFontCalls=%d"),
					*Comparator->GetName(),
					*Comparator->GetClass()->GetPathName(),
					Comparator->GetOuter() ?
						*Comparator->GetOuter()->GetPathName() : TEXT("<none>"),
					*ComparatorOwner->GetPathName(),
					ComparatorName == TEXT("HeadlineTitleText") ?
						TEXT("SectionHeading") : TEXT("TabText"),
					*Comparator->GetText().ToString(),
					*DescribeMSPFont(Comparator->GetFont()),
					MSPFontSetCounts.FindRef(Comparator));
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("[AvSettingsPageSwapDump] END"));
}

void AMenuHUD::ValidateStableMSPSettingsInvariant()
{
	if (!bStableMSPSettingsReady || !GetWorld())
	{
		return;
	}
	int32 SettingsRoots = 0;
	int32 ScreenPages = 0;
	int32 GraphicsPages = 0;
	int32 AudioPages = 0;
	int32 LanguagePages = 0;
	int32 VisiblePages = 0;
	int32 VisibleNonProductionVendorPages = 0;
	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Widget = *It;
		if (!IsValid(Widget) || Widget->GetWorld() != GetWorld())
		{
			continue;
		}
		const FString ClassPath = Widget->GetClass()->GetPathName();
		const bool bScreen = ClassPath.Contains(TEXT("WBP_DisplaySettings_Silence_C"));
		const bool bGraphics = ClassPath.Contains(TEXT("WBP_AvMSP_Graphics_C"));
		const bool bAudio = ClassPath.Contains(TEXT("WBP_AudioSettings_Silence_C"));
		const bool bLanguage = ClassPath.Contains(TEXT("WBP_AvMSP_Language_C"));
		SettingsRoots += ClassPath.Contains(TEXT("WBP_AvMSP_Settings_C")) ? 1 : 0;
		ScreenPages += bScreen ? 1 : 0;
		GraphicsPages += bGraphics ? 1 : 0;
		AudioPages += bAudio ? 1 : 0;
		LanguagePages += bLanguage ? 1 : 0;
		const ESlateVisibility Visibility = Widget->GetVisibility();
		const bool bSelfVisible =
			Visibility != ESlateVisibility::Collapsed &&
			Visibility != ESlateVisibility::Hidden;
		VisiblePages += (bScreen || bGraphics || bAudio || bLanguage) &&
			bSelfVisible ? 1 : 0;
		const bool bCachedProduction =
			Widget == StableMSPScreenPage || Widget == StableMSPAudioPage;
		VisibleNonProductionVendorPages +=
			(bScreen || bAudio) && !bCachedProduction && bSelfVisible ? 1 : 0;
	}
	const int32 ExpectedVisiblePages = bMSPSettingsOpen ? 1 : 0;
	const FString Signature = FString::Printf(
		TEXT("roots=%d screen=%d graphics=%d audio=%d language=%d visible=%d expected=%d active=%d main=%d duplicateBindings=%d staleCallbacks=%d"),
		SettingsRoots, ScreenPages, GraphicsPages, AudioPages, LanguagePages,
		VisiblePages, ExpectedVisiblePages, MSPSettingsActiveCategory,
		MSPMainSwitcher ? MSPMainSwitcher->GetActiveWidgetIndex() : INDEX_NONE,
		MSPSettingsDuplicateBindings, MSPSettingsStaleCallbacks);
	if (Signature != LastMSPSettingsInvariantSignature)
	{
		LastMSPSettingsInvariantSignature = Signature;
		UE_LOG(LogTemp, Log,
			TEXT("[AvSettingsPageSwapInvariant] %s SettingsRootInstances=%d VisibleSettingsPages=%d TabTransitionCallsPerClick=1 DuplicateBindings=%d VendorPageVisible=%s StaleTransitionCallbacks=%d"),
			*Signature, SettingsRoots, VisiblePages, MSPSettingsDuplicateBindings,
			VisibleNonProductionVendorPages == 0 ? TEXT("false") : TEXT("true"),
			MSPSettingsStaleCallbacks);
	}
	const bool bValid =
		SettingsRoots == 1 && ScreenPages == 1 && GraphicsPages == 1 &&
		AudioPages == 1 && LanguagePages == 1 &&
		VisiblePages == ExpectedVisiblePages &&
		VisibleNonProductionVendorPages == 0 &&
		MSPSettingsDuplicateBindings == 0 && MSPSettingsStaleCallbacks == 0;
	if (!bValid)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvSettingsPageSwapInvariant] FAILED %s vendorVisible=%d"),
			*Signature, VisibleNonProductionVendorPages);
	}
}

void AMenuHUD::HandleMSPScreenTabClicked()
{
	RequestStableMSPSettingsCategory(0);
}

void AMenuHUD::HandleMSPGraphicsTabClicked()
{
	RequestStableMSPSettingsCategory(1);
}

void AMenuHUD::HandleMSPAudioTabClicked()
{
	RequestStableMSPSettingsCategory(2);
}

void AMenuHUD::HandleMSPSettingsClicked()
{
	PendingMSPHomeFocusOwner = TEXT("OptionsTitleScreenButton");
	if (const TWeakObjectPtr<UButton>* Options =
		MSPHomeButtons.Find(TEXT("OptionsTitleScreenButton"));
		Options && Options->IsValid() && Options->Get()->IsHovered())
	{
		bMSPMouseInputActive = true;
	}
	OpenStableMSPSettings();
}

void AMenuHUD::HandleMSPLanguageClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[AvMainMenuSettings] Language tab activated by runtime input."));
	RequestStableMSPSettingsCategory(3);
}

void AMenuHUD::BindMSPLanguageTab(UUserWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	const FText LanguageLabel = FAvLoc::Text(TEXT("MainMenu.Settings.Language"));
	if (WriteMSPTextProperty(Widget, TEXT("OptionName"), LanguageLabel))
	{
		// MSP renders the authored OptionName through its own SetupButtonText
		// function. It is intentionally invoked even when the UObject property
		// already contains the localized value: the first runtime bridge pass
		// can otherwise leave Slate displaying the PreConstruct-era label.
		if (UFunction* SetupButtonText = Widget->FindFunction(TEXT("SetupButtonText")))
		{
			Widget->ProcessEvent(SetupButtonText, nullptr);
		}
		Widget->InvalidateLayoutAndVolatility();
	}

	Widget->SetIsEnabled(true);
	const bool bObsoleteInnerLanguageTab =
		Widget->GetName().Contains(TEXT("GeneralSmallTabButton"));
	Widget->SetVisibility(bObsoleteInnerLanguageTab ?
		ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	Widget->SetRenderOpacity(bObsoleteInnerLanguageTab ? 0.f : 1.f);
	const bool bNewLanguageTab = !BoundMSPLanguageTabWidgets.Contains(Widget);
	if (bNewLanguageTab)
	{
		WriteMSPEnumProperty(Widget, TEXT("FallbackState"), TEXT("Normal"));
		InvokeMSPEnumFunction(Widget, TEXT("SetWidgetState"), TEXT("Normal"));
	}
	UButton* Button = FindFirstButton(Widget);
	if (Button)
	{
		Button->SetIsEnabled(true);
		Button->OnClicked.RemoveDynamic(this, &AMenuHUD::HandleMSPLanguageClicked);
		if (!bObsoleteInnerLanguageTab)
		{
			MSPLanguageButton = Button;
		}
		MSPLanguageTabButtons.FindOrAdd(Widget) = Button;
	}
	if (!bNewLanguageTab)
	{
		return;
	}

	FMulticastDelegateProperty* ClickedProperty =
		FindFProperty<FMulticastDelegateProperty>(Widget->GetClass(), TEXT("OnWidgetClicked"));
	FMulticastScriptDelegate* ClickedDelegate = ClickedProperty ?
		ClickedProperty->ContainerPtrToValuePtr<FMulticastScriptDelegate>(Widget) : nullptr;
	if (!ClickedDelegate)
	{
		if (Button)
		{
			Button->OnClicked.AddUniqueDynamic(this, &AMenuHUD::HandleMSPLanguageClicked);
		}
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenuSettings] Language tab %s has no OnWidgetClicked multicast delegate."),
			*Widget->GetPathName());
		return;
	}

	// Remove only the authored Settings owner's old Gameplay route. The tab's
	// own inner-button click, animations, sounds and state machine remain intact.
	for (UObject* Outer = Widget->GetOuter(); Outer; Outer = Outer->GetOuter())
	{
		const FString OuterClassPath = Outer->GetClass()->GetPathName();
		if (Outer->IsA<UUserWidget>() &&
			OuterClassPath.Contains(TEXT("/Game/Avariika/UI/MainMenu/MSP/")))
		{
			ClickedDelegate->RemoveAll(Outer);
			break;
		}
	}

	FScriptDelegate Handler;
	Handler.BindUFunction(
		this, GET_FUNCTION_NAME_CHECKED(AMenuHUD, HandleMSPLanguageClicked));
	ClickedDelegate->Remove(Handler);
	ClickedDelegate->AddUnique(Handler);
	BoundMSPLanguageTabWidgets.Add(Widget);
	if (UUserWidget* SettingsMenu = Widget->GetTypedOuter<UUserWidget>())
	{
		SettingsMenu->InvalidateLayoutAndVolatility();
		SettingsMenu->ForceLayoutPrepass();
	}
	if (FSlateApplicationBase::IsInitialized())
	{
		FSlateApplicationBase::Get().InvalidateAllWidgets(false);
	}
	UE_LOG(LogTemp, Log,
		TEXT("[AvMainMenuSettings] Bound Language tab instance: %s OptionName=\"%s\" SetupButtonText=%s"),
		*Widget->GetPathName(),
		*ReadMSPTextProperty(Widget, TEXT("OptionName")),
		Widget->FindFunction(TEXT("SetupButtonText")) ? TEXT("true") : TEXT("false"));
}

void AMenuHUD::OpenMSPLanguage()
{
	UUserWidget* SettingsMenu = nullptr;
	for (const TWeakObjectPtr<UUserWidget>& Candidate : MSPSettingsMenuWidgets)
	{
		if (Candidate.IsValid() && Candidate->IsVisible())
		{
			SettingsMenu = Candidate.Get();
			break;
		}
	}
	if (!SettingsMenu && MSPSettingsMenuWidget.IsValid() &&
		MSPSettingsMenuWidget->IsVisible())
	{
		SettingsMenu = MSPSettingsMenuWidget.Get();
	}
	MSPLanguagePreviousSubMenuClassPath.Empty();
	if (AuthenticMSPRoot.IsValid())
	{
		if (UBorder* SubMenuLayer = Cast<UBorder>(
			AuthenticMSPRoot->GetWidgetFromName(TEXT("SubMenu"))))
		{
			if (UUserWidget* CurrentSubMenu = Cast<UUserWidget>(SubMenuLayer->GetContent()))
			{
				MSPLanguagePreviousSubMenuClassPath =
					CurrentSubMenu->GetClass()->GetPathName();
			}
		}
	}
	if (!SettingsMenu ||
		MSPLanguagePreviousSubMenuClassPath.IsEmpty() ||
		!TransitionMSPMenu(
			SettingsMenu,
			TEXT("/Game/Avariika/UI/MainMenu/MSP/WBP_AvMSP_Language.WBP_AvMSP_Language_C"),
			TEXT("SubMenu")))
	{
		bMSPLanguageOpen = false;
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenuSettings] Could not open the project-owned Language menu."));
		return;
	}

	if (!MSPLanguageReturnFocusButton.IsValid())
	{
		for (const TPair<TWeakObjectPtr<UUserWidget>, TWeakObjectPtr<UButton>>& Pair :
			MSPLanguageTabButtons)
		{
			if (Pair.Key.IsValid() && Pair.Value.IsValid() &&
				Pair.Key->GetName().Contains(TEXT("GeneralSmallTabButton")) &&
				Pair.Key->IsVisible() && Pair.Value->GetIsEnabled())
			{
				MSPLanguageReturnFocusButton = Pair.Value;
				break;
			}
		}
	}
	if (!MSPLanguageReturnFocusButton.IsValid())
	{
		for (const TPair<TWeakObjectPtr<UUserWidget>, TWeakObjectPtr<UButton>>& Pair :
			MSPLanguageTabButtons)
		{
			if (!Pair.Key.IsValid() || !Pair.Value.IsValid())
			{
				continue;
			}
			for (UObject* Outer = Pair.Key->GetOuter(); Outer; Outer = Outer->GetOuter())
			{
				if (Outer == SettingsMenu)
				{
					MSPLanguageReturnFocusButton = Pair.Value;
					break;
				}
			}
			if (MSPLanguageReturnFocusButton.IsValid())
			{
				break;
			}
		}
	}
	bMSPSettingsOpen = true;
	bMSPLanguageOpen = true;
	bFocusMSPSettingsAfterLanguageBack = false;
	UE_LOG(LogTemp, Log,
		TEXT("[AvMainMenuSettings] Opening project-owned Language menu from visible Settings instance %s."),
		*SettingsMenu->GetPathName());
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]
		{
			if (!bMSPMouseInputActive)
			{
				FocusFirstVisibleMSPSettingsButton();
			}
		}));
	}
}

bool AMenuHUD::TransitionMSPMenu(
	UUserWidget* FromMenu, const TCHAR* TargetClassPath, const TCHAR* MenuLayer)
{
	if (!FromMenu || !TargetClassPath || !MenuLayer)
	{
		return false;
	}

	UFunction* GetContainerFunction = FromMenu->FindFunction(TEXT("GetOwningContainer"));
	if (!GetContainerFunction)
	{
		return false;
	}
	FStructOnScope GetContainerParameters(GetContainerFunction);
	FromMenu->ProcessEvent(GetContainerFunction, GetContainerParameters.GetStructMemory());

	UObject* Container = nullptr;
	for (TFieldIterator<FProperty> It(GetContainerFunction); It; ++It)
	{
		if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(*It);
			ObjectProperty &&
			ObjectProperty->HasAnyPropertyFlags(CPF_ReturnParm | CPF_OutParm))
		{
			UObject* Candidate = ObjectProperty->GetObjectPropertyValue(
				ObjectProperty->ContainerPtrToValuePtr<void>(
					GetContainerParameters.GetStructMemory()));
			if (Candidate)
			{
				Container = Candidate;
				break;
			}
		}
	}
	if (!Container && AuthenticMSPRoot.IsValid())
	{
		Container = AuthenticMSPRoot.Get();
	}
	if (!Container)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenuSettings] Transition failed: no authored owning container."));
		return false;
	}

	UClass* TargetClass = LoadClass<UUserWidget>(nullptr, TargetClassPath);
	UFunction* TransitionFunction = Container->FindFunction(TEXT("TransitionToTargetMenu"));
	if (!TargetClass || !TransitionFunction)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenuSettings] Transition failed: TargetClass=%s TransitionFunction=%s Container=%s."),
			TargetClass ? TEXT("valid") : TEXT("null"),
			TransitionFunction ? TEXT("valid") : TEXT("null"),
			*Container->GetPathName());
		return false;
	}

	FStructOnScope Parameters(TransitionFunction);
	for (TFieldIterator<FProperty> It(TransitionFunction);
		It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
	{
		FProperty* Property = *It;
		if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			continue;
		}
		void* Storage = Property->ContainerPtrToValuePtr<void>(Parameters.GetStructMemory());
		const FName Name = Property->GetFName();
		if (Name == TEXT("FromMenu"))
		{
			if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(Property))
			{
				ObjectProperty->SetObjectPropertyValue(Storage, FromMenu);
			}
		}
		else if (Name == TEXT("ToMenu"))
		{
			if (FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Property))
			{
				*SoftClassProperty->ContainerPtrToValuePtr<FSoftObjectPtr>(
					Parameters.GetStructMemory()) = FSoftObjectPtr(TargetClass);
			}
			else if (FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
			{
				ClassProperty->SetPropertyValue(Storage, TargetClass);
			}
		}
		else if (Name == TEXT("MenuLayer"))
		{
			if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
			{
				StringProperty->SetPropertyValue(Storage, MenuLayer);
			}
			else if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
			{
				NameProperty->SetPropertyValue(Storage, FName(MenuLayer));
			}
		}
		else if (Name == TEXT("TransitionAnimation"))
		{
			// UFunction metadata is editor-only. The copied MSP transition's authored runtime
			// default is the same stable enum entry used by the accepted PIE flow.
			const FString EnumValue = TEXT("NewEnumerator4");
			if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
			{
				const int64 Value = FindMSPEnumValue(EnumProperty->GetEnum(), EnumValue);
				if (Value != INDEX_NONE)
				{
					EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(Storage, Value);
				}
			}
			else if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
			{
				const int64 Value = FindMSPEnumValue(ByteProperty->Enum, EnumValue);
				if (Value != INDEX_NONE)
				{
					ByteProperty->SetPropertyValue(Storage, static_cast<uint8>(Value));
				}
			}
		}
		else if (Name == TEXT("TransitionAnimationSpeed"))
		{
			if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
			{
				NumericProperty->SetFloatingPointPropertyValue(Storage, 1.0);
			}
		}
	}
	Container->ProcessEvent(TransitionFunction, Parameters.GetStructMemory());
	return true;
}

void AMenuHUD::HandleMSPCreditsClicked()
{
	PendingMSPHomeFocusOwner = TEXT("ExtrasTitleScreenButton");
	OpenMSPCredits();
}

void AMenuHUD::HandleMSPExitClicked()
{
	PendingMSPHomeFocusOwner = TEXT("QuitTitleScreenButton");
	OpenMSPExitConfirm();
}

void AMenuHUD::OpenMSPCredits()
{
	if (ActiveMSPOverlay)
	{
		return;
	}

	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Existing = *It;
		if (IsValid(Existing) && Existing->GetWorld() == GetWorld() && Existing->IsVisible() &&
			Existing->GetClass()->GetPathName().Contains(TEXT("WBP_AvMSP_Credits_C")))
		{
			if (UTextBlock* CreditsText = Cast<UTextBlock>(
				Existing->GetWidgetFromName(TEXT("CreditsTextBlock"))))
			{
				CreditsText->SetText(FText::FromString(
					FAvLoc::Text(TEXT("MainMenu.Credits.Title")).ToString() +
					TEXT("\n\n") +
					FAvLoc::Text(TEXT("MainMenu.Credits.Placeholder")).ToString()));
			}
			ActiveMSPOverlay = Existing;
			ActiveMSPOverlayKind = EMSPOverlay::Credits;
			bActiveMSPOverlayOwnedByRouter = false;
			InitializeAuthenticMSPWidgetNow(Existing);
			UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] Adopted the authored MSP Credits transition."));
			return;
		}
	}

	APlayerController* PC = GetOwningPlayerController();
	UClass* WidgetClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/Avariika/UI/MainMenu/MSP/WBP_AvMSP_Credits.WBP_AvMSP_Credits_C"));
	if (!PC || !WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenu] Authored MSP Credits widget is unavailable."));
		PendingMSPHomeFocusOwner = TEXT("ExtrasTitleScreenButton");
		return;
	}

	if (UUserWidget* Credits = CreateWidget<UUserWidget>(PC, WidgetClass))
	{
		if (UTextBlock* CreditsText = Cast<UTextBlock>(
			Credits->GetWidgetFromName(TEXT("CreditsTextBlock"))))
		{
			CreditsText->SetText(FText::FromString(
				FAvLoc::Text(TEXT("MainMenu.Credits.Title")).ToString() +
				TEXT("\n\n") +
				FAvLoc::Text(TEXT("MainMenu.Credits.Placeholder")).ToString()));
		}
		Credits->AddToViewport(500);
		InitializeAuthenticMSPWidgetNow(Credits);
		ActiveMSPOverlay = Credits;
		ActiveMSPOverlayKind = EMSPOverlay::Credits;
		bActiveMSPOverlayOwnedByRouter = true;
		PendingMSPHomeFocusOwner = TEXT("ExtrasTitleScreenButton");
		UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] Opened authored MSP Credits."));
	}
}

void AMenuHUD::OpenMSPComingSoon()
{
	if (ActiveMSPOverlay)
	{
		return;
	}

	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Existing = *It;
		if (IsValid(Existing) && Existing->GetWorld() == GetWorld() && Existing->IsVisible() &&
			Existing->GetClass()->GetPathName().Contains(TEXT("WBP_AvMSP_ComingSoon_C")))
		{
			ActiveMSPOverlay = Existing;
			ActiveMSPOverlayKind = EMSPOverlay::ComingSoon;
			bActiveMSPOverlayOwnedByRouter = false;
			ConfigureMSPDialog(
				Existing,
				FAvLoc::Text(TEXT("MainMenu.Multiplayer")),
				FAvLoc::Text(TEXT("MainMenu.ComingSoon")),
				false);
			UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] Adopted the authored MSP Coming Soon transition."));
			return;
		}
	}

	APlayerController* PC = GetOwningPlayerController();
	UClass* WidgetClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/Avariika/UI/MainMenu/MSP/WBP_AvMSP_ComingSoon.WBP_AvMSP_ComingSoon_C"));
	if (!PC || !WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenu] Authored MSP Coming Soon dialog is unavailable."));
		PendingMSPHomeFocusOwner = TEXT("MultiplayerTitleScreenButton");
		return;
	}

	if (UUserWidget* Dialog = CreateWidget<UUserWidget>(PC, WidgetClass))
	{
		Dialog->AddToViewport(500);
		ActiveMSPOverlay = Dialog;
		ActiveMSPOverlayKind = EMSPOverlay::ComingSoon;
		bActiveMSPOverlayOwnedByRouter = true;
		PendingMSPHomeFocusOwner = TEXT("MultiplayerTitleScreenButton");
		ConfigureMSPDialog(
			Dialog,
			FAvLoc::Text(TEXT("MainMenu.Multiplayer")),
			FAvLoc::Text(TEXT("MainMenu.ComingSoon")),
			false);
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenu] Multiplayer backend exists, but the production MSP browser is incomplete; showing Coming Soon."));
	}
}

void AMenuHUD::OpenMSPExitConfirm()
{
	if (ActiveMSPOverlay)
	{
		return;
	}

	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Existing = *It;
		if (IsValid(Existing) && Existing->GetWorld() == GetWorld() && Existing->IsVisible() &&
			Existing->GetClass()->GetPathName().Contains(TEXT("WBP_AvMSP_ConfirmDialog_C")))
		{
			ActiveMSPOverlay = Existing;
			ActiveMSPOverlayKind = EMSPOverlay::ExitConfirm;
			bActiveMSPOverlayOwnedByRouter = false;
			ConfigureMSPDialog(
				Existing,
				FAvLoc::Text(TEXT("MainMenu.Confirm.Exit.Title")),
				FAvLoc::Text(TEXT("MainMenu.Confirm.Exit.Body")),
				true);
			UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] Adopted the authored MSP exit confirmation transition."));
			return;
		}
	}

	APlayerController* PC = GetOwningPlayerController();
	UClass* WidgetClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/Avariika/UI/MainMenu/MSP/WBP_AvMSP_ConfirmDialog.WBP_AvMSP_ConfirmDialog_C"));
	if (!PC || !WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenu] Authored MSP ConfirmDialog is unavailable."));
		PendingMSPHomeFocusOwner = TEXT("QuitTitleScreenButton");
		return;
	}

	if (UUserWidget* Dialog = CreateWidget<UUserWidget>(PC, WidgetClass))
	{
		Dialog->AddToViewport(500);
		ActiveMSPOverlay = Dialog;
		ActiveMSPOverlayKind = EMSPOverlay::ExitConfirm;
		bActiveMSPOverlayOwnedByRouter = true;
		PendingMSPHomeFocusOwner = TEXT("QuitTitleScreenButton");
		ConfigureMSPDialog(
			Dialog,
			FAvLoc::Text(TEXT("MainMenu.Confirm.Exit.Title")),
			FAvLoc::Text(TEXT("MainMenu.Confirm.Exit.Body")),
			true);
		UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] Opened authored MSP exit confirmation."));
	}
}

void AMenuHUD::OpenMSPSettingsConfirm(const uint8 ConfirmKind)
{
	if (ActiveMSPOverlay)
	{
		return;
	}

	const EMSPOverlay Kind = static_cast<EMSPOverlay>(ConfirmKind);
	if (Kind != EMSPOverlay::DisplayConfirm &&
		Kind != EMSPOverlay::ResetConfirm &&
		Kind != EMSPOverlay::UnsavedConfirm)
	{
		return;
	}

	for (TObjectIterator<UButton> It; It; ++It)
	{
		UButton* Candidate = *It;
		if (IsValid(Candidate) && Candidate->GetWorld() == GetWorld() &&
			Candidate->IsVisible() && Candidate->HasAnyUserFocus())
		{
			MSPSettingsReturnFocusButton = Candidate;
			break;
		}
	}

	APlayerController* PC = GetOwningPlayerController();
	UClass* WidgetClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/Avariika/UI/MainMenu/MSP/WBP_AvMSP_ConfirmDialog.WBP_AvMSP_ConfirmDialog_C"));
	if (!PC || !WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenuSettings] MSP ConfirmDialog is unavailable."));
		return;
	}

	UUserWidget* Dialog = CreateWidget<UUserWidget>(PC, WidgetClass);
	if (!Dialog)
	{
		return;
	}
	Dialog->AddToViewport(600);
	ActiveMSPOverlay = Dialog;
	ActiveMSPOverlayKind = Kind;
	bActiveMSPOverlayOwnedByRouter = true;

	FText Title;
	FText Body;
	FText YesLabel;
	FText NoLabel;
	switch (Kind)
	{
	case EMSPOverlay::DisplayConfirm:
		Title = FAvLoc::Text(TEXT("MainMenu.Confirm.Display.Title"));
		Body = FAvLoc::Text(TEXT("MainMenu.Confirm.Display.Body"));
		YesLabel = FAvLoc::Text(TEXT("MainMenu.Confirm.Display.Keep"));
		NoLabel = FAvLoc::Text(TEXT("MainMenu.Confirm.Display.Revert"));
		break;
	case EMSPOverlay::ResetConfirm:
		Title = FAvLoc::Text(TEXT("MainMenu.Confirm.Reset.Title"));
		Body = FAvLoc::Text(TEXT("MainMenu.Confirm.Reset.Body"));
		YesLabel = FAvLoc::Text(TEXT("MainMenu.Settings.Revert"));
		NoLabel = FAvLoc::Text(TEXT("MainMenu.Common.Cancel"));
		break;
	default:
		Title = FAvLoc::Text(TEXT("MainMenu.Confirm.Unsaved.Title"));
		Body = FAvLoc::Text(TEXT("MainMenu.Confirm.Unsaved.Body"));
		YesLabel = FAvLoc::Text(TEXT("MainMenu.MSP.Common.Apply"));
		NoLabel = FAvLoc::Text(TEXT("MainMenu.Settings.Discard"));
		break;
	}
	ConfigureMSPDialog(Dialog, Title, Body, true, YesLabel, NoLabel);

	if (Kind == EMSPOverlay::DisplayConfirm)
	{
		GetWorld()->GetTimerManager().SetTimer(
			MSPDisplayConfirmationTimer,
			FTimerDelegate::CreateWeakLambda(this, [this]
			{
				if (ActiveMSPOverlayKind == EMSPOverlay::DisplayConfirm)
				{
					UE_LOG(LogTemp, Warning,
						TEXT("[AvMainMenuSettings] Display confirmation timed out; reverting."));
					HandleMSPOverlayNoClicked();
				}
			}),
			15.f, false);
	}
}

void AMenuHUD::ConfigureMSPDialog(
	UUserWidget* Dialog, const FText& Title, const FText& Body, const bool bShowYes,
	const FText& YesLabel, const FText& NoLabel)
{
	if (!Dialog)
	{
		return;
	}
	// Overlay widgets can be created after the bounded startup bridge has
	// stopped. Initialize their project-owned localization/style explicitly.
	InitializeAuthenticMSPWidgetNow(Dialog);

	if (UTextBlock* TitleText = Cast<UTextBlock>(
		Dialog->GetWidgetFromName(TEXT("HeadlineTitleText"))))
	{
		TitleText->SetText(Title);
	}
	if (URichTextBlock* BodyText = Cast<URichTextBlock>(
		Dialog->GetWidgetFromName(TEXT("TextBody"))))
	{
		BodyText->SetText(Body);
	}

	UUserWidget* YesWidget = Cast<UUserWidget>(
		Dialog->GetWidgetFromName(TEXT("DecisionButton_Yes")));
	UUserWidget* NoWidget = Cast<UUserWidget>(
		Dialog->GetWidgetFromName(TEXT("DecisionButton_No")));
	if (YesWidget)
	{
		YesWidget->SetVisibility(bShowYes ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		MSPOverlayYesButton = FindFirstButton(YesWidget);
	}
	if (NoWidget)
	{
		MSPOverlayNoButton = FindFirstButton(NoWidget);
	}
	if (YesWidget && !YesLabel.IsEmpty())
	{
		if (UTextBlock* Label = Cast<UTextBlock>(
			YesWidget->GetWidgetFromName(TEXT("ButtonText"))))
		{
			Label->SetText(YesLabel);
		}
	}
	if (NoWidget && !NoLabel.IsEmpty())
	{
		if (UTextBlock* Label = Cast<UTextBlock>(
			NoWidget->GetWidgetFromName(TEXT("ButtonText"))))
		{
			Label->SetText(NoLabel);
		}
	}
	if (MSPOverlayYesButton.IsValid())
	{
		MSPOverlayYesButton->OnClicked.AddUniqueDynamic(this, &AMenuHUD::HandleMSPOverlayYesClicked);
	}
	if (MSPOverlayNoButton.IsValid())
	{
		MSPOverlayNoButton->OnClicked.AddUniqueDynamic(this, &AMenuHUD::HandleMSPOverlayNoClicked);
	}
	if (bShowYes && MSPOverlayYesButton.IsValid() && MSPOverlayNoButton.IsValid())
	{
		MSPOverlayYesButton->SetNavigationRuleExplicit(
			EUINavigation::Left, MSPOverlayNoButton.Get());
		MSPOverlayYesButton->SetNavigationRuleExplicit(
			EUINavigation::Right, MSPOverlayNoButton.Get());
		MSPOverlayNoButton->SetNavigationRuleExplicit(
			EUINavigation::Left, MSPOverlayYesButton.Get());
		MSPOverlayNoButton->SetNavigationRuleExplicit(
			EUINavigation::Right, MSPOverlayYesButton.Get());
	}

	bMSPOverlayYesFocused = false;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]
		{
			FocusMSPButton(MSPOverlayNoButton.Get());
		}));
	}
}

void AMenuHUD::CloseMSPOverlay(const FName RestoreOwner)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MSPDisplayConfirmationTimer);
	}
	if (ActiveMSPOverlay && bActiveMSPOverlayOwnedByRouter)
	{
		ActiveMSPOverlay->RemoveFromParent();
	}
	ActiveMSPOverlay = nullptr;
	ActiveMSPOverlayKind = EMSPOverlay::None;
	bActiveMSPOverlayOwnedByRouter = false;
	MSPOverlayYesButton.Reset();
	MSPOverlayNoButton.Reset();
	bMSPOverlayYesFocused = false;
	PendingMSPHomeFocusOwner = RestoreOwner;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, RestoreOwner]
		{
			if (ActiveMSPOverlayKind != EMSPOverlay::None)
			{
				return;
			}
			if (RestoreOwner.IsNone() && bMSPSettingsOpen)
			{
				if (MSPSettingsReturnFocusButton.IsValid() &&
					MSPSettingsReturnFocusButton->IsVisible() &&
					MSPSettingsReturnFocusButton->GetIsEnabled())
				{
					FocusMSPButton(MSPSettingsReturnFocusButton.Get());
				}
				else
				{
					FocusFirstVisibleMSPSettingsButton();
				}
			}
			else
			{
				RefreshMSPHomeFocus();
			}
			MSPSettingsReturnFocusButton.Reset();
		}));
	}
}

void AMenuHUD::HandleMSPOverlayYesClicked()
{
	const EMSPOverlay Kind = ActiveMSPOverlayKind;
	if (Kind == EMSPOverlay::DisplayConfirm)
	{
		const bool bReturnHome = bReturnHomeAfterMSPApply;
		bReturnHomeAfterMSPApply = false;
		if (SettingsAdapter)
		{
			SettingsAdapter->ConfirmApply();
			SyncAuthenticMSPSettingsFromAdapter();
		}
		CloseMSPOverlay(NAME_None);
		if (bReturnHome && GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(
				FTimerDelegate::CreateWeakLambda(this, [this]
				{
					InvokeMSPSettingsBack();
				}));
		}
		return;
	}
	if (Kind == EMSPOverlay::ResetConfirm)
	{
		CloseMSPOverlay(NAME_None);
		if (!SettingsAdapter)
		{
			return;
		}
		SettingsAdapter->ResetToDefaults();
		SyncAuthenticMSPSettingsFromAdapter();
		bool bNeedsDisplayConfirmation = false;
		SettingsAdapter->BeginApply(bNeedsDisplayConfirmation);
		if (bNeedsDisplayConfirmation)
		{
			OpenMSPSettingsConfirm(static_cast<uint8>(EMSPOverlay::DisplayConfirm));
		}
		else
		{
			SettingsAdapter->ConfirmApply();
			SyncAuthenticMSPSettingsFromAdapter();
		}
		return;
	}
	if (Kind == EMSPOverlay::UnsavedConfirm)
	{
		CloseMSPOverlay(NAME_None);
		if (!SettingsAdapter)
		{
			return;
		}
		CaptureAuthenticMSPSettings();
		bool bNeedsDisplayConfirmation = false;
		SettingsAdapter->BeginApply(bNeedsDisplayConfirmation);
		if (bNeedsDisplayConfirmation)
		{
			bReturnHomeAfterMSPApply = true;
			OpenMSPSettingsConfirm(static_cast<uint8>(EMSPOverlay::DisplayConfirm));
		}
		else
		{
			SettingsAdapter->ConfirmApply();
			SyncAuthenticMSPSettingsFromAdapter();
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick(
					FTimerDelegate::CreateWeakLambda(this, [this]
					{
						InvokeMSPSettingsBack();
					}));
			}
		}
		return;
	}
	if (Kind != EMSPOverlay::ExitConfirm)
	{
		HandleMSPOverlayNoClicked();
		return;
	}

	UWorld* World = GetWorld();
	if (World && World->WorldType == EWorldType::PIE)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenu] Exit confirmed in PIE; editor session remains open by design."));
		CloseMSPOverlay(TEXT("QuitTitleScreenButton"));
		return;
	}

	UKismetSystemLibrary::QuitGame(
		this, GetOwningPlayerController(), EQuitPreference::Quit, false);
}

void AMenuHUD::HandleMSPOverlayNoClicked()
{
	const EMSPOverlay Kind = ActiveMSPOverlayKind;
	if (Kind == EMSPOverlay::DisplayConfirm)
	{
		bReturnHomeAfterMSPApply = false;
		if (SettingsAdapter)
		{
			SettingsAdapter->RevertDisplayPreview();
			SyncAuthenticMSPSettingsFromAdapter();
		}
		CloseMSPOverlay(NAME_None);
		return;
	}
	if (Kind == EMSPOverlay::ResetConfirm)
	{
		CloseMSPOverlay(NAME_None);
		return;
	}
	if (Kind == EMSPOverlay::UnsavedConfirm)
	{
		if (SettingsAdapter)
		{
			SettingsAdapter->RevertPending();
			SyncAuthenticMSPSettingsFromAdapter();
		}
		CloseMSPOverlay(NAME_None);
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(
				FTimerDelegate::CreateWeakLambda(this, [this]
				{
					InvokeMSPSettingsBack();
				}));
		}
		return;
	}

	FName RestoreOwner = TEXT("QuitTitleScreenButton");
	if (Kind == EMSPOverlay::Credits)
	{
		RestoreOwner = TEXT("ExtrasTitleScreenButton");
	}
	else if (Kind == EMSPOverlay::ComingSoon)
	{
		RestoreOwner = TEXT("MultiplayerTitleScreenButton");
	}
	CloseMSPOverlay(RestoreOwner);
}

void AMenuHUD::TravelToGameplay(const bool bOpenCustomization)
{
	if (!GetWorld())
	{
		return;
	}

	const FString Options = bOpenCustomization ? TEXT("AvStartCustomize") : FString();
	UGameplayStatics::OpenLevel(
		this, FName(TEXT("/Game/Avariika/Maps/Lvl_FirstPerson")), true, Options);
}

void AMenuHUD::HandleMSPApplyClicked()
{
	++MSPApplyMouseClicks;
	UE_LOG(LogTemp, Log,
		TEXT("[AvFooterClick] role=Apply count=%d projectHandler=HandleMSPApplyClicked pendingBeforeCapture=%s"),
		MSPApplyMouseClicks,
		SettingsAdapter && SettingsAdapter->HasPendingChanges() ?
			TEXT("true") : TEXT("false"));
	if (!SettingsAdapter)
	{
		return;
	}
	CaptureAuthenticMSPSettings();
	if (!SettingsAdapter->HasPendingChanges())
	{
		RefreshMSPApplyState();
		return;
	}

	bool bNeedsDisplayConfirmation = false;
	if (!SettingsAdapter->BeginApply(bNeedsDisplayConfirmation))
	{
		return;
	}
	if (bNeedsDisplayConfirmation)
	{
		OpenMSPSettingsConfirm(static_cast<uint8>(EMSPOverlay::DisplayConfirm));
	}
	else
	{
		SettingsAdapter->ConfirmApply();
		SyncAuthenticMSPSettingsFromAdapter();
	}
	RemoveLegacyMSPJsonMirror();
}

void AMenuHUD::HandleMSPResetClicked()
{
	++MSPResetMouseClicks;
	UE_LOG(LogTemp, Log,
		TEXT("[AvFooterClick] role=Reset count=%d projectHandler=HandleMSPResetClicked"),
		MSPResetMouseClicks);
	if (SettingsAdapter && ActiveMSPOverlayKind == EMSPOverlay::None)
	{
		CaptureAuthenticMSPSettings();
		OpenMSPSettingsConfirm(static_cast<uint8>(EMSPOverlay::ResetConfirm));
	}
}

void AMenuHUD::HandleMSPBackClicked()
{
	++MSPBackMouseClicks;
	UE_LOG(LogTemp, Log,
		TEXT("[AvFooterClick] role=Back count=%d projectHandler=HandleMSPBackClicked pending=%s"),
		MSPBackMouseClicks,
		SettingsAdapter && SettingsAdapter->HasPendingChanges() ?
			TEXT("true") : TEXT("false"));
	InvokeMSPSettingsBack();
}

void AMenuHUD::InvokeMSPSettingsBack()
{
	if (!bMSPSettingsOpen || ActiveMSPOverlayKind != EMSPOverlay::None)
	{
		return;
	}
	if (bStableMSPSettingsReady)
	{
		CaptureAuthenticMSPSettings();
		if (SettingsAdapter && SettingsAdapter->HasPendingChanges())
		{
			OpenMSPSettingsConfirm(
				static_cast<uint8>(EMSPOverlay::UnsavedConfirm));
			return;
		}
		CloseStableMSPSettings();
		return;
	}
	bool bReturningFromLanguage = bMSPLanguageOpen;
	for (const TWeakObjectPtr<UUserWidget>& LanguageMenu : MSPLanguageMenuWidgets)
	{
		bReturningFromLanguage =
			bReturningFromLanguage || LanguageMenu.IsValid() && LanguageMenu->IsVisible();
	}
	CaptureAuthenticMSPSettings();
	if (!bReturningFromLanguage && SettingsAdapter && SettingsAdapter->HasPendingChanges())
	{
		OpenMSPSettingsConfirm(static_cast<uint8>(EMSPOverlay::UnsavedConfirm));
		return;
	}
	if (bReturningFromLanguage)
	{
		for (const TPair<TWeakObjectPtr<UUserWidget>, uint8>& Pair : MSPSettingRows)
		{
			if (Pair.Key.IsValid() &&
				static_cast<EAvMainMenuSettingRow>(Pair.Value) ==
					EAvMainMenuSettingRow::Language)
			{
				const int32 ActiveIndex = static_cast<int32>(ReadMSPNumericProperty(
					Pair.Key.Get(), TEXT("ActiveOptionIndex"), 0.0));
				WriteMSPNumericProperty(
					Pair.Key.Get(), TEXT("DefaultOptionIndex"), ActiveIndex);
			}
		}

		UUserWidget* LanguageScreen = nullptr;
		if (AuthenticMSPRoot.IsValid())
		{
			if (UBorder* SubMenuLayer = Cast<UBorder>(
				AuthenticMSPRoot->GetWidgetFromName(TEXT("SubMenu"))))
			{
				LanguageScreen = Cast<UUserWidget>(SubMenuLayer->GetContent());
			}
		}
		if (!LanguageScreen)
		{
			for (const TWeakObjectPtr<UUserWidget>& Candidate : MSPLanguageMenuWidgets)
			{
				if (Candidate.IsValid() && Candidate->IsVisible())
				{
					LanguageScreen = Candidate.Get();
					break;
				}
			}
		}
		UUserWidget* TransitionSource = LanguageScreen;
		if (!TransitionSource)
		{
			for (const TWeakObjectPtr<UUserWidget>& Candidate : MSPSettingsMenuWidgets)
			{
				if (Candidate.IsValid() && Candidate->IsVisible())
				{
					TransitionSource = Candidate.Get();
					break;
				}
			}
		}
		if (!TransitionSource && MSPSettingsMenuWidget.IsValid())
		{
			TransitionSource = MSPSettingsMenuWidget.Get();
		}
		if (!TransitionSource || MSPLanguagePreviousSubMenuClassPath.IsEmpty() ||
			!TransitionMSPMenu(
				TransitionSource, *MSPLanguagePreviousSubMenuClassPath, TEXT("SubMenu")))
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvMainMenuSettings] Language Back could not restore the previous Settings page."));
			return;
		}

		MSPLanguagePreviousSubMenuClassPath.Empty();
		bMSPLanguageOpen = false;
		bFocusMSPSettingsAfterLanguageBack = true;
		UE_LOG(LogTemp, Log,
			TEXT("[AvMainMenuSettings] Language Back is returning from %s to the previous Settings screen."),
			*TransitionSource->GetPathName());
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(
				FTimerDelegate::CreateWeakLambda(this, [this]
				{
					SyncAuthenticMSPSettingsFromAdapter();
				}));
		}
		return;
	}

	UUserWidget* Footer = MSPFooterBarWidget.Get();
	if (!Footer || !Footer->IsVisible())
	{
		for (TObjectIterator<UUserWidget> It; It; ++It)
		{
			UUserWidget* Candidate = *It;
			if (IsValid(Candidate) && Candidate->GetWorld() == GetWorld() &&
				Candidate->IsVisible() &&
				Candidate->GetClass()->GetPathName().Contains(TEXT("WBP_MenuFooterBar_Silence_C")))
			{
				Footer = Candidate;
				break;
			}
		}
	}

	bool bNavigated = false;
	if (Footer)
	{
		if (UFunction* NavigateBack = Footer->FindFunction(TEXT("NavigateBack")))
		{
			Footer->ProcessEvent(NavigateBack, nullptr);
			bNavigated = true;
		}
	}
	if (!bNavigated && MSPSettingsMenuWidget.IsValid())
	{
		if (UFunction* PressedBack = MSPSettingsMenuWidget->FindFunction(TEXT("OnPressedBack")))
		{
			MSPSettingsMenuWidget->ProcessEvent(PressedBack, nullptr);
			bNavigated = true;
		}
	}
	if (!bNavigated)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvMainMenuSettings] Authored Settings Back route was unavailable."));
		return;
	}

	bMSPSettingsOpen = false;
	PendingMSPHomeFocusOwner = TEXT("OptionsTitleScreenButton");
	MSPSettingsReturnFocusButton.Reset();
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this]
			{
				RefreshMSPHomeFocus();
			}));
	}
}

void AMenuHUD::RemoveLegacyMSPJsonMirror()
{
	const FString LegacyPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Config/MenuSystemConfig.json"));
	if (IFileManager::Get().FileExists(*LegacyPath))
	{
		if (IFileManager::Get().Delete(*LegacyPath, false, true, true))
		{
			UE_LOG(LogTemp, Log,
				TEXT("[AvMainMenu] Removed non-authoritative MSP JSON mirror; AvariikaUserSettings remains canonical."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AvMainMenu] Could not remove non-authoritative MSP JSON mirror."));
		}
	}
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

void AMenuHUD::DrawButton(const FString& Label, const float CenterX, const float Y,
	const float Width, const float Height, const FName BoxName, UFont* Font)
{
	const bool bHover = HoveredBox == BoxName;
	const float X = CenterX - Width * 0.5f;

	DrawRect(bHover ? PanelHover : PanelIdle, X, Y, Width, Height);
	DrawRect(Orange, X, Y, Width, 2.f);
	DrawRect(Orange, X, Y + Height - 2.f, Width, 2.f);
	DrawRect(Orange, X, Y, 2.f, Height);
	DrawRect(Orange, X + Width - 2.f, Y, 2.f, Height);

	float TW = 0.f;
	float TH = 0.f;
	GetTextSize(Label, TW, TH, Font, 1.2f);
	DrawText(Label, bHover ? FLinearColor::White : Orange,
		CenterX - TW * 0.5f, Y + (Height - TH) * 0.5f, Font, 1.2f);
	AddHitBox(FVector2D(X, Y), FVector2D(Width, Height), BoxName, true);
}

void AMenuHUD::DrawHUD()
{
	Super::DrawHUD();
	if (bAuthenticMSPMap || MainMenuRoot || !Canvas)
	{
		return;
	}

	const float SX = Canvas->SizeX;
	const float SY = Canvas->SizeY;
	UFont* Big = GEngine ? GEngine->GetLargeFont() : nullptr;
	UFont* Med = GEngine ? GEngine->GetMediumFont() : nullptr;

	DrawRect(MenuBG, 0.f, 0.f, SX, SY);
	DrawRect(Orange, 0.f, 0.f, SX, 4.f);

	const FString Title = FAvLoc::S(TEXT("menu_title"));
	float TW = 0.f;
	float TH = 0.f;
	GetTextSize(Title, TW, TH, Big, 3.0f);
	DrawText(Title, Orange, SX * 0.5f - TW * 0.5f, SY * 0.16f, Big, 3.0f);

	const FString Sub = FAvLoc::S(TEXT("menu_subtitle"));
	GetTextSize(Sub, TW, TH, Med, 1.1f);
	DrawText(Sub, Grey, SX * 0.5f - TW * 0.5f, SY * 0.16f + 64.f, Med, 1.1f);

	if (SettingsWidget.IsValid() && SettingsWidget->IsInViewport())
	{
		return;
	}

	const float CX = SX * 0.5f;
	const float BW = 380.f;
	const float BH = 58.f;
	const float Gap = 16.f;
	float Y = SY * 0.42f;

	if (Screen == EMenuScreen::Main)
	{
		DrawButton(FAvLoc::S(TEXT("menu_host")), CX, Y, BW, BH, TEXT("host"), Med); Y += BH + Gap;
		DrawButton(FAvLoc::S(TEXT("menu_find")), CX, Y, BW, BH, TEXT("find"), Med); Y += BH + Gap;
		DrawButton(FAvLoc::S(TEXT("menu_settings")), CX, Y, BW, BH, TEXT("settings"), Med); Y += BH + Gap;
		DrawButton(FAvLoc::S(TEXT("menu_quit")), CX, Y, BW, BH, TEXT("quit"), Med);
	}
	else
	{
		const FString Hdr = FAvLoc::S(TEXT("menu_browse_header"));
		GetTextSize(Hdr, TW, TH, Med, 1.3f);
		DrawText(Hdr, Orange, CX - TW * 0.5f, Y - 50.f, Med, 1.3f);

		UAvariikaOnlineSubsystem* Online = GetOnline();
		const int32 Num = Online ? Online->GetNumFoundSessions() : 0;
		if (bSearching && GetWorld() && GetWorld()->GetRealTimeSeconds() - SearchStartTime > 6.0)
		{
			bSearching = false;
		}

		if (Num <= 0)
		{
			const FString Msg = bSearching ? FAvLoc::S(TEXT("menu_searching")) : FAvLoc::S(TEXT("menu_no_games"));
			GetTextSize(Msg, TW, TH, Med, 1.1f);
			DrawText(Msg, Grey, CX - TW * 0.5f, Y + 8.f, Med, 1.1f);
			Y += 50.f;
		}
		else
		{
			for (int32 Index = 0; Index < Num; ++Index)
			{
				const FString Label = FAvLoc::S(TEXT("menu_game_prefix")) + FString::FromInt(Index + 1) +
					FAvLoc::S(TEXT("menu_join_suffix"));
				DrawButton(Label, CX, Y, BW, BH, FName(*FString::Printf(TEXT("join_%d"), Index)), Med);
				Y += BH + Gap;
			}
		}

		Y += Gap;
		DrawButton(FAvLoc::S(TEXT("menu_refresh")), CX, Y, BW, BH, TEXT("refresh"), Med); Y += BH + Gap;
		DrawButton(FAvLoc::S(TEXT("menu_back")), CX, Y, BW, BH, TEXT("back"), Med);
	}

	const FString Hint = FAvLoc::S(TEXT("menu_hint"));
	GetTextSize(Hint, TW, TH, Med, 0.9f);
	DrawText(Hint, FLinearColor(0.4f, 0.4f, 0.42f, 1.f), SX - TW - 16.f, SY - TH - 12.f, Med, 0.9f);
}

void AMenuHUD::NotifyHitBoxClick(const FName BoxName)
{
	Super::NotifyHitBoxClick(BoxName);
	if (bAuthenticMSPMap || MainMenuRoot)
	{
		return;
	}

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

void AMenuHUD::NotifyHitBoxBeginCursorOver(const FName BoxName)
{
	Super::NotifyHitBoxBeginCursorOver(BoxName);
	if (!bAuthenticMSPMap && !MainMenuRoot)
	{
		HoveredBox = BoxName;
	}
}

void AMenuHUD::NotifyHitBoxEndCursorOver(const FName BoxName)
{
	Super::NotifyHitBoxEndCursorOver(BoxName);
	if (!bAuthenticMSPMap && !MainMenuRoot && HoveredBox == BoxName)
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
	if (!WidgetClass || SettingsWidget.IsValid() && SettingsWidget->IsInViewport())
	{
		return;
	}

	if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass))
	{
		Widget->AddToViewport(100);
		SettingsWidget = Widget;
	}
}
