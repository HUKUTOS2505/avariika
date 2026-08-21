#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/Widget.h"
#include "Styling/SlateBrush.h"
#include "TimerManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/UniformGridSlot.h"
#include "Components/WorkerAppearanceComponent.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "AvCustomizeWidgets.generated.h"

class AAvaryoCharacter;
class AAvCustomizePreviewActor;
class USkeletalMesh;
class UTextureRenderTarget2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAvCustomizeIndexClicked, int32, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAvCustomizeActionClicked);

UCLASS(BlueprintType, Blueprintable)
class AVARYO_API UAvCategoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(int32 InIndex, const FText& InLabel, bool bInSelected);
	void SetSelected(bool bInSelected);

	UFUNCTION(BlueprintCallable, Category="AvCustomize")
	void SetIconBrush(const FSlateBrush& InBrush);

	UPROPERTY(BlueprintAssignable, Category="AvCustomize")
	FAvCustomizeIndexClicked OnCategoryClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleClicked();

	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();

	void EnsureBuilt();
	void ApplyVisuals();
	void StartVisualTransition(float InTargetOpacity, FVector2D InTargetScale, float InDuration);
	void StepVisualTransition();
	bool IsVisualTarget(float InTargetOpacity, const FVector2D& InTargetScale) const;

	UPROPERTY(Transient)
	TObjectPtr<UButton> Button;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> Fill;

	UPROPERTY(Transient)
	TObjectPtr<UImage> IconImage;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LabelText;

	int32 Index = INDEX_NONE;
	bool bSelected = false;
	bool bHovered = false;
	bool bVisualInitialized = false;
	float CurrentOpacity = 1.f;
	float StartOpacity = 1.f;
	float TargetOpacity = 1.f;
	FVector2D CurrentScale = FVector2D(1.f, 1.f);
	FVector2D StartScale = FVector2D(1.f, 1.f);
	FVector2D TargetScale = FVector2D(1.f, 1.f);
	float TransitionStartTime = 0.f;
	float TransitionDuration = 0.14f;
	FTimerHandle VisualTransitionTimer;
};

UCLASS(BlueprintType, Blueprintable)
class AVARYO_API UAvOptionCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(int32 InIndex, const FText& InLabel, bool bInSelected);

	UFUNCTION(BlueprintCallable, Category="AvCustomize")
	void SetIconBrush(const FSlateBrush& InBrush);

	UPROPERTY(BlueprintAssignable, Category="AvCustomize")
	FAvCustomizeIndexClicked OnOptionClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleClicked();

	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();

	void EnsureBuilt();
	void ApplyVisuals();
	void StartVisualTransition(float InTargetOpacity, FVector2D InTargetScale, FVector2D InTargetTranslation, float InDuration);
	void StepVisualTransition();
	bool IsVisualTarget(float InTargetOpacity, const FVector2D& InTargetScale, const FVector2D& InTargetTranslation) const;

	UPROPERTY(Transient)
	TObjectPtr<UButton> Button;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> Fill;

	UPROPERTY(Transient)
	TObjectPtr<UImage> IconImage;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LabelText;

	int32 Index = INDEX_NONE;
	bool bSelected = false;
	bool bHovered = false;
	bool bVisualInitialized = false;
	float CurrentOpacity = 1.f;
	float StartOpacity = 1.f;
	float TargetOpacity = 1.f;
	FVector2D CurrentScale = FVector2D(1.f, 1.f);
	FVector2D StartScale = FVector2D(1.f, 1.f);
	FVector2D TargetScale = FVector2D(1.f, 1.f);
	FVector2D CurrentTranslation = FVector2D::ZeroVector;
	FVector2D StartTranslation = FVector2D::ZeroVector;
	FVector2D TargetTranslation = FVector2D::ZeroVector;
	float TransitionStartTime = 0.f;
	float TransitionDuration = 0.14f;
	FTimerHandle VisualTransitionTimer;
};

UCLASS(BlueprintType, Blueprintable)
class AVARYO_API UAvColorSwatchWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(int32 InIndex, FLinearColor InColor, bool bInSelected);

	UPROPERTY(BlueprintAssignable, Category="AvCustomize")
	FAvCustomizeIndexClicked OnSwatchClicked;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleClicked();

	void EnsureBuilt();
	void ApplyVisuals();

	UPROPERTY(Transient)
	TObjectPtr<UButton> Button;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> Fill;

	int32 Index = INDEX_NONE;
	FLinearColor Color = FLinearColor::White;
	bool bSelected = false;
};

UCLASS(BlueprintType, Blueprintable)
class AVARYO_API UAvActionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const FText& InLabel, bool bInPrimary);

	UFUNCTION(BlueprintCallable, Category="AvCustomize")
	void SetIconBrush(const FSlateBrush& InBrush);

	UPROPERTY(BlueprintAssignable, Category="AvCustomize")
	FAvCustomizeActionClicked OnActionClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleClicked();

	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();

	UFUNCTION()
	void HandlePressed();

	UFUNCTION()
	void HandleReleased();

	void EnsureBuilt();
	void ApplyVisuals();
	void StartVisualTransition(float InTargetOpacity, FVector2D InTargetScale, float InDuration);
	void StepVisualTransition();
	bool IsVisualTarget(float InTargetOpacity, const FVector2D& InTargetScale) const;

	UPROPERTY(Transient)
	TObjectPtr<UButton> Button;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> Fill;

	UPROPERTY(Transient)
	TObjectPtr<UImage> IconImage;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LabelText;

	FText Label;
	bool bPrimary = false;
	bool bHovered = false;
	bool bPressed = false;
	bool bVisualInitialized = false;
	float CurrentOpacity = 1.f;
	float StartOpacity = 1.f;
	float TargetOpacity = 1.f;
	FVector2D CurrentScale = FVector2D(1.f, 1.f);
	FVector2D StartScale = FVector2D(1.f, 1.f);
	FVector2D TargetScale = FVector2D(1.f, 1.f);
	float TransitionStartTime = 0.f;
	float TransitionDuration = 0.14f;
	FTimerHandle VisualTransitionTimer;
};

UCLASS(BlueprintType, Blueprintable)
class AVARYO_API UAvCustomizeRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="AvCustomize")
	void RefreshFromCharacter();

	void EnsureBuiltForViewport();

	struct FSlotRow
	{
		EWorkerSlot Slot;
		const TCHAR* Command;
	};

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	struct FSelectedCustomizationState
	{
		int32 SelectedSlotIndex = 0;
		int32 OptionPage = 0;
		int32 SelectedColorIndex = 0;
		FWorkerAppearance Appearance;
		TArray<FString> CurrentOptions;
	};

	float OpenedAtTime = 0.f;
	bool bLayoutBuilt = false;

	void BuildLayout();
	void RebuildCategories();
	void RefreshCategoryFocus();
	void RebuildOptions();
	void BuildEquipmentBar(UVerticalBox* Parent);
	void AddTopTabs(UVerticalBox* Parent);
	void BuildPreviewPanel(UHorizontalBox* Parent);
	FText DisplayMeshName(const FString& Name) const;
	AAvaryoCharacter* GetAvCharacter() const;
	void SyncStateFromCharacter();
	void ApplyStateToPreview();
	void EnsurePreview();
	bool ResolveMeshForSlot(EWorkerSlot WorkerSlot, const FString& Option, TSoftObjectPtr<USkeletalMesh>& OutMesh) const;
	EWorkerSlot GetSelectedSlot() const;
	const TCHAR* GetSelectedCommand() const;

	UFUNCTION()
	void HandleCategoryClicked(int32 Index);

	UFUNCTION()
	void HandleOptionClicked(int32 Index);

	UFUNCTION()
	void HandleColorClicked(int32 Index);

	UFUNCTION()
	void HandlePrevPageClicked();

	UFUNCTION()
	void HandleNextPageClicked();

	UFUNCTION()
	void HandleSaveClicked();

	UFUNCTION()
	void HandleApplyClicked();

	UFUNCTION()
	void HandleCloseClicked();

	UPROPERTY(Transient)
	TObjectPtr<UScrollBox> CategoryScroll;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UAvCategoryButtonWidget>> CategoryButtons;

	UPROPERTY(Transient)
	TObjectPtr<UUniformGridPanel> OptionGrid;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CategoryTitle;

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> RightPanel;

	UPROPERTY(Transient)
	TObjectPtr<UImage> PreviewImage;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;

	UPROPERTY(Transient)
	TObjectPtr<AAvCustomizePreviewActor> PreviewActor;

	FSelectedCustomizationState SelectedCustomizationState;
};
