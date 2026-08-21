#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WorkerAppearanceComponent.h"
#include "AvHeadCatalogItemButton.generated.h"

class UAvCharacterCustomizationRootWidget;
class UBorder;
class UButton;
class UImage;
class UTextBlock;
class UTexture2D;

/** Reusable payload-aware Widget Blueprint base for every dynamic item on the Head page. */
UCLASS(BlueprintType, Blueprintable)
class AVARYO_API UAvHeadCatalogItemButton : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeHeadCatalogButton(
		UAvCharacterCustomizationRootWidget* InOwner,
		EAvHeadCustomizationSection InSection,
		int32 InCatalogIndex,
		bool bInNoneCard,
		UTexture2D* InThumbnailTexture,
		const FText& InDisplayName,
		bool bInDisabled,
		const FText& InTooltip,
		const FString& InExactAssetPath);
	void InitializeFaceProtectionCatalogButton(
		UAvCharacterCustomizationRootWidget* InOwner,
		EAvFaceProtectionSection InSection,
		int32 InCatalogIndex,
		bool bInNoneCard,
		UTexture2D* InThumbnailTexture,
		const FText& InDisplayName,
		bool bInDisabled,
		const FText& InTooltip,
		const FString& InExactAssetPath);
	void InitializeHandsAccessoriesCatalogButton(
		UAvCharacterCustomizationRootWidget* InOwner,
		EAvHandsAccessoriesSection InSection,
		int32 InCatalogIndex,
		bool bInNoneCard,
		UTexture2D* InThumbnailTexture,
		const FText& InDisplayName,
		bool bInDisabled,
		const FText& InTooltip,
		const FString& InExactAssetPath);
	void InitializeTorsoCatalogButton(
		UAvCharacterCustomizationRootWidget* InOwner, int32 InCatalogIndex,
		bool bInNoneCard, UTexture2D* InThumbnailTexture, const FText& InDisplayName,
		bool bInDisabled, const FText& InTooltip, const FString& InExactAssetPath);
	void InitializeLowerHipCatalogButton(
		UAvCharacterCustomizationRootWidget* InOwner, EAvLowerHipSection InSection,
		int32 InCatalogIndex, bool bInNoneCard, UTexture2D* InThumbnailTexture,
		const FText& InDisplayName, bool bInDisabled, const FText& InTooltip,
		const FString& InExactAssetPath);
	void InitializeFullOutfitCatalogButton(
		UAvCharacterCustomizationRootWidget* InOwner, int32 InCatalogIndex,
		bool bInNoneCard, UTexture2D* InThumbnailTexture, const FText& InDisplayName,
		bool bInDisabled, const FText& InTooltip, const FString& InExactAssetPath);

	void SetSelectedState(bool bSelected);
	int32 GetCatalogIndex() const { return CatalogIndex; }
	bool IsNoneCatalogCard() const { return bNoneCard; }
	UTexture2D* GetThumbnailTextureForDiagnostics() const { return ThumbnailTexture.Get(); }
	UObject* GetThumbnailBrushResourceForDiagnostics() const;
	void SetDesignTimePreview(
		const FText& InDisplayName,
		UTexture2D* InThumbnailTexture,
		bool bInSelected);

	/** Generic automation entry point; it traverses the same handler as an actual click. */
	UFUNCTION(BlueprintCallable, Category="AvCustomize|Head|Diagnostics")
	void TriggerCatalogSelectionForAutomation();

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleHeadCatalogClicked();

	void ApplyPresentation();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_CardFrame;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_CardTitle;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_CardThumbnail;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_CardSelectionCircle;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_CardSelectedCheck;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_CardHitTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AvCustomize|Head|Designer", meta=(AllowPrivateAccess="true"))
	FText DesignerDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AvCustomize|Head|Designer", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTexture2D> DesignerThumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AvCustomize|Head|Designer", meta=(AllowPrivateAccess="true"))
	bool bDesignerSelected = false;

	TWeakObjectPtr<UAvCharacterCustomizationRootWidget> CustomizationOwner;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> ThumbnailTexture;

	FText DisplayName;
	FText Tooltip;
	FString ExactAssetPath;
	EAvHeadCustomizationSection Section = EAvHeadCustomizationSection::HeadType;
	EAvFaceProtectionSection FaceProtectionSection = EAvFaceProtectionSection::Glasses;
	EAvHandsAccessoriesSection HandsAccessoriesSection = EAvHandsAccessoriesSection::Gloves;
	EAvLowerHipSection LowerHipSection = EAvLowerHipSection::Legs;
	int32 CatalogIndex = INDEX_NONE;
	bool bFaceProtectionPayload = false;
	bool bHandsAccessoriesPayload = false;
	bool bTorsoPayload = false;
	bool bLowerHipPayload = false;
	bool bFullOutfitPayload = false;
	bool bNoneCard = false;
	bool bDisabled = false;
	bool bSelected = false;
};
