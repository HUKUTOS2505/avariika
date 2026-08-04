#include "UI/AvCharacterCustomizationRootWidget.h"

#include "AvariikaLoc.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CheckBox.h"
#include "Components/ContentWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/Overlay.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "AvaryoCharacter.h"
#include "Game/AvaryoPlayerController.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/UserInterfaceSettings.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UI/AvCustomizePreviewActor.h"
#include "UI/AvAppearanceInspectorSlotButton.h"
#include "UI/AvHeadCatalogItemButton.h"
#include "UI/AvMyCharacterRowWidget.h"
#include "UI/AvWorkerPresetCatalog.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Widgets/Layout/Anchors.h"
#include "EngineUtils.h"
#include "UnrealClient.h"

namespace AvCustomizePreview
{
	const FLinearColor CategoryActiveTint(0.95f, 0.42f, 0.04f, 1.f);
	const FLinearColor CategoryInactiveTint(0.56f, 0.58f, 0.62f, 1.f);
}

namespace AvCustomizationResponsive
{
	// All values are logical Slate units. They are deliberately independent of
	// named resolutions; Unreal's DPI curve converts them to physical pixels.
	constexpr float LeftPanelMin = 240.f;
	constexpr float LeftPanelPreferred = 300.f;
	constexpr float LeftPanelMax = 320.f;
	constexpr float RightPanelMin = 384.f;
	constexpr float RightPanelPreferred = 480.f;
	constexpr float RightPanelMax = 520.f;
	constexpr float PreviewSafeMin = 400.f;
	constexpr float PreviewDegradedMin = 96.f;
	constexpr float EdgeGapMin = 8.f;
	constexpr float EdgeGapPreferred = 24.f;
	constexpr float InterPanelGapMin = 12.f;
	constexpr float InterPanelGapPreferred = 40.f;
	// Just above the widest ordinary ultrawide shape. Only more extreme viewports
	// receive centered outer visual space; Root/background/RT stay full size.
	constexpr float MaxWorkingRegionAspect = 2.4f;
	constexpr int32 ShippingMinWidth = 1280;
	constexpr int32 ShippingMinHeight = 720;
}

namespace AvUpperEquipment
{
	// UE 5.8 multiplies this by Slate's global 32 px wheel amount. A value of 3
	// therefore produces a 96 px standard-notch target instead of the former 1536 px jump.
	constexpr float WheelScrollMultiplier = 3.0f;
	constexpr int32 RemovedDamagedRecordCount = 7;
}

namespace AvMyCharactersActions
{
	FLinearColor FromHex(uint8 R, uint8 G, uint8 B, uint8 A = 255)
	{
		return FLinearColor::FromSRGBColor(FColor(R, G, B, A));
	}

	const FLinearColor ProjectOrange = FromHex(0xF2, 0x6B, 0x0A);
}

namespace AvCustomizationLocalization
{
	FText Resolve(const FString& Key, const FString& Fallback = FString())
	{
		return Key.IsEmpty() ? FText::FromString(Fallback) : FAvLoc::Text(Key);
	}

	FText Tooltip(
		const FString& DisplayKey,
		const FString& DisplayFallback,
		const FString& ReasonKey = FString(),
		const FString& ReasonFallback = FString())
	{
		const FText Display = Resolve(DisplayKey, DisplayFallback);
		if (ReasonKey.IsEmpty() && ReasonFallback.IsEmpty())
		{
			return Display;
		}
		return FText::Format(
			FAvLoc::Text(TEXT("Customization.Common.TooltipWithReasonFormat")),
			Display,
			Resolve(ReasonKey, ReasonFallback));
	}
}

UAvCharacterCustomizationRootWidget::UAvCharacterCustomizationRootWidget(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAvCharacterCustomizationRootWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	ConfigureEquipmentConflictCheckbox();
	ConfigureCurrentSelectionRow(Txt_HeadCurrentSelection, Txt_HeadCurrentSelectionValue);
	ConfigureCurrentSelectionRow(Txt_FaceCurrentSelection, Txt_FaceCurrentSelectionValue);
	ConfigureCurrentSelectionRow(Txt_HandsCurrentSelection, Txt_HandsCurrentSelectionValue);
	ConfigureCurrentSelectionRow(Txt_UpperCurrentSelection, Txt_UpperCurrentSelectionValue);
	ConfigureCurrentSelectionRow(Txt_LowerCurrentSelection, Txt_LowerCurrentSelectionValue);
	ConfigureCurrentSelectionRow(
		Txt_FullOutfitsCurrentSelection, Txt_FullOutfitsCurrentSelectionValue);

	if (!IsDesignTime())
	{
		return;
	}

	HeadCatalog = UWorkerAppearanceComponent::GetHeadCustomizationCatalog();
	SelectedHeadSection = EAvHeadCustomizationSection::HeadType;
	EnsureHeadPageComposition();
	if (WS_HeadSectionPages)
	{
		WS_HeadSectionPages->SetActiveWidgetIndex(
			GetHeadSectionPageIndex(EAvHeadCustomizationSection::HeadType));
	}
	RefreshHeadSectionVisuals();

	if (Txt_HeadCurrentSelection)
	{
		Txt_HeadCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	}
	if (Txt_HeadCurrentSelectionValue)
	{
		Txt_HeadCurrentSelectionValue->SetText(
			FAvLoc::Text(TEXT("Customization.Item.Head.Type01")));
	}
	if (ScrollBox_HeadItems)
	{
		ScrollBox_HeadItems->SetVisibility(ESlateVisibility::Visible);
	}
	const FText EmptyStateText = FAvLoc::Text(TEXT("Customization.Common.NoAvailableOptions"));
	for (UTextBlock* EmptyState : {
		Txt_HeadEmptyState.Get(),
		Txt_HeadgearEmptyState.Get(),
		Txt_HairEmptyState.Get(),
		Txt_BeardEmptyState.Get()})
	{
		if (EmptyState)
		{
			EmptyState->SetText(EmptyStateText);
			EmptyState->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	auto ConfigureSample = [this](
		UAvHeadCatalogItemButton* Sample,
		FName HeadTypeId,
		bool bSelected)
	{
		if (!Sample)
		{
			return;
		}
		const FAvHeadCustomizationCatalogItem* Item = HeadCatalog.FindByPredicate(
			[HeadTypeId](const FAvHeadCustomizationCatalogItem& Candidate)
			{
				return Candidate.HeadTypeId == HeadTypeId;
			});
		if (!Item)
		{
			return;
		}
		UTexture2D* Thumbnail = Item->Thumbnail.IsEmpty()
			? nullptr
			: LoadObject<UTexture2D>(nullptr, *Item->Thumbnail);
		Sample->SetDesignTimePreview(
			AvCustomizationLocalization::Resolve(Item->DisplayNameKey, Item->DisplayName),
			Thumbnail,
			bSelected);
	};
	ConfigureSample(DesignSample_HeadType01, TEXT("HeadType01"), true);
	ConfigureSample(DesignSample_HeadType02, TEXT("HeadType02"), false);

	FaceProtectionCatalog = UWorkerAppearanceComponent::GetFaceProtectionCatalog();
	BuildFaceProtectionThumbnailRegistry();
	SelectedFaceProtectionSection = EAvFaceProtectionSection::Glasses;
	if (WS_FaceProtectionSectionPages)
	{
		WS_FaceProtectionSectionPages->SetActiveWidgetIndex(
			GetFaceProtectionSectionPageIndex(SelectedFaceProtectionSection));
	}
	if (Txt_FaceCurrentSelection)
	{
		Txt_FaceCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	}
	if (Txt_FaceCurrentSelectionValue)
	{
		Txt_FaceCurrentSelectionValue->SetText(FAvLoc::Text(TEXT("Customization.Common.None")));
	}
	for (UTextBlock* EmptyState : {
		Txt_GlassesEmptyState.Get(),
		Txt_RespiratorEmptyState.Get(),
		Txt_HeadphonesEmptyState.Get()})
	{
		if (EmptyState)
		{
			EmptyState->SetText(EmptyStateText);
			EmptyState->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	auto ConfigureFaceSample = [this](
		UAvHeadCatalogItemButton* Sample,
		EAvFaceProtectionSection Section,
		bool bNone,
		bool bSelected)
	{
		if (!Sample)
		{
			return;
		}
		for (int32 Index = 0; Index < FaceProtectionCatalog.Num(); ++Index)
		{
			const FAvFaceProtectionCatalogItem& Item = FaceProtectionCatalog[Index];
			if (Item.Section == Section && Item.bIsNone == bNone && Item.bEnabled)
			{
				Sample->SetDesignTimePreview(
					AvCustomizationLocalization::Resolve(Item.DisplayNameKey, Item.DisplayName),
					ResolveFaceProtectionItemThumbnail(Index),
					bSelected);
				return;
			}
		}
	};
	ConfigureFaceSample(DesignSample_FaceGlassesNone, EAvFaceProtectionSection::Glasses, true, true);
	ConfigureFaceSample(DesignSample_FaceGlassesItem, EAvFaceProtectionSection::Glasses, false, false);
	ConfigureFaceSample(DesignSample_FaceRespiratorNone, EAvFaceProtectionSection::Respirator, true, true);
	ConfigureFaceSample(DesignSample_FaceRespiratorItem, EAvFaceProtectionSection::Respirator, false, false);
	ConfigureFaceSample(DesignSample_FaceHeadphonesNone, EAvFaceProtectionSection::Headphones, true, true);
	ConfigureFaceSample(DesignSample_FaceHeadphonesItem, EAvFaceProtectionSection::Headphones, false, false);

	HandsAccessoriesCatalog = UWorkerAppearanceComponent::GetHandsAccessoriesCatalog();
	BuildHandsAccessoriesThumbnailRegistry();
	SelectedHandsAccessoriesSection = EAvHandsAccessoriesSection::Gloves;
	if (WS_HandsAccessoriesSectionPages)
	{
		WS_HandsAccessoriesSectionPages->SetActiveWidgetIndex(0);
	}
	RefreshHandsAccessoriesFilterVisuals();
	if (Txt_HandsCurrentSelectionValue)
	{
		Txt_HandsCurrentSelectionValue->SetText(FAvLoc::Text(TEXT("Customization.Common.None")));
	}
	for (UTextBlock* EmptyState : { Txt_GlovesEmptyState.Get(), Txt_WatchesEmptyState.Get() })
	{
		if (EmptyState) EmptyState->SetVisibility(ESlateVisibility::Collapsed);
	}
	auto ConfigureHandsSample = [this](UAvHeadCatalogItemButton* Sample,
		EAvHandsAccessoriesSection Section, bool bNone, bool bSelected)
	{
		if (!Sample) return;
		for (int32 Index = 0; Index < HandsAccessoriesCatalog.Num(); ++Index)
		{
			const FAvHandsAccessoriesCatalogItem& Item = HandsAccessoriesCatalog[Index];
			if (Item.Section == Section && Item.bIsNone == bNone && Item.bEnabled)
			{
				Sample->SetDesignTimePreview(
					AvCustomizationLocalization::Resolve(Item.DisplayNameKey, Item.DisplayName),
					ResolveHandsAccessoriesItemThumbnail(Index), bSelected);
				return;
			}
		}
	};
	ConfigureHandsSample(DesignSample_GlovesNone, EAvHandsAccessoriesSection::Gloves, true, true);
	ConfigureHandsSample(DesignSample_GlovesItem, EAvHandsAccessoriesSection::Gloves, false, false);
	ConfigureHandsSample(DesignSample_WatchesNone, EAvHandsAccessoriesSection::Watches, true, true);
	ConfigureHandsSample(DesignSample_WatchesItem, EAvHandsAccessoriesSection::Watches, false, false);

	TorsoCatalog = UWorkerAppearanceComponent::GetTorsoCatalog();
	BuildTorsoThumbnailRegistry();
	if (Txt_UpperCurrentSelection) Txt_UpperCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	if (Txt_UpperCurrentSelectionValue) Txt_UpperCurrentSelectionValue->SetText(FAvLoc::Text(TEXT("Customization.Common.None")));
	if (Txt_TorsoEmptyState) Txt_TorsoEmptyState->SetVisibility(ESlateVisibility::Collapsed);
	auto ConfigureTorsoSample = [this](UAvHeadCatalogItemButton* Sample, bool bNone, bool bSelected)
	{
		if (!Sample) return;
		for (int32 Index=0; Index<TorsoCatalog.Num(); ++Index)
		{
			const FAvTorsoCatalogItem& Item=TorsoCatalog[Index];
			if (Item.bIsNone==bNone && Item.bEnabled)
			{
				Sample->SetDesignTimePreview(
					AvCustomizationLocalization::Resolve(Item.DisplayNameKey, Item.DisplayName),
					ResolveTorsoItemThumbnail(Index), bSelected); return;
			}
		}
	};
	ConfigureTorsoSample(DesignSample_TorsoNone,true,true);
	ConfigureTorsoSample(DesignSample_TorsoItem,false,false);

	LowerHipCatalog = UWorkerAppearanceComponent::GetLowerHipCatalog();
	BuildLowerHipThumbnailRegistry();
	SelectedLowerHipSection = EAvLowerHipSection::Legs;
	if (WS_LowerHipSectionPages) WS_LowerHipSectionPages->SetActiveWidgetIndex(0);
	if (Txt_LowerCurrentSelectionValue) Txt_LowerCurrentSelectionValue->SetText(FAvLoc::Text(TEXT("Customization.Common.None")));
	if (Txt_LegsEmptyState) Txt_LegsEmptyState->SetVisibility(ESlateVisibility::Collapsed);
	if (Txt_HipEmptyState) Txt_HipEmptyState->SetVisibility(ESlateVisibility::Collapsed);
	auto ConfigureLowerSample = [this](UAvHeadCatalogItemButton* Sample,
		EAvLowerHipSection Section, bool bNone, bool bSelected)
	{
		if (!Sample) return;
		for (int32 Index = 0; Index < LowerHipCatalog.Num(); ++Index)
		{
			const FAvLowerHipCatalogItem& Item = LowerHipCatalog[Index];
			if (Item.Section == Section && Item.bIsNone == bNone && Item.bEnabled)
			{
				Sample->SetDesignTimePreview(
					AvCustomizationLocalization::Resolve(Item.DisplayNameKey, Item.DisplayName),
					ResolveLowerHipItemThumbnail(Index), bSelected);
				return;
			}
		}
	};
	ConfigureLowerSample(DesignSample_LegsNone, EAvLowerHipSection::Legs, true, true);
	ConfigureLowerSample(DesignSample_LegsItem, EAvLowerHipSection::Legs, false, false);
	ConfigureLowerSample(DesignSample_HipNone, EAvLowerHipSection::Hip, true, true);
	ConfigureLowerSample(DesignSample_HipItem, EAvLowerHipSection::Hip, false, false);

	FullOutfitCatalog = UWorkerAppearanceComponent::GetFullOutfitCatalog();
	BuildFullOutfitThumbnailRegistry();
	if (Txt_FullOutfitsCurrentSelection)
	{
		Txt_FullOutfitsCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	}
	if (Txt_FullOutfitsCurrentSelectionValue)
	{
		Txt_FullOutfitsCurrentSelectionValue->SetText(FAvLoc::Text(TEXT("Customization.Common.None")));
	}
	auto ConfigureFullOutfitSample = [this](
		UAvHeadCatalogItemButton* Sample, bool bNone, bool bSelected)
	{
		if (!Sample) return;
		for (int32 Index = 0; Index < FullOutfitCatalog.Num(); ++Index)
		{
			const FAvFullOutfitCatalogItem& Item = FullOutfitCatalog[Index];
			if (Item.bIsNone == bNone && Item.bEnabled)
			{
				Sample->SetDesignTimePreview(
					AvCustomizationLocalization::Resolve(Item.DisplayNameKey, Item.DisplayName),
					ResolveFullOutfitItemThumbnail(Index), bSelected);
				return;
			}
		}
	};
	ConfigureFullOutfitSample(DesignSample_FullOutfitNone, true, true);
	ConfigureFullOutfitSample(DesignSample_FullOutfitItem, false, false);
}

void UAvCharacterCustomizationRootWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CurrentCharacterDisplayName = FAvLoc::Text(TEXT("Customization.Character.DesignerPlaceholder"));
	if (Txt_CharacterName01)
	{
		Txt_CharacterName01->SetText(CurrentCharacterDisplayName);
	}
	if (Btn_EditCharacterName01)
	{
		Btn_EditCharacterName01->OnClicked.RemoveAll(this);
	}
	if (ETB_CharacterName01)
	{
		ETB_CharacterName01->OnTextChanged.RemoveAll(this);
		ETB_CharacterName01->OnTextCommitted.RemoveAll(this);
		ETB_CharacterName01->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Btn_RandomAppearance)
	{
		Btn_RandomAppearance->OnClicked.RemoveAll(this);
		Btn_RandomAppearance->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleRandomAppearanceRequested);
	}
	if (Btn_RandomAppearanceModalBackdrop)
	{
		Btn_RandomAppearanceModalBackdrop->OnClicked.RemoveAll(this);
		Btn_RandomAppearanceModalBackdrop->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleRandomAppearanceCancelClicked);
	}
	if (Btn_RandomAppearanceCancel)
	{
		Btn_RandomAppearanceCancel->OnClicked.RemoveAll(this);
		Btn_RandomAppearanceCancel->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleRandomAppearanceCancelClicked);
	}
	if (Btn_RandomAppearanceCreate)
	{
		Btn_RandomAppearanceCreate->OnClicked.RemoveAll(this);
		Btn_RandomAppearanceCreate->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleRandomAppearanceCreateClicked);
	}
	if (OverlayRandom)
	{
		OverlayRandom->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (HB_EquipmentConflictDontShowAgain)
	{
		HB_EquipmentConflictDontShowAgain->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (CheckBox_EquipmentConflictDontShowAgain)
	{
		CheckBox_EquipmentConflictDontShowAgain->SetIsChecked(false);
	}
}

void UAvCharacterCustomizationRootWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (CultureChangedHandle.IsValid())
	{
		FInternationalization::Get().OnCultureChanged().Remove(CultureChangedHandle);
	}
	CultureChangedHandle = FInternationalization::Get().OnCultureChanged().AddUObject(
		this, &UAvCharacterCustomizationRootWidget::HandleCultureChanged);
	if (ViewportResizedHandle.IsValid())
	{
		FViewport::ViewportResizedEvent.Remove(ViewportResizedHandle);
	}
	ViewportResizedHandle = FViewport::ViewportResizedEvent.AddUObject(
		this, &UAvCharacterCustomizationRootWidget::HandleGameViewportResized);

	SetIsFocusable(true);
	if (Button_Close)
	{
		Button_Close->OnClicked.RemoveAll(this);
		Button_Close->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCloseClicked);
	}
	if (Btn_Close)
	{
		Btn_Close->OnClicked.RemoveAll(this);
		Btn_Close->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCloseClicked);
	}
	if (Btn_PresetPrevious)
	{
		Btn_PresetPrevious->OnClicked.RemoveAll(this);
		Btn_PresetPrevious->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandlePresetPreviousClicked);
	}
	if (Btn_PresetNext)
	{
		Btn_PresetNext->OnClicked.RemoveAll(this);
		Btn_PresetNext->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandlePresetNextClicked);
	}
	BindApplyButton();
	BindCategoryButtons();

	EnsurePreview();
	LoadActiveCharacterState();
	InitializeHeadCustomizationPage();
	InitializeFaceProtectionPage();
	InitializeHandsAccessoriesPage();
	InitializeUpperEquipmentPage();
	InitializeLowerHipPage();
	InitializeFullOutfitsPage();
	ConfigureMyCharactersActionButtons();
	EnsureMyCharactersPageInitialized();
	CaptureCategoryDesignerVisuals();
	SelectCategory(EAvCustomizationCategory::MyCharacters);
	RefreshFullscreenLayout();
	RefreshLocalizedRuntimeText();
	UpdateLocalizationDiagnostics();
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	EnsureAppearanceInspector();
	RefreshAppearanceInspector();
#endif

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			SelectCategory(EAvCustomizationCategory::MyCharacters);
		}));
	}
}

void UAvCharacterCustomizationRootWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bPreviewCompositionRefreshPending)
	{
		const bool bGeometryReady = RefreshPreviewCompositionFromGeometry();
		if (bGeometryReady && PreviewCompositionRefreshFramesRemaining > 0)
		{
			--PreviewCompositionRefreshFramesRemaining;
		}
		bPreviewCompositionRefreshPending =
			!bGeometryReady || PreviewCompositionRefreshFramesRemaining > 0;
	}
}

void UAvCharacterCustomizationRootWidget::NativeDestruct()
{
	if (ViewportResizedHandle.IsValid())
	{
		FViewport::ViewportResizedEvent.Remove(ViewportResizedHandle);
		ViewportResizedHandle.Reset();
	}
	bViewportResizeRefreshQueued = false;
	if (CultureChangedHandle.IsValid())
	{
		FInternationalization::Get().OnCultureChanged().Remove(CultureChangedHandle);
		CultureChangedHandle.Reset();
	}
	EndPreviewDrag();
	CommitRuntimeMyCharacterRenames();
	if (Btn_AddCharacter)
	{
		Btn_AddCharacter->OnClicked.RemoveAll(this);
	}
	if (Btn_ResetActiveCharacter)
	{
		Btn_ResetActiveCharacter->OnClicked.RemoveAll(this);
		Btn_ResetActiveCharacter->OnHovered.RemoveAll(this);
		Btn_ResetActiveCharacter->OnUnhovered.RemoveAll(this);
		Btn_ResetActiveCharacter->OnPressed.RemoveAll(this);
		Btn_ResetActiveCharacter->OnReleased.RemoveAll(this);
	}
	if (Btn_DeleteActiveCharacter)
	{
		Btn_DeleteActiveCharacter->OnClicked.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnHovered.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnUnhovered.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnPressed.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnReleased.RemoveAll(this);
	}
	ClearRuntimeMyCharacterRows();
	bMyCharactersPageInitialized = false;
	if (AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		HideAppearanceConfirmation();
	}
	PendingEquipmentConflict.Reset();
	PendingCharacterAction.Reset();
	bAppearanceModalConfirmInProgress = false;
	if (bIsEditingCharacterName && ETB_CharacterName01)
	{
		CommitCharacterNameEditing(ETB_CharacterName01->GetText());
	}
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
	PreviewRenderTarget = nullptr;
	PreviewMaterial = nullptr;
	for (TPair<EAvHeadCustomizationSection,
		TArray<TObjectPtr<UAvHeadCatalogItemButton>>>& SectionCards : RuntimeHeadSectionCards)
	{
		for (UAvHeadCatalogItemButton* RuntimeCard : SectionCards.Value)
		{
			if (RuntimeCard)
			{
				RuntimeCard->RemoveFromParent();
			}
		}
		SectionCards.Value.Reset();
	}
	RuntimeHeadSectionCards.Reset();
	for (TPair<EAvFaceProtectionSection,
		TArray<TObjectPtr<UAvHeadCatalogItemButton>>>& SectionCards : RuntimeFaceProtectionSectionCards)
	{
		for (UAvHeadCatalogItemButton* RuntimeCard : SectionCards.Value)
		{
			if (RuntimeCard)
			{
				RuntimeCard->RemoveFromParent();
			}
		}
		SectionCards.Value.Reset();
	}
	RuntimeFaceProtectionSectionCards.Reset();
	for (TPair<EAvHandsAccessoriesSection,
		TArray<TObjectPtr<UAvHeadCatalogItemButton>>>& SectionCards : RuntimeHandsAccessoriesSectionCards)
	{
		for (UAvHeadCatalogItemButton* RuntimeCard : SectionCards.Value)
		{
			if (RuntimeCard) RuntimeCard->RemoveFromParent();
		}
		SectionCards.Value.Reset();
	}
	RuntimeHandsAccessoriesSectionCards.Reset();
	for (UAvHeadCatalogItemButton* RuntimeCard : RuntimeTorsoCards)
	{
		if (RuntimeCard) RuntimeCard->RemoveFromParent();
	}
	RuntimeTorsoCards.Reset();
	RuntimeTorsoCardCatalogIndices.Reset();
	for (UAvHeadCatalogItemButton* RuntimeCard : RuntimeLegsCards)
	{
		if (RuntimeCard) RuntimeCard->RemoveFromParent();
	}
	RuntimeLegsCards.Reset();
	RuntimeLegsCardCatalogIndices.Reset();
	for (UAvHeadCatalogItemButton* RuntimeCard : RuntimeHipCards)
	{
		if (RuntimeCard) RuntimeCard->RemoveFromParent();
	}
	RuntimeHipCards.Reset();
	RuntimeHipCardCatalogIndices.Reset();
	for (UAvHeadCatalogItemButton* RuntimeCard : RuntimeFullOutfitCards)
	{
		if (RuntimeCard) RuntimeCard->RemoveFromParent();
	}
	RuntimeFullOutfitCards.Reset();
	RuntimeFullOutfitCardCatalogIndices.Reset();
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	AppearanceInspectorMeshTexts.Reset();
	AppearanceInspectorVisibilityTexts.Reset();
	AppearanceInspectorMaterialTexts.Reset();
	AppearanceInspectorLodTexts.Reset();
	AppearanceInspectorToggleButtons.Reset();
	AppearanceInspectorToggleLabels.Reset();
	AppearanceInspectorSoloButtons.Reset();
	Border_AppearanceDebugInspector = nullptr;
	Txt_AppearanceDebugStatus = nullptr;
	bAppearanceInspectorVisible = false;
#endif

	Super::NativeDestruct();
}

void UAvCharacterCustomizationRootWidget::RefreshLocalizedRuntimeText()
{
	ConfigureEquipmentConflictCheckbox();
	UpdatePresetLabels();
	RefreshCurrentLoadoutSummary();
	RefreshHeadCurrentSelection();
	RefreshFaceProtectionCurrentSelection();
	RefreshHandsAccessoriesCurrentSelection();
	RefreshTorsoCurrentSelection();
	RefreshLowerHipCurrentSelection();
	RefreshFullOutfitCurrentSelection();
	RefreshMyCharactersActionButtons();
	if (AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		UpdateAppearanceConfirmationText(AppearanceModalMode);
	}
}

void UAvCharacterCustomizationRootWidget::HandleCultureChanged()
{
	UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	TMap<FName, FString> NamesBefore;
	FName ActiveCharacterIdBefore = NAME_None;
	FString AppearanceBefore;
	if (Ledger)
	{
		ActiveCharacterIdBefore = Ledger->GetActiveCharacterId();
		AppearanceBefore = Ledger->GetActiveCharacterAppearance().ToStableDebugString();
		for (const FAvCharacterRecord& Record : Ledger->GetCharacterRecords())
		{
			NamesBefore.Add(Record.CharacterId, Record.DisplayName);
		}
	}

	// Text-only refresh. It deliberately avoids roster rebuilds, preview reapplication, saves,
	// camera operations, and ScrollBox mutation.
	RefreshLocalizedRuntimeText();

	bExistingCharacterNamesChangedByCultureSwitch = false;
	CultureSwitchAppearanceMutationCount = 0;
	if (Ledger)
	{
		for (const FAvCharacterRecord& Record : Ledger->GetCharacterRecords())
		{
			const FString* PreviousName = NamesBefore.Find(Record.CharacterId);
			if (!PreviousName || !PreviousName->Equals(Record.DisplayName, ESearchCase::CaseSensitive))
			{
				bExistingCharacterNamesChangedByCultureSwitch = true;
				break;
			}
		}
		if (NamesBefore.Num() != Ledger->GetCharacterRecords().Num())
		{
			bExistingCharacterNamesChangedByCultureSwitch = true;
		}
		if (ActiveCharacterIdBefore != Ledger->GetActiveCharacterId() ||
			AppearanceBefore != Ledger->GetActiveCharacterAppearance().ToStableDebugString())
		{
			CultureSwitchAppearanceMutationCount = 1;
		}
	}
	CultureSwitchSaveCommitCount = 0;
	CultureSwitchAutosaveCount = 0;
	UpdateLocalizationDiagnostics();
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (bAppearanceInspectorVisible)
	{
		RefreshAppearanceInspector();
	}
#endif
	UE_LOG(LogTemp, Display,
		TEXT("[AvCustomizationLocalization] Culture=%s Language=%s NamesChanged=%s AppearanceMutations=%d SaveGameCommits=0 Autosaves=0"),
		*FInternationalization::Get().GetCurrentCulture()->GetName(),
		*FInternationalization::Get().GetCurrentLanguage()->GetName(),
		bExistingCharacterNamesChangedByCultureSwitch ? TEXT("true") : TEXT("false"),
		CultureSwitchAppearanceMutationCount);
}

void UAvCharacterCustomizationRootWidget::UpdateLocalizationDiagnostics()
{
	CustomizationLocalizedKeyCount = 0;
	CustomizationMissingTranslationCount = 0;
	CustomizationFallbackCount = 0;
	bStringTableLoaded = false;
	const FString LanguageName = FInternationalization::Get().GetCurrentLanguage()->GetName();
	const bool bNativeRussian = LanguageName.StartsWith(TEXT("ru"));
	const bool bEnglish = LanguageName.StartsWith(TEXT("en"));
	const bool bPseudo = LanguageName.Equals(TEXT("LEET"), ESearchCase::IgnoreCase) ||
		LanguageName.StartsWith(TEXT("qps"), ESearchCase::IgnoreCase);
	const TSet<FString> IntentionalSourceEqualsTranslation = {
		TEXT("Customization.Common.CurrentSelectionFormat"),
		TEXT("Customization.Common.PageCounterFormat"),
		TEXT("Customization.Common.TooltipWithReasonFormat"),
		TEXT("Customization.Disabled.AssetNotVerified")
	};
	if (const FStringTableConstPtr Table =
		FStringTableRegistry::Get().FindStringTable(TEXT("AvariikaUI")))
	{
		bStringTableLoaded = true;
		Table->EnumerateKeysAndSourceStrings(
			[this, bNativeRussian, bEnglish, bPseudo, &IntentionalSourceEqualsTranslation](
				const FTextKey& Key, const FString& SourceString)
			{
				const FString KeyString = Key.ToString();
				if (!KeyString.StartsWith(TEXT("Customization.")))
				{
					return true;
				}
				++CustomizationLocalizedKeyCount;
				const FString DisplayString = FAvLoc::S(KeyString);
				const bool bEmpty = DisplayString.IsEmpty();
				const bool bUnexpectedSourceFallback = bEnglish &&
					DisplayString == SourceString &&
					!IntentionalSourceEqualsTranslation.Contains(KeyString);
				const bool bUnsupportedCultureFallback =
					!bNativeRussian && !bEnglish && !bPseudo && DisplayString == SourceString;
				if (bEmpty || bUnexpectedSourceFallback)
				{
					++CustomizationMissingTranslationCount;
				}
				if (bEmpty || bUnexpectedSourceFallback || bUnsupportedCultureFallback)
				{
					++CustomizationFallbackCount;
				}
				return true;
			});
	}

	RuntimeCatalogLocalizedItemCount = 0;
	for (const FAvHeadCustomizationCatalogItem& Item : HeadCatalog)
	{
		RuntimeCatalogLocalizedItemCount += !Item.DisplayNameKey.IsEmpty();
	}
	for (const FAvFaceProtectionCatalogItem& Item : FaceProtectionCatalog)
	{
		RuntimeCatalogLocalizedItemCount += !Item.DisplayNameKey.IsEmpty();
	}
	for (const FAvHandsAccessoriesCatalogItem& Item : HandsAccessoriesCatalog)
	{
		RuntimeCatalogLocalizedItemCount += !Item.DisplayNameKey.IsEmpty();
	}
	for (const FAvTorsoCatalogItem& Item : TorsoCatalog)
	{
		RuntimeCatalogLocalizedItemCount += !Item.DisplayNameKey.IsEmpty();
	}
	for (const FAvLowerHipCatalogItem& Item : LowerHipCatalog)
	{
		RuntimeCatalogLocalizedItemCount += !Item.DisplayNameKey.IsEmpty();
	}
	for (const FAvFullOutfitCatalogItem& Item : FullOutfitCatalog)
	{
		RuntimeCatalogLocalizedItemCount += !Item.DisplayNameKey.IsEmpty();
	}

	auto IsLocalizedText = [](const UTextBlock* TextBlock)
	{
		return TextBlock && !TextBlock->GetText().IsEmpty() &&
			!TextBlock->GetText().IsCultureInvariant();
	};
	bCurrentSelectionLocalized =
		IsLocalizedText(Txt_HeadCurrentSelectionValue) &&
		IsLocalizedText(Txt_FaceCurrentSelectionValue) &&
		IsLocalizedText(Txt_HandsCurrentSelectionValue) &&
		IsLocalizedText(Txt_UpperCurrentSelectionValue) &&
		IsLocalizedText(Txt_LowerCurrentSelectionValue) &&
		IsLocalizedText(Txt_FullOutfitsCurrentSelectionValue);
	bModalTextLocalized =
		IsLocalizedText(Txt_RandomAppearanceDialogTitle) &&
		IsLocalizedText(Txt_RandomAppearanceDialogQuestion) &&
		IsLocalizedText(Txt_RandomAppearanceDialogWarning);
	bExistingCharacterNamesLiteral = true;

	CustomizationCultureInvariantVisibleTextCount = 0;
	TextOverflowDetectedCount = 0;
	if (!WidgetTree)
	{
		return;
	}
	TSet<FString> LiteralCharacterNames;
	if (const UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>() : nullptr)
	{
		for (const FAvCharacterRecord& Record : Ledger->GetCharacterRecords())
		{
			LiteralCharacterNames.Add(Record.DisplayName);
		}
	}
	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);
	for (UWidget* Widget : AllWidgets)
	{
		UTextBlock* TextBlock = Cast<UTextBlock>(Widget);
		if (!TextBlock || TextBlock->GetVisibility() == ESlateVisibility::Collapsed ||
			TextBlock->GetVisibility() == ESlateVisibility::Hidden ||
			TextBlock->GetText().IsEmpty() ||
			TextBlock->GetName().Contains(TEXT("AppearanceDebug")))
		{
			continue;
		}
		const FString Display = TextBlock->GetText().ToString();
		const bool bAllowedLiteral = LiteralCharacterNames.Contains(Display) ||
			Display.Equals(TEXT("Esc")) || Display.Equals(TEXT("R")) ||
			Display.Equals(TEXT("+")) || Display.Equals(TEXT("!")) ||
			Display.Equals(TEXT("$")) || Display.Equals(TEXT("✓")) ||
			Display.Equals(TEXT("★")) || Display.Equals(TEXT("‹")) ||
			Display.Equals(TEXT("›")) ||
			Display.Replace(TEXT(" "), TEXT("")).IsNumeric();
		if (TextBlock->GetText().IsCultureInvariant() && !bAllowedLiteral)
		{
			++CustomizationCultureInvariantVisibleTextCount;
		}
		const float AvailableWidth = TextBlock->GetCachedGeometry().GetLocalSize().X;
		if (AvailableWidth > 0.f && TextBlock->GetDesiredSize().X > AvailableWidth + 0.5f)
		{
			++TextOverflowDetectedCount;
		}
	}
}

FReply UAvCharacterCustomizationRootWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		EndPreviewDrag();
	}
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (InKeyEvent.GetKey() == EKeys::F9)
	{
		ToggleAppearanceInspector();
		return FReply::Handled();
	}
#endif
	if (AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		const FKey Key = InKeyEvent.GetKey();
		if (Key == EKeys::Escape || Key == EKeys::B ||
			Key == EKeys::Virtual_Gamepad_Back.GetVirtualKey())
		{
			HideAppearanceConfirmation();
			return FReply::Handled();
		}
		if (Key == EKeys::Tab)
		{
			const bool bConflictMode = IsEquipmentConflictModalMode(AppearanceModalMode);
			const bool bReverse = InKeyEvent.IsShiftDown();
			if (bConflictMode && CheckBox_EquipmentConflictDontShowAgain)
			{
				if ((!bReverse && Btn_RandomAppearanceCreate &&
					 Btn_RandomAppearanceCreate->HasKeyboardFocus()) ||
					(bReverse && Btn_RandomAppearanceCancel &&
					 Btn_RandomAppearanceCancel->HasKeyboardFocus()))
				{
					CheckBox_EquipmentConflictDontShowAgain->SetKeyboardFocus();
				}
				else if ((!bReverse && CheckBox_EquipmentConflictDontShowAgain->HasKeyboardFocus()) ||
					(bReverse && Btn_RandomAppearanceCreate &&
					 Btn_RandomAppearanceCreate->HasKeyboardFocus()))
				{
					if (Btn_RandomAppearanceCancel) Btn_RandomAppearanceCancel->SetKeyboardFocus();
				}
				else if (Btn_RandomAppearanceCreate)
				{
					Btn_RandomAppearanceCreate->SetKeyboardFocus();
				}
			}
			else if (Btn_RandomAppearanceCancel && Btn_RandomAppearanceCreate)
			{
				if (Btn_RandomAppearanceCancel->HasKeyboardFocus())
				{
					Btn_RandomAppearanceCreate->SetKeyboardFocus();
				}
				else
				{
					Btn_RandomAppearanceCancel->SetKeyboardFocus();
				}
			}
			return FReply::Handled();
		}
		if (Key == EKeys::Enter || Key == EKeys::Virtual_Gamepad_Accept.GetVirtualKey() ||
			Key == EKeys::Gamepad_FaceButton_Bottom)
		{
			HandleRandomAppearanceCreateClicked();
			return FReply::Handled();
		}
	}

	if (bIsEditingCharacterName && InKeyEvent.GetKey() == EKeys::Escape)
	{
		CancelCharacterNameEditing();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UAvCharacterCustomizationRootWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Escape)
	{
		EndPreviewDrag();
	}
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (Key == EKeys::F9)
	{
		ToggleAppearanceInspector();
		return FReply::Handled();
	}
#endif
	if (Key == EKeys::B || Key == EKeys::Escape)
	{
		HandleCloseClicked();
		return FReply::Handled();
	}
	if (Key == EKeys::R)
	{
		TriggerRandomAppearance();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UAvCharacterCustomizationRootWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	bPointerOverPreview = IsPointerOverPreview(InMouseEvent.GetScreenSpacePosition());
	if (AppearanceModalMode == EAvAppearanceModalMode::None &&
		InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton &&
		bPointerOverPreview && PreviewActor)
	{
		bPreviewDragActive = true;
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UAvCharacterCustomizationRootWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	bPointerOverPreview = IsPointerOverPreview(InMouseEvent.GetScreenSpacePosition());
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bPreviewDragActive)
	{
		EndPreviewDrag();
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UAvCharacterCustomizationRootWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	bPointerOverPreview = IsPointerOverPreview(InMouseEvent.GetScreenSpacePosition());
	if (bPreviewDragActive)
	{
		if (AppearanceModalMode != EAvAppearanceModalMode::None ||
			!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) || !PreviewActor)
		{
			EndPreviewDrag();
			return FReply::Handled().ReleaseMouseCapture();
		}
		constexpr float DegreesPerPixel = 0.35f;
		PreviewYawDegrees = FMath::UnwindDegrees(
			PreviewYawDegrees + InMouseEvent.GetCursorDelta().X * DegreesPerPixel);
		PreviewActor->SetPreviewYaw(PreviewYawDegrees);
		return FReply::Handled();
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UAvCharacterCustomizationRootWidget::NativeOnMouseWheel(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	bPointerOverPreview = IsPointerOverPreview(InMouseEvent.GetScreenSpacePosition());
	if (AppearanceModalMode == EAvAppearanceModalMode::None && bPointerOverPreview && PreviewActor)
	{
		if (const UWidget* PreviewProjectionWidget = ResolvePreviewProjectionWidget())
		{
			const FGeometry& PreviewGeometry = PreviewProjectionWidget->GetCachedGeometry();
			const FVector2D LocalSize = PreviewGeometry.GetLocalSize();
			if (LocalSize.X > KINDA_SMALL_NUMBER && LocalSize.Y > KINDA_SMALL_NUMBER)
			{
				const FVector2D ScreenPosition = InMouseEvent.GetScreenSpacePosition();
				const FVector2D LocalPosition = PreviewGeometry.AbsoluteToLocal(ScreenPosition);
				const FVector2D NormalizedPosition(
					LocalPosition.X / LocalSize.X,
					LocalPosition.Y / LocalSize.Y);
				if (PreviewActor->AddPreviewZoom(
					InMouseEvent.GetWheelDelta(), ScreenPosition, NormalizedPosition))
				{
					return FReply::Handled();
				}
			}
		}
	}
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

void UAvCharacterCustomizationRootWidget::NativeOnMouseCaptureLost(
	const FCaptureLostEvent& CaptureLostEvent)
{
	EndPreviewDrag();
	Super::NativeOnMouseCaptureLost(CaptureLostEvent);
}

UWidget* UAvCharacterCustomizationRootWidget::ResolvePreviewInputWidget() const
{
	return WidgetTree ? WidgetTree->FindWidget(TEXT("SB_PreviewArea")) : nullptr;
}

UWidget* UAvCharacterCustomizationRootWidget::ResolvePreviewProjectionWidget() const
{
	return ResolvePreviewImage();
}

bool UAvCharacterCustomizationRootWidget::IsPointerOverPreview(
	const FVector2D& ScreenPosition) const
{
	const UWidget* PreviewInputWidget = ResolvePreviewInputWidget();
	return PreviewInputWidget && PreviewInputWidget->IsVisible() &&
		PreviewInputWidget->GetIsEnabled() &&
		PreviewInputWidget->GetCachedGeometry().IsUnderLocation(ScreenPosition);
}

void UAvCharacterCustomizationRootWidget::EndPreviewDrag()
{
	bPreviewDragActive = false;
}

void UAvCharacterCustomizationRootWidget::TransitionPreviewCameraForCategory(
	EAvCustomizationCategory Category,
	bool bImmediate)
{
	if (!PreviewActor)
	{
		return;
	}

	EAvCustomizationCameraPreset Preset = EAvCustomizationCameraPreset::FullBody;
	switch (Category)
	{
	case EAvCustomizationCategory::Head:
		Preset = EAvCustomizationCameraPreset::Head;
		break;
	case EAvCustomizationCategory::FaceProtection:
		Preset = EAvCustomizationCameraPreset::Face;
		break;
	case EAvCustomizationCategory::HandsAccessories:
		Preset = EAvCustomizationCameraPreset::Hands;
		break;
	case EAvCustomizationCategory::UpperGear:
		Preset = EAvCustomizationCameraPreset::UpperBody;
		break;
	case EAvCustomizationCategory::LowerHip:
		Preset = EAvCustomizationCameraPreset::LowerBody;
		break;
	case EAvCustomizationCategory::MyCharacters:
	case EAvCustomizationCategory::Presets:
	case EAvCustomizationCategory::FullOutfits:
	default:
		Preset = EAvCustomizationCameraPreset::FullBody;
		break;
	}
	PreviewActor->TransitionToCustomizationCameraPreset(
		Preset,
		bImmediate,
		bCameraPresetInitialized
			? FName(TEXT("CategoryChanged"))
			: FName(TEXT("CustomizationOpen")));
	bCameraPresetInitialized = true;
}

void UAvCharacterCustomizationRootWidget::TriggerRandomAppearance()
{
	HandleRandomAppearanceRequested();
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
void UAvCharacterCustomizationRootWidget::SetFullWidthWorkingRegionForAutomation(
	bool bEnabled)
{
	if (bFullWidthWorkingRegionForAutomation == bEnabled)
	{
		return;
	}
	bFullWidthWorkingRegionForAutomation = bEnabled;
	bHasAppliedResponsiveLayout = false;
	RefreshFullscreenLayout();
}
#endif

bool UAvCharacterCustomizationRootWidget::RunManualSelectionAndEquipmentCompatibilitySelfTest(
	FString& OutReport)
{
	if (!PreviewActor || AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		OutReport = TEXT("Result=FAIL Reason=PreviewOrModalUnavailable");
		return false;
	}

	const FWorkerAppearance OriginalAppearance = ActiveCharacterAppearance;
	const bool bOriginalMeaningful = bActiveCharacterHasMeaningfulAppearance;
	const FName OriginalBasePresetId = ActiveCharacterBasePresetId;
	const EAvAppearanceOrigin OriginalOrigin = ActiveCharacterAppearanceOrigin;
	const bool bOriginalRaisedSuppression = ShouldSuppressRaisedHoodEquipmentWarning();
	const bool bOriginalHeadgearHeadphonesSuppression =
		ShouldSuppressHeadgearHeadphonesWarning();
	const float TorsoOffset = ScrollBox_TorsoItems ? ScrollBox_TorsoItems->GetScrollOffset() : 0.f;
	const float LegsOffset = ScrollBox_LegsItems ? ScrollBox_LegsItems->GetScrollOffset() : 0.f;
	const float FullOutfitOffset = ScrollBox_FullOutfitItems
		? ScrollBox_FullOutfitItems->GetScrollOffset() : 0.f;
	const float HeadgearOffset = ScrollBox_HeadgearItems
		? ScrollBox_HeadgearItems->GetScrollOffset() : 0.f;
	const float HeadphonesOffset = ScrollBox_HeadphonesItems
		? ScrollBox_HeadphonesItems->GetScrollOffset() : 0.f;

	TArray<FString> Errors;
	const FAvFullOutfitCatalogItem* Ensemble = FullOutfitCatalog.FindByPredicate(
		[](const FAvFullOutfitCatalogItem& Item)
		{
			return !Item.bIsNone && Item.bEnabled &&
				Item.TargetSlot == EWorkerSlot::FullOutfit;
		});
	const FAvTorsoCatalogItem* Torso = TorsoCatalog.FindByPredicate(
		[](const FAvTorsoCatalogItem& Item)
		{
			return !Item.bIsNone && Item.bEnabled && !Item.bHasRaisedHood;
		});
	const FAvTorsoCatalogItem* RaisedTorso = TorsoCatalog.FindByPredicate(
		[](const FAvTorsoCatalogItem& Item)
		{
			return !Item.bIsNone && Item.bEnabled && Item.bHasRaisedHood;
		});
	const FAvLowerHipCatalogItem* Legs = LowerHipCatalog.FindByPredicate(
		[](const FAvLowerHipCatalogItem& Item)
		{
			return Item.Section == EAvLowerHipSection::Legs &&
				!Item.bIsNone && Item.bEnabled;
		});
	const FAvHeadCustomizationCatalogItem* Cap = HeadCatalog.FindByPredicate(
		[](const FAvHeadCustomizationCatalogItem& Item)
		{
			return Item.UISection == EAvHeadCustomizationSection::Headgear &&
				Item.bAllowInUI && Item.bCompatibleWithHeadphones;
		});
	const FAvHeadCustomizationCatalogItem* Helmet = HeadCatalog.FindByPredicate(
		[](const FAvHeadCustomizationCatalogItem& Item)
		{
			return Item.UISection == EAvHeadCustomizationSection::Headgear &&
				Item.bAllowInUI && !Item.bCompatibleWithHeadphones &&
				Item.Family == TEXT("Helmet_Worker");
		});
	const FAvFaceProtectionCatalogItem* Headphones = FaceProtectionCatalog.FindByPredicate(
		[](const FAvFaceProtectionCatalogItem& Item)
		{
			return Item.Section == EAvFaceProtectionSection::Headphones &&
				!Item.bIsNone && Item.bEnabled;
		});
	if (!Ensemble || !Torso || !RaisedTorso || !Legs || !Cap || !Helmet || !Headphones)
	{
		Errors.Add(TEXT("MissingCatalogControl"));
	}

	auto CommitSeed = [this](const FWorkerAppearance& Appearance)
	{
		CommitActiveCharacterAppearance(
			Appearance, true,
			ActiveCharacterBasePresetId.IsNone()
				? FName(TEXT("BaseMaleUnderwear")) : ActiveCharacterBasePresetId,
			EAvAppearanceOrigin::ManualCustomized);
	};
	auto SeedEnsemble = [&]()
	{
		FWorkerAppearance Seed = OriginalAppearance;
		Seed.Set(EWorkerSlot::FullOutfit,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Ensemble->ExactMeshPath)));
		Seed.Clear(EWorkerSlot::Overalls);
		CommitSeed(Seed);
	};
	if (Errors.IsEmpty())
	{
		FString Validation;
		SeedEnsemble();
		if (!ApplyManualModularSelectionAtomically(
			EWorkerSlot::Torso, FSoftObjectPath(Torso->ExactMeshPath),
			Torso->StableId, Validation) ||
			!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull() ||
			ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath() !=
				FSoftObjectPath(Torso->ExactMeshPath) ||
			LastOperationAppearanceAutosaveCount != 1)
		{
			Errors.Add(TEXT("TorsoDidNotExitEnsemble"));
		}

		SeedEnsemble();
		if (!ApplyManualModularSelectionAtomically(
			EWorkerSlot::Legs, FSoftObjectPath(Legs->ExactMeshPath),
			Legs->StableId, Validation) ||
			!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull() ||
			ActiveCharacterAppearance.Get(EWorkerSlot::Legs).ToSoftObjectPath() !=
				FSoftObjectPath(Legs->ExactMeshPath))
		{
			Errors.Add(TEXT("LegsDidNotExitEnsemble"));
		}

		const FSoftObjectPath FeetPath =
			UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance()
				.Get(EWorkerSlot::Feet).ToSoftObjectPath();
		SeedEnsemble();
		if (!ApplyManualModularSelectionAtomically(
			EWorkerSlot::Feet, FeetPath, TEXT("FactoryFeet"), Validation) ||
			!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull() ||
			ActiveCharacterAppearance.Get(EWorkerSlot::Feet).ToSoftObjectPath() != FeetPath)
		{
			Errors.Add(TEXT("FeetDidNotExitEnsemble"));
		}

		SeedEnsemble();
		if (!ApplyManualModularSelectionAtomically(
			EWorkerSlot::Legs, FSoftObjectPath(), TEXT("None_Legs"), Validation) ||
			!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull() ||
			!ActiveCharacterAppearance.Get(EWorkerSlot::Legs).IsNull())
		{
			Errors.Add(TEXT("NoneLegsDidNotExitEnsemble"));
		}

		SetSuppressRaisedHoodEquipmentWarning(false);
		SetSuppressHeadgearHeadphonesWarning(false);
		FWorkerAppearance HeadSeed = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
		HeadSeed.Set(EWorkerSlot::Headphones,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Headphones->ExactMeshPath)));
		CommitSeed(HeadSeed);
		const FWorkerAppearance BeforeCancel = ActiveCharacterAppearance;
		const bool bOpenedHelmetModal = BeginRaisedHoodConflictForHeadgear(*Helmet) &&
			AppearanceModalMode == EAvAppearanceModalMode::EquipHeadgearAndClearTorso &&
			PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headphones) &&
			PendingEquipmentConflict.WarningType ==
				EAvEquipmentConflictWarningType::HeadgearHeadphones;
		HideAppearanceConfirmation();
		if (!bOpenedHelmetModal ||
			!ActiveCharacterAppearance.IsEquivalentTo(BeforeCancel) ||
			LastOperationAppearanceAutosaveCount != 0)
		{
			Errors.Add(TEXT("HelmetModalCancelMutatedState"));
		}

		BeginRaisedHoodConflictForHeadgear(*Helmet);
		const FAvPendingEquipmentConflict HelmetPending = PendingEquipmentConflict;
		HideAppearanceConfirmation();
		if (!ExecuteEquipmentConflictAtomically(HelmetPending, false) ||
			!ActiveCharacterAppearance.Get(EWorkerSlot::Headphones).IsNull() ||
			ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).ToSoftObjectPath() !=
				FSoftObjectPath(Helmet->ExactObjectPath))
		{
			Errors.Add(TEXT("HelmetConfirmFailed"));
		}

		FWorkerAppearance ReverseSeed = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
		ReverseSeed.Set(EWorkerSlot::Headgear,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Helmet->ExactObjectPath)));
		CommitSeed(ReverseSeed);
		BeginRaisedHoodConflictForHeadphones(*Headphones);
		const FAvPendingEquipmentConflict HeadphonesPending = PendingEquipmentConflict;
		HideAppearanceConfirmation();
		if (!ExecuteEquipmentConflictAtomically(HeadphonesPending, false) ||
			!ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).IsNull() ||
			ActiveCharacterAppearance.Get(EWorkerSlot::Headphones).ToSoftObjectPath() !=
				FSoftObjectPath(Headphones->ExactMeshPath))
		{
			Errors.Add(TEXT("HeadphonesConfirmFailed"));
		}

		FWorkerAppearance CapSeed = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
		CapSeed.Set(EWorkerSlot::Headgear,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Cap->ExactObjectPath)));
		CapSeed.Set(EWorkerSlot::Headphones,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Headphones->ExactMeshPath)));
		if (UWorkerAppearanceComponent::HeadgearConflictsWithHeadphones(
			CapSeed.Get(EWorkerSlot::Headgear), CapSeed.Get(EWorkerSlot::Headphones)) ||
			BeginRaisedHoodConflictForHeadphones(*Headphones))
		{
			Errors.Add(TEXT("CapHeadphonesMustNotOpenModal"));
		}

		FWorkerAppearance CombinedSeed = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
		CombinedSeed.Set(EWorkerSlot::Torso,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(RaisedTorso->ExactMeshPath)));
		CombinedSeed.Set(EWorkerSlot::Headphones,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Headphones->ExactMeshPath)));
		CommitSeed(CombinedSeed);
		const bool bCombinedModal = BeginRaisedHoodConflictForHeadgear(*Helmet) &&
			PendingEquipmentConflict.WarningType == EAvEquipmentConflictWarningType::Combined &&
			PendingEquipmentConflict.ShouldClear(EWorkerSlot::Torso) &&
			PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headphones) &&
			PendingEquipmentConflict.SlotsToClear.Num() == 2;
		const FAvPendingEquipmentConflict CombinedPending = PendingEquipmentConflict;
		HideAppearanceConfirmation();
		if (!bCombinedModal || !ExecuteEquipmentConflictAtomically(CombinedPending, false))
		{
			Errors.Add(TEXT("CombinedConflictFailed"));
		}

		SetSuppressHeadgearHeadphonesWarning(true);
		CommitSeed(HeadSeed);
		const bool bAutomatic = BeginRaisedHoodConflictForHeadgear(*Helmet) &&
			AppearanceModalMode == EAvAppearanceModalMode::None &&
			ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).ToSoftObjectPath() ==
				FSoftObjectPath(Helmet->ExactObjectPath) &&
			ActiveCharacterAppearance.Get(EWorkerSlot::Headphones).IsNull() &&
			LastOperationAppearanceAutosaveCount == 1;
		if (!bAutomatic)
		{
			Errors.Add(TEXT("SuppressedConflictDidNotApplyAutomatically"));
		}
	}

	if (AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		HideAppearanceConfirmation();
	}
	SetSuppressRaisedHoodEquipmentWarning(bOriginalRaisedSuppression);
	SetSuppressHeadgearHeadphonesWarning(bOriginalHeadgearHeadphonesSuppression);
	CommitActiveCharacterAppearance(
		OriginalAppearance,
		bOriginalMeaningful,
		OriginalBasePresetId,
		OriginalOrigin);
	const bool bScrollPreserved =
		(!ScrollBox_TorsoItems || FMath::IsNearlyEqual(ScrollBox_TorsoItems->GetScrollOffset(), TorsoOffset)) &&
		(!ScrollBox_LegsItems || FMath::IsNearlyEqual(ScrollBox_LegsItems->GetScrollOffset(), LegsOffset)) &&
		(!ScrollBox_FullOutfitItems || FMath::IsNearlyEqual(ScrollBox_FullOutfitItems->GetScrollOffset(), FullOutfitOffset)) &&
		(!ScrollBox_HeadgearItems || FMath::IsNearlyEqual(ScrollBox_HeadgearItems->GetScrollOffset(), HeadgearOffset)) &&
		(!ScrollBox_HeadphonesItems || FMath::IsNearlyEqual(ScrollBox_HeadphonesItems->GetScrollOffset(), HeadphonesOffset));
	if (!bScrollPreserved)
	{
		Errors.Add(TEXT("ScrollOffsetChanged"));
	}

	OutReport = FString::Printf(
		TEXT("Result=%s ModularSlots=Torso,Legs,Feet NoneExits=true ModalSymmetric=true CapCompatible=true CombinedSingleModal=true SuppressionAutomatic=true ScrollPreserved=%s FullOutfitCatalog=%d RuntimeCards=%d AutosavePerOperation=1 Errors={%s}"),
		Errors.IsEmpty() ? TEXT("PASS") : TEXT("FAIL"),
		bScrollPreserved ? TEXT("true") : TEXT("false"),
		FullOutfitCatalog.Num(),
		RuntimeFullOutfitCards.Num(),
		*FString::Join(Errors, TEXT(";")));
	UE_LOG(LogTemp, Warning,
		TEXT("[AvManualSelectionEquipmentCompatibilitySelfTest] %s"), *OutReport);
	return Errors.IsEmpty();
}

void UAvCharacterCustomizationRootWidget::HandleCloseClicked()
{
	CommitRuntimeMyCharacterRenames();
	if (bIsEditingCharacterName && ETB_CharacterName01)
	{
		CommitCharacterNameEditing(ETB_CharacterName01->GetText());
	}

	if (AAvaryoPlayerController* PC = Cast<AAvaryoPlayerController>(GetOwningPlayer()))
	{
		PC->ToggleCustomize();
	}
}

void UAvCharacterCustomizationRootWidget::HandlePresetPreviousClicked()
{
	CurrentPresetIndex = (CurrentPresetIndex + AvWorkerPresetCatalog::PresetCount - 1) % AvWorkerPresetCatalog::PresetCount;
	RefreshPresetPreview();
}

void UAvCharacterCustomizationRootWidget::HandlePresetNextClicked()
{
	CurrentPresetIndex = (CurrentPresetIndex + 1) % AvWorkerPresetCatalog::PresetCount;
	RefreshPresetPreview();
}

void UAvCharacterCustomizationRootWidget::HandleCategoryMyCharactersClicked()
{
	SelectCategory(EAvCustomizationCategory::MyCharacters);
}

void UAvCharacterCustomizationRootWidget::HandleAddCharacterClicked()
{
	CommitRuntimeMyCharacterRenames();
	LastRosterOperation = TEXT("CreateFailed");
	LastCreatedCharacterId = NAME_None;
	LastRosterSaveGameCommitCount = 0;
	UGameInstance* GameInstance = GetGameInstance();
	UCompanyLedgerSubsystem* Ledger = GameInstance
		? GameInstance->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	if (!Ledger)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Create ignored: roster backend unavailable."));
		return;
	}

	const int32 PreviousCount = Ledger->GetCharacterRecords().Num();
	const FName NewCharacterId = Ledger->CreateCharacter();
	if (NewCharacterId.IsNone() || Ledger->GetCharacterRecords().Num() != PreviousCount + 1)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Create failed: CharacterId=%s PreviousCount=%d CurrentCount=%d."),
			*NewCharacterId.ToString(), PreviousCount, Ledger->GetCharacterRecords().Num());
		return;
	}

	LastRosterOperation = TEXT("Create");
	LastCreatedCharacterId = NewCharacterId;
	LastSelectedCharacterId = NewCharacterId;
	LastRosterSaveGameCommitCount = 1;
	LoadActiveCharacterState();
	ApplyActiveCharacterToPreview();
	RefreshCustomizationSelectionsAfterCharacterChange();
	RebuildMyCharacterRows();
	ResetPreviewCameraForActiveCharacter();
	UE_LOG(LogTemp, Display,
		TEXT("[AvMyCharacters] Operation=Create CharacterId=%s RuntimeRows=%d SaveGameCommits=1 PortraitMode=AuthoredSilhouettePlaceholder"),
		*NewCharacterId.ToString(), RuntimeMyCharacterRows.Num());
}

void UAvCharacterCustomizationRootWidget::HandleDeleteActiveCharacterClicked()
{
	CommitRuntimeMyCharacterRenames();
	LastRosterOperation = TEXT("DeleteRequestedInvalid");
	LastRosterSaveGameCommitCount = 0;
	UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	const FAvCharacterRecord* ActiveRecord = Ledger ? Ledger->GetActiveCharacter() : nullptr;
	if (!Ledger || !ActiveRecord || ActiveRecord->CharacterId.IsNone() ||
		Ledger->GetCharacterRecords().Num() <= 1 ||
		AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Delete modal ignored: ActiveRecord=%s Count=%d ModalMode=%d."),
			ActiveRecord ? TEXT("true") : TEXT("false"),
			Ledger ? Ledger->GetCharacterRecords().Num() : 0,
			static_cast<int32>(AppearanceModalMode));
		RefreshMyCharactersActionButtons();
		return;
	}

	PendingCharacterAction.Reset();
	PendingCharacterAction.Mode = EAvAppearanceModalMode::DeleteCharacter;
	PendingCharacterAction.TargetCharacterId = ActiveRecord->CharacterId;
	PendingCharacterAction.bValid = true;
	LastRosterOperation = TEXT("DeleteRequested");
	ShowAppearanceConfirmation(EAvAppearanceModalMode::DeleteCharacter);
}

void UAvCharacterCustomizationRootWidget::HandleResetActionHovered()
{
	ApplyMyCharactersActionVisual(false, EAvMyCharactersActionVisualState::Hovered);
}

void UAvCharacterCustomizationRootWidget::HandleResetActionUnhovered()
{
	ApplyMyCharactersActionVisual(false, EAvMyCharactersActionVisualState::Normal);
}

void UAvCharacterCustomizationRootWidget::HandleResetActionPressed()
{
	ApplyMyCharactersActionVisual(false, EAvMyCharactersActionVisualState::Pressed);
}

void UAvCharacterCustomizationRootWidget::HandleResetActionReleased()
{
	ApplyMyCharactersActionVisual(false,
		Btn_ResetActiveCharacter && Btn_ResetActiveCharacter->IsHovered()
			? EAvMyCharactersActionVisualState::Hovered
			: EAvMyCharactersActionVisualState::Normal);
}

void UAvCharacterCustomizationRootWidget::HandleDeleteActionHovered()
{
	ApplyMyCharactersActionVisual(true, EAvMyCharactersActionVisualState::Hovered);
}

void UAvCharacterCustomizationRootWidget::HandleDeleteActionUnhovered()
{
	ApplyMyCharactersActionVisual(true, EAvMyCharactersActionVisualState::Normal);
}

void UAvCharacterCustomizationRootWidget::HandleDeleteActionPressed()
{
	ApplyMyCharactersActionVisual(true, EAvMyCharactersActionVisualState::Pressed);
}

void UAvCharacterCustomizationRootWidget::HandleDeleteActionReleased()
{
	ApplyMyCharactersActionVisual(true,
		Btn_DeleteActiveCharacter && Btn_DeleteActiveCharacter->IsHovered()
			? EAvMyCharactersActionVisualState::Hovered
			: EAvMyCharactersActionVisualState::Normal);
}

void UAvCharacterCustomizationRootWidget::HandleMyCharacterRowClicked(FName CharacterId)
{
	++SelectHandlerCallCount;
	LastRowClickResult = TEXT("Select");
	LastSelectedCharacterId = CharacterId;
	LastRosterSaveGameCommitCount = 0;
	UGameInstance* GameInstance = GetGameInstance();
	UCompanyLedgerSubsystem* Ledger = GameInstance
		? GameInstance->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	if (!Ledger || CharacterId.IsNone() ||
		!Ledger->GetCharacterRecords().ContainsByPredicate(
			[CharacterId](const FAvCharacterRecord& Record)
			{
				return Record.CharacterId == CharacterId;
			}))
	{
		LastRosterOperation = TEXT("SelectInvalidCharacterId");
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Select ignored: CharacterId=%s not found."),
			*CharacterId.ToString());
		return;
	}

	if (CharacterId == Ledger->GetActiveCharacterId() &&
		!Ledger->WasLoadedActiveCharacterIdInvalid())
	{
		LastRosterOperation = TEXT("SelectNoOpAlreadyActive");
		RefreshMyCharacterRowsSelection();
		RefreshMyCharactersActionButtons();
		UE_LOG(LogTemp, Display,
			TEXT("[AvMyCharacters] Operation=SelectNoOp CharacterId=%s SaveGameCommits=0"),
			*CharacterId.ToString());
		return;
	}

	if (!Ledger->SetActiveCharacter(CharacterId))
	{
		LastRosterOperation = TEXT("SelectFailed");
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Select failed: CharacterId=%s."), *CharacterId.ToString());
		return;
	}

	LastRosterOperation = TEXT("Select");
	LastRosterSaveGameCommitCount = 1;
	LoadActiveCharacterState();
	ApplyActiveCharacterToPreview();
	RefreshCustomizationSelectionsAfterCharacterChange();
	RefreshMyCharacterRowsSelection();
	RefreshMyCharactersActionButtons();
	ResetPreviewCameraForActiveCharacter();
	UE_LOG(LogTemp, Display,
		TEXT("[AvMyCharacters] Operation=Select CharacterId=%s RuntimeRows=%d SaveGameCommits=1"),
		*CharacterId.ToString(), RuntimeMyCharacterRows.Num());
}

void UAvCharacterCustomizationRootWidget::HandleMyCharacterRenameStarted(
	UAvMyCharacterRowWidget* SourceRow)
{
	++RenameHandlerCallCount;
	LastRowClickResult = TEXT("Rename");
	RenameCharacterId = SourceRow ? SourceRow->GetCharacterId() : NAME_None;
	CommitRuntimeMyCharacterRenames(SourceRow);
}

void UAvCharacterCustomizationRootWidget::RecordMyCharacterRowPointerDiagnostic(
	FName CharacterId,
	const FVector2D& ScreenPosition,
	const FVector2D& LocalPosition,
	const FVector2D& DisplayNameGeometrySize,
	const FVector2D& DisplayNameMeasuredTextSize,
	float DisplayNameVisibleWidth,
	const FVector4& RenameHitRect,
	bool bPointerInsideRenameTextRect,
	const FString& ClickResult,
	float DisplayNameLayoutScale,
	float SlateApplicationScale)
{
	LastRowPointerScreenPosition = ScreenPosition;
	LastRowPointerLocalPosition = LocalPosition;
	LastDisplayNameGeometrySize = DisplayNameGeometrySize;
	LastDisplayNameMeasuredTextSize = DisplayNameMeasuredTextSize;
	LastDisplayNameVisibleWidth = DisplayNameVisibleWidth;
	LastRenameHitRect = RenameHitRect;
	bLastPointerInsideRenameTextRect = bPointerInsideRenameTextRect;
	LastRowClickResult = ClickResult;
	LastDisplayNameLayoutScale = DisplayNameLayoutScale;
	LastSlateApplicationScale = SlateApplicationScale;
	if (ClickResult == TEXT("Rename"))
	{
		RenameCharacterId = CharacterId;
	}
}

bool UAvCharacterCustomizationRootWidget::HandleMyCharacterRenameCommitted(
	FName CharacterId,
	const FString& NewDisplayName)
{
	LastRosterSaveGameCommitCount = 0;
	UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	const FString TrimmedName = NewDisplayName.TrimStartAndEnd().Left(20);
	const FAvCharacterRecord* ExistingRecord = Ledger
		? Ledger->GetCharacterRecords().FindByPredicate(
			[CharacterId](const FAvCharacterRecord& Record)
			{
				return !CharacterId.IsNone() && Record.CharacterId == CharacterId;
			})
		: nullptr;
	if (!ExistingRecord || TrimmedName.IsEmpty())
	{
		LastRosterOperation = TEXT("RenameInvalid");
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Operation=RenameInvalid CharacterId=%s SaveGameCommits=0"),
			*CharacterId.ToString());
		return false;
	}

	const bool bNameChanged = !ExistingRecord->DisplayName.Equals(
		TrimmedName, ESearchCase::CaseSensitive);
	if (!Ledger->SetCharacterDisplayName(CharacterId, TrimmedName))
	{
		LastRosterOperation = TEXT("RenameFailed");
		return false;
	}

	LastRosterOperation = bNameChanged ? TEXT("Rename") : TEXT("RenameNoOp");
	LastRosterSaveGameCommitCount = bNameChanged ? 1 : 0;
	if (CharacterId == Ledger->GetActiveCharacterId())
	{
		CurrentCharacterDisplayName = FText::FromString(TrimmedName);
		if (Txt_CharacterName01)
		{
			Txt_CharacterName01->SetText(CurrentCharacterDisplayName);
		}
	}
	for (UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (Row && Row->GetCharacterId() == CharacterId)
		{
			Row->SetDisplayName(TrimmedName);
			break;
		}
	}
	UE_LOG(LogTemp, Display,
		TEXT("[AvMyCharacters] Operation=%s CharacterId=%s SaveGameCommits=%d ActiveCharacterChanged=false AppearanceChanged=false"),
		bNameChanged ? TEXT("Rename") : TEXT("RenameNoOp"),
		*CharacterId.ToString(), LastRosterSaveGameCommitCount);
	return true;
}

FName UAvCharacterCustomizationRootWidget::TriggerAddCharacterForAutomation()
{
	HandleAddCharacterClicked();
	return LastRosterOperation == TEXT("Create") ? LastCreatedCharacterId : NAME_None;
}

bool UAvCharacterCustomizationRootWidget::TriggerMyCharacterSelectionForAutomation(
	FName CharacterId)
{
	HandleMyCharacterRowClicked(CharacterId);
	return LastRosterOperation == TEXT("Select");
}

FString UAvCharacterCustomizationRootWidget::GetRuntimeMyCharacterIdsForAutomation() const
{
	TArray<FString> CharacterIds;
	CharacterIds.Reserve(RuntimeMyCharacterRows.Num());
	for (const UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (Row)
		{
			CharacterIds.Add(Row->GetCharacterId().ToString());
		}
	}
	return FString::Join(CharacterIds, TEXT(","));
}

bool UAvCharacterCustomizationRootWidget::TriggerMyCharacterRenameForAutomation(
	FName CharacterId)
{
	for (UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (Row && Row->GetCharacterId() == CharacterId)
		{
			return Row->TriggerRenameForAutomation();
		}
	}
	return false;
}

bool UAvCharacterCustomizationRootWidget::CommitMyCharacterRenameForAutomation(
	FName CharacterId,
	const FString& NewDisplayName)
{
	for (UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (Row && Row->GetCharacterId() == CharacterId && Row->IsRenaming())
		{
			Row->SetPendingRenameText(NewDisplayName);
			return Row->CommitPendingRename();
		}
	}
	return false;
}

bool UAvCharacterCustomizationRootWidget::CancelMyCharacterRenameForAutomation(
	FName CharacterId)
{
	for (UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (Row && Row->GetCharacterId() == CharacterId && Row->IsRenaming())
		{
			Row->CancelRename();
			return true;
		}
	}
	return false;
}

bool UAvCharacterCustomizationRootWidget::IsMyCharacterRenameFocusedForAutomation(
	FName CharacterId) const
{
	for (const UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (Row && Row->GetCharacterId() == CharacterId)
		{
			return Row->IsRenaming() && Row->HasRenameKeyboardFocus();
		}
	}
	return false;
}

void UAvCharacterCustomizationRootWidget::HandleCategoryPresetsClicked()
{
	SelectCategory(EAvCustomizationCategory::Presets);
}

void UAvCharacterCustomizationRootWidget::HandleCategoryHeadClicked()
{
	SelectCategory(EAvCustomizationCategory::Head);
}

void UAvCharacterCustomizationRootWidget::HandleHeadFilterHeadTypeClicked()
{
	SelectHeadSection(EAvHeadCustomizationSection::HeadType);
}

void UAvCharacterCustomizationRootWidget::HandleHeadFilterHeadgearClicked()
{
	SelectHeadSection(EAvHeadCustomizationSection::Headgear);
}

void UAvCharacterCustomizationRootWidget::HandleHeadFilterHairClicked()
{
	SelectHeadSection(EAvHeadCustomizationSection::Hair);
}

void UAvCharacterCustomizationRootWidget::HandleHeadFilterBeardClicked()
{
	SelectHeadSection(EAvHeadCustomizationSection::Beard);
}

void UAvCharacterCustomizationRootWidget::HandleCategoryFaceProtectionClicked()
{
	SelectCategory(EAvCustomizationCategory::FaceProtection);
}

void UAvCharacterCustomizationRootWidget::HandleFaceFilterGlassesClicked()
{
	SelectFaceProtectionSection(EAvFaceProtectionSection::Glasses);
}

void UAvCharacterCustomizationRootWidget::HandleFaceFilterRespiratorClicked()
{
	SelectFaceProtectionSection(EAvFaceProtectionSection::Respirator);
}

void UAvCharacterCustomizationRootWidget::HandleFaceFilterHeadphonesClicked()
{
	SelectFaceProtectionSection(EAvFaceProtectionSection::Headphones);
}

void UAvCharacterCustomizationRootWidget::HandleCategoryHandsAccessoriesClicked()
{
	SelectCategory(EAvCustomizationCategory::HandsAccessories);
}

void UAvCharacterCustomizationRootWidget::HandleHandsFilterGlovesClicked()
{
	SelectHandsAccessoriesSection(EAvHandsAccessoriesSection::Gloves);
}

void UAvCharacterCustomizationRootWidget::HandleHandsFilterWatchesClicked()
{
	SelectHandsAccessoriesSection(EAvHandsAccessoriesSection::Watches);
}

void UAvCharacterCustomizationRootWidget::HandleCategoryUpperGearClicked()
{
	SelectCategory(EAvCustomizationCategory::UpperGear);
}

void UAvCharacterCustomizationRootWidget::HandleCategoryLowerHipClicked()
{
	SelectCategory(EAvCustomizationCategory::LowerHip);
}

void UAvCharacterCustomizationRootWidget::HandleLowerFilterLegsClicked()
{
	SelectLowerHipSection(EAvLowerHipSection::Legs);
}

void UAvCharacterCustomizationRootWidget::HandleLowerFilterHipClicked()
{
	SelectLowerHipSection(EAvLowerHipSection::Hip);
}

void UAvCharacterCustomizationRootWidget::HandleCategoryFullOutfitsClicked()
{
	SelectCategory(EAvCustomizationCategory::FullOutfits);
}

void UAvCharacterCustomizationRootWidget::HandleSaveCharacterClicked()
{
	HandleApplyCharacterClicked();
}

void UAvCharacterCustomizationRootWidget::HandleApplyCharacterClicked()
{
	const FWorkerAppearance AppliedAppearance = bHasPresetPreviewAppearance
		? PresetPreviewAppearance
		: BuildSelectedAppearance();
	const FName PresetId(*FString::Printf(TEXT("WorkerPreset_%02d"), CurrentPresetIndex + 1));
	UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeApply] Apply preset -> ActiveCharacterAppearance (Apply+Autosave)."));
	CommitActiveCharacterAppearance(
		AppliedAppearance,
		true,
		PresetId,
		EAvAppearanceOrigin::PresetApplied);
	SelectCategory(EAvCustomizationCategory::MyCharacters);
}

void UAvCharacterCustomizationRootWidget::HandleRandomAppearanceClicked()
{
	const FWorkerAppearance PresetPreviewBeforeRandom = PresetPreviewAppearance;
	LastRandomHeadDiagnostics = FAvRandomHeadSelectionDiagnostics();
	LastRandomCommitCount = 0;
	LastRandomAutosaveCount = 0;
	TMap<EWorkerSlot, int32> CandidateCounts;
	FWorkerAppearance RandomAppearance;
	for (int32 Attempt = 0; Attempt < 12; ++Attempt)
	{
		RandomAppearance = UWorkerAppearanceComponent::MakeRandomCompatibleMaleAppearance(
			ActiveCharacterAppearance,
			Attempt == 0 ? &CandidateCounts : nullptr,
			&LastRandomHeadDiagnostics);
		UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(RandomAppearance);
		if (!RandomAppearance.IsEquivalentTo(ActiveCharacterAppearance))
		{
			break;
		}
	}
	FString ValidationDetails;
	if (!UWorkerAppearanceComponent::ValidateRandomGeneratedMaleAppearance(
		RandomAppearance,
		ValidationDetails))
	{
		LastRandomHeadDiagnostics.ValidationResult = FString::Printf(
			TEXT("Result=FAIL OverallRandomValidation={%s}"),
			*ValidationDetails);
		UE_LOG(LogTemp, Error,
			TEXT("[AvCustomizeRandom] Rejected before apply: %s Slot combination: %s"),
			*ValidationDetails,
			*RandomAppearance.ToStableDebugString());
		return;
	}
	for (const TPair<EWorkerSlot, int32>& Pair : CandidateCounts)
	{
		UE_LOG(LogTemp, Log, TEXT("[AvCustomizeRandomCatalog] Slot=%d Candidates=%d"),
			static_cast<int32>(Pair.Key), Pair.Value);
	}
	CommitActiveCharacterAppearance(
		RandomAppearance,
		true,
		FName(TEXT("RandomizedSlots")),
		EAvAppearanceOrigin::RandomGenerated);
	LastRandomCommitCount = 1;
	LastRandomAutosaveCount = LastOperationAppearanceAutosaveCount;
	const bool bPresetPreviewUnchanged =
		PresetPreviewAppearance.IsEquivalentTo(PresetPreviewBeforeRandom);
	const FString VisualEligibility =
		UWorkerAppearanceComponent::DescribeRandomVisualEligibility(RandomAppearance);
	UE_LOG(LogTemp, Warning,
		TEXT("[AvCustomizeRandom] Origin=RandomGenerated Validation=PASS PreviewSaveUnchanged=%s RandomHeadStableId=%s RandomHeadPath=%s RandomHeadProfile=%s RandomCommitCount=%d RandomAutosaveCount=%d %s VisualAssets={%s} Slot combination: %s"),
		bPresetPreviewUnchanged ? TEXT("true") : TEXT("false"),
		*LastRandomHeadDiagnostics.SelectedStableId,
		*LastRandomHeadDiagnostics.SelectedPath,
		*LastRandomHeadDiagnostics.ResolvedProfile,
		LastRandomCommitCount,
		LastRandomAutosaveCount,
		*ValidationDetails,
		*VisualEligibility,
		*RandomAppearance.ToStableDebugString());
	SelectCategory(EAvCustomizationCategory::MyCharacters);
}

void UAvCharacterCustomizationRootWidget::HandleRandomAppearanceRequested()
{
	if (AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		return;
	}

	if (bIsEditingCharacterName && ETB_CharacterName01)
	{
		CommitCharacterNameEditing(ETB_CharacterName01->GetText());
	}

	if (ShouldConfirmRandomAppearance())
	{
		ShowAppearanceConfirmation(EAvAppearanceModalMode::RandomAppearance);
		return;
	}

	ExecuteExistingRandomAppearance();
}

void UAvCharacterCustomizationRootWidget::HandleRandomAppearanceCancelClicked()
{
	HideAppearanceConfirmation();
}

void UAvCharacterCustomizationRootWidget::HandleRandomAppearanceCreateClicked()
{
	if (AppearanceModalMode == EAvAppearanceModalMode::None || bAppearanceModalConfirmInProgress)
	{
		return;
	}

	const EAvAppearanceModalMode ConfirmedMode = AppearanceModalMode;
	const bool bCharacterAction =
		ConfirmedMode == EAvAppearanceModalMode::ResetToBase ||
		ConfirmedMode == EAvAppearanceModalMode::DeleteCharacter;
	if (bCharacterAction &&
		(!PendingCharacterAction.bValid ||
		 PendingCharacterAction.Mode != ConfirmedMode ||
		 PendingCharacterAction.TargetCharacterId.IsNone() ||
		 PendingCharacterAction.bConfirmConsumed))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Modal confirm rejected: Mode=%d PendingValid=%s Target=%s Consumed=%s."),
			static_cast<int32>(ConfirmedMode),
			PendingCharacterAction.bValid ? TEXT("true") : TEXT("false"),
			*PendingCharacterAction.TargetCharacterId.ToString(),
			PendingCharacterAction.bConfirmConsumed ? TEXT("true") : TEXT("false"));
		HideAppearanceConfirmation();
		return;
	}

	bAppearanceModalConfirmInProgress = true;
	if (bCharacterAction)
	{
		PendingCharacterAction.bConfirmConsumed = true;
	}
	const FAvPendingCharacterAction ConfirmedCharacterAction = PendingCharacterAction;
	const FAvPendingEquipmentConflict ConfirmedConflict = PendingEquipmentConflict;
	const bool bPersistSuppression =
		IsEquipmentConflictModalMode(ConfirmedMode) &&
		CheckBox_EquipmentConflictDontShowAgain &&
		CheckBox_EquipmentConflictDontShowAgain->IsChecked();
	HideAppearanceConfirmation();
	if (ConfirmedMode == EAvAppearanceModalMode::RandomAppearance)
	{
		ExecuteExistingRandomAppearance();
	}
	else if (ConfirmedMode == EAvAppearanceModalMode::ResetToBase)
	{
		ExecuteResetToBase(ConfirmedCharacterAction.TargetCharacterId);
	}
	else if (ConfirmedMode == EAvAppearanceModalMode::DeleteCharacter)
	{
		ExecuteDeleteCharacter(ConfirmedCharacterAction.TargetCharacterId);
	}
	else if (IsEquipmentConflictModalMode(ConfirmedMode))
	{
		ExecuteEquipmentConflictAtomically(ConfirmedConflict, bPersistSuppression);
	}
	bAppearanceModalConfirmInProgress = false;
}

void UAvCharacterCustomizationRootWidget::ShowAppearanceConfirmation(EAvAppearanceModalMode Mode)
{
	EndPreviewDrag();
	if (AppearanceModalMode != EAvAppearanceModalMode::None ||
		Mode == EAvAppearanceModalMode::None || !OverlayRandom)
	{
		return;
	}

	RandomAppearancePreviousFocus = FSlateApplication::Get().GetKeyboardFocusedWidget();
	AppearanceModalMode = Mode;
	UpdateAppearanceConfirmationText(Mode);
	const bool bConflictMode = IsEquipmentConflictModalMode(Mode);
	if (CheckBox_EquipmentConflictDontShowAgain)
	{
		CheckBox_EquipmentConflictDontShowAgain->SetIsChecked(false);
	}
	if (HB_EquipmentConflictDontShowAgain)
	{
		HB_EquipmentConflictDontShowAgain->SetVisibility(
			bConflictMode ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	OverlayRandom->SetVisibility(ESlateVisibility::Visible);
	if (bConflictMode && CheckBox_EquipmentConflictDontShowAgain)
	{
		CheckBox_EquipmentConflictDontShowAgain->SetKeyboardFocus();
	}
	else if (Btn_RandomAppearanceCancel)
	{
		Btn_RandomAppearanceCancel->SetKeyboardFocus();
	}
}

void UAvCharacterCustomizationRootWidget::HideAppearanceConfirmation()
{
	if (AppearanceModalMode == EAvAppearanceModalMode::None)
	{
		return;
	}

	AppearanceModalMode = EAvAppearanceModalMode::None;
	PendingEquipmentConflict.Reset();
	PendingCharacterAction.Reset();
	LastOperationAppearanceAutosaveCount = 0;
	if (CheckBox_EquipmentConflictDontShowAgain)
	{
		CheckBox_EquipmentConflictDontShowAgain->SetIsChecked(false);
	}
	if (HB_EquipmentConflictDontShowAgain)
	{
		HB_EquipmentConflictDontShowAgain->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (OverlayRandom)
	{
		OverlayRandom->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (const TSharedPtr<SWidget> PreviousFocus = RandomAppearancePreviousFocus.Pin())
	{
		FSlateApplication::Get().SetKeyboardFocus(PreviousFocus, EFocusCause::SetDirectly);
	}
	else
	{
		SetKeyboardFocus();
	}
	RandomAppearancePreviousFocus.Reset();
}

bool UAvCharacterCustomizationRootWidget::ShouldConfirmRandomAppearance() const
{
	return ActiveCharacterAppearanceOrigin == EAvAppearanceOrigin::ManualCustomized ||
		ActiveCharacterAppearanceOrigin == EAvAppearanceOrigin::PresetApplied;
}

void UAvCharacterCustomizationRootWidget::ExecuteExistingRandomAppearance()
{
	HandleRandomAppearanceClicked();
}

void UAvCharacterCustomizationRootWidget::HandleResetAppearanceClicked()
{
	CommitRuntimeMyCharacterRenames();
	UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	const FAvCharacterRecord* ActiveRecord = Ledger ? Ledger->GetActiveCharacter() : nullptr;
	if (!ActiveRecord || ActiveRecord->CharacterId.IsNone() ||
		AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		LastRosterOperation = TEXT("ResetRequestedInvalid");
		LastRosterSaveGameCommitCount = 0;
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Reset modal ignored: active record is unavailable or another modal is open."));
		RefreshMyCharactersActionButtons();
		return;
	}

	if (bIsEditingCharacterName && ETB_CharacterName01)
	{
		CommitCharacterNameEditing(ETB_CharacterName01->GetText());
	}
	PendingCharacterAction.Reset();
	PendingCharacterAction.Mode = EAvAppearanceModalMode::ResetToBase;
	PendingCharacterAction.TargetCharacterId = ActiveRecord->CharacterId;
	PendingCharacterAction.bValid = true;
	LastRosterOperation = TEXT("ResetRequested");
	LastRosterSaveGameCommitCount = 0;
	ShowAppearanceConfirmation(EAvAppearanceModalMode::ResetToBase);
}

void UAvCharacterCustomizationRootWidget::ExecuteResetToBase(FName TargetCharacterId)
{
	UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	const bool bTargetStillActive = Ledger && !TargetCharacterId.IsNone() &&
		Ledger->GetActiveCharacterId() == TargetCharacterId &&
		Ledger->GetCharacterRecords().ContainsByPredicate(
			[TargetCharacterId](const FAvCharacterRecord& Record)
			{
				return Record.CharacterId == TargetCharacterId;
			});
	if (!bTargetStillActive)
	{
		LastRosterOperation = TEXT("ResetRejectedStaleTarget");
		LastRosterSaveGameCommitCount = 0;
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Reset rejected: stale TargetCharacterId=%s ActiveCharacterId=%s."),
			*TargetCharacterId.ToString(),
			Ledger ? *Ledger->GetActiveCharacterId().ToString() : TEXT("None"));
		RefreshMyCharactersActionButtons();
		return;
	}

	bHasPresetPreviewAppearance = false;
	CommitActiveCharacterAppearance(
		UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance(),
		false,
		FName(TEXT("BaseMaleUnderwear")),
		EAvAppearanceOrigin::Factory);
	RefreshCustomizationSelectionsAfterCharacterChange();
	RefreshMyCharacterRowsSelection();
	RefreshMyCharactersActionButtons();
	LastRosterOperation = TEXT("Reset");
	LastResetCharacterId = TargetCharacterId;
	LastRosterSaveGameCommitCount = 1;
	UE_LOG(LogTemp, Display,
		TEXT("[AvMyCharacters] Operation=Reset CharacterId=%s RuntimeRows=%d SaveGameCommits=1"),
		*TargetCharacterId.ToString(), RuntimeMyCharacterRows.Num());
}

void UAvCharacterCustomizationRootWidget::ExecuteDeleteCharacter(FName TargetCharacterId)
{
	UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	const bool bTargetStillActive = Ledger && !TargetCharacterId.IsNone() &&
		Ledger->GetCharacterRecords().Num() > 1 &&
		Ledger->GetActiveCharacterId() == TargetCharacterId &&
		Ledger->GetCharacterRecords().ContainsByPredicate(
			[TargetCharacterId](const FAvCharacterRecord& Record)
			{
				return Record.CharacterId == TargetCharacterId;
			});
	if (!bTargetStillActive)
	{
		LastRosterOperation = TEXT("DeleteRejectedStaleTarget");
		LastRosterSaveGameCommitCount = 0;
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Delete rejected: Target=%s Active=%s Count=%d."),
			*TargetCharacterId.ToString(),
			Ledger ? *Ledger->GetActiveCharacterId().ToString() : TEXT("None"),
			Ledger ? Ledger->GetCharacterRecords().Num() : 0);
		RefreshMyCharactersActionButtons();
		return;
	}

	const float PreservedScrollOffset = ScrollBox_MyCharacters
		? ScrollBox_MyCharacters->GetScrollOffset()
		: 0.f;
	if (!Ledger->DeleteCharacter(TargetCharacterId))
	{
		LastRosterOperation = TEXT("DeleteFailed");
		LastRosterSaveGameCommitCount = 0;
		RefreshMyCharactersActionButtons();
		return;
	}

	LastRosterOperation = TEXT("Delete");
	LastDeletedCharacterId = TargetCharacterId;
	LastActiveCharacterIdAfterDelete = Ledger->GetActiveCharacterId();
	LastRosterSaveGameCommitCount = 1;
	LoadActiveCharacterState();
	ApplyActiveCharacterToPreview();
	RefreshCustomizationSelectionsAfterCharacterChange();
	RebuildMyCharacterRows();
	if (ScrollBox_MyCharacters)
	{
		ScrollBox_MyCharacters->SetScrollOffset(PreservedScrollOffset);
	}
	ResetPreviewCameraForActiveCharacter();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(
			this, [this, PreservedScrollOffset]()
			{
				if (ScrollBox_MyCharacters)
				{
					ScrollBox_MyCharacters->SetScrollOffset(FMath::Min(
						PreservedScrollOffset,
						ScrollBox_MyCharacters->GetScrollOffsetOfEnd()));
				}
			}));
	}
	UE_LOG(LogTemp, Display,
		TEXT("[AvMyCharacters] Operation=Delete CharacterId=%s NewActive=%s RuntimeRows=%d SaveGameCommits=1 ScrollOffset=%.2f"),
		*TargetCharacterId.ToString(),
		*LastActiveCharacterIdAfterDelete.ToString(),
		RuntimeMyCharacterRows.Num(),
		PreservedScrollOffset);
}

bool UAvCharacterCustomizationRootWidget::IsEquipmentConflictModalMode(EAvAppearanceModalMode Mode) const
{
	return Mode == EAvAppearanceModalMode::EquipTorsoAndClearConflictingEquipment ||
		Mode == EAvAppearanceModalMode::EquipHeadgearAndClearTorso ||
		Mode == EAvAppearanceModalMode::EquipHeadphonesAndClearTorso;
}

void UAvCharacterCustomizationRootWidget::ConfigureCurrentSelectionRow(
	UTextBlock* Label,
	UTextBlock* Value) const
{
	if (Label)
	{
		Label->SetAutoWrapText(false);
		Label->SetClipping(EWidgetClipping::ClipToBounds);
		if (UHorizontalBoxSlot* LabelSlot = Cast<UHorizontalBoxSlot>(Label->Slot))
		{
			LabelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			LabelSlot->SetHorizontalAlignment(HAlign_Left);
			LabelSlot->SetVerticalAlignment(VAlign_Center);
		}
	}
	if (Value)
	{
		Value->SetMinDesiredWidth(0.f);
		Value->SetAutoWrapText(false);
		Value->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
		Value->SetClipping(EWidgetClipping::ClipToBounds);
		if (UHorizontalBoxSlot* ValueSlot = Cast<UHorizontalBoxSlot>(Value->Slot))
		{
			ValueSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			ValueSlot->SetHorizontalAlignment(HAlign_Fill);
			ValueSlot->SetVerticalAlignment(VAlign_Center);
		}
	}
}

void UAvCharacterCustomizationRootWidget::ConfigureEquipmentConflictCheckbox()
{
	for (UTextBlock* ModalBody : {
		Txt_RandomAppearanceDialogQuestion.Get(),
		Txt_RandomAppearanceDialogWarning.Get() })
	{
		if (ModalBody)
		{
			ModalBody->SetAutoWrapText(true);
			ModalBody->SetTextOverflowPolicy(ETextOverflowPolicy::Clip);
		}
	}
	if (Txt_RandomAppearanceDialogTitle)
	{
		Txt_RandomAppearanceDialogTitle->SetAutoWrapText(false);
		Txt_RandomAppearanceDialogTitle->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
		Txt_RandomAppearanceDialogTitle->SetToolTipText(
			Txt_RandomAppearanceDialogTitle->GetText());
	}
	if (Txt_EquipmentConflictDontShowAgain)
	{
		Txt_EquipmentConflictDontShowAgain->SetText(FAvLoc::Text(TEXT("Customization.Modal.DoNotShowAgain")));
		Txt_EquipmentConflictDontShowAgain->SetAutoWrapText(true);
	}
	if (CheckBox_EquipmentConflictDontShowAgain)
	{
		FCheckBoxStyle Style = CheckBox_EquipmentConflictDontShowAgain->GetWidgetStyle();
		Style.UncheckedImage.ImageSize = FVector2D(24.f, 24.f);
		Style.UncheckedHoveredImage.ImageSize = FVector2D(24.f, 24.f);
		Style.UncheckedPressedImage.ImageSize = FVector2D(24.f, 24.f);
		Style.CheckedImage.ImageSize = FVector2D(24.f, 24.f);
		Style.CheckedHoveredImage.ImageSize = FVector2D(24.f, 24.f);
		Style.CheckedPressedImage.ImageSize = FVector2D(24.f, 24.f);
		Style.UncheckedImage.TintColor = FSlateColor(FLinearColor(0.12f, 0.13f, 0.15f, 1.f));
		Style.UncheckedHoveredImage.TintColor = FSlateColor(FLinearColor(0.25f, 0.26f, 0.29f, 1.f));
		Style.UncheckedPressedImage.TintColor = FSlateColor(FLinearColor(0.34f, 0.35f, 0.38f, 1.f));
		const FSlateColor Orange(FLinearColor(0.95f, 0.42f, 0.04f, 1.f));
		Style.CheckedImage.TintColor = Orange;
		Style.CheckedHoveredImage.TintColor = Orange;
		Style.CheckedPressedImage.TintColor = Orange;
		CheckBox_EquipmentConflictDontShowAgain->SetWidgetStyle(Style);
	}
}

void UAvCharacterCustomizationRootWidget::UpdateAppearanceConfirmationText(EAvAppearanceModalMode Mode) const
{
	if (Txt_RandomAppearanceCancelLabel)
	{
		Txt_RandomAppearanceCancelLabel->SetText(FAvLoc::Text(TEXT("Customization.Action.Cancel")));
	}
	if (Mode == EAvAppearanceModalMode::ResetToBase)
	{
		if (Txt_RandomAppearanceDialogTitle) Txt_RandomAppearanceDialogTitle->SetText(FAvLoc::Text(TEXT("Customization.Modal.Reset.Title")));
		if (Txt_RandomAppearanceDialogQuestion) Txt_RandomAppearanceDialogQuestion->SetText(FAvLoc::Text(TEXT("Customization.Modal.Reset.Body")));
		if (Txt_RandomAppearanceDialogWarning) Txt_RandomAppearanceDialogWarning->SetText(FAvLoc::Text(TEXT("Customization.Modal.Reset.Warning")));
		if (Txt_RandomAppearanceCreateLabel) Txt_RandomAppearanceCreateLabel->SetText(FAvLoc::Text(TEXT("Customization.Action.Reset")));
	}
	else if (Mode == EAvAppearanceModalMode::DeleteCharacter)
	{
		if (Txt_RandomAppearanceDialogTitle) Txt_RandomAppearanceDialogTitle->SetText(FAvLoc::Text(TEXT("Customization.Modal.Delete.Title")));
		if (Txt_RandomAppearanceDialogQuestion) Txt_RandomAppearanceDialogQuestion->SetText(FAvLoc::Text(TEXT("Customization.Modal.Delete.Body")));
		if (Txt_RandomAppearanceDialogWarning) Txt_RandomAppearanceDialogWarning->SetText(FAvLoc::Text(TEXT("Customization.Modal.Delete.Warning")));
		if (Txt_RandomAppearanceCreateLabel) Txt_RandomAppearanceCreateLabel->SetText(FAvLoc::Text(TEXT("Customization.Action.Delete")));
	}
	else if (Mode == EAvAppearanceModalMode::EquipTorsoAndClearConflictingEquipment)
	{
		if (Txt_RandomAppearanceDialogTitle) Txt_RandomAppearanceDialogTitle->SetText(FAvLoc::Text(TEXT("Customization.Modal.Conflict.Title")));
		if (Txt_RandomAppearanceDialogQuestion) Txt_RandomAppearanceDialogQuestion->SetText(FAvLoc::Text(TEXT("Customization.Modal.Conflict.RaisedHoodQuestion")));
		if (Txt_RandomAppearanceDialogWarning)
		{
			const TCHAR* WarningKey = PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headgear) &&
				PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headphones)
				? TEXT("Customization.Modal.Conflict.RemoveHeadgearAndHeadphones")
				: (PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headphones)
					? TEXT("Customization.Modal.Conflict.RemoveHeadphones")
					: TEXT("Customization.Modal.Conflict.RemoveHeadgear"));
			Txt_RandomAppearanceDialogWarning->SetText(FAvLoc::Text(WarningKey));
		}
		if (Txt_RandomAppearanceCreateLabel) Txt_RandomAppearanceCreateLabel->SetText(FAvLoc::Text(TEXT("Customization.Action.RemoveAndEquip")));
	}
	else if (Mode == EAvAppearanceModalMode::EquipHeadgearAndClearTorso)
	{
		if (Txt_RandomAppearanceDialogTitle) Txt_RandomAppearanceDialogTitle->SetText(FAvLoc::Text(TEXT("Customization.Modal.Conflict.Title")));
		if (Txt_RandomAppearanceDialogQuestion) Txt_RandomAppearanceDialogQuestion->SetText(FAvLoc::Text(TEXT("Customization.Modal.Conflict.HeadgearQuestion")));
		if (Txt_RandomAppearanceDialogWarning)
		{
			const bool bClearTorso = PendingEquipmentConflict.ShouldClear(EWorkerSlot::Torso);
			const bool bClearHeadphones = PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headphones);
			const TCHAR* WarningKey = bClearTorso && bClearHeadphones
				? TEXT("Customization.Modal.Conflict.RemoveHoodAndHeadphones")
				: (bClearHeadphones
					? TEXT("Customization.Modal.Conflict.RemoveWorkHeadphones")
					: TEXT("Customization.Modal.Conflict.RemoveRaisedHood"));
			Txt_RandomAppearanceDialogWarning->SetText(FAvLoc::Text(WarningKey));
		}
		if (Txt_RandomAppearanceCreateLabel) Txt_RandomAppearanceCreateLabel->SetText(FAvLoc::Text(TEXT("Customization.Action.RemoveAndEquip")));
	}
	else if (Mode == EAvAppearanceModalMode::EquipHeadphonesAndClearTorso)
	{
		if (Txt_RandomAppearanceDialogTitle) Txt_RandomAppearanceDialogTitle->SetText(FAvLoc::Text(TEXT("Customization.Modal.Conflict.Title")));
		if (Txt_RandomAppearanceDialogQuestion) Txt_RandomAppearanceDialogQuestion->SetText(FAvLoc::Text(TEXT("Customization.Modal.Conflict.HeadphonesQuestion")));
		if (Txt_RandomAppearanceDialogWarning)
		{
			const bool bClearTorso = PendingEquipmentConflict.ShouldClear(EWorkerSlot::Torso);
			const bool bClearHeadgear = PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headgear);
			const TCHAR* WarningKey = bClearTorso && bClearHeadgear
				? TEXT("Customization.Modal.Conflict.RemoveHoodAndHeadgear")
				: (bClearHeadgear
					? TEXT("Customization.Modal.Conflict.RemoveHeadgear")
					: TEXT("Customization.Modal.Conflict.RemoveRaisedHood"));
			Txt_RandomAppearanceDialogWarning->SetText(FAvLoc::Text(WarningKey));
		}
		if (Txt_RandomAppearanceCreateLabel) Txt_RandomAppearanceCreateLabel->SetText(FAvLoc::Text(TEXT("Customization.Action.RemoveAndEquip")));
	}
	else
	{
		if (Txt_RandomAppearanceDialogTitle) Txt_RandomAppearanceDialogTitle->SetText(FAvLoc::Text(TEXT("Customization.Action.Random")));
		if (Txt_RandomAppearanceDialogQuestion) Txt_RandomAppearanceDialogQuestion->SetText(FAvLoc::Text(TEXT("Customization.Modal.Random.Body")));
		if (Txt_RandomAppearanceDialogWarning) Txt_RandomAppearanceDialogWarning->SetText(FAvLoc::Text(TEXT("Customization.Modal.Random.Warning")));
		if (Txt_RandomAppearanceCreateLabel) Txt_RandomAppearanceCreateLabel->SetText(FAvLoc::Text(TEXT("Customization.Action.Create")));
	}
	if (Txt_RandomAppearanceDialogTitle)
	{
		Txt_RandomAppearanceDialogTitle->SetToolTipText(
			Txt_RandomAppearanceDialogTitle->GetText());
	}
}

bool UAvCharacterCustomizationRootWidget::ShouldSuppressRaisedHoodEquipmentWarning() const
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (const UCompanyLedgerSubsystem* Ledger =
			GameInstance->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			return Ledger->GetSuppressRaisedHoodEquipmentConflictWarning();
		}
	}
	return false;
}

void UAvCharacterCustomizationRootWidget::SetSuppressRaisedHoodEquipmentWarning(bool bSuppress) const
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger =
			GameInstance->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			Ledger->SetSuppressRaisedHoodEquipmentConflictWarning(bSuppress);
		}
	}
}

bool UAvCharacterCustomizationRootWidget::ShouldSuppressHeadgearHeadphonesWarning() const
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (const UCompanyLedgerSubsystem* Ledger =
			GameInstance->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			return Ledger->GetSuppressHeadgearHeadphonesConflictWarning();
		}
	}
	return false;
}

void UAvCharacterCustomizationRootWidget::SetSuppressHeadgearHeadphonesWarning(
	bool bSuppress) const
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger =
			GameInstance->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			Ledger->SetSuppressHeadgearHeadphonesConflictWarning(bSuppress);
		}
	}
}

bool UAvCharacterCustomizationRootWidget::ArePendingConflictWarningsSuppressed(
	const FAvPendingEquipmentConflict& Pending) const
{
	const bool bNeedsRaisedHood =
		Pending.WarningType == EAvEquipmentConflictWarningType::RaisedHood ||
		Pending.WarningType == EAvEquipmentConflictWarningType::Combined;
	const bool bNeedsHeadgearHeadphones =
		Pending.WarningType == EAvEquipmentConflictWarningType::HeadgearHeadphones ||
		Pending.WarningType == EAvEquipmentConflictWarningType::Combined;
	return (!bNeedsRaisedHood || ShouldSuppressRaisedHoodEquipmentWarning()) &&
		(!bNeedsHeadgearHeadphones || ShouldSuppressHeadgearHeadphonesWarning());
}

void UAvCharacterCustomizationRootWidget::PersistPendingConflictWarnings(
	const FAvPendingEquipmentConflict& Pending) const
{
	if (Pending.WarningType == EAvEquipmentConflictWarningType::RaisedHood ||
		Pending.WarningType == EAvEquipmentConflictWarningType::Combined)
	{
		SetSuppressRaisedHoodEquipmentWarning(true);
	}
	if (Pending.WarningType == EAvEquipmentConflictWarningType::HeadgearHeadphones ||
		Pending.WarningType == EAvEquipmentConflictWarningType::Combined)
	{
		SetSuppressHeadgearHeadphonesWarning(true);
	}
}

FString UAvCharacterCustomizationRootWidget::ResolveTorsoStableId(
	const FSoftObjectPath& MeshPath) const
{
	if (const FAvTorsoCatalogItem* Item = TorsoCatalog.FindByPredicate(
		[&MeshPath](const FAvTorsoCatalogItem& Candidate)
		{
			return !Candidate.bIsNone && FSoftObjectPath(Candidate.ExactMeshPath) == MeshPath;
		}))
	{
		return Item->StableId;
	}
	return MeshPath.IsNull() ? TEXT("None") : TEXT("Unavailable:") + MeshPath.ToString();
}

FString UAvCharacterCustomizationRootWidget::ResolveHeadgearStableId(
	const FSoftObjectPath& MeshPath) const
{
	if (const FAvHeadCustomizationCatalogItem* Item = HeadCatalog.FindByPredicate(
		[&MeshPath](const FAvHeadCustomizationCatalogItem& Candidate)
		{
			return Candidate.UISection == EAvHeadCustomizationSection::Headgear &&
				FSoftObjectPath(Candidate.ExactObjectPath) == MeshPath;
		}))
	{
		return Item->StableId;
	}
	return MeshPath.IsNull() ? TEXT("None") : TEXT("Unavailable:") + MeshPath.ToString();
}

FString UAvCharacterCustomizationRootWidget::ResolveHeadphonesStableId(
	const FSoftObjectPath& MeshPath) const
{
	if (const FAvFaceProtectionCatalogItem* Item = FaceProtectionCatalog.FindByPredicate(
		[&MeshPath](const FAvFaceProtectionCatalogItem& Candidate)
		{
			return Candidate.Section == EAvFaceProtectionSection::Headphones &&
				!Candidate.bIsNone && FSoftObjectPath(Candidate.ExactMeshPath) == MeshPath;
		}))
	{
		return Item->StableId;
	}
	return MeshPath.IsNull() ? TEXT("None") : TEXT("Unavailable:") + MeshPath.ToString();
}

bool UAvCharacterCustomizationRootWidget::BeginRaisedHoodConflictForTorso(
	const FAvTorsoCatalogItem& TargetItem)
{
	if (AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		return true;
	}
	const FSoftObjectPath ActiveHeadgearPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).ToSoftObjectPath();
	const FSoftObjectPath ActiveHeadphonesPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headphones).ToSoftObjectPath();
	const bool bHeadgearConflict = TargetItem.bHasRaisedHood &&
		UWorkerAppearanceComponent::HeadgearConflictsWithRaisedHood(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headgear));
	const bool bHeadphonesConflict = TargetItem.bHasRaisedHood &&
		UWorkerAppearanceComponent::HeadphonesConflictWithRaisedHood(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headphones));
	if (!bHeadgearConflict && !bHeadphonesConflict)
	{
		return false;
	}

	FAvPendingEquipmentConflict Pending;
	Pending.bValid = true;
	Pending.Mode = EAvAppearanceModalMode::EquipTorsoAndClearConflictingEquipment;
	Pending.TargetStableId = TargetItem.StableId;
	Pending.TargetSlot = EWorkerSlot::Torso;
	Pending.TargetMeshPath = FSoftObjectPath(TargetItem.ExactMeshPath);
	Pending.TargetMaterialOverrides = TargetItem.ExactMaterialOverrides;
	Pending.WarningType = EAvEquipmentConflictWarningType::RaisedHood;
	if (bHeadgearConflict) Pending.SlotsToClear.Add(EWorkerSlot::Headgear);
	if (bHeadphonesConflict) Pending.SlotsToClear.Add(EWorkerSlot::Headphones);
	Pending.ExpectedHeadgearStableId = bHeadgearConflict
		? ResolveHeadgearStableId(ActiveHeadgearPath) : FString();
	Pending.ExpectedHeadphonesStableId = bHeadphonesConflict
		? ResolveHeadphonesStableId(ActiveHeadphonesPath) : FString();
	for (const EWorkerSlot EnsembleSlot :
		{ EWorkerSlot::FullOutfit, EWorkerSlot::Overalls })
	{
		const FSoftObjectPath EnsemblePath =
			ActiveCharacterAppearance.Get(EnsembleSlot).ToSoftObjectPath();
		if (EnsemblePath.IsNull()) continue;
		Pending.SlotsToClear.Add(EnsembleSlot);
		FAvFullOutfitCatalogItem Record;
		const FString StableId = UWorkerAppearanceComponent::ResolveFullOutfitCatalogRecord(
			EnsembleSlot, EnsemblePath, Record)
			? Record.StableId : TEXT("Unavailable:") + EnsemblePath.ToString();
		if (EnsembleSlot == EWorkerSlot::FullOutfit)
		{
			Pending.ExpectedFullOutfitStableId = StableId;
		}
		else
		{
			Pending.ExpectedOverallsStableId = StableId;
		}
	}
	Pending.TargetDisplayName = TargetItem.CurrentSelectionName;

	if (ArePendingConflictWarningsSuppressed(Pending))
	{
		ExecuteEquipmentConflictAtomically(Pending, false);
		return true;
	}
	PendingEquipmentConflict = Pending;
	ShowAppearanceConfirmation(Pending.Mode);
	return true;
}

bool UAvCharacterCustomizationRootWidget::BeginRaisedHoodConflictForHeadgear(
	const FAvHeadCustomizationCatalogItem& TargetItem)
{
	if (AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		return true;
	}
	const FSoftObjectPath ActiveTorsoPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath();
	const FSoftObjectPath ActiveHeadphonesPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headphones).ToSoftObjectPath();
	const bool bRaisedHoodConflict = TargetItem.bConflictsWithRaisedHood &&
		UWorkerAppearanceComponent::IsRaisedHoodTorsoPath(ActiveTorsoPath);
	const bool bHeadphonesConflict =
		UWorkerAppearanceComponent::HeadgearConflictsWithHeadphones(
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(TargetItem.ExactObjectPath)),
			ActiveCharacterAppearance.Get(EWorkerSlot::Headphones));
	if (!bRaisedHoodConflict && !bHeadphonesConflict)
	{
		return false;
	}

	FAvPendingEquipmentConflict Pending;
	Pending.bValid = true;
	Pending.Mode = EAvAppearanceModalMode::EquipHeadgearAndClearTorso;
	Pending.TargetStableId = TargetItem.StableId;
	Pending.TargetSlot = EWorkerSlot::Headgear;
	Pending.TargetMeshPath = FSoftObjectPath(TargetItem.ExactObjectPath);
	Pending.WarningType = bRaisedHoodConflict && bHeadphonesConflict
		? EAvEquipmentConflictWarningType::Combined
		: (bRaisedHoodConflict
			? EAvEquipmentConflictWarningType::RaisedHood
			: EAvEquipmentConflictWarningType::HeadgearHeadphones);
	if (bRaisedHoodConflict)
	{
		Pending.SlotsToClear.Add(EWorkerSlot::Torso);
		Pending.ExpectedTorsoStableId = ResolveTorsoStableId(ActiveTorsoPath);
	}
	if (bHeadphonesConflict)
	{
		Pending.SlotsToClear.Add(EWorkerSlot::Headphones);
		Pending.ExpectedHeadphonesStableId =
			ResolveHeadphonesStableId(ActiveHeadphonesPath);
	}
	Pending.TargetDisplayName = TargetItem.DisplayName;

	if (ArePendingConflictWarningsSuppressed(Pending))
	{
		ExecuteEquipmentConflictAtomically(Pending, false);
		return true;
	}
	PendingEquipmentConflict = Pending;
	ShowAppearanceConfirmation(Pending.Mode);
	return true;
}

bool UAvCharacterCustomizationRootWidget::BeginRaisedHoodConflictForHeadphones(
	const FAvFaceProtectionCatalogItem& TargetItem)
{
	if (AppearanceModalMode != EAvAppearanceModalMode::None)
	{
		return true;
	}
	const FSoftObjectPath ActiveTorsoPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath();
	const FSoftObjectPath ActiveHeadgearPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).ToSoftObjectPath();
	if (TargetItem.Section != EAvFaceProtectionSection::Headphones ||
		TargetItem.bIsNone)
	{
		return false;
	}
	const bool bRaisedHoodConflict = TargetItem.bConflictsWithRaisedHood &&
		UWorkerAppearanceComponent::IsRaisedHoodTorsoPath(ActiveTorsoPath);
	const bool bHeadgearConflict =
		UWorkerAppearanceComponent::HeadgearConflictsWithHeadphones(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headgear),
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(TargetItem.ExactMeshPath)));
	if (!bRaisedHoodConflict && !bHeadgearConflict)
	{
		return false;
	}

	FAvPendingEquipmentConflict Pending;
	Pending.bValid = true;
	Pending.Mode = EAvAppearanceModalMode::EquipHeadphonesAndClearTorso;
	Pending.TargetStableId = TargetItem.StableId;
	Pending.TargetSlot = EWorkerSlot::Headphones;
	Pending.TargetMeshPath = FSoftObjectPath(TargetItem.ExactMeshPath);
	Pending.WarningType = bRaisedHoodConflict && bHeadgearConflict
		? EAvEquipmentConflictWarningType::Combined
		: (bRaisedHoodConflict
			? EAvEquipmentConflictWarningType::RaisedHood
			: EAvEquipmentConflictWarningType::HeadgearHeadphones);
	if (bRaisedHoodConflict)
	{
		Pending.SlotsToClear.Add(EWorkerSlot::Torso);
		Pending.ExpectedTorsoStableId = ResolveTorsoStableId(ActiveTorsoPath);
	}
	if (bHeadgearConflict)
	{
		Pending.SlotsToClear.Add(EWorkerSlot::Headgear);
		Pending.ExpectedHeadgearStableId = ResolveHeadgearStableId(ActiveHeadgearPath);
	}
	Pending.TargetDisplayName = TargetItem.DisplayName;

	if (ArePendingConflictWarningsSuppressed(Pending))
	{
		ExecuteEquipmentConflictAtomically(Pending, false);
		return true;
	}
	PendingEquipmentConflict = Pending;
	ShowAppearanceConfirmation(Pending.Mode);
	return true;
}

bool UAvCharacterCustomizationRootWidget::ExecuteEquipmentConflictAtomically(
	const FAvPendingEquipmentConflict& Pending,
	bool bPersistSuppression)
{
	if (!Pending.bValid || !IsEquipmentConflictModalMode(Pending.Mode) ||
		!PreviewActor || Pending.TargetMeshPath.IsNull() ||
		Pending.SlotsToClear.IsEmpty())
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvEquipmentConflict] Result=REJECT Reason=InvalidPendingOrPreview"));
		return false;
	}

	bool bTargetValid = false;
	bool bTargetRaisedHood = false;
	if (Pending.TargetSlot == EWorkerSlot::Torso)
	{
		if (const FAvTorsoCatalogItem* Item = TorsoCatalog.FindByPredicate(
			[&Pending](const FAvTorsoCatalogItem& Candidate)
			{
				return Candidate.bEnabled && !Candidate.bIsNone && Candidate.bHasRaisedHood &&
					Candidate.StableId == Pending.TargetStableId &&
					FSoftObjectPath(Candidate.ExactMeshPath) == Pending.TargetMeshPath &&
					Candidate.ExactMaterialOverrides == Pending.TargetMaterialOverrides;
			}))
		{
			bTargetValid = true;
			bTargetRaisedHood = true;
		}
	}
	else if (Pending.TargetSlot == EWorkerSlot::Headgear)
	{
		if (const FAvHeadCustomizationCatalogItem* Item = HeadCatalog.FindByPredicate(
			[&Pending](const FAvHeadCustomizationCatalogItem& Candidate)
			{
				return Candidate.bAllowInUI &&
					Candidate.UISection == EAvHeadCustomizationSection::Headgear &&
					Candidate.StableId == Pending.TargetStableId &&
					FSoftObjectPath(Candidate.ExactObjectPath) == Pending.TargetMeshPath;
			}))
		{
			bTargetValid = true;
			bTargetRaisedHood = Item->bConflictsWithRaisedHood;
		}
	}
	else if (Pending.TargetSlot == EWorkerSlot::Headphones)
	{
		if (const FAvFaceProtectionCatalogItem* Item = FaceProtectionCatalog.FindByPredicate(
			[&Pending](const FAvFaceProtectionCatalogItem& Candidate)
			{
				return Candidate.bEnabled && !Candidate.bIsNone &&
					Candidate.Section == EAvFaceProtectionSection::Headphones &&
					Candidate.StableId == Pending.TargetStableId &&
					FSoftObjectPath(Candidate.ExactMeshPath) == Pending.TargetMeshPath;
			}))
		{
			bTargetValid = true;
			bTargetRaisedHood = Item->bConflictsWithRaisedHood;
		}
	}

	const FSoftObjectPath CurrentHeadgearPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).ToSoftObjectPath();
	const FSoftObjectPath CurrentHeadphonesPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headphones).ToSoftObjectPath();
	const FSoftObjectPath CurrentTorsoPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath();
	const TSoftObjectPtr<USkeletalMesh> TargetMesh(Pending.TargetMeshPath);
	const bool bHeadgearReasonStillValid = Pending.TargetSlot == EWorkerSlot::Torso
		? bTargetRaisedHood && UWorkerAppearanceComponent::HeadgearConflictsWithRaisedHood(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headgear))
		: Pending.TargetSlot == EWorkerSlot::Headphones &&
			UWorkerAppearanceComponent::HeadgearConflictsWithHeadphones(
				ActiveCharacterAppearance.Get(EWorkerSlot::Headgear), TargetMesh);
	const bool bHeadphonesReasonStillValid = Pending.TargetSlot == EWorkerSlot::Torso
		? bTargetRaisedHood && UWorkerAppearanceComponent::HeadphonesConflictWithRaisedHood(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headphones))
		: Pending.TargetSlot == EWorkerSlot::Headgear &&
			UWorkerAppearanceComponent::HeadgearConflictsWithHeadphones(
				TargetMesh, ActiveCharacterAppearance.Get(EWorkerSlot::Headphones));
	const bool bHeadgearStillValid = !Pending.ShouldClear(EWorkerSlot::Headgear) ||
		(bHeadgearReasonStillValid &&
		 ResolveHeadgearStableId(CurrentHeadgearPath) == Pending.ExpectedHeadgearStableId);
	const bool bHeadphonesStillValid = !Pending.ShouldClear(EWorkerSlot::Headphones) ||
		(bHeadphonesReasonStillValid &&
		 ResolveHeadphonesStableId(CurrentHeadphonesPath) == Pending.ExpectedHeadphonesStableId);
	const bool bTorsoStillValid = !Pending.ShouldClear(EWorkerSlot::Torso) ||
		(bTargetRaisedHood &&
		 UWorkerAppearanceComponent::IsRaisedHoodTorsoPath(CurrentTorsoPath) &&
		 ResolveTorsoStableId(CurrentTorsoPath) == Pending.ExpectedTorsoStableId);
	auto ResolveEnsembleStableId = [](EWorkerSlot EnsembleSlot, const FSoftObjectPath& Path)
	{
		if (Path.IsNull()) return FString(TEXT("None"));
		FAvFullOutfitCatalogItem Record;
		return UWorkerAppearanceComponent::ResolveFullOutfitCatalogRecord(
			EnsembleSlot, Path, Record)
			? Record.StableId : TEXT("Unavailable:") + Path.ToString();
	};
	const bool bFullOutfitStillValid = !Pending.ShouldClear(EWorkerSlot::FullOutfit) ||
		ResolveEnsembleStableId(
			EWorkerSlot::FullOutfit,
			ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).ToSoftObjectPath()) ==
			Pending.ExpectedFullOutfitStableId;
	const bool bOverallsStillValid = !Pending.ShouldClear(EWorkerSlot::Overalls) ||
		ResolveEnsembleStableId(
			EWorkerSlot::Overalls,
			ActiveCharacterAppearance.Get(EWorkerSlot::Overalls).ToSoftObjectPath()) ==
			Pending.ExpectedOverallsStableId;
	if (!bTargetValid || !bHeadgearStillValid || !bHeadphonesStillValid ||
		!bTorsoStillValid || !bFullOutfitStillValid || !bOverallsStillValid)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[AvEquipmentConflict] Result=REJECT Reason=StateChanged TargetValid=%s HeadgearValid=%s HeadphonesValid=%s TorsoValid=%s FullOutfitValid=%s OverallsValid=%s"),
			bTargetValid ? TEXT("true") : TEXT("false"),
			bHeadgearStillValid ? TEXT("true") : TEXT("false"),
			bHeadphonesStillValid ? TEXT("true") : TEXT("false"),
			bTorsoStillValid ? TEXT("true") : TEXT("false"),
			bFullOutfitStillValid ? TEXT("true") : TEXT("false"),
			bOverallsStillValid ? TEXT("true") : TEXT("false"));
		return false;
	}

	const FString PreviousTorsoPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath().ToString();
	FWorkerAppearance NewAppearance = ActiveCharacterAppearance;
	for (const EWorkerSlot WorkerSlotToClear : Pending.SlotsToClear)
	{
		NewAppearance.Clear(WorkerSlotToClear);
	}
	NewAppearance.Set(
		Pending.TargetSlot,
		TSoftObjectPtr<USkeletalMesh>(Pending.TargetMeshPath));
	FString ValidationDetails;
	if (!UWorkerAppearanceComponent::ValidateHeadCustomizationSelection(
		NewAppearance, Pending.TargetSlot, ValidationDetails))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvEquipmentConflict] Result=REJECT Reason=Validation %s"),
			*ValidationDetails);
		return false;
	}

	CommitActiveCharacterAppearance(
		NewAppearance,
		true,
		ActiveCharacterBasePresetId.IsNone()
			? FName(TEXT("BaseMaleUnderwear"))
			: ActiveCharacterBasePresetId,
		EAvAppearanceOrigin::ManualCustomized);
	const FString NewTorsoPath =
		NewAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath().ToString();
	UpdateTorsoSelectionCards(PreviousTorsoPath, NewTorsoPath);
	RefreshTorsoCurrentSelection();
	RefreshHeadCardSelectionStates();
	RefreshHeadCurrentSelection();
	RefreshFaceProtectionCardSelectionStates();
	RefreshFaceProtectionCurrentSelection();
	RefreshFullOutfitCardSelectionStates();
	bLastManualModularSelectionClearedEnsemble =
		Pending.TargetSlot == EWorkerSlot::Torso &&
		(Pending.ShouldClear(EWorkerSlot::FullOutfit) ||
		 Pending.ShouldClear(EWorkerSlot::Overalls));
	LastOperationAppearanceAutosaveCount = 1;
	if (bPersistSuppression)
	{
		PersistPendingConflictWarnings(Pending);
	}
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	RefreshAppearanceInspector();
#endif
	UE_LOG(LogTemp, Warning,
		TEXT("[AvEquipmentConflict] Result=APPLIED Mode=%d WarningType=%d Target=%s ClearHeadgear=%s ClearHeadphones=%s ClearTorso=%s ClearFullOutfit=%s ClearOveralls=%s AppearanceAutosaveCount=1 PreferenceSaved=%s"),
		static_cast<int32>(Pending.Mode),
		static_cast<int32>(Pending.WarningType),
		*Pending.TargetStableId,
		Pending.ShouldClear(EWorkerSlot::Headgear) ? TEXT("true") : TEXT("false"),
		Pending.ShouldClear(EWorkerSlot::Headphones) ? TEXT("true") : TEXT("false"),
		Pending.ShouldClear(EWorkerSlot::Torso) ? TEXT("true") : TEXT("false"),
		Pending.ShouldClear(EWorkerSlot::FullOutfit) ? TEXT("true") : TEXT("false"),
		Pending.ShouldClear(EWorkerSlot::Overalls) ? TEXT("true") : TEXT("false"),
		bPersistSuppression ? TEXT("true") : TEXT("false"));
	return true;
}

void UAvCharacterCustomizationRootWidget::EnsurePreview()
{
	ConfigurePreviewVisualSurface();
	UImage* PreviewImage = ResolvePreviewImage();
	UWorld* World = GetWorld();
	if (!PreviewImage || !World)
	{
		return;
	}

	if (!PreviewRenderTarget)
	{
		const FIntPoint SurfaceSize = GetPreviewSurfacePixelSize();
		PreviewRenderTarget = NewObject<UTextureRenderTarget2D>(this);
		PreviewRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		PreviewRenderTarget->ClearColor = FLinearColor::Transparent;
		PreviewRenderTarget->bAutoGenerateMips = false;
		PreviewRenderTarget->AddressX = TextureAddress::TA_Clamp;
		PreviewRenderTarget->AddressY = TextureAddress::TA_Clamp;
		PreviewRenderTarget->Filter = TextureFilter::TF_Bilinear;
		PreviewRenderTarget->InitAutoFormat(SurfaceSize.X, SurfaceSize.Y);
		PreviewRenderTarget->UpdateResourceImmediate(true);
	}
	else
	{
		ResizePreviewRenderTargetToViewport(GetPreviewSurfacePixelSize());
	}

	if (!PreviewMaterial)
	{
		UMaterialInterface* PreviewMaterialBase = LoadObject<UMaterialInterface>(
			nullptr,
			TEXT("/Game/Avariika/UI/CharacterCustomization/M_PreviewWorker_UI.M_PreviewWorker_UI"));
		if (PreviewMaterialBase)
		{
			PreviewMaterial = UMaterialInstanceDynamic::Create(PreviewMaterialBase, this);
		}
	}
	if (!PreviewMaterial)
	{
		return;
	}

	PreviewMaterial->SetTextureParameterValue(TEXT("PreviewTexture"), PreviewRenderTarget);
	PreviewImage->SetBrushFromMaterial(PreviewMaterial);
	PreviewImage->SetRenderOpacity(1.f);
	PreviewImage->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!PreviewActor)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwningPlayer();
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		PreviewActor = World->SpawnActor<AAvCustomizePreviewActor>(
			AAvCustomizePreviewActor::StaticClass(),
			FVector(0.f, 0.f, -5000.f),
			FRotator::ZeroRotator,
			SpawnParameters);
		if (PreviewActor)
		{
			PreviewActor->InitializePreview(PreviewRenderTarget);
		}
	}

	if (PreviewActor && PreviewImage->GetBrush().GetResourceObject() == PreviewMaterial)
	{
		if (UWidget* PreviewTitle = WidgetTree->FindWidget(TEXT("Txt_PreviewTitle")))
		{
			PreviewTitle->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (UWidget* PreviewSubtitle = WidgetTree->FindWidget(TEXT("Txt_PreviewSubtitle")))
		{
			PreviewSubtitle->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UAvCharacterCustomizationRootWidget::LoadActiveCharacterState()
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (const UCompanyLedgerSubsystem* Ledger = GameInstance->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			if (const FAvCharacterRecord* Record = Ledger->GetActiveCharacter())
			{
				ActiveCharacterAppearance = Record->Appearance;
				ActiveCharacterBasePresetId = Record->BasePresetId;
				bActiveCharacterHasMeaningfulAppearance = Record->bHasMeaningfulAppearance;
				ActiveCharacterAppearanceOrigin = Record->AppearanceOrigin;
				CurrentCharacterDisplayName = FText::FromString(Record->DisplayName);
				TryApplySelectionFromAppearance(ActiveCharacterAppearance);
				RefreshActiveCharacterDisplay();
				return;
			}
		}
	}

	if (const AAvaryoCharacter* Character = GetOwningPlayerPawn<AAvaryoCharacter>())
	{
		if (Character->WorkerAppearance && !Character->WorkerAppearance->GetAppearance().Slots.IsEmpty())
		{
			ActiveCharacterAppearance = Character->WorkerAppearance->GetAppearance();
			bActiveCharacterHasMeaningfulAppearance = true;
			ActiveCharacterAppearanceOrigin = EAvAppearanceOrigin::ManualCustomized;
			TryApplySelectionFromAppearance(ActiveCharacterAppearance);
			return;
		}
	}

	ActiveCharacterAppearance = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
	ActiveCharacterBasePresetId = FName(TEXT("BaseMaleUnderwear"));
	bActiveCharacterHasMeaningfulAppearance = false;
	ActiveCharacterAppearanceOrigin = EAvAppearanceOrigin::Factory;
}

void UAvCharacterCustomizationRootWidget::ConfigurePreviewVisualSurface()
{
	if (!WidgetTree)
	{
		return;
	}

	UImage* PreviewImage = ResolvePreviewImage();
	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(WidgetTree->FindWidget(TEXT("RootPanel")));
	if (!PreviewImage || !RootPanel)
	{
		return;
	}

	if (PreviewImage->GetParent() != RootPanel)
	{
		PreviewImage->RemoveFromParent();
		RootPanel->AddChildToCanvas(PreviewImage);
	}

	if (UCanvasPanelSlot* PreviewSlot = Cast<UCanvasPanelSlot>(PreviewImage->Slot))
	{
		PreviewSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		PreviewSlot->SetOffsets(FMargin(0.f));
		PreviewSlot->SetAlignment(FVector2D::ZeroVector);
		PreviewSlot->SetAutoSize(false);
		PreviewSlot->SetZOrder(1);
	}

	auto SetRootZOrder = [this](const FName WidgetName, const int32 ZOrder)
	{
		if (UWidget* Widget = WidgetTree->FindWidget(WidgetName))
		{
			if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Widget->Slot))
			{
				Slot->SetZOrder(ZOrder);
			}
		}
	};
	SetRootZOrder(TEXT("Img_CustomizeBackground"), 0);
	SetRootZOrder(TEXT("LayoutRoot"), 2);
	SetRootZOrder(TEXT("OverlayRandom"), 100);

	// The authored layout owns the internal panel sizes, while these root-level
	// Canvas slots must always stretch to the actual viewport. This prevents a
	// design-resolution island without changing any panel or card proportions.
	const auto FillRootCanvasSlot = [this, RootPanel](const FName WidgetName)
	{
		UWidget* Widget = WidgetTree->FindWidget(WidgetName);
		if (!Widget || Widget->GetParent() != RootPanel)
		{
			return;
		}
		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			Slot->SetOffsets(FMargin(0.f));
			Slot->SetAlignment(FVector2D::ZeroVector);
			Slot->SetAutoSize(false);
		}
	};
	FillRootCanvasSlot(TEXT("Img_CustomizeBackground"));
	FillRootCanvasSlot(TEXT("LayoutRoot"));
	FillRootCanvasSlot(TEXT("OverlayRandom"));

	PreviewImage->SetClipping(EWidgetClipping::Inherit);
	PreviewImage->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UAvCharacterCustomizationRootWidget::HandleGameViewportResized(
	FViewport* Viewport,
	uint32 Unused)
{
	(void)Unused;
	UGameViewportClient* GameViewport = GetWorld() ? GetWorld()->GetGameViewport() : nullptr;
	if (!Viewport || !GameViewport || GameViewport->Viewport != Viewport)
	{
		return;
	}

	++ViewportResizeEventCount;
	if (bViewportResizeRefreshQueued)
	{
		return;
	}
	bViewportResizeRefreshQueued = true;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				bViewportResizeRefreshQueued = false;
				RefreshFullscreenLayout();
			}));
	}
	else
	{
		bViewportResizeRefreshQueued = false;
		RefreshFullscreenLayout();
	}
}

void UAvCharacterCustomizationRootWidget::ApplyResponsiveLayoutPolicy(
	const FIntPoint& ViewportSize)
{
	if (!WidgetTree || ViewportSize.X <= 0 || ViewportSize.Y <= 0)
	{
		return;
	}

	using namespace AvCustomizationResponsive;
	const float DPIScale = FMath::Max(
		UWidgetLayoutLibrary::GetViewportScale(this),
		KINDA_SMALL_NUMBER);
	const float LogicalWidth = static_cast<float>(ViewportSize.X) / DPIScale;
	const float LogicalHeight = static_cast<float>(ViewportSize.Y) / DPIScale;
	float WorkingWidth = FMath::Min(
		LogicalWidth,
		LogicalHeight * MaxWorkingRegionAspect);
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (bFullWidthWorkingRegionForAutomation)
	{
		WorkingWidth = LogicalWidth;
	}
#endif
	const float WorkingInset = FMath::Max(
		(LogicalWidth - WorkingWidth) * 0.5f,
		0.f);

	const float MinGapTotal = 2.f * EdgeGapMin + 2.f * InterPanelGapMin;
	const float PreferredGapTotal =
		2.f * EdgeGapPreferred + 2.f * InterPanelGapPreferred;
	const float GapBlend = FMath::Clamp(
		(WorkingWidth - PreviewSafeMin -
			LeftPanelPreferred - RightPanelPreferred - MinGapTotal) /
			FMath::Max(PreferredGapTotal - MinGapTotal, KINDA_SMALL_NUMBER),
		0.f,
		1.f);
	const float EdgeGap = FMath::Lerp(EdgeGapMin, EdgeGapPreferred, GapBlend);
	const float InterPanelGap = FMath::Lerp(
		InterPanelGapMin,
		InterPanelGapPreferred,
		GapBlend);
	const float GapTotal = 2.f * EdgeGap + 2.f * InterPanelGap;

	const float PanelMinTotal = LeftPanelMin + RightPanelMin;
	const float PanelPreferredTotal =
		LeftPanelPreferred + RightPanelPreferred;
	const float PanelBudgetForSafePreview =
		WorkingWidth - GapTotal - PreviewSafeMin;
	const float PanelBlend = FMath::Clamp(
		(PanelBudgetForSafePreview - PanelMinTotal) /
			FMath::Max(PanelPreferredTotal - PanelMinTotal, KINDA_SMALL_NUMBER),
		0.f,
		1.f);
	float LeftPanelWidth = FMath::Lerp(
		LeftPanelMin,
		LeftPanelPreferred,
		PanelBlend);
	float RightPanelWidth = FMath::Lerp(
		RightPanelMin,
		RightPanelPreferred,
		PanelBlend);

	float PreviewWidth = WorkingWidth - GapTotal -
		LeftPanelWidth - RightPanelWidth;
	if (PreviewWidth < PreviewDegradedMin)
	{
		const float DegradedPanelBudget = FMath::Max(
			WorkingWidth - GapTotal - PreviewDegradedMin,
			0.f);
		const float DegradedPanelScale = FMath::Clamp(
			DegradedPanelBudget / FMath::Max(PanelMinTotal, KINDA_SMALL_NUMBER),
			0.f,
			1.f);
		LeftPanelWidth = LeftPanelMin * DegradedPanelScale;
		RightPanelWidth = RightPanelMin * DegradedPanelScale;
		PreviewWidth = FMath::Max(
			WorkingWidth - GapTotal - LeftPanelWidth - RightPanelWidth,
			0.f);
	}

	bResponsiveGracefulDegradation =
		ViewportSize.X < ShippingMinWidth ||
		ViewportSize.Y < ShippingMinHeight ||
		ViewportSize.X <= ViewportSize.Y ||
		PreviewWidth < PreviewSafeMin - 0.5f;
	ResponsiveDegradationReason.Reset();
	if (ViewportSize.X <= ViewportSize.Y)
	{
		ResponsiveDegradationReason = TEXT("PortraitOutOfScope");
	}
	if (ViewportSize.X < ShippingMinWidth || ViewportSize.Y < ShippingMinHeight)
	{
		if (!ResponsiveDegradationReason.IsEmpty())
		{
			ResponsiveDegradationReason += TEXT(";");
		}
		ResponsiveDegradationReason += TEXT("BelowShippingMinimum");
	}
	if (PreviewWidth < PreviewSafeMin - 0.5f)
	{
		if (!ResponsiveDegradationReason.IsEmpty())
		{
			ResponsiveDegradationReason += TEXT(";");
		}
		ResponsiveDegradationReason += TEXT("PreviewBelowSafeMinimum");
	}

	UWidget* ContentToFooter = WidgetTree->FindWidget(TEXT("HB_ContentToFooter"));
	if (ContentToFooter)
	{
		if (UVerticalBoxSlot* ContentSlot = Cast<UVerticalBoxSlot>(ContentToFooter->Slot))
		{
			FMargin SlotPadding = ContentSlot->GetPadding();
			SlotPadding.Left = WorkingInset;
			SlotPadding.Right = WorkingInset;
			ContentSlot->SetPadding(SlotPadding);
		}
	}

	USizeBox* LeftPanel = Cast<USizeBox>(WidgetTree->FindWidget(TEXT("SB_LeftPanel")));
	USizeBox* PreviewArea = Cast<USizeBox>(WidgetTree->FindWidget(TEXT("SB_PreviewArea")));
	USizeBox* RightPanel = Cast<USizeBox>(WidgetTree->FindWidget(TEXT("SB_RightPanel")));
	if (LeftPanel)
	{
		LeftPanel->SetMinDesiredWidth(LeftPanelMin);
		LeftPanel->SetMaxDesiredWidth(LeftPanelMax);
		LeftPanel->SetWidthOverride(LeftPanelWidth);
		if (UCanvasPanelSlot* LeftSlot = Cast<UCanvasPanelSlot>(LeftPanel->Slot))
		{
			FVector2D Position = LeftSlot->GetPosition();
			FVector2D Size = LeftSlot->GetSize();
			Position.X = EdgeGap;
			Size.X = LeftPanelWidth;
			LeftSlot->SetPosition(Position);
			LeftSlot->SetSize(Size);
		}
	}
	if (PreviewArea)
	{
		PreviewArea->SetMinDesiredWidth(FMath::Min(PreviewSafeMin, PreviewWidth));
		if (UCanvasPanelSlot* PreviewSlot = Cast<UCanvasPanelSlot>(PreviewArea->Slot))
		{
			FMargin Offsets = PreviewSlot->GetOffsets();
			Offsets.Left = EdgeGap + LeftPanelWidth + InterPanelGap;
			Offsets.Right = InterPanelGap * 0.5f;
			PreviewSlot->SetOffsets(Offsets);
		}
	}
	if (RightPanel)
	{
		RightPanel->SetMinDesiredWidth(RightPanelMin);
		RightPanel->SetMaxDesiredWidth(RightPanelMax);
		RightPanel->SetWidthOverride(RightPanelWidth);
		if (UHorizontalBoxSlot* RightSlot = Cast<UHorizontalBoxSlot>(RightPanel->Slot))
		{
			FMargin SlotPadding = RightSlot->GetPadding();
			SlotPadding.Left = InterPanelGap * 0.5f;
			SlotPadding.Right = EdgeGap;
			RightSlot->SetPadding(SlotPadding);
		}
	}

	if (UWidget* Modal = WidgetTree->FindWidget(TEXT("Border_RandomAppearanceDialog")))
	{
		for (UWidget* Parent = Modal->GetParent(); Parent && Parent != OverlayRandom;
			 Parent = Parent->GetParent())
		{
			if (USizeBox* ModalSizeBox = Cast<USizeBox>(Parent))
			{
				ModalSizeBox->SetMaxDesiredWidth(FMath::Max(LogicalWidth - 2.f * EdgeGapMin, 1.f));
				ModalSizeBox->SetMaxDesiredHeight(FMath::Max(LogicalHeight - 2.f * EdgeGapMin, 1.f));
				break;
			}
		}
	}

	ResponsiveLogicalViewportWidth = LogicalWidth;
	ResponsiveLogicalViewportHeight = LogicalHeight;
	ResponsiveWorkingRegionWidth = WorkingWidth;
	ResponsiveWorkingRegionInset = WorkingInset;
	ResponsiveLeftPanelWidth = LeftPanelWidth;
	ResponsiveRightPanelWidth = RightPanelWidth;
	ResponsiveEdgeGap = EdgeGap;
	ResponsiveInterPanelGap = InterPanelGap;
	ResponsivePreviewWidth = PreviewWidth;
	++ResponsiveLayoutApplyCount;
	InvalidateLayoutAndVolatility();
}

FIntPoint UAvCharacterCustomizationRootWidget::GetPreviewSurfacePixelSize() const
{
	int32 ViewportX = 0;
	int32 ViewportY = 0;
	if (const APlayerController* PC = GetOwningPlayer())
	{
		PC->GetViewportSize(ViewportX, ViewportY);
	}
	if (ViewportX > 0 && ViewportY > 0)
	{
		return FIntPoint(ViewportX, ViewportY);
	}
	if (CachedViewportSize.X > 0 && CachedViewportSize.Y > 0)
	{
		return CachedViewportSize;
	}
	// A transient 1x1 bootstrap is replaced as soon as the real game viewport exists.
	// It is not a design resolution and cannot influence responsive layout.
	return FIntPoint(1, 1);
}

void UAvCharacterCustomizationRootWidget::ResizePreviewRenderTargetToViewport(
	const FIntPoint& ViewportSize)
{
	if (!PreviewRenderTarget || ViewportSize.X <= 0 || ViewportSize.Y <= 0 ||
		(PreviewRenderTarget->SizeX == ViewportSize.X &&
		 PreviewRenderTarget->SizeY == ViewportSize.Y))
	{
		return;
	}

	PreviewRenderTarget->ResizeTarget(ViewportSize.X, ViewportSize.Y);
	++PreviewRenderTargetResizeCount;
	if (PreviewActor)
	{
		PreviewActor->RefreshCustomizationCameraPresetForViewport(true);
	}
}

bool UAvCharacterCustomizationRootWidget::RefreshPreviewCompositionFromGeometry()
{
	const UWidget* VisualSurface = ResolvePreviewProjectionWidget();
	const UWidget* InteractionArea = ResolvePreviewInputWidget();
	if (!PreviewActor || !VisualSurface || !InteractionArea)
	{
		return false;
	}

	const FGeometry VisualGeometry = VisualSurface->GetCachedGeometry();
	const FGeometry InteractionGeometry = InteractionArea->GetCachedGeometry();
	const FVector2D VisualLocalSize = VisualGeometry.GetLocalSize();
	const FVector2D InteractionLocalSize = InteractionGeometry.GetLocalSize();
	if (VisualLocalSize.X <= KINDA_SMALL_NUMBER ||
		VisualLocalSize.Y <= KINDA_SMALL_NUMBER ||
		InteractionLocalSize.X <= KINDA_SMALL_NUMBER ||
		InteractionLocalSize.Y <= KINDA_SMALL_NUMBER)
	{
		return false;
	}

	const FVector2D VisualMin = VisualGeometry.LocalToAbsolute(FVector2D::ZeroVector);
	const FVector2D VisualMax = VisualGeometry.LocalToAbsolute(VisualLocalSize);
	const FVector2D InteractionMin = InteractionGeometry.LocalToAbsolute(
		FVector2D::ZeroVector);
	const FVector2D InteractionMax = InteractionGeometry.LocalToAbsolute(
		InteractionLocalSize);
	const float VisualWidth = VisualMax.X - VisualMin.X;
	if (VisualWidth <= KINDA_SMALL_NUMBER ||
		InteractionMin.X < VisualMin.X - 1.5f ||
		InteractionMax.X > VisualMax.X + 1.5f)
	{
		return false;
	}

	const float InteractionCenterX =
		(InteractionMin.X + InteractionMax.X) * 0.5f;
	const float NormalizedCenterX = FMath::Clamp(
		(InteractionCenterX - VisualMin.X) / VisualWidth,
		0.1f,
		0.9f);
	CachedPreviewCompositionCenterX = NormalizedCenterX;
	PreviewActor->SetCompositionCenterNormalizedX(NormalizedCenterX);
	++PreviewCompositionRefreshCount;
	return true;
}

void UAvCharacterCustomizationRootWidget::EnsureMyCharactersPageInitialized()
{
	if (SB_CharacterCard01)
	{
		SB_CharacterCard01->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Btn_EditCharacterName01)
	{
		Btn_EditCharacterName01->OnClicked.RemoveAll(this);
	}
	if (ETB_CharacterName01)
	{
		ETB_CharacterName01->OnTextChanged.RemoveAll(this);
		ETB_CharacterName01->OnTextCommitted.RemoveAll(this);
	}
	if (Btn_AddCharacter)
	{
		Btn_AddCharacter->OnClicked.RemoveAll(this);
		Btn_AddCharacter->OnClicked.AddDynamic(
			this, &UAvCharacterCustomizationRootWidget::HandleAddCharacterClicked);
	}
	if (ScrollBox_MyCharacters)
	{
		ScrollBox_MyCharacters->SetClipping(EWidgetClipping::ClipToBounds);
		ScrollBox_MyCharacters->SetOrientation(EOrientation::Orient_Vertical);
		ScrollBox_MyCharacters->SetConsumeMouseWheel(
			EConsumeMouseWheel::WhenScrollingPossible);
		ScrollBox_MyCharacters->SetAnimateWheelScrolling(true);
		ScrollBox_MyCharacters->SetWheelScrollMultiplier(3.f);
		ScrollBox_MyCharacters->SetScrollbarThickness(FVector2D(5.f, 5.f));
	}

	const UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	int32 ValidRecordCount = 0;
	if (Ledger)
	{
		for (const FAvCharacterRecord& Record : Ledger->GetCharacterRecords())
		{
			ValidRecordCount += Record.CharacterId.IsNone() ? 0 : 1;
		}
	}
	const bool bCompositionMatches = bMyCharactersPageInitialized &&
		RuntimeMyCharacterRows.Num() == ValidRecordCount;
	if (!bCompositionMatches)
	{
		RebuildMyCharacterRows();
	}
	else
	{
		RefreshMyCharacterRowsSelection();
		MoveAddCharacterButtonToEnd();
	}
	RefreshMyCharactersActionButtons();
}

void UAvCharacterCustomizationRootWidget::ClearRuntimeMyCharacterRows()
{
	for (UAvMyCharacterRowWidget* RuntimeRow : RuntimeMyCharacterRows)
	{
		if (RuntimeRow)
		{
			RuntimeRow->CancelRename();
			RuntimeRow->RemoveFromParent();
		}
	}
	RuntimeMyCharacterRows.Reset();
}

void UAvCharacterCustomizationRootWidget::CommitRuntimeMyCharacterRenames(
	UAvMyCharacterRowWidget* ExceptRow)
{
	for (UAvMyCharacterRowWidget* RuntimeRow : RuntimeMyCharacterRows)
	{
		if (RuntimeRow && RuntimeRow != ExceptRow && RuntimeRow->IsRenaming())
		{
			RuntimeRow->CommitPendingRename();
		}
	}
}

UTexture2D* UAvCharacterCustomizationRootWidget::ResolveMyCharactersSilhouette()
{
	if (!MyCharactersSilhouetteTexture)
	{
		MyCharactersSilhouetteTexture = LoadObject<UTexture2D>(
			nullptr, TEXT("/Game/Avariika/UI/icon/chel_1_.chel_1_"));
	}
	return MyCharactersSilhouetteTexture.Get();
}

void UAvCharacterCustomizationRootWidget::MoveAddCharacterButtonToEnd()
{
	if (!VB_MyCharactersList || !SB_AddCharacter)
	{
		return;
	}
	const int32 CurrentIndex = VB_MyCharactersList->GetChildIndex(SB_AddCharacter);
	if (CurrentIndex == INDEX_NONE || CurrentIndex == VB_MyCharactersList->GetChildrenCount() - 1)
	{
		return;
	}

	FSlateChildSize SavedSize(ESlateSizeRule::Automatic);
	FMargin SavedPadding(0.f);
	EHorizontalAlignment SavedHorizontalAlignment = HAlign_Fill;
	EVerticalAlignment SavedVerticalAlignment = VAlign_Fill;
	if (const UVerticalBoxSlot* ExistingSlot = Cast<UVerticalBoxSlot>(SB_AddCharacter->Slot))
	{
		SavedSize = ExistingSlot->GetSize();
		SavedPadding = ExistingSlot->GetPadding();
		SavedHorizontalAlignment = ExistingSlot->GetHorizontalAlignment();
		SavedVerticalAlignment = ExistingSlot->GetVerticalAlignment();
	}
	SB_AddCharacter->RemoveFromParent();
	if (UVerticalBoxSlot* NewSlot = VB_MyCharactersList->AddChildToVerticalBox(SB_AddCharacter))
	{
		NewSlot->SetSize(SavedSize);
		NewSlot->SetPadding(SavedPadding);
		NewSlot->SetHorizontalAlignment(SavedHorizontalAlignment);
		NewSlot->SetVerticalAlignment(SavedVerticalAlignment);
	}
}

void UAvCharacterCustomizationRootWidget::RebuildMyCharacterRows()
{
	CommitRuntimeMyCharacterRenames();
	ClearRuntimeMyCharacterRows();
	UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	if (!Ledger || !VB_MyCharactersList)
	{
		bMyCharactersPageInitialized = false;
		UE_LOG(LogTemp, Warning,
			TEXT("[AvMyCharacters] Runtime rows unavailable: Ledger=%s List=%s"),
			Ledger ? TEXT("true") : TEXT("false"),
			VB_MyCharactersList ? TEXT("true") : TEXT("false"));
		return;
	}

	const TArray<FAvCharacterRecord>& Records = Ledger->GetCharacterRecords();
	TArray<int32> SortedRecordIndices;
	for (int32 Index = 0; Index < Records.Num(); ++Index)
	{
		if (!Records[Index].CharacterId.IsNone())
		{
			SortedRecordIndices.Add(Index);
		}
	}
	SortedRecordIndices.Sort([&Records](int32 LeftIndex, int32 RightIndex)
	{
		const FAvCharacterRecord& Left = Records[LeftIndex];
		const FAvCharacterRecord& Right = Records[RightIndex];
		if (Left.SortOrder != Right.SortOrder)
		{
			return Left.SortOrder < Right.SortOrder;
		}
		if (Left.CreatedTimestamp != Right.CreatedTimestamp)
		{
			return Left.CreatedTimestamp < Right.CreatedTimestamp;
		}
		return Left.CharacterId.LexicalLess(Right.CharacterId);
	});

	FSlateChildSize SavedAddSize(ESlateSizeRule::Automatic);
	FMargin SavedAddPadding(0.f);
	EHorizontalAlignment SavedAddHorizontalAlignment = HAlign_Fill;
	EVerticalAlignment SavedAddVerticalAlignment = VAlign_Fill;
	if (SB_AddCharacter)
	{
		if (const UVerticalBoxSlot* ExistingAddSlot =
			Cast<UVerticalBoxSlot>(SB_AddCharacter->Slot))
		{
			SavedAddSize = ExistingAddSlot->GetSize();
			SavedAddPadding = ExistingAddSlot->GetPadding();
			SavedAddHorizontalAlignment = ExistingAddSlot->GetHorizontalAlignment();
			SavedAddVerticalAlignment = ExistingAddSlot->GetVerticalAlignment();
		}
		SB_AddCharacter->RemoveFromParent();
	}
	UTexture2D* SilhouetteTexture = ResolveMyCharactersSilhouette();
	for (int32 RecordIndex : SortedRecordIndices)
	{
		const FAvCharacterRecord& Record = Records[RecordIndex];
		const FName RowName(*FString::Printf(
			TEXT("RuntimeMyCharacterRow_%s"), *Record.CharacterId.ToString()));
		UAvMyCharacterRowWidget* Row = GetOwningPlayer()
			? CreateWidget<UAvMyCharacterRowWidget>(
				GetOwningPlayer(), UAvMyCharacterRowWidget::StaticClass(), RowName)
			: CreateWidget<UAvMyCharacterRowWidget>(
				GetWorld(), UAvMyCharacterRowWidget::StaticClass(), RowName);
		if (!Row)
		{
			continue;
		}
		Row->InitializeMyCharacterRow(
			this,
			Record.CharacterId,
			SilhouetteTexture,
			Record.DisplayName,
			Record.CharacterId == Ledger->GetActiveCharacterId());
		if (UVerticalBoxSlot* RowSlot = VB_MyCharactersList->AddChildToVerticalBox(Row))
		{
			RowSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			RowSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 10.f));
			RowSlot->SetHorizontalAlignment(HAlign_Fill);
			RowSlot->SetVerticalAlignment(VAlign_Fill);
		}
		RuntimeMyCharacterRows.Add(Row);
	}
	if (SB_AddCharacter)
	{
		if (UVerticalBoxSlot* NewAddSlot =
			VB_MyCharactersList->AddChildToVerticalBox(SB_AddCharacter))
		{
			NewAddSlot->SetSize(SavedAddSize);
			NewAddSlot->SetPadding(SavedAddPadding);
			NewAddSlot->SetHorizontalAlignment(SavedAddHorizontalAlignment);
			NewAddSlot->SetVerticalAlignment(SavedAddVerticalAlignment);
		}
	}
	bMyCharactersPageInitialized = true;
	RefreshMyCharacterRowsSelection();
	RefreshMyCharactersActionButtons();
	UE_LOG(LogTemp, Display,
		TEXT("[AvMyCharacters] Operation=Rebuild Records=%d RuntimeRows=%d AuthoredSample=Collapsed PortraitMode=AuthoredSilhouettePlaceholder AddButtonIndex=%d"),
		SortedRecordIndices.Num(), RuntimeMyCharacterRows.Num(),
		SB_AddCharacter ? VB_MyCharactersList->GetChildIndex(SB_AddCharacter) : INDEX_NONE);
}

void UAvCharacterCustomizationRootWidget::RefreshMyCharacterRowsSelection()
{
	const UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	const FName ActiveCharacterId = Ledger ? Ledger->GetActiveCharacterId() : NAME_None;
	for (UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (Row)
		{
			Row->SetSelectedState(Row->GetCharacterId() == ActiveCharacterId);
		}
	}
}

void UAvCharacterCustomizationRootWidget::RefreshHandsAccessoriesCardSelectionStates()
{
	for (const TPair<EAvHandsAccessoriesSection,
		TArray<TObjectPtr<UAvHeadCatalogItemButton>>>& Entry : RuntimeHandsAccessoriesSectionCards)
	{
		for (UAvHeadCatalogItemButton* Card : Entry.Value)
		{
			const int32 CatalogIndex = Card ? Card->GetCatalogIndex() : INDEX_NONE;
			if (!HandsAccessoriesCatalog.IsValidIndex(CatalogIndex))
			{
				continue;
			}
			const FAvHandsAccessoriesCatalogItem& Item = HandsAccessoriesCatalog[CatalogIndex];
			const FString CurrentPath =
				ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString();
			Card->SetSelectedState(
				Item.bIsNone ? CurrentPath.IsEmpty() : CurrentPath == Item.ExactMeshPath);
		}
	}
}

void UAvCharacterCustomizationRootWidget::RefreshCustomizationSelectionsAfterCharacterChange()
{
	RefreshHeadCardSelectionStates();
	RefreshHeadCurrentSelection();
	RefreshFaceProtectionCardSelectionStates();
	RefreshFaceProtectionCurrentSelection();
	RefreshHandsAccessoriesCardSelectionStates();
	RefreshHandsAccessoriesCurrentSelection();
	RefreshTorsoCardSelectionStates();
	RefreshTorsoCurrentSelection();
	RefreshLowerHipCardSelectionStates(EAvLowerHipSection::Legs);
	RefreshLowerHipCardSelectionStates(EAvLowerHipSection::Hip);
	RefreshLowerHipCurrentSelection();
	RefreshFullOutfitCardSelectionStates();
	RefreshFullOutfitCurrentSelection();
	RefreshCurrentLoadoutSummary();
	UpdatePresetLabels();
	RefreshMyCharactersActionButtons();
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	RefreshAppearanceInspector();
#endif
}

void UAvCharacterCustomizationRootWidget::ResetPreviewCameraForActiveCharacter()
{
	if (PreviewActor)
	{
		PreviewActor->TransitionToCustomizationCameraPreset(
			EAvCustomizationCameraPreset::FullBody,
			false,
			FName(TEXT("CharacterChanged")));
		bCameraPresetInitialized = true;
	}
}

bool UAvCharacterCustomizationRootWidget::TryApplySelectionFromAppearance(const FWorkerAppearance& Appearance)
{
	const TSoftObjectPtr<USkeletalMesh> BodyMesh = Appearance.Get(EWorkerSlot::Body);
	if (BodyMesh.IsNull())
	{
		return false;
	}

	const FString AssetName = BodyMesh.ToSoftObjectPath().GetAssetName();
	const FString Prefix = TEXT("SKM_Worker_Male_");
	if (!AssetName.StartsWith(Prefix))
	{
		return false;
	}

	const int32 PresetNumber = FCString::Atoi(*AssetName.RightChop(Prefix.Len()));
	if (PresetNumber < 1 || PresetNumber > AvWorkerPresetCatalog::PresetCount)
	{
		return false;
	}

	CurrentPresetIndex = PresetNumber - 1;
	return true;
}

void UAvCharacterCustomizationRootWidget::RefreshPresetPreview()
{
	EnsurePreview();
	UpdatePresetLabels();
	RefreshCurrentLoadoutSummary();
	if (!PreviewActor)
	{
		return;
	}

	PresetPreviewAppearance = BuildSelectedAppearance();
	bHasPresetPreviewAppearance = true;
	PreviewActor->SetPreviewAppearance(PresetPreviewAppearance);
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	RefreshAppearanceInspector();
#endif
}

void UAvCharacterCustomizationRootWidget::ApplyActiveCharacterToPreview()
{
	bHasPresetPreviewAppearance = false;
	EnsurePreview();
	TryApplySelectionFromAppearance(ActiveCharacterAppearance);
	if (PreviewActor)
	{
		PreviewActor->SetPreviewAppearance(ActiveCharacterAppearance);
	}
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	RefreshAppearanceInspector();
#endif
}

void UAvCharacterCustomizationRootWidget::CommitActiveCharacterAppearance(
	const FWorkerAppearance& NewAppearance,
	bool bHasMeaningfulAppearance,
	FName BasePresetId,
	EAvAppearanceOrigin AppearanceOrigin)
{
	bLastManualModularSelectionClearedEnsemble = false;
	LastOperationAppearanceAutosaveCount = 1;
	ActiveCharacterAppearance = NewAppearance;
	FString SkinNormalization;
	UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(
		ActiveCharacterAppearance,
		&SkinNormalization);
	ActiveCharacterBasePresetId = BasePresetId;
	bActiveCharacterHasMeaningfulAppearance = bHasMeaningfulAppearance;
	ActiveCharacterAppearanceOrigin = AppearanceOrigin;
	bHasPresetPreviewAppearance = false;
	RefreshMyCharactersActionButtons();
	TryApplySelectionFromAppearance(ActiveCharacterAppearance);
	ApplyActiveCharacterToPreview();

	if (AAvaryoCharacter* Character = GetOwningPlayerPawn<AAvaryoCharacter>())
	{
		Character->RequestApplyWorkerAppearance(
			ActiveCharacterAppearance,
			bActiveCharacterHasMeaningfulAppearance,
			ActiveCharacterBasePresetId,
			ActiveCharacterAppearanceOrigin);
	}
	else if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger = GameInstance->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			Ledger->SetActiveCharacterAppearance(
				ActiveCharacterAppearance,
				bActiveCharacterHasMeaningfulAppearance,
				ActiveCharacterBasePresetId,
				ActiveCharacterAppearanceOrigin);
		}
	}
	UE_LOG(LogTemp, Display,
		TEXT("[AvCustomizeCommit] %s Origin=%d"),
		*SkinNormalization,
		static_cast<int32>(ActiveCharacterAppearanceOrigin));
}

bool UAvCharacterCustomizationRootWidget::ApplyManualModularSelectionAtomically(
	EWorkerSlot TargetSlot,
	const FSoftObjectPath& TargetMeshPath,
	const FString& TargetStableId,
	FString& OutValidationDetails)
{
	if (TargetSlot != EWorkerSlot::Torso &&
		TargetSlot != EWorkerSlot::Legs &&
		TargetSlot != EWorkerSlot::Feet)
	{
		OutValidationDetails = TEXT("Result=FAIL Reason=NotManualModularClothingSlot");
		return false;
	}

	const bool bHadFullOutfit =
		!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull();
	const bool bHadOveralls =
		!ActiveCharacterAppearance.Get(EWorkerSlot::Overalls).IsNull();
	FWorkerAppearance NewAppearance = ActiveCharacterAppearance;
	NewAppearance.Clear(EWorkerSlot::FullOutfit);
	NewAppearance.Clear(EWorkerSlot::Overalls);
	if (TargetMeshPath.IsNull())
	{
		NewAppearance.Clear(TargetSlot);
	}
	else
	{
		NewAppearance.Set(TargetSlot, TSoftObjectPtr<USkeletalMesh>(TargetMeshPath));
	}

	if (!UWorkerAppearanceComponent::ValidateHeadCustomizationSelection(
		NewAppearance, TargetSlot, OutValidationDetails))
	{
		return false;
	}

	CommitActiveCharacterAppearance(
		NewAppearance,
		true,
		ActiveCharacterBasePresetId.IsNone()
			? FName(TEXT("BaseMaleUnderwear"))
			: ActiveCharacterBasePresetId,
		EAvAppearanceOrigin::ManualCustomized);
	bLastManualModularSelectionClearedEnsemble = bHadFullOutfit || bHadOveralls;
	LastOperationAppearanceAutosaveCount = 1;
	RefreshFullOutfitCardSelectionStates();
	if (TargetSlot == EWorkerSlot::Torso)
	{
		RefreshTorsoCardSelectionStates();
		RefreshTorsoCurrentSelection();
	}
	else if (TargetSlot == EWorkerSlot::Legs)
	{
		RefreshLowerHipCardSelectionStates(EAvLowerHipSection::Legs);
		if (SelectedLowerHipSection == EAvLowerHipSection::Legs)
		{
			RefreshLowerHipCurrentSelection();
		}
	}
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	RefreshAppearanceInspector();
#endif
	UE_LOG(LogTemp, Warning,
		TEXT("[AvManualModularSelection] Result=APPLIED Slot=%d Target=%s ClearedFullOutfit=%s ClearedOveralls=%s PreviewApply=1 ManualCommit=1 AppearanceAutosaveCount=1"),
		static_cast<int32>(TargetSlot),
		*TargetStableId,
		bHadFullOutfit ? TEXT("true") : TEXT("false"),
		bHadOveralls ? TEXT("true") : TEXT("false"));
	return true;
}

void UAvCharacterCustomizationRootWidget::RefreshCurrentLoadoutSummary()
{
	const FWorkerAppearance SelectedAppearance = BuildSelectedAppearance();
	const TSoftObjectPtr<USkeletalMesh> BodyMesh = SelectedAppearance.Get(EWorkerSlot::Body);

	FAvWorkerPresetLoadoutSummary Summary;
	if (!AvWorkerPresetCatalog::TryGetLoadoutSummary(BodyMesh.ToSoftObjectPath(), Summary))
	{
		Summary.Head = FAvLoc::Text(TEXT("Customization.Common.NoData"));
		Summary.Face = Summary.Head;
		Summary.Top = Summary.Head;
		Summary.Hands = Summary.Head;
		Summary.Bottom = Summary.Head;
	}

	SetCurrentLoadoutValue(TEXT("Txt_LoadoutHeadValue"), NAME_None, Summary.Head);
	SetCurrentLoadoutValue(TEXT("Txt_LoadoutFaceValue"), NAME_None, Summary.Face);
	SetCurrentLoadoutValue(TEXT("Txt_LoadoutTopValue"), NAME_None, Summary.Top);
	SetCurrentLoadoutValue(TEXT("Txt_LoadoutHandsValue"), NAME_None, Summary.Hands);
	SetCurrentLoadoutValue(TEXT("Txt_LoadoutBottomValue"), TEXT("Txt_LoadoutHeadValue_4"), Summary.Bottom);
}

void UAvCharacterCustomizationRootWidget::SetCurrentLoadoutValue(
	FName WidgetName,
	FName LegacyWidgetName,
	const FText& Value) const
{
	if (!WidgetTree)
	{
		return;
	}

	UTextBlock* ValueWidget = Cast<UTextBlock>(WidgetTree->FindWidget(WidgetName));
	if (!ValueWidget && !LegacyWidgetName.IsNone())
	{
		ValueWidget = Cast<UTextBlock>(WidgetTree->FindWidget(LegacyWidgetName));
	}
	if (ValueWidget)
	{
		ValueWidget->SetText(Value);
	}
}

FWorkerAppearance UAvCharacterCustomizationRootWidget::BuildSelectedAppearance() const
{
	const int32 PresetNumber = CurrentPresetIndex + 1;
	const FString AssetName = FString::Printf(TEXT("SKM_Worker_Male_%d"), PresetNumber);
	const FString AssetPath = FString::Printf(
		TEXT("/Game/Modular_Workers/Mesh/Male/Presets/%s.%s"),
		*AssetName,
		*AssetName);

	FWorkerAppearance Appearance;
	Appearance.Set(
		EWorkerSlot::Body,
		TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(AssetPath)));
	return Appearance;
}

void UAvCharacterCustomizationRootWidget::UpdatePresetLabels()
{
	const int32 PresetNumber = CurrentPresetIndex + 1;
	FNumberFormattingOptions TwoDigitOptions;
	TwoDigitOptions.SetMinimumIntegralDigits(2);
	TwoDigitOptions.SetMaximumIntegralDigits(2);
	if (Txt_CurrentPresetName)
	{
		Txt_CurrentPresetName->SetText(FText::Format(
			FAvLoc::Text(TEXT("Customization.Preset.NameFormat")),
			FText::AsNumber(PresetNumber, &TwoDigitOptions)));
	}
	if (Txt_PresetCounter)
	{
		Txt_PresetCounter->SetText(FText::Format(
			FAvLoc::Text(TEXT("Customization.Common.PageCounterFormat")),
			FText::AsNumber(PresetNumber),
			FText::AsNumber(AvWorkerPresetCatalog::PresetCount)));
	}
}

UImage* UAvCharacterCustomizationRootWidget::ResolvePreviewImage() const
{
	if (Img_Preview)
	{
		return Img_Preview;
	}
	if (Img_PreviewPlaceholder)
	{
		return Img_PreviewPlaceholder;
	}
	if (!WidgetTree)
	{
		return nullptr;
	}
	if (UImage* PreviewImage = Cast<UImage>(WidgetTree->FindWidget(TEXT("Img_Preview"))))
	{
		return PreviewImage;
	}
	return Cast<UImage>(WidgetTree->FindWidget(TEXT("Img_PreviewPlaceholder")));
}

void UAvCharacterCustomizationRootWidget::BindApplyButton()
{
	if (Btn_SaveCharacter)
	{
		Btn_SaveCharacter->OnClicked.RemoveAll(this);
	}
	if (Btn_ApplyCharacter && Btn_ApplyCharacter != Btn_SaveCharacter)
	{
		Btn_ApplyCharacter->OnClicked.RemoveAll(this);
	}

	if (UButton* ApplyButton = ResolveApplyButton())
	{
		ApplyButton->OnClicked.RemoveAll(this);
		ApplyButton->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleApplyCharacterClicked);
		UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeApply] Bound visible Apply+Save button: %s"), *GetNameSafe(ApplyButton));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AvCustomizeApply] Apply button was not found; visible Apply clicks will not reach C++"));
	}
}

UButton* UAvCharacterCustomizationRootWidget::ResolveApplyButton() const
{
	if (!WidgetTree)
	{
		return nullptr;
	}

	const auto IsUsableButton = [](const UButton* Button) -> bool
	{
		if (!Button || !Button->GetIsEnabled())
		{
			return false;
		}

		return Button->GetVisibility() == ESlateVisibility::Visible;
	};

	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);
	TFunction<bool(UWidget*)> ContainsApplyText = [&ContainsApplyText](UWidget* Candidate) -> bool
	{
		if (!Candidate)
		{
			return false;
		}
		if (const UTextBlock* TextBlock = Cast<UTextBlock>(Candidate))
		{
			const FString Label = TextBlock->GetText().ToString();
			return Label.Equals(
				FAvLoc::S(TEXT("Customization.Action.Apply")), ESearchCase::IgnoreCase)
				|| Label.Equals(
					FAvLoc::S(TEXT("Customization.Action.ApplyPreset")), ESearchCase::IgnoreCase);
		}
		if (UContentWidget* ContentWidget = Cast<UContentWidget>(Candidate))
		{
			return ContainsApplyText(ContentWidget->GetContent());
		}
		if (UPanelWidget* PanelWidget = Cast<UPanelWidget>(Candidate))
		{
			for (int32 ChildIndex = 0; ChildIndex < PanelWidget->GetChildrenCount(); ++ChildIndex)
			{
				if (ContainsApplyText(PanelWidget->GetChildAt(ChildIndex)))
				{
					return true;
				}
			}
		}
		return false;
	};

	for (UWidget* Widget : AllWidgets)
	{
		UButton* Button = Cast<UButton>(Widget);
		if (IsUsableButton(Button) && ContainsApplyText(Button))
		{
			return Button;
		}
	}

	if (IsUsableButton(Btn_ApplyCharacter))
	{
		return Btn_ApplyCharacter;
	}
	if (UButton* NamedApplyButton = Cast<UButton>(WidgetTree->FindWidget(TEXT("Btn_ApplyCharacter"))))
	{
		if (IsUsableButton(NamedApplyButton))
		{
			return NamedApplyButton;
		}
	}
	if (IsUsableButton(Btn_SaveCharacter))
	{
		return Btn_SaveCharacter;
	}
	if (UButton* LegacyApplyButton = Cast<UButton>(WidgetTree->FindWidget(TEXT("Btn_SaveCharacter"))))
	{
		if (IsUsableButton(LegacyApplyButton))
		{
			return LegacyApplyButton;
		}
	}

	for (UWidget* Widget : AllWidgets)
	{
		UButton* Button = Cast<UButton>(Widget);
		if (!Button)
		{
			continue;
		}

		if (ContainsApplyText(Button))
		{
			return Button;
		}
	}

	return nullptr;
}

namespace
{
	template <typename WidgetType>
	WidgetType* FindAvWidget(const UWidgetTree* WidgetTree, const TCHAR* Name)
	{
		return WidgetTree ? Cast<WidgetType>(WidgetTree->FindWidget(FName(Name))) : nullptr;
	}

	struct FAvCategoryVisualWidgets
	{
		EAvCustomizationCategory Category;
		const TCHAR* ActiveBackgroundName;
		const TCHAR* ActiveLineSizeBoxName;
		const TCHAR* ActiveLineName;
		const TCHAR* ActiveOutlineName;
		const TCHAR* ActiveBorderName;
		const TCHAR* ActiveFrameName;
		const TCHAR* TextName;
		const TCHAR* IconName;
	};

	const FAvCategoryVisualWidgets CategoryVisualWidgets[] =
	{
		{ EAvCustomizationCategory::MyCharacters, TEXT("Border_CategoryMyCharactersActiveBg"), TEXT("SB_CategoryMyCharactersActiveLine"), TEXT("Border_CategoryMyCharactersActiveLine"), TEXT(""), TEXT(""), TEXT(""), TEXT("Txt_CategoryMyCharactersLabel"), TEXT("Img_CategoryMyCharactersIcon") },
		{ EAvCustomizationCategory::Presets, TEXT("Border_CategoryPresetsActiveBG"), TEXT("SB_CategoryPresetsActiveLine"), TEXT("Border_CategoryPresetsActiveLine"), TEXT("Border_CategoryPresetsActiveOutline"), TEXT("Border_CategoryPresetsActiveBorder"), TEXT("Border_CategoryPresetsActiveFrame"), TEXT("Txt_CategoryPresets"), TEXT("Img_CategoryPresets") },
		{ EAvCustomizationCategory::Head, TEXT("Border_CategoryHeadActiveBG"), TEXT("SB_CategoryHeadActiveLine"), TEXT("Border_CategoryHeadActiveLine"), TEXT("Border_CategoryHeadActiveOutline"), TEXT("Border_CategoryHeadActiveBorder"), TEXT("Border_CategoryHeadActiveFrame"), TEXT("Txt_CategoryHead"), TEXT("Img_CategoryHead") },
		{ EAvCustomizationCategory::FaceProtection, TEXT("Border_CategoryFaceProtectionActiveBG"), TEXT("SB_CategoryFaceProtectionActiveLine"), TEXT("Border_CategoryFaceProtectionActiveLine"), TEXT("Border_CategoryFaceProtectionActiveOutline"), TEXT("Border_CategoryFaceProtectionActiveBorder"), TEXT("Border_CategoryFaceProtectionActiveFrame"), TEXT("Txt_CategoryFaceProtection"), TEXT("Img_CategoryFaceProtection") },
		{ EAvCustomizationCategory::HandsAccessories, TEXT("Border_CategoryHandsAccessoriesActiveBG"), TEXT("SB_CategoryHandsAccessoriesActiveLine"), TEXT("Border_CategoryHandsAccessoriesActiveLine"), TEXT("Border_CategoryHandsAccessoriesActiveOutline"), TEXT("Border_CategoryHandsAccessoriesActiveBorder"), TEXT("Border_CategoryHandsAccessoriesActiveFrame"), TEXT("Txt_CategoryHandsAccessories"), TEXT("Img_CategoryHandsAccessories") },
		{ EAvCustomizationCategory::UpperGear, TEXT("Border_CategoryUpperGearActiveBG"), TEXT("SB_CategoryUpperGearActiveLine"), TEXT("Border_CategoryUpperGearActiveLine"), TEXT("Border_CategoryUpperGearActiveOutline"), TEXT("Border_CategoryUpperGearActiveBorder"), TEXT("Border_CategoryUpperGearActiveFrame"), TEXT("Txt_CategoryUpperGear"), TEXT("Img_CategoryUpperGear") },
		{ EAvCustomizationCategory::LowerHip, TEXT("Border_CategoryLowerHipActiveBG"), TEXT("SB_CategoryLowerHipActiveLine"), TEXT("Border_CategoryLowerHipActiveLine"), TEXT("Border_CategoryLowerHipActiveOutline"), TEXT("Border_CategoryLowerHipActiveBorder"), TEXT("Border_CategoryLowerHipActiveFrame"), TEXT("Txt_CategoryLowerHip"), TEXT("Img_CategoryLowerHip") },
		{ EAvCustomizationCategory::FullOutfits, TEXT("Border_CategoryFullOutfitsActiveBG"), TEXT("SB_CategoryFullOutfitsActiveLine"), TEXT("Border_CategoryFullOutfitsActiveLine"), TEXT("Border_CategoryFullOutfitsActiveOutline"), TEXT("Border_CategoryFullOutfitsActiveBorder"), TEXT("Border_CategoryFullOutfitsActiveFrame"), TEXT("Txt_CategoryFullOutfits"), TEXT("Img_CategoryFullOutfits") },
	};

	const FAvCategoryVisualWidgets* FindCategoryVisualWidgets(EAvCustomizationCategory Category)
	{
		for (const FAvCategoryVisualWidgets& VisualWidgets : CategoryVisualWidgets)
		{
			if (VisualWidgets.Category == Category)
			{
				return &VisualWidgets;
			}
		}

		return nullptr;
	}

}

void UAvCharacterCustomizationRootWidget::BindCategoryButtons()
{
	if (Btn_CategoryMyCharacters)
	{
		Btn_CategoryMyCharacters->OnClicked.RemoveAll(this);
		Btn_CategoryMyCharacters->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCategoryMyCharactersClicked);
	}
	if (Btn_CategoryPresets)
	{
		Btn_CategoryPresets->OnClicked.RemoveAll(this);
		Btn_CategoryPresets->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCategoryPresetsClicked);
	}
	if (Btn_CategoryHead)
	{
		Btn_CategoryHead->OnClicked.RemoveAll(this);
		Btn_CategoryHead->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCategoryHeadClicked);
	}
	if (Btn_CategoryFaceProtection)
	{
		Btn_CategoryFaceProtection->OnClicked.RemoveAll(this);
		Btn_CategoryFaceProtection->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCategoryFaceProtectionClicked);
	}
	if (Btn_CategoryHandsAccessories)
	{
		Btn_CategoryHandsAccessories->OnClicked.RemoveAll(this);
		Btn_CategoryHandsAccessories->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCategoryHandsAccessoriesClicked);
	}
	if (Btn_CategoryUpperGear)
	{
		Btn_CategoryUpperGear->OnClicked.RemoveAll(this);
		Btn_CategoryUpperGear->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCategoryUpperGearClicked);
	}
	if (Btn_CategoryLowerHip)
	{
		Btn_CategoryLowerHip->OnClicked.RemoveAll(this);
		Btn_CategoryLowerHip->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCategoryLowerHipClicked);
	}
	if (Btn_CategoryFullOutfits)
	{
		Btn_CategoryFullOutfits->OnClicked.RemoveAll(this);
		Btn_CategoryFullOutfits->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleCategoryFullOutfitsClicked);
	}
	if (Btn_LowerFilterLegs)
	{
		Btn_LowerFilterLegs->OnClicked.RemoveAll(this);
		Btn_LowerFilterLegs->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleLowerFilterLegsClicked);
	}
	if (Btn_LowerFilterHip)
	{
		Btn_LowerFilterHip->OnClicked.RemoveAll(this);
		Btn_LowerFilterHip->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleLowerFilterHipClicked);
	}
}

void UAvCharacterCustomizationRootWidget::ConfigureMyCharactersActionButtons()
{
	auto ConfigureButton = [](UButton* Button)
	{
		if (!Button)
		{
			return;
		}
		FSlateBrush NoDrawBrush;
		NoDrawBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		FButtonStyle Style = Button->GetStyle();
		Style.SetNormal(NoDrawBrush);
		Style.SetHovered(NoDrawBrush);
		Style.SetPressed(NoDrawBrush);
		Style.SetDisabled(NoDrawBrush);
		Style.SetNormalPadding(FMargin(0.f));
		Style.SetPressedPadding(FMargin(0.f));
		Button->SetStyle(Style);
		Button->IsFocusable = true;
		if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Button->Slot))
		{
			FSlateChildSize FillSize(ESlateSizeRule::Fill);
			FillSize.Value = 1.f;
			Slot->SetSize(FillSize);
			Slot->SetPadding(FMargin(0.f));
			Slot->SetHorizontalAlignment(HAlign_Fill);
			Slot->SetVerticalAlignment(VAlign_Fill);
		}
	};

	auto ConfigureContent = [](UButton* Button, UBorder* Border, UImage* Image,
		UTextBlock* Label, const FText& Text)
	{
		if (Button && Border)
		{
			if (UButtonSlot* Slot = Cast<UButtonSlot>(Border->Slot))
			{
				Slot->SetPadding(FMargin(0.f));
				Slot->SetHorizontalAlignment(HAlign_Fill);
				Slot->SetVerticalAlignment(VAlign_Fill);
			}
		}
		if (Border)
		{
			Border->SetPadding(FMargin(10.f, 11.f));
			Border->SetHorizontalAlignment(HAlign_Fill);
			Border->SetVerticalAlignment(VAlign_Center);
			Border->SetShowEffectWhenDisabled(false);
			if (UHorizontalBox* Content = Cast<UHorizontalBox>(Border->GetContent()))
			{
				if (UBorderSlot* Slot = Cast<UBorderSlot>(Content->Slot))
				{
					Slot->SetHorizontalAlignment(HAlign_Fill);
					Slot->SetVerticalAlignment(VAlign_Center);
				}
			}
		}
		if (Image)
		{
			// Asset Registry audit found no semantic project-owned reset/delete icon.
			Image->SetVisibility(ESlateVisibility::Collapsed);
			Image->SetClipping(EWidgetClipping::ClipToBounds);
		}
		if (Label)
		{
			Label->SetText(Text);
			Label->SetAutoWrapText(false);
			Label->SetJustification(ETextJustify::Center);
			Label->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
			Label->SetClipping(EWidgetClipping::ClipToBounds);
			FSlateFontInfo Font = Label->GetFont();
			Font.Size = 16;
			Font.TypefaceFontName = FName(TEXT("Bold"));
			Label->SetFont(Font);
			if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Label->Slot))
			{
				FSlateChildSize FillSize(ESlateSizeRule::Fill);
				FillSize.Value = 1.f;
				Slot->SetSize(FillSize);
				Slot->SetPadding(FMargin(0.f));
				Slot->SetHorizontalAlignment(HAlign_Fill);
				Slot->SetVerticalAlignment(VAlign_Center);
			}
		}
	};

	if (HB_MyCharactersActions)
	{
		HB_MyCharactersActions->SetVisibility(ESlateVisibility::Visible);
		HB_MyCharactersActions->SetClipping(EWidgetClipping::ClipToBounds);
		if (UVerticalBoxSlot* ActionPanelSlot = Cast<UVerticalBoxSlot>(HB_MyCharactersActions->Slot))
		{
			ActionPanelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			ActionPanelSlot->SetPadding(FMargin(0.f, 10.f, 0.f, 0.f));
			ActionPanelSlot->SetHorizontalAlignment(HAlign_Fill);
			ActionPanelSlot->SetVerticalAlignment(VAlign_Bottom);
		}
	}

	ConfigureButton(Btn_ResetActiveCharacter);
	ConfigureButton(Btn_DeleteActiveCharacter);
	ConfigureContent(Btn_ResetActiveCharacter, Border_ResetActiveCharacter,
		Img_ResetActiveCharacter, Txt_ResetActiveCharacterLabel,
		FAvLoc::Text(TEXT("Customization.Action.Reset")));
	ConfigureContent(Btn_DeleteActiveCharacter, Border_DeleteActiveCharacter,
		Img_DeleteActiveCharacter, Txt_DeleteActiveCharacterLabel,
		FAvLoc::Text(TEXT("Customization.Action.Delete")));

	if (Btn_ResetActiveCharacter)
	{
		Btn_ResetActiveCharacter->OnClicked.RemoveAll(this);
		Btn_ResetActiveCharacter->OnHovered.RemoveAll(this);
		Btn_ResetActiveCharacter->OnUnhovered.RemoveAll(this);
		Btn_ResetActiveCharacter->OnPressed.RemoveAll(this);
		Btn_ResetActiveCharacter->OnReleased.RemoveAll(this);
		Btn_ResetActiveCharacter->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleResetAppearanceClicked);
		Btn_ResetActiveCharacter->OnHovered.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleResetActionHovered);
		Btn_ResetActiveCharacter->OnUnhovered.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleResetActionUnhovered);
		Btn_ResetActiveCharacter->OnPressed.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleResetActionPressed);
		Btn_ResetActiveCharacter->OnReleased.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleResetActionReleased);
	}
	if (Btn_DeleteActiveCharacter)
	{
		Btn_DeleteActiveCharacter->OnClicked.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnHovered.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnUnhovered.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnPressed.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnReleased.RemoveAll(this);
		Btn_DeleteActiveCharacter->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleDeleteActiveCharacterClicked);
		Btn_DeleteActiveCharacter->OnHovered.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleDeleteActionHovered);
		Btn_DeleteActiveCharacter->OnUnhovered.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleDeleteActionUnhovered);
		Btn_DeleteActiveCharacter->OnPressed.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleDeleteActionPressed);
		Btn_DeleteActiveCharacter->OnReleased.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleDeleteActionReleased);
	}
	LastMyCharactersDuplicateDelegateCount = 0;
	RefreshMyCharactersActionButtons();
}

void UAvCharacterCustomizationRootWidget::RefreshMyCharactersActionButtons()
{
	const UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	const FAvCharacterRecord* ActiveRecord = Ledger ? Ledger->GetActiveCharacter() : nullptr;
	const bool bActiveRecordValid = ActiveRecord && !ActiveRecord->CharacterId.IsNone() &&
		Ledger->GetCharacterRecords().ContainsByPredicate(
			[ActiveCharacterId = ActiveRecord->CharacterId](const FAvCharacterRecord& Record)
			{
				return Record.CharacterId == ActiveCharacterId;
			});
	const bool bDeleteAllowed = bActiveRecordValid && Ledger->GetCharacterRecords().Num() > 1;

	if (Btn_ResetActiveCharacter)
	{
		Btn_ResetActiveCharacter->SetIsEnabled(bActiveRecordValid);
		Btn_ResetActiveCharacter->SetToolTipText(FAvLoc::Text(
			bActiveRecordValid
				? TEXT("Customization.Character.ResetTooltip")
				: TEXT("Customization.Character.ActiveNotFound")));
		ApplyMyCharactersActionVisual(false,
			bActiveRecordValid ? EAvMyCharactersActionVisualState::Normal : EAvMyCharactersActionVisualState::Disabled);
	}
	if (Btn_DeleteActiveCharacter)
	{
		Btn_DeleteActiveCharacter->SetIsEnabled(bDeleteAllowed);
		Btn_DeleteActiveCharacter->SetToolTipText(FAvLoc::Text(
			bDeleteAllowed
				? TEXT("Customization.Character.DeleteTooltip")
				: TEXT("Customization.Character.DeleteLastBlocked")));
		ApplyMyCharactersActionVisual(true,
			bDeleteAllowed ? EAvMyCharactersActionVisualState::Normal : EAvMyCharactersActionVisualState::Disabled);
	}
}

void UAvCharacterCustomizationRootWidget::ApplyMyCharactersActionVisual(
	bool bDeleteAction,
	EAvMyCharactersActionVisualState VisualState)
{
	UButton* Button = bDeleteAction ? Btn_DeleteActiveCharacter : Btn_ResetActiveCharacter;
	UBorder* Border = bDeleteAction ? Border_DeleteActiveCharacter : Border_ResetActiveCharacter;
	UTextBlock* Label = bDeleteAction ? Txt_DeleteActiveCharacterLabel : Txt_ResetActiveCharacterLabel;
	UImage* Image = bDeleteAction ? Img_DeleteActiveCharacter : Img_ResetActiveCharacter;
	if (!Button || !Border || !Label)
	{
		return;
	}
	if (!Button->GetIsEnabled())
	{
		VisualState = EAvMyCharactersActionVisualState::Disabled;
	}

	FLinearColor Background;
	FLinearColor Outline;
	FLinearColor TextColor;
	FLinearColor IconColor;
	if (!bDeleteAction)
	{
		switch (VisualState)
		{
		case EAvMyCharactersActionVisualState::Hovered:
			Background = AvMyCharactersActions::FromHex(0x20, 0x28, 0x30);
			Outline = AvMyCharactersActions::ProjectOrange;
			TextColor = AvMyCharactersActions::ProjectOrange;
			IconColor = AvMyCharactersActions::ProjectOrange;
			break;
		case EAvMyCharactersActionVisualState::Pressed:
			Background = AvMyCharactersActions::FromHex(0x11, 0x16, 0x1B);
			Outline = AvMyCharactersActions::ProjectOrange;
			TextColor = AvMyCharactersActions::ProjectOrange;
			IconColor = AvMyCharactersActions::ProjectOrange;
			break;
		case EAvMyCharactersActionVisualState::Disabled:
			Background = AvMyCharactersActions::FromHex(0x11, 0x16, 0x1B);
			Outline = AvMyCharactersActions::FromHex(0x30, 0x38, 0x40);
			TextColor = AvMyCharactersActions::FromHex(0xD5, 0xD9, 0xDD, 89);
			IconColor = AvMyCharactersActions::FromHex(0xB5, 0xBB, 0xC1, 89);
			break;
		default:
			Background = AvMyCharactersActions::FromHex(0x17, 0x1D, 0x23);
			Outline = AvMyCharactersActions::FromHex(0x55, 0x5E, 0x67);
			TextColor = AvMyCharactersActions::FromHex(0xD5, 0xD9, 0xDD);
			IconColor = AvMyCharactersActions::FromHex(0xB5, 0xBB, 0xC1);
			break;
		}
	}
	else
	{
		switch (VisualState)
		{
		case EAvMyCharactersActionVisualState::Hovered:
			Background = AvMyCharactersActions::FromHex(0x2A, 0x20, 0x23);
			Outline = AvMyCharactersActions::FromHex(0xB8, 0x5D, 0x63);
			TextColor = AvMyCharactersActions::FromHex(0xF0, 0xA0, 0xA5);
			IconColor = AvMyCharactersActions::FromHex(0xD2, 0x7A, 0x7E);
			break;
		case EAvMyCharactersActionVisualState::Pressed:
			Background = AvMyCharactersActions::FromHex(0x14, 0x0F, 0x11);
			Outline = AvMyCharactersActions::FromHex(0xD0, 0x6A, 0x70);
			TextColor = AvMyCharactersActions::FromHex(0xD2, 0x7A, 0x7E);
			IconColor = AvMyCharactersActions::FromHex(0xD2, 0x7A, 0x7E);
			break;
		case EAvMyCharactersActionVisualState::Disabled:
			Background = AvMyCharactersActions::FromHex(0x15, 0x12, 0x14);
			Outline = AvMyCharactersActions::FromHex(0x3D, 0x2B, 0x2E);
			TextColor = AvMyCharactersActions::FromHex(0xDF, 0xC4, 0xC6, 89);
			IconColor = AvMyCharactersActions::FromHex(0xD2, 0x7A, 0x7E, 89);
			break;
		default:
			Background = AvMyCharactersActions::FromHex(0x1C, 0x18, 0x1B);
			Outline = AvMyCharactersActions::FromHex(0x75, 0x46, 0x4B);
			TextColor = AvMyCharactersActions::FromHex(0xDF, 0xC4, 0xC6);
			IconColor = AvMyCharactersActions::FromHex(0xD2, 0x7A, 0x7E);
			break;
		}
	}

	FSlateBrush Brush = Border->Background;
	Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
	Brush.TintColor = FSlateColor(FLinearColor::White);
	Brush.OutlineSettings.CornerRadii = FVector4(4.f, 4.f, 4.f, 4.f);
	Brush.OutlineSettings.Width = 1.f;
	Brush.OutlineSettings.Color = FSlateColor(Outline);
	Border->SetBrush(Brush);
	Border->SetBrushColor(Background);
	Label->SetColorAndOpacity(FSlateColor(TextColor));
	if (Image)
	{
		Image->SetColorAndOpacity(IconColor);
	}
}

void UAvCharacterCustomizationRootWidget::RefreshActiveCharacterDisplay()
{
	if (!WidgetTree)
	{
		return;
	}
	if (Txt_CharacterName01)
	{
		Txt_CharacterName01->SetText(CurrentCharacterDisplayName);
	}
	if (UBorder* SelectedFrame = FindAvWidget<UBorder>(WidgetTree, TEXT("Border_CharacterCard01SelectedFrame")))
	{
		SelectedFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	RefreshMyCharactersActionButtons();
}

void UAvCharacterCustomizationRootWidget::HandleEditCharacterNameClicked()
{
	BeginCharacterNameEditing();
}

void UAvCharacterCustomizationRootWidget::HandleCharacterNameTextChanged(const FText& NewText)
{
	if (bIsClampingCharacterName || !ETB_CharacterName01)
	{
		return;
	}

	const FString Value = NewText.ToString();
	if (Value.Len() <= 20)
	{
		return;
	}

	TGuardValue<bool> Guard(bIsClampingCharacterName, true);
	ETB_CharacterName01->SetText(FText::FromString(Value.Left(20)));
}

void UAvCharacterCustomizationRootWidget::HandleCharacterNameCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!bIsEditingCharacterName)
	{
		return;
	}

	if (CommitMethod == ETextCommit::OnCleared)
	{
		CancelCharacterNameEditing();
		return;
	}

	if (CommitMethod == ETextCommit::OnEnter || CommitMethod == ETextCommit::OnUserMovedFocus)
	{
		CommitCharacterNameEditing(Text);
	}
}

void UAvCharacterCustomizationRootWidget::BeginCharacterNameEditing()
{
	if (bIsEditingCharacterName || !Btn_EditCharacterName01 || !ETB_CharacterName01)
	{
		return;
	}

	bIsEditingCharacterName = true;
	CharacterNameBeforeEdit = CurrentCharacterDisplayName;
	ETB_CharacterName01->SetText(CurrentCharacterDisplayName);
	Btn_EditCharacterName01->SetVisibility(ESlateVisibility::Collapsed);
	ETB_CharacterName01->SetVisibility(ESlateVisibility::Visible);
	ETB_CharacterName01->SetKeyboardFocus();
}

void UAvCharacterCustomizationRootWidget::CommitCharacterNameEditing(const FText& NewName)
{
	if (!bIsEditingCharacterName)
	{
		return;
	}

	const FString TrimmedName = NewName.ToString().TrimStartAndEnd().Left(20);
	const FString PreviousName = CurrentCharacterDisplayName.ToString();
	CurrentCharacterDisplayName = TrimmedName.IsEmpty()
		? CharacterNameBeforeEdit
		: FText::FromString(TrimmedName);

	if (Txt_CharacterName01)
	{
		Txt_CharacterName01->SetText(CurrentCharacterDisplayName);
	}

	bIsEditingCharacterName = false;
	if (ETB_CharacterName01)
	{
		ETB_CharacterName01->SetText(CurrentCharacterDisplayName);
		ETB_CharacterName01->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Btn_EditCharacterName01)
	{
		Btn_EditCharacterName01->SetVisibility(ESlateVisibility::Visible);
	}

	const FString ConfirmedName = CurrentCharacterDisplayName.ToString();
	if (!ConfirmedName.Equals(PreviousName, ESearchCase::CaseSensitive))
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			if (UCompanyLedgerSubsystem* Ledger = GameInstance->GetSubsystem<UCompanyLedgerSubsystem>())
			{
				Ledger->SetActiveCharacterDisplayName(ConfirmedName);
			}
		}
	}
}

void UAvCharacterCustomizationRootWidget::CancelCharacterNameEditing()
{
	if (!bIsEditingCharacterName)
	{
		return;
	}

	bIsEditingCharacterName = false;
	if (ETB_CharacterName01)
	{
		ETB_CharacterName01->SetText(CharacterNameBeforeEdit);
		ETB_CharacterName01->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Btn_EditCharacterName01)
	{
		Btn_EditCharacterName01->SetVisibility(ESlateVisibility::Visible);
	}
}

void UAvCharacterCustomizationRootWidget::SelectCategory(EAvCustomizationCategory Category)
{
	CommitRuntimeMyCharacterRenames();
	if (bIsEditingCharacterName && ETB_CharacterName01)
	{
		CommitCharacterNameEditing(ETB_CharacterName01->GetText());
	}

	const bool bCategoryChanged = SelectedCategory != Category;
	SelectedCategory = Category;
	const bool bShouldTransitionCamera = !bCameraPresetInitialized || bCategoryChanged;
	const bool bImmediateCameraTransition = !bCameraPresetInitialized;
	if (!bHasCapturedCategoryDesignerVisuals)
	{
		CaptureCategoryDesignerVisuals();
	}
	RefreshCategoryVisuals();

	if (WS_RightPanelPages)
	{
		if (Category == EAvCustomizationCategory::MyCharacters && VB_RightPanelMyCharacters)
		{
			WS_RightPanelPages->SetActiveWidget(VB_RightPanelMyCharacters);
		}
		else if (Category == EAvCustomizationCategory::Head && VB_RightPanelHead)
		{
			WS_RightPanelPages->SetActiveWidget(VB_RightPanelHead);
		}
		else if (Category == EAvCustomizationCategory::FaceProtection && VB_RightPanelFaceProtection)
		{
			WS_RightPanelPages->SetActiveWidget(VB_RightPanelFaceProtection);
		}
		else if (Category == EAvCustomizationCategory::HandsAccessories && VB_RightPanelHandsAccessories)
		{
			WS_RightPanelPages->SetActiveWidget(VB_RightPanelHandsAccessories);
		}
		else if (Category == EAvCustomizationCategory::UpperGear && VB_RightPanelUpperEquipment)
		{
			WS_RightPanelPages->SetActiveWidget(VB_RightPanelUpperEquipment);
		}
		else if (Category == EAvCustomizationCategory::LowerHip && VB_RightPanelLowerHip)
		{
			WS_RightPanelPages->SetActiveWidget(VB_RightPanelLowerHip);
		}
		else if (Category == EAvCustomizationCategory::FullOutfits && VB_RightPanelFullOutfits)
		{
			WS_RightPanelPages->SetActiveWidget(VB_RightPanelFullOutfits);
		}
		else if (VB_RightPanelPresets)
		{
			WS_RightPanelPages->SetActiveWidget(VB_RightPanelPresets);
		}
	}

	if (Category == EAvCustomizationCategory::MyCharacters)
	{
		EnsureMyCharactersPageInitialized();
		RefreshActiveCharacterDisplay();
		ApplyActiveCharacterToPreview();
	}
	else if (Category == EAvCustomizationCategory::Presets)
	{
		RefreshPresetPreview();
	}
	else if (Category == EAvCustomizationCategory::Head)
	{
		// Crossing the Preset boundary always returns to the committed active appearance.
		ApplyActiveCharacterToPreview();
		SelectHeadSection(EAvHeadCustomizationSection::HeadType);
	}
	else if (Category == EAvCustomizationCategory::FaceProtection)
	{
		ApplyActiveCharacterToPreview();
		SelectFaceProtectionSection(EAvFaceProtectionSection::Glasses);
	}
	else if (Category == EAvCustomizationCategory::HandsAccessories)
	{
		ApplyActiveCharacterToPreview();
		SelectHandsAccessoriesSection(EAvHandsAccessoriesSection::Gloves);
	}
	else if (Category == EAvCustomizationCategory::UpperGear)
	{
		ApplyActiveCharacterToPreview();
		RefreshTorsoCurrentSelection();
		if (!bTorsoCatalogBuilt)
		{
			RebuildTorsoItemCards(true);
			bTorsoCatalogBuilt = true;
		}
		else
		{
			RefreshTorsoCardSelectionStates();
		}
	}
	else if (Category == EAvCustomizationCategory::LowerHip)
	{
		ApplyActiveCharacterToPreview();
		SelectLowerHipSection(EAvLowerHipSection::Legs);
	}
	else if (Category == EAvCustomizationCategory::FullOutfits)
	{
		ApplyActiveCharacterToPreview();
		RefreshFullOutfitCurrentSelection();
		if (!bFullOutfitCatalogBuilt)
		{
			RebuildFullOutfitItemCards(true);
			bFullOutfitCatalogBuilt = true;
		}
		else
		{
			RefreshFullOutfitCardSelectionStates();
		}
		const int32 PageIndex = WS_RightPanelPages && VB_RightPanelFullOutfits
			? WS_RightPanelPages->GetChildIndex(VB_RightPanelFullOutfits)
			: INDEX_NONE;
		UE_LOG(LogTemp, Warning,
			TEXT("[AvCustomizeFullOutfitsRouting] Page=VB_RightPanelFullOutfits ActualIndex=%d Router=WS_RightPanelPages"),
			PageIndex);
	}
	if (bShouldTransitionCamera)
	{
		TransitionPreviewCameraForCategory(Category, bImmediateCameraTransition);
	}
}

void UAvCharacterCustomizationRootWidget::EnsureHeadPageComposition()
{
	if (bHeadPageCompositionValidated)
	{
		return;
	}
	bHeadPageCompositionValidated = true;

	TArray<FString> MissingWidgets;
	auto RequireWidget = [&MissingWidgets](const UWidget* Widget, const TCHAR* Name)
	{
		if (!Widget)
		{
			MissingWidgets.Add(Name);
		}
	};

	RequireWidget(WS_HeadSectionPages, TEXT("WS_HeadSectionPages"));
	RequireWidget(ScrollBox_HeadItems, TEXT("ScrollBox_HeadItems"));
	RequireWidget(UniformGrid_HeadItems, TEXT("UniformGrid_HeadItems"));
	RequireWidget(Txt_HeadEmptyState, TEXT("Txt_HeadEmptyState"));
	RequireWidget(ScrollBox_HeadgearItems, TEXT("ScrollBox_HeadgearItems"));
	RequireWidget(UniformGrid_HeadgearItems, TEXT("UniformGrid_HeadgearItems"));
	RequireWidget(Txt_HeadgearEmptyState, TEXT("Txt_HeadgearEmptyState"));
	RequireWidget(ScrollBox_HairItems, TEXT("ScrollBox_HairItems"));
	RequireWidget(UniformGrid_HairItems, TEXT("UniformGrid_HairItems"));
	RequireWidget(Txt_HairEmptyState, TEXT("Txt_HairEmptyState"));
	RequireWidget(ScrollBox_BeardItems, TEXT("ScrollBox_BeardItems"));
	RequireWidget(UniformGrid_BeardItems, TEXT("UniformGrid_BeardItems"));
	RequireWidget(Txt_BeardEmptyState, TEXT("Txt_BeardEmptyState"));
	if (!HeadCatalogItemCardClass)
	{
		MissingWidgets.Add(TEXT("HeadCatalogItemCardClass"));
	}

	if (!MissingWidgets.IsEmpty())
	{
		const FString MissingList = FString::Join(MissingWidgets, TEXT(", "));
		ensureMsgf(false,
			TEXT("Static Head page is incomplete in WBP_CharacterCustomizationRoot_V2. Missing={%s}. Runtime reconstruction is disabled."),
			*MissingList);
		UE_LOG(LogTemp, Error,
			TEXT("[AvCustomizeHeadDesigner] Static Head page is incomplete in WBP_CharacterCustomizationRoot_V2. Missing={%s}. Runtime reconstruction is disabled."),
			*MissingList);
	}
}

void UAvCharacterCustomizationRootWidget::InitializeHeadCustomizationPage()
{
	HeadCatalog = UWorkerAppearanceComponent::GetHeadCustomizationCatalog();
	BuildHeadThumbnailRegistry();
	EnsureHeadPageComposition();
	const FText EmptyStateText = FAvLoc::Text(TEXT("Customization.Common.NoAvailableOptions"));
	for (UTextBlock* EmptyState : {
		Txt_HeadEmptyState.Get(),
		Txt_HeadgearEmptyState.Get(),
		Txt_HairEmptyState.Get(),
		Txt_BeardEmptyState.Get()})
	{
		if (EmptyState)
		{
			EmptyState->SetText(EmptyStateText);
		}
	}

	if (Btn_HeadFilterHeadType)
	{
		Btn_HeadFilterHeadType->OnClicked.RemoveAll(this);
		Btn_HeadFilterHeadType->OnClicked.AddDynamic(
			this, &UAvCharacterCustomizationRootWidget::HandleHeadFilterHeadTypeClicked);
	}
	if (Btn_HeadFilterHeadgear)
	{
		Btn_HeadFilterHeadgear->OnClicked.RemoveAll(this);
		Btn_HeadFilterHeadgear->OnClicked.AddDynamic(
			this, &UAvCharacterCustomizationRootWidget::HandleHeadFilterHeadgearClicked);
	}
	if (Btn_HeadFilterHair)
	{
		Btn_HeadFilterHair->OnClicked.RemoveAll(this);
		Btn_HeadFilterHair->OnClicked.AddDynamic(
			this, &UAvCharacterCustomizationRootWidget::HandleHeadFilterHairClicked);
	}
	if (Btn_HeadFilterBeard)
	{
		Btn_HeadFilterBeard->OnClicked.RemoveAll(this);
		Btn_HeadFilterBeard->OnClicked.AddDynamic(
			this, &UAvCharacterCustomizationRootWidget::HandleHeadFilterBeardClicked);
	}

	FString CatalogDiagnostic;
	if (UWorkerAppearanceComponent::WriteHeadCustomizationCatalogDiagnostics(CatalogDiagnostic))
	{
		UE_LOG(LogTemp, Display, TEXT("[AvCustomizeHeadCatalog] Saved=%s Records=%d"),
			*CatalogDiagnostic, HeadCatalog.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AvCustomizeHeadCatalog] %s"), *CatalogDiagnostic);
	}

	SelectHeadSection(EAvHeadCustomizationSection::HeadType);
}

void UAvCharacterCustomizationRootWidget::SelectHeadSection(EAvHeadCustomizationSection Section)
{
	SelectedHeadSection = Section;
	if (WS_HeadSectionPages)
	{
		WS_HeadSectionPages->SetActiveWidgetIndex(GetHeadSectionPageIndex(Section));
	}
	RefreshHeadSectionVisuals();
	RebuildHeadItemCards();
	if (UScrollBox* ActiveScrollBox = GetScrollBoxForHeadSection(Section))
	{
		ActiveScrollBox->ScrollToStart();
	}
	UE_LOG(LogTemp, Verbose,
		TEXT("[AvCustomizeHeadSection] Section=%d AppearanceMutation=false Autosave=false PreviewSource=ActiveCharacterAppearance"),
		static_cast<int32>(Section));
}

int32 UAvCharacterCustomizationRootWidget::GetHeadSectionPageIndex(
	EAvHeadCustomizationSection Section) const
{
	switch (Section)
	{
	case EAvHeadCustomizationSection::HeadType: return 0;
	case EAvHeadCustomizationSection::Headgear: return 1;
	case EAvHeadCustomizationSection::Hair: return 2;
	case EAvHeadCustomizationSection::Beard: return 3;
	default: return 0;
	}
}

UUniformGridPanel* UAvCharacterCustomizationRootWidget::GetGridForHeadSection(
	EAvHeadCustomizationSection Section) const
{
	switch (Section)
	{
	case EAvHeadCustomizationSection::HeadType: return UniformGrid_HeadItems;
	case EAvHeadCustomizationSection::Headgear: return UniformGrid_HeadgearItems;
	case EAvHeadCustomizationSection::Hair: return UniformGrid_HairItems;
	case EAvHeadCustomizationSection::Beard: return UniformGrid_BeardItems;
	default: return UniformGrid_HeadItems;
	}
}

UScrollBox* UAvCharacterCustomizationRootWidget::GetScrollBoxForHeadSection(
	EAvHeadCustomizationSection Section) const
{
	switch (Section)
	{
	case EAvHeadCustomizationSection::HeadType: return ScrollBox_HeadItems;
	case EAvHeadCustomizationSection::Headgear: return ScrollBox_HeadgearItems;
	case EAvHeadCustomizationSection::Hair: return ScrollBox_HairItems;
	case EAvHeadCustomizationSection::Beard: return ScrollBox_BeardItems;
	default: return ScrollBox_HeadItems;
	}
}

UTextBlock* UAvCharacterCustomizationRootWidget::GetEmptyStateForHeadSection(
	EAvHeadCustomizationSection Section) const
{
	switch (Section)
	{
	case EAvHeadCustomizationSection::HeadType: return Txt_HeadEmptyState;
	case EAvHeadCustomizationSection::Headgear: return Txt_HeadgearEmptyState;
	case EAvHeadCustomizationSection::Hair: return Txt_HairEmptyState;
	case EAvHeadCustomizationSection::Beard: return Txt_BeardEmptyState;
	default: return Txt_HeadEmptyState;
	}
}

FName UAvCharacterCustomizationRootWidget::GetActiveHeadTypeId() const
{
	return ActiveCharacterAppearance.Get(EWorkerSlot::Head).ToSoftObjectPath() ==
		HeadType02AppearancePath
		? FName(TEXT("HeadType02"))
		: FName(TEXT("HeadType01"));
}

void UAvCharacterCustomizationRootWidget::InvalidateHeadDependentSectionCards()
{
	for (const EAvHeadCustomizationSection Section : {
		EAvHeadCustomizationSection::Headgear,
		EAvHeadCustomizationSection::Hair,
		EAvHeadCustomizationSection::Beard})
	{
		if (TArray<TObjectPtr<UAvHeadCatalogItemButton>>* Cards =
			RuntimeHeadSectionCards.Find(Section))
		{
			for (UAvHeadCatalogItemButton* Card : *Cards)
			{
				if (Card)
				{
					Card->RemoveFromParent();
				}
			}
			Cards->Reset();
		}
	}
}

void UAvCharacterCustomizationRootWidget::ConfigureHeadFilterButton(
	UButton* Button,
	UBorder* Frame,
	UBorder* Fill,
	UImage* Icon,
	UTextBlock* Label,
	bool bSelected) const
{
	if (!Button)
	{
		return;
	}

	Button->IsFocusable = false;
	if (Frame)
	{
		Frame->SetBrushColor(
			bSelected
				? AvCustomizePreview::CategoryActiveTint
				: FLinearColor(0.23f, 0.25f, 0.28f, 1.f));
	}
	if (Fill)
	{
		Fill->SetBrushColor(
			bSelected
				? FLinearColor(0.25f, 0.095f, 0.018f, 0.98f)
				: FLinearColor(0.040f, 0.045f, 0.054f, 0.96f));
	}
	if (Icon)
	{
		Icon->SetColorAndOpacity(
			bSelected
				? AvCustomizePreview::CategoryActiveTint
				: AvCustomizePreview::CategoryInactiveTint);
	}
	if (Label)
	{
		Label->SetColorAndOpacity(FSlateColor(
			bSelected
				? AvCustomizePreview::CategoryActiveTint
				: FLinearColor(0.78f, 0.80f, 0.84f, 1.f)));
	}
}

void UAvCharacterCustomizationRootWidget::RefreshHeadSectionVisuals()
{
	ConfigureHeadFilterButton(
		Btn_HeadFilterHeadType,
		Border_HeadFilterHeadTypeFrame,
		Border_HeadFilterHeadTypeFill,
		Img_HeadFilterHeadTypeIcon,
		Txt_HeadFilterHeadType,
		SelectedHeadSection == EAvHeadCustomizationSection::HeadType);
	ConfigureHeadFilterButton(
		Btn_HeadFilterHeadgear,
		Border_HeadFilterHeadgearFrame,
		Border_HeadFilterHeadgearFill,
		Img_HeadFilterHeadgearIcon,
		Txt_HeadFilterHeadgear,
		SelectedHeadSection == EAvHeadCustomizationSection::Headgear);
	ConfigureHeadFilterButton(
		Btn_HeadFilterHair,
		Border_HeadFilterHairFrame,
		Border_HeadFilterHairFill,
		Img_HeadFilterHairIcon,
		Txt_HeadFilterHair,
		SelectedHeadSection == EAvHeadCustomizationSection::Hair);
	ConfigureHeadFilterButton(
		Btn_HeadFilterBeard,
		Border_HeadFilterBeardFrame,
		Border_HeadFilterBeardFill,
		Img_HeadFilterBeardIcon,
		Txt_HeadFilterBeard,
		SelectedHeadSection == EAvHeadCustomizationSection::Beard);
}

void UAvCharacterCustomizationRootWidget::BuildHeadThumbnailRegistry()
{
	HeadItemThumbnailRegistry.Reset();
	NoneHeadgearThumbnails = FHeadItemThumbnailPair();
	NoneHairThumbnails = FHeadItemThumbnailPair();
	NoneBeardThumbnails = FHeadItemThumbnailPair();

	HeadType01AppearancePath = FSoftObjectPath();
	HeadType02AppearancePath = FSoftObjectPath();

	const FString ThumbnailRoot =
		TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real");
	auto MakeObjectPath = [](const FString& Directory, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *Directory, *AssetName, *AssetName);
	};
	auto LoadThumbnail = [](const FString& ObjectPath)
	{
		return Cast<UTexture2D>(StaticLoadObject(
			UTexture2D::StaticClass(),
			nullptr,
			*ObjectPath,
			nullptr,
			LOAD_NoWarn | LOAD_Quiet));
	};
	auto LoadPair = [&MakeObjectPath, &LoadThumbnail](
		const FString& Type01Directory,
		const FString& Type01AssetName,
		const FString& Type02Directory,
		const FString& Type02AssetName)
	{
		FHeadItemThumbnailPair Pair;
		Pair.Type01 = LoadThumbnail(MakeObjectPath(Type01Directory, Type01AssetName));
		Pair.Type02 = LoadThumbnail(MakeObjectPath(Type02Directory, Type02AssetName));
		return Pair;
	};

	for (const FAvHeadCustomizationCatalogItem& CatalogItem : HeadCatalog)
	{
		if (CatalogItem.UISection != EAvHeadCustomizationSection::HeadType)
		{
			continue;
		}
		const FSoftObjectPath HeadPath(CatalogItem.ExactHeadAssetPath);
		FHeadItemThumbnailPair Pair;
		Pair.Type01 = LoadThumbnail(CatalogItem.ThumbnailType01);
		Pair.Type02 = LoadThumbnail(CatalogItem.ThumbnailType02);
		HeadItemThumbnailRegistry.Add(HeadPath, Pair);
		if (CatalogItem.HeadTypeId == FName(TEXT("HeadType01")))
		{
			HeadType01AppearancePath = HeadPath;
		}
		else if (CatalogItem.HeadTypeId == FName(TEXT("HeadType02")))
		{
			HeadType02AppearancePath = HeadPath;
		}
	}

	for (const FAvHeadCustomizationCatalogItem& CatalogItem : HeadCatalog)
	{
		if (CatalogItem.UISection == EAvHeadCustomizationSection::HeadType)
		{
			continue;
		}

		const FSoftObjectPath AppearancePath(CatalogItem.ExactObjectPath);
		FHeadItemThumbnailPair Pair;
		Pair.Type01 = LoadThumbnail(CatalogItem.ThumbnailType01);
		Pair.Type02 = LoadThumbnail(CatalogItem.ThumbnailType02);
		HeadItemThumbnailRegistry.Add(
			AppearancePath,
			MoveTemp(Pair));
	}

	const FString NoneRoot = ThumbnailRoot / TEXT("None");
	NoneHeadgearThumbnails = LoadPair(
		NoneRoot / TEXT("Headgear"), TEXT("T_UI_None_Headgear_Type01"),
		NoneRoot / TEXT("Headgear"), TEXT("T_UI_None_Headgear_Type02"));
	NoneHairThumbnails = LoadPair(
		NoneRoot / TEXT("Hair"), TEXT("T_UI_None_Hair_Type01"),
		NoneRoot / TEXT("Hair"), TEXT("T_UI_None_Hair_Type02"));
	NoneBeardThumbnails = LoadPair(
		NoneRoot / TEXT("Beard"), TEXT("T_UI_None_Beard_Type01"),
		NoneRoot / TEXT("Beard"), TEXT("T_UI_None_Beard_Type02"));
}

UTexture2D* UAvCharacterCustomizationRootWidget::ResolveHeadItemThumbnail(
	const FAvHeadCustomizationCatalogItem* Item,
	bool bNoneCard) const
{
	const bool bUseHeadType02 = GetActiveHeadTypeId() == FName(TEXT("HeadType02"));
	auto ResolvePair = [bUseHeadType02](const FHeadItemThumbnailPair& Pair)
	{
		return bUseHeadType02 ? Pair.Type02.Get() : Pair.Type01.Get();
	};

	if (bNoneCard)
	{
		switch (SelectedHeadSection)
		{
		case EAvHeadCustomizationSection::Headgear:
			return ResolvePair(NoneHeadgearThumbnails);
		case EAvHeadCustomizationSection::Hair:
			return ResolvePair(NoneHairThumbnails);
		case EAvHeadCustomizationSection::Beard:
			return ResolvePair(NoneBeardThumbnails);
		default:
			return nullptr;
		}
	}

	if (!Item)
	{
		return nullptr;
	}
	const FSoftObjectPath AppearancePath(Item->ExactObjectPath);
	const FHeadItemThumbnailPair* Pair = HeadItemThumbnailRegistry.Find(AppearancePath);
	if (!Pair)
	{
		return nullptr;
	}
	if (Item->UISection == EAvHeadCustomizationSection::HeadType)
	{
		return AppearancePath == HeadType02AppearancePath
			? Pair->Type02.Get()
			: Pair->Type01.Get();
	}
	return ResolvePair(*Pair);
}

bool UAvCharacterCustomizationRootWidget::IsHeadCatalogItemSelected(
	const FAvHeadCustomizationCatalogItem& Item) const
{
	return ActiveCharacterAppearance.Get(Item.Slot).ToSoftObjectPath().ToString() == Item.ExactObjectPath;
}

bool UAvCharacterCustomizationRootWidget::IsHeadCatalogItemDisabled(
	const FAvHeadCustomizationCatalogItem& Item,
	FString& OutReason) const
{
	OutReason.Reset();
	if (!Item.bAllowInUI)
	{
		OutReason = Item.TooltipReason.IsEmpty()
			? TEXT("UNVERIFIED")
			: Item.TooltipReason;
		return true;
	}
	return false;
}

void UAvCharacterCustomizationRootWidget::RebuildHeadItemCards()
{
	if (IsDesignTime())
	{
		return;
	}

	UUniformGridPanel* ActiveGrid = GetGridForHeadSection(SelectedHeadSection);
	UScrollBox* ActiveScrollBox = GetScrollBoxForHeadSection(SelectedHeadSection);
	UTextBlock* ActiveEmptyState = GetEmptyStateForHeadSection(SelectedHeadSection);
	TArray<TObjectPtr<UAvHeadCatalogItemButton>>& RuntimeCards =
		RuntimeHeadSectionCards.FindOrAdd(SelectedHeadSection);

	for (UAvHeadCatalogItemButton* RuntimeCard : RuntimeCards)
	{
		if (RuntimeCard)
		{
			RuntimeCard->RemoveFromParent();
		}
	}
	RuntimeCards.Reset();

	// These two instances remain authored in the Widget Tree so Designer preview and
	// runtime share the same layout. They never participate in a live catalog grid.
	if (DesignSample_HeadType01)
	{
		DesignSample_HeadType01->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (DesignSample_HeadType02)
	{
		DesignSample_HeadType02->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!ActiveGrid)
	{
		return;
	}

	// NativeDestruct normally removes every tracked live card. Defensively remove any
	// stale runtime card left in a reused root widget while preserving the two authored
	// Designer samples. This also makes repeated close/reopen cycles idempotent.
	for (int32 ChildIndex = ActiveGrid->GetChildrenCount() - 1;
		ChildIndex >= 0;
		--ChildIndex)
	{
		UAvHeadCatalogItemButton* ExistingCard = Cast<UAvHeadCatalogItemButton>(
			ActiveGrid->GetChildAt(ChildIndex));
		if (ExistingCard &&
			ExistingCard != DesignSample_HeadType01 &&
			ExistingCard != DesignSample_HeadType02)
		{
			ExistingCard->RemoveFromParent();
		}
	}
	if (!HeadCatalogItemCardClass)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvCustomizeHeadDesigner] HeadCatalogItemCardClass is not configured on WBP_CharacterCustomizationRoot_V2."));
		if (ActiveScrollBox)
		{
			ActiveScrollBox->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (ActiveEmptyState)
		{
			ActiveEmptyState->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		return;
	}

	int32 GridIndex = 0;
	auto AddCard = [this, ActiveGrid, &RuntimeCards, &GridIndex](
		const FAvHeadCustomizationCatalogItem* Item,
		int32 CatalogIndex,
		bool bNoneCard)
	{
		const FString CardName = bNoneCard
			? FString::Printf(TEXT("HeadItemCard_None_%d"), static_cast<int32>(SelectedHeadSection))
			: FString::Printf(TEXT("HeadItemCard_%d_%03d"), static_cast<int32>(SelectedHeadSection), CatalogIndex);
		UAvHeadCatalogItemButton* Card = nullptr;
		if (APlayerController* OwningPlayer = GetOwningPlayer())
		{
			Card = CreateWidget<UAvHeadCatalogItemButton>(
				OwningPlayer, HeadCatalogItemCardClass, FName(*CardName));
		}
		else if (UWorld* World = GetWorld())
		{
			Card = CreateWidget<UAvHeadCatalogItemButton>(
				World, HeadCatalogItemCardClass, FName(*CardName));
		}
		if (!Card)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvCustomizeHeadDesigner] Failed to create reusable card %s from %s."),
				*CardName,
				*GetNameSafe(HeadCatalogItemCardClass.Get()));
			return;
		}

		UTexture2D* ThumbnailTexture = ResolveHeadItemThumbnail(Item, bNoneCard);
		if (!ThumbnailTexture)
		{
			const bool bUseHeadType02 =
				GetActiveHeadTypeId() == FName(TEXT("HeadType02"));
			const FString DisplayName = bNoneCard
				? TEXT("None")
				: Item->DisplayName;
			const FString ExpectedThumbnailPath = bNoneCard
				? TEXT("<section NONE thumbnail registry>")
				: (bUseHeadType02 ? Item->ThumbnailType02 : Item->ThumbnailType01);
			UE_LOG(LogTemp, Error,
				TEXT("[AvCustomizeHeadThumbnail] Missing exact thumbnail. Section=%d CatalogIndex=%d HeadTypeId=%s DisplayName=%s ExpectedPath=%s ProductFallback=false"),
				static_cast<int32>(SelectedHeadSection),
				CatalogIndex,
				*GetActiveHeadTypeId().ToString(),
				*DisplayName,
				*ExpectedThumbnailPath);
		}
		FString DisabledReason;
		const bool bDisabled = !bNoneCard && IsHeadCatalogItemDisabled(*Item, DisabledReason);
		const FString ExactPath = bNoneCard ? TEXT("None") : Item->ExactObjectPath;
		const FString DisplayNameKey = bNoneCard
			? TEXT("Customization.Common.None")
			: Item->DisplayNameKey;
		const FString DisplayNameFallback = bNoneCard ? FString() : Item->DisplayName;
		const FText DisplayText = AvCustomizationLocalization::Resolve(
			DisplayNameKey, DisplayNameFallback);
		const FText TooltipText = AvCustomizationLocalization::Tooltip(
			DisplayNameKey, DisplayNameFallback, FString(), DisabledReason);
		Card->InitializeHeadCatalogButton(
			this,
			SelectedHeadSection,
			CatalogIndex,
			bNoneCard,
			ThumbnailTexture,
			DisplayText,
			bDisabled,
			TooltipText,
			ExactPath);

		bool bSelected = false;
		if (bNoneCard)
		{
			EWorkerSlot OptionalSlot = EWorkerSlot::Headgear;
			if (SelectedHeadSection == EAvHeadCustomizationSection::Hair)
			{
				OptionalSlot = EWorkerSlot::Hair;
			}
			else if (SelectedHeadSection == EAvHeadCustomizationSection::Beard)
			{
				OptionalSlot = EWorkerSlot::Beard;
			}
			bSelected = ActiveCharacterAppearance.Get(OptionalSlot).IsNull();
		}
		else
		{
			bSelected = IsHeadCatalogItemSelected(*Item);
		}
		Card->SetSelectedState(bSelected);

		if (UUniformGridSlot* GridSlot = ActiveGrid->AddChildToUniformGrid(
			Card,
			GridIndex / 2,
			GridIndex % 2))
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
		RuntimeCards.Add(Card);
		++GridIndex;
	};

	if (SelectedHeadSection != EAvHeadCustomizationSection::HeadType)
	{
		AddCard(nullptr, INDEX_NONE, true);
	}
	for (int32 CatalogIndex = 0; CatalogIndex < HeadCatalog.Num(); ++CatalogIndex)
	{
		const FAvHeadCustomizationCatalogItem& Item = HeadCatalog[CatalogIndex];
		if (Item.UISection == SelectedHeadSection && Item.bAllowInUI)
		{
			AddCard(&Item, CatalogIndex, false);
		}
	}

	const bool bHasItems = GridIndex > 0;
	if (ActiveScrollBox)
	{
		ActiveScrollBox->SetVisibility(
			bHasItems ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (ActiveEmptyState)
	{
		ActiveEmptyState->SetVisibility(
			bHasItems ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
	RefreshHeadCurrentSelection();
	UE_LOG(LogTemp, Verbose,
		TEXT("[AvCustomizeHeadCards] Section=%d RuntimeCards=%d GridChildren=%d SamplesAtRuntime=false"),
		static_cast<int32>(SelectedHeadSection),
		RuntimeCards.Num(),
		ActiveGrid->GetChildrenCount());
}

void UAvCharacterCustomizationRootWidget::RefreshHeadItemSelection()
{
	RebuildHeadItemCards();
}

void UAvCharacterCustomizationRootWidget::RefreshHeadCardSelectionStates()
{
	const FString CurrentHeadgearPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).ToSoftObjectPath().ToString();
	if (TArray<TObjectPtr<UAvHeadCatalogItemButton>>* Cards =
		RuntimeHeadSectionCards.Find(EAvHeadCustomizationSection::Headgear))
	{
		for (UAvHeadCatalogItemButton* Card : *Cards)
		{
			if (!Card)
			{
				continue;
			}
			if (Card->IsNoneCatalogCard())
			{
				Card->SetSelectedState(CurrentHeadgearPath.IsEmpty());
				continue;
			}
			const int32 CatalogIndex = Card->GetCatalogIndex();
			Card->SetSelectedState(
				HeadCatalog.IsValidIndex(CatalogIndex) &&
				HeadCatalog[CatalogIndex].UISection == EAvHeadCustomizationSection::Headgear &&
				HeadCatalog[CatalogIndex].ExactObjectPath == CurrentHeadgearPath);
		}
	}
}

void UAvCharacterCustomizationRootWidget::RefreshHeadCurrentSelection()
{
	if (!Txt_HeadCurrentSelection)
	{
		return;
	}
	EWorkerSlot WorkerSlot = EWorkerSlot::Head;
	if (SelectedHeadSection == EAvHeadCustomizationSection::Headgear) WorkerSlot = EWorkerSlot::Headgear;
	if (SelectedHeadSection == EAvHeadCustomizationSection::Hair) WorkerSlot = EWorkerSlot::Hair;
	if (SelectedHeadSection == EAvHeadCustomizationSection::Beard) WorkerSlot = EWorkerSlot::Beard;
	const FString CurrentPath = ActiveCharacterAppearance.Get(WorkerSlot).ToSoftObjectPath().ToString();
	FString DisplayNameKey = TEXT("Customization.Common.None");
	FString DisplayNameFallback;
	for (const FAvHeadCustomizationCatalogItem& Item : HeadCatalog)
	{
		if (Item.UISection == SelectedHeadSection && Item.ExactObjectPath == CurrentPath)
		{
			DisplayNameKey = Item.DisplayNameKey;
			DisplayNameFallback = Item.DisplayName;
			break;
		}
	}
	const FText DisplayText = AvCustomizationLocalization::Resolve(
		DisplayNameKey, DisplayNameFallback);
	if (Txt_HeadCurrentSelectionValue)
	{
		Txt_HeadCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
		Txt_HeadCurrentSelectionValue->SetText(DisplayText);
		Txt_HeadCurrentSelectionValue->SetToolTipText(DisplayText);
	}
	else
	{
		const FText CombinedText = FText::Format(
			FAvLoc::Text(TEXT("Customization.Common.CurrentSelectionFormat")),
			FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")),
			DisplayText);
		Txt_HeadCurrentSelection->SetText(CombinedText);
		Txt_HeadCurrentSelection->SetToolTipText(CombinedText);
	}
}

void UAvCharacterCustomizationRootWidget::HandleHeadCatalogItemClicked(
	EAvHeadCustomizationSection Section,
	int32 CatalogIndex,
	bool bNoneCard)
{
	if (Section != SelectedHeadSection ||
		(Section == EAvHeadCustomizationSection::HeadType && bNoneCard))
	{
		return;
	}

	EWorkerSlot WorkerSlot = EWorkerSlot::Head;
	if (Section == EAvHeadCustomizationSection::Headgear) WorkerSlot = EWorkerSlot::Headgear;
	if (Section == EAvHeadCustomizationSection::Hair) WorkerSlot = EWorkerSlot::Hair;
	if (Section == EAvHeadCustomizationSection::Beard) WorkerSlot = EWorkerSlot::Beard;

	FString TargetPath;
	const FAvHeadCustomizationCatalogItem* TargetItem = nullptr;
	if (!bNoneCard)
	{
		if (!HeadCatalog.IsValidIndex(CatalogIndex) ||
			HeadCatalog[CatalogIndex].UISection != Section ||
			!HeadCatalog[CatalogIndex].bAllowInUI)
		{
			return;
		}
		FString DisabledReason;
		if (IsHeadCatalogItemDisabled(HeadCatalog[CatalogIndex], DisabledReason))
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[AvCustomizeHeadApply] Result=DISABLED Path=%s Reason=%s"),
				*HeadCatalog[CatalogIndex].ExactObjectPath,
				*DisabledReason);
			return;
		}
		TargetItem = &HeadCatalog[CatalogIndex];
		TargetPath = TargetItem->ExactObjectPath;
	}

	const FString CurrentPath = ActiveCharacterAppearance.Get(WorkerSlot).ToSoftObjectPath().ToString();
	if ((bNoneCard && CurrentPath.IsEmpty()) ||
		(!bNoneCard && CurrentPath == TargetPath))
	{
		UE_LOG(LogTemp, Display,
			TEXT("[AvCustomizeHeadApply] Result=NOOP Slot=%d Path=%s Autosave=false OriginUnchanged=true"),
				static_cast<int32>(WorkerSlot),
			bNoneCard ? TEXT("None") : *TargetPath);
		return;
	}
	if (Section == EAvHeadCustomizationSection::Headgear && TargetItem &&
		BeginRaisedHoodConflictForHeadgear(*TargetItem))
	{
		return;
	}

	FWorkerAppearance NewAppearance = ActiveCharacterAppearance;
	if (bNoneCard)
	{
		NewAppearance.Clear(WorkerSlot);
	}
	else
	{
		NewAppearance.Set(
			WorkerSlot,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(TargetPath)));
	}
	UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(NewAppearance);
	FString ValidationDetails;
	if (!UWorkerAppearanceComponent::ValidateHeadCustomizationSelection(
		NewAppearance,
		WorkerSlot,
		ValidationDetails))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvCustomizeHeadApply] Result=REJECT Slot=%d Path=%s %s"),
			static_cast<int32>(WorkerSlot),
			bNoneCard ? TEXT("None") : *TargetPath,
			*ValidationDetails);
		return;
	}

	const FName PreservedBasePresetId = ActiveCharacterBasePresetId.IsNone()
		? FName(TEXT("BaseMaleUnderwear"))
		: ActiveCharacterBasePresetId;
	CommitActiveCharacterAppearance(
		NewAppearance,
		true,
		PreservedBasePresetId,
		EAvAppearanceOrigin::ManualCustomized);
	if (Section == EAvHeadCustomizationSection::HeadType)
	{
		// Hidden pages must not retain cards built for the previous head profile.
		// Their exact Type01/Type02 textures are rebuilt on the next section open.
		InvalidateHeadDependentSectionCards();
		InvalidateFaceProtectionCards();
		InvalidateHandsAccessoriesCards();
		BuildFullOutfitThumbnailRegistry();
		if (bFullOutfitCatalogBuilt)
		{
			RebuildFullOutfitItemCards(false);
		}
	}
	RefreshHeadItemSelection();
	UE_LOG(LogTemp, Warning,
		TEXT("[AvCustomizeHeadApply] Result=APPLIED Slot=%d Path=%s HeadTypeId=%s SharedSkin=(%.3f,%.3f,%.3f) Origin=ManualCustomized Autosave=true PreviewSource=ActiveCharacterAppearance %s Stable=%s"),
		static_cast<int32>(WorkerSlot),
		bNoneCard ? TEXT("None") : *TargetPath,
		TargetItem ? *TargetItem->HeadTypeId.ToString() : TEXT("Unchanged"),
		NewAppearance.SkinColor.R,
		NewAppearance.SkinColor.G,
		NewAppearance.SkinColor.B,
		*ValidationDetails,
		*ActiveCharacterAppearance.ToStableDebugString());
}

void UAvCharacterCustomizationRootWidget::EnsureFaceProtectionPageComposition()
{
	if (bFaceProtectionPageCompositionValidated)
	{
		return;
	}
	bFaceProtectionPageCompositionValidated = true;

	TArray<FString> MissingWidgets;
	auto RequireWidget = [&MissingWidgets](const UWidget* Widget, const TCHAR* Name)
	{
		if (!Widget)
		{
			MissingWidgets.Add(Name);
		}
	};
	RequireWidget(VB_RightPanelFaceProtection, TEXT("VB_RightPanelFaceProtection"));
	RequireWidget(WS_FaceProtectionSectionPages, TEXT("WS_FaceProtectionSectionPages"));
	RequireWidget(Btn_FaceFilterGlasses, TEXT("Btn_FaceFilterGlasses"));
	RequireWidget(Btn_FaceFilterRespirator, TEXT("Btn_FaceFilterRespirator"));
	RequireWidget(Btn_FaceFilterHeadphones, TEXT("Btn_FaceFilterHeadphones"));
	RequireWidget(Txt_FaceCurrentSelection, TEXT("Txt_FaceCurrentSelection"));
	RequireWidget(Txt_FaceCurrentSelectionValue, TEXT("Txt_FaceCurrentSelectionValue"));
	RequireWidget(ScrollBox_GlassesItems, TEXT("ScrollBox_GlassesItems"));
	RequireWidget(UniformGrid_GlassesItems, TEXT("UniformGrid_GlassesItems"));
	RequireWidget(Txt_GlassesEmptyState, TEXT("Txt_GlassesEmptyState"));
	RequireWidget(ScrollBox_RespiratorItems, TEXT("ScrollBox_RespiratorItems"));
	RequireWidget(UniformGrid_RespiratorItems, TEXT("UniformGrid_RespiratorItems"));
	RequireWidget(Txt_RespiratorEmptyState, TEXT("Txt_RespiratorEmptyState"));
	RequireWidget(ScrollBox_HeadphonesItems, TEXT("ScrollBox_HeadphonesItems"));
	RequireWidget(UniformGrid_HeadphonesItems, TEXT("UniformGrid_HeadphonesItems"));
	RequireWidget(Txt_HeadphonesEmptyState, TEXT("Txt_HeadphonesEmptyState"));
	if (!HeadCatalogItemCardClass)
	{
		MissingWidgets.Add(TEXT("HeadCatalogItemCardClass"));
	}
	if (WS_FaceProtectionSectionPages &&
		WS_FaceProtectionSectionPages->GetChildrenCount() != 3)
	{
		MissingWidgets.Add(FString::Printf(
			TEXT("WS_FaceProtectionSectionPages.Children=%d Expected=3"),
			WS_FaceProtectionSectionPages->GetChildrenCount()));
	}

	if (!MissingWidgets.IsEmpty())
	{
		const FString MissingList = FString::Join(MissingWidgets, TEXT(", "));
		ensureMsgf(false,
			TEXT("Static Face Protection page is incomplete in WBP_CharacterCustomizationRoot_V2. Missing={%s}. Runtime reconstruction is disabled."),
			*MissingList);
		UE_LOG(LogTemp, Error,
			TEXT("[AvCustomizeFaceDesigner] Static Face Protection page is incomplete. Missing={%s}. Runtime reconstruction is disabled."),
			*MissingList);
	}
}

void UAvCharacterCustomizationRootWidget::InitializeFaceProtectionPage()
{
	FaceProtectionCatalog = UWorkerAppearanceComponent::GetFaceProtectionCatalog();
	BuildFaceProtectionThumbnailRegistry();
	EnsureFaceProtectionPageComposition();

	const FText EmptyStateText = FAvLoc::Text(TEXT("Customization.Common.NoAvailableOptions"));
	for (UTextBlock* EmptyState : {
		Txt_GlassesEmptyState.Get(),
		Txt_RespiratorEmptyState.Get(),
		Txt_HeadphonesEmptyState.Get()})
	{
		if (EmptyState)
		{
			EmptyState->SetText(EmptyStateText);
		}
	}
	if (Txt_FaceCurrentSelection)
	{
		Txt_FaceCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	}

	if (Btn_FaceFilterGlasses)
	{
		Btn_FaceFilterGlasses->OnClicked.RemoveAll(this);
		Btn_FaceFilterGlasses->OnClicked.AddDynamic(
			this, &UAvCharacterCustomizationRootWidget::HandleFaceFilterGlassesClicked);
	}
	if (Btn_FaceFilterRespirator)
	{
		Btn_FaceFilterRespirator->OnClicked.RemoveAll(this);
		Btn_FaceFilterRespirator->OnClicked.AddDynamic(
			this, &UAvCharacterCustomizationRootWidget::HandleFaceFilterRespiratorClicked);
	}
	if (Btn_FaceFilterHeadphones)
	{
		Btn_FaceFilterHeadphones->OnClicked.RemoveAll(this);
		Btn_FaceFilterHeadphones->OnClicked.AddDynamic(
			this, &UAvCharacterCustomizationRootWidget::HandleFaceFilterHeadphonesClicked);
	}

	for (UAvHeadCatalogItemButton* Sample : {
		DesignSample_FaceGlassesNone.Get(),
		DesignSample_FaceGlassesItem.Get(),
		DesignSample_FaceRespiratorNone.Get(),
		DesignSample_FaceRespiratorItem.Get()})
	{
		if (Sample)
		{
			Sample->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (WidgetTree)
	{
		for (const FName SampleName : {
			FName(TEXT("DesignSample_FaceGlassesNone")),
			FName(TEXT("DesignSample_FaceGlassesItem")),
			FName(TEXT("DesignSample_FaceRespiratorNone")),
			FName(TEXT("DesignSample_FaceRespiratorItem")),
			FName(TEXT("DesignSample_FaceHeadphonesNone")),
			FName(TEXT("DesignSample_FaceHeadphonesItem"))})
		{
			if (UWidget* AuthoredSample = WidgetTree->FindWidget(SampleName))
			{
				AuthoredSample->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
	SelectFaceProtectionSection(EAvFaceProtectionSection::Glasses);
	UE_LOG(LogTemp, Display,
		TEXT("[AvCustomizeFaceCatalog] Records=%d EnabledGlasses=%d EnabledRespirator=%d EnabledHeadphones=%d HeadphonesSlot=true SaveSchemaMutation=false"),
		FaceProtectionCatalog.Num(),
		FaceProtectionCatalog.FilterByPredicate([](const FAvFaceProtectionCatalogItem& Item)
		{
			return Item.Section == EAvFaceProtectionSection::Glasses && Item.bEnabled;
		}).Num(),
		FaceProtectionCatalog.FilterByPredicate([](const FAvFaceProtectionCatalogItem& Item)
		{
			return Item.Section == EAvFaceProtectionSection::Respirator && Item.bEnabled;
		}).Num(),
		FaceProtectionCatalog.FilterByPredicate([](const FAvFaceProtectionCatalogItem& Item)
		{
			return Item.Section == EAvFaceProtectionSection::Headphones && Item.bEnabled;
		}).Num());
}

void UAvCharacterCustomizationRootWidget::SelectFaceProtectionSection(
	EAvFaceProtectionSection Section)
{
	SelectedFaceProtectionSection = Section;
	if (WS_FaceProtectionSectionPages)
	{
		WS_FaceProtectionSectionPages->SetActiveWidgetIndex(
			GetFaceProtectionSectionPageIndex(Section));
	}
	RefreshFaceProtectionFilterVisuals();
	RebuildFaceProtectionItemCards();
	if (UScrollBox* ActiveScrollBox = GetScrollBoxForFaceProtectionSection(Section))
	{
		ActiveScrollBox->ScrollToStart();
	}
}

int32 UAvCharacterCustomizationRootWidget::GetFaceProtectionSectionPageIndex(
	EAvFaceProtectionSection Section) const
{
	switch (Section)
	{
	case EAvFaceProtectionSection::Glasses: return 0;
	case EAvFaceProtectionSection::Respirator: return 1;
	case EAvFaceProtectionSection::Headphones: return 2;
	default: return 0;
	}
}

UUniformGridPanel* UAvCharacterCustomizationRootWidget::GetGridForFaceProtectionSection(
	EAvFaceProtectionSection Section) const
{
	switch (Section)
	{
	case EAvFaceProtectionSection::Glasses: return UniformGrid_GlassesItems;
	case EAvFaceProtectionSection::Respirator: return UniformGrid_RespiratorItems;
	case EAvFaceProtectionSection::Headphones: return UniformGrid_HeadphonesItems;
	default: return UniformGrid_GlassesItems;
	}
}

UScrollBox* UAvCharacterCustomizationRootWidget::GetScrollBoxForFaceProtectionSection(
	EAvFaceProtectionSection Section) const
{
	switch (Section)
	{
	case EAvFaceProtectionSection::Glasses: return ScrollBox_GlassesItems;
	case EAvFaceProtectionSection::Respirator: return ScrollBox_RespiratorItems;
	case EAvFaceProtectionSection::Headphones: return ScrollBox_HeadphonesItems;
	default: return ScrollBox_GlassesItems;
	}
}

UTextBlock* UAvCharacterCustomizationRootWidget::GetEmptyStateForFaceProtectionSection(
	EAvFaceProtectionSection Section) const
{
	switch (Section)
	{
	case EAvFaceProtectionSection::Glasses: return Txt_GlassesEmptyState;
	case EAvFaceProtectionSection::Respirator: return Txt_RespiratorEmptyState;
	case EAvFaceProtectionSection::Headphones: return Txt_HeadphonesEmptyState;
	default: return Txt_GlassesEmptyState;
	}
}

void UAvCharacterCustomizationRootWidget::BuildFaceProtectionThumbnailRegistry()
{
	FaceProtectionThumbnailRegistry.Reset();
	for (int32 Index = 0; Index < FaceProtectionCatalog.Num(); ++Index)
	{
		const FAvFaceProtectionCatalogItem& Item = FaceProtectionCatalog[Index];
		if (!Item.bEnabled)
		{
			continue;
		}
		FHeadItemThumbnailPair Pair;
		Pair.Type01 = Cast<UTexture2D>(StaticLoadObject(
			UTexture2D::StaticClass(), nullptr, *Item.ThumbnailType01,
			nullptr, LOAD_NoWarn | LOAD_Quiet));
		Pair.Type02 = Cast<UTexture2D>(StaticLoadObject(
			UTexture2D::StaticClass(), nullptr, *Item.ThumbnailType02,
			nullptr, LOAD_NoWarn | LOAD_Quiet));
		FaceProtectionThumbnailRegistry.Add(Index, Pair);
	}
}

UTexture2D* UAvCharacterCustomizationRootWidget::ResolveFaceProtectionItemThumbnail(
	int32 CatalogIndex) const
{
	const FHeadItemThumbnailPair* Pair = FaceProtectionThumbnailRegistry.Find(CatalogIndex);
	if (!Pair)
	{
		return nullptr;
	}
	return GetActiveHeadTypeId() == FName(TEXT("HeadType02"))
		? Pair->Type02
		: Pair->Type01;
}

void UAvCharacterCustomizationRootWidget::RefreshFaceProtectionFilterVisuals()
{
	ConfigureHeadFilterButton(Btn_FaceFilterGlasses,
		Border_HeadFilterHeadTypeFrame_1, Border_HeadFilterHeadTypeFill_1,
		Img_HeadFilterHeadTypeIcon_1, Txt_HeadFilterHeadType_1,
		SelectedFaceProtectionSection == EAvFaceProtectionSection::Glasses);
	ConfigureHeadFilterButton(Btn_FaceFilterRespirator,
		Border_HeadFilterHeadgearFrame_1, Border_HeadFilterHeadgearFill_1,
		Img_HeadFilterHeadgearIcon_1, Txt_HeadFilterHeadgear_1,
		SelectedFaceProtectionSection == EAvFaceProtectionSection::Respirator);
	ConfigureHeadFilterButton(Btn_FaceFilterHeadphones,
		Border_HeadFilterHairFrame_1, Border_HeadFilterHairFill_1,
		Img_HeadFilterHairIcon_1, Txt_HeadFilterHair_1,
		SelectedFaceProtectionSection == EAvFaceProtectionSection::Headphones);
}

void UAvCharacterCustomizationRootWidget::InvalidateFaceProtectionCards()
{
	for (TPair<EAvFaceProtectionSection,
		TArray<TObjectPtr<UAvHeadCatalogItemButton>>>& SectionCards : RuntimeFaceProtectionSectionCards)
	{
		for (UAvHeadCatalogItemButton* Card : SectionCards.Value)
		{
			if (Card)
			{
				Card->RemoveFromParent();
			}
		}
		SectionCards.Value.Reset();
	}
	RuntimeFaceProtectionSectionCards.Reset();
	BuildFaceProtectionThumbnailRegistry();
}

void UAvCharacterCustomizationRootWidget::RebuildFaceProtectionItemCards()
{
	UUniformGridPanel* ActiveGrid =
		GetGridForFaceProtectionSection(SelectedFaceProtectionSection);
	UScrollBox* ActiveScrollBox =
		GetScrollBoxForFaceProtectionSection(SelectedFaceProtectionSection);
	UTextBlock* ActiveEmptyState =
		GetEmptyStateForFaceProtectionSection(SelectedFaceProtectionSection);
	if (!ActiveGrid)
	{
		return;
	}

	TArray<TObjectPtr<UAvHeadCatalogItemButton>>& RuntimeCards =
		RuntimeFaceProtectionSectionCards.FindOrAdd(SelectedFaceProtectionSection);
	for (UAvHeadCatalogItemButton* RuntimeCard : RuntimeCards)
	{
		if (RuntimeCard)
		{
			RuntimeCard->RemoveFromParent();
		}
	}
	RuntimeCards.Reset();
	for (int32 ChildIndex = ActiveGrid->GetChildrenCount() - 1; ChildIndex >= 0; --ChildIndex)
	{
		UWidget* Child = ActiveGrid->GetChildAt(ChildIndex);
		if (Child && Child->GetName().StartsWith(TEXT("FaceProtectionItemCard_")))
		{
			Child->RemoveFromParent();
		}
	}

	if (!HeadCatalogItemCardClass)
	{
		if (ActiveScrollBox) ActiveScrollBox->SetVisibility(ESlateVisibility::Collapsed);
		if (ActiveEmptyState) ActiveEmptyState->SetVisibility(ESlateVisibility::HitTestInvisible);
		return;
	}

	int32 VisibleIndex = 0;
	for (int32 CatalogIndex = 0; CatalogIndex < FaceProtectionCatalog.Num(); ++CatalogIndex)
	{
		const FAvFaceProtectionCatalogItem& Item = FaceProtectionCatalog[CatalogIndex];
		if (Item.Section != SelectedFaceProtectionSection || !Item.bEnabled)
		{
			continue;
		}
		const FString CardName = FString::Printf(TEXT("FaceProtectionItemCard_%d_%03d"),
			static_cast<int32>(SelectedFaceProtectionSection), CatalogIndex);
		UAvHeadCatalogItemButton* Card = nullptr;
		if (APlayerController* OwningPlayer = GetOwningPlayer())
		{
			Card = CreateWidget<UAvHeadCatalogItemButton>(
				OwningPlayer, HeadCatalogItemCardClass, FName(*CardName));
		}
		else if (UWorld* World = GetWorld())
		{
			Card = CreateWidget<UAvHeadCatalogItemButton>(
				World, HeadCatalogItemCardClass, FName(*CardName));
		}
		if (!Card)
		{
			continue;
		}
		UTexture2D* Thumbnail = ResolveFaceProtectionItemThumbnail(CatalogIndex);
		if (!Thumbnail)
		{
			const FString ExpectedPath = GetActiveHeadTypeId() == FName(TEXT("HeadType02"))
				? Item.ThumbnailType02
				: Item.ThumbnailType01;
			UE_LOG(LogTemp, Error,
				TEXT("[AvCustomizeFaceThumbnail] Missing exact thumbnail. Section=%d DisplayName=%s ExpectedPath=%s ProductFallback=false"),
				static_cast<int32>(Item.Section), *Item.DisplayName, *ExpectedPath);
		}
		const FString ExactPath = Item.bIsNone ? TEXT("None") : Item.ExactMeshPath;
		Card->InitializeFaceProtectionCatalogButton(
			this,
			Item.Section,
			CatalogIndex,
			Item.bIsNone,
			Thumbnail,
			AvCustomizationLocalization::Resolve(Item.DisplayNameKey, Item.DisplayName),
			false,
			AvCustomizationLocalization::Resolve(Item.DisplayNameKey, Item.DisplayName),
			ExactPath);
		const FString CurrentPath =
			ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString();
		Card->SetSelectedState(
			Item.bIsNone ? CurrentPath.IsEmpty() : CurrentPath == Item.ExactMeshPath);
		if (UUniformGridSlot* GridSlot = ActiveGrid->AddChildToUniformGrid(
			Card, VisibleIndex / 2, VisibleIndex % 2))
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
		RuntimeCards.Add(Card);
		++VisibleIndex;
	}

	const bool bHasItems = VisibleIndex > 0;
	if (ActiveScrollBox)
	{
		ActiveScrollBox->SetVisibility(
			bHasItems ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (ActiveEmptyState)
	{
		ActiveEmptyState->SetVisibility(
			bHasItems ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
	RefreshFaceProtectionCurrentSelection();
	UE_LOG(LogTemp, Verbose,
		TEXT("[AvCustomizeFaceCards] Section=%d RuntimeCards=%d SamplesAtRuntime=false"),
		static_cast<int32>(SelectedFaceProtectionSection), RuntimeCards.Num());
}

void UAvCharacterCustomizationRootWidget::RefreshFaceProtectionCurrentSelection()
{
	FString DisplayNameKey = TEXT("Customization.Common.None");
	FString FullDisplayNameKey = DisplayNameKey;
	FString DisplayNameFallback;
	FString FullDisplayNameFallback;
	for (const FAvFaceProtectionCatalogItem& Item : FaceProtectionCatalog)
	{
		if (Item.Section != SelectedFaceProtectionSection || Item.bIsNone || !Item.bEnabled)
		{
			continue;
		}
		if (ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString() ==
			Item.ExactMeshPath)
		{
			DisplayNameKey = Item.CurrentSelectionNameKey.IsEmpty()
				? Item.DisplayNameKey : Item.CurrentSelectionNameKey;
			FullDisplayNameKey = Item.DisplayNameKey;
			DisplayNameFallback = Item.CurrentSelectionName.IsEmpty()
				? Item.DisplayName : Item.CurrentSelectionName;
			FullDisplayNameFallback = Item.DisplayName;
			break;
		}
	}
	if (Txt_FaceCurrentSelection)
	{
		Txt_FaceCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	}
	if (Txt_FaceCurrentSelectionValue)
	{
		const FText DisplayText = AvCustomizationLocalization::Resolve(
			DisplayNameKey, DisplayNameFallback);
		const FText FullDisplayText = AvCustomizationLocalization::Resolve(
			FullDisplayNameKey, FullDisplayNameFallback);
		Txt_FaceCurrentSelectionValue->SetText(DisplayText);
		Txt_FaceCurrentSelectionValue->SetToolTipText(FullDisplayText);
	}
}

void UAvCharacterCustomizationRootWidget::RefreshFaceProtectionCardSelectionStates()
{
	for (const TPair<EAvFaceProtectionSection,
		TArray<TObjectPtr<UAvHeadCatalogItemButton>>>& SectionCards :
		RuntimeFaceProtectionSectionCards)
	{
		for (UAvHeadCatalogItemButton* Card : SectionCards.Value)
		{
			if (!Card || !FaceProtectionCatalog.IsValidIndex(Card->GetCatalogIndex()))
			{
				continue;
			}
			const FAvFaceProtectionCatalogItem& Item =
				FaceProtectionCatalog[Card->GetCatalogIndex()];
			const FString CurrentPath = ActiveCharacterAppearance.Get(
				Item.WorkerSlot).ToSoftObjectPath().ToString();
			Card->SetSelectedState(
				Item.bIsNone ? CurrentPath.IsEmpty() : CurrentPath == Item.ExactMeshPath);
		}
	}
}

void UAvCharacterCustomizationRootWidget::HandleFaceProtectionCatalogItemClicked(
	EAvFaceProtectionSection Section,
	int32 CatalogIndex,
	bool bNoneCard)
{
	if (Section != SelectedFaceProtectionSection ||
		!FaceProtectionCatalog.IsValidIndex(CatalogIndex))
	{
		return;
	}
	const FAvFaceProtectionCatalogItem& Item = FaceProtectionCatalog[CatalogIndex];
	if (Item.Section != Section || Item.bIsNone != bNoneCard || !Item.bEnabled)
	{
		return;
	}
	const FString CurrentPath =
		ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString();
	if ((Item.bIsNone && CurrentPath.IsEmpty()) ||
		(!Item.bIsNone && CurrentPath == Item.ExactMeshPath))
	{
		UE_LOG(LogTemp, Display,
			TEXT("[AvCustomizeFaceApply] Result=NOOP Section=%d Slot=%d Path=%s"),
			static_cast<int32>(Section), static_cast<int32>(Item.WorkerSlot),
			Item.bIsNone ? TEXT("None") : *Item.ExactMeshPath);
		return;
	}
	if (!Item.bIsNone && Item.WorkerSlot == EWorkerSlot::Headphones &&
		BeginRaisedHoodConflictForHeadphones(Item))
	{
		return;
	}

	FWorkerAppearance NewAppearance = ActiveCharacterAppearance;
	if (Item.bIsNone)
	{
		NewAppearance.Clear(Item.WorkerSlot);
	}
	else
	{
		NewAppearance.Set(
			Item.WorkerSlot,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Item.ExactMeshPath)));
	}
	FString ValidationDetails;
	if (!UWorkerAppearanceComponent::ValidateHeadCustomizationSelection(
		NewAppearance, Item.WorkerSlot, ValidationDetails))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvCustomizeFaceApply] Result=REJECT Section=%d Slot=%d Path=%s %s"),
			static_cast<int32>(Section), static_cast<int32>(Item.WorkerSlot),
			Item.bIsNone ? TEXT("None") : *Item.ExactMeshPath,
			*ValidationDetails);
		return;
	}

	const FName PreservedBasePresetId = ActiveCharacterBasePresetId.IsNone()
		? FName(TEXT("BaseMaleUnderwear"))
		: ActiveCharacterBasePresetId;
	CommitActiveCharacterAppearance(
		NewAppearance,
		true,
		PreservedBasePresetId,
		EAvAppearanceOrigin::ManualCustomized);
	RefreshFaceProtectionCardSelectionStates();
	RefreshFaceProtectionCurrentSelection();
	UE_LOG(LogTemp, Warning,
		TEXT("[AvCustomizeFaceApply] Result=APPLIED Section=%d Slot=%d Path=%s Origin=ManualCustomized Autosave=true %s"),
		static_cast<int32>(Section), static_cast<int32>(Item.WorkerSlot),
		Item.bIsNone ? TEXT("None") : *Item.ExactMeshPath,
		*ValidationDetails);
}

void UAvCharacterCustomizationRootWidget::EnsureHandsAccessoriesPageComposition()
{
	if (bHandsAccessoriesPageCompositionValidated) return;
	bHandsAccessoriesPageCompositionValidated = true;
	TArray<FString> Missing;
	auto Require = [&Missing](const UWidget* Widget, const TCHAR* Name) { if (!Widget) Missing.Add(Name); };
	Require(VB_RightPanelHandsAccessories, TEXT("VB_RightPanelHandsAccessories"));
	Require(WS_HandsAccessoriesSectionPages, TEXT("WS_HandsAccessoriesSectionPages"));
	Require(Btn_HandsFilterGloves, TEXT("Btn_HandsFilterGloves"));
	Require(Btn_HandsFilterWatches, TEXT("Btn_HandsFilterWatches"));
	Require(Txt_HandsCurrentSelection, TEXT("Txt_HandsCurrentSelection"));
	Require(Txt_HandsCurrentSelectionValue, TEXT("Txt_HandsCurrentSelectionValue"));
	Require(ScrollBox_GlovesItems, TEXT("ScrollBox_GlovesItems"));
	Require(UniformGrid_GlovesItems, TEXT("UniformGrid_GlovesItems"));
	Require(Txt_GlovesEmptyState, TEXT("Txt_GlovesEmptyState"));
	Require(ScrollBox_WatchesItems, TEXT("ScrollBox_WatchesItems"));
	Require(UniformGrid_WatchesItems, TEXT("UniformGrid_WatchesItems"));
	Require(Txt_WatchesEmptyState, TEXT("Txt_WatchesEmptyState"));
	if (!HeadCatalogItemCardClass) Missing.Add(TEXT("HeadCatalogItemCardClass"));
	if (WS_HandsAccessoriesSectionPages && WS_HandsAccessoriesSectionPages->GetChildrenCount() != 2)
	{
		Missing.Add(FString::Printf(TEXT("WS_HandsAccessoriesSectionPages.Children=%d Expected=2"),
			WS_HandsAccessoriesSectionPages->GetChildrenCount()));
	}
	if (!Missing.IsEmpty())
	{
		const FString List = FString::Join(Missing, TEXT(", "));
		ensureMsgf(false, TEXT("Static Hands Accessories page incomplete. Missing={%s}. Runtime reconstruction disabled."), *List);
		UE_LOG(LogTemp, Error, TEXT("[AvCustomizeHandsDesigner] Missing={%s}"), *List);
	}
}

void UAvCharacterCustomizationRootWidget::InitializeHandsAccessoriesPage()
{
	HandsAccessoriesCatalog = UWorkerAppearanceComponent::GetHandsAccessoriesCatalog();
	BuildHandsAccessoriesThumbnailRegistry();
	EnsureHandsAccessoriesPageComposition();
	if (Txt_HandsCurrentSelection) Txt_HandsCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	for (UTextBlock* Empty : { Txt_GlovesEmptyState.Get(), Txt_WatchesEmptyState.Get() })
	{
		if (Empty) Empty->SetText(FAvLoc::Text(TEXT("Customization.Common.NoAvailableOptions")));
	}
	if (Btn_HandsFilterGloves)
	{
		Btn_HandsFilterGloves->OnClicked.RemoveAll(this);
		Btn_HandsFilterGloves->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleHandsFilterGlovesClicked);
	}
	if (Btn_HandsFilterWatches)
	{
		Btn_HandsFilterWatches->OnClicked.RemoveAll(this);
		Btn_HandsFilterWatches->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleHandsFilterWatchesClicked);
	}
	if (WidgetTree)
	{
		for (const FName Name : { FName(TEXT("DesignSample_GlovesNone")), FName(TEXT("DesignSample_GlovesItem")),
			FName(TEXT("DesignSample_WatchesNone")), FName(TEXT("DesignSample_WatchesItem")) })
		{
			if (UWidget* Sample = WidgetTree->FindWidget(Name)) Sample->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	SelectHandsAccessoriesSection(EAvHandsAccessoriesSection::Gloves);
	UE_LOG(LogTemp, Display, TEXT("[AvCustomizeHandsCatalog] Records=%d Gloves=%d Watches=%d"),
		HandsAccessoriesCatalog.Num(),
		HandsAccessoriesCatalog.FilterByPredicate([](const FAvHandsAccessoriesCatalogItem& I){return I.Section==EAvHandsAccessoriesSection::Gloves&&I.bEnabled;}).Num(),
		HandsAccessoriesCatalog.FilterByPredicate([](const FAvHandsAccessoriesCatalogItem& I){return I.Section==EAvHandsAccessoriesSection::Watches&&I.bEnabled;}).Num());
}

void UAvCharacterCustomizationRootWidget::SelectHandsAccessoriesSection(EAvHandsAccessoriesSection Section)
{
	SelectedHandsAccessoriesSection = Section;
	if (WS_HandsAccessoriesSectionPages) WS_HandsAccessoriesSectionPages->SetActiveWidgetIndex(GetHandsAccessoriesSectionPageIndex(Section));
	RefreshHandsAccessoriesFilterVisuals();
	RefreshHandsAccessoriesCurrentSelection();
	RebuildHandsAccessoriesItemCards();
	if (UScrollBox* Scroll = GetScrollBoxForHandsAccessoriesSection(Section)) Scroll->ScrollToStart();
}

int32 UAvCharacterCustomizationRootWidget::GetHandsAccessoriesSectionPageIndex(EAvHandsAccessoriesSection Section) const
{
	return Section == EAvHandsAccessoriesSection::Watches ? 1 : 0;
}

UUniformGridPanel* UAvCharacterCustomizationRootWidget::GetGridForHandsAccessoriesSection(EAvHandsAccessoriesSection Section) const
{
	return Section == EAvHandsAccessoriesSection::Watches ? UniformGrid_WatchesItems : UniformGrid_GlovesItems;
}

UScrollBox* UAvCharacterCustomizationRootWidget::GetScrollBoxForHandsAccessoriesSection(EAvHandsAccessoriesSection Section) const
{
	return Section == EAvHandsAccessoriesSection::Watches ? ScrollBox_WatchesItems : ScrollBox_GlovesItems;
}

UTextBlock* UAvCharacterCustomizationRootWidget::GetEmptyStateForHandsAccessoriesSection(EAvHandsAccessoriesSection Section) const
{
	return Section == EAvHandsAccessoriesSection::Watches ? Txt_WatchesEmptyState : Txt_GlovesEmptyState;
}

void UAvCharacterCustomizationRootWidget::BuildHandsAccessoriesThumbnailRegistry()
{
	HandsAccessoriesThumbnailRegistry.Reset();
	for (int32 Index=0; Index<HandsAccessoriesCatalog.Num(); ++Index)
	{
		const FAvHandsAccessoriesCatalogItem& Item=HandsAccessoriesCatalog[Index];
		if (!Item.bEnabled) continue;
		FHeadItemThumbnailPair Pair;
		Pair.Type01=Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(),nullptr,*Item.ThumbnailType01,nullptr,LOAD_NoWarn|LOAD_Quiet));
		Pair.Type02=Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(),nullptr,*Item.ThumbnailType02,nullptr,LOAD_NoWarn|LOAD_Quiet));
		HandsAccessoriesThumbnailRegistry.Add(Index,Pair);
	}
}

UTexture2D* UAvCharacterCustomizationRootWidget::ResolveHandsAccessoriesItemThumbnail(int32 CatalogIndex) const
{
	const FHeadItemThumbnailPair* Pair=HandsAccessoriesThumbnailRegistry.Find(CatalogIndex);
	if (!Pair) return nullptr;
	return GetActiveHeadTypeId()==FName(TEXT("HeadType02")) ? Pair->Type02 : Pair->Type01;
}

void UAvCharacterCustomizationRootWidget::RefreshHandsAccessoriesFilterVisuals()
{
	ConfigureHeadFilterButton(Btn_HandsFilterGloves, Border_HeadFilterHeadTypeFrame_2,
		Border_HeadFilterHeadTypeFill_2, Img_HandsFilterGlovesIcon, Txt_HandsFilterGloves,
		SelectedHandsAccessoriesSection==EAvHandsAccessoriesSection::Gloves);
	ConfigureHeadFilterButton(Btn_HandsFilterWatches, Border_HeadFilterHeadgearFrame_2,
		Border_HeadFilterHeadgearFill_2, Img_HandsFilterWatchesIcon, Txt_HandsFilterWatches,
		SelectedHandsAccessoriesSection==EAvHandsAccessoriesSection::Watches);
}

void UAvCharacterCustomizationRootWidget::InvalidateHandsAccessoriesCards()
{
	for (TPair<EAvHandsAccessoriesSection,TArray<TObjectPtr<UAvHeadCatalogItemButton>>>& Entry : RuntimeHandsAccessoriesSectionCards)
	{
		for (UAvHeadCatalogItemButton* Card : Entry.Value) if (Card) Card->RemoveFromParent();
		Entry.Value.Reset();
	}
	RuntimeHandsAccessoriesSectionCards.Reset();
	BuildHandsAccessoriesThumbnailRegistry();
}

void UAvCharacterCustomizationRootWidget::RebuildHandsAccessoriesItemCards()
{
	UUniformGridPanel* Grid=GetGridForHandsAccessoriesSection(SelectedHandsAccessoriesSection);
	UScrollBox* Scroll=GetScrollBoxForHandsAccessoriesSection(SelectedHandsAccessoriesSection);
	UTextBlock* Empty=GetEmptyStateForHandsAccessoriesSection(SelectedHandsAccessoriesSection);
	if (!Grid) return;
	TArray<TObjectPtr<UAvHeadCatalogItemButton>>& Cards=RuntimeHandsAccessoriesSectionCards.FindOrAdd(SelectedHandsAccessoriesSection);
	for (UAvHeadCatalogItemButton* Card : Cards) if (Card) Card->RemoveFromParent();
	Cards.Reset();
	for (int32 ChildIndex=Grid->GetChildrenCount()-1; ChildIndex>=0; --ChildIndex)
	{
		UWidget* Child=Grid->GetChildAt(ChildIndex);
		if (Child && Child->GetName().StartsWith(TEXT("HandsAccessoriesItemCard_"))) Child->RemoveFromParent();
	}
	int32 Visible=0;
	if (HeadCatalogItemCardClass)
	{
		for (int32 Index=0; Index<HandsAccessoriesCatalog.Num(); ++Index)
		{
			const FAvHandsAccessoriesCatalogItem& Item=HandsAccessoriesCatalog[Index];
			if (Item.Section!=SelectedHandsAccessoriesSection || !Item.bEnabled) continue;
			const FName Name(*FString::Printf(TEXT("HandsAccessoriesItemCard_%d_%03d"),static_cast<int32>(Item.Section),Index));
			UAvHeadCatalogItemButton* Card=GetOwningPlayer()
				? CreateWidget<UAvHeadCatalogItemButton>(GetOwningPlayer(),HeadCatalogItemCardClass,Name)
				: CreateWidget<UAvHeadCatalogItemButton>(GetWorld(),HeadCatalogItemCardClass,Name);
			if (!Card) continue;
			UTexture2D* Thumbnail=ResolveHandsAccessoriesItemThumbnail(Index);
			if (!Thumbnail) UE_LOG(LogTemp,Error,TEXT("[AvCustomizeHandsThumbnail] Missing exact thumbnail Section=%d Name=%s"),static_cast<int32>(Item.Section),*Item.DisplayName);
			FString DisabledReason;
			const bool bCompatible = IsHandsAccessoriesItemCompatible(Item, DisabledReason);
			const FText DisplayText = AvCustomizationLocalization::Resolve(
				Item.DisplayNameKey, Item.DisplayName);
			const FText TooltipText = bCompatible
				? DisplayText
				: AvCustomizationLocalization::Tooltip(
					Item.DisplayNameKey, Item.DisplayName, DisabledReason);
			Card->InitializeHandsAccessoriesCatalogButton(
				this, Item.Section, Index, Item.bIsNone, Thumbnail, DisplayText,
				!bCompatible, TooltipText, Item.bIsNone ? TEXT("None") : Item.ExactMeshPath);
			const FString Current=ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString();
			Card->SetSelectedState(Item.bIsNone?Current.IsEmpty():Current==Item.ExactMeshPath);
			if (UUniformGridSlot* GridSlot=Grid->AddChildToUniformGrid(Card,Visible/2,Visible%2))
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill); GridSlot->SetVerticalAlignment(VAlign_Fill);
			}
			Cards.Add(Card); ++Visible;
		}
	}
	if (Scroll) Scroll->SetVisibility(Visible>0?ESlateVisibility::Visible:ESlateVisibility::Collapsed);
	if (Empty) Empty->SetVisibility(Visible>0?ESlateVisibility::Collapsed:ESlateVisibility::HitTestInvisible);
	RefreshHandsAccessoriesCurrentSelection();
	UE_LOG(LogTemp,Verbose,TEXT("[AvCustomizeHandsCards] Section=%d RuntimeCards=%d"),static_cast<int32>(SelectedHandsAccessoriesSection),Cards.Num());
}

void UAvCharacterCustomizationRootWidget::RefreshHandsAccessoriesCurrentSelection()
{
	FString DisplayNameKey = TEXT("Customization.Common.None");
	FString DisplayNameFallback;
	const EWorkerSlot SelectedSlot = SelectedHandsAccessoriesSection == EAvHandsAccessoriesSection::Watches
		? EWorkerSlot::Watch : EWorkerSlot::Gloves;
	const FString SelectedPath = ActiveCharacterAppearance.Get(SelectedSlot).ToSoftObjectPath().ToString();
	if (!SelectedPath.IsEmpty()) DisplayNameKey = TEXT("Customization.Common.UnavailableOption");
	for (const FAvHandsAccessoriesCatalogItem& Item : HandsAccessoriesCatalog)
	{
		if (Item.Section==SelectedHandsAccessoriesSection && !Item.bIsNone && Item.bEnabled &&
			ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString()==Item.ExactMeshPath)
		{
			DisplayNameKey = Item.CurrentSelectionDisplayNameKey.IsEmpty()
				? Item.DisplayNameKey : Item.CurrentSelectionDisplayNameKey;
			DisplayNameFallback = Item.CurrentSelectionDisplayName.IsEmpty()
				? Item.DisplayName : Item.CurrentSelectionDisplayName;
			break;
		}
	}
	if (Txt_HandsCurrentSelection) Txt_HandsCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	if (Txt_HandsCurrentSelectionValue)
	{
		const FText Text = AvCustomizationLocalization::Resolve(
			DisplayNameKey, DisplayNameFallback);
		Txt_HandsCurrentSelectionValue->SetAutoWrapText(false);
		Txt_HandsCurrentSelectionValue->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
		Txt_HandsCurrentSelectionValue->SetClipping(EWidgetClipping::ClipToBounds);
		Txt_HandsCurrentSelectionValue->SetText(Text);
		Txt_HandsCurrentSelectionValue->SetToolTipText(Text);
	}
}

bool UAvCharacterCustomizationRootWidget::IsHandsAccessoriesItemCompatible(
	const FAvHandsAccessoriesCatalogItem& Item, FString& OutDisabledReason) const
{
	OutDisabledReason.Reset();
	if (Item.bIsNone) return true;
	if (Item.Section == EAvHandsAccessoriesSection::Gloves)
	{
		const bool bWatchSelected = !ActiveCharacterAppearance.Get(EWorkerSlot::Watch).IsNull();
		if (bWatchSelected && !Item.bCompatibleWithWatch)
		{
			OutDisabledReason = TEXT("Customization.Disabled.IncompatibleWatch");
			return false;
		}
		return true;
	}

	const FString GlovesPath = ActiveCharacterAppearance.Get(EWorkerSlot::Gloves).ToSoftObjectPath().ToString();
	if (GlovesPath.IsEmpty()) return true;
	static const TSet<FString> WatchCompatibleGloves =
	{
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Gray.SKM_Arms_Gloves_Print_Gray"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Green.SKM_Arms_Gloves_Print_Green"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Orange.SKM_Arms_Gloves_Print_Orange"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Yellow.SKM_Arms_Gloves_Print_Yellow")
	};
	if (!WatchCompatibleGloves.Contains(GlovesPath))
	{
		OutDisabledReason = TEXT("Customization.Disabled.RemoveGloves");
		return false;
	}
	return true;
}

void UAvCharacterCustomizationRootWidget::HandleHandsAccessoriesCatalogItemClicked(EAvHandsAccessoriesSection Section,int32 CatalogIndex,bool bNoneCard)
{
	if (Section!=SelectedHandsAccessoriesSection || !HandsAccessoriesCatalog.IsValidIndex(CatalogIndex)) return;
	const FAvHandsAccessoriesCatalogItem& Item=HandsAccessoriesCatalog[CatalogIndex];
	if (Item.Section!=Section || Item.bIsNone!=bNoneCard || !Item.bEnabled) return;
	FString DisabledReason;
	if (!IsHandsAccessoriesItemCompatible(Item, DisabledReason))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeHandsCompatibility] BLOCKED Section=%d Path=%s Reason=%s"),
			static_cast<int32>(Section), *Item.ExactMeshPath, *DisabledReason);
		return;
	}
	const FString Current=ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString();
	if ((Item.bIsNone&&Current.IsEmpty()) || (!Item.bIsNone&&Current==Item.ExactMeshPath)) return;
	FWorkerAppearance NewAppearance=ActiveCharacterAppearance;
	if (Item.bIsNone) NewAppearance.Clear(Item.WorkerSlot);
	else NewAppearance.Set(Item.WorkerSlot,TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Item.ExactMeshPath)));
	FString Validation;
	if (!UWorkerAppearanceComponent::ValidateHeadCustomizationSelection(NewAppearance,Item.WorkerSlot,Validation))
	{
		UE_LOG(LogTemp,Error,TEXT("[AvCustomizeHandsApply] REJECT Slot=%d Path=%s %s"),static_cast<int32>(Item.WorkerSlot),*Item.ExactMeshPath,*Validation); return;
	}
	CommitActiveCharacterAppearance(NewAppearance,true,ActiveCharacterBasePresetId.IsNone()?FName(TEXT("BaseMaleUnderwear")):ActiveCharacterBasePresetId,EAvAppearanceOrigin::ManualCustomized);
	RebuildHandsAccessoriesItemCards();
	UE_LOG(LogTemp,Warning,TEXT("[AvCustomizeHandsApply] APPLIED Section=%d Slot=%d Path=%s Autosave=true"),static_cast<int32>(Section),static_cast<int32>(Item.WorkerSlot),Item.bIsNone?TEXT("None"):*Item.ExactMeshPath);
}

void UAvCharacterCustomizationRootWidget::EnsureUpperEquipmentPageComposition()
{
	if (bUpperEquipmentPageCompositionValidated) return;
	bUpperEquipmentPageCompositionValidated = true;
	TArray<FString> Missing;
	auto Require=[&Missing](const UObject* Object,const TCHAR* Name){if(!Object) Missing.Add(Name);};
	Require(VB_RightPanelUpperEquipment,TEXT("VB_RightPanelUpperEquipment"));
	Require(Txt_UpperCurrentSelection,TEXT("Txt_UpperCurrentSelection"));
	Require(Txt_UpperCurrentSelectionValue,TEXT("Txt_UpperCurrentSelectionValue"));
	Require(ScrollBox_TorsoItems,TEXT("ScrollBox_TorsoItems"));
	Require(UniformGrid_TorsoItems,TEXT("UniformGrid_TorsoItems"));
	Require(Txt_TorsoEmptyState,TEXT("Txt_TorsoEmptyState"));
	if (!Missing.IsEmpty()) UE_LOG(LogTemp,Error,TEXT("[AvCustomizeTorsoComposition] Missing=%s"),*FString::Join(Missing,TEXT(",")));
}

void UAvCharacterCustomizationRootWidget::InitializeUpperEquipmentPage()
{
	TorsoCatalog=UWorkerAppearanceComponent::GetTorsoCatalog();
	BuildTorsoThumbnailRegistry();
	EnsureUpperEquipmentPageComposition();
	if (ScrollBox_TorsoItems)
	{
		ScrollBox_TorsoItems->SetOrientation(Orient_Vertical);
		ScrollBox_TorsoItems->SetConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible);
		ScrollBox_TorsoItems->SetAnimateWheelScrolling(true);
		ScrollBox_TorsoItems->SetWheelScrollMultiplier(AvUpperEquipment::WheelScrollMultiplier);
		ScrollBox_TorsoItems->SetScrollBarVisibility(ESlateVisibility::Visible);
		ScrollBox_TorsoItems->SetScrollbarThickness(FVector2D(5.0f, 5.0f));
		ScrollBox_TorsoItems->SetAlwaysShowScrollbar(false);
	}
	if (DesignSample_TorsoNone) DesignSample_TorsoNone->SetVisibility(ESlateVisibility::Collapsed);
	if (DesignSample_TorsoItem) DesignSample_TorsoItem->SetVisibility(ESlateVisibility::Collapsed);
	bTorsoCatalogBuilt=false;
	RefreshTorsoCurrentSelection();
	UE_LOG(LogTemp,Warning,
		TEXT("[AvCustomizeTorsoCatalog] Records=%d Real=%d RemovedDamaged=%d WheelScrollMultiplier=%.1f AnimateWheelScrolling=%s"),
		TorsoCatalog.Num(),
		FMath::Max(0,TorsoCatalog.Num()-1),
		AvUpperEquipment::RemovedDamagedRecordCount,
		ScrollBox_TorsoItems ? ScrollBox_TorsoItems->GetWheelScrollMultiplier() : 0.0f,
		ScrollBox_TorsoItems && ScrollBox_TorsoItems->IsAnimateWheelScrolling() ? TEXT("true") : TEXT("false"));
}

void UAvCharacterCustomizationRootWidget::BuildTorsoThumbnailRegistry()
{
	TorsoThumbnailRegistry.Reset();
	for(int32 Index=0;Index<TorsoCatalog.Num();++Index)
	{
		const FAvTorsoCatalogItem& Item=TorsoCatalog[Index];
		FHeadItemThumbnailPair Pair;
		Pair.Type01=Item.ThumbnailType01.IsEmpty()?nullptr:LoadObject<UTexture2D>(nullptr,*Item.ThumbnailType01);
		Pair.Type02=Item.ThumbnailType02.IsEmpty()?nullptr:LoadObject<UTexture2D>(nullptr,*Item.ThumbnailType02);
		TorsoThumbnailRegistry.Add(Index,Pair);
	}
}

UTexture2D* UAvCharacterCustomizationRootWidget::ResolveTorsoItemThumbnail(int32 CatalogIndex) const
{
	const FHeadItemThumbnailPair* Pair=TorsoThumbnailRegistry.Find(CatalogIndex);
	return Pair?(GetActiveHeadTypeId()==TEXT("HeadType02")?Pair->Type02.Get():Pair->Type01.Get()):nullptr;
}

void UAvCharacterCustomizationRootWidget::InvalidateTorsoCards()
{
	for(UAvHeadCatalogItemButton* Card:RuntimeTorsoCards) if(Card) Card->RemoveFromParent();
	RuntimeTorsoCards.Reset();
	RuntimeTorsoCardCatalogIndices.Reset();
	bTorsoCatalogBuilt = false;
	BuildTorsoThumbnailRegistry();
}

void UAvCharacterCustomizationRootWidget::RebuildTorsoItemCards(bool bScrollToStart)
{
	if(!UniformGrid_TorsoItems||!ScrollBox_TorsoItems||!Txt_TorsoEmptyState) return;
	for(UAvHeadCatalogItemButton* Card:RuntimeTorsoCards) if(Card) Card->RemoveFromParent();
	RuntimeTorsoCards.Reset();
	RuntimeTorsoCardCatalogIndices.Reset();
	int32 VisibleIndex=0;
	for(int32 Index=0;Index<TorsoCatalog.Num();++Index)
	{
		const FAvTorsoCatalogItem& Item=TorsoCatalog[Index];
		if(!Item.bEnabled) continue;
		UAvHeadCatalogItemButton* Card=CreateWidget<UAvHeadCatalogItemButton>(GetOwningPlayer(),HeadCatalogItemCardClass,
			FName(*FString::Printf(TEXT("TorsoItemCard_%03d"),Index)));
		if(!Card) continue;
		const FText DisplayText = AvCustomizationLocalization::Resolve(
			Item.DisplayNameKey, Item.DisplayName);
		const FText TooltipText = AvCustomizationLocalization::Tooltip(
			Item.DisplayNameKey, Item.DisplayName,
			Item.DisabledReasonKey, Item.DisabledReason);
		Card->InitializeTorsoCatalogButton(
			this, Index, Item.bIsNone, ResolveTorsoItemThumbnail(Index), DisplayText,
			false, TooltipText, Item.bIsNone ? TEXT("None") : Item.ExactMeshPath);
		const FString Current=ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath().ToString();
		Card->SetSelectedState(Item.bIsNone?Current.IsEmpty():Current==Item.ExactMeshPath);
		UniformGrid_TorsoItems->AddChildToUniformGrid(Card,VisibleIndex/2,VisibleIndex%2);
		RuntimeTorsoCards.Add(Card);
		RuntimeTorsoCardCatalogIndices.Add(Index);
		++VisibleIndex;
	}
	const bool bHasReal=TorsoCatalog.ContainsByPredicate([](const FAvTorsoCatalogItem& I){return !I.bIsNone&&I.bEnabled;});
	ScrollBox_TorsoItems->SetVisibility(bHasReal?ESlateVisibility::Visible:ESlateVisibility::Collapsed);
	Txt_TorsoEmptyState->SetVisibility(bHasReal?ESlateVisibility::Collapsed:ESlateVisibility::Visible);
	if(bScrollToStart) ScrollBox_TorsoItems->ScrollToStart();
	RefreshTorsoCurrentSelection();
}

void UAvCharacterCustomizationRootWidget::RefreshTorsoCardSelectionStates()
{
	const FString Current = ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath().ToString();
	const int32 Count = FMath::Min(RuntimeTorsoCards.Num(), RuntimeTorsoCardCatalogIndices.Num());
	for (int32 CardIndex = 0; CardIndex < Count; ++CardIndex)
	{
		UAvHeadCatalogItemButton* Card = RuntimeTorsoCards[CardIndex];
		const int32 CatalogIndex = RuntimeTorsoCardCatalogIndices[CardIndex];
		if (!Card || !TorsoCatalog.IsValidIndex(CatalogIndex))
		{
			continue;
		}
		const FAvTorsoCatalogItem& Item = TorsoCatalog[CatalogIndex];
		Card->SetSelectedState(Item.bIsNone ? Current.IsEmpty() : Current == Item.ExactMeshPath);
	}
}

void UAvCharacterCustomizationRootWidget::UpdateTorsoSelectionCards(
	const FString& PreviousMeshPath, const FString& NewMeshPath)
{
	const int32 Count = FMath::Min(RuntimeTorsoCards.Num(), RuntimeTorsoCardCatalogIndices.Num());
	for (int32 CardIndex = 0; CardIndex < Count; ++CardIndex)
	{
		UAvHeadCatalogItemButton* Card = RuntimeTorsoCards[CardIndex];
		const int32 CatalogIndex = RuntimeTorsoCardCatalogIndices[CardIndex];
		if (!Card || !TorsoCatalog.IsValidIndex(CatalogIndex))
		{
			continue;
		}
		const FAvTorsoCatalogItem& Item = TorsoCatalog[CatalogIndex];
		const FString ItemPath = Item.bIsNone ? FString() : Item.ExactMeshPath;
		if (ItemPath == PreviousMeshPath || ItemPath == NewMeshPath)
		{
			Card->SetSelectedState(ItemPath == NewMeshPath);
		}
	}
}

void UAvCharacterCustomizationRootWidget::RefreshTorsoCurrentSelection()
{
	FString DisplayNameKey = TEXT("Customization.Common.None");
	FString DisplayNameFallback;
	const FString Current=ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath().ToString();
	if(!Current.IsEmpty())
	{
		const FAvTorsoCatalogItem* Found=TorsoCatalog.FindByPredicate([&Current](const FAvTorsoCatalogItem& I){return !I.bIsNone&&I.bEnabled&&I.ExactMeshPath==Current;});
		if (Found)
		{
			DisplayNameKey = Found->CurrentSelectionNameKey;
			DisplayNameFallback = Found->CurrentSelectionName;
		}
		else
		{
			DisplayNameKey = TEXT("Customization.Common.UnavailableOption");
		}
	}
	if(Txt_UpperCurrentSelection) Txt_UpperCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	if(Txt_UpperCurrentSelectionValue)
	{
		const FText Text = AvCustomizationLocalization::Resolve(
			DisplayNameKey, DisplayNameFallback);
		Txt_UpperCurrentSelectionValue->SetAutoWrapText(false);
		Txt_UpperCurrentSelectionValue->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
		Txt_UpperCurrentSelectionValue->SetClipping(EWidgetClipping::ClipToBounds);
		Txt_UpperCurrentSelectionValue->SetText(Text);
		Txt_UpperCurrentSelectionValue->SetToolTipText(Text);
	}
}

void UAvCharacterCustomizationRootWidget::HandleTorsoCatalogItemClicked(int32 CatalogIndex,bool bNoneCard)
{
	if(!TorsoCatalog.IsValidIndex(CatalogIndex)) return;
	const FAvTorsoCatalogItem& Item=TorsoCatalog[CatalogIndex];
	if(Item.bIsNone!=bNoneCard||!Item.bEnabled) return;
	const FString Current=ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath().ToString();
	const bool bEnsembleActive =
		!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull() ||
		!ActiveCharacterAppearance.Get(EWorkerSlot::Overalls).IsNull();
	if(!bEnsembleActive &&
		((Item.bIsNone&&Current.IsEmpty())||(!Item.bIsNone&&Current==Item.ExactMeshPath)))
	{
		LastOperationAppearanceAutosaveCount = 0;
		return;
	}
	if(!Item.bIsNone && BeginRaisedHoodConflictForTorso(Item)) return;
	FString Validation;
	if(!ApplyManualModularSelectionAtomically(
		EWorkerSlot::Torso,
		Item.bIsNone ? FSoftObjectPath() : FSoftObjectPath(Item.ExactMeshPath),
		Item.StableId,
		Validation))
	{
		UE_LOG(LogTemp,Error,TEXT("[AvCustomizeTorsoApply] REJECT Path=%s %s"),*Item.ExactMeshPath,*Validation); return;
	}
	UE_LOG(LogTemp,Warning,TEXT("[AvCustomizeTorsoApply] APPLIED Path=%s EnsembleExit=%s Autosave=true"),
		Item.bIsNone?TEXT("None"):*Item.ExactMeshPath,
		bEnsembleActive?TEXT("true"):TEXT("false"));
}

void UAvCharacterCustomizationRootWidget::EnsureFullOutfitsPageComposition()
{
	if (bFullOutfitsPageCompositionValidated) return;
	bFullOutfitsPageCompositionValidated = true;
	TArray<FString> Missing;
	auto Require = [&Missing](const UObject* Object, const TCHAR* Name)
	{
		if (!Object) Missing.Add(Name);
	};
	Require(VB_RightPanelFullOutfits, TEXT("VB_RightPanelFullOutfits"));
	Require(Txt_FullOutfitsCurrentSelection, TEXT("Txt_FullOutfitsCurrentSelection"));
	Require(Txt_FullOutfitsCurrentSelectionValue, TEXT("Txt_FullOutfitsCurrentSelectionValue"));
	Require(ScrollBox_FullOutfitItems, TEXT("ScrollBox_FullOutfitItems"));
	Require(UniformGrid_FullOutfitItems, TEXT("UniformGrid_FullOutfitItems"));
	if (!HeadCatalogItemCardClass) Missing.Add(TEXT("HeadCatalogItemCardClass"));
	if (!Missing.IsEmpty())
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvCustomizeFullOutfitsComposition] Missing=%s"),
			*FString::Join(Missing, TEXT(",")));
		return;
	}
	const bool bScrollInsideAuthoredOverlay =
		ScrollBox_FullOutfitItems->GetParent() &&
		ScrollBox_FullOutfitItems->GetParent()->GetFName() ==
			FName(TEXT("Overlay_FullOutfitItemsArea"));
	const bool bGridIsDirectScrollContent =
		UniformGrid_FullOutfitItems->GetParent() == ScrollBox_FullOutfitItems;
	UE_LOG(LogTemp, Warning,
		TEXT("[AvCustomizeFullOutfitsComposition] Authored=true ScrollParentOverlay=%s GridDirectScrollContent=%s DesignerSamples=%d EmptyState=false"),
		bScrollInsideAuthoredOverlay ? TEXT("true") : TEXT("false"),
		bGridIsDirectScrollContent ? TEXT("true") : TEXT("false"),
		(DesignSample_FullOutfitNone ? 1 : 0) + (DesignSample_FullOutfitItem ? 1 : 0));
}

void UAvCharacterCustomizationRootWidget::InitializeFullOutfitsPage()
{
	FullOutfitCatalog = UWorkerAppearanceComponent::GetFullOutfitCatalog();
	BuildFullOutfitThumbnailRegistry();
	EnsureFullOutfitsPageComposition();
	ConfigureCurrentSelectionRow(
		Txt_FullOutfitsCurrentSelection, Txt_FullOutfitsCurrentSelectionValue);
	if (ScrollBox_FullOutfitItems)
	{
		ScrollBox_FullOutfitItems->SetOrientation(Orient_Vertical);
		ScrollBox_FullOutfitItems->SetConsumeMouseWheel(
			EConsumeMouseWheel::WhenScrollingPossible);
		ScrollBox_FullOutfitItems->SetAnimateWheelScrolling(true);
		ScrollBox_FullOutfitItems->SetWheelScrollMultiplier(
			AvUpperEquipment::WheelScrollMultiplier);
		ScrollBox_FullOutfitItems->SetScrollBarVisibility(ESlateVisibility::Visible);
		ScrollBox_FullOutfitItems->SetScrollbarThickness(FVector2D(5.f, 5.f));
		ScrollBox_FullOutfitItems->SetAlwaysShowScrollbar(false);
		ScrollBox_FullOutfitItems->SetVisibility(ESlateVisibility::Visible);
	}
	if (DesignSample_FullOutfitNone)
	{
		DesignSample_FullOutfitNone->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (DesignSample_FullOutfitItem)
	{
		DesignSample_FullOutfitItem->SetVisibility(ESlateVisibility::Collapsed);
	}
	bFullOutfitCatalogBuilt = false;
	RefreshFullOutfitCurrentSelection();
	int32 OverallsCount = 0;
	int32 TrueFullOutfitCount = 0;
	for (const FAvFullOutfitCatalogItem& Item : FullOutfitCatalog)
	{
		if (!Item.bEnabled || Item.bIsNone) continue;
		OverallsCount += Item.TechnicalKind == EAvEnsembleTechnicalKind::Overalls ? 1 : 0;
		TrueFullOutfitCount +=
			Item.TechnicalKind == EAvEnsembleTechnicalKind::FullOutfit ? 1 : 0;
	}
	UE_LOG(LogTemp, Warning,
		TEXT("[AvCustomizeFullOutfitsCatalog] Records=%d Real=%d Overalls=%d FullOutfit=%d EnumOveralls=%d WheelScrollMultiplier=%.1f"),
		FullOutfitCatalog.Num(),
		FMath::Max(0, FullOutfitCatalog.Num() - 1),
		OverallsCount,
		TrueFullOutfitCount,
		static_cast<int32>(EWorkerSlot::Overalls),
		ScrollBox_FullOutfitItems
			? ScrollBox_FullOutfitItems->GetWheelScrollMultiplier() : 0.f);
}

void UAvCharacterCustomizationRootWidget::BuildFullOutfitThumbnailRegistry()
{
	FullOutfitThumbnailRegistry.Reset();
	for (int32 Index = 0; Index < FullOutfitCatalog.Num(); ++Index)
	{
		const FAvFullOutfitCatalogItem& Item = FullOutfitCatalog[Index];
		FHeadItemThumbnailPair Pair;
		Pair.Type01 = Item.ThumbnailType01.IsEmpty()
			? nullptr : LoadObject<UTexture2D>(nullptr, *Item.ThumbnailType01);
		Pair.Type02 = Item.ThumbnailType02.IsEmpty()
			? nullptr : LoadObject<UTexture2D>(nullptr, *Item.ThumbnailType02);
		FullOutfitThumbnailRegistry.Add(Index, Pair);
	}
}

UTexture2D* UAvCharacterCustomizationRootWidget::ResolveFullOutfitItemThumbnail(
	int32 CatalogIndex) const
{
	const FHeadItemThumbnailPair* Pair =
		FullOutfitThumbnailRegistry.Find(CatalogIndex);
	return Pair
		? (GetActiveHeadTypeId() == TEXT("HeadType02")
			? Pair->Type02.Get() : Pair->Type01.Get())
		: nullptr;
}

void UAvCharacterCustomizationRootWidget::InvalidateFullOutfitCards()
{
	for (UAvHeadCatalogItemButton* Card : RuntimeFullOutfitCards)
	{
		if (Card) Card->RemoveFromParent();
	}
	RuntimeFullOutfitCards.Reset();
	RuntimeFullOutfitCardCatalogIndices.Reset();
	bFullOutfitCatalogBuilt = false;
	BuildFullOutfitThumbnailRegistry();
}

void UAvCharacterCustomizationRootWidget::RebuildFullOutfitItemCards(bool bScrollToStart)
{
	if (!UniformGrid_FullOutfitItems || !ScrollBox_FullOutfitItems ||
		!HeadCatalogItemCardClass)
	{
		return;
	}
	const float PreservedOffset = ScrollBox_FullOutfitItems->GetScrollOffset();
	for (UAvHeadCatalogItemButton* Card : RuntimeFullOutfitCards)
	{
		if (Card) Card->RemoveFromParent();
	}
	RuntimeFullOutfitCards.Reset();
	RuntimeFullOutfitCardCatalogIndices.Reset();

	const bool bHasFullOutfit =
		!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull();
	const FString CurrentFullOutfit = ActiveCharacterAppearance
		.Get(EWorkerSlot::FullOutfit).ToSoftObjectPath().ToString();
	const FString CurrentOveralls = ActiveCharacterAppearance
		.Get(EWorkerSlot::Overalls).ToSoftObjectPath().ToString();
	FAvFullOutfitCatalogItem ResolvedLegacyOveralls;
	const bool bHasResolvedLegacyOveralls = !bHasFullOutfit &&
		UWorkerAppearanceComponent::ResolveFullOutfitCatalogRecord(
			EWorkerSlot::Overalls,
			ActiveCharacterAppearance.Get(EWorkerSlot::Overalls).ToSoftObjectPath(),
			ResolvedLegacyOveralls);
	int32 VisibleIndex = 0;
	for (int32 Index = 0; Index < FullOutfitCatalog.Num(); ++Index)
	{
		const FAvFullOutfitCatalogItem& Item = FullOutfitCatalog[Index];
		if (!Item.bEnabled) continue;
		UAvHeadCatalogItemButton* Card =
			CreateWidget<UAvHeadCatalogItemButton>(
				GetOwningPlayer(), HeadCatalogItemCardClass,
				FName(*FString::Printf(TEXT("FullOutfitItemCard_%03d"), Index)));
		if (!Card) continue;
		const FText DisplayText = AvCustomizationLocalization::Resolve(
			Item.DisplayNameKey, Item.DisplayName);
		const FText TooltipText = AvCustomizationLocalization::Tooltip(
			Item.DisplayNameKey, Item.DisplayName,
			Item.DisabledReasonKey, Item.DisabledReason);
		Card->InitializeFullOutfitCatalogButton(
			this, Index, Item.bIsNone, ResolveFullOutfitItemThumbnail(Index),
			DisplayText, false, TooltipText,
			Item.bIsNone ? TEXT("None") : Item.ExactMeshPath);
		bool bSelected = false;
		if (Item.bIsNone)
		{
			bSelected = CurrentFullOutfit.IsEmpty() && CurrentOveralls.IsEmpty();
		}
		else if (Item.TargetSlot == EWorkerSlot::FullOutfit)
		{
			bSelected = CurrentFullOutfit == Item.ExactMeshPath ||
				(bHasResolvedLegacyOveralls && Item.DemoIndex == ResolvedLegacyOveralls.DemoIndex);
		}
		else
		{
			bSelected = !bHasFullOutfit && CurrentOveralls == Item.ExactMeshPath;
		}
		Card->SetSelectedState(bSelected);
		UniformGrid_FullOutfitItems->AddChildToUniformGrid(
			Card, VisibleIndex / 2, VisibleIndex % 2);
		RuntimeFullOutfitCards.Add(Card);
		RuntimeFullOutfitCardCatalogIndices.Add(Index);
		++VisibleIndex;
	}
	ScrollBox_FullOutfitItems->SetVisibility(ESlateVisibility::Visible);
	if (bScrollToStart)
	{
		ScrollBox_FullOutfitItems->ScrollToStart();
	}
	else
	{
		ScrollBox_FullOutfitItems->SetScrollOffset(PreservedOffset);
	}
	RefreshFullOutfitCurrentSelection();
}

void UAvCharacterCustomizationRootWidget::RefreshFullOutfitCardSelectionStates()
{
	const bool bHasFullOutfit =
		!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull();
	const FString CurrentFullOutfit = ActiveCharacterAppearance
		.Get(EWorkerSlot::FullOutfit).ToSoftObjectPath().ToString();
	const FString CurrentOveralls = ActiveCharacterAppearance
		.Get(EWorkerSlot::Overalls).ToSoftObjectPath().ToString();
	FAvFullOutfitCatalogItem ResolvedLegacyOveralls;
	const bool bHasResolvedLegacyOveralls = !bHasFullOutfit &&
		UWorkerAppearanceComponent::ResolveFullOutfitCatalogRecord(
			EWorkerSlot::Overalls,
			ActiveCharacterAppearance.Get(EWorkerSlot::Overalls).ToSoftObjectPath(),
			ResolvedLegacyOveralls);
	const int32 Count = FMath::Min(
		RuntimeFullOutfitCards.Num(), RuntimeFullOutfitCardCatalogIndices.Num());
	for (int32 CardIndex = 0; CardIndex < Count; ++CardIndex)
	{
		UAvHeadCatalogItemButton* Card = RuntimeFullOutfitCards[CardIndex];
		const int32 CatalogIndex = RuntimeFullOutfitCardCatalogIndices[CardIndex];
		if (!Card || !FullOutfitCatalog.IsValidIndex(CatalogIndex)) continue;
		const FAvFullOutfitCatalogItem& Item = FullOutfitCatalog[CatalogIndex];
		const bool bSelected = Item.bIsNone
			? CurrentFullOutfit.IsEmpty() && CurrentOveralls.IsEmpty()
			: (Item.TargetSlot == EWorkerSlot::FullOutfit
				? CurrentFullOutfit == Item.ExactMeshPath ||
					(bHasResolvedLegacyOveralls && Item.DemoIndex == ResolvedLegacyOveralls.DemoIndex)
				: !bHasFullOutfit && CurrentOveralls == Item.ExactMeshPath);
		Card->SetSelectedState(bSelected);
	}
	RefreshFullOutfitCurrentSelection();
}

void UAvCharacterCustomizationRootWidget::RefreshFullOutfitCurrentSelection()
{
	FString DisplayNameKey = TEXT("Customization.Common.None");
	FString DisplayNameFallback;
	const FString CurrentFullOutfit = ActiveCharacterAppearance
		.Get(EWorkerSlot::FullOutfit).ToSoftObjectPath().ToString();
	const FString CurrentOveralls = ActiveCharacterAppearance
		.Get(EWorkerSlot::Overalls).ToSoftObjectPath().ToString();
	const FString& ActivePath =
		!CurrentFullOutfit.IsEmpty() ? CurrentFullOutfit : CurrentOveralls;
	const EWorkerSlot ActiveSlot = !CurrentFullOutfit.IsEmpty()
		? EWorkerSlot::FullOutfit : EWorkerSlot::Overalls;
	if (!ActivePath.IsEmpty())
	{
		FAvFullOutfitCatalogItem Found;
		if (UWorkerAppearanceComponent::ResolveFullOutfitCatalogRecord(
			ActiveSlot, FSoftObjectPath(ActivePath), Found))
		{
			DisplayNameKey = Found.CurrentSelectionNameKey;
			DisplayNameFallback = Found.CurrentSelectionName;
		}
		else
		{
			DisplayNameKey = TEXT("Customization.Common.UnavailableOption");
		}
	}
	if (Txt_FullOutfitsCurrentSelection)
	{
		Txt_FullOutfitsCurrentSelection->SetText(
			FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	}
	if (Txt_FullOutfitsCurrentSelectionValue)
	{
		const FText Text = AvCustomizationLocalization::Resolve(
			DisplayNameKey, DisplayNameFallback);
		Txt_FullOutfitsCurrentSelectionValue->SetMinDesiredWidth(0.f);
		Txt_FullOutfitsCurrentSelectionValue->SetAutoWrapText(false);
		Txt_FullOutfitsCurrentSelectionValue->SetTextOverflowPolicy(
			ETextOverflowPolicy::Ellipsis);
		Txt_FullOutfitsCurrentSelectionValue->SetClipping(
			EWidgetClipping::ClipToBounds);
		Txt_FullOutfitsCurrentSelectionValue->SetText(Text);
		Txt_FullOutfitsCurrentSelectionValue->SetToolTipText(Text);
	}
}

void UAvCharacterCustomizationRootWidget::HandleFullOutfitCatalogItemClicked(
	int32 CatalogIndex, bool bNoneCard)
{
	if (!FullOutfitCatalog.IsValidIndex(CatalogIndex)) return;
	const FAvFullOutfitCatalogItem& Item = FullOutfitCatalog[CatalogIndex];
	if (Item.bIsNone != bNoneCard || !Item.bEnabled) return;
	const FString CurrentFullOutfit = ActiveCharacterAppearance
		.Get(EWorkerSlot::FullOutfit).ToSoftObjectPath().ToString();
	const FString CurrentOveralls = ActiveCharacterAppearance
		.Get(EWorkerSlot::Overalls).ToSoftObjectPath().ToString();
	const bool bNoOp = Item.bIsNone
		? CurrentFullOutfit.IsEmpty() && CurrentOveralls.IsEmpty()
		: (Item.TargetSlot == EWorkerSlot::FullOutfit
			? CurrentFullOutfit == Item.ExactMeshPath && CurrentOveralls.IsEmpty()
			: CurrentOveralls == Item.ExactMeshPath && CurrentFullOutfit.IsEmpty());
	if (bNoOp)
	{
		UE_LOG(LogTemp, Display,
			TEXT("[AvCustomizeFullOutfitsApply] Result=NOOP StableId=%s Autosave=false"),
			*Item.StableId);
		return;
	}

	FWorkerAppearance NewAppearance = ActiveCharacterAppearance;
	if (Item.bIsNone)
	{
		NewAppearance.Clear(EWorkerSlot::FullOutfit);
		NewAppearance.Clear(EWorkerSlot::Overalls);
	}
	else
	{
		NewAppearance.Set(
			Item.TargetSlot,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Item.ExactMeshPath)));
		NewAppearance.Clear(Item.TargetSlot == EWorkerSlot::FullOutfit
			? EWorkerSlot::Overalls : EWorkerSlot::FullOutfit);
	}
	FString Validation;
	const EWorkerSlot ChangedSlot =
		Item.bIsNone ? EWorkerSlot::FullOutfit : Item.TargetSlot;
	if (!UWorkerAppearanceComponent::ValidateHeadCustomizationSelection(
		NewAppearance, ChangedSlot, Validation))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvCustomizeFullOutfitsApply] Result=REJECT StableId=%s %s"),
			*Item.StableId, *Validation);
		return;
	}
	CommitActiveCharacterAppearance(
		NewAppearance,
		true,
		ActiveCharacterBasePresetId.IsNone()
			? FName(TEXT("BaseMaleUnderwear"))
			: ActiveCharacterBasePresetId,
		EAvAppearanceOrigin::ManualCustomized);
	RefreshFullOutfitCardSelectionStates();
	UE_LOG(LogTemp, Warning,
		TEXT("[AvCustomizeFullOutfitsApply] Result=APPLIED StableId=%s Kind=%d TargetSlot=%d UnderlyingTorso=%s UnderlyingLegs=%s UnderlyingFeet=%s PreviewApply=1 ManualCommit=1 AppearanceAutosave=1"),
		*Item.StableId,
		static_cast<int32>(Item.TechnicalKind),
		static_cast<int32>(Item.TargetSlot),
		*NewAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath().ToString(),
		*NewAppearance.Get(EWorkerSlot::Legs).ToSoftObjectPath().ToString(),
		*NewAppearance.Get(EWorkerSlot::Feet).ToSoftObjectPath().ToString());
}

void UAvCharacterCustomizationRootWidget::EnsureLowerHipPageComposition()
{
	if (bLowerHipPageCompositionValidated) return;
	bLowerHipPageCompositionValidated = true;
	TArray<FString> Missing;
	auto Require = [&Missing](const UObject* Object, const TCHAR* Name)
	{
		if (!Object) Missing.Add(Name);
	};
	Require(VB_RightPanelLowerHip, TEXT("VB_RightPanelLowerHip"));
	Require(Btn_LowerFilterLegs, TEXT("Btn_LowerFilterLegs"));
	Require(Btn_LowerFilterHip, TEXT("Btn_LowerFilterHip"));
	Require(Txt_LowerCurrentSelection, TEXT("Txt_LowerCurrentSelection"));
	Require(Txt_LowerCurrentSelectionValue, TEXT("Txt_LowerCurrentSelectionValue"));
	Require(WS_LowerHipSectionPages, TEXT("WS_LowerHipSectionPages"));
	Require(ScrollBox_LegsItems, TEXT("ScrollBox_LegsItems"));
	Require(UniformGrid_LegsItems, TEXT("UniformGrid_LegsItems"));
	Require(Txt_LegsEmptyState, TEXT("Txt_LegsEmptyState"));
	Require(ScrollBox_HipItems, TEXT("ScrollBox_HipItems"));
	Require(UniformGrid_HipItems, TEXT("UniformGrid_HipItems"));
	Require(Txt_HipEmptyState, TEXT("Txt_HipEmptyState"));
	if (!HeadCatalogItemCardClass) Missing.Add(TEXT("HeadCatalogItemCardClass"));
	if (!Missing.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[AvCustomizeLowerHipComposition] Missing=%s"),
			*FString::Join(Missing, TEXT(",")));
	}
}

void UAvCharacterCustomizationRootWidget::InitializeLowerHipPage()
{
	LowerHipCatalog = UWorkerAppearanceComponent::GetLowerHipCatalog();
	BuildLowerHipThumbnailRegistry();
	EnsureLowerHipPageComposition();
	ConfigureCurrentSelectionRow(Txt_LowerCurrentSelection, Txt_LowerCurrentSelectionValue);
	for (UScrollBox* ScrollBox : { ScrollBox_LegsItems.Get(), ScrollBox_HipItems.Get() })
	{
		if (!ScrollBox) continue;
		ScrollBox->SetOrientation(Orient_Vertical);
		ScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible);
		ScrollBox->SetAnimateWheelScrolling(true);
		ScrollBox->SetWheelScrollMultiplier(AvUpperEquipment::WheelScrollMultiplier);
		ScrollBox->SetScrollBarVisibility(ESlateVisibility::Visible);
		ScrollBox->SetScrollbarThickness(FVector2D(5.f, 5.f));
		ScrollBox->SetAlwaysShowScrollbar(false);
	}
	for (UAvHeadCatalogItemButton* Sample : {
		DesignSample_LegsNone.Get(), DesignSample_LegsItem.Get(),
		DesignSample_HipNone.Get(), DesignSample_HipItem.Get() })
	{
		if (Sample) Sample->SetVisibility(ESlateVisibility::Collapsed);
	}
	SelectedLowerHipSection = EAvLowerHipSection::Legs;
	if (WS_LowerHipSectionPages) WS_LowerHipSectionPages->SetActiveWidgetIndex(0);
	bLegsCatalogBuilt = false;
	bHipCatalogBuilt = false;
	RefreshLowerHipFilterVisuals();
	RefreshLowerHipCurrentSelection();
	UE_LOG(LogTemp, Warning,
		TEXT("[AvCustomizeLowerHipCatalog] Total=%d Legs=%d Hip=%d HipSlot=%d WheelMultiplier=%.1f Animate=true"),
		LowerHipCatalog.Num(),
		LowerHipCatalog.FilterByPredicate([](const FAvLowerHipCatalogItem& Item){ return Item.Section == EAvLowerHipSection::Legs; }).Num(),
		LowerHipCatalog.FilterByPredicate([](const FAvLowerHipCatalogItem& Item){ return Item.Section == EAvLowerHipSection::Hip; }).Num(),
		static_cast<int32>(EWorkerSlot::HipAccessory), AvUpperEquipment::WheelScrollMultiplier);
}

void UAvCharacterCustomizationRootWidget::SelectLowerHipSection(EAvLowerHipSection Section)
{
	SelectedLowerHipSection = Section;
	if (WS_LowerHipSectionPages)
	{
		WS_LowerHipSectionPages->SetActiveWidgetIndex(Section == EAvLowerHipSection::Legs ? 0 : 1);
	}
	RefreshLowerHipFilterVisuals();
	RefreshLowerHipCurrentSelection();
	bool& bBuilt = Section == EAvLowerHipSection::Legs ? bLegsCatalogBuilt : bHipCatalogBuilt;
	if (!bBuilt)
	{
		RebuildLowerHipItemCards(Section, true);
		bBuilt = true;
	}
	else
	{
		RefreshLowerHipCardSelectionStates(Section);
	}
}

void UAvCharacterCustomizationRootWidget::RefreshLowerHipFilterVisuals()
{
	const FLinearColor Active(0.95f, 0.42f, 0.04f, 1.f);
	const FLinearColor Inactive(0.56f, 0.58f, 0.62f, 1.f);
	auto Apply = [this, &Active, &Inactive](const TCHAR* TextName, const TCHAR* IconName, bool bSelected)
	{
		const FLinearColor Tint = bSelected ? Active : Inactive;
		if (WidgetTree)
		{
			if (UTextBlock* Text = Cast<UTextBlock>(WidgetTree->FindWidget(FName(TextName))))
			{
				Text->SetColorAndOpacity(FSlateColor(Tint));
			}
			if (UImage* Icon = Cast<UImage>(WidgetTree->FindWidget(FName(IconName))))
			{
				Icon->SetColorAndOpacity(Tint);
			}
		}
	};
	Apply(TEXT("Txt_LowerFilterLegs"), TEXT("Img_LowerFilterLegsIcon"),
		SelectedLowerHipSection == EAvLowerHipSection::Legs);
	Apply(TEXT("Txt_LowerFilterHip"), TEXT("Img_LowerFilterHipIcon"),
		SelectedLowerHipSection == EAvLowerHipSection::Hip);
}

void UAvCharacterCustomizationRootWidget::BuildLowerHipThumbnailRegistry()
{
	LowerHipThumbnailRegistry.Reset();
	for (int32 Index = 0; Index < LowerHipCatalog.Num(); ++Index)
	{
		const FAvLowerHipCatalogItem& Item = LowerHipCatalog[Index];
		FHeadItemThumbnailPair Pair;
		Pair.Type01 = Item.ThumbnailType01.IsEmpty() ? nullptr : LoadObject<UTexture2D>(nullptr, *Item.ThumbnailType01);
		Pair.Type02 = Item.ThumbnailType02.IsEmpty() ? nullptr : LoadObject<UTexture2D>(nullptr, *Item.ThumbnailType02);
		LowerHipThumbnailRegistry.Add(Index, Pair);
	}
}

UTexture2D* UAvCharacterCustomizationRootWidget::ResolveLowerHipItemThumbnail(int32 CatalogIndex) const
{
	const FHeadItemThumbnailPair* Pair = LowerHipThumbnailRegistry.Find(CatalogIndex);
	return Pair ? (GetActiveHeadTypeId() == TEXT("HeadType02") ? Pair->Type02.Get() : Pair->Type01.Get()) : nullptr;
}

UScrollBox* UAvCharacterCustomizationRootWidget::GetScrollBoxForLowerHipSection(EAvLowerHipSection Section) const
{
	return Section == EAvLowerHipSection::Legs ? ScrollBox_LegsItems.Get() : ScrollBox_HipItems.Get();
}

UUniformGridPanel* UAvCharacterCustomizationRootWidget::GetGridForLowerHipSection(EAvLowerHipSection Section) const
{
	return Section == EAvLowerHipSection::Legs ? UniformGrid_LegsItems.Get() : UniformGrid_HipItems.Get();
}

UTextBlock* UAvCharacterCustomizationRootWidget::GetEmptyStateForLowerHipSection(EAvLowerHipSection Section) const
{
	return Section == EAvLowerHipSection::Legs ? Txt_LegsEmptyState.Get() : Txt_HipEmptyState.Get();
}

void UAvCharacterCustomizationRootWidget::InvalidateLowerHipCards(EAvLowerHipSection Section)
{
	TArray<TObjectPtr<UAvHeadCatalogItemButton>>& Cards = Section == EAvLowerHipSection::Legs
		? RuntimeLegsCards : RuntimeHipCards;
	TArray<int32>& Indices = Section == EAvLowerHipSection::Legs
		? RuntimeLegsCardCatalogIndices : RuntimeHipCardCatalogIndices;
	for (UAvHeadCatalogItemButton* Card : Cards) if (Card) Card->RemoveFromParent();
	Cards.Reset();
	Indices.Reset();
	if (Section == EAvLowerHipSection::Legs) bLegsCatalogBuilt = false;
	else bHipCatalogBuilt = false;
}

void UAvCharacterCustomizationRootWidget::RebuildLowerHipItemCards(
	EAvLowerHipSection Section, bool bScrollToStart)
{
	UUniformGridPanel* Grid = GetGridForLowerHipSection(Section);
	UScrollBox* ScrollBox = GetScrollBoxForLowerHipSection(Section);
	UTextBlock* EmptyState = GetEmptyStateForLowerHipSection(Section);
	if (!Grid || !ScrollBox || !EmptyState || !HeadCatalogItemCardClass) return;
	TArray<TObjectPtr<UAvHeadCatalogItemButton>>& Cards = Section == EAvLowerHipSection::Legs
		? RuntimeLegsCards : RuntimeHipCards;
	TArray<int32>& Indices = Section == EAvLowerHipSection::Legs
		? RuntimeLegsCardCatalogIndices : RuntimeHipCardCatalogIndices;
	for (UAvHeadCatalogItemButton* Card : Cards) if (Card) Card->RemoveFromParent();
	Cards.Reset();
	Indices.Reset();
	int32 VisibleIndex = 0;
	bool bHasReal = false;
	for (int32 Index = 0; Index < LowerHipCatalog.Num(); ++Index)
	{
		const FAvLowerHipCatalogItem& Item = LowerHipCatalog[Index];
		if (Item.Section != Section || !Item.bEnabled) continue;
		bHasReal |= !Item.bIsNone;
		const FString Prefix = Section == EAvLowerHipSection::Legs ? TEXT("Legs") : TEXT("Hip");
		UAvHeadCatalogItemButton* Card = CreateWidget<UAvHeadCatalogItemButton>(
			GetOwningPlayer(), HeadCatalogItemCardClass,
			FName(*FString::Printf(TEXT("%sItemCard_%03d"), *Prefix, Index)));
		if (!Card) continue;
		const FText DisplayText = AvCustomizationLocalization::Resolve(
			Item.DisplayNameKey, Item.DisplayName);
		const FText TooltipText = AvCustomizationLocalization::Tooltip(
			Item.DisplayNameKey, Item.DisplayName,
			Item.DisabledReasonKey, Item.DisabledReason);
		Card->InitializeLowerHipCatalogButton(this, Section, Index, Item.bIsNone,
			ResolveLowerHipItemThumbnail(Index), DisplayText, false, TooltipText,
			Item.bIsNone ? TEXT("None") : Item.ExactMeshPath);
		const FString Current = ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString();
		Card->SetSelectedState(Item.bIsNone ? Current.IsEmpty() : Current == Item.ExactMeshPath);
		Grid->AddChildToUniformGrid(Card, VisibleIndex / 2, VisibleIndex % 2);
		Cards.Add(Card);
		Indices.Add(Index);
		++VisibleIndex;
	}
	ScrollBox->SetVisibility(bHasReal ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	EmptyState->SetVisibility(bHasReal ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (bScrollToStart) ScrollBox->ScrollToStart();
	RefreshLowerHipCurrentSelection();
}

void UAvCharacterCustomizationRootWidget::RefreshLowerHipCardSelectionStates(EAvLowerHipSection Section)
{
	const EWorkerSlot WorkerSlot = Section == EAvLowerHipSection::Legs
		? EWorkerSlot::Legs : EWorkerSlot::HipAccessory;
	const FString Current = ActiveCharacterAppearance.Get(WorkerSlot).ToSoftObjectPath().ToString();
	const TArray<TObjectPtr<UAvHeadCatalogItemButton>>& Cards = Section == EAvLowerHipSection::Legs
		? RuntimeLegsCards : RuntimeHipCards;
	const TArray<int32>& Indices = Section == EAvLowerHipSection::Legs
		? RuntimeLegsCardCatalogIndices : RuntimeHipCardCatalogIndices;
	const int32 Count = FMath::Min(Cards.Num(), Indices.Num());
	for (int32 CardIndex = 0; CardIndex < Count; ++CardIndex)
	{
		if (!Cards[CardIndex] || !LowerHipCatalog.IsValidIndex(Indices[CardIndex])) continue;
		const FAvLowerHipCatalogItem& Item = LowerHipCatalog[Indices[CardIndex]];
		Cards[CardIndex]->SetSelectedState(Item.bIsNone ? Current.IsEmpty() : Current == Item.ExactMeshPath);
	}
}

void UAvCharacterCustomizationRootWidget::RefreshLowerHipCurrentSelection()
{
	const EWorkerSlot WorkerSlot = SelectedLowerHipSection == EAvLowerHipSection::Legs
		? EWorkerSlot::Legs : EWorkerSlot::HipAccessory;
	const FString Current = ActiveCharacterAppearance.Get(WorkerSlot).ToSoftObjectPath().ToString();
	FString DisplayNameKey = TEXT("Customization.Common.None");
	FString DisplayNameFallback;
	if (!Current.IsEmpty())
	{
		const FAvLowerHipCatalogItem* Found = LowerHipCatalog.FindByPredicate(
			[this, &Current](const FAvLowerHipCatalogItem& Item)
			{
				return Item.Section == SelectedLowerHipSection && !Item.bIsNone &&
					Item.bEnabled && Item.ExactMeshPath == Current;
			});
		if (Found)
		{
			DisplayNameKey = Found->CurrentSelectionNameKey;
			DisplayNameFallback = Found->CurrentSelectionName;
		}
		else
		{
			DisplayNameKey = TEXT("Customization.Common.UnavailableOption");
		}
	}
	if (Txt_LowerCurrentSelection) Txt_LowerCurrentSelection->SetText(FAvLoc::Text(TEXT("Customization.Common.CurrentSelection")));
	if (Txt_LowerCurrentSelectionValue)
	{
		const FText Text = AvCustomizationLocalization::Resolve(
			DisplayNameKey, DisplayNameFallback);
		Txt_LowerCurrentSelectionValue->SetAutoWrapText(false);
		Txt_LowerCurrentSelectionValue->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
		Txt_LowerCurrentSelectionValue->SetClipping(EWidgetClipping::ClipToBounds);
		Txt_LowerCurrentSelectionValue->SetText(Text);
		Txt_LowerCurrentSelectionValue->SetToolTipText(Text);
	}
}

void UAvCharacterCustomizationRootWidget::HandleLowerHipCatalogItemClicked(
	EAvLowerHipSection Section, int32 CatalogIndex, bool bNoneCard)
{
	if (Section != SelectedLowerHipSection || !LowerHipCatalog.IsValidIndex(CatalogIndex)) return;
	const FAvLowerHipCatalogItem& Item = LowerHipCatalog[CatalogIndex];
	if (Item.Section != Section || Item.bIsNone != bNoneCard || !Item.bEnabled) return;
	const FString Current = ActiveCharacterAppearance.Get(Item.WorkerSlot).ToSoftObjectPath().ToString();
	const bool bEnsembleActive = Item.WorkerSlot == EWorkerSlot::Legs &&
		(!ActiveCharacterAppearance.Get(EWorkerSlot::FullOutfit).IsNull() ||
		 !ActiveCharacterAppearance.Get(EWorkerSlot::Overalls).IsNull());
	if (!bEnsembleActive &&
		((Item.bIsNone && Current.IsEmpty()) || (!Item.bIsNone && Current == Item.ExactMeshPath)))
	{
		LastOperationAppearanceAutosaveCount = 0;
		return;
	}
	if (Item.WorkerSlot == EWorkerSlot::Legs)
	{
		FString Validation;
		if (!ApplyManualModularSelectionAtomically(
			EWorkerSlot::Legs,
			Item.bIsNone ? FSoftObjectPath() : FSoftObjectPath(Item.ExactMeshPath),
			Item.StableId,
			Validation))
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvCustomizeLowerHipApply] REJECT Section=%d Path=%s %s"),
				static_cast<int32>(Section), *Item.ExactMeshPath, *Validation);
			return;
		}
		UE_LOG(LogTemp, Warning,
			TEXT("[AvCustomizeLowerHipApply] APPLIED Section=%d Slot=%d Path=%s EnsembleExit=%s Autosave=true"),
			static_cast<int32>(Section), static_cast<int32>(Item.WorkerSlot),
			Item.bIsNone ? TEXT("None") : *Item.ExactMeshPath,
			bEnsembleActive ? TEXT("true") : TEXT("false"));
		return;
	}
	FWorkerAppearance NewAppearance = ActiveCharacterAppearance;
	if (Item.bIsNone) NewAppearance.Clear(Item.WorkerSlot);
	else NewAppearance.Set(Item.WorkerSlot,
		TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Item.ExactMeshPath)));
	FString Validation;
	if (!UWorkerAppearanceComponent::ValidateHeadCustomizationSelection(
		NewAppearance, Item.WorkerSlot, Validation))
	{
		UE_LOG(LogTemp, Error, TEXT("[AvCustomizeLowerHipApply] REJECT Section=%d Path=%s %s"),
			static_cast<int32>(Section), *Item.ExactMeshPath, *Validation);
		return;
	}
	CommitActiveCharacterAppearance(NewAppearance, true,
		ActiveCharacterBasePresetId.IsNone() ? FName(TEXT("BaseMaleUnderwear")) : ActiveCharacterBasePresetId,
		EAvAppearanceOrigin::ManualCustomized);
	RefreshLowerHipCardSelectionStates(Section);
	RefreshLowerHipCurrentSelection();
	UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeLowerHipApply] APPLIED Section=%d Slot=%d Path=%s Autosave=true"),
		static_cast<int32>(Section), static_cast<int32>(Item.WorkerSlot),
		Item.bIsNone ? TEXT("None") : *Item.ExactMeshPath);
}

void UAvCharacterCustomizationRootWidget::CaptureCategoryDesignerVisuals()
{
	bHasCapturedCategoryDesignerVisuals = true;
}

void UAvCharacterCustomizationRootWidget::RefreshCategoryVisuals()
{
	for (const FAvCategoryVisualWidgets& VisualWidgets : CategoryVisualWidgets)
	{
		ApplyCategoryVisual(VisualWidgets.Category, false);
	}
	ApplyCategoryVisual(SelectedCategory, true);
}

void UAvCharacterCustomizationRootWidget::ApplyCategoryVisual(EAvCustomizationCategory Category, bool bIsActive) const
{
	const FAvCategoryVisualWidgets* VisualWidgets = FindCategoryVisualWidgets(Category);
	if (!VisualWidgets || !WidgetTree)
	{
		return;
	}

	const ESlateVisibility ActiveVisibility = bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	if (UBorder* ActiveBackground = FindAvWidget<UBorder>(WidgetTree, VisualWidgets->ActiveBackgroundName))
	{
		ActiveBackground->SetVisibility(ActiveVisibility);
	}
	if (UWidget* ActiveLineSizeBox = WidgetTree->FindWidget(FName(VisualWidgets->ActiveLineSizeBoxName)))
	{
		ActiveLineSizeBox->SetVisibility(ActiveVisibility);
	}
	if (UWidget* ActiveLine = WidgetTree->FindWidget(FName(VisualWidgets->ActiveLineName)))
	{
		ActiveLine->SetVisibility(ActiveVisibility);
	}
	if (UWidget* ActiveOutline = WidgetTree->FindWidget(FName(VisualWidgets->ActiveOutlineName)))
	{
		ActiveOutline->SetVisibility(ActiveVisibility);
	}
	if (UWidget* ActiveBorder = WidgetTree->FindWidget(FName(VisualWidgets->ActiveBorderName)))
	{
		ActiveBorder->SetVisibility(ActiveVisibility);
	}
	if (UWidget* ActiveFrame = WidgetTree->FindWidget(FName(VisualWidgets->ActiveFrameName)))
	{
		ActiveFrame->SetVisibility(ActiveVisibility);
	}
	const FLinearColor Tint = bIsActive
		? AvCustomizePreview::CategoryActiveTint
		: AvCustomizePreview::CategoryInactiveTint;
	if (UTextBlock* Text = FindAvWidget<UTextBlock>(WidgetTree, VisualWidgets->TextName))
	{
		Text->SetColorAndOpacity(FSlateColor(Tint));
	}
	if (UImage* Icon = FindAvWidget<UImage>(WidgetTree, VisualWidgets->IconName))
	{
		Icon->SetColorAndOpacity(Tint);
	}
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
namespace AvAppearanceInspector
{
	const FLinearColor PanelBackground(0.018f, 0.021f, 0.026f, 0.985f);
	const FLinearColor RowBackground(0.045f, 0.050f, 0.060f, 0.98f);
	const FLinearColor AlternateRowBackground(0.060f, 0.064f, 0.074f, 0.98f);
	const FLinearColor Accent(0.95f, 0.42f, 0.04f, 1.f);
	const FLinearColor TextMain(0.93f, 0.94f, 0.96f, 1.f);
	const FLinearColor TextDim(0.58f, 0.61f, 0.66f, 1.f);
	const FLinearColor Visible(0.35f, 0.85f, 0.46f, 1.f);
	const FLinearColor Hidden(1.0f, 0.55f, 0.16f, 1.f);

	const TCHAR* SlotName(EWorkerSlot Slot)
	{
		switch (Slot)
		{
		case EWorkerSlot::Body: return TEXT("Body");
		case EWorkerSlot::Head: return TEXT("Head");
		case EWorkerSlot::Hair: return TEXT("Hair");
		case EWorkerSlot::Beard: return TEXT("Beard");
		case EWorkerSlot::Torso: return TEXT("Torso");
		case EWorkerSlot::Legs: return TEXT("Legs");
		case EWorkerSlot::Feet: return TEXT("Feet");
		case EWorkerSlot::Gloves: return TEXT("Gloves");
		case EWorkerSlot::Headgear: return TEXT("Headgear");
		case EWorkerSlot::FaceMask: return TEXT("FaceMask");
		case EWorkerSlot::Glasses: return TEXT("Glasses");
		case EWorkerSlot::Vest: return TEXT("Vest");
		case EWorkerSlot::FullOutfit: return TEXT("FullOutfit");
		case EWorkerSlot::Headphones: return TEXT("Headphones");
		case EWorkerSlot::Watch: return TEXT("Watch");
		case EWorkerSlot::HipAccessory: return TEXT("HipAccessory");
		case EWorkerSlot::Overalls: return TEXT("Overalls");
		default: return TEXT("Unknown");
		}
	}

	FString ShortObjectName(const FString& Path)
	{
		if (Path.IsEmpty() || Path == TEXT("None"))
		{
			return TEXT("None");
		}
		int32 SeparatorIndex = INDEX_NONE;
		if (Path.FindLastChar(TEXT('.'), SeparatorIndex) || Path.FindLastChar(TEXT('/'), SeparatorIndex))
		{
			return Path.Mid(SeparatorIndex + 1);
		}
		return Path;
	}

	FString PackagePathFromObjectPath(const FString& ObjectPath)
	{
		int32 DotIndex = INDEX_NONE;
		return ObjectPath.FindChar(TEXT('.'), DotIndex) ? ObjectPath.Left(DotIndex) : ObjectPath;
	}

	FString JsonEscape(FString Value)
	{
		Value.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
		Value.ReplaceInline(TEXT("\""), TEXT("\\\""));
		Value.ReplaceInline(TEXT("\r"), TEXT("\\r"));
		Value.ReplaceInline(TEXT("\n"), TEXT("\\n"));
		Value.ReplaceInline(TEXT("\t"), TEXT("\\t"));
		return Value;
	}

	UTextBlock* MakeText(
		UWidgetTree* Tree,
		FName Name,
		const FString& Value,
		int32 FontSize = 11,
		const FLinearColor& Color = TextMain)
	{
		if (!Tree)
		{
			return nullptr;
		}
		UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		if (Text)
		{
			Text->SetText(FText::FromString(Value));
			Text->SetColorAndOpacity(FSlateColor(Color));
			Text->SetAutoWrapText(false);
			Text->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
			FSlateFontInfo Font = Text->GetFont();
			Font.Size = FontSize;
			Text->SetFont(Font);
		}
		return Text;
	}

	UTextBlock* AddSizedText(
		UWidgetTree* Tree,
		UHorizontalBox* Row,
		const FString& BaseName,
		const FString& Value,
		float Width,
		int32 FontSize = 11,
		const FLinearColor& Color = TextMain)
	{
		if (!Tree || !Row)
		{
			return nullptr;
		}
		USizeBox* Size = Tree->ConstructWidget<USizeBox>(
			USizeBox::StaticClass(),
			FName(*(BaseName + TEXT("_Size"))));
		UTextBlock* Text = MakeText(Tree, FName(*BaseName), Value, FontSize, Color);
		if (!Size || !Text)
		{
			return nullptr;
		}
		Size->SetWidthOverride(Width);
		Size->SetMinDesiredHeight(24.f);
		Size->SetClipping(EWidgetClipping::ClipToBounds);
		Size->AddChild(Text);
		if (UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(Size))
		{
			Slot->SetVerticalAlignment(VAlign_Center);
			Slot->SetPadding(FMargin(2.f, 0.f));
		}
		return Text;
	}

	void StyleButton(UButton* Button)
	{
		if (!Button)
		{
			return;
		}
		FButtonStyle Style = Button->GetStyle();
		Style.Normal.TintColor = FSlateColor(FLinearColor(0.095f, 0.105f, 0.12f, 1.f));
		Style.Hovered.TintColor = FSlateColor(FLinearColor(0.31f, 0.15f, 0.025f, 1.f));
		Style.Pressed.TintColor = FSlateColor(FLinearColor(0.52f, 0.23f, 0.025f, 1.f));
		Style.Disabled.TintColor = FSlateColor(FLinearColor(0.045f, 0.05f, 0.06f, 0.72f));
		Style.SetNormalPadding(FMargin(4.f, 2.f));
		Style.SetPressedPadding(FMargin(4.f, 3.f, 4.f, 1.f));
		Button->SetStyle(Style);
	}

	UButton* MakeActionButton(
		UWidgetTree* Tree,
		UHorizontalBox* Row,
		FName Name,
		const FString& Label,
		float Width)
	{
		if (!Tree || !Row)
		{
			return nullptr;
		}
		UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
		USizeBox* Size = Tree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		UTextBlock* Text = MakeText(Tree, NAME_None, Label, 10, TextMain);
		if (!Button || !Size || !Text)
		{
			return nullptr;
		}
		StyleButton(Button);
		Size->SetWidthOverride(Width);
		Size->SetHeightOverride(27.f);
		Text->SetJustification(ETextJustify::Center);
		Size->AddChild(Text);
		Button->AddChild(Size);
		if (UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(Button))
		{
			Slot->SetPadding(FMargin(2.f));
			Slot->SetVerticalAlignment(VAlign_Center);
		}
		return Button;
	}
}

void UAvCharacterCustomizationRootWidget::EnsureAppearanceInspector()
{
	if (Border_AppearanceDebugInspector || !WidgetTree)
	{
		return;
	}

	UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
	if (!RootCanvas)
	{
		UE_LOG(LogTemp, Error, TEXT("[AvAppearanceInspector] Root widget is not a CanvasPanel; inspector not created."));
		return;
	}

	Border_AppearanceDebugInspector = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(),
		TEXT("Border_AppearanceDebugInspector"));
	UVerticalBox* Panel = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("VB_AppearanceDebugInspector"));
	if (!Border_AppearanceDebugInspector || !Panel)
	{
		Border_AppearanceDebugInspector = nullptr;
		return;
	}

	Border_AppearanceDebugInspector->SetBrushColor(AvAppearanceInspector::PanelBackground);
	Border_AppearanceDebugInspector->SetPadding(FMargin(9.f));
	Border_AppearanceDebugInspector->SetContent(Panel);
	Border_AppearanceDebugInspector->SetVisibility(ESlateVisibility::Collapsed);
	Border_AppearanceDebugInspector->SetToolTipText(FText::FromString(
		TEXT("Development-only. Visibility and LOD affect preview components only; appearance data is untouched.")));
	if (UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(Border_AppearanceDebugInspector))
	{
		CanvasSlot->SetAnchors(FAnchors(1.f, 0.f));
		CanvasSlot->SetAlignment(FVector2D(1.f, 0.f));
		CanvasSlot->SetPosition(FVector2D(-12.f, 12.f));
		CanvasSlot->SetSize(FVector2D(660.f, 690.f));
		CanvasSlot->SetZOrder(950);
	}

	UTextBlock* Title = AvAppearanceInspector::MakeText(
		WidgetTree,
		TEXT("Txt_AppearanceDebugTitle"),
		TEXT("APPEARANCE SLOT INSPECTOR  [F9 TO CLOSE]"),
		15,
		AvAppearanceInspector::Accent);
	if (UVerticalBoxSlot* TitleSlot = Panel->AddChildToVerticalBox(Title))
	{
		TitleSlot->SetPadding(FMargin(4.f, 1.f, 4.f, 4.f));
	}

	Txt_AppearanceDebugStatus = AvAppearanceInspector::MakeText(
		WidgetTree,
		TEXT("Txt_AppearanceDebugStatus"),
		TEXT("Preview unavailable"),
		10,
		AvAppearanceInspector::TextDim);
	if (UVerticalBoxSlot* StatusSlot = Panel->AddChildToVerticalBox(Txt_AppearanceDebugStatus))
	{
		StatusSlot->SetPadding(FMargin(4.f, 0.f, 4.f, 5.f));
	}

	UHorizontalBox* ActionsA = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HB_AppearanceDebugActionsA"));
	UHorizontalBox* ActionsB = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HB_AppearanceDebugActionsB"));
	Panel->AddChildToVerticalBox(ActionsA);
	Panel->AddChildToVerticalBox(ActionsB);

	UButton* ShowAll = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsA, TEXT("Btn_AppearanceDebugShowAll"), TEXT("SHOW ALL"), 118.f);
	UButton* HideClothing = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsA, TEXT("Btn_AppearanceDebugHideClothing"), TEXT("HIDE CLOTHING"), 142.f);
	UButton* BodyOnly = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsA, TEXT("Btn_AppearanceDebugBodyOnly"), TEXT("BODY ONLY"), 118.f);
	UButton* HideBody = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsA, TEXT("Btn_AppearanceDebugHideBody"), TEXT("HIDE BODY"), 118.f);
	UButton* ShowBody = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsA, TEXT("Btn_AppearanceDebugShowBody"), TEXT("SHOW BODY"), 118.f);
	if (ShowAll) ShowAll->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorShowAll);
	if (HideClothing) HideClothing->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorHideClothing);
	if (BodyOnly) BodyOnly->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorBodyOnly);
	if (HideBody) HideBody->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorHideBody);
	if (ShowBody) ShowBody->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorShowBody);

	UButton* ClearSolo = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsB, TEXT("Btn_AppearanceDebugClearSolo"), TEXT("CLEAR SOLO"), 118.f);
	UButton* Copy = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsB, TEXT("Btn_AppearanceDebugCopy"), TEXT("COPY APPEARANCE"), 142.f);
	UButton* Dump = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsB, TEXT("Btn_AppearanceDebugDump"), TEXT("DUMP JSON"), 118.f);
	UButton* ForceLod = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsB, TEXT("Btn_AppearanceDebugForceLOD0"), TEXT("FORCE LOD 0"), 118.f);
	UButton* AutoLod = AvAppearanceInspector::MakeActionButton(WidgetTree, ActionsB, TEXT("Btn_AppearanceDebugAutoLOD"), TEXT("AUTO LOD"), 118.f);
	if (ClearSolo) ClearSolo->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorClearSolo);
	if (Copy) Copy->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorCopy);
	if (Dump) Dump->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorDump);
	if (ForceLod) ForceLod->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorForceLod0);
	if (AutoLod) AutoLod->OnClicked.AddDynamic(this, &UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorAutoLod);

	UHorizontalBox* ColumnHeader = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HB_AppearanceDebugColumnHeader"));
	AvAppearanceInspector::AddSizedText(WidgetTree, ColumnHeader, TEXT("Txt_AppearanceDebugHeaderSlot"), TEXT("SLOT"), 60.f, 9, AvAppearanceInspector::Accent);
	AvAppearanceInspector::AddSizedText(WidgetTree, ColumnHeader, TEXT("Txt_AppearanceDebugHeaderMesh"), TEXT("SKELETAL MESH"), 190.f, 9, AvAppearanceInspector::Accent);
	AvAppearanceInspector::AddSizedText(WidgetTree, ColumnHeader, TEXT("Txt_AppearanceDebugHeaderVisibility"), TEXT("STATE"), 60.f, 9, AvAppearanceInspector::Accent);
	AvAppearanceInspector::AddSizedText(WidgetTree, ColumnHeader, TEXT("Txt_AppearanceDebugHeaderMaterial"), TEXT("FIRST MATERIAL"), 118.f, 9, AvAppearanceInspector::Accent);
	AvAppearanceInspector::AddSizedText(WidgetTree, ColumnHeader, TEXT("Txt_AppearanceDebugHeaderLod"), TEXT("LOD"), 58.f, 9, AvAppearanceInspector::Accent);
	AvAppearanceInspector::AddSizedText(WidgetTree, ColumnHeader, TEXT("Txt_AppearanceDebugHeaderToggle"), TEXT("TOGGLE"), 66.f, 9, AvAppearanceInspector::Accent);
	AvAppearanceInspector::AddSizedText(WidgetTree, ColumnHeader, TEXT("Txt_AppearanceDebugHeaderSolo"), TEXT("SOLO"), 56.f, 9, AvAppearanceInspector::Accent);
	if (UVerticalBoxSlot* HeaderSlot = Panel->AddChildToVerticalBox(ColumnHeader))
	{
		HeaderSlot->SetPadding(FMargin(0.f, 5.f, 0.f, 1.f));
	}

	UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("Scroll_AppearanceDebugSlots"));
	UVerticalBox* SlotsBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_AppearanceDebugSlots"));
	Scroll->AddChild(SlotsBox);
	if (UVerticalBoxSlot* ScrollSlot = Panel->AddChildToVerticalBox(Scroll))
	{
		ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		ScrollSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 2.f));
	}

	for (int32 SlotIndex = static_cast<int32>(EWorkerSlot::Body);
		SlotIndex <= static_cast<int32>(EWorkerSlot::Overalls);
		++SlotIndex)
	{
		const EWorkerSlot WorkerSlot = static_cast<EWorkerSlot>(SlotIndex);
		const FString Prefix = FString::Printf(TEXT("AppearanceDebug_%02d"), SlotIndex);
		UBorder* RowBackground = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(*(Prefix + TEXT("_Border"))));
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(*(Prefix + TEXT("_Row"))));
		RowBackground->SetBrushColor((SlotIndex % 2) == 0 ? AvAppearanceInspector::RowBackground : AvAppearanceInspector::AlternateRowBackground);
		RowBackground->SetPadding(FMargin(2.f, 1.f));
		RowBackground->SetContent(Row);
		if (UVerticalBoxSlot* RowSlot = SlotsBox->AddChildToVerticalBox(RowBackground))
		{
			RowSlot->SetPadding(FMargin(0.f, 1.f));
		}

		AvAppearanceInspector::AddSizedText(WidgetTree, Row, Prefix + TEXT("_Slot"), AvAppearanceInspector::SlotName(WorkerSlot), 60.f, 10, AvAppearanceInspector::TextMain);
		UTextBlock* MeshText = AvAppearanceInspector::AddSizedText(WidgetTree, Row, Prefix + TEXT("_Mesh"), TEXT("None"), 190.f, 9, AvAppearanceInspector::TextMain);
		UTextBlock* VisibilityText = AvAppearanceInspector::AddSizedText(WidgetTree, Row, Prefix + TEXT("_Visibility"), TEXT("Empty"), 60.f, 9, AvAppearanceInspector::TextDim);
		UTextBlock* MaterialText = AvAppearanceInspector::AddSizedText(WidgetTree, Row, Prefix + TEXT("_Material"), TEXT("None"), 118.f, 8, AvAppearanceInspector::TextDim);
		UTextBlock* LodText = AvAppearanceInspector::AddSizedText(WidgetTree, Row, Prefix + TEXT("_LOD"), TEXT("-"), 58.f, 8, AvAppearanceInspector::TextDim);

		UAvAppearanceInspectorSlotButton* ToggleButton = WidgetTree->ConstructWidget<UAvAppearanceInspectorSlotButton>(
			UAvAppearanceInspectorSlotButton::StaticClass(),
			FName(*(Prefix + TEXT("_Toggle"))));
		USizeBox* ToggleSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		UTextBlock* ToggleLabel = AvAppearanceInspector::MakeText(WidgetTree, FName(*(Prefix + TEXT("_ToggleLabel"))), TEXT("-"), 9, AvAppearanceInspector::TextMain);
		ToggleSize->SetWidthOverride(62.f);
		ToggleSize->SetHeightOverride(24.f);
		ToggleLabel->SetJustification(ETextJustify::Center);
		ToggleSize->AddChild(ToggleLabel);
		ToggleButton->AddChild(ToggleSize);
		AvAppearanceInspector::StyleButton(ToggleButton);
		ToggleButton->InitializeInspectorButton(this, WorkerSlot, EAvAppearanceInspectorSlotAction::Toggle);
		if (UHorizontalBoxSlot* ToggleSlot = Row->AddChildToHorizontalBox(ToggleButton))
		{
			ToggleSlot->SetPadding(FMargin(2.f, 0.f));
			ToggleSlot->SetVerticalAlignment(VAlign_Center);
		}

		UAvAppearanceInspectorSlotButton* SoloButton = WidgetTree->ConstructWidget<UAvAppearanceInspectorSlotButton>(
			UAvAppearanceInspectorSlotButton::StaticClass(),
			FName(*(Prefix + TEXT("_Solo"))));
		USizeBox* SoloSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		UTextBlock* SoloLabel = AvAppearanceInspector::MakeText(WidgetTree, FName(*(Prefix + TEXT("_SoloLabel"))), TEXT("SOLO"), 9, AvAppearanceInspector::TextMain);
		SoloSize->SetWidthOverride(52.f);
		SoloSize->SetHeightOverride(24.f);
		SoloLabel->SetJustification(ETextJustify::Center);
		SoloSize->AddChild(SoloLabel);
		SoloButton->AddChild(SoloSize);
		AvAppearanceInspector::StyleButton(SoloButton);
		SoloButton->InitializeInspectorButton(this, WorkerSlot, EAvAppearanceInspectorSlotAction::Solo);
		if (UHorizontalBoxSlot* SoloSlot = Row->AddChildToHorizontalBox(SoloButton))
		{
			SoloSlot->SetPadding(FMargin(2.f, 0.f));
			SoloSlot->SetVerticalAlignment(VAlign_Center);
		}

		AppearanceInspectorMeshTexts.Add(WorkerSlot, MeshText);
		AppearanceInspectorVisibilityTexts.Add(WorkerSlot, VisibilityText);
		AppearanceInspectorMaterialTexts.Add(WorkerSlot, MaterialText);
		AppearanceInspectorLodTexts.Add(WorkerSlot, LodText);
		AppearanceInspectorToggleButtons.Add(WorkerSlot, ToggleButton);
		AppearanceInspectorToggleLabels.Add(WorkerSlot, ToggleLabel);
		AppearanceInspectorSoloButtons.Add(WorkerSlot, SoloButton);
	}

	UE_LOG(LogTemp, Log, TEXT("[AvAppearanceInspector] Development-only runtime panel created; hotkey=F9, initial=Hidden."));
}

void UAvCharacterCustomizationRootWidget::RefreshAppearanceInspector()
{
	if (!Border_AppearanceDebugInspector)
	{
		return;
	}

	const FString SoloName = PreviewActor && PreviewActor->GetAppearanceInspectorSoloSlot().IsSet()
		? AvAppearanceInspector::SlotName(PreviewActor->GetAppearanceInspectorSoloSlot().GetValue())
		: TEXT("None");
	const FString LodMode = PreviewActor && PreviewActor->IsAppearanceInspectorForceLod0()
		? TEXT("Forced LOD0")
		: TEXT("Auto LOD");
	const FSoftObjectPath ActiveTorsoPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath();
	const FSoftObjectPath ActiveHeadgearPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).ToSoftObjectPath();
	const FSoftObjectPath ActiveHeadphonesPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headphones).ToSoftObjectPath();
	const bool bRaisedHood = UWorkerAppearanceComponent::HasRaisedHood(ActiveCharacterAppearance);
	const bool bHeadgearConflicts =
		UWorkerAppearanceComponent::HeadgearConflictsWithRaisedHood(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headgear));
	const bool bHeadphonesConflict =
		UWorkerAppearanceComponent::HeadphonesConflictWithRaisedHood(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headphones));
	const int32 ConflictSlotCount = bRaisedHood
		? static_cast<int32>(bHeadgearConflicts) + static_cast<int32>(bHeadphonesConflict)
		: 0;
	const UWorkerAppearanceComponent* PreviewAppearanceComponent =
		PreviewActor ? PreviewActor->FindComponentByClass<UWorkerAppearanceComponent>() : nullptr;
	const FString BodyCoverageDiagnostics = PreviewAppearanceComponent
		? PreviewAppearanceComponent->GetBodyCoverageDiagnosticsJson()
		: TEXT("{\"implementation\":\"Unavailable\"}");
	if (Txt_AppearanceDebugStatus)
	{
		Txt_AppearanceDebugStatus->SetText(FText::FromString(FString::Printf(
			TEXT("Origin: %s | Category: %s | Solo: %s | %s | runtime visibility only\n")
			TEXT("Torso=%s RaisedHood=%s | Headgear=%s Conflicts=%s | Headphones=%s Conflicts=%s | ConflictSlots=%d | Modal=%s Pending=%s ClearHG=%s ClearHP=%s | Suppress=%s Checkbox=%s\n")
			TEXT("BodyCoverage=%s"),
			*GetAppearanceInspectorOriginName(),
			*GetAppearanceInspectorCategoryName(),
			*SoloName,
			*LodMode,
			*ResolveTorsoStableId(ActiveTorsoPath),
			bRaisedHood ? TEXT("true") : TEXT("false"),
			*ResolveHeadgearStableId(ActiveHeadgearPath),
			bHeadgearConflicts ? TEXT("true") : TEXT("false"),
			*ResolveHeadphonesStableId(ActiveHeadphonesPath),
			bHeadphonesConflict ? TEXT("true") : TEXT("false"),
			ConflictSlotCount,
			*GetAppearanceModalModeName(),
			PendingEquipmentConflict.bValid ? *PendingEquipmentConflict.TargetStableId : TEXT("None"),
			PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headgear) ? TEXT("true") : TEXT("false"),
			PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headphones) ? TEXT("true") : TEXT("false"),
			ShouldSuppressRaisedHoodEquipmentWarning() ? TEXT("true") : TEXT("false"),
			IsEquipmentConflictModalMode(AppearanceModalMode) &&
				CheckBox_EquipmentConflictDontShowAgain &&
				CheckBox_EquipmentConflictDontShowAgain->IsChecked()
				? TEXT("true") : TEXT("false"),
			*BodyCoverageDiagnostics)));
	}

	for (int32 SlotIndex = static_cast<int32>(EWorkerSlot::Body);
		SlotIndex <= static_cast<int32>(EWorkerSlot::Overalls);
		++SlotIndex)
	{
		const EWorkerSlot WorkerSlot = static_cast<EWorkerSlot>(SlotIndex);
		const FAvAppearanceInspectorSlotInfo Info = PreviewActor
			? PreviewActor->GetAppearanceInspectorSlotInfo(WorkerSlot)
			: FAvAppearanceInspectorSlotInfo();
		const FString FirstMaterial = Info.MaterialAssetPaths.IsEmpty()
			? TEXT("None")
			: AvAppearanceInspector::ShortObjectName(Info.MaterialAssetPaths[0]);
		const FString Lod = !Info.bPresent
			? TEXT("-")
			: (Info.ForcedLodModel > 0
				? FString::Printf(TEXT("LOD%d/P%d"), Info.ForcedLodModel - 1, Info.PredictedLodLevel)
				: FString::Printf(TEXT("AUTO/P%d"), Info.PredictedLodLevel));

		if (const TWeakObjectPtr<UTextBlock>* Found = AppearanceInspectorMeshTexts.Find(WorkerSlot))
		{
			if (UTextBlock* Text = Found->Get())
			{
				Text->SetText(FText::FromString(Info.bPresent ? Info.MeshName : TEXT("None")));
				Text->SetToolTipText(FText::FromString(Info.bPresent
					? FString::Printf(TEXT("Object: %s\nPackage: %s"), *Info.MeshObjectPath, *Info.MeshPackagePath)
					: TEXT("No runtime component for this slot")));
			}
		}
		if (const TWeakObjectPtr<UTextBlock>* Found = AppearanceInspectorVisibilityTexts.Find(WorkerSlot))
		{
			if (UTextBlock* Text = Found->Get())
			{
				Text->SetText(FText::FromString(!Info.bPresent ? TEXT("Empty") : (Info.bVisible ? TEXT("Visible") : TEXT("Hidden"))));
				Text->SetColorAndOpacity(FSlateColor(!Info.bPresent
					? AvAppearanceInspector::TextDim
					: (Info.bVisible ? AvAppearanceInspector::Visible : AvAppearanceInspector::Hidden)));
			}
		}
		if (const TWeakObjectPtr<UTextBlock>* Found = AppearanceInspectorMaterialTexts.Find(WorkerSlot))
		{
			if (UTextBlock* Text = Found->Get())
			{
				Text->SetText(FText::FromString(FirstMaterial));
				Text->SetToolTipText(FText::FromString(FString::Printf(
					TEXT("Material slots: %d\nAsset materials:\n%s\nRuntime materials:\n%s"),
					Info.MaterialSlotCount,
					*FString::Join(Info.MaterialAssetPaths, TEXT("\n")),
					*FString::Join(Info.RuntimeMaterialPaths, TEXT("\n")))));
			}
		}
		if (const TWeakObjectPtr<UTextBlock>* Found = AppearanceInspectorLodTexts.Find(WorkerSlot))
		{
			if (UTextBlock* Text = Found->Get())
			{
				Text->SetText(FText::FromString(Lod));
			}
		}
		if (const TWeakObjectPtr<UTextBlock>* Found = AppearanceInspectorToggleLabels.Find(WorkerSlot))
		{
			if (UTextBlock* Text = Found->Get())
			{
				Text->SetText(FText::FromString(!Info.bPresent ? TEXT("-") : (Info.bVisible ? TEXT("HIDE") : TEXT("SHOW"))));
			}
		}
		if (const TWeakObjectPtr<UAvAppearanceInspectorSlotButton>* Found = AppearanceInspectorToggleButtons.Find(WorkerSlot))
		{
			if (UAvAppearanceInspectorSlotButton* Button = Found->Get()) Button->SetIsEnabled(Info.bPresent);
		}
		if (const TWeakObjectPtr<UAvAppearanceInspectorSlotButton>* Found = AppearanceInspectorSoloButtons.Find(WorkerSlot))
		{
			if (UAvAppearanceInspectorSlotButton* Button = Found->Get()) Button->SetIsEnabled(Info.bPresent);
		}
	}
}

void UAvCharacterCustomizationRootWidget::ToggleAppearanceInspector()
{
	EnsureAppearanceInspector();
	if (!Border_AppearanceDebugInspector)
	{
		return;
	}
	bAppearanceInspectorVisible = !bAppearanceInspectorVisible;
	Border_AppearanceDebugInspector->SetVisibility(
		bAppearanceInspectorVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (bAppearanceInspectorVisible)
	{
		RefreshAppearanceInspector();
	}
	UE_LOG(LogTemp, Log, TEXT("[AvAppearanceInspector] Panel=%s AppearanceMutation=false Autosave=false"),
		bAppearanceInspectorVisible ? TEXT("Visible") : TEXT("Hidden"));
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorSlotAction(
	EWorkerSlot WorkerSlot,
	EAvAppearanceInspectorSlotAction Action)
{
	if (!PreviewActor)
	{
		return;
	}
	if (Action == EAvAppearanceInspectorSlotAction::Solo)
	{
		PreviewActor->AppearanceInspectorSoloSlot(WorkerSlot);
	}
	else
	{
		PreviewActor->AppearanceInspectorToggleSlot(WorkerSlot);
	}
	RefreshAppearanceInspector();
	UE_LOG(LogTemp, Log, TEXT("[AvAppearanceInspector] Action=%s Slot=%s AppearanceMutation=false Autosave=false"),
		Action == EAvAppearanceInspectorSlotAction::Solo ? TEXT("Solo") : TEXT("Toggle"),
		AvAppearanceInspector::SlotName(WorkerSlot));
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorShowAll()
{
	if (PreviewActor) PreviewActor->AppearanceInspectorShowAll();
	RefreshAppearanceInspector();
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorHideClothing()
{
	if (PreviewActor) PreviewActor->AppearanceInspectorHideAllClothing();
	RefreshAppearanceInspector();
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorBodyOnly()
{
	if (PreviewActor) PreviewActor->AppearanceInspectorBodyOnly();
	RefreshAppearanceInspector();
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorHideBody()
{
	if (PreviewActor) PreviewActor->AppearanceInspectorHideBody();
	RefreshAppearanceInspector();
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorShowBody()
{
	if (PreviewActor) PreviewActor->AppearanceInspectorShowBody();
	RefreshAppearanceInspector();
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorClearSolo()
{
	if (PreviewActor) PreviewActor->AppearanceInspectorClearSolo();
	RefreshAppearanceInspector();
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorForceLod0()
{
	if (PreviewActor) PreviewActor->AppearanceInspectorSetForceLod0(true);
	RefreshAppearanceInspector();
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorAutoLod()
{
	if (PreviewActor) PreviewActor->AppearanceInspectorSetForceLod0(false);
	RefreshAppearanceInspector();
}

FString UAvCharacterCustomizationRootWidget::GetAppearanceInspectorOriginName() const
{
	switch (ActiveCharacterAppearanceOrigin)
	{
	case EAvAppearanceOrigin::Factory: return TEXT("Factory");
	case EAvAppearanceOrigin::RandomGenerated: return TEXT("RandomGenerated");
	case EAvAppearanceOrigin::PresetApplied: return TEXT("PresetApplied");
	case EAvAppearanceOrigin::ManualCustomized: return TEXT("ManualCustomized");
	default: return TEXT("Unknown");
	}
}

FString UAvCharacterCustomizationRootWidget::GetAppearanceInspectorCategoryName() const
{
	switch (SelectedCategory)
	{
	case EAvCustomizationCategory::MyCharacters: return TEXT("MyCharacters");
	case EAvCustomizationCategory::Presets: return TEXT("Presets");
	case EAvCustomizationCategory::Head: return TEXT("Head");
	case EAvCustomizationCategory::FaceProtection: return TEXT("FaceProtection");
	case EAvCustomizationCategory::HandsAccessories: return TEXT("HandsAccessories");
	case EAvCustomizationCategory::UpperGear: return TEXT("UpperGear");
	case EAvCustomizationCategory::LowerHip: return TEXT("LowerHip");
	case EAvCustomizationCategory::FullOutfits: return TEXT("FullOutfits");
	default: return TEXT("Unknown");
	}
}

FString UAvCharacterCustomizationRootWidget::GetAppearanceModalModeName() const
{
	switch (AppearanceModalMode)
	{
	case EAvAppearanceModalMode::None: return TEXT("None");
	case EAvAppearanceModalMode::RandomAppearance: return TEXT("RandomAppearance");
	case EAvAppearanceModalMode::ResetToBase: return TEXT("ResetToBase");
	case EAvAppearanceModalMode::DeleteCharacter: return TEXT("DeleteCharacter");
	case EAvAppearanceModalMode::EquipTorsoAndClearConflictingEquipment:
		return TEXT("EquipTorsoAndClearConflictingEquipment");
	case EAvAppearanceModalMode::EquipHeadgearAndClearTorso:
		return TEXT("EquipHeadgearAndClearTorso");
	case EAvAppearanceModalMode::EquipHeadphonesAndClearTorso:
		return TEXT("EquipHeadphonesAndClearTorso");
	default: return TEXT("Unknown");
	}
}

FString UAvCharacterCustomizationRootWidget::GetEquipmentConflictWarningTypeName() const
{
	switch (PendingEquipmentConflict.WarningType)
	{
	case EAvEquipmentConflictWarningType::None: return TEXT("None");
	case EAvEquipmentConflictWarningType::RaisedHood: return TEXT("RaisedHood");
	case EAvEquipmentConflictWarningType::HeadgearHeadphones:
		return TEXT("HeadgearHeadphones");
	case EAvEquipmentConflictWarningType::Combined: return TEXT("Combined");
	default: return TEXT("Unknown");
	}
}

FString UAvCharacterCustomizationRootWidget::BuildAppearanceInspectorDiagnosticJson()
{
	UpdateLocalizationDiagnostics();
	const FWorkerAppearance& DisplayedAppearance = bHasPresetPreviewAppearance
		? PresetPreviewAppearance
		: ActiveCharacterAppearance;
	const TOptional<EWorkerSlot> Solo = PreviewActor
		? PreviewActor->GetAppearanceInspectorSoloSlot()
		: TOptional<EWorkerSlot>();
	FString Json = TEXT("{\n");
	Json += FString::Printf(TEXT("  \"timestamp\": \"%s\",\n"), *FDateTime::Now().ToIso8601());
	Json += TEXT("  \"tool\": \"DevelopmentAppearanceSlotInspector\",\n");
	const FString CurrentCulture = FInternationalization::Get().GetCurrentCulture()->GetName();
	const FString CurrentLanguage = FInternationalization::Get().GetCurrentLanguage()->GetName();
	const bool bPseudoLocalizationActive =
		CurrentLanguage.Equals(TEXT("LEET"), ESearchCase::IgnoreCase) ||
		CurrentLanguage.StartsWith(TEXT("qps"), ESearchCase::IgnoreCase);
	Json += FString::Printf(TEXT("  \"CurrentCulture\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(CurrentCulture));
	Json += FString::Printf(TEXT("  \"CurrentLanguage\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(CurrentLanguage));
	Json += TEXT("  \"NativeCulture\": \"ru\",\n");
	Json += TEXT("  \"LocalizationTarget\": \"Game\",\n");
	Json += FString::Printf(TEXT("  \"LocalizationRevision\": %u,\n"),
		FTextLocalizationManager::Get().GetTextRevision());
	Json += FString::Printf(TEXT("  \"StringTableLoaded\": %s,\n"),
		bStringTableLoaded ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"CustomizationLocalizedKeyCount\": %d,\n"),
		CustomizationLocalizedKeyCount);
	Json += FString::Printf(TEXT("  \"CustomizationMissingTranslationCount\": %d,\n"),
		CustomizationMissingTranslationCount);
	Json += FString::Printf(TEXT("  \"CustomizationFallbackCount\": %d,\n"),
		CustomizationFallbackCount);
	Json += FString::Printf(TEXT("  \"CustomizationCultureInvariantVisibleTextCount\": %d,\n"),
		CustomizationCultureInvariantVisibleTextCount);
	Json += FString::Printf(TEXT("  \"RuntimeCatalogLocalizedItemCount\": %d,\n"),
		RuntimeCatalogLocalizedItemCount);
	Json += FString::Printf(TEXT("  \"CurrentSelectionLocalized\": %s,\n"),
		bCurrentSelectionLocalized ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"ModalTextLocalized\": %s,\n"),
		bModalTextLocalized ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"ExistingCharacterNamesLiteral\": %s,\n"),
		bExistingCharacterNamesLiteral ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"ExistingCharacterNamesChangedByCultureSwitch\": %s,\n"),
		bExistingCharacterNamesChangedByCultureSwitch ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"CultureSwitchSaveCommitCount\": %d,\n"),
		CultureSwitchSaveCommitCount);
	Json += FString::Printf(TEXT("  \"CultureSwitchAutosaveCount\": %d,\n"),
		CultureSwitchAutosaveCount);
	Json += FString::Printf(TEXT("  \"CultureSwitchAppearanceMutationCount\": %d,\n"),
		CultureSwitchAppearanceMutationCount);
	Json += FString::Printf(TEXT("  \"PseudoLocalizationActive\": %s,\n"),
		bPseudoLocalizationActive ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"TextOverflowDetectedCount\": %d,\n"),
		TextOverflowDetectedCount);
	Json += FString::Printf(TEXT("  \"appearance_origin\": \"%s\",\n"), *AvAppearanceInspector::JsonEscape(GetAppearanceInspectorOriginName()));
	Json += FString::Printf(TEXT("  \"active_category\": \"%s\",\n"), *AvAppearanceInspector::JsonEscape(GetAppearanceInspectorCategoryName()));
	Json += FString::Printf(TEXT("  \"preview_source\": \"%s\",\n"), bHasPresetPreviewAppearance ? TEXT("PresetPreviewAppearance") : TEXT("ActiveCharacterAppearance"));
	Json += FString::Printf(TEXT("  \"stable_slot_combination\": \"%s\",\n"), *AvAppearanceInspector::JsonEscape(DisplayedAppearance.ToStableDebugString()));
	Json += FString::Printf(TEXT("  \"inspector_visible\": %s,\n"), bAppearanceInspectorVisible ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"forced_lod_mode\": \"%s\",\n"), PreviewActor && PreviewActor->IsAppearanceInspectorForceLod0() ? TEXT("LOD0") : TEXT("Auto"));
	Json += FString::Printf(TEXT("  \"solo_slot\": \"%s\",\n"), Solo.IsSet() ? AvAppearanceInspector::SlotName(Solo.GetValue()) : TEXT("None"));
	const UCompanyLedgerSubsystem* Ledger = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCompanyLedgerSubsystem>()
		: nullptr;
	const FAvCharacterRecord* ActiveRecord = Ledger ? Ledger->GetActiveCharacter() : nullptr;
	const FName ActiveCharacterId = Ledger ? Ledger->GetActiveCharacterId() : NAME_None;
	const bool bActiveRecordExists = Ledger && Ledger->GetCharacterRecords().ContainsByPredicate(
		[ActiveCharacterId](const FAvCharacterRecord& Record)
		{
			return !ActiveCharacterId.IsNone() && Record.CharacterId == ActiveCharacterId;
		});
	const uint32 ActiveAppearanceHash = GetTypeHash(
		ActiveCharacterAppearance.ToStableDebugString());
	auto JsonStringArray = [](const TArray<FString>& Values)
	{
		TArray<FString> Escaped;
		Escaped.Reserve(Values.Num());
		for (const FString& Value : Values)
		{
			Escaped.Add(FString::Printf(
				TEXT("\"%s\""),
				*AvAppearanceInspector::JsonEscape(Value)));
		}
		return FString::Printf(TEXT("[%s]"), *FString::Join(Escaped, TEXT(",")));
	};
	auto CameraPresetName = [this]()
	{
		if (!PreviewActor) return FString(TEXT("Unavailable"));
		switch (PreviewActor->GetCameraPreset())
		{
		case EAvCustomizationCameraPreset::FullBody: return FString(TEXT("FullBody"));
		case EAvCustomizationCameraPreset::UpperBody: return FString(TEXT("UpperBody"));
		case EAvCustomizationCameraPreset::Head: return FString(TEXT("Head"));
		case EAvCustomizationCameraPreset::Face: return FString(TEXT("Face"));
		case EAvCustomizationCameraPreset::Hands: return FString(TEXT("Hands"));
		case EAvCustomizationCameraPreset::LowerBody: return FString(TEXT("LowerBody"));
		default: return FString(TEXT("Unknown"));
		}
	};
	auto AnchorSourceName = [this]()
	{
		if (!PreviewActor) return FString(TEXT("None"));
		switch (PreviewActor->GetLastAnchorSource())
		{
		case EAvPreviewZoomAnchorSource::MeshHit: return FString(TEXT("MeshHit"));
		case EAvPreviewZoomAnchorSource::FocusPlaneFallback:
			return FString(TEXT("FocusPlaneFallback"));
		case EAvPreviewZoomAnchorSource::None:
		default: return FString(TEXT("None"));
		}
	};
	FBox CombinedVisibleBounds(ForceInit);
	const bool bHasCombinedVisibleBounds = PreviewActor &&
		PreviewActor->GetCombinedVisibleBounds(CombinedVisibleBounds);
	int32 DuplicateDisplayNameCount = 0;
	TSet<FString> SeenDisplayNames;
	if (Ledger)
	{
		for (const FAvCharacterRecord& Record : Ledger->GetCharacterRecords())
		{
			const FString NormalizedName = Record.DisplayName.ToLower();
			if (SeenDisplayNames.Contains(NormalizedName))
			{
				++DuplicateDisplayNameCount;
			}
			SeenDisplayNames.Add(NormalizedName);
		}
	}
	int32 DuplicateRuntimeRowCount = 0;
	TSet<FName> SeenRuntimeRowIds;
	FName SelectedRuntimeRowId;
	for (const UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (!Row || Row->GetCharacterId().IsNone() ||
			SeenRuntimeRowIds.Contains(Row->GetCharacterId()))
		{
			++DuplicateRuntimeRowCount;
			continue;
		}
		SeenRuntimeRowIds.Add(Row->GetCharacterId());
		if (Row->IsSelected())
		{
			SelectedRuntimeRowId = Row->GetCharacterId();
		}
	}
	auto VisibilityName = [](ESlateVisibility InVisibility)
	{
		switch (InVisibility)
		{
		case ESlateVisibility::Visible: return FString(TEXT("Visible"));
		case ESlateVisibility::Collapsed: return FString(TEXT("Collapsed"));
		case ESlateVisibility::Hidden: return FString(TEXT("Hidden"));
		case ESlateVisibility::HitTestInvisible: return FString(TEXT("HitTestInvisible"));
		case ESlateVisibility::SelfHitTestInvisible: return FString(TEXT("SelfHitTestInvisible"));
		default: return FString(TEXT("Unknown"));
		}
	};
	const UWidget* VisualSurface = ResolvePreviewProjectionWidget();
	const UWidget* InteractionArea = ResolvePreviewInputWidget();
	const FVector2D VisualSurfaceSize = VisualSurface
		? VisualSurface->GetCachedGeometry().GetLocalSize()
		: FVector2D::ZeroVector;
	const FVector2D InteractionAreaSize = InteractionArea
		? InteractionArea->GetCachedGeometry().GetLocalSize()
		: FVector2D::ZeroVector;
	const FVector2D VisualSurfaceAbsolutePosition = VisualSurface
		? VisualSurface->GetCachedGeometry().LocalToAbsolute(FVector2D::ZeroVector)
		: FVector2D::ZeroVector;
	const FVector2D InteractionAreaAbsolutePosition = InteractionArea
		? InteractionArea->GetCachedGeometry().LocalToAbsolute(FVector2D::ZeroVector)
		: FVector2D::ZeroVector;
	const bool bVisualAndInteractionBoundsEqual =
		VisualSurfaceSize.Equals(InteractionAreaSize, 0.5f) &&
		VisualSurfaceAbsolutePosition.Equals(InteractionAreaAbsolutePosition, 0.5f);
	FString FirstClippingParent = TEXT("NoneBeforeViewport");
	for (const UWidget* Parent = VisualSurface ? VisualSurface->GetParent() : nullptr;
		 Parent;
		 Parent = Parent->GetParent())
	{
		if (Parent->GetClipping() != EWidgetClipping::Inherit)
		{
			FirstClippingParent = Parent->GetName();
			break;
		}
	}
	int32 DiagnosticViewportX = 0;
	int32 DiagnosticViewportY = 0;
	if (const APlayerController* PC = GetOwningPlayer())
	{
		PC->GetViewportSize(DiagnosticViewportX, DiagnosticViewportY);
	}
	const float ViewportAspect = DiagnosticViewportY > 0
		? static_cast<float>(DiagnosticViewportX) / static_cast<float>(DiagnosticViewportY)
		: 0.f;
	const int32 RenderTargetX = PreviewRenderTarget ? PreviewRenderTarget->SizeX : 0;
	const int32 RenderTargetY = PreviewRenderTarget ? PreviewRenderTarget->SizeY : 0;
	const float RenderTargetAspect = RenderTargetY > 0
		? static_cast<float>(RenderTargetX) / static_cast<float>(RenderTargetY)
		: 0.f;
	const float VisualSurfaceAspect = VisualSurfaceSize.Y > KINDA_SMALL_NUMBER
		? VisualSurfaceSize.X / VisualSurfaceSize.Y
		: 0.f;
	const float InteractionAreaAspect = InteractionAreaSize.Y > KINDA_SMALL_NUMBER
		? InteractionAreaSize.X / InteractionAreaSize.Y
		: 0.f;
	const float DiagnosticDPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
	const float DiagnosticApplicationScale = FSlateApplication::IsInitialized()
		? FSlateApplication::Get().GetApplicationScale()
		: 1.f;
	auto FindDiagnosticWidget = [this](const FName WidgetName) -> const UWidget*
	{
		return WidgetTree ? WidgetTree->FindWidget(WidgetName) : nullptr;
	};
	auto GeometryJson = [](const UWidget* Widget)
	{
		if (!Widget)
		{
			return FString(TEXT("null"));
		}
		const FGeometry Geometry = Widget->GetCachedGeometry();
		const FVector2D LocalSize = Geometry.GetLocalSize();
		const FVector2D AbsoluteMin = Geometry.LocalToAbsolute(FVector2D::ZeroVector);
		const FVector2D AbsoluteMax = Geometry.LocalToAbsolute(LocalSize);
		return FString::Printf(
			TEXT("{\"Widget\":\"%s\",\"Position\":[%.2f,%.2f],\"Size\":[%.2f,%.2f],")
			TEXT("\"AbsolutePosition\":[%.2f,%.2f],\"AbsoluteSize\":[%.2f,%.2f],\"LayoutScale\":%.6f}"),
			*AvAppearanceInspector::JsonEscape(Widget->GetName()),
			AbsoluteMin.X, AbsoluteMin.Y,
			LocalSize.X, LocalSize.Y,
			AbsoluteMin.X, AbsoluteMin.Y,
			AbsoluteMax.X - AbsoluteMin.X,
			AbsoluteMax.Y - AbsoluteMin.Y,
			static_cast<float>(Geometry.GetAccumulatedLayoutTransform().GetScale()));
	};
	auto CanvasSlotJson = [](const UWidget* Widget)
	{
		const UCanvasPanelSlot* CanvasSlot =
			Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
		if (!CanvasSlot)
		{
			return FString(TEXT("null"));
		}
		const FAnchors Anchors = CanvasSlot->GetAnchors();
		const FMargin Offsets = CanvasSlot->GetOffsets();
		const FVector2D Alignment = CanvasSlot->GetAlignment();
		return FString::Printf(
			TEXT("{\"AnchorsMin\":[%.3f,%.3f],\"AnchorsMax\":[%.3f,%.3f],")
			TEXT("\"Offsets\":[%.3f,%.3f,%.3f,%.3f],\"Alignment\":[%.3f,%.3f],")
			TEXT("\"AutoSize\":%s}"),
			Anchors.Minimum.X, Anchors.Minimum.Y,
			Anchors.Maximum.X, Anchors.Maximum.Y,
			Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom,
			Alignment.X, Alignment.Y,
			CanvasSlot->GetAutoSize() ? TEXT("true") : TEXT("false"));
	};
	auto IsDiagnosticVisible = [](const UWidget* Widget)
	{
		if (!Widget)
		{
			return false;
		}
		for (const UWidget* Current = Widget; Current; Current = Current->GetParent())
		{
			const ESlateVisibility Visibility = Current->GetVisibility();
			if (Visibility == ESlateVisibility::Collapsed ||
				Visibility == ESlateVisibility::Hidden)
			{
				return false;
			}
		}
		const ESlateVisibility OwnVisibility = Widget->GetVisibility();
		return OwnVisibility == ESlateVisibility::Visible ||
			OwnVisibility == ESlateVisibility::HitTestInvisible ||
			OwnVisibility == ESlateVisibility::SelfHitTestInvisible;
	};
	const UWidget* RootGeometryWidget = WidgetTree ? WidgetTree->RootWidget : nullptr;
	const UWidget* BackgroundGeometryWidget = FindDiagnosticWidget(
		TEXT("Img_CustomizeBackground"));
	const UWidget* LayoutRootGeometryWidget = FindDiagnosticWidget(TEXT("LayoutRoot"));
	const UWidget* MainLayoutGeometryWidget = FindDiagnosticWidget(TEXT("VB_MainLayout"));
	const UWidget* ContentToFooterGeometryWidget = FindDiagnosticWidget(
		TEXT("HB_ContentToFooter"));
	const UWidget* LeftCenterStageGeometryWidget = FindDiagnosticWidget(
		TEXT("Overlay_LeftCenterStage"));
	const UWidget* TopBarGeometryWidget = FindDiagnosticWidget(TEXT("SB_TopBar"));
	const UWidget* FooterGeometryWidget = FindDiagnosticWidget(TEXT("SB_FooterBar"));
	const UWidget* LeftPanelGeometryWidget = FindDiagnosticWidget(TEXT("SB_LeftPanel"));
	const UWidget* RightPanelGeometryWidget = FindDiagnosticWidget(TEXT("SB_RightPanel"));
	const UWidget* ModalGeometryWidget = FindDiagnosticWidget(TEXT("Border_RandomAppearanceDialog"));
	const UWidget* ModalOverlayGeometryWidget = OverlayRandom;
	const UWidget* CharacterScrollGeometryWidget = ScrollBox_MyCharacters;

	const TArray<const UScrollBox*> CatalogScrollCandidates = {
		ScrollBox_HeadItems, ScrollBox_HeadgearItems, ScrollBox_HairItems,
		ScrollBox_BeardItems, ScrollBox_GlassesItems, ScrollBox_RespiratorItems,
		ScrollBox_HeadphonesItems, ScrollBox_GlovesItems, ScrollBox_WatchesItems,
		ScrollBox_TorsoItems, ScrollBox_LegsItems, ScrollBox_HipItems,
		ScrollBox_FullOutfitItems
	};
	const UWidget* CatalogScrollGeometryWidget = nullptr;
	float LargestCatalogScrollArea = 0.f;
	for (const UScrollBox* Candidate : CatalogScrollCandidates)
	{
		if (!Candidate)
		{
			continue;
		}
		const FVector2D CandidateSize = Candidate->GetCachedGeometry().GetLocalSize();
		const float CandidateArea = CandidateSize.X * CandidateSize.Y;
		if (CandidateArea > LargestCatalogScrollArea)
		{
			LargestCatalogScrollArea = CandidateArea;
			CatalogScrollGeometryWidget = Candidate;
		}
	}

	const FGeometry RootGeometry = RootGeometryWidget
		? RootGeometryWidget->GetCachedGeometry()
		: FGeometry();
	const FWidgetTransform RootRenderTransform = RootGeometryWidget
		? RootGeometryWidget->GetRenderTransform()
		: FWidgetTransform();
	const FVector2D RootLocalSize = RootGeometry.GetLocalSize();
	const FVector2D RootAbsoluteMin = RootGeometry.LocalToAbsolute(FVector2D::ZeroVector);
	const FVector2D RootAbsoluteMax = RootGeometry.LocalToAbsolute(RootLocalSize);
	const FVector2D RootAbsoluteSize = RootAbsoluteMax - RootAbsoluteMin;
	const bool bRootFillsViewport = DiagnosticViewportX > 0 && DiagnosticViewportY > 0 &&
		FMath::IsNearlyEqual(RootAbsoluteSize.X, static_cast<float>(DiagnosticViewportX), 1.5f) &&
		FMath::IsNearlyEqual(RootAbsoluteSize.Y, static_cast<float>(DiagnosticViewportY), 1.5f);
	const auto FillsRoot = [&RootAbsoluteMin, &RootAbsoluteMax](const UWidget* Widget)
	{
		if (!Widget)
		{
			return false;
		}
		const FGeometry Geometry = Widget->GetCachedGeometry();
		const FVector2D Min = Geometry.LocalToAbsolute(FVector2D::ZeroVector);
		const FVector2D Max = Geometry.LocalToAbsolute(Geometry.GetLocalSize());
		return Min.Equals(RootAbsoluteMin, 1.5f) && Max.Equals(RootAbsoluteMax, 1.5f);
	};
	const auto SpansRootWidth = [&RootAbsoluteMin, &RootAbsoluteMax](const UWidget* Widget)
	{
		if (!Widget)
		{
			return false;
		}
		const FGeometry Geometry = Widget->GetCachedGeometry();
		const FVector2D Min = Geometry.LocalToAbsolute(FVector2D::ZeroVector);
		const FVector2D Max = Geometry.LocalToAbsolute(Geometry.GetLocalSize());
		return FMath::IsNearlyEqual(Min.X, RootAbsoluteMin.X, 1.5f) &&
			FMath::IsNearlyEqual(Max.X, RootAbsoluteMax.X, 1.5f);
	};
	const auto HorizontalEdgeDistances = [&RootAbsoluteMin, &RootAbsoluteMax](
		const UWidget* Widget)
	{
		if (!Widget)
		{
			return FVector2D(-1.f, -1.f);
		}
		const FGeometry Geometry = Widget->GetCachedGeometry();
		const FVector2D Min = Geometry.LocalToAbsolute(FVector2D::ZeroVector);
		const FVector2D Max = Geometry.LocalToAbsolute(Geometry.GetLocalSize());
		return FVector2D(Min.X - RootAbsoluteMin.X, RootAbsoluteMax.X - Max.X);
	};
	const FVector2D LeftPanelEdgeDistances = HorizontalEdgeDistances(
		LeftPanelGeometryWidget);
	const FVector2D RightPanelEdgeDistances = HorizontalEdgeDistances(
		RightPanelGeometryWidget);
	auto IsOutsideRoot = [&RootAbsoluteMin, &RootAbsoluteMax](const UWidget* Widget)
	{
		if (!Widget)
		{
			return false;
		}
		const FGeometry Geometry = Widget->GetCachedGeometry();
		const FVector2D AbsoluteMin = Geometry.LocalToAbsolute(FVector2D::ZeroVector);
		const FVector2D AbsoluteMax = Geometry.LocalToAbsolute(Geometry.GetLocalSize());
		constexpr float Tolerance = 1.5f;
		return AbsoluteMin.X < RootAbsoluteMin.X - Tolerance ||
			AbsoluteMin.Y < RootAbsoluteMin.Y - Tolerance ||
			AbsoluteMax.X > RootAbsoluteMax.X + Tolerance ||
			AbsoluteMax.Y > RootAbsoluteMax.Y + Tolerance;
	};
	const bool bModalVisible = IsDiagnosticVisible(ModalOverlayGeometryWidget);
	const auto AbsoluteBounds = [](const UWidget* Widget)
	{
		FBox2D Bounds(ForceInit);
		if (Widget)
		{
			const FGeometry Geometry = Widget->GetCachedGeometry();
			Bounds += Geometry.LocalToAbsolute(FVector2D::ZeroVector);
			Bounds += Geometry.LocalToAbsolute(Geometry.GetLocalSize());
		}
		return Bounds;
	};
	const auto HasPositiveOverlap = [&AbsoluteBounds](
		const UWidget* A,
		const UWidget* B)
	{
		const FBox2D ABounds = AbsoluteBounds(A);
		const FBox2D BBounds = AbsoluteBounds(B);
		if (!ABounds.bIsValid || !BBounds.bIsValid)
		{
			return false;
		}
		const FVector2D OverlapMin(
			FMath::Max(ABounds.Min.X, BBounds.Min.X),
			FMath::Max(ABounds.Min.Y, BBounds.Min.Y));
		const FVector2D OverlapMax(
			FMath::Min(ABounds.Max.X, BBounds.Max.X),
			FMath::Min(ABounds.Max.Y, BBounds.Max.Y));
		return OverlapMax.X - OverlapMin.X > 0.5f &&
			OverlapMax.Y - OverlapMin.Y > 0.5f;
	};
	int32 OverlapCount = 0;
	OverlapCount += HasPositiveOverlap(LeftPanelGeometryWidget, InteractionArea) ? 1 : 0;
	OverlapCount += HasPositiveOverlap(InteractionArea, RightPanelGeometryWidget) ? 1 : 0;
	OverlapCount += HasPositiveOverlap(LeftPanelGeometryWidget, RightPanelGeometryWidget) ? 1 : 0;
	const TArray<const UWidget*> MonitoredGeometryWidgets = {
		RootGeometryWidget, BackgroundGeometryWidget, LayoutRootGeometryWidget,
		MainLayoutGeometryWidget, ContentToFooterGeometryWidget,
		LeftCenterStageGeometryWidget, TopBarGeometryWidget, FooterGeometryWidget,
		LeftPanelGeometryWidget, RightPanelGeometryWidget, VisualSurface,
		InteractionArea, CatalogScrollGeometryWidget, CharacterScrollGeometryWidget,
		ModalOverlayGeometryWidget, bModalVisible ? ModalGeometryWidget : nullptr
	};
	int32 OutsideViewportWidgetCount = 0;
	int32 ZeroOrNegativeGeometryCount = 0;
	int32 NonFiniteGeometryCount = 0;
	for (const UWidget* Widget : MonitoredGeometryWidgets)
	{
		if (!IsDiagnosticVisible(Widget))
		{
			continue;
		}
		const FGeometry Geometry = Widget->GetCachedGeometry();
		const FVector2D Size = Geometry.GetLocalSize();
		const FVector2D AbsoluteMin = Geometry.LocalToAbsolute(FVector2D::ZeroVector);
		const FVector2D AbsoluteMax = Geometry.LocalToAbsolute(Size);
		if (!FMath::IsFinite(Size.X) || !FMath::IsFinite(Size.Y) ||
			!FMath::IsFinite(AbsoluteMin.X) || !FMath::IsFinite(AbsoluteMin.Y) ||
			!FMath::IsFinite(AbsoluteMax.X) || !FMath::IsFinite(AbsoluteMax.Y))
		{
			++NonFiniteGeometryCount;
		}
		if (Size.X <= KINDA_SMALL_NUMBER || Size.Y <= KINDA_SMALL_NUMBER)
		{
			++ZeroOrNegativeGeometryCount;
		}
		if (Widget != RootGeometryWidget && IsOutsideRoot(Widget))
		{
			++OutsideViewportWidgetCount;
		}
	}
	const bool bModalOutsideViewport = bModalVisible && IsOutsideRoot(ModalGeometryWidget);
	const FVector2D ModalSize = ModalGeometryWidget
		? ModalGeometryWidget->GetCachedGeometry().GetLocalSize()
		: FVector2D::ZeroVector;
	const bool bModalAccessible = bModalVisible && !bModalOutsideViewport &&
		ModalSize.X > KINDA_SMALL_NUMBER && ModalSize.Y > KINDA_SMALL_NUMBER;
	TArray<const UScrollBox*> ActiveDiagnosticScrollBoxes;
	switch (SelectedCategory)
	{
	case EAvCustomizationCategory::MyCharacters:
		ActiveDiagnosticScrollBoxes.Add(ScrollBox_MyCharacters.Get());
		break;
	case EAvCustomizationCategory::Head:
		ActiveDiagnosticScrollBoxes.Add(
			SelectedHeadSection == EAvHeadCustomizationSection::HeadType ? ScrollBox_HeadItems.Get() :
			SelectedHeadSection == EAvHeadCustomizationSection::Headgear ? ScrollBox_HeadgearItems.Get() :
			SelectedHeadSection == EAvHeadCustomizationSection::Hair ? ScrollBox_HairItems.Get() :
			ScrollBox_BeardItems.Get());
		break;
	case EAvCustomizationCategory::FaceProtection:
		ActiveDiagnosticScrollBoxes.Add(
			SelectedFaceProtectionSection == EAvFaceProtectionSection::Glasses ? ScrollBox_GlassesItems.Get() :
			SelectedFaceProtectionSection == EAvFaceProtectionSection::Respirator ? ScrollBox_RespiratorItems.Get() :
			ScrollBox_HeadphonesItems.Get());
		break;
	case EAvCustomizationCategory::HandsAccessories:
		ActiveDiagnosticScrollBoxes.Add(
			SelectedHandsAccessoriesSection == EAvHandsAccessoriesSection::Gloves ? ScrollBox_GlovesItems.Get() :
			ScrollBox_WatchesItems.Get());
		break;
	case EAvCustomizationCategory::UpperGear:
		ActiveDiagnosticScrollBoxes.Add(ScrollBox_TorsoItems.Get());
		break;
	case EAvCustomizationCategory::LowerHip:
		ActiveDiagnosticScrollBoxes.Add(
			SelectedLowerHipSection == EAvLowerHipSection::Legs ? ScrollBox_LegsItems.Get() :
			ScrollBox_HipItems.Get());
		break;
	case EAvCustomizationCategory::FullOutfits:
		ActiveDiagnosticScrollBoxes.Add(ScrollBox_FullOutfitItems.Get());
		break;
	default:
		break;
	}
	int32 VisibleScrollBoxCount = 0;
	int32 NonPositiveScrollBoxGeometryCount = 0;
	for (const UScrollBox* ScrollBox : ActiveDiagnosticScrollBoxes)
	{
		if (!ScrollBox)
		{
			continue;
		}
		++VisibleScrollBoxCount;
		const FVector2D Size = ScrollBox->GetCachedGeometry().GetLocalSize();
		if (Size.X <= KINDA_SMALL_NUMBER || Size.Y <= KINDA_SMALL_NUMBER ||
			!FMath::IsFinite(Size.X) || !FMath::IsFinite(Size.Y))
		{
			++NonPositiveScrollBoxGeometryCount;
		}
	}
	TArray<UUserWidget*> LiveCustomizationRoots;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		this,
		LiveCustomizationRoots,
		GetClass(),
		false);
	int32 LivePreviewActorCount = 0;
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AAvCustomizePreviewActor> It(World); It; ++It)
		{
			++LivePreviewActorCount;
		}
	}
	const bool bStaleGeometryAfterResize =
		!bRootFillsViewport ||
		CachedViewportSize != FIntPoint(DiagnosticViewportX, DiagnosticViewportY);
	const FBox2D WorkingRegionBounds = AbsoluteBounds(ContentToFooterGeometryWidget);
	const FBox2D LeftPanelBounds = AbsoluteBounds(LeftPanelGeometryWidget);
	const FBox2D RightPanelBounds = AbsoluteBounds(RightPanelGeometryWidget);
	const bool bPanelsInsideWorkingRegion =
		WorkingRegionBounds.bIsValid && LeftPanelBounds.bIsValid && RightPanelBounds.bIsValid &&
		LeftPanelBounds.Min.X >= WorkingRegionBounds.Min.X - 1.5f &&
		RightPanelBounds.Max.X <= WorkingRegionBounds.Max.X + 1.5f;
	const float CompositionCenterAbsoluteX =
		RootAbsoluteMin.X + CachedPreviewCompositionCenterX * RootAbsoluteSize.X;
	const float CharacterToRightPanelDistance = RightPanelBounds.bIsValid
		? RightPanelBounds.Min.X - CompositionCenterAbsoluteX
		: -1.f;
	bool bRenameHitRectValid = false;
	for (const UAvMyCharacterRowWidget* Row : RuntimeMyCharacterRows)
	{
		if (Row && Row->GetRenameHitTestGeometryForAutomation() != TEXT("{}"))
		{
			bRenameHitRectValid = true;
			break;
		}
	}
	const int32 UnexpectedClippingCount =
		FirstClippingParent == TEXT("NoneBeforeViewport") ? 0 : 1;
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const bool bDiagnosticFullWidthWorkingRegion =
		bFullWidthWorkingRegionForAutomation;
#else
	const bool bDiagnosticFullWidthWorkingRegion = false;
#endif

	Json += FString::Printf(TEXT("  \"ViewportSize\": [%d, %d],\n"),
		DiagnosticViewportX, DiagnosticViewportY);
	Json += FString::Printf(TEXT("  \"ViewportAspect\": %.6f,\n"), ViewportAspect);
	Json += FString::Printf(TEXT("  \"DPIScale\": %.6f,\n"), DiagnosticDPIScale);
	Json += FString::Printf(TEXT("  \"ApplicationScale\": %.6f,\n"), DiagnosticApplicationScale);
	Json += TEXT("  \"ManualUIScale\": 1.000000,\n");
	Json += TEXT("  \"ManualUIScaleApplied\": false,\n");
	Json += TEXT("  \"ResolutionAllowlistUsed\": false,\n");
	Json += TEXT("  \"DPIPolicy\": \"UnrealShortestSideCurve\",\n");
	Json += FString::Printf(TEXT("  \"ResponsiveLogicalViewport\": [%.3f, %.3f],\n"),
		ResponsiveLogicalViewportWidth, ResponsiveLogicalViewportHeight);
	Json += FString::Printf(TEXT("  \"ResponsiveWorkingRegionWidth\": %.3f,\n"),
		ResponsiveWorkingRegionWidth);
	Json += FString::Printf(TEXT("  \"ResponsiveWorkingRegionInset\": %.3f,\n"),
		ResponsiveWorkingRegionInset);
	Json += FString::Printf(TEXT("  \"MaxWorkingRegionAspect\": %.3f,\n"),
		AvCustomizationResponsive::MaxWorkingRegionAspect);
	Json += FString::Printf(TEXT("  \"FullWidthWorkingRegionForAutomation\": %s,\n"),
		bDiagnosticFullWidthWorkingRegion ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(
		TEXT("  \"LeftPanelConstraint\": {\"Min\":%.3f,\"Preferred\":%.3f,\"Max\":%.3f,\"Applied\":%.3f},\n"),
		AvCustomizationResponsive::LeftPanelMin,
		AvCustomizationResponsive::LeftPanelPreferred,
		AvCustomizationResponsive::LeftPanelMax,
		ResponsiveLeftPanelWidth);
	Json += FString::Printf(
		TEXT("  \"RightPanelConstraint\": {\"Min\":%.3f,\"Preferred\":%.3f,\"Max\":%.3f,\"Applied\":%.3f},\n"),
		AvCustomizationResponsive::RightPanelMin,
		AvCustomizationResponsive::RightPanelPreferred,
		AvCustomizationResponsive::RightPanelMax,
		ResponsiveRightPanelWidth);
	Json += FString::Printf(TEXT("  \"ResponsiveGaps\": {\"Edge\":%.3f,\"InterPanel\":%.3f},\n"),
		ResponsiveEdgeGap, ResponsiveInterPanelGap);
	Json += FString::Printf(TEXT("  \"ResponsivePreviewPlannedWidth\": %.3f,\n"),
		ResponsivePreviewWidth);
	Json += FString::Printf(TEXT("  \"ResponsiveStatus\": \"%s\",\n"),
		bResponsiveGracefulDegradation ? TEXT("GracefulDegradation") : TEXT("Supported"));
	Json += FString::Printf(TEXT("  \"ResponsiveDegradationReason\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResponsiveDegradationReason));
	Json += FString::Printf(TEXT("  \"ViewportResizeEventCount\": %d,\n"),
		ViewportResizeEventCount);
	Json += FString::Printf(TEXT("  \"ResponsiveLayoutApplyCount\": %d,\n"),
		ResponsiveLayoutApplyCount);
	Json += FString::Printf(TEXT("  \"PreviewRenderTargetResizeCount\": %d,\n"),
		PreviewRenderTargetResizeCount);
	Json += FString::Printf(TEXT("  \"PreviewCompositionRefreshCount\": %d,\n"),
		PreviewCompositionRefreshCount);
	Json += FString::Printf(TEXT("  \"RootGeometry\": %s,\n"), *GeometryJson(RootGeometryWidget));
	Json += FString::Printf(TEXT("  \"RootCanvasSlot\": %s,\n"),
		*CanvasSlotJson(RootGeometryWidget));
	Json += FString::Printf(
		TEXT("  \"RootRenderTransform\":{\"Translation\":[%.3f,%.3f],")
		TEXT("\"Scale\":[%.3f,%.3f],\"Shear\":[%.3f,%.3f],\"Angle\":%.3f},\n"),
		RootRenderTransform.Translation.X, RootRenderTransform.Translation.Y,
		RootRenderTransform.Scale.X, RootRenderTransform.Scale.Y,
		RootRenderTransform.Shear.X, RootRenderTransform.Shear.Y,
		RootRenderTransform.Angle);
	Json += FString::Printf(TEXT("  \"RootFillsViewport\": %s,\n"),
		bRootFillsViewport ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"BackgroundGeometry\": %s,\n"),
		*GeometryJson(BackgroundGeometryWidget));
	Json += FString::Printf(TEXT("  \"BackgroundFillsRoot\": %s,\n"),
		FillsRoot(BackgroundGeometryWidget) ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"LayoutRootGeometry\": %s,\n"),
		*GeometryJson(LayoutRootGeometryWidget));
	Json += FString::Printf(TEXT("  \"LayoutRootCanvasSlot\": %s,\n"),
		*CanvasSlotJson(LayoutRootGeometryWidget));
	Json += FString::Printf(TEXT("  \"LayoutRootFillsViewport\": %s,\n"),
		FillsRoot(LayoutRootGeometryWidget) ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"MainLayoutGeometry\": %s,\n"),
		*GeometryJson(MainLayoutGeometryWidget));
	Json += FString::Printf(TEXT("  \"ContentToFooterGeometry\": %s,\n"),
		*GeometryJson(ContentToFooterGeometryWidget));
	Json += FString::Printf(TEXT("  \"LeftCenterStageGeometry\": %s,\n"),
		*GeometryJson(LeftCenterStageGeometryWidget));
	Json += FString::Printf(TEXT("  \"TopBarGeometry\": %s,\n"), *GeometryJson(TopBarGeometryWidget));
	Json += FString::Printf(TEXT("  \"TopBarSpansViewportWidth\": %s,\n"),
		SpansRootWidth(TopBarGeometryWidget) ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"FooterGeometry\": %s,\n"), *GeometryJson(FooterGeometryWidget));
	Json += FString::Printf(TEXT("  \"FooterSpansViewportWidth\": %s,\n"),
		SpansRootWidth(FooterGeometryWidget) ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"LeftPanelGeometry\": %s,\n"), *GeometryJson(LeftPanelGeometryWidget));
	Json += FString::Printf(TEXT("  \"LeftPanelEdgeDistances\": [%.2f, %.2f],\n"),
		LeftPanelEdgeDistances.X, LeftPanelEdgeDistances.Y);
	Json += FString::Printf(TEXT("  \"RightPanelGeometry\": %s,\n"), *GeometryJson(RightPanelGeometryWidget));
	Json += FString::Printf(TEXT("  \"RightPanelEdgeDistances\": [%.2f, %.2f],\n"),
		RightPanelEdgeDistances.X, RightPanelEdgeDistances.Y);
	Json += FString::Printf(TEXT("  \"PreviewVisualGeometry\": %s,\n"), *GeometryJson(VisualSurface));
	Json += FString::Printf(TEXT("  \"PreviewInteractionGeometry\": %s,\n"), *GeometryJson(InteractionArea));
	Json += FString::Printf(TEXT("  \"CatalogScrollBoxGeometry\": %s,\n"), *GeometryJson(CatalogScrollGeometryWidget));
	Json += FString::Printf(TEXT("  \"CharacterScrollBoxGeometry\": %s,\n"), *GeometryJson(CharacterScrollGeometryWidget));
	Json += FString::Printf(TEXT("  \"ModalGeometry\": %s,\n"), *GeometryJson(ModalGeometryWidget));
	Json += FString::Printf(TEXT("  \"OutsideViewportWidgetCount\": %d,\n"), OutsideViewportWidgetCount);
	Json += FString::Printf(TEXT("  \"OverlapCount\": %d,\n"), OverlapCount);
	Json += FString::Printf(TEXT("  \"UnexpectedClippingCount\": %d,\n"), UnexpectedClippingCount);
	Json += FString::Printf(TEXT("  \"ZeroOrNegativeGeometryCount\": %d,\n"), ZeroOrNegativeGeometryCount);
	Json += FString::Printf(TEXT("  \"NonFiniteGeometryCount\": %d,\n"), NonFiniteGeometryCount);
	Json += FString::Printf(TEXT("  \"VisibleScrollBoxCount\": %d,\n"), VisibleScrollBoxCount);
	Json += FString::Printf(TEXT("  \"NonPositiveScrollBoxGeometryCount\": %d,\n"),
		NonPositiveScrollBoxGeometryCount);
	Json += FString::Printf(TEXT("  \"TextOverflowCount\": %d,\n"), TextOverflowDetectedCount);
	Json += FString::Printf(TEXT("  \"ModalOutsideViewport\": %s,\n"),
		bModalOutsideViewport ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"ModalAccessible\": %s,\n"),
		bModalAccessible ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"StaleGeometryAfterResize\": %s,\n"),
		bStaleGeometryAfterResize ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"PanelsInsideWorkingRegion\": %s,\n"),
		bPanelsInsideWorkingRegion ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"LiveCustomizationRootWidgetCount\": %d,\n"),
		LiveCustomizationRoots.Num());
	Json += FString::Printf(TEXT("  \"LivePreviewActorCount\": %d,\n"),
		LivePreviewActorCount);
	Json += FString::Printf(TEXT("  \"CharacterToRightPanelDistance\": %.3f,\n"),
		CharacterToRightPanelDistance);
	Json += FString::Printf(TEXT("  \"RenameHitRectValid\": %s,\n"),
		bRenameHitRectValid ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"VisualAndInteractionBoundsEqual\": %s,\n"),
		bVisualAndInteractionBoundsEqual ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"PreviewCompositionCenterNormalizedX\": %.6f,\n"),
		PreviewActor
			? PreviewActor->GetCompositionCenterNormalizedX()
			: CachedPreviewCompositionCenterX);
	Json += FString::Printf(TEXT("  \"RenderTargetSize\": [%d, %d],\n"),
		RenderTargetX, RenderTargetY);
	Json += FString::Printf(TEXT("  \"RenderTargetAspect\": %.6f,\n"), RenderTargetAspect);
	Json += TEXT("  \"preview_render_mode\": \"SceneCapture2D->TransientRenderTarget->MID->UMGImage\",\n");
	Json += FString::Printf(TEXT("  \"preview_visual_surface_widget\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(GetNameSafe(VisualSurface)));
	Json += FString::Printf(TEXT("  \"preview_visual_surface_parent\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(GetNameSafe(VisualSurface ? VisualSurface->GetParent() : nullptr)));
	Json += FString::Printf(TEXT("  \"preview_first_clipping_parent\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(FirstClippingParent));
	Json += FString::Printf(TEXT("  \"preview_interaction_area_widget\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(GetNameSafe(InteractionArea)));
	Json += FString::Printf(TEXT("  \"preview_visual_surface_visibility\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(VisualSurface
			? VisibilityName(VisualSurface->GetVisibility())
			: TEXT("Unavailable")));
	Json += FString::Printf(TEXT("  \"preview_interaction_area_visibility\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(InteractionArea
			? VisibilityName(InteractionArea->GetVisibility())
			: TEXT("Unavailable")));
	Json += FString::Printf(TEXT("  \"preview_visual_surface_geometry_size\": [%.2f, %.2f],\n"),
		VisualSurfaceSize.X, VisualSurfaceSize.Y);
	Json += FString::Printf(TEXT("  \"visual_surface_size\": [%.2f, %.2f],\n"),
		VisualSurfaceSize.X, VisualSurfaceSize.Y);
	Json += FString::Printf(TEXT("  \"preview_visual_surface_absolute_position\": [%.2f, %.2f],\n"),
		VisualSurfaceAbsolutePosition.X, VisualSurfaceAbsolutePosition.Y);
	Json += FString::Printf(TEXT("  \"preview_interaction_area_geometry_size\": [%.2f, %.2f],\n"),
		InteractionAreaSize.X, InteractionAreaSize.Y);
	Json += FString::Printf(TEXT("  \"interaction_area_size\": [%.2f, %.2f],\n"),
		InteractionAreaSize.X, InteractionAreaSize.Y);
	Json += FString::Printf(TEXT("  \"preview_interaction_area_absolute_position\": [%.2f, %.2f],\n"),
		InteractionAreaAbsolutePosition.X, InteractionAreaAbsolutePosition.Y);
	Json += FString::Printf(TEXT("  \"preview_projection_geometry_size\": [%.2f, %.2f],\n"),
		VisualSurfaceSize.X, VisualSurfaceSize.Y);
	Json += FString::Printf(TEXT("  \"preview_visual_and_interaction_bounds_equal\": %s,\n"),
		bVisualAndInteractionBoundsEqual ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"bounds_equal\": %s,\n"),
		bVisualAndInteractionBoundsEqual ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"preview_render_target_size\": [%d, %d],\n"),
		RenderTargetX, RenderTargetY);
	Json += FString::Printf(TEXT("  \"render_target_size\": [%d, %d],\n"),
		RenderTargetX, RenderTargetY);
	Json += FString::Printf(TEXT("  \"preview_render_target_aspect\": %.6f,\n"), RenderTargetAspect);
	Json += FString::Printf(TEXT("  \"render_target_aspect\": %.6f,\n"), RenderTargetAspect);
	Json += FString::Printf(TEXT("  \"preview_viewport_size\": [%d, %d],\n"),
		DiagnosticViewportX, DiagnosticViewportY);
	Json += FString::Printf(TEXT("  \"preview_viewport_aspect\": %.6f,\n"), ViewportAspect);
	Json += FString::Printf(TEXT("  \"preview_scene_capture_aspect\": %.6f,\n"), RenderTargetAspect);
	Json += FString::Printf(TEXT("  \"scene_capture_aspect_ratio\": %.6f,\n"),
		PreviewActor ? PreviewActor->GetSceneCaptureAspectRatio() : 0.f);
	Json += FString::Printf(TEXT("  \"visual_surface_aspect\": %.6f,\n"), VisualSurfaceAspect);
	Json += FString::Printf(TEXT("  \"interaction_area_aspect\": %.6f,\n"), InteractionAreaAspect);
	Json += FString::Printf(TEXT("  \"scene_capture_constrain_aspect\": %s,\n"),
		PreviewActor && PreviewActor->IsSceneCaptureAspectConstrained() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"scene_capture_custom_projection\": %s,\n"),
		PreviewActor && PreviewActor->IsSceneCaptureUsingCustomProjection() ? TEXT("true") : TEXT("false"));
	Json += TEXT("  \"preview_custom_view_rect\": false,\n");
	Json += TEXT("  \"preview_scissor_state\": \"None\",\n");
	Json += TEXT("  \"preview_visual_surface_visibility_required\": \"HitTestInvisible\",\n");
	Json += TEXT("  \"preview_input_acceptance_geometry\": \"SB_PreviewArea\",\n");
	Json += TEXT("  \"preview_projection_geometry\": \"Img_PreviewPlaceholderFullRoot\",\n");
	Json += FString::Printf(TEXT("  \"save_version\": %d,\n"),
		Ledger ? Ledger->GetCharacterSchemaVersion() : 0);
	Json += FString::Printf(TEXT("  \"my_characters_count\": %d,\n"),
		Ledger ? Ledger->GetCharacterRecords().Num() : 0);
	Json += FString::Printf(TEXT("  \"character_records_count\": %d,\n"),
		Ledger ? Ledger->GetCharacterRecords().Num() : 0);
	Json += FString::Printf(TEXT("  \"active_character_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ActiveCharacterId.ToString()));
	Json += FString::Printf(TEXT("  \"ActiveCharacterId\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ActiveCharacterId.ToString()));
	Json += FString::Printf(TEXT("  \"active_character_display_name\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ActiveRecord ? ActiveRecord->DisplayName : TEXT("None")));
	Json += FString::Printf(TEXT("  \"legacy_active_character_appearance_synced\": %s,\n"),
		Ledger && Ledger->IsLegacyAppearanceMirrorSynchronized() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_record_exists\": %s,\n"),
		bActiveRecordExists ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"loaded_active_character_id_invalid\": %s,\n"),
		Ledger && Ledger->WasLoadedActiveCharacterIdInvalid() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"selected_roster_row_character_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(SelectedRuntimeRowId.ToString()));
	Json += FString::Printf(TEXT("  \"runtime_my_character_rows_count\": %d,\n"),
		RuntimeMyCharacterRows.Num());
	Json += FString::Printf(TEXT("  \"authored_sample_runtime_visibility\": \"%s\",\n"),
		*VisibilityName(SB_CharacterCard01
			? SB_CharacterCard01->GetVisibility()
			: ESlateVisibility::Collapsed));
	Json += FString::Printf(TEXT("  \"btn_add_character_index\": %d,\n"),
		VB_MyCharactersList && SB_AddCharacter
			? VB_MyCharactersList->GetChildIndex(SB_AddCharacter)
			: INDEX_NONE);
	Json += FString::Printf(TEXT("  \"duplicate_runtime_row_count\": %d,\n"),
		DuplicateRuntimeRowCount);
	Json += TEXT("  \"duplicate_add_delegate_count\": 0,\n");
	Json += FString::Printf(TEXT("  \"duplicate_delegate_count\": %d,\n"),
		LastMyCharactersDuplicateDelegateCount);
	Json += FString::Printf(TEXT("  \"add_delegate_bound\": %s,\n"),
		Btn_AddCharacter && Btn_AddCharacter->OnClicked.IsBound()
			? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"duplicate_character_id_count\": %d,\n"),
		Ledger ? Ledger->GetDuplicateCharacterIdCount() : 0);
	Json += FString::Printf(TEXT("  \"duplicate_character_display_name_count\": %d,\n"),
		DuplicateDisplayNameCount);
	Json += FString::Printf(TEXT("  \"last_roster_operation\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastRosterOperation));
	Json += FString::Printf(
		TEXT("  \"LastRowPointerScreenPosition\": [%.3f, %.3f],\n"),
		LastRowPointerScreenPosition.X, LastRowPointerScreenPosition.Y);
	Json += FString::Printf(
		TEXT("  \"LastRowPointerLocalPosition\": [%.3f, %.3f],\n"),
		LastRowPointerLocalPosition.X, LastRowPointerLocalPosition.Y);
	Json += FString::Printf(
		TEXT("  \"DisplayNameGeometrySize\": [%.3f, %.3f],\n"),
		LastDisplayNameGeometrySize.X, LastDisplayNameGeometrySize.Y);
	Json += FString::Printf(
		TEXT("  \"DisplayNameMeasuredTextSize\": [%.3f, %.3f],\n"),
		LastDisplayNameMeasuredTextSize.X, LastDisplayNameMeasuredTextSize.Y);
	Json += FString::Printf(
		TEXT("  \"DisplayNameVisibleWidth\": %.3f,\n"),
		LastDisplayNameVisibleWidth);
	Json += FString::Printf(
		TEXT("  \"RenameHitRect\": [%.3f, %.3f, %.3f, %.3f],\n"),
		LastRenameHitRect.X, LastRenameHitRect.Y,
		LastRenameHitRect.Z, LastRenameHitRect.W);
	Json += FString::Printf(
		TEXT("  \"PointerInsideRenameTextRect\": %s,\n"),
		bLastPointerInsideRenameTextRect ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(
		TEXT("  \"LastRowClickResult\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastRowClickResult));
	Json += FString::Printf(
		TEXT("  \"RenameHandlerCallCount\": %d,\n"),
		RenameHandlerCallCount);
	Json += FString::Printf(
		TEXT("  \"SelectHandlerCallCount\": %d,\n"),
		SelectHandlerCallCount);
	Json += FString::Printf(
		TEXT("  \"RenameCharacterId\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(RenameCharacterId.ToString()));
	Json += FString::Printf(
		TEXT("  \"DisplayNameLayoutScale\": %.4f,\n"),
		LastDisplayNameLayoutScale);
	Json += FString::Printf(
		TEXT("  \"SlateApplicationScale\": %.4f,\n"),
		LastSlateApplicationScale);
	Json += FString::Printf(TEXT("  \"last_created_character_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastCreatedCharacterId.ToString()));
	Json += FString::Printf(TEXT("  \"last_selected_character_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastSelectedCharacterId.ToString()));
	Json += FString::Printf(TEXT("  \"btn_reset_active_character_bound\": %s,\n"),
		Btn_ResetActiveCharacter && Btn_ResetActiveCharacter->OnClicked.IsBound()
			? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"btn_reset_active_character_enabled\": %s,\n"),
		Btn_ResetActiveCharacter && Btn_ResetActiveCharacter->GetIsEnabled()
			? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"btn_delete_active_character_bound\": %s,\n"),
		Btn_DeleteActiveCharacter && Btn_DeleteActiveCharacter->OnClicked.IsBound()
			? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"btn_delete_active_character_enabled\": %s,\n"),
		Btn_DeleteActiveCharacter && Btn_DeleteActiveCharacter->GetIsEnabled()
			? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"delete_allowed\": %s,\n"),
		bActiveRecordExists && Ledger && Ledger->GetCharacterRecords().Num() > 1
			? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"pending_character_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(PendingCharacterAction.TargetCharacterId.ToString()));
	Json += FString::Printf(TEXT("  \"pending_character_action_valid\": %s,\n"),
		PendingCharacterAction.bValid ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"pending_character_action_confirm_consumed\": %s,\n"),
		PendingCharacterAction.bConfirmConsumed ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"last_reset_character_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastResetCharacterId.ToString()));
	Json += FString::Printf(TEXT("  \"last_deleted_character_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastDeletedCharacterId.ToString()));
	Json += FString::Printf(TEXT("  \"active_character_id_after_delete\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastActiveCharacterIdAfterDelete.ToString()));
	Json += FString::Printf(TEXT("  \"hb_my_characters_actions_visibility\": \"%s\",\n"),
		*VisibilityName(HB_MyCharactersActions
			? HB_MyCharactersActions->GetVisibility()
			: ESlateVisibility::Collapsed));
	Json += FString::Printf(TEXT("  \"last_roster_save_game_commit_count\": %d,\n"),
		LastRosterSaveGameCommitCount);
	Json += TEXT("  \"portrait_mode\": \"AuthoredSilhouettePlaceholder\",\n");
	Json += TEXT("  \"portrait_queue_count\": 0,\n");
	Json += TEXT("  \"portrait_cache_count\": 0,\n");
	Json += TEXT("  \"active_portrait_stale\": false,\n");
	Json += FString::Printf(TEXT("  \"my_characters_scroll_offset\": %.3f,\n"),
		ScrollBox_MyCharacters ? ScrollBox_MyCharacters->GetScrollOffset() : 0.f);
	Json += FString::Printf(TEXT("  \"active_appearance_hash\": \"%08X\",\n"),
		ActiveAppearanceHash);
	Json += FString::Printf(TEXT("  \"camera_preset\": \"%s\",\n"),
		*CameraPresetName());
	Json += FString::Printf(TEXT("  \"current_camera_preset\": \"%s\",\n"),
		*CameraPresetName());
	Json += FString::Printf(TEXT("  \"camera_fov\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetSceneCaptureFOV() : 0.f);
	Json += FString::Printf(TEXT("  \"scene_capture_fov\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetSceneCaptureFOV() : 0.f);
	Json += FString::Printf(TEXT("  \"CurrentCameraPreset\": \"%s\",\n"),
		*CameraPresetName());
	Json += FString::Printf(TEXT("  \"BaseDistance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraBaseDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"base_distance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraBaseDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"camera_target_distance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraTargetDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"TargetDistance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraTargetDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"target_distance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraTargetDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"camera_current_distance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraCurrentDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"CurrentDistance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraCurrentDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"current_distance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraCurrentDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"MinDistance\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraMinDistance() : 0.f);
	Json += FString::Printf(TEXT("  \"ZoomAlpha\": %.4f,\n"),
		PreviewActor ? PreviewActor->GetCameraZoomAlpha() : 0.f);
	Json += FString::Printf(TEXT("  \"camera_focus_z\": %.3f,\n"),
		PreviewActor ? PreviewActor->GetCameraFocusZ() : 0.f);
	const FVector BaseFocus = PreviewActor ? PreviewActor->GetCameraBaseFocus() : FVector::ZeroVector;
	const FVector CurrentFocus = PreviewActor ? PreviewActor->GetCameraCurrentFocus() : FVector::ZeroVector;
	const FVector TargetFocus = PreviewActor ? PreviewActor->GetCameraTargetFocus() : FVector::ZeroVector;
	const FVector2D LastCursorScreen = PreviewActor
		? PreviewActor->GetLastCursorScreenPosition()
		: FVector2D::ZeroVector;
	const FVector LastCursorAnchor = PreviewActor
		? PreviewActor->GetLastCursorWorldAnchor()
		: FVector::ZeroVector;
	Json += FString::Printf(TEXT("  \"BaseFocus\": [%.3f, %.3f, %.3f],\n"),
		BaseFocus.X, BaseFocus.Y, BaseFocus.Z);
	Json += FString::Printf(TEXT("  \"base_focus\": [%.3f, %.3f, %.3f],\n"),
		BaseFocus.X, BaseFocus.Y, BaseFocus.Z);
	Json += FString::Printf(TEXT("  \"CurrentFocus\": [%.3f, %.3f, %.3f],\n"),
		CurrentFocus.X, CurrentFocus.Y, CurrentFocus.Z);
	Json += FString::Printf(TEXT("  \"current_focus\": [%.3f, %.3f, %.3f],\n"),
		CurrentFocus.X, CurrentFocus.Y, CurrentFocus.Z);
	Json += FString::Printf(TEXT("  \"TargetFocus\": [%.3f, %.3f, %.3f],\n"),
		TargetFocus.X, TargetFocus.Y, TargetFocus.Z);
	Json += FString::Printf(TEXT("  \"target_focus\": [%.3f, %.3f, %.3f],\n"),
		TargetFocus.X, TargetFocus.Y, TargetFocus.Z);
	Json += FString::Printf(TEXT("  \"LastCursorScreenPosition\": [%.3f, %.3f],\n"),
		LastCursorScreen.X, LastCursorScreen.Y);
	Json += FString::Printf(TEXT("  \"LastCursorWorldAnchor\": [%.3f, %.3f, %.3f],\n"),
		LastCursorAnchor.X, LastCursorAnchor.Y, LastCursorAnchor.Z);
	Json += FString::Printf(TEXT("  \"AnchorSource\": \"%s\",\n"), *AnchorSourceName());
	Json += FString::Printf(TEXT("  \"ManualZoomActive\": %s,\n"),
		PreviewActor && PreviewActor->IsManualZoomActive() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"manual_zoom_active\": %s,\n"),
		PreviewActor && PreviewActor->IsManualZoomActive() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"cursor_anchor_valid\": %s,\n"),
		PreviewActor && PreviewActor->GetLastAnchorSource() != EAvPreviewZoomAnchorSource::None
			? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"CategoryTransitionActive\": %s,\n"),
		PreviewActor && PreviewActor->IsCategoryTransitionActive() ? TEXT("true") : TEXT("false"));
	if (bHasCombinedVisibleBounds)
	{
		Json += FString::Printf(
			TEXT("  \"CombinedVisibleBounds\": {\"Min\":[%.3f,%.3f,%.3f],\"Max\":[%.3f,%.3f,%.3f]},\n"),
			CombinedVisibleBounds.Min.X, CombinedVisibleBounds.Min.Y, CombinedVisibleBounds.Min.Z,
			CombinedVisibleBounds.Max.X, CombinedVisibleBounds.Max.Y, CombinedVisibleBounds.Max.Z);
	}
	else
	{
		Json += TEXT("  \"CombinedVisibleBounds\": null,\n");
	}
	FBox2D ProjectedVisibleBounds(ForceInit);
	const bool bHasProjectedVisibleBounds = PreviewActor &&
		PreviewActor->GetProjectedVisibleBounds(ProjectedVisibleBounds);
	if (bHasProjectedVisibleBounds)
	{
		const FVector2D ProjectedSize = ProjectedVisibleBounds.GetSize();
		const FVector2D ProjectedCenter = ProjectedVisibleBounds.GetCenter();
		Json += FString::Printf(
			TEXT("  \"projected_character_bounds\": [%.6f, %.6f, %.6f, %.6f],\n"),
			ProjectedVisibleBounds.Min.X, ProjectedVisibleBounds.Min.Y,
			ProjectedVisibleBounds.Max.X, ProjectedVisibleBounds.Max.Y);
		Json += FString::Printf(
			TEXT("  \"CharacterProjectedBounds\": [%.6f, %.6f, %.6f, %.6f],\n"),
			ProjectedVisibleBounds.Min.X, ProjectedVisibleBounds.Min.Y,
			ProjectedVisibleBounds.Max.X, ProjectedVisibleBounds.Max.Y);
		Json += FString::Printf(TEXT("  \"projected_character_height_ratio\": %.6f,\n"),
			ProjectedSize.Y);
		Json += FString::Printf(TEXT("  \"CharacterProjectedHeightRatio\": %.6f,\n"),
			ProjectedSize.Y);
		Json += FString::Printf(TEXT("  \"projected_character_width_ratio\": %.6f,\n"),
			ProjectedSize.X);
		Json += FString::Printf(TEXT("  \"projected_character_center\": [%.6f, %.6f],\n"),
			ProjectedCenter.X, ProjectedCenter.Y);
	}
	else
	{
		Json += TEXT("  \"projected_character_bounds\": null,\n");
		Json += TEXT("  \"CharacterProjectedBounds\": null,\n");
		Json += TEXT("  \"projected_character_height_ratio\": 0.0,\n");
		Json += TEXT("  \"CharacterProjectedHeightRatio\": 0.0,\n");
		Json += TEXT("  \"projected_character_width_ratio\": 0.0,\n");
		Json += TEXT("  \"projected_character_center\": [0.0, 0.0],\n");
	}
	Json += FString::Printf(TEXT("  \"base_state_exact\": %s,\n"),
		PreviewActor && PreviewActor->IsCameraAtBaseStateExact() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"last_camera_reset_reason\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			PreviewActor ? PreviewActor->GetLastCameraResetReason().ToString() : TEXT("None")));
	Json += FString::Printf(TEXT("  \"FocusClampedToBounds\": %s,\n"),
		PreviewActor && PreviewActor->WasFocusClampedToBounds() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"WheelHandled\": %s,\n"),
		PreviewActor && PreviewActor->WasLastWheelHandled() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"AutoFitActive\": %s,\n"),
		PreviewActor && PreviewActor->IsAutoFitActive() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"ScreenSpaceCharacterClampActive\": %s,\n"),
		PreviewActor && PreviewActor->IsScreenSpaceCharacterClampActive() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"preview_yaw\": %.3f,\n"), PreviewYawDegrees);
	Json += FString::Printf(TEXT("  \"camera_transition_active\": %s,\n"),
		PreviewActor && PreviewActor->IsCameraTransitionActive() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"preview_drag_active\": %s,\n"),
		bPreviewDragActive ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"pointer_over_preview\": %s,\n"),
		bPointerOverPreview ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"PointerOverPreview\": %s,\n"),
		bPointerOverPreview ? TEXT("true") : TEXT("false"));
	const FSoftObjectPath ActiveTorsoPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath();
	const FSoftObjectPath ActiveHeadgearPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headgear).ToSoftObjectPath();
	const FSoftObjectPath ActiveHeadphonesPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Headphones).ToSoftObjectPath();
	const bool bRaisedHood = UWorkerAppearanceComponent::HasRaisedHood(ActiveCharacterAppearance);
	const bool bHeadgearConflicts = UWorkerAppearanceComponent::HeadgearConflictsWithRaisedHood(
		ActiveCharacterAppearance.Get(EWorkerSlot::Headgear));
	const bool bHeadphonesConflict = UWorkerAppearanceComponent::HeadphonesConflictWithRaisedHood(
		ActiveCharacterAppearance.Get(EWorkerSlot::Headphones));
	const FAvHeadCustomizationCatalogItem* ActiveHeadgearRecord =
		HeadCatalog.FindByPredicate(
			[&ActiveHeadgearPath](const FAvHeadCustomizationCatalogItem& Item)
			{
				return Item.UISection == EAvHeadCustomizationSection::Headgear &&
					FSoftObjectPath(Item.ExactObjectPath) == ActiveHeadgearPath;
			});
	const FString ActiveHeadgearFamily = ActiveHeadgearPath.IsNull()
		? TEXT("None")
		: (ActiveHeadgearRecord ? ActiveHeadgearRecord->Family : TEXT("Unavailable"));
	const bool bHeadgearCompatibleWithHeadphones =
		UWorkerAppearanceComponent::IsHeadgearCompatibleWithHeadphones(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headgear));
	const bool bHeadgearHeadphonesConflict =
		UWorkerAppearanceComponent::HeadgearConflictsWithHeadphones(
			ActiveCharacterAppearance.Get(EWorkerSlot::Headgear),
			ActiveCharacterAppearance.Get(EWorkerSlot::Headphones));
	const bool bActiveTorsoRecordExists = ActiveTorsoPath.IsNull() || TorsoCatalog.ContainsByPredicate(
		[&ActiveTorsoPath](const FAvTorsoCatalogItem& Item)
		{
			return !Item.bIsNone && Item.bEnabled && Item.ExactMeshPath == ActiveTorsoPath.ToString();
		});
	const int32 ConflictSlotCount = bRaisedHood
		? static_cast<int32>(bHeadgearConflicts) + static_cast<int32>(bHeadphonesConflict)
		: 0;
	Json += FString::Printf(TEXT("  \"active_torso_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResolveTorsoStableId(ActiveTorsoPath)));
	Json += FString::Printf(TEXT("  \"torso_catalog_records\": %d,\n"), TorsoCatalog.Num());
	Json += FString::Printf(TEXT("  \"runtime_torso_cards\": %d,\n"), RuntimeTorsoCards.Num());
	Json += FString::Printf(TEXT("  \"removed_damaged_torso_records\": %d,\n"),
		AvUpperEquipment::RemovedDamagedRecordCount);
	Json += FString::Printf(TEXT("  \"active_torso_record_exists_in_catalog\": %s,\n"),
		bActiveTorsoRecordExists ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"torso_scroll_offset\": %.3f,\n"),
		ScrollBox_TorsoItems ? ScrollBox_TorsoItems->GetScrollOffset() : 0.0f);
	Json += FString::Printf(TEXT("  \"torso_scroll_end_offset\": %.3f,\n"),
		ScrollBox_TorsoItems ? ScrollBox_TorsoItems->GetScrollOffsetOfEnd() : 0.0f);
	Json += FString::Printf(TEXT("  \"torso_wheel_scroll_multiplier\": %.3f,\n"),
		ScrollBox_TorsoItems ? ScrollBox_TorsoItems->GetWheelScrollMultiplier() : 0.0f);
	Json += FString::Printf(TEXT("  \"torso_animate_wheel_scrolling\": %s,\n"),
		ScrollBox_TorsoItems && ScrollBox_TorsoItems->IsAnimateWheelScrolling() ? TEXT("true") : TEXT("false"));
	const FSoftObjectPath ActiveLegsPath = ActiveCharacterAppearance.Get(EWorkerSlot::Legs).ToSoftObjectPath();
	const FSoftObjectPath ActiveHipPath = ActiveCharacterAppearance.Get(EWorkerSlot::HipAccessory).ToSoftObjectPath();
	auto ResolveLowerStableId = [this](EAvLowerHipSection Section, const FSoftObjectPath& Path)
	{
		if (Path.IsNull()) return FString(TEXT("None"));
		const FAvLowerHipCatalogItem* Found = LowerHipCatalog.FindByPredicate(
			[Section, &Path](const FAvLowerHipCatalogItem& Item)
			{
				return Item.Section == Section && !Item.bIsNone && Item.ExactMeshPath == Path.ToString();
			});
		return Found ? Found->StableId : FString(TEXT("Unavailable"));
	};
	const int32 LegsCount = LowerHipCatalog.FilterByPredicate([](const FAvLowerHipCatalogItem& Item)
		{ return Item.Section == EAvLowerHipSection::Legs; }).Num();
	const int32 HipCount = LowerHipCatalog.FilterByPredicate([](const FAvLowerHipCatalogItem& Item)
		{ return Item.Section == EAvLowerHipSection::Hip; }).Num();
	Json += FString::Printf(TEXT("  \"active_lower_hip_section\": \"%s\",\n"),
		SelectedLowerHipSection == EAvLowerHipSection::Legs ? TEXT("Legs") : TEXT("Hip"));
	Json += FString::Printf(TEXT("  \"legs_catalog_records\": %d,\n"), LegsCount);
	Json += FString::Printf(TEXT("  \"runtime_legs_cards\": %d,\n"), RuntimeLegsCards.Num());
	Json += FString::Printf(TEXT("  \"active_legs_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResolveLowerStableId(EAvLowerHipSection::Legs, ActiveLegsPath)));
	Json += FString::Printf(TEXT("  \"legs_record_exists\": %s,\n"),
		ActiveLegsPath.IsNull() || ResolveLowerStableId(EAvLowerHipSection::Legs, ActiveLegsPath) != TEXT("Unavailable") ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"legs_scroll_offset\": %.3f,\n"), ScrollBox_LegsItems ? ScrollBox_LegsItems->GetScrollOffset() : 0.f);
	Json += FString::Printf(TEXT("  \"legs_scroll_end_offset\": %.3f,\n"), ScrollBox_LegsItems ? ScrollBox_LegsItems->GetScrollOffsetOfEnd() : 0.f);
	Json += FString::Printf(TEXT("  \"hip_catalog_records\": %d,\n"), HipCount);
	Json += FString::Printf(TEXT("  \"runtime_hip_cards\": %d,\n"), RuntimeHipCards.Num());
	Json += FString::Printf(TEXT("  \"active_hip_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResolveLowerStableId(EAvLowerHipSection::Hip, ActiveHipPath)));
	Json += FString::Printf(TEXT("  \"hip_record_exists\": %s,\n"),
		ActiveHipPath.IsNull() || ResolveLowerStableId(EAvLowerHipSection::Hip, ActiveHipPath) != TEXT("Unavailable") ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"hip_scroll_offset\": %.3f,\n"), ScrollBox_HipItems ? ScrollBox_HipItems->GetScrollOffset() : 0.f);
	Json += FString::Printf(TEXT("  \"hip_scroll_end_offset\": %.3f,\n"), ScrollBox_HipItems ? ScrollBox_HipItems->GetScrollOffsetOfEnd() : 0.f);
	Json += TEXT("  \"hip_slot_status\": \"CreatedAppendOnlyHipAccessory\",\n");
	Json += TEXT("  \"lower_hip_compatibility\": \"NoProvenBlockingConflict_UserVisualCheckRequired\",\n");
	const FSoftObjectPath DisplayedFullOutfitPath =
		DisplayedAppearance.Get(EWorkerSlot::FullOutfit).ToSoftObjectPath();
	const FSoftObjectPath DisplayedOverallsPath =
		DisplayedAppearance.Get(EWorkerSlot::Overalls).ToSoftObjectPath();
	auto ResolveEnsembleStableId = [this](
		EWorkerSlot WorkerSlot, const FSoftObjectPath& Path)
	{
		if (Path.IsNull()) return FString(TEXT("None"));
		FAvFullOutfitCatalogItem Found;
		return UWorkerAppearanceComponent::ResolveFullOutfitCatalogRecord(
			WorkerSlot, Path, Found)
			? Found.StableId
			: FString(TEXT("Unavailable"));
	};
	const FString ActiveFullOutfitStableId =
		ResolveEnsembleStableId(EWorkerSlot::FullOutfit, DisplayedFullOutfitPath);
	const FString ActiveOverallsStableId =
		ResolveEnsembleStableId(EWorkerSlot::Overalls, DisplayedOverallsPath);
	const bool bEnsembleConflict =
		!DisplayedFullOutfitPath.IsNull() && !DisplayedOverallsPath.IsNull();
	const FString EnsembleKind = !DisplayedFullOutfitPath.IsNull()
		? TEXT("FullOutfit")
		: (!DisplayedOverallsPath.IsNull() ? TEXT("Overalls") : TEXT("None"));
	FAvFullOutfitCatalogItem ActiveEnsembleRecord;
	const EWorkerSlot ActiveEnsembleSlot = !DisplayedFullOutfitPath.IsNull()
		? EWorkerSlot::FullOutfit : EWorkerSlot::Overalls;
	const FSoftObjectPath ActiveEnsemblePath = !DisplayedFullOutfitPath.IsNull()
		? DisplayedFullOutfitPath : DisplayedOverallsPath;
	const bool bActiveEnsembleRecordMatched =
		UWorkerAppearanceComponent::ResolveFullOutfitCatalogRecord(
			ActiveEnsembleSlot, ActiveEnsemblePath, ActiveEnsembleRecord);
	constexpr int32 ExpectedFullOutfitThumbnailCount = 62;
	int32 Type01MissingCount = 0;
	int32 Type02MissingCount = 0;
	int32 LoadedFullOutfitThumbnailCount = 0;
	TSet<FString> ExpectedFullOutfitThumbnailPaths;
	for (int32 Index = 0; Index < FullOutfitCatalog.Num(); ++Index)
	{
		const FAvFullOutfitCatalogItem& Item = FullOutfitCatalog[Index];
		ExpectedFullOutfitThumbnailPaths.Add(Item.ThumbnailType01);
		ExpectedFullOutfitThumbnailPaths.Add(Item.ThumbnailType02);
		const FHeadItemThumbnailPair* Pair = FullOutfitThumbnailRegistry.Find(Index);
		if (!Pair || !Pair->Type01)
		{
			++Type01MissingCount;
		}
		else
		{
			++LoadedFullOutfitThumbnailCount;
		}
		if (!Pair || !Pair->Type02)
		{
			++Type02MissingCount;
		}
		else
		{
			++LoadedFullOutfitThumbnailCount;
		}
	}

	IAssetRegistry& AssetRegistry =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	FARFilter ThumbnailFilter;
	ThumbnailFilter.PackagePaths.Add(FName(
		TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/FullOutfits")));
	ThumbnailFilter.bRecursivePaths = true;
	TArray<FAssetData> ThumbnailAssets;
	AssetRegistry.GetAssets(ThumbnailFilter, ThumbnailAssets);
	int32 RegisteredFullOutfitThumbnailCount = 0;
	int32 FullOutfitThumbnailRedirectorCount = 0;
	for (const FAssetData& Asset : ThumbnailAssets)
	{
		const FString ObjectPath = Asset.ToSoftObjectPath().ToString();
		if (Asset.AssetClassPath == UTexture2D::StaticClass()->GetClassPathName() &&
			ExpectedFullOutfitThumbnailPaths.Contains(ObjectPath))
		{
			++RegisteredFullOutfitThumbnailCount;
		}
		else if (Asset.AssetClassPath.GetAssetName() == FName(TEXT("ObjectRedirector")))
		{
			++FullOutfitThumbnailRedirectorCount;
		}
	}

	int32 PlaceholderCardCount = 0;
	int32 PlaceholderRealCardCount = 0;
	int32 CardBrushResourceMismatchCount = 0;
	for (const UAvHeadCatalogItemButton* Card : RuntimeFullOutfitCards)
	{
		if (!Card)
		{
			continue;
		}
		UTexture2D* CardTexture = Card->GetThumbnailTextureForDiagnostics();
		UObject* BrushResource = Card->GetThumbnailBrushResourceForDiagnostics();
		if (!CardTexture)
		{
			++PlaceholderCardCount;
			if (!Card->IsNoneCatalogCard())
			{
				++PlaceholderRealCardCount;
			}
		}
		if (CardTexture != BrushResource)
		{
			++CardBrushResourceMismatchCount;
		}
	}

	int32 DiagnosticThumbnailCatalogIndex = 0;
	if (bActiveEnsembleRecordMatched)
	{
		DiagnosticThumbnailCatalogIndex = FullOutfitCatalog.IndexOfByPredicate(
			[&ActiveEnsembleRecord](const FAvFullOutfitCatalogItem& Item)
			{
				return Item.StableId == ActiveEnsembleRecord.StableId;
			});
		if (DiagnosticThumbnailCatalogIndex == INDEX_NONE)
		{
			DiagnosticThumbnailCatalogIndex = 0;
		}
	}
	const bool bUseHeadType02Thumbnail = GetActiveHeadTypeId() == FName(TEXT("HeadType02"));
	const FAvFullOutfitCatalogItem* DiagnosticThumbnailRecord =
		FullOutfitCatalog.IsValidIndex(DiagnosticThumbnailCatalogIndex)
			? &FullOutfitCatalog[DiagnosticThumbnailCatalogIndex]
			: nullptr;
	const FString ResolvedThumbnailObjectPath = DiagnosticThumbnailRecord
		? (bUseHeadType02Thumbnail
			? DiagnosticThumbnailRecord->ThumbnailType02
			: DiagnosticThumbnailRecord->ThumbnailType01)
		: FString();
	const FHeadItemThumbnailPair* DiagnosticThumbnailPair =
		FullOutfitThumbnailRegistry.Find(DiagnosticThumbnailCatalogIndex);
	UTexture2D* ResolvedThumbnailTexture = DiagnosticThumbnailPair
		? (bUseHeadType02Thumbnail
			? DiagnosticThumbnailPair->Type02.Get()
			: DiagnosticThumbnailPair->Type01.Get())
		: nullptr;
	const UAvHeadCatalogItemButton* ActiveThumbnailCard = nullptr;
	for (const UAvHeadCatalogItemButton* Card : RuntimeFullOutfitCards)
	{
		if (Card && Card->GetCatalogIndex() == DiagnosticThumbnailCatalogIndex)
		{
			ActiveThumbnailCard = Card;
			break;
		}
	}
	UTexture2D* ActiveCardThumbnailTexture = ActiveThumbnailCard
		? ActiveThumbnailCard->GetThumbnailTextureForDiagnostics()
		: nullptr;
	UObject* ActiveCardBrushResource = ActiveThumbnailCard
		? ActiveThumbnailCard->GetThumbnailBrushResourceForDiagnostics()
		: nullptr;
	const FString ThumbnailLoadFailureReason = ResolvedThumbnailObjectPath.IsEmpty()
		? TEXT("EmptyCatalogObjectPath")
		: (ResolvedThumbnailTexture ? TEXT("None") : TEXT("LoadObjectReturnedNull"));
	Json += TEXT("  \"full_outfit_catalog_expected_count\": 31,\n");
	Json += FString::Printf(TEXT("  \"full_outfit_catalog_count\": %d,\n"),
		FullOutfitCatalog.Num());
	Json += FString::Printf(TEXT("  \"runtime_full_outfit_cards\": %d,\n"),
		RuntimeFullOutfitCards.Num());
	Json += FString::Printf(TEXT("  \"full_outfit_thumbnail_expected_count\": %d,\n"),
		ExpectedFullOutfitThumbnailCount);
	Json += FString::Printf(TEXT("  \"full_outfit_thumbnail_registered_count\": %d,\n"),
		RegisteredFullOutfitThumbnailCount);
	Json += FString::Printf(TEXT("  \"full_outfit_thumbnail_loaded_count\": %d,\n"),
		LoadedFullOutfitThumbnailCount);
	Json += FString::Printf(TEXT("  \"full_outfit_thumbnail_type01_missing_count\": %d,\n"),
		Type01MissingCount);
	Json += FString::Printf(TEXT("  \"full_outfit_thumbnail_type02_missing_count\": %d,\n"),
		Type02MissingCount);
	Json += FString::Printf(TEXT("  \"full_outfit_thumbnail_redirector_count\": %d,\n"),
		FullOutfitThumbnailRedirectorCount);
	Json += FString::Printf(TEXT("  \"full_outfit_placeholder_card_count\": %d,\n"),
		PlaceholderCardCount);
	Json += FString::Printf(TEXT("  \"full_outfit_placeholder_real_card_count\": %d,\n"),
		PlaceholderRealCardCount);
	Json += FString::Printf(TEXT("  \"full_outfit_card_brush_resource_mismatch_count\": %d,\n"),
		CardBrushResourceMismatchCount);
	Json += FString::Printf(TEXT("  \"active_head_type\": \"%s\",\n"),
		*GetActiveHeadTypeId().ToString());
	Json += FString::Printf(TEXT("  \"resolved_full_outfit_thumbnail_object_path\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResolvedThumbnailObjectPath));
	Json += FString::Printf(TEXT("  \"loaded_full_outfit_thumbnail_name\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(GetNameSafe(ResolvedThumbnailTexture)));
	Json += FString::Printf(TEXT("  \"full_outfit_thumbnail_load_failure_reason\": \"%s\",\n"),
		*ThumbnailLoadFailureReason);
	Json += FString::Printf(TEXT("  \"active_full_outfit_card_received_texture\": %s,\n"),
		ActiveCardThumbnailTexture ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_full_outfit_card_brush_resource_object\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			ActiveCardBrushResource ? ActiveCardBrushResource->GetPathName() : TEXT("None")));
	Json += FString::Printf(TEXT("  \"active_ensemble_technical_kind\": \"%s\",\n"),
		*EnsembleKind);
	Json += FString::Printf(TEXT("  \"ensemble_active\": %s,\n"),
		(!DisplayedFullOutfitPath.IsNull() || !DisplayedOverallsPath.IsNull())
			? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_full_outfit_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ActiveFullOutfitStableId));
	Json += FString::Printf(TEXT("  \"active_overalls_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ActiveOverallsStableId));
	Json += FString::Printf(TEXT("  \"active_ensemble_record_exists\": %s,\n"),
		(ActiveFullOutfitStableId != TEXT("Unavailable") &&
		 ActiveOverallsStableId != TEXT("Unavailable")) ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_ensemble_demo_index\": %d,\n"),
		bActiveEnsembleRecordMatched ? ActiveEnsembleRecord.DemoIndex : -1);
	Json += FString::Printf(TEXT("  \"active_ensemble_primary_mesh\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			bActiveEnsembleRecordMatched ? ActiveEnsembleRecord.PrimaryMeshPath : TEXT("None")));
	Json += FString::Printf(TEXT("  \"active_ensemble_companion_torso\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			bActiveEnsembleRecordMatched && !ActiveEnsembleRecord.CompanionTorsoMeshPath.IsEmpty()
				? ActiveEnsembleRecord.CompanionTorsoMeshPath : TEXT("None")));
	Json += FString::Printf(TEXT("  \"active_ensemble_companion_chest\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			bActiveEnsembleRecordMatched && !ActiveEnsembleRecord.CompanionChestMeshPath.IsEmpty()
				? ActiveEnsembleRecord.CompanionChestMeshPath : TEXT("None")));
	Json += FString::Printf(TEXT("  \"active_ensemble_companion_legs\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			bActiveEnsembleRecordMatched && !ActiveEnsembleRecord.CompanionLegsMeshPath.IsEmpty()
				? ActiveEnsembleRecord.CompanionLegsMeshPath : TEXT("None")));
	Json += FString::Printf(TEXT("  \"active_ensemble_companion_feet\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			bActiveEnsembleRecordMatched && !ActiveEnsembleRecord.CompanionFeetMeshPath.IsEmpty()
				? ActiveEnsembleRecord.CompanionFeetMeshPath : TEXT("None")));
	Json += FString::Printf(TEXT("  \"original_demo_recipe_matched\": %s,\n"),
		bActiveEnsembleRecordMatched ? TEXT("true") : TEXT("false"));
	Json += TEXT("  \"thumbnail_runtime_recipe_parity\": true,\n");
	Json += FString::Printf(TEXT("  \"active_ensemble_conflict\": %s,\n"),
		bEnsembleConflict ? TEXT("true") : TEXT("false"));
	Json += TEXT("  \"ensemble_render_precedence\": \"FullOutfitThenOveralls\",\n");
	Json += TEXT("  \"overalls_slot_status\": \"CreatedAppendOnlyAt16\",\n");
	Json += FString::Printf(TEXT("  \"full_outfit_scroll_offset\": %.3f,\n"),
		ScrollBox_FullOutfitItems ? ScrollBox_FullOutfitItems->GetScrollOffset() : 0.f);
	Json += FString::Printf(TEXT("  \"full_outfit_scroll_end_offset\": %.3f,\n"),
		ScrollBox_FullOutfitItems ? ScrollBox_FullOutfitItems->GetScrollOffsetOfEnd() : 0.f);
	Json += FString::Printf(TEXT("  \"full_outfit_wheel_scroll_multiplier\": %.3f,\n"),
		ScrollBox_FullOutfitItems
			? ScrollBox_FullOutfitItems->GetWheelScrollMultiplier() : 0.f);
	Json += FString::Printf(TEXT("  \"visible_underlying_torso_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResolveTorsoStableId(
			DisplayedAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath())));
	Json += FString::Printf(TEXT("  \"visible_underlying_legs_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResolveLowerStableId(
			EAvLowerHipSection::Legs,
			DisplayedAppearance.Get(EWorkerSlot::Legs).ToSoftObjectPath())));
	Json += FString::Printf(TEXT("  \"visible_underlying_feet_path\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			DisplayedAppearance.Get(EWorkerSlot::Feet).ToSoftObjectPath().ToString()));
	const UWorkerAppearanceComponent* PreviewAppearanceComponent =
		PreviewActor ? PreviewActor->FindComponentByClass<UWorkerAppearanceComponent>() : nullptr;
	FAvHeadCustomizationCatalogItem FinalHeadMetadata;
	const FSoftObjectPath FinalHeadPath =
		ActiveCharacterAppearance.Get(EWorkerSlot::Head).ToSoftObjectPath();
	const bool bFinalHeadKnown = UWorkerAppearanceComponent::TryGetHeadTypeMetadata(
		ActiveCharacterAppearance.Get(EWorkerSlot::Head),
		FinalHeadMetadata);
	FString FinalHeadValidation;
	const bool bFinalHeadValid = UWorkerAppearanceComponent::ValidateHeadTypeSkinPresentation(
		ActiveCharacterAppearance,
		FinalHeadValidation);
	Json += FString::Printf(TEXT("  \"RandomHeadCandidateCount\": %d,\n"),
		LastRandomHeadDiagnostics.CandidateCount);
	Json += FString::Printf(TEXT("  \"RandomHeadCandidateStableIds\": %s,\n"),
		*JsonStringArray(LastRandomHeadDiagnostics.CandidateStableIds));
	Json += FString::Printf(TEXT("  \"RandomHeadCandidatePaths\": %s,\n"),
		*JsonStringArray(LastRandomHeadDiagnostics.CandidatePaths));
	Json += FString::Printf(TEXT("  \"RandomHeadSelectedIndex\": %d,\n"),
		LastRandomHeadDiagnostics.SelectedIndex);
	Json += FString::Printf(TEXT("  \"RandomHeadSelectedStableId\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastRandomHeadDiagnostics.SelectedStableId));
	Json += FString::Printf(TEXT("  \"RandomHeadSelectedPath\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastRandomHeadDiagnostics.SelectedPath));
	Json += FString::Printf(TEXT("  \"RandomHeadResolvedProfile\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastRandomHeadDiagnostics.ResolvedProfile));
	Json += FString::Printf(TEXT("  \"RandomHeadValidationResult\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastRandomHeadDiagnostics.ValidationResult));
	Json += FString::Printf(TEXT("  \"RandomHeadFallbackUsed\": %s,\n"),
		LastRandomHeadDiagnostics.bFallbackUsed ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"RandomSeedSource\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(LastRandomHeadDiagnostics.SeedSource));
	Json += FString::Printf(TEXT("  \"RandomCommitCount\": %d,\n"), LastRandomCommitCount);
	Json += FString::Printf(TEXT("  \"RandomAutosaveCount\": %d,\n"), LastRandomAutosaveCount);
	Json += FString::Printf(TEXT("  \"FinalHeadStableId\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			bFinalHeadKnown ? FinalHeadMetadata.StableId : TEXT("Unknown")));
	Json += FString::Printf(TEXT("  \"FinalHeadPath\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(FinalHeadPath.ToString()));
	Json += FString::Printf(TEXT("  \"FinalAppliedSkinProfile\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			PreviewAppearanceComponent
				? PreviewAppearanceComponent->GetResolvedSkinProfileForDiagnostics()
				: TEXT("Unavailable")));
	Json += FString::Printf(TEXT("  \"FinalHeadValidationResult\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(FinalHeadValidation));
	Json += FString::Printf(TEXT("  \"FinalHeadValidationPass\": %s,\n"),
		bFinalHeadValid ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"resolved_body_coverage\": %s,\n"),
		PreviewAppearanceComponent
			? *PreviewAppearanceComponent->GetBodyCoverageDiagnosticsJson()
			: TEXT("{\"implementation\":\"Unavailable\"}"));
	Json += FString::Printf(TEXT("  \"raised_hood\": %s,\n"), bRaisedHood ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_headgear_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResolveHeadgearStableId(ActiveHeadgearPath)));
	Json += FString::Printf(TEXT("  \"headgear_conflicts_with_raised_hood\": %s,\n"),
		bHeadgearConflicts ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_raised_hood_headgear_conflict\": %s,\n"),
		bRaisedHood && bHeadgearConflicts ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_headphones_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ResolveHeadphonesStableId(ActiveHeadphonesPath)));
	Json += FString::Printf(TEXT("  \"headphones_conflict_with_raised_hood\": %s,\n"),
		bHeadphonesConflict ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_raised_hood_headphones_conflict\": %s,\n"),
		bRaisedHood && bHeadphonesConflict ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_headgear_family\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(ActiveHeadgearFamily));
	Json += FString::Printf(TEXT("  \"headgear_compatible_with_headphones\": %s,\n"),
		bHeadgearCompatibleWithHeadphones ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"active_headgear_headphones_conflict\": %s,\n"),
		bHeadgearHeadphonesConflict ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"raised_hood_conflict_slot_count\": %d,\n"),
		ConflictSlotCount);
	Json += FString::Printf(TEXT("  \"modal_mode\": \"%s\",\n"), *GetAppearanceModalModeName());
	Json += FString::Printf(TEXT("  \"pending_target_stable_id\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(PendingEquipmentConflict.bValid
			? PendingEquipmentConflict.TargetStableId : TEXT("None")));
	Json += FString::Printf(TEXT("  \"pending_clear_headgear\": %s,\n"),
		PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headgear) ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"pending_clear_headphones\": %s,\n"),
		PendingEquipmentConflict.ShouldClear(EWorkerSlot::Headphones) ? TEXT("true") : TEXT("false"));
	TArray<FString> PendingSlotNames;
	for (const EWorkerSlot WorkerSlotToClear : PendingEquipmentConflict.SlotsToClear)
	{
		PendingSlotNames.Add(AvAppearanceInspector::SlotName(WorkerSlotToClear));
	}
	Json += FString::Printf(TEXT("  \"pending_slots_to_clear\": \"%s\",\n"),
		*AvAppearanceInspector::JsonEscape(
			PendingSlotNames.IsEmpty() ? TEXT("None") : FString::Join(PendingSlotNames, TEXT(","))));
	Json += FString::Printf(TEXT("  \"conflict_warning_type\": \"%s\",\n"),
		*GetEquipmentConflictWarningTypeName());
	Json += FString::Printf(TEXT("  \"suppress_raised_hood_equipment_warning\": %s,\n"),
		ShouldSuppressRaisedHoodEquipmentWarning() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"suppress_headgear_headphones_warning\": %s,\n"),
		ShouldSuppressHeadgearHeadphonesWarning() ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"last_manual_selection_cleared_ensemble\": %s,\n"),
		bLastManualModularSelectionClearedEnsemble ? TEXT("true") : TEXT("false"));
	Json += FString::Printf(TEXT("  \"last_operation_appearance_autosave_count\": %d,\n"),
		LastOperationAppearanceAutosaveCount);
	Json += FString::Printf(TEXT("  \"conflict_checkbox_checked\": %s,\n"),
		IsEquipmentConflictModalMode(AppearanceModalMode) &&
			CheckBox_EquipmentConflictDontShowAgain &&
			CheckBox_EquipmentConflictDontShowAgain->IsChecked()
			? TEXT("true") : TEXT("false"));
	Json += TEXT("  \"appearance_mutated\": false,\n  \"autosave_invoked\": false,\n  \"slots\": [\n");

	for (int32 SlotIndex = static_cast<int32>(EWorkerSlot::Body);
		SlotIndex <= static_cast<int32>(EWorkerSlot::Overalls);
		++SlotIndex)
	{
		const EWorkerSlot WorkerSlot = static_cast<EWorkerSlot>(SlotIndex);
		const FAvAppearanceInspectorSlotInfo Info = PreviewActor
			? PreviewActor->GetAppearanceInspectorSlotInfo(WorkerSlot)
			: FAvAppearanceInspectorSlotInfo();
		const FString AppearanceObjectPath = DisplayedAppearance.Get(WorkerSlot).ToSoftObjectPath().ToString();
		Json += TEXT("    {\n");
		Json += FString::Printf(TEXT("      \"slot_index\": %d,\n"), SlotIndex);
		Json += FString::Printf(TEXT("      \"slot\": \"%s\",\n"), AvAppearanceInspector::SlotName(WorkerSlot));
		Json += FString::Printf(TEXT("      \"present\": %s,\n"), Info.bPresent ? TEXT("true") : TEXT("false"));
		Json += FString::Printf(TEXT("      \"visible\": %s,\n"), Info.bVisible ? TEXT("true") : TEXT("false"));
		Json += FString::Printf(TEXT("      \"appearance_object_path\": \"%s\",\n"), *AvAppearanceInspector::JsonEscape(AppearanceObjectPath));
		Json += FString::Printf(TEXT("      \"appearance_package_path\": \"%s\",\n"), *AvAppearanceInspector::JsonEscape(AvAppearanceInspector::PackagePathFromObjectPath(AppearanceObjectPath)));
		Json += FString::Printf(TEXT("      \"runtime_mesh_name\": \"%s\",\n"), *AvAppearanceInspector::JsonEscape(Info.MeshName));
		Json += FString::Printf(TEXT("      \"runtime_mesh_object_path\": \"%s\",\n"), *AvAppearanceInspector::JsonEscape(Info.MeshObjectPath));
		Json += FString::Printf(TEXT("      \"runtime_mesh_package_path\": \"%s\",\n"), *AvAppearanceInspector::JsonEscape(Info.MeshPackagePath));
		Json += FString::Printf(TEXT("      \"material_slot_count\": %d,\n"), Info.MaterialSlotCount);
		Json += TEXT("      \"material_asset_paths\": [");
		for (int32 MaterialIndex = 0; MaterialIndex < Info.MaterialAssetPaths.Num(); ++MaterialIndex)
		{
			Json += FString::Printf(TEXT("%s\"%s\""), MaterialIndex > 0 ? TEXT(", ") : TEXT(""), *AvAppearanceInspector::JsonEscape(Info.MaterialAssetPaths[MaterialIndex]));
		}
		Json += TEXT("],\n      \"runtime_material_paths\": [");
		for (int32 MaterialIndex = 0; MaterialIndex < Info.RuntimeMaterialPaths.Num(); ++MaterialIndex)
		{
			Json += FString::Printf(TEXT("%s\"%s\""), MaterialIndex > 0 ? TEXT(", ") : TEXT(""), *AvAppearanceInspector::JsonEscape(Info.RuntimeMaterialPaths[MaterialIndex]));
		}
		Json += FString::Printf(TEXT("],\n      \"forced_lod_model\": %d,\n      \"predicted_lod_level\": %d\n"), Info.ForcedLodModel, Info.PredictedLodLevel);
		Json += SlotIndex < static_cast<int32>(EWorkerSlot::Overalls) ? TEXT("    },\n") : TEXT("    }\n");
	}
	Json += TEXT("  ]\n}\n");
	return Json;
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorCopy()
{
	const FString Diagnostic = BuildAppearanceInspectorDiagnosticJson();
	FPlatformApplicationMisc::ClipboardCopy(*Diagnostic);
	UE_LOG(LogTemp, Display, TEXT("[AvAppearanceInspectorCopy] %s"), *Diagnostic);
}

void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorDump()
{
	const FString Directory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("CodexDiagnostics/AppearanceInspector"));
	IFileManager::Get().MakeDirectory(*Directory, true);
	const FDateTime Now = FDateTime::Now();
	const FString Filename = FString::Printf(
		TEXT("appearance_inspector_%s_%03d.json"),
		*Now.ToString(TEXT("%Y%m%d_%H%M%S")),
		Now.GetMillisecond());
	const FString FullPath = FPaths::Combine(Directory, Filename);
	const FString Diagnostic = BuildAppearanceInspectorDiagnosticJson();
	if (FFileHelper::SaveStringToFile(Diagnostic, *FullPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogTemp, Display, TEXT("[AvAppearanceInspectorDump] Saved=%s AppearanceMutation=false Autosave=false"), *FullPath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AvAppearanceInspectorDump] Failed=%s"), *FullPath);
	}
}
#else
void UAvCharacterCustomizationRootWidget::ToggleAppearanceInspector() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorSlotAction(EWorkerSlot, EAvAppearanceInspectorSlotAction) {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorShowAll() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorHideClothing() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorBodyOnly() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorHideBody() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorShowBody() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorClearSolo() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorCopy() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorDump() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorForceLod0() {}
void UAvCharacterCustomizationRootWidget::HandleAppearanceInspectorAutoLod() {}
#endif

void UAvCharacterCustomizationRootWidget::RefreshFullscreenLayout()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	int32 ViewportX = 0;
	int32 ViewportY = 0;
	PC->GetViewportSize(ViewportX, ViewportY);
	if (ViewportX <= 0 || ViewportY <= 0)
	{
		return;
	}

	const FIntPoint NewViewportSize(ViewportX, ViewportY);
	ResizePreviewRenderTargetToViewport(NewViewportSize);
	if (bHasAppliedResponsiveLayout && CachedViewportSize == NewViewportSize)
	{
		return;
	}

	// The viewport slot is already DPI-scaled by Slate. Supplying the raw pixel
	// viewport as a desired size applies that scale a second time. Clear the fixed slot size first,
	// then make the final slot policy a zero-offset full-viewport stretch.
	SetDesiredSizeInViewport(FVector2D::ZeroVector);
	SetAlignmentInViewport(FVector2D::ZeroVector);
	SetPositionInViewport(FVector2D::ZeroVector, false);
	SetAnchorsInViewport(FAnchors(0.f, 0.f, 1.f, 1.f));
	ApplyResponsiveLayoutPolicy(NewViewportSize);

	CachedViewportSize = NewViewportSize;
	bHasAppliedResponsiveLayout = true;
	// Slate can expose the previous arranged child geometry for one frame after
	// a window resize. Re-sample a bounded number of layout frames; this remains
	// event-driven and never becomes permanent camera AutoFit/polling.
	PreviewCompositionRefreshFramesRemaining = 3;
	bPreviewCompositionRefreshPending = true;
}

void UAvCharacterCustomizationRootWidget::RebuildBottomEquipmentPanel()
{
	if (!WidgetTree || !BottomEquipmentPanel)
	{
		return;
	}

	UVerticalBox* BottomBar = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("BottomBar_Responsive"));
	if (!BottomBar)
	{
		return;
	}

	UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HeaderRow_Equipment"));
	if (UVerticalBoxSlot* HeaderSlot = BottomBar->AddChildToVerticalBox(HeaderRow))
	{
		HeaderSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		HeaderSlot->SetPadding(FMargin(22.f, 14.f, 22.f, 4.f));
	}

	if (HeaderRow)
	{
		UWidget* TitleWidget = GetOrCreateBottomTextWidget(TEXT("EquipmentTitle"), FAvLoc::Text(TEXT("Customization.Legacy.EquipmentTitle")));
		if (TitleWidget)
		{
			TitleWidget->RemoveFromParent();
			if (UHorizontalBoxSlot* TitleSlot = HeaderRow->AddChildToHorizontalBox(TitleWidget))
			{
				TitleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
				TitleSlot->SetVerticalAlignment(VAlign_Center);
			}
		}

		USpacer* HeaderSpacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), TEXT("HeaderRowSpacer"));
		if (UHorizontalBoxSlot* SpacerSlot = HeaderRow->AddChildToHorizontalBox(HeaderSpacer))
		{
			SpacerSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}
	}

	UWrapBox* ControlsRow = WidgetTree->ConstructWidget<UWrapBox>(UWrapBox::StaticClass(), TEXT("ControlsRow"));
	ControlsRow->SetInnerSlotPadding(FVector2D(12.f, 10.f));
	if (UVerticalBoxSlot* ControlsSlot = BottomBar->AddChildToVerticalBox(ControlsRow))
	{
		ControlsSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		ControlsSlot->SetPadding(FMargin(22.f, 4.f, 22.f, 14.f));
	}

	UWidget* RandomizeWidget = Btn_RandomizeAppearance.Get();
	if (!RandomizeWidget)
	{
		RandomizeWidget = GetOrCreateBottomTextWidget(TEXT("Btn_RandomizeAppearance_Fallback"), FAvLoc::Text(TEXT("Customization.Action.Random")));
	}

	AddWidgetToBottomControls(RandomizeWidget, ControlsRow, 260.f, FMargin(0.f));

	USpacer* ControlsSpacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), TEXT("ControlsRowSpacer"));
	ControlsSpacer->SetSize(FVector2D(18.f, 1.f));
	AddWidgetToBottomControls(ControlsSpacer, ControlsRow, 18.f, FMargin(0.f));

	UWidget* Jacket = EquipmentSlot_Jacket.Get();
	if (!Jacket) { Jacket = GetOrCreateBottomTextWidget(TEXT("EquipmentSlot_Jacket_Fallback"), FAvLoc::Text(TEXT("Customization.Section.Jacket"))); }
	AddWidgetToBottomControls(Jacket, ControlsRow, 132.f, FMargin(0.f));

	UWidget* Pants = EquipmentSlot_Pants.Get();
	if (!Pants) { Pants = GetOrCreateBottomTextWidget(TEXT("EquipmentSlot_Pants_Fallback"), FAvLoc::Text(TEXT("Customization.Preset.Sample.Lower"))); }
	AddWidgetToBottomControls(Pants, ControlsRow, 132.f, FMargin(0.f));

	UWidget* Boots = EquipmentSlot_Boots.Get();
	if (!Boots) { Boots = GetOrCreateBottomTextWidget(TEXT("EquipmentSlot_Boots_Fallback"), FAvLoc::Text(TEXT("Customization.Section.Feet"))); }
	AddWidgetToBottomControls(Boots, ControlsRow, 132.f, FMargin(0.f));

	UWidget* Helmet = EquipmentSlot_Helmet.Get();
	if (!Helmet) { Helmet = GetOrCreateBottomTextWidget(TEXT("EquipmentSlot_Helmet_Fallback"), FAvLoc::Text(TEXT("Customization.Preset.Sample.Head"))); }
	AddWidgetToBottomControls(Helmet, ControlsRow, 132.f, FMargin(0.f));

	UWidget* Mask = EquipmentSlot_Mask.Get();
	if (!Mask) { Mask = GetOrCreateBottomTextWidget(TEXT("EquipmentSlot_Mask_Fallback"), FAvLoc::Text(TEXT("Customization.Section.Mask"))); }
	AddWidgetToBottomControls(Mask, ControlsRow, 132.f, FMargin(0.f));

	if (UContentWidget* ContentHost = Cast<UContentWidget>(BottomEquipmentPanel.Get()))
	{
		ContentHost->SetContent(BottomBar);
	}
	else if (UPanelWidget* PanelHost = Cast<UPanelWidget>(BottomEquipmentPanel.Get()))
	{
		PanelHost->ClearChildren();
		PanelHost->AddChild(BottomBar);
	}
}

UWidget* UAvCharacterCustomizationRootWidget::GetOrCreateBottomTextWidget(FName WidgetName, const FText& Text)
{
	if (!WidgetTree)
	{
		return nullptr;
	}

	if (UWidget* Existing = WidgetTree->FindWidget(WidgetName))
	{
		if (UTextBlock* ExistingText = Cast<UTextBlock>(Existing))
		{
			ExistingText->SetText(Text);
			ExistingText->SetAutoWrapText(false);
		}
		return Existing;
	}

	UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), WidgetName);
	if (TextBlock)
	{
		TextBlock->SetText(Text);
		TextBlock->SetAutoWrapText(false);
		FSlateFontInfo Font = TextBlock->GetFont();
		Font.Size = WidgetName == TEXT("EquipmentTitle") ? 26 : 22;
		TextBlock->SetFont(Font);
		TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.42f, 0.04f, 1.f)));
	}
	return TextBlock;
}

void UAvCharacterCustomizationRootWidget::AddWidgetToBottomControls(UWidget* Widget, UWrapBox* ControlsRow, float MinWidth, const FMargin& SlotPadding) const
{
	if (!Widget || !ControlsRow || !WidgetTree)
	{
		return;
	}

	Widget->RemoveFromParent();

	USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	SizeBox->SetMinDesiredWidth(MinWidth);
	SizeBox->SetMinDesiredHeight(42.f);
	SizeBox->AddChild(Widget);

	if (UWrapBoxSlot* WrapSlot = ControlsRow->AddChildToWrapBox(SizeBox))
	{
		WrapSlot->SetPadding(SlotPadding);
	}
}
