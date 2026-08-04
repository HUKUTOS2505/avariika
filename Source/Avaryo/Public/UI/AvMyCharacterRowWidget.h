#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"
#include "AvMyCharacterRowWidget.generated.h"

class SBorder;
class SButton;
class SEditableTextBox;
class STextBlock;
class UAvCharacterCustomizationRootWidget;
class UTexture2D;

/** Compact data row matching the authored My Characters sample. */
UCLASS()
class AVARYO_API UAvMyCharacterRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeMyCharacterRow(
		UAvCharacterCustomizationRootWidget* InOwner,
		FName InCharacterId,
		UTexture2D* InSilhouetteTexture,
		const FString& InDisplayName,
		bool bInSelected);

	void SetSelectedState(bool bInSelected);
	void SetDisplayName(const FString& InDisplayName);
	FName GetCharacterId() const { return CharacterId; }
	bool IsSelected() const { return bSelected; }
	bool IsRenaming() const { return bRenaming; }
	bool HasRenameKeyboardFocus() const;
	bool BeginRename();
	bool CommitPendingRename();
	void CancelRename();
	void SetPendingRenameText(const FString& InDisplayName);
	FString GetPendingRenameText() const;

	UFUNCTION(BlueprintCallable, Category="AvCustomize|MyCharacters|Diagnostics")
	void TriggerSelectionForAutomation();

	UFUNCTION(BlueprintCallable, Category="AvCustomize|MyCharacters|Diagnostics")
	bool TriggerRenameForAutomation();

	UFUNCTION(BlueprintPure, Category="AvCustomize|MyCharacters|Diagnostics")
	FString GetRenameHitTestGeometryForAutomation() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual FReply NativeOnPreviewMouseButtonDown(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	FReply HandleRowClicked();
	FReply HandleNameKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent);
	void HandleNameTextChanged(const FText& NewText);
	void HandleNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	bool FinishRename(bool bCommit, const FString& CandidateName);
	void RefreshSlatePresentation();
	bool CalculateRenameHitTestMetrics(
		FVector2D& OutGeometrySize,
		FVector2D& OutMeasuredTextSize,
		float& OutVisibleWidth,
		FVector4& OutRenameHitRect,
		float& OutLayoutScale,
		float& OutApplicationScale) const;

	TWeakObjectPtr<UAvCharacterCustomizationRootWidget> CustomizationOwner;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> SilhouetteTexture;

	FName CharacterId;
	FString DisplayName;
	FString DisplayNameBeforeRename;
	bool bSelected = false;
	bool bRenaming = false;
	bool bIsClampingName = false;
	FSlateBrush BaseFrameBrush;
	FSlateBrush SelectedFrameBrush;
	FSlateBrush SilhouetteFrameBrush;
	FSlateBrush SilhouetteBrush;
	FButtonStyle RowButtonStyle;
	FEditableTextBoxStyle NameEditorStyle;
	FSlateFontInfo DisplayNameFont;
	TSharedPtr<SBorder> SelectedFrame;
	TSharedPtr<STextBlock> NameText;
	TSharedPtr<SEditableTextBox> NameEditor;
};
