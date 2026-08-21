#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "Fonts/SlateFontInfo.h"
#include "GameFramework/HUD.h"
#include "Styling/SlateTypes.h"
#include "MenuHUD.generated.h"

class AMenuHUD;
class UAvariikaOnlineSubsystem;
class AActor;
class UActorComponent;
class UAvMainMenuRootWidget;
class UAvMainMenuSavePresenceAdapter;
class UAvMainMenuSettingsAdapter;
class UButton;
class UFont;
class UProgressBar;
class URichTextBlock;
class USlider;
class UTextBlock;
class UUserWidget;
class UWidget;
class UWidgetSwitcher;

enum class EAvSettingsVisualState : uint8
{
	Normal,
	Hovered,
	Focused,
	Pressed,
	Disabled
};

/**
 * Per-row event bridge. Vendor controls expose sender-less native button and
 * slider events, so a dedicated proxy keeps the exact production row identity
 * without relying on hover/focus scans or delayed capture timers.
 */
UCLASS()
class UAvMSPSettingEventProxy final : public UObject
{
	GENERATED_BODY()
	friend class AMenuHUD;

public:
	void Initialize(AMenuHUD* InOwner, UUserWidget* InSettingWidget);

private:
	UFUNCTION() void HandlePreviousClicked();
	UFUNCTION() void HandleNextClicked();
	UFUNCTION() void HandleSliderChanged(float Value);
	UFUNCTION() void HandleComboSelectionChanged(
		FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void HandleRowHovered();
	UFUNCTION() void HandleRowUnhovered();
	UFUNCTION() void HandleRowPressed();
	UFUNCTION() void HandleRowReleased();
	void HandleRowFocused();
	void HandleRowFocusLost();

	UPROPERTY(Transient) TObjectPtr<AMenuHUD> Owner;
	UPROPERTY(Transient) TObjectPtr<UUserWidget> SettingWidget;
};

/**
 * Existing startup-map HUD plus the map-scoped authentic Menu System Pro mount.
 * The authentic framework is enabled only for /Game/Avariika/Maps/MainMenu/*;
 * the accepted legacy startup map keeps its previous Canvas behavior.
 */
UCLASS()
class AVARYO_API AMenuHUD : public AHUD
{
	GENERATED_BODY()
	friend class UAvMSPSettingEventProxy;

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DrawHUD() override;
	virtual void NotifyHitBoxClick(FName BoxName) override;
	virtual void NotifyHitBoxBeginCursorOver(FName BoxName) override;
	virtual void NotifyHitBoxEndCursorOver(FName BoxName) override;

	UAvMainMenuRootWidget* GetMainMenuRoot() const { return MainMenuRoot; }
	bool HandleAuthenticMSPInput(const FKey& Key, EInputEvent Event, float AnalogValue);

protected:
	enum class EMenuScreen : uint8 { Main, Browse };
	EMenuScreen Screen = EMenuScreen::Main;

	FName HoveredBox = NAME_None;
	bool bSearching = false;
	double SearchStartTime = 0.0;

	bool IsAuthoredShellWorld() const;
	UAvariikaOnlineSubsystem* GetOnline() const;
	void OpenSettings();
	void DrawButton(const FString& Label, float CenterX, float Y, float Width, float Height,
		FName BoxName, UFont* Font);
	bool MountAuthenticMSP();
	void RefreshAuthenticMSPBridge();
	void LocalizeAuthenticMSPWidget(UUserWidget* Widget);
	void ApplyAuthenticMSPBackground(UUserWidget* Widget);
	void ApplyAuthenticMSPSupportPolicy(UUserWidget* Widget);
	void ApplyAuthenticMSPFontPolicy(UUserWidget* Widget);
	void ApplyAuthenticMSPStyle(UUserWidget* Widget);
	void ApplyAuthenticMSPResponsiveLayout(UUserWidget* Widget);
	void InitializeAuthenticMSPSettingWidget(UUserWidget* Widget);
	void BindAuthenticMSPSettingInput(UUserWidget* Widget);
	void BindMSPSettingRowVisualState(UUserWidget* Widget);
	void RefreshMSPSettingRowVisualState(
		UUserWidget* Widget, const TCHAR* Source, bool bForce = false);
	void ApplyMSPSettingRowVisualState(
		UUserWidget* Widget, EAvSettingsVisualState State, const TCHAR* Source);
	void UpdateMSPSettingDescription(UUserWidget* Widget, const TCHAR* Source);
	void UpdateMSPSliderVisual(
		UUserWidget* Widget, const FLinearColor& TrackColor,
		const FLinearColor& FillColor);
	void UpdateMSPOptionIndicators(
		UUserWidget* Widget, EAvSettingsVisualState State,
		const FLinearColor& AccentColor);
	void HandleMSPSettingRowHovered(UUserWidget* Widget);
	void HandleMSPSettingRowUnhovered(UUserWidget* Widget);
	void HandleMSPSettingRowPressed(UUserWidget* Widget);
	void HandleMSPSettingRowReleased(UUserWidget* Widget);
	void HandleMSPSettingRowFocused(UUserWidget* Widget);
	void HandleMSPSettingRowFocusLost(UUserWidget* Widget);
	void LogMSPSettingRowHoverTarget(UUserWidget* Widget, const TCHAR* EventName);
	void AdjustMSPSettingOption(UUserWidget* Widget, int32 Delta);
	void CommitMSPSettingIndex(UUserWidget* Widget, int32 Index);
	void CommitMSPSettingScalar(UUserWidget* Widget, float Value);
	void BindMSPMouseModality(UUserWidget* Widget);
	void CaptureAuthenticMSPSettings(UUserWidget* TargetWidget = nullptr);
	void SyncAuthenticMSPSettingsFromAdapter();
	void SyncAuthenticMSPSettingRow(uint8 Row);
	void RefreshAuthenticMSPLocalization();
	UUserWidget* GetActiveMSPSubMenu() const;
	bool IsInActiveMSPSubMenu(const UWidget* Widget) const;
	void RefreshMSPApplyState();
	void OpenMSPSettingsConfirm(uint8 ConfirmKind);
	void InvokeMSPSettingsBack();
	void OpenMSPLanguage();
	void BindMSPLanguageTab(UUserWidget* Widget);
	void EnsureStableMSPSettingsRuntime();
	void PollStableMSPSettingsWarmup(int32 Generation);
	void FinalizeStableMSPSettingsRuntime();
	void CompleteStableMSPSettingsRuntimeAfterReparent(int32 Generation);
	void ApplyStableMSPSettingsRowFontsAfterFirstReveal();
	void LogStableMSPResolutionFontIdentity(const TCHAR* Source);
	void BindStableMSPSettingsTabs(UUserWidget* Widget);
	void BindStableMSPSettingsFooter(const TCHAR* Source);
	void ApplyFinalMSPHomeStyle(const TCHAR* Source);
	void RefreshStableMSPSettingsVisualState(uint8 Category);
	void SetMSPSettingsFooterVisible(bool bVisible);
	void RequestStableMSPSettingsCategory(uint8 Category);
	void ActivateStableMSPSettingsCategory(uint8 Category, bool bCountAsClick);
	void OpenStableMSPSettings();
	void CloseStableMSPSettings();
	void InitializeAuthenticMSPWidgetNow(UUserWidget* Widget);
	void LogStableMSPSettingsWidgetDump();
	void ValidateStableMSPSettingsInvariant();
	bool TransitionMSPMenu(UUserWidget* FromMenu, const TCHAR* TargetClassPath,
		const TCHAR* MenuLayer);
	void RemoveLegacyMSPJsonMirror();
	UButton* FindFirstButton(UUserWidget* Widget) const;
	USlider* FindFirstSlider(UUserWidget* Widget) const;
	void RegisterMSPHomeButton(FName OwnerName, UButton* Button);
	void ConfigureMSPHomeNavigation();
	void RefreshMSPHomeButtonVisualStates(const TCHAR* Source);
	void ApplyMSPHomeButtonVisualState(
		UButton* Button, EAvSettingsVisualState State, const TCHAR* Source);
	void RefreshMSPHomeFocus();
	void FocusMSPButton(UButton* Button);
	void MoveMSPHomeFocus(int32 Direction);
	void ActivateFocusedMSPHomeButton();
	void FocusFirstVisibleMSPSettingsButton();
	void BindMSPSettingsInteractionDiagnostics(UUserWidget* Widget);
	void LogMSPSettingsInteractionSnapshot();
	FString DescribeHoveredMSPSettingsControl() const;
	void OpenMSPCredits();
	void OpenMSPComingSoon();
	void OpenMSPExitConfirm();
	void CloseMSPOverlay(FName RestoreOwner);
	void ConfigureMSPDialog(UUserWidget* Dialog, const FText& Title, const FText& Body,
		bool bShowYes, const FText& YesLabel = FText::GetEmpty(),
		const FText& NoLabel = FText::GetEmpty());
	void TravelToGameplay(bool bOpenCustomization);

	UFUNCTION() void HandleMSPContinueClicked();
	UFUNCTION() void HandleMSPNewGameClicked();
	UFUNCTION() void HandleMSPMultiplayerClicked();
	UFUNCTION() void HandleMSPSettingsClicked();
	UFUNCTION() void HandleMSPCreditsClicked();
	UFUNCTION() void HandleMSPExitClicked();
	UFUNCTION() void HandleMSPOverlayYesClicked();
	UFUNCTION() void HandleMSPOverlayNoClicked();
	UFUNCTION() void HandleMSPApplyClicked();
	UFUNCTION() void HandleMSPResetClicked();
	UFUNCTION() void HandleMSPBackClicked();
	UFUNCTION() void HandleMSPScreenTabClicked();
	UFUNCTION() void HandleMSPGraphicsTabClicked();
	UFUNCTION() void HandleMSPAudioTabClicked();
	UFUNCTION() void HandleMSPLanguageClicked();
	UFUNCTION() void HandleMSPSettingSliderCaptureBegin();
	UFUNCTION() void HandleMSPSettingSliderCaptureEnd();
	UFUNCTION() void HandleMSPMouseModalityActivated();
	UFUNCTION() void HandleMSPHomeButtonHovered();
	UFUNCTION() void HandleMSPHomeButtonUnhovered();
	UFUNCTION() void HandleMSPHomeButtonPressed();
	UFUNCTION() void HandleMSPHomeButtonReleased();
	UFUNCTION() void HandleMSPDiagnosticHovered();
	UFUNCTION() void HandleMSPDiagnosticUnhovered();
	UFUNCTION() void HandleMSPDiagnosticPressed();
	UFUNCTION() void HandleMSPDiagnosticReleased();
	UFUNCTION() void HandleMSPDiagnosticClicked();
	UFUNCTION() void HandleMSPDiagnosticSliderChanged(float Value);
	UFUNCTION() void HandleMSPDiagnosticSliderCaptureBegin();
	UFUNCTION() void HandleMSPDiagnosticSliderCaptureEnd();

	TWeakObjectPtr<UUserWidget> SettingsWidget;

private:
	enum class EMSPOverlay : uint8
	{
		None,
		Credits,
		ComingSoon,
		ExitConfirm,
		DisplayConfirm,
		ResetConfirm,
		UnsavedConfirm
	};

	/** Deprecated authored placeholder. Retained for a later cleanup task, never mounted here. */
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuRootWidget> MainMenuRoot;

	UPROPERTY(Transient) TObjectPtr<AActor> AuthenticMSPActor;
	UPROPERTY(Transient) TObjectPtr<UActorComponent> AuthenticMSPControllerComponent;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuSavePresenceAdapter> SavePresenceAdapter;
	UPROPERTY(Transient) TObjectPtr<UAvMainMenuSettingsAdapter> SettingsAdapter;
	UPROPERTY(Transient) TObjectPtr<UUserWidget> ActiveMSPOverlay;
	UPROPERTY(Transient) TObjectPtr<UWidgetSwitcher> MSPMainSwitcher;
	UPROPERTY(Transient) TObjectPtr<UWidgetSwitcher> MSPSettingsPageSwitcher;
	UPROPERTY(Transient) TObjectPtr<UUserWidget> MSPHomeMenuWidget;
	UPROPERTY(Transient) TObjectPtr<UUserWidget> StableMSPSettingsRoot;
	UPROPERTY(Transient) TObjectPtr<UUserWidget> StableMSPScreenPage;
	UPROPERTY(Transient) TObjectPtr<UUserWidget> StableMSPGraphicsPage;
	UPROPERTY(Transient) TObjectPtr<UUserWidget> StableMSPAudioPage;
	UPROPERTY(Transient) TObjectPtr<UUserWidget> StableMSPLanguagePage;
	TWeakObjectPtr<UUserWidget> AuthenticMSPRoot;
	TWeakObjectPtr<UButton> MSPOverlayYesButton;
	TWeakObjectPtr<UButton> MSPOverlayNoButton;
	TSet<TWeakObjectPtr<UButton>> BoundMSPButtons;
	TMap<FName, TWeakObjectPtr<UButton>> MSPHomeButtons;
	TSet<TWeakObjectPtr<UUserWidget>> InitializedMSPBackgroundWidgets;
	TSet<TWeakObjectPtr<UUserWidget>> StyledMSPWidgets;
	TSet<TWeakObjectPtr<UButton>> MSPApplyButtons;
	TSet<TWeakObjectPtr<UUserWidget>> MSPApplyWidgets;
	TSet<TWeakObjectPtr<UUserWidget>> MSPSettingsMenuWidgets;
	TSet<TWeakObjectPtr<UUserWidget>> MSPLanguageMenuWidgets;
	TSet<TWeakObjectPtr<UUserWidget>> BoundMSPLanguageTabWidgets;
	TSet<TWeakObjectPtr<UUserWidget>> BoundStableMSPSettingsTabWidgets;
	TMap<TWeakObjectPtr<UUserWidget>, TWeakObjectPtr<UButton>> MSPLanguageTabButtons;
	TSet<TWeakObjectPtr<UUserWidget>> ResponsiveMSPWidgets;
	TSet<TWeakObjectPtr<UUserWidget>> InitializedMSPSettingWidgets;
	TSet<TWeakObjectPtr<UUserWidget>> InitializedMSPBridgeWidgets;
	TSet<TWeakObjectPtr<UButton>> BoundMSPSettingInputButtons;
	TSet<TWeakObjectPtr<USlider>> BoundMSPSettingInputSliders;
	TSet<TWeakObjectPtr<UComboBoxString>> BoundMSPSettingInputComboBoxes;
	TSet<TWeakObjectPtr<UButton>> BoundMSPMouseModalityButtons;
	TSet<TWeakObjectPtr<UButton>> DiagnosticMSPButtons;
	TSet<TWeakObjectPtr<USlider>> DiagnosticMSPSliders;
	TMap<TWeakObjectPtr<UUserWidget>, uint8> MSPSettingRows;
	TMap<TWeakObjectPtr<UUserWidget>, EAvSettingsVisualState> MSPSettingVisualStates;
	TSet<TWeakObjectPtr<UUserWidget>> PressedMSPSettingRows;
	TSet<TWeakObjectPtr<UUserWidget>> BoundMSPSettingVisualRows;
	TSet<TWeakObjectPtr<UUserWidget>> LoggedMSPOptionIndicatorRows;
	TSet<TWeakObjectPtr<UUserWidget>> LoggedMSPSettingHoverTargets;
	UPROPERTY(Transient) TArray<TObjectPtr<UAvMSPSettingEventProxy>> MSPSettingEventProxies;
	TMap<TWeakObjectPtr<UUserWidget>, int32> MSPLastCapturedIndices;
	TMap<TWeakObjectPtr<UUserWidget>, float> MSPLastCapturedScalars;
	TMap<TWeakObjectPtr<UUserWidget>, FString> ActiveMSPDescriptionKeys;
	TMap<TWeakObjectPtr<UTextBlock>, FSlateFontInfo> OriginalMSPTextFonts;
	TMap<TWeakObjectPtr<UTextBlock>, int32> MSPFontSetCounts;
	TMap<TWeakObjectPtr<URichTextBlock>, FTextBlockStyle> OriginalMSPRichTextStyles;
	FTimerHandle AuthenticMSPBridgeTimer;
	FTimerHandle MSPDisplayConfirmationTimer;
	TWeakObjectPtr<UButton> MSPApplyButton;
	TWeakObjectPtr<UButton> MSPResetButton;
	TWeakObjectPtr<UButton> MSPBackButton;
	TWeakObjectPtr<UButton> MSPLanguageButton;
	TWeakObjectPtr<UButton> MSPLanguageReturnFocusButton;
	TWeakObjectPtr<UUserWidget> MSPApplyWidget;
	TWeakObjectPtr<UUserWidget> MSPFooterBarWidget;
	TWeakObjectPtr<UUserWidget> MSPSettingsMenuWidget;
	TWeakObjectPtr<UButton> MSPSettingsReturnFocusButton;
	TWeakObjectPtr<UButton> MSPHoveredHomeButton;
	TWeakObjectPtr<UButton> MSPPressedHomeButton;
	FString MSPLanguagePreviousSubMenuClassPath;
	FString ActiveMSPVisualSubMenuClassPath;
	FName PendingMSPHomeFocusOwner = NAME_None;
	EMSPOverlay ActiveMSPOverlayKind = EMSPOverlay::None;
	int32 MSPHomeFocusIndex = INDEX_NONE;
	double LastMSPAnalogNavigationTime = -1.0;
	double LastMSPSettingsDiagnosticSnapshotTime = -1.0;
	FString LastMSPSettingsDiagnosticFocus;
	FString LastMSPSettingsInvariantSignature;
	int32 MSPSettingsDiagnosticBridgePasses = 0;
	int32 MSPSettingsDiagnosticCaptureCalls = 0;
	int32 MSPSettingsDiagnosticSyncCalls = 0;
	int32 MSPSettingsDiagnosticFocusCalls = 0;
	int32 MSPSettingsDiagnosticPendingChanges = 0;
	int32 MSPSettingsWarmupGeneration = 0;
	int32 MSPSettingsWarmupCategory = 0;
	int32 MSPSettingsWarmupStableFrames = 0;
	int32 MSPSettingsWarmupAttempts = 0;
	int32 MSPSettingsActiveCategory = 0;
	int32 MSPSettingsTabClickSerial = 0;
	int32 MSPSettingsLogicalTransitions = 0;
	int32 MSPSettingsDuplicateBindings = 0;
	int32 MSPSettingsStaleCallbacks = 0;
	int32 MSPFooterBindingPasses = 0;
	int32 MSPApplyMouseClicks = 0;
	int32 MSPResetMouseClicks = 0;
	int32 MSPBackMouseClicks = 0;
	bool bMSPOverlayYesFocused = false;
	bool bActiveMSPOverlayOwnedByRouter = false;
	bool bMSPSettingsOpen = false;
	bool bMSPLanguageOpen = false;
	bool bSynchronizingSettingsUI = false;
	bool bMSPSliderDragging = false;
	bool bMSPMouseInputActive = false;
	bool bFocusMSPSettingsAfterLanguageBack = false;
	bool bReturnHomeAfterMSPApply = false;
	bool bStableMSPSettingsWarmupStarted = false;
	bool bStableMSPSettingsReady = false;
	bool bStableMSPInitialRowFontPassComplete = false;
	bool bOpenStableMSPSettingsWhenReady = false;
	bool bLoggedStableMSPSettingsDump = false;
	bool bAuthenticMSPMap = false;
	bool bReportedDuplicateMSPRoot = false;
	bool bLoggedMSPHomeInitialNoSelection = false;
	bool bOwnsAuthenticMSPControllerComponent = false;
	bool bMSPSettingsInteractionDiagnostics = false;
};
