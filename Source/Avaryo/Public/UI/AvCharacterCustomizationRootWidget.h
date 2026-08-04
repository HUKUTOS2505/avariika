#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WorkerAppearanceComponent.h"
#include "Game/AvariikaSaveGame.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "Types/SlateEnums.h"
#include "AvCharacterCustomizationRootWidget.generated.h"

class UButton;
class UBorder;
class UCanvasPanel;
class UCheckBox;
class UEditableTextBox;
class UHorizontalBox;
class UImage;
class UMaterialInstanceDynamic;
class UOverlay;
class UScrollBox;
class USizeBox;
class UTextBlock;
class UTexture2D;
class UTextureRenderTarget2D;
class UUniformGridPanel;
class UVerticalBox;
class UWidget;
class UWidgetSwitcher;
class UWrapBox;
class AAvCustomizePreviewActor;
class UAvAppearanceInspectorSlotButton;
class UAvHeadCatalogItemButton;
class UAvMyCharacterRowWidget;
class FViewport;
class SWidget;
enum class EAvAppearanceInspectorSlotAction : uint8;

UENUM()
enum class EAvCustomizationCategory : uint8
{
	MyCharacters,
	Presets,
	Head,
	FaceProtection,
	HandsAccessories,
	UpperGear,
	LowerHip,
	FullOutfits
};

UENUM()
enum class EAvAppearanceModalMode : uint8
{
	None,
	RandomAppearance,
	ResetToBase,
	DeleteCharacter,
	EquipTorsoAndClearConflictingEquipment,
	EquipHeadgearAndClearTorso,
	EquipHeadphonesAndClearTorso
};

enum class EAvEquipmentConflictWarningType : uint8
{
	None,
	RaisedHood,
	HeadgearHeadphones,
	Combined
};

struct FAvPendingEquipmentConflict
{
	bool bValid = false;
	EAvAppearanceModalMode Mode = EAvAppearanceModalMode::None;
	FString TargetStableId;
	EWorkerSlot TargetSlot = EWorkerSlot::Torso;
	FSoftObjectPath TargetMeshPath;
	TArray<FString> TargetMaterialOverrides;
	FString ExpectedHeadgearStableId;
	FString ExpectedHeadphonesStableId;
	FString ExpectedTorsoStableId;
	FString ExpectedFullOutfitStableId;
	FString ExpectedOverallsStableId;
	TArray<EWorkerSlot> SlotsToClear;
	EAvEquipmentConflictWarningType WarningType = EAvEquipmentConflictWarningType::None;
	FString TargetDisplayName;

	bool ShouldClear(EWorkerSlot Slot) const { return SlotsToClear.Contains(Slot); }
	void Reset() { *this = FAvPendingEquipmentConflict(); }
};

enum class EAvMyCharactersActionVisualState : uint8
{
	Normal,
	Hovered,
	Pressed,
	Disabled
};

struct FAvPendingCharacterAction
{
	EAvAppearanceModalMode Mode = EAvAppearanceModalMode::None;
	FName TargetCharacterId;
	bool bValid = false;
	bool bConfirmConsumed = false;

	void Reset() { *this = FAvPendingCharacterAction(); }
};

USTRUCT()
struct FHeadItemThumbnailPair
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> Type01 = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> Type02 = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class AVARYO_API UAvCharacterCustomizationRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UAvCharacterCustomizationRootWidget(const FObjectInitializer& ObjectInitializer);

	void RefreshFullscreenLayout();

	UFUNCTION()
	void TriggerRandomAppearance();

	/** Development-only F9 overlay. Shipping implementation is an inert no-op. */
	void ToggleAppearanceInspector();
	UFUNCTION(BlueprintCallable, Category="AvCustomize|Diagnostics")
	bool RunManualSelectionAndEquipmentCompatibilitySelfTest(FString& OutReport);
	void HandleAppearanceInspectorSlotAction(EWorkerSlot WorkerSlot, EAvAppearanceInspectorSlotAction Action);
	void HandleHeadCatalogItemClicked(
		EAvHeadCustomizationSection Section,
		int32 CatalogIndex,
		bool bNoneCard);
	void HandleFaceProtectionCatalogItemClicked(
		EAvFaceProtectionSection Section,
		int32 CatalogIndex,
		bool bNoneCard);
	void HandleHandsAccessoriesCatalogItemClicked(
		EAvHandsAccessoriesSection Section,
		int32 CatalogIndex,
		bool bNoneCard);
	void HandleTorsoCatalogItemClicked(int32 CatalogIndex, bool bNoneCard);
	void HandleLowerHipCatalogItemClicked(
		EAvLowerHipSection Section, int32 CatalogIndex, bool bNoneCard);
	void HandleFullOutfitCatalogItemClicked(int32 CatalogIndex, bool bNoneCard);
	void HandleMyCharacterRowClicked(FName CharacterId);
	void HandleMyCharacterRenameStarted(UAvMyCharacterRowWidget* SourceRow);
	bool HandleMyCharacterRenameCommitted(FName CharacterId, const FString& NewDisplayName);
	void RecordMyCharacterRowPointerDiagnostic(
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
		float SlateApplicationScale);

	UFUNCTION(BlueprintCallable, Category="AvCustomize|MyCharacters|Diagnostics")
	FName TriggerAddCharacterForAutomation();

	UFUNCTION(BlueprintCallable, Category="AvCustomize|MyCharacters|Diagnostics")
	bool TriggerMyCharacterSelectionForAutomation(FName CharacterId);

	UFUNCTION(BlueprintPure, Category="AvCustomize|MyCharacters|Diagnostics")
	int32 GetRuntimeMyCharacterRowCountForAutomation() const
	{
		return RuntimeMyCharacterRows.Num();
	}

	UFUNCTION(BlueprintPure, Category="AvCustomize|MyCharacters|Diagnostics")
	FString GetRuntimeMyCharacterIdsForAutomation() const;

	UFUNCTION(BlueprintCallable, Category="AvCustomize|MyCharacters|Diagnostics")
	bool TriggerMyCharacterRenameForAutomation(FName CharacterId);

	UFUNCTION(BlueprintCallable, Category="AvCustomize|MyCharacters|Diagnostics")
	bool CommitMyCharacterRenameForAutomation(FName CharacterId, const FString& NewDisplayName);

	UFUNCTION(BlueprintCallable, Category="AvCustomize|MyCharacters|Diagnostics")
	bool CancelMyCharacterRenameForAutomation(FName CharacterId);

	UFUNCTION(BlueprintPure, Category="AvCustomize|MyCharacters|Diagnostics")
	bool IsMyCharacterRenameFocusedForAutomation(FName CharacterId) const;

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	/** Diagnostic A/B switch. Shipping responsive layout always uses the capped working region. */
	UFUNCTION(BlueprintCallable, Category="AvCustomize|Diagnostics")
	void SetFullWidthWorkingRegionForAutomation(bool bEnabled);
#endif

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseCaptureLost(
		const FCaptureLostEvent& CaptureLostEvent) override;

private:
	void HandleCultureChanged();
	void RefreshLocalizedRuntimeText();
	void UpdateLocalizationDiagnostics();

	UFUNCTION()
	void HandleCloseClicked();

	UFUNCTION()
	void HandlePresetPreviousClicked();

	UFUNCTION()
	void HandlePresetNextClicked();

	UFUNCTION()
	void HandleCategoryMyCharactersClicked();

	UFUNCTION()
	void HandleAddCharacterClicked();

	UFUNCTION()
	void HandleDeleteActiveCharacterClicked();

	UFUNCTION()
	void HandleResetActionHovered();

	UFUNCTION()
	void HandleResetActionUnhovered();

	UFUNCTION()
	void HandleResetActionPressed();

	UFUNCTION()
	void HandleResetActionReleased();

	UFUNCTION()
	void HandleDeleteActionHovered();

	UFUNCTION()
	void HandleDeleteActionUnhovered();

	UFUNCTION()
	void HandleDeleteActionPressed();

	UFUNCTION()
	void HandleDeleteActionReleased();

	UFUNCTION()
	void HandleCategoryPresetsClicked();

	UFUNCTION()
	void HandleCategoryHeadClicked();

	UFUNCTION()
	void HandleHeadFilterHeadTypeClicked();

	UFUNCTION()
	void HandleHeadFilterHeadgearClicked();

	UFUNCTION()
	void HandleHeadFilterHairClicked();

	UFUNCTION()
	void HandleHeadFilterBeardClicked();

	UFUNCTION()
	void HandleCategoryFaceProtectionClicked();

	UFUNCTION()
	void HandleFaceFilterGlassesClicked();

	UFUNCTION()
	void HandleFaceFilterRespiratorClicked();

	UFUNCTION()
	void HandleFaceFilterHeadphonesClicked();

	UFUNCTION()
	void HandleCategoryHandsAccessoriesClicked();

	UFUNCTION()
	void HandleHandsFilterGlovesClicked();

	UFUNCTION()
	void HandleHandsFilterWatchesClicked();

	UFUNCTION()
	void HandleCategoryUpperGearClicked();

	UFUNCTION()
	void HandleCategoryLowerHipClicked();

	UFUNCTION()
	void HandleLowerFilterLegsClicked();

	UFUNCTION()
	void HandleLowerFilterHipClicked();

	UFUNCTION()
	void HandleCategoryFullOutfitsClicked();

	UFUNCTION()
	void HandleSaveCharacterClicked();

	UFUNCTION()
	void HandleApplyCharacterClicked();

	UFUNCTION()
	void HandleRandomAppearanceClicked();

	UFUNCTION()
	void HandleRandomAppearanceRequested();

	UFUNCTION()
	void HandleRandomAppearanceCancelClicked();

	UFUNCTION()
	void HandleRandomAppearanceCreateClicked();

	UFUNCTION()
	void HandleResetAppearanceClicked();

	UFUNCTION()
	void HandleEditCharacterNameClicked();

	UFUNCTION()
	void HandleAppearanceInspectorShowAll();

	UFUNCTION()
	void HandleAppearanceInspectorHideClothing();

	UFUNCTION()
	void HandleAppearanceInspectorBodyOnly();

	UFUNCTION()
	void HandleAppearanceInspectorHideBody();

	UFUNCTION()
	void HandleAppearanceInspectorShowBody();

	UFUNCTION()
	void HandleAppearanceInspectorClearSolo();

	UFUNCTION()
	void HandleAppearanceInspectorCopy();

	UFUNCTION()
	void HandleAppearanceInspectorDump();

	UFUNCTION()
	void HandleAppearanceInspectorForceLod0();

	UFUNCTION()
	void HandleAppearanceInspectorAutoLod();

	UFUNCTION()
	void HandleCharacterNameTextChanged(const FText& NewText);

	UFUNCTION()
	void HandleCharacterNameCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	void BeginCharacterNameEditing();
	void CommitCharacterNameEditing(const FText& NewName);
	void CancelCharacterNameEditing();
	void ShowAppearanceConfirmation(EAvAppearanceModalMode Mode);
	void HideAppearanceConfirmation();
	void UpdateAppearanceConfirmationText(EAvAppearanceModalMode Mode) const;
	void ConfigureEquipmentConflictCheckbox();
	void ConfigureCurrentSelectionRow(UTextBlock* Label, UTextBlock* Value) const;
	bool IsEquipmentConflictModalMode(EAvAppearanceModalMode Mode) const;
	bool ShouldSuppressRaisedHoodEquipmentWarning() const;
	void SetSuppressRaisedHoodEquipmentWarning(bool bSuppress) const;
	bool ShouldSuppressHeadgearHeadphonesWarning() const;
	void SetSuppressHeadgearHeadphonesWarning(bool bSuppress) const;
	bool ArePendingConflictWarningsSuppressed(
		const FAvPendingEquipmentConflict& Pending) const;
	void PersistPendingConflictWarnings(
		const FAvPendingEquipmentConflict& Pending) const;
	FString ResolveTorsoStableId(const FSoftObjectPath& MeshPath) const;
	FString ResolveHeadgearStableId(const FSoftObjectPath& MeshPath) const;
	FString ResolveHeadphonesStableId(const FSoftObjectPath& MeshPath) const;
	bool BeginRaisedHoodConflictForTorso(const FAvTorsoCatalogItem& TargetItem);
	bool BeginRaisedHoodConflictForHeadgear(const FAvHeadCustomizationCatalogItem& TargetItem);
	bool BeginRaisedHoodConflictForHeadphones(const FAvFaceProtectionCatalogItem& TargetItem);
	bool ApplyManualModularSelectionAtomically(
		EWorkerSlot TargetSlot,
		const FSoftObjectPath& TargetMeshPath,
		const FString& TargetStableId,
		FString& OutValidationDetails);
	bool ExecuteEquipmentConflictAtomically(
		const FAvPendingEquipmentConflict& Pending,
		bool bPersistSuppression);
	void RefreshHeadCardSelectionStates();
	void RefreshFaceProtectionCardSelectionStates();
	bool ShouldConfirmRandomAppearance() const;
	void ExecuteExistingRandomAppearance();
	void ExecuteResetToBase(FName TargetCharacterId);
	void ExecuteDeleteCharacter(FName TargetCharacterId);

	void EnsurePreview();
	void ConfigurePreviewVisualSurface();
	void ApplyResponsiveLayoutPolicy(const FIntPoint& ViewportSize);
	void HandleGameViewportResized(FViewport* Viewport, uint32 Unused);
	void ResizePreviewRenderTargetToViewport(const FIntPoint& ViewportSize);
	bool RefreshPreviewCompositionFromGeometry();
	FIntPoint GetPreviewSurfacePixelSize() const;
	void LoadActiveCharacterState();
	bool TryApplySelectionFromAppearance(const FWorkerAppearance& Appearance);
	void RefreshPresetPreview();
	void ApplyActiveCharacterToPreview();
	void EnsureMyCharactersPageInitialized();
	void RebuildMyCharacterRows();
	void ClearRuntimeMyCharacterRows();
	void CommitRuntimeMyCharacterRenames(UAvMyCharacterRowWidget* ExceptRow = nullptr);
	void RefreshMyCharacterRowsSelection();
	void MoveAddCharacterButtonToEnd();
	void ConfigureMyCharactersActionButtons();
	void RefreshMyCharactersActionButtons();
	void ApplyMyCharactersActionVisual(
		bool bDeleteAction,
		EAvMyCharactersActionVisualState VisualState);
	void RefreshCustomizationSelectionsAfterCharacterChange();
	void RefreshHandsAccessoriesCardSelectionStates();
	void ResetPreviewCameraForActiveCharacter();
	UTexture2D* ResolveMyCharactersSilhouette();
	void CommitActiveCharacterAppearance(
		const FWorkerAppearance& NewAppearance,
		bool bHasMeaningfulAppearance,
		FName BasePresetId,
		EAvAppearanceOrigin AppearanceOrigin);
	void RefreshCurrentLoadoutSummary();
	void SetCurrentLoadoutValue(FName WidgetName, FName LegacyWidgetName, const FText& Value) const;
	FWorkerAppearance BuildSelectedAppearance() const;
	void UpdatePresetLabels();
	UImage* ResolvePreviewImage() const;
	UWidget* ResolvePreviewProjectionWidget() const;
	void BindApplyButton();
	UButton* ResolveApplyButton() const;
	void BindCategoryButtons();
	void RefreshActiveCharacterDisplay();
	void SelectCategory(EAvCustomizationCategory Category);
	UWidget* ResolvePreviewInputWidget() const;
	bool IsPointerOverPreview(const FVector2D& ScreenPosition) const;
	void EndPreviewDrag();
	void TransitionPreviewCameraForCategory(
		EAvCustomizationCategory Category, bool bImmediate = false);
	void InitializeHeadCustomizationPage();
	void EnsureHeadPageComposition();
	void SelectHeadSection(EAvHeadCustomizationSection Section);
	int32 GetHeadSectionPageIndex(EAvHeadCustomizationSection Section) const;
	UUniformGridPanel* GetGridForHeadSection(EAvHeadCustomizationSection Section) const;
	UScrollBox* GetScrollBoxForHeadSection(EAvHeadCustomizationSection Section) const;
	UTextBlock* GetEmptyStateForHeadSection(EAvHeadCustomizationSection Section) const;
	FName GetActiveHeadTypeId() const;
	void InvalidateHeadDependentSectionCards();
	void RefreshHeadSectionVisuals();
	void RebuildHeadItemCards();
	void RefreshHeadItemSelection();
	void RefreshHeadCurrentSelection();
	void BuildHeadThumbnailRegistry();
	void ConfigureHeadFilterButton(
		UButton* Button,
		UBorder* Frame,
		UBorder* Fill,
		UImage* Icon,
		UTextBlock* Label,
		bool bSelected) const;
	UTexture2D* ResolveHeadItemThumbnail(
		const FAvHeadCustomizationCatalogItem* Item,
		bool bNoneCard) const;
	bool IsHeadCatalogItemSelected(const FAvHeadCustomizationCatalogItem& Item) const;
	bool IsHeadCatalogItemDisabled(
		const FAvHeadCustomizationCatalogItem& Item,
		FString& OutReason) const;
	void InitializeFaceProtectionPage();
	void EnsureFaceProtectionPageComposition();
	void SelectFaceProtectionSection(EAvFaceProtectionSection Section);
	int32 GetFaceProtectionSectionPageIndex(EAvFaceProtectionSection Section) const;
	UUniformGridPanel* GetGridForFaceProtectionSection(EAvFaceProtectionSection Section) const;
	UScrollBox* GetScrollBoxForFaceProtectionSection(EAvFaceProtectionSection Section) const;
	UTextBlock* GetEmptyStateForFaceProtectionSection(EAvFaceProtectionSection Section) const;
	void BuildFaceProtectionThumbnailRegistry();
	UTexture2D* ResolveFaceProtectionItemThumbnail(int32 CatalogIndex) const;
	void RefreshFaceProtectionFilterVisuals();
	void RebuildFaceProtectionItemCards();
	void RefreshFaceProtectionCurrentSelection();
	void InvalidateFaceProtectionCards();
	void InitializeHandsAccessoriesPage();
	void EnsureHandsAccessoriesPageComposition();
	void SelectHandsAccessoriesSection(EAvHandsAccessoriesSection Section);
	int32 GetHandsAccessoriesSectionPageIndex(EAvHandsAccessoriesSection Section) const;
	UUniformGridPanel* GetGridForHandsAccessoriesSection(EAvHandsAccessoriesSection Section) const;
	UScrollBox* GetScrollBoxForHandsAccessoriesSection(EAvHandsAccessoriesSection Section) const;
	UTextBlock* GetEmptyStateForHandsAccessoriesSection(EAvHandsAccessoriesSection Section) const;
	void BuildHandsAccessoriesThumbnailRegistry();
	UTexture2D* ResolveHandsAccessoriesItemThumbnail(int32 CatalogIndex) const;
	void RefreshHandsAccessoriesFilterVisuals();
	void RebuildHandsAccessoriesItemCards();
	void RefreshHandsAccessoriesCurrentSelection();
	bool IsHandsAccessoriesItemCompatible(const FAvHandsAccessoriesCatalogItem& Item, FString& OutDisabledReason) const;
	void InvalidateHandsAccessoriesCards();
	void InitializeUpperEquipmentPage();
	void EnsureUpperEquipmentPageComposition();
	void BuildTorsoThumbnailRegistry();
	UTexture2D* ResolveTorsoItemThumbnail(int32 CatalogIndex) const;
	void RebuildTorsoItemCards(bool bScrollToStart = false);
	void RefreshTorsoCurrentSelection();
	void RefreshTorsoCardSelectionStates();
	void UpdateTorsoSelectionCards(const FString& PreviousMeshPath, const FString& NewMeshPath);
	void InvalidateTorsoCards();
	void InitializeLowerHipPage();
	void EnsureLowerHipPageComposition();
	void SelectLowerHipSection(EAvLowerHipSection Section);
	void RefreshLowerHipFilterVisuals();
	void BuildLowerHipThumbnailRegistry();
	UTexture2D* ResolveLowerHipItemThumbnail(int32 CatalogIndex) const;
	void RebuildLowerHipItemCards(EAvLowerHipSection Section, bool bScrollToStart = false);
	void RefreshLowerHipCardSelectionStates(EAvLowerHipSection Section);
	void RefreshLowerHipCurrentSelection();
	void InvalidateLowerHipCards(EAvLowerHipSection Section);
	UScrollBox* GetScrollBoxForLowerHipSection(EAvLowerHipSection Section) const;
	UUniformGridPanel* GetGridForLowerHipSection(EAvLowerHipSection Section) const;
	UTextBlock* GetEmptyStateForLowerHipSection(EAvLowerHipSection Section) const;
	void InitializeFullOutfitsPage();
	void EnsureFullOutfitsPageComposition();
	void BuildFullOutfitThumbnailRegistry();
	UTexture2D* ResolveFullOutfitItemThumbnail(int32 CatalogIndex) const;
	void RebuildFullOutfitItemCards(bool bScrollToStart = false);
	void RefreshFullOutfitCardSelectionStates();
	void RefreshFullOutfitCurrentSelection();
	void InvalidateFullOutfitCards();
	void CaptureCategoryDesignerVisuals();
	void RefreshCategoryVisuals();
	void ApplyCategoryVisual(EAvCustomizationCategory Category, bool bIsActive) const;
	void RebuildBottomEquipmentPanel();
	UWidget* GetOrCreateBottomTextWidget(FName WidgetName, const FText& Text);
	void AddWidgetToBottomControls(UWidget* Widget, UWrapBox* ControlsRow, float MinWidth, const FMargin& SlotPadding) const;

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	void EnsureAppearanceInspector();
	void RefreshAppearanceInspector();
	FString BuildAppearanceInspectorDiagnosticJson();
	FString GetAppearanceInspectorOriginName() const;
	FString GetAppearanceInspectorCategoryName() const;
	FString GetAppearanceModalModeName() const;
	FString GetEquipmentConflictWarningTypeName() const;
#endif

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Button_Close;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_Close;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_PresetPrevious;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_PresetNext;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_SaveCharacter;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_ApplyCharacter;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_CategoryMyCharacters;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_CategoryPresets;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidgetSwitcher> WS_RightPanelPages;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UVerticalBox> VB_RightPanelPresets;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UVerticalBox> VB_RightPanelMyCharacters;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UScrollBox> ScrollBox_MyCharacters;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UVerticalBox> VB_MyCharactersList;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_AddCharacter;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<USizeBox> SB_CharacterCard01;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<USizeBox> SB_AddCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HB_MyCharactersActions;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_ResetActiveCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_ResetActiveCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_ResetActiveCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_ResetActiveCharacterLabel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_DeleteActiveCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_DeleteActiveCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_DeleteActiveCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_DeleteActiveCharacterLabel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UVerticalBox> VB_RightPanelHead;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VB_RightPanelFaceProtection;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_HeadTitle;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_HeadFilterHeadType;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_HeadFilterHeadgear;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_HeadFilterHair;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_HeadFilterBeard;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_HeadFilterHeadType;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_HeadFilterHeadgear;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_HeadFilterHair;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_HeadFilterBeard;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_HeadFilterHeadTypeFrame;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_HeadFilterHeadTypeFill;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Img_HeadFilterHeadTypeIcon;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_HeadFilterHeadgearFrame;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_HeadFilterHeadgearFill;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Img_HeadFilterHeadgearIcon;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_HeadFilterHairFrame;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_HeadFilterHairFill;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Img_HeadFilterHairIcon;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_HeadFilterBeardFrame;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_HeadFilterBeardFill;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Img_HeadFilterBeardIcon;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_HeadCurrentSelection;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_HeadCurrentSelectionValue;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_HeadEmptyState;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_HeadItems;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UUniformGridPanel> UniformGrid_HeadSubcategories;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGrid_HeadItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WS_HeadSectionPages;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_HeadgearItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGrid_HeadgearItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_HeadgearEmptyState;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_HairItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGrid_HairItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_HairEmptyState;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_BeardItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGrid_BeardItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_BeardEmptyState;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UAvHeadCatalogItemButton> DesignSample_HeadType01;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UAvHeadCatalogItemButton> DesignSample_HeadType02;

	UPROPERTY(EditDefaultsOnly, Category="AvCustomize|Head")
	TSubclassOf<UAvHeadCatalogItemButton> HeadCatalogItemCardClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WS_FaceProtectionSectionPages;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_FaceFilterGlasses;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_FaceFilterRespirator;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_FaceFilterHeadphones;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHeadTypeFrame_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHeadTypeFill_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> Img_HeadFilterHeadTypeIcon_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_HeadFilterHeadType_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHeadgearFrame_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHeadgearFill_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> Img_HeadFilterHeadgearIcon_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_HeadFilterHeadgear_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHairFrame_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHairFill_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> Img_HeadFilterHairIcon_1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_HeadFilterHair_1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_FaceCurrentSelection;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_FaceCurrentSelectionValue;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_GlassesItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGrid_GlassesItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_GlassesEmptyState;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_RespiratorItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGrid_RespiratorItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_RespiratorEmptyState;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_HeadphonesItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGrid_HeadphonesItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_HeadphonesEmptyState;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UAvHeadCatalogItemButton> DesignSample_FaceGlassesNone;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UAvHeadCatalogItemButton> DesignSample_FaceGlassesItem;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UAvHeadCatalogItemButton> DesignSample_FaceRespiratorNone;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UAvHeadCatalogItemButton> DesignSample_FaceRespiratorItem;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UAvHeadCatalogItemButton> DesignSample_FaceHeadphonesNone;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UAvHeadCatalogItemButton> DesignSample_FaceHeadphonesItem;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UVerticalBox> VB_RightPanelHandsAccessories;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UWidgetSwitcher> WS_HandsAccessoriesSectionPages;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Btn_HandsFilterGloves;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Btn_HandsFilterWatches;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHeadTypeFrame_2;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHeadTypeFill_2;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> Img_HandsFilterGlovesIcon;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_HandsFilterGloves;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHeadgearFrame_2;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UBorder> Border_HeadFilterHeadgearFill_2;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> Img_HandsFilterWatchesIcon;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_HandsFilterWatches;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_HandsCurrentSelection;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_HandsCurrentSelectionValue;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UScrollBox> ScrollBox_GlovesItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UUniformGridPanel> UniformGrid_GlovesItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_GlovesEmptyState;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UScrollBox> ScrollBox_WatchesItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UUniformGridPanel> UniformGrid_WatchesItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_WatchesEmptyState;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_GlovesNone;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_GlovesItem;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_WatchesNone;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_WatchesItem;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_UpperCurrentSelection;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_UpperCurrentSelectionValue;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UScrollBox> ScrollBox_TorsoItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UUniformGridPanel> UniformGrid_TorsoItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_TorsoEmptyState;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UVerticalBox> VB_RightPanelUpperEquipment;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_TorsoNone;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_TorsoItem;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Btn_LowerFilterLegs;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Btn_LowerFilterHip;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_LowerCurrentSelection;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_LowerCurrentSelectionValue;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UWidgetSwitcher> WS_LowerHipSectionPages;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UScrollBox> ScrollBox_LegsItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UUniformGridPanel> UniformGrid_LegsItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_LegsEmptyState;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UScrollBox> ScrollBox_HipItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UUniformGridPanel> UniformGrid_HipItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_HipEmptyState;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UVerticalBox> VB_RightPanelLowerHip;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_LegsNone;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_LegsItem;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_HipNone;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_HipItem;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UVerticalBox> VB_RightPanelFullOutfits;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_FullOutfitsCurrentSelection;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_FullOutfitsCurrentSelectionValue;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UScrollBox> ScrollBox_FullOutfitItems;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UUniformGridPanel> UniformGrid_FullOutfitItems;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_FullOutfitNone;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UAvHeadCatalogItemButton> DesignSample_FullOutfitItem;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_CategoryHead;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_CategoryFaceProtection;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_CategoryHandsAccessories;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_CategoryUpperGear;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_CategoryLowerHip;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_CategoryFullOutfits;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_RandomAppearance;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UOverlay> OverlayRandom;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Btn_RandomAppearanceModalBackdrop;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_RandomAppearanceDialogTitle;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_RandomAppearanceDialogQuestion;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_RandomAppearanceDialogWarning;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Btn_RandomAppearanceCancel;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_RandomAppearanceCancelLabel;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Btn_RandomAppearanceCreate;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_RandomAppearanceCreateLabel;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UHorizontalBox> HB_EquipmentConflictDontShowAgain;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UCheckBox> CheckBox_EquipmentConflictDontShowAgain;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Txt_EquipmentConflictDontShowAgain;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_EditCharacterName01;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_CharacterName01;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UEditableTextBox> ETB_CharacterName01;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_CurrentPresetName;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_PresetCounter;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Img_Preview;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Img_PreviewPlaceholder;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> FullscreenDarkImage;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCanvasPanel> SafeContentContainer;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> TopNavigation;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> TopOrangeLine;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> LeftCategoryPanel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> CenterPreviewPanel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> RightOptionsPanel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> BottomEquipmentPanel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> Btn_RandomizeAppearance;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> EquipmentSlot_Jacket;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> EquipmentSlot_Pants;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> EquipmentSlot_Boots;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> EquipmentSlot_Helmet;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> EquipmentSlot_Mask;

	FIntPoint CachedViewportSize = FIntPoint::ZeroValue;
	bool bHasAppliedResponsiveLayout = false;
	bool bViewportResizeRefreshQueued = false;
	bool bPreviewCompositionRefreshPending = false;
	uint8 PreviewCompositionRefreshFramesRemaining = 0;
	float CachedPreviewCompositionCenterX = 0.5f;
	FDelegateHandle ViewportResizedHandle;
	int32 ViewportResizeEventCount = 0;
	int32 ResponsiveLayoutApplyCount = 0;
	int32 PreviewRenderTargetResizeCount = 0;
	int32 PreviewCompositionRefreshCount = 0;
	float ResponsiveLogicalViewportWidth = 0.f;
	float ResponsiveLogicalViewportHeight = 0.f;
	float ResponsiveWorkingRegionWidth = 0.f;
	float ResponsiveWorkingRegionInset = 0.f;
	float ResponsiveLeftPanelWidth = 0.f;
	float ResponsiveRightPanelWidth = 0.f;
	float ResponsiveEdgeGap = 0.f;
	float ResponsiveInterPanelGap = 0.f;
	float ResponsivePreviewWidth = 0.f;
	bool bResponsiveGracefulDegradation = false;
	FString ResponsiveDegradationReason;
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	bool bFullWidthWorkingRegionForAutomation = false;
#endif
	FText CurrentCharacterDisplayName;
	FText CharacterNameBeforeEdit;
	bool bIsEditingCharacterName = false;
	bool bIsClampingCharacterName = false;
	bool bActiveCharacterHasMeaningfulAppearance = false;
	EAvAppearanceOrigin ActiveCharacterAppearanceOrigin = EAvAppearanceOrigin::Factory;
	TWeakPtr<SWidget> RandomAppearancePreviousFocus;
	int32 CurrentPresetIndex = 0;
	bool bHasCapturedCategoryDesignerVisuals = false;
	bool bHeadPageCompositionValidated = false;
	bool bFaceProtectionPageCompositionValidated = false;
	bool bHandsAccessoriesPageCompositionValidated = false;
	bool bUpperEquipmentPageCompositionValidated = false;
	bool bLowerHipPageCompositionValidated = false;
	bool bTorsoCatalogBuilt = false;
	bool bLegsCatalogBuilt = false;
	bool bHipCatalogBuilt = false;
	bool bFullOutfitsPageCompositionValidated = false;
	bool bFullOutfitCatalogBuilt = false;
	bool bPreviewDragActive = false;
	bool bPointerOverPreview = false;
	bool bCameraPresetInitialized = false;
	float PreviewYawDegrees = 0.f;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> PreviewMaterial;

	UPROPERTY(Transient)
	TObjectPtr<AAvCustomizePreviewActor> PreviewActor;

	UPROPERTY(Transient)
	TMap<FSoftObjectPath, FHeadItemThumbnailPair> HeadItemThumbnailRegistry;

	UPROPERTY(Transient)
	TMap<int32, FHeadItemThumbnailPair> FaceProtectionThumbnailRegistry;
	UPROPERTY(Transient)
	TMap<int32, FHeadItemThumbnailPair> HandsAccessoriesThumbnailRegistry;
	UPROPERTY(Transient) TMap<int32, FHeadItemThumbnailPair> TorsoThumbnailRegistry;
	UPROPERTY(Transient) TMap<int32, FHeadItemThumbnailPair> LowerHipThumbnailRegistry;
	UPROPERTY(Transient) TMap<int32, FHeadItemThumbnailPair> FullOutfitThumbnailRegistry;

	UPROPERTY(Transient)
	FHeadItemThumbnailPair NoneHeadgearThumbnails;

	UPROPERTY(Transient)
	FHeadItemThumbnailPair NoneHairThumbnails;

	UPROPERTY(Transient)
	FHeadItemThumbnailPair NoneBeardThumbnails;

	FSoftObjectPath HeadType01AppearancePath;
	FSoftObjectPath HeadType02AppearancePath;

	FWorkerAppearance ActiveCharacterAppearance;
	FWorkerAppearance PresetPreviewAppearance;
	FName ActiveCharacterBasePresetId;
	bool bHasPresetPreviewAppearance = false;
	EAvAppearanceModalMode AppearanceModalMode = EAvAppearanceModalMode::None;
	FAvPendingEquipmentConflict PendingEquipmentConflict;
	FAvPendingCharacterAction PendingCharacterAction;
	bool bAppearanceModalConfirmInProgress = false;
	bool bLastManualModularSelectionClearedEnsemble = false;
	int32 LastOperationAppearanceAutosaveCount = 0;
	FAvRandomHeadSelectionDiagnostics LastRandomHeadDiagnostics;
	int32 LastRandomCommitCount = 0;
	int32 LastRandomAutosaveCount = 0;
	FDelegateHandle CultureChangedHandle;
	int32 CultureSwitchSaveCommitCount = 0;
	int32 CultureSwitchAutosaveCount = 0;
	int32 CultureSwitchAppearanceMutationCount = 0;
	bool bExistingCharacterNamesChangedByCultureSwitch = false;
	bool bExistingCharacterNamesLiteral = true;
	bool bCurrentSelectionLocalized = false;
	bool bModalTextLocalized = false;
	bool bStringTableLoaded = false;
	int32 CustomizationLocalizedKeyCount = 0;
	int32 CustomizationMissingTranslationCount = 0;
	int32 CustomizationFallbackCount = 0;
	int32 CustomizationCultureInvariantVisibleTextCount = 0;
	int32 RuntimeCatalogLocalizedItemCount = 0;
	int32 TextOverflowDetectedCount = 0;

	EAvCustomizationCategory SelectedCategory = EAvCustomizationCategory::MyCharacters;
	EAvHeadCustomizationSection SelectedHeadSection = EAvHeadCustomizationSection::HeadType;
	EAvFaceProtectionSection SelectedFaceProtectionSection = EAvFaceProtectionSection::Glasses;
	EAvHandsAccessoriesSection SelectedHandsAccessoriesSection = EAvHandsAccessoriesSection::Gloves;
	EAvLowerHipSection SelectedLowerHipSection = EAvLowerHipSection::Legs;
	TArray<FAvHeadCustomizationCatalogItem> HeadCatalog;
	TArray<FAvFaceProtectionCatalogItem> FaceProtectionCatalog;
	TArray<FAvHandsAccessoriesCatalogItem> HandsAccessoriesCatalog;
	TArray<FAvTorsoCatalogItem> TorsoCatalog;
	TArray<FAvLowerHipCatalogItem> LowerHipCatalog;
	TArray<FAvFullOutfitCatalogItem> FullOutfitCatalog;

	TMap<EAvHeadCustomizationSection, TArray<TObjectPtr<UAvHeadCatalogItemButton>>>
		RuntimeHeadSectionCards;
	TMap<EAvFaceProtectionSection, TArray<TObjectPtr<UAvHeadCatalogItemButton>>>
		RuntimeFaceProtectionSectionCards;
	TMap<EAvHandsAccessoriesSection, TArray<TObjectPtr<UAvHeadCatalogItemButton>>>
		RuntimeHandsAccessoriesSectionCards;
	UPROPERTY(Transient) TArray<TObjectPtr<UAvHeadCatalogItemButton>> RuntimeTorsoCards;
	TArray<int32> RuntimeTorsoCardCatalogIndices;
	UPROPERTY(Transient) TArray<TObjectPtr<UAvHeadCatalogItemButton>> RuntimeLegsCards;
	UPROPERTY(Transient) TArray<TObjectPtr<UAvHeadCatalogItemButton>> RuntimeHipCards;
	TArray<int32> RuntimeLegsCardCatalogIndices;
	TArray<int32> RuntimeHipCardCatalogIndices;
	UPROPERTY(Transient) TArray<TObjectPtr<UAvHeadCatalogItemButton>> RuntimeFullOutfitCards;
	TArray<int32> RuntimeFullOutfitCardCatalogIndices;
	UPROPERTY(Transient) TArray<TObjectPtr<UAvMyCharacterRowWidget>> RuntimeMyCharacterRows;
	UPROPERTY(Transient) TObjectPtr<UTexture2D> MyCharactersSilhouetteTexture;
	bool bMyCharactersPageInitialized = false;
	FString LastRosterOperation = TEXT("None");
	FName LastCreatedCharacterId;
	FName LastSelectedCharacterId;
	FName LastResetCharacterId;
	FName LastDeletedCharacterId;
	FName LastActiveCharacterIdAfterDelete;
	int32 LastRosterSaveGameCommitCount = 0;
	int32 LastMyCharactersDuplicateDelegateCount = 0;
	FVector2D LastRowPointerScreenPosition = FVector2D::ZeroVector;
	FVector2D LastRowPointerLocalPosition = FVector2D::ZeroVector;
	FVector2D LastDisplayNameGeometrySize = FVector2D::ZeroVector;
	FVector2D LastDisplayNameMeasuredTextSize = FVector2D::ZeroVector;
	float LastDisplayNameVisibleWidth = 0.f;
	FVector4 LastRenameHitRect = FVector4(0.f, 0.f, 0.f, 0.f);
	bool bLastPointerInsideRenameTextRect = false;
	FString LastRowClickResult = TEXT("None");
	int32 RenameHandlerCallCount = 0;
	int32 SelectHandlerCallCount = 0;
	FName RenameCharacterId;
	float LastDisplayNameLayoutScale = 1.f;
	float LastSlateApplicationScale = 1.f;

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	TObjectPtr<UBorder> Border_AppearanceDebugInspector;
	TObjectPtr<UTextBlock> Txt_AppearanceDebugStatus;
	TMap<EWorkerSlot, TWeakObjectPtr<UTextBlock>> AppearanceInspectorMeshTexts;
	TMap<EWorkerSlot, TWeakObjectPtr<UTextBlock>> AppearanceInspectorVisibilityTexts;
	TMap<EWorkerSlot, TWeakObjectPtr<UTextBlock>> AppearanceInspectorMaterialTexts;
	TMap<EWorkerSlot, TWeakObjectPtr<UTextBlock>> AppearanceInspectorLodTexts;
	TMap<EWorkerSlot, TWeakObjectPtr<UAvAppearanceInspectorSlotButton>> AppearanceInspectorToggleButtons;
	TMap<EWorkerSlot, TWeakObjectPtr<UTextBlock>> AppearanceInspectorToggleLabels;
	TMap<EWorkerSlot, TWeakObjectPtr<UAvAppearanceInspectorSlotButton>> AppearanceInspectorSoloButtons;
	bool bAppearanceInspectorVisible = false;
#endif
};
