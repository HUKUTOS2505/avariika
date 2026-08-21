#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Reply.h"
#include "AvMainMenuShell.generated.h"

class APlayerController;
class UBorder;
class UButton;
class UEnhancedInputLocalPlayerSubsystem;
class UInputMappingContext;
class UOverlay;
class UScrollBox;
class USizeBox;
class UTextBlock;
class UVerticalBox;
class UWidgetSwitcher;

UENUM(BlueprintType)
enum class EAvMainMenuScreen : uint8
{
	Home,
	Settings,
	Credits
};

UENUM(BlueprintType)
enum class EAvMainMenuModal : uint8
{
	None,
	ContinueInfo,
	NewGame,
	Exit,
	DisplayConfirm
};

UENUM(BlueprintType)
enum class EAvMainMenuSettingRow : uint8
{
	Resolution,
	WindowMode,
	AspectRatio,
	VSync,
	FrameRateLimit,
	Brightness,
	ColorDeficiencyType,
	ColorDeficiencyStrength,
	OverallQuality,
	GlobalIlluminationQuality,
	ShadowQuality,
	AntiAliasingQuality,
	ViewDistanceQuality,
	TextureQuality,
	EffectsQuality,
	ReflectionQuality,
	FoliageQuality,
	ShadingQuality,
	PostProcessQuality,
	GlobalIlluminationMethod,
	ReflectionMethod,
	MotionBlur,
	LensFlares,
	Bloom,
	AmbientOcclusion,
	ResolutionScale,
	AntiAliasingMethod,
	MasterVolume,
	SFXVolume,
	MusicVolume,
	VoiceVolume,
	Language,
	Controls
};

USTRUCT(BlueprintType)
struct FAvMainMenuSettingsSnapshot
{
	GENERATED_BODY()

	UPROPERTY() FIntPoint Resolution = FIntPoint(1920, 1080);
	UPROPERTY() int32 WindowMode = 1;
	UPROPERTY() int32 AspectRatio = 0;
	UPROPERTY() bool bVSync = false;
	UPROPERTY() float FrameRateLimit = 0.f;
	UPROPERTY() float Brightness = 1.f;
	UPROPERTY() int32 ColorDeficiencyType = 0;
	UPROPERTY() int32 ColorDeficiencyStrength = 0;
	/** 0..3 = Low..Epic, 4 = Custom. */
	UPROPERTY() int32 OverallQuality = 2;
	UPROPERTY() int32 GlobalIlluminationQuality = 2;
	UPROPERTY() int32 ShadowQuality = 2;
	UPROPERTY() int32 AntiAliasingQuality = 2;
	UPROPERTY() int32 ViewDistanceQuality = 2;
	UPROPERTY() int32 TextureQuality = 2;
	UPROPERTY() int32 EffectsQuality = 2;
	UPROPERTY() int32 ReflectionQuality = 2;
	UPROPERTY() int32 FoliageQuality = 2;
	UPROPERTY() int32 ShadingQuality = 2;
	UPROPERTY() int32 PostProcessQuality = 2;
	UPROPERTY() int32 GlobalIlluminationMethod = 1;
	UPROPERTY() int32 ReflectionMethod = 1;
	UPROPERTY() int32 AntiAliasingMethod = 4;
	UPROPERTY() bool bMotionBlur = false;
	UPROPERTY() bool bLensFlares = true;
	UPROPERTY() bool bBloom = true;
	UPROPERTY() bool bAmbientOcclusion = true;
	UPROPERTY() float ResolutionScale = 100.f;
	UPROPERTY() float MasterVolume = 1.f;
	UPROPERTY() float SFXVolume = 1.f;
	UPROPERTY() float MusicVolume = 1.f;
	UPROPERTY() float VoiceVolume = 1.f;
	UPROPERTY() FString Culture = TEXT("ru");

	bool EqualsDisplay(const FAvMainMenuSettingsSnapshot& Other) const;
	bool Equals(const FAvMainMenuSettingsSnapshot& Other) const;
};

/** Read-only validation of the project-owned company/gameplay save. */
UCLASS(BlueprintType)
class AVARYO_API UAvMainMenuSavePresenceAdapter : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="AVARIIKA|MainMenu")
	bool Refresh();

	UFUNCTION(BlueprintPure, Category="AVARIIKA|MainMenu")
	bool HasValidSave() const { return bHasValidSave; }

	UFUNCTION(BlueprintPure, Category="AVARIIKA|MainMenu")
	FString GetValidationReason() const { return ValidationReason; }

private:
	UPROPERTY(Transient) bool bHasValidSave = false;
	UPROPERTY(Transient) FString ValidationReason;
};

/**
 * Project-owned settings boundary. AvariikaUserSettings remains persistence;
 * UGameUserSettings is only the native display/scalability apply layer.
 */
UCLASS(BlueprintType)
class AVARYO_API UAvMainMenuSettingsAdapter : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UWorld* InWorld);
	void Adjust(EAvMainMenuSettingRow Row, int32 Delta);
	FText GetValueText(EAvMainMenuSettingRow Row) const;
	bool BeginApply(bool& bOutNeedsDisplayConfirmation);
	void ConfirmApply();
	void ApplyPersistedRuntimeState();
	void SetPendingIndex(EAvMainMenuSettingRow Row, int32 Value);
	void SetPendingScalar(EAvMainMenuSettingRow Row, float Value);
	int32 GetPendingIndex(EAvMainMenuSettingRow Row) const;
	float GetPendingScalar(EAvMainMenuSettingRow Row) const;
	int32 GetOriginalIndex(EAvMainMenuSettingRow Row) const;
	float GetOriginalScalar(EAvMainMenuSettingRow Row) const;
	TArray<FText> GetOptions(EAvMainMenuSettingRow Row) const;
	void CaptureNativeStateAndCommit();
	void ResetToDefaults();
	void RevertPending();
	void RevertDisplayPreview();
	bool IsDisplayChanged() const;
	bool HasPendingChanges() const { return !Pending.Equals(Original); }
	bool HasDedicatedAudioChannels() const { return bDedicatedAudioChannelsReady; }
	bool IsSupportedResolution(const FIntPoint& Resolution) const;
	const FAvMainMenuSettingsSnapshot& GetPending() const { return Pending; }
	const FAvMainMenuSettingsSnapshot& GetOriginal() const { return Original; }
	int32 GetSaveCommitCount() const { return SaveCommitCount; }
	bool WasLastApplyPIESafe() const { return bLastApplyWasPIESafe; }

private:
	void LoadSupportedResolutions();
	void LoadProjectSettingsSave();
	void ApplyCulture(const FString& Culture) const;
	void ApplyNonDisplayRuntimeState();
	void ApplyAudioRuntimeState();
	void ApplyNativePreview();
	void SaveProjectSettings();
	int32 FindResolutionIndex(const FIntPoint& Resolution) const;
	TArray<FIntPoint> GetFilteredResolutions() const;
	void ApplyQualityPreset(int32 Quality);
	void MarkQualityCustom();
	bool IsPIEWorld() const;

	UPROPERTY(Transient) TObjectPtr<UWorld> World;
	UPROPERTY(Transient) TArray<FIntPoint> SupportedResolutions;
	UPROPERTY(Transient) FAvMainMenuSettingsSnapshot Original;
	UPROPERTY(Transient) FAvMainMenuSettingsSnapshot Pending;
	UPROPERTY(Transient) FAvMainMenuSettingsSnapshot BeforeDisplayPreview;
	UPROPERTY(Transient) bool bDisplayPreviewApplied = false;
	UPROPERTY(Transient) bool bLastApplyWasPIESafe = false;
	UPROPERTY(Transient) bool bDedicatedAudioChannelsReady = false;
	UPROPERTY(Transient) bool bAudioMixPushed = false;
	UPROPERTY(Transient) int32 SaveCommitCount = 0;
};

/** Single owner of cursor/input mode and the temporary vendor menu IMC. */
UCLASS(BlueprintType)
class AVARYO_API UAvMainMenuInputRouter : public UObject
{
	GENERATED_BODY()

public:
	void Activate(APlayerController* InController, UWidget* InitialFocus);
	void Deactivate();
	bool IsActive() const { return bActive; }
	bool HasVendorMenuContext() const { return bVendorMenuContextLoaded; }

	static bool IsBackKey(const FKey& Key);
	static bool IsAcceptKey(const FKey& Key);
	static int32 GetVerticalDirection(const FKey& Key);
	static int32 GetHorizontalDirection(const FKey& Key);

private:
	UPROPERTY(Transient) TObjectPtr<APlayerController> Controller;
	UPROPERTY(Transient) TObjectPtr<UInputMappingContext> VendorMenuContext;
	UPROPERTY(Transient) TObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedSubsystem;
	UPROPERTY(Transient) bool bActive = false;
	UPROPERTY(Transient) bool bVendorMenuContextLoaded = false;
};

/** Lightweight state coordinator; widgets request routes instead of owning travel/persistence. */
UCLASS(BlueprintType)
class AVARYO_API UAvMainMenuCoordinator : public UObject
{
	GENERATED_BODY()

public:
	void SetScreen(EAvMainMenuScreen InScreen) { Screen = InScreen; }
	void SetModal(EAvMainMenuModal InModal) { Modal = InModal; }
	EAvMainMenuScreen GetScreen() const { return Screen; }
	EAvMainMenuModal GetModal() const { return Modal; }
	bool HasModal() const { return Modal != EAvMainMenuModal::None; }

private:
	UPROPERTY(Transient) EAvMainMenuScreen Screen = EAvMainMenuScreen::Home;
	UPROPERTY(Transient) EAvMainMenuModal Modal = EAvMainMenuModal::None;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAvMainMenuActionEvent, int32, ActionId);

/** Rectangular, project-owned button with explicit hover/focus/disabled states. */
UCLASS(Blueprintable)
class AVARYO_API UAvMainMenuActionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	void Configure(int32 InActionId, const TCHAR* InTextKey, bool bInEnabled = true);
	void SetEnabledState(bool bInEnabled);
	void RefreshText();
	int32 GetActionId() const { return ActionId; }
	bool IsActionEnabled() const { return bActionEnabled; }

	UPROPERTY(BlueprintAssignable) FAvMainMenuActionEvent OnActivated;

private:
	UFUNCTION() void HandleClicked();
	UFUNCTION() void HandleHovered();
	UFUNCTION() void HandleUnhovered();
	void BuildTree();
	void RefreshStyle();

	UPROPERTY(Transient) TObjectPtr<UButton> Button;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> Label;
	UPROPERTY(Transient) int32 ActionId = INDEX_NONE;
	UPROPERTY(Transient) FString TextKey;
	UPROPERTY(Transient) bool bActionEnabled = true;
	UPROPERTY(Transient) bool bHovered = false;
	UPROPERTY(Transient) bool bFocused = false;
};

UCLASS(Blueprintable)
class AVARYO_API UAvMainMenuSettingRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	void Configure(EAvMainMenuSettingRow InRow, const TCHAR* InLabelKey, bool bInEnabled = true);
	void SetValue(const FText& Value);
	void RefreshLabel();
	EAvMainMenuSettingRow GetRow() const { return Row; }
	bool IsRowEnabled() const { return bRowEnabled; }
	UPROPERTY(BlueprintAssignable) FAvMainMenuActionEvent OnActivated;

private:
	UFUNCTION() void HandleLeft();
	UFUNCTION() void HandleRight();
	UFUNCTION() void HandleHover();
	UFUNCTION() void HandleUnhover();
	void BuildTree();
	void RefreshStyle();

	UPROPERTY(Transient) TObjectPtr<UBorder> Frame;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> Label;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> ValueText;
	UPROPERTY(Transient) TObjectPtr<UButton> LeftButton;
	UPROPERTY(Transient) TObjectPtr<UButton> RightButton;
	UPROPERTY(Transient) EAvMainMenuSettingRow Row = EAvMainMenuSettingRow::Resolution;
	UPROPERTY(Transient) FString LabelKey;
	UPROPERTY(Transient) bool bRowEnabled = true;
	UPROPERTY(Transient) bool bHovered = false;
	UPROPERTY(Transient) bool bFocused = false;
};

class UAvMainMenuRootWidget;

UCLASS(Blueprintable)
class AVARYO_API UAvMainMenuHomeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	void InitializeHome(UAvMainMenuRootWidget* InRoot, bool bContinueEnabled);
	void RefreshText();
	const TArray<TObjectPtr<UAvMainMenuActionButtonWidget>>& GetButtons() const { return Buttons; }

private:
	void BuildTree();
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuRootWidget> Root;
	UPROPERTY(Transient) TObjectPtr<UVerticalBox> ButtonList;
	UPROPERTY(Transient) TArray<TObjectPtr<UAvMainMenuActionButtonWidget>> Buttons;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> ComingSoonLabel;
};

UCLASS(Blueprintable)
class AVARYO_API UAvMainMenuConfirmDialogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	void InitializeDialog();
	void ShowDialog(const FText& Title, const FText& Body, bool bShowYes = true);
	void HideDialog();
	void RefreshText();
	UAvMainMenuActionButtonWidget* GetYesButton() const { return YesButton; }
	UAvMainMenuActionButtonWidget* GetNoButton() const { return NoButton; }

private:
	void BuildTree();
	UPROPERTY(Transient) TObjectPtr<UTextBlock> TitleText;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> BodyText;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuActionButtonWidget> YesButton;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuActionButtonWidget> NoButton;
	UPROPERTY(Transient) FText CurrentTitle;
	UPROPERTY(Transient) FText CurrentBody;
};

UCLASS(Blueprintable)
class AVARYO_API UAvMainMenuSettingsHostWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	void InitializeSettings(UAvMainMenuRootWidget* InRoot, UAvMainMenuSettingsAdapter* InAdapter);
	void RefreshAll();
	const TArray<TObjectPtr<UAvMainMenuSettingRowWidget>>& GetRows() const { return Rows; }
	UAvMainMenuActionButtonWidget* GetApplyButton() const { return ApplyButton; }
	UAvMainMenuActionButtonWidget* GetRevertButton() const { return RevertButton; }
	UAvMainMenuActionButtonWidget* GetBackButton() const { return BackButton; }

private:
	void BuildTree();
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuRootWidget> Root;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuSettingsAdapter> Adapter;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> Heading;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> AudioNote;
	UPROPERTY(Transient) TObjectPtr<UScrollBox> Scroll;
	UPROPERTY(Transient) TArray<TObjectPtr<UAvMainMenuSettingRowWidget>> Rows;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuActionButtonWidget> ApplyButton;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuActionButtonWidget> RevertButton;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuActionButtonWidget> BackButton;
};

UCLASS(Blueprintable)
class AVARYO_API UAvMainMenuCreditsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	void InitializeCredits();
	void RefreshText();
	UAvMainMenuActionButtonWidget* GetBackButton() const { return BackButton; }

private:
	void BuildTree();
	UPROPERTY(Transient) TObjectPtr<UTextBlock> Heading;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> Body;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuActionButtonWidget> BackButton;
};

/** Authored root: screen stack, modal overlay, routing and responsive shell. */
UCLASS(Blueprintable)
class AVARYO_API UAvMainMenuRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent) override;

	UFUNCTION(BlueprintCallable, Category="AVARIIKA|MainMenu|Diagnostics") void DevNavigate(int32 Direction);
	UFUNCTION(BlueprintCallable, Category="AVARIIKA|MainMenu|Diagnostics") void DevAdjust(int32 Direction);
	UFUNCTION(BlueprintCallable, Category="AVARIIKA|MainMenu|Diagnostics") void DevAccept();
	UFUNCTION(BlueprintCallable, Category="AVARIIKA|MainMenu|Diagnostics") void DevBack();
	UFUNCTION(BlueprintCallable, Category="AVARIIKA|MainMenu|Diagnostics") void DevActivateHomeAction(int32 ActionId);
	UFUNCTION(BlueprintCallable, Category="AVARIIKA|MainMenu|Diagnostics") void DevConfirmModal(bool bYes);
	UFUNCTION(BlueprintCallable, Category="AVARIIKA|MainMenu|Diagnostics") void DevSetCulture(const FString& Culture);
	UFUNCTION(BlueprintPure, Category="AVARIIKA|MainMenu|Diagnostics") FString GetDiagnosticState() const;

	UFUNCTION() void HandleAction(int32 ActionId);
	void HandleSettingAdjustment(EAvMainMenuSettingRow Row, int32 Direction);
	void RefreshLocalizedText();

private:
	void BuildTree();
	void CreateScreens();
	void ShowScreen(EAvMainMenuScreen Screen, int32 RestoreAction = INDEX_NONE);
	void OpenModal(EAvMainMenuModal Modal);
	void CloseModal(bool bRestoreFocus = true);
	void ConfirmModal(bool bYes);
	void RebuildFocusList(int32 PreferredAction = INDEX_NONE);
	void SyncFocusIndexFromSlateFocus();
	void MoveFocus(int32 Direction);
	void AdjustFocusedSetting(int32 Direction);
	void AcceptFocused();
	void ApplyFocus();
	void SetLastSafeAction(const TCHAR* Action);
	bool IsPIE() const;
	UClass* ResolveWidgetClass(const TCHAR* Path, UClass* Fallback) const;

	UPROPERTY(Transient) TObjectPtr<UWidgetSwitcher> ScreenSwitcher;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> BrandTitle;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> BrandSubtitle;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> FooterHint;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuHomeWidget> Home;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuSettingsHostWidget> Settings;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuCreditsWidget> Credits;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuConfirmDialogWidget> ModalDialog;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuCoordinator> Coordinator;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuInputRouter> InputRouter;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuSettingsAdapter> SettingsAdapter;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuSavePresenceAdapter> SavePresenceAdapter;
	UPROPERTY(Transient) TArray<TObjectPtr<UWidget>> FocusWidgets;
	UPROPERTY(Transient) TArray<int32> FocusActionIds;
	UPROPERTY(Transient) int32 FocusIndex = 0;
	UPROPERTY(Transient) int32 ModalReturnAction = INDEX_NONE;
	UPROPERTY(Transient) FString LastSafeAction = TEXT("None");
	UPROPERTY(Transient) float LastAnalogNavigationTime = -100.f;
	UPROPERTY(Transient) bool bContinueEnabled = false;
};
