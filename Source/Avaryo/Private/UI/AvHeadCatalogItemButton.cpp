#include "UI/AvHeadCatalogItemButton.h"

#include "AvariikaLoc.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateTypes.h"
#include "UI/AvCharacterCustomizationRootWidget.h"

namespace AvHeadCatalogCard
{
	const FLinearColor SelectedAccent(0.95f, 0.42f, 0.04f, 1.f);
	const FLinearColor CardBackground(0.035f, 0.042f, 0.055f, 0.98f);
	const FLinearColor UnselectedFrame(0.25f, 0.27f, 0.30f, 1.f);
	const FLinearColor UnselectedIndicator(0.48f, 0.50f, 0.54f, 1.f);
	const FLinearColor SelectedText(1.f, 0.57f, 0.12f, 1.f);
	const FLinearColor UnselectedText(0.89f, 0.90f, 0.92f, 1.f);

	FSlateBrush MakeInvisibleHitTargetBrush()
	{
		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
		Brush.TintColor = FSlateColor(FLinearColor::Transparent);
		return Brush;
	}
}

void UAvHeadCatalogItemButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime())
	{
		DisplayName = DesignerDisplayName.IsEmpty()
			? FAvLoc::Text(TEXT("Customization.Item.Head.Type01"))
			: DesignerDisplayName;
		ThumbnailTexture = DesignerThumbnail;
		bDisabled = false;
		bSelected = bDesignerSelected;
		Tooltip = DisplayName;
		ApplyPresentation();
	}
}

void UAvHeadCatalogItemButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_CardHitTarget)
	{
		Btn_CardHitTarget->IsFocusable = false;
		Btn_CardHitTarget->OnClicked.RemoveAll(this);
		Btn_CardHitTarget->OnClicked.AddDynamic(
			this, &UAvHeadCatalogItemButton::HandleHeadCatalogClicked);
	}
	ApplyPresentation();
}

void UAvHeadCatalogItemButton::InitializeHeadCatalogButton(
	UAvCharacterCustomizationRootWidget* InOwner,
	EAvHeadCustomizationSection InSection,
	int32 InCatalogIndex,
	bool bInNoneCard,
	UTexture2D* InThumbnailTexture,
	const FText& InDisplayName,
	bool bInDisabled,
	const FText& InTooltip,
	const FString& InExactAssetPath)
{
	CustomizationOwner = InOwner;
	bFaceProtectionPayload = false;
	bHandsAccessoriesPayload = false;
	bTorsoPayload = false;
	bLowerHipPayload = false;
	bFullOutfitPayload = false;
	Section = InSection;
	CatalogIndex = InCatalogIndex;
	bNoneCard = bInNoneCard;
	ThumbnailTexture = InThumbnailTexture;
	DisplayName = InDisplayName;
	bDisabled = bInDisabled;
	Tooltip = InTooltip;
	ExactAssetPath = InExactAssetPath;
	ApplyPresentation();
}

void UAvHeadCatalogItemButton::InitializeFaceProtectionCatalogButton(
	UAvCharacterCustomizationRootWidget* InOwner,
	EAvFaceProtectionSection InSection,
	int32 InCatalogIndex,
	bool bInNoneCard,
	UTexture2D* InThumbnailTexture,
	const FText& InDisplayName,
	bool bInDisabled,
	const FText& InTooltip,
	const FString& InExactAssetPath)
{
	CustomizationOwner = InOwner;
	bFaceProtectionPayload = true;
	bHandsAccessoriesPayload = false;
	bTorsoPayload = false;
	bLowerHipPayload = false;
	bFullOutfitPayload = false;
	FaceProtectionSection = InSection;
	CatalogIndex = InCatalogIndex;
	bNoneCard = bInNoneCard;
	ThumbnailTexture = InThumbnailTexture;
	DisplayName = InDisplayName;
	bDisabled = bInDisabled;
	Tooltip = InTooltip;
	ExactAssetPath = InExactAssetPath;
	ApplyPresentation();
}

void UAvHeadCatalogItemButton::InitializeHandsAccessoriesCatalogButton(
	UAvCharacterCustomizationRootWidget* InOwner,
	EAvHandsAccessoriesSection InSection,
	int32 InCatalogIndex,
	bool bInNoneCard,
	UTexture2D* InThumbnailTexture,
	const FText& InDisplayName,
	bool bInDisabled,
	const FText& InTooltip,
	const FString& InExactAssetPath)
{
	CustomizationOwner = InOwner;
	bFaceProtectionPayload = false;
	bHandsAccessoriesPayload = true;
	bTorsoPayload = false;
	bLowerHipPayload = false;
	bFullOutfitPayload = false;
	HandsAccessoriesSection = InSection;
	CatalogIndex = InCatalogIndex;
	bNoneCard = bInNoneCard;
	ThumbnailTexture = InThumbnailTexture;
	DisplayName = InDisplayName;
	bDisabled = bInDisabled;
	Tooltip = InTooltip;
	ExactAssetPath = InExactAssetPath;
	ApplyPresentation();
}

void UAvHeadCatalogItemButton::InitializeTorsoCatalogButton(
	UAvCharacterCustomizationRootWidget* InOwner, int32 InCatalogIndex,
	bool bInNoneCard, UTexture2D* InThumbnailTexture, const FText& InDisplayName,
	bool bInDisabled, const FText& InTooltip, const FString& InExactAssetPath)
{
	CustomizationOwner = InOwner;
	bFaceProtectionPayload = false;
	bHandsAccessoriesPayload = false;
	bTorsoPayload = true;
	bLowerHipPayload = false;
	bFullOutfitPayload = false;
	CatalogIndex = InCatalogIndex;
	bNoneCard = bInNoneCard;
	ThumbnailTexture = InThumbnailTexture;
	DisplayName = InDisplayName;
	bDisabled = bInDisabled;
	Tooltip = InTooltip;
	ExactAssetPath = InExactAssetPath;
	ApplyPresentation();
}

void UAvHeadCatalogItemButton::InitializeLowerHipCatalogButton(
	UAvCharacterCustomizationRootWidget* InOwner, EAvLowerHipSection InSection,
	int32 InCatalogIndex, bool bInNoneCard, UTexture2D* InThumbnailTexture,
	const FText& InDisplayName, bool bInDisabled, const FText& InTooltip,
	const FString& InExactAssetPath)
{
	CustomizationOwner = InOwner;
	bFaceProtectionPayload = false;
	bHandsAccessoriesPayload = false;
	bTorsoPayload = false;
	bLowerHipPayload = true;
	bFullOutfitPayload = false;
	LowerHipSection = InSection;
	CatalogIndex = InCatalogIndex;
	bNoneCard = bInNoneCard;
	ThumbnailTexture = InThumbnailTexture;
	DisplayName = InDisplayName;
	bDisabled = bInDisabled;
	Tooltip = InTooltip;
	ExactAssetPath = InExactAssetPath;
	ApplyPresentation();
}

void UAvHeadCatalogItemButton::InitializeFullOutfitCatalogButton(
	UAvCharacterCustomizationRootWidget* InOwner, int32 InCatalogIndex,
	bool bInNoneCard, UTexture2D* InThumbnailTexture, const FText& InDisplayName,
	bool bInDisabled, const FText& InTooltip, const FString& InExactAssetPath)
{
	CustomizationOwner = InOwner;
	bFaceProtectionPayload = false;
	bHandsAccessoriesPayload = false;
	bTorsoPayload = false;
	bLowerHipPayload = false;
	bFullOutfitPayload = true;
	CatalogIndex = InCatalogIndex;
	bNoneCard = bInNoneCard;
	ThumbnailTexture = InThumbnailTexture;
	DisplayName = InDisplayName;
	bDisabled = bInDisabled;
	Tooltip = InTooltip;
	ExactAssetPath = InExactAssetPath;
	ApplyPresentation();
}

void UAvHeadCatalogItemButton::SetDesignTimePreview(
	const FText& InDisplayName,
	UTexture2D* InThumbnailTexture,
	bool bInSelected)
{
	DesignerDisplayName = InDisplayName;
	DesignerThumbnail = InThumbnailTexture;
	bDesignerSelected = bInSelected;

	if (IsDesignTime())
	{
		DisplayName = InDisplayName;
		ThumbnailTexture = InThumbnailTexture;
		bDisabled = false;
		bSelected = bInSelected;
		Tooltip = InDisplayName;
		ApplyPresentation();
	}
}

void UAvHeadCatalogItemButton::ApplyPresentation()
{
	if (Txt_CardTitle)
	{
		Txt_CardTitle->SetAutoWrapText(false);
		Txt_CardTitle->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
		Txt_CardTitle->SetClipping(EWidgetClipping::ClipToBounds);
		Txt_CardTitle->SetText(DisplayName);
		Txt_CardTitle->SetToolTipText(DisplayName);
	}
	if (Img_CardThumbnail)
	{
		Img_CardThumbnail->SetColorAndOpacity(FLinearColor::White);
		Img_CardThumbnail->SetRenderOpacity(1.f);
		Img_CardThumbnail->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (ThumbnailTexture)
		{
			Img_CardThumbnail->SetBrushFromTexture(ThumbnailTexture, true);
			FSlateBrush PortraitBrush = Img_CardThumbnail->GetBrush();
			PortraitBrush.DrawAs = ESlateBrushDrawType::Image;
			PortraitBrush.TintColor = FSlateColor(FLinearColor::White);
			PortraitBrush.SetResourceObject(ThumbnailTexture);
			Img_CardThumbnail->SetBrush(PortraitBrush);
		}
		else
		{
			// Runtime cards must never inherit the reusable WBP's Type01 Designer preview.
			// A missing exact product thumbnail is presented as empty and logged by the owner.
			FSlateBrush EmptyBrush;
			EmptyBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
			EmptyBrush.SetResourceObject(nullptr);
			Img_CardThumbnail->SetBrush(EmptyBrush);
		}
	}

	SetToolTipText(Tooltip);
	SetIsEnabled(!bDisabled);
	SetRenderOpacity(bDisabled ? 0.42f : 1.f);
	if (Btn_CardHitTarget)
	{
		// The button is only the input surface. Authored Hovered/Pressed brushes used to
		// paint a full-card white/orange overlay and produced clipped side glow artifacts.
		const FSlateBrush InvisibleBrush = AvHeadCatalogCard::MakeInvisibleHitTargetBrush();
		FButtonStyle HitTargetStyle = Btn_CardHitTarget->GetStyle();
		HitTargetStyle.SetNormal(InvisibleBrush);
		HitTargetStyle.SetHovered(InvisibleBrush);
		HitTargetStyle.SetPressed(InvisibleBrush);
		HitTargetStyle.SetDisabled(InvisibleBrush);
		HitTargetStyle.SetNormalPadding(FMargin(0.f));
		HitTargetStyle.SetPressedPadding(FMargin(0.f));
		Btn_CardHitTarget->SetStyle(HitTargetStyle);
		Btn_CardHitTarget->SetBackgroundColor(FLinearColor::White);
		Btn_CardHitTarget->SetColorAndOpacity(FLinearColor::White);
		Btn_CardHitTarget->SetToolTipText(Tooltip);
		Btn_CardHitTarget->SetIsEnabled(!bDisabled);
	}
	SetSelectedState(bSelected);
}

void UAvHeadCatalogItemButton::SetSelectedState(bool bInSelected)
{
	bSelected = bInSelected;
	if (Border_CardFrame)
	{
		// Border_CardFrame is a full-card Overlay sibling. Tinting its Image brush
		// orange paints the entire card, including the thumbnail area. Keep its fill
		// neutral and express selection only through the rounded-box outline.
		FSlateBrush FrameBrush = Border_CardFrame->Background;
		FrameBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
		FrameBrush.TintColor = FSlateColor(AvHeadCatalogCard::CardBackground);
		FrameBrush.OutlineSettings.Width = bSelected ? 2.f : 1.f;
		FrameBrush.OutlineSettings.Color = FSlateColor(
			bSelected
				? AvHeadCatalogCard::SelectedAccent
				: AvHeadCatalogCard::UnselectedFrame);
		Border_CardFrame->SetBrush(FrameBrush);
		Border_CardFrame->SetBrushColor(FLinearColor::White);
	}
	if (Border_CardSelectionCircle)
	{
		FSlateBrush CircleBrush = Border_CardSelectionCircle->Background;
		CircleBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
		CircleBrush.OutlineSettings.CornerRadii = FVector4(14.f, 14.f, 14.f, 14.f);
		CircleBrush.OutlineSettings.Width = 1.f;
		CircleBrush.TintColor = FSlateColor(
			bSelected ? AvHeadCatalogCard::SelectedAccent : FLinearColor::Transparent);
		CircleBrush.OutlineSettings.Color = FSlateColor(
			bSelected
				? AvHeadCatalogCard::SelectedAccent
				: AvHeadCatalogCard::UnselectedIndicator);
		Border_CardSelectionCircle->SetBrush(CircleBrush);
		Border_CardSelectionCircle->SetBrushColor(FLinearColor::White);
	}
	if (Txt_CardSelectedCheck)
	{
		Txt_CardSelectedCheck->SetVisibility(
			bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (Txt_CardTitle)
	{
		Txt_CardTitle->SetColorAndOpacity(FSlateColor(
			bSelected
				? AvHeadCatalogCard::SelectedText
				: AvHeadCatalogCard::UnselectedText));
	}
}

UObject* UAvHeadCatalogItemButton::GetThumbnailBrushResourceForDiagnostics() const
{
	return Img_CardThumbnail ? Img_CardThumbnail->GetBrush().GetResourceObject() : nullptr;
}

void UAvHeadCatalogItemButton::TriggerCatalogSelectionForAutomation()
{
	HandleHeadCatalogClicked();
}

void UAvHeadCatalogItemButton::HandleHeadCatalogClicked()
{
	if (!bDisabled)
	{
		if (UAvCharacterCustomizationRootWidget* Owner = CustomizationOwner.Get())
		{
			if (bFullOutfitPayload)
			{
				Owner->HandleFullOutfitCatalogItemClicked(CatalogIndex, bNoneCard);
			}
			else if (bLowerHipPayload)
			{
				Owner->HandleLowerHipCatalogItemClicked(
					LowerHipSection, CatalogIndex, bNoneCard);
			}
			else if (bTorsoPayload)
			{
				Owner->HandleTorsoCatalogItemClicked(CatalogIndex, bNoneCard);
			}
			else if (bHandsAccessoriesPayload)
			{
				Owner->HandleHandsAccessoriesCatalogItemClicked(
					HandsAccessoriesSection, CatalogIndex, bNoneCard);
			}
			else if (bFaceProtectionPayload)
			{
				Owner->HandleFaceProtectionCatalogItemClicked(
					FaceProtectionSection, CatalogIndex, bNoneCard);
			}
			else
			{
				Owner->HandleHeadCatalogItemClicked(Section, CatalogIndex, bNoneCard);
			}
		}
	}
}
