#include "UI/AvCustomizeWidgets.h"

#include "AvariikaLoc.h"

#include "AvaryoCharacter.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Components/WorkerAppearanceComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Game/AvaryoPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"
#include "UI/AvCustomizePreviewActor.h"

namespace AvCustomizeStyle
{
	const FLinearColor Bg(0.008f, 0.010f, 0.012f, 0.96f);
	const FLinearColor Panel(0.020f, 0.026f, 0.032f, 0.96f);
	const FLinearColor PanelActive(0.038f, 0.046f, 0.054f, 0.98f);
	const FLinearColor PanelLight(0.050f, 0.060f, 0.070f, 0.96f);
	const FLinearColor PanelHover(0.090f, 0.078f, 0.060f, 0.98f);
	const FLinearColor Accent(0.95f, 0.42f, 0.04f, 1.f);
	const FLinearColor AccentDim(0.44f, 0.18f, 0.03f, 1.f);
	const FLinearColor AccentGlow(0.72f, 0.30f, 0.055f, 1.f);
	const FLinearColor Text(0.92f, 0.90f, 0.86f, 1.f);
	const FLinearColor TextDim(0.58f, 0.60f, 0.62f, 1.f);

	void StyleText(UTextBlock* TextBlock, float Size, const FLinearColor& Color)
	{
		if (!TextBlock)
		{
			return;
		}
		FSlateFontInfo Font = TextBlock->GetFont();
		Font.Size = FMath::RoundToInt(Size);
		TextBlock->SetFont(Font);
		TextBlock->SetColorAndOpacity(Color);
	}

	void HideEmptyIcon(UImage* Image)
	{
		if (Image)
		{
			Image->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	float EaseUi(float Alpha)
	{
		return FMath::InterpEaseOut(0.f, 1.f, FMath::Clamp(Alpha, 0.f, 1.f), 2.35f);
	}
}

static const UAvCustomizeRootWidget::FSlotRow CustomizeSlots[] =
{
	{ EWorkerSlot::Body, TEXT("body") },
	{ EWorkerSlot::Head, TEXT("head") },
	{ EWorkerSlot::Hair, TEXT("hair") },
	{ EWorkerSlot::Beard, TEXT("beard") },
	{ EWorkerSlot::Torso, TEXT("torso") },
	{ EWorkerSlot::Legs, TEXT("legs") },
	{ EWorkerSlot::Headgear, TEXT("headgear") },
	{ EWorkerSlot::FaceMask, TEXT("face") },
	{ EWorkerSlot::Glasses, TEXT("glasses") },
	{ EWorkerSlot::Headphones, TEXT("headphones") },
	{ EWorkerSlot::Gloves, TEXT("gloves") },
	{ EWorkerSlot::Watch, TEXT("watch") },
	{ EWorkerSlot::HipAccessory, TEXT("hip") },
	{ EWorkerSlot::FullOutfit, TEXT("fulloutfit") },
	{ EWorkerSlot::Overalls, TEXT("overalls") },
	{ EWorkerSlot::Vest, TEXT("vest") },
	{ EWorkerSlot::Feet, TEXT("feet") },
};

static constexpr int32 OptionsPerPage = 6;

static FText GetCustomizeSlotLabel(int32 SlotIndex)
{
	const int32 SafeIndex = FMath::Clamp(SlotIndex, 0, static_cast<int32>(UE_ARRAY_COUNT(CustomizeSlots)) - 1);
	switch (CustomizeSlots[SafeIndex].Slot)
	{
	case EWorkerSlot::Body: return FAvLoc::Text(TEXT("Customization.Section.Body"));
	case EWorkerSlot::Head: return FAvLoc::Text(TEXT("Customization.Category.Head"));
	case EWorkerSlot::Hair: return FAvLoc::Text(TEXT("Customization.Section.Hair"));
	case EWorkerSlot::Beard: return FAvLoc::Text(TEXT("Customization.Section.Beard"));
	case EWorkerSlot::Torso: return FAvLoc::Text(TEXT("Customization.Section.Torso"));
	case EWorkerSlot::Legs: return FAvLoc::Text(TEXT("Customization.Section.Lower"));
	case EWorkerSlot::Headgear: return FAvLoc::Text(TEXT("Customization.Section.Helmets"));
	case EWorkerSlot::FaceMask: return FAvLoc::Text(TEXT("Customization.Section.Masks"));
	case EWorkerSlot::Glasses: return FAvLoc::Text(TEXT("Customization.Preset.Sample.Face"));
	case EWorkerSlot::Headphones: return FAvLoc::Text(TEXT("Customization.Section.Headphones"));
	case EWorkerSlot::Gloves: return FAvLoc::Text(TEXT("Customization.Section.Gloves"));
	case EWorkerSlot::Watch: return FAvLoc::Text(TEXT("Customization.Section.Watches"));
	case EWorkerSlot::HipAccessory: return FAvLoc::Text(TEXT("Customization.Section.HipAccessory"));
	case EWorkerSlot::FullOutfit: return FAvLoc::Text(TEXT("Customization.Section.FullOutfit"));
	case EWorkerSlot::Overalls: return FAvLoc::Text(TEXT("Customization.Section.Overalls"));
	case EWorkerSlot::Vest: return FAvLoc::Text(TEXT("Customization.Section.Vest"));
	case EWorkerSlot::Feet: return FAvLoc::Text(TEXT("Customization.Section.Feet"));
	default: return FAvLoc::Text(TEXT("Customization.Legacy.Slot"));
	}
}

void UAvCategoryButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureBuilt();
	ApplyVisuals();
}

void UAvCategoryButtonWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(VisualTransitionTimer);
	}
	Super::NativeDestruct();
}

void UAvCategoryButtonWidget::Setup(int32 InIndex, const FText& InLabel, bool bInSelected)
{
	Index = InIndex;
	bSelected = bInSelected;
	EnsureBuilt();
	if (LabelText)
	{
		LabelText->SetText(InLabel);
	}
	ApplyVisuals();
}

void UAvCategoryButtonWidget::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;
	ApplyVisuals();
}

void UAvCategoryButtonWidget::SetIconBrush(const FSlateBrush& InBrush)
{
	EnsureBuilt();
	if (IconImage)
	{
		IconImage->SetBrush(InBrush);
		IconImage->SetVisibility(InBrush.GetResourceObject() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UAvCategoryButtonWidget::HandleClicked()
{
	OnCategoryClicked.Broadcast(Index);
}

void UAvCategoryButtonWidget::HandleHovered()
{
	bHovered = true;
	ApplyVisuals();
}

void UAvCategoryButtonWidget::HandleUnhovered()
{
	bHovered = false;
	ApplyVisuals();
}

void UAvCategoryButtonWidget::EnsureBuilt()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Button"));
		Fill = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Fill"));
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Row"));
		IconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Icon"));
		LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Label"));
		WidgetTree->RootWidget = Button;
		Button->AddChild(Fill);
		Fill->SetContent(Row);
		Fill->SetPadding(FMargin(14.f, 9.f));
		if (UHorizontalBoxSlot* IconSlot = Row->AddChildToHorizontalBox(IconImage))
		{
			IconSlot->SetPadding(FMargin(0.f, 0.f, 10.f, 0.f));
			IconSlot->SetVerticalAlignment(VAlign_Center);
		}
		Row->AddChildToHorizontalBox(LabelText)->SetVerticalAlignment(VAlign_Center);
		AvCustomizeStyle::HideEmptyIcon(IconImage);
		Button->OnClicked.AddDynamic(this, &UAvCategoryButtonWidget::HandleClicked);
		Button->OnHovered.AddDynamic(this, &UAvCategoryButtonWidget::HandleHovered);
		Button->OnUnhovered.AddDynamic(this, &UAvCategoryButtonWidget::HandleUnhovered);
		SetRenderTransformPivot(FVector2D(0.f, 0.5f));
	}
}

void UAvCategoryButtonWidget::ApplyVisuals()
{
	const bool bActive = bSelected || bHovered;
	if (Fill)
	{
		Fill->SetBrushColor(bSelected ? AvCustomizeStyle::AccentDim : (bHovered ? AvCustomizeStyle::PanelHover : AvCustomizeStyle::PanelLight));
	}
	if (LabelText)
	{
		LabelText->SetAutoWrapText(false);
		AvCustomizeStyle::StyleText(LabelText, 20.f, bSelected ? AvCustomizeStyle::Accent : (bHovered ? AvCustomizeStyle::Text : AvCustomizeStyle::TextDim));
	}
	StartVisualTransition(bActive ? 1.f : 0.66f, bHovered ? FVector2D(1.035f, 1.035f) : FVector2D(1.f, 1.f), 0.14f);
}

void UAvCategoryButtonWidget::StartVisualTransition(float InTargetOpacity, FVector2D InTargetScale, float InDuration)
{
	if (!bVisualInitialized)
	{
		bVisualInitialized = true;
		CurrentOpacity = TargetOpacity = InTargetOpacity;
		CurrentScale = TargetScale = InTargetScale;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
		return;
	}

	if (IsVisualTarget(InTargetOpacity, InTargetScale))
	{
		return;
	}

	StartOpacity = CurrentOpacity;
	TargetOpacity = InTargetOpacity;
	StartScale = CurrentScale;
	TargetScale = InTargetScale;
	TransitionDuration = FMath::Max(0.01f, InDuration);

	if (UWorld* World = GetWorld())
	{
		TransitionStartTime = World->GetTimeSeconds();
		World->GetTimerManager().ClearTimer(VisualTransitionTimer);
		World->GetTimerManager().SetTimer(VisualTransitionTimer, this, &UAvCategoryButtonWidget::StepVisualTransition, 1.f / 60.f, true);
		StepVisualTransition();
	}
	else
	{
		CurrentOpacity = TargetOpacity;
		CurrentScale = TargetScale;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
	}
}

bool UAvCategoryButtonWidget::IsVisualTarget(float InTargetOpacity, const FVector2D& InTargetScale) const
{
	const bool bSameTarget = FMath::IsNearlyEqual(TargetOpacity, InTargetOpacity, 0.005f) && TargetScale.Equals(InTargetScale, 0.005f);
	const bool bAtTarget = CurrentScale.Equals(InTargetScale, 0.005f) && FMath::IsNearlyEqual(CurrentOpacity, InTargetOpacity, 0.005f);
	const UWorld* World = GetWorld();
	return bSameTarget && (bAtTarget || (World && World->GetTimerManager().IsTimerActive(VisualTransitionTimer)));
}

void UAvCategoryButtonWidget::StepVisualTransition()
{
	UWorld* World = GetWorld();
	const float Alpha = World ? (World->GetTimeSeconds() - TransitionStartTime) / TransitionDuration : 1.f;
	const float Eased = AvCustomizeStyle::EaseUi(Alpha);
	CurrentOpacity = FMath::Lerp(StartOpacity, TargetOpacity, Eased);
	CurrentScale = FMath::Lerp(StartScale, TargetScale, Eased);
	SetRenderOpacity(CurrentOpacity);
	SetRenderScale(CurrentScale);

	if (!World || Alpha >= 1.f)
	{
		CurrentOpacity = TargetOpacity;
		CurrentScale = TargetScale;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
		if (World)
		{
			World->GetTimerManager().ClearTimer(VisualTransitionTimer);
		}
	}
}

void UAvOptionCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureBuilt();
	ApplyVisuals();
}

void UAvOptionCardWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(VisualTransitionTimer);
	}
	Super::NativeDestruct();
}

void UAvOptionCardWidget::Setup(int32 InIndex, const FText& InLabel, bool bInSelected)
{
	Index = InIndex;
	bSelected = bInSelected;
	EnsureBuilt();
	if (LabelText)
	{
		LabelText->SetText(InLabel);
	}
	ApplyVisuals();
}

void UAvOptionCardWidget::HandleClicked()
{
	OnOptionClicked.Broadcast(Index);
}

void UAvOptionCardWidget::HandleHovered()
{
	bHovered = true;
	ApplyVisuals();
}

void UAvOptionCardWidget::HandleUnhovered()
{
	bHovered = false;
	ApplyVisuals();
}

void UAvOptionCardWidget::SetIconBrush(const FSlateBrush& InBrush)
{
	EnsureBuilt();
	if (IconImage)
	{
		IconImage->SetBrush(InBrush);
		IconImage->SetVisibility(InBrush.GetResourceObject() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UAvOptionCardWidget::EnsureBuilt()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Button"));
		Fill = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Fill"));
		UVerticalBox* Stack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Stack"));
		IconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Icon"));
		LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Label"));
		WidgetTree->RootWidget = Button;
		Button->AddChild(Fill);
		Fill->SetContent(Stack);
		Fill->SetPadding(FMargin(12.f));
		if (UVerticalBoxSlot* IconSlot = Stack->AddChildToVerticalBox(IconImage))
		{
			IconSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));
			IconSlot->SetHorizontalAlignment(HAlign_Left);
		}
		Stack->AddChildToVerticalBox(LabelText)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		AvCustomizeStyle::HideEmptyIcon(IconImage);
		Button->OnClicked.AddDynamic(this, &UAvOptionCardWidget::HandleClicked);
		Button->OnHovered.AddDynamic(this, &UAvOptionCardWidget::HandleHovered);
		Button->OnUnhovered.AddDynamic(this, &UAvOptionCardWidget::HandleUnhovered);
		SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	}
}

void UAvOptionCardWidget::ApplyVisuals()
{
	if (Fill)
	{
		Fill->SetBrushColor(bSelected ? AvCustomizeStyle::AccentDim : (bHovered ? AvCustomizeStyle::PanelHover : AvCustomizeStyle::PanelLight));
		Fill->SetPadding(FMargin(12.f));
	}
	if (LabelText)
	{
		LabelText->SetAutoWrapText(true);
		AvCustomizeStyle::StyleText(LabelText, 17.f, bSelected ? AvCustomizeStyle::Accent : (bHovered ? AvCustomizeStyle::Text : AvCustomizeStyle::TextDim));
	}
	StartVisualTransition(
		(bSelected || bHovered) ? 1.f : 0.78f,
		bHovered ? FVector2D(1.025f, 1.025f) : FVector2D(1.f, 1.f),
		bHovered ? FVector2D(0.f, -3.f) : FVector2D::ZeroVector,
		0.14f);
}

void UAvOptionCardWidget::StartVisualTransition(float InTargetOpacity, FVector2D InTargetScale, FVector2D InTargetTranslation, float InDuration)
{
	if (!bVisualInitialized)
	{
		bVisualInitialized = true;
		CurrentOpacity = TargetOpacity = InTargetOpacity;
		CurrentScale = TargetScale = InTargetScale;
		CurrentTranslation = TargetTranslation = InTargetTranslation;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
		SetRenderTranslation(CurrentTranslation);
		return;
	}

	if (IsVisualTarget(InTargetOpacity, InTargetScale, InTargetTranslation))
	{
		return;
	}

	StartOpacity = CurrentOpacity;
	TargetOpacity = InTargetOpacity;
	StartScale = CurrentScale;
	TargetScale = InTargetScale;
	StartTranslation = CurrentTranslation;
	TargetTranslation = InTargetTranslation;
	TransitionDuration = FMath::Max(0.01f, InDuration);

	if (UWorld* World = GetWorld())
	{
		TransitionStartTime = World->GetTimeSeconds();
		World->GetTimerManager().ClearTimer(VisualTransitionTimer);
		World->GetTimerManager().SetTimer(VisualTransitionTimer, this, &UAvOptionCardWidget::StepVisualTransition, 1.f / 60.f, true);
		StepVisualTransition();
	}
	else
	{
		CurrentOpacity = TargetOpacity;
		CurrentScale = TargetScale;
		CurrentTranslation = TargetTranslation;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
		SetRenderTranslation(CurrentTranslation);
	}
}

bool UAvOptionCardWidget::IsVisualTarget(float InTargetOpacity, const FVector2D& InTargetScale, const FVector2D& InTargetTranslation) const
{
	const bool bSameTarget = FMath::IsNearlyEqual(TargetOpacity, InTargetOpacity, 0.005f)
		&& TargetScale.Equals(InTargetScale, 0.005f)
		&& TargetTranslation.Equals(InTargetTranslation, 0.005f);
	const bool bAtTarget = CurrentScale.Equals(InTargetScale, 0.005f)
		&& CurrentTranslation.Equals(InTargetTranslation, 0.005f)
		&& FMath::IsNearlyEqual(CurrentOpacity, InTargetOpacity, 0.005f);
	const UWorld* World = GetWorld();
	return bSameTarget && (bAtTarget || (World && World->GetTimerManager().IsTimerActive(VisualTransitionTimer)));
}

void UAvOptionCardWidget::StepVisualTransition()
{
	UWorld* World = GetWorld();
	const float Alpha = World ? (World->GetTimeSeconds() - TransitionStartTime) / TransitionDuration : 1.f;
	const float Eased = AvCustomizeStyle::EaseUi(Alpha);
	CurrentOpacity = FMath::Lerp(StartOpacity, TargetOpacity, Eased);
	CurrentScale = FMath::Lerp(StartScale, TargetScale, Eased);
	CurrentTranslation = FMath::Lerp(StartTranslation, TargetTranslation, Eased);
	SetRenderOpacity(CurrentOpacity);
	SetRenderScale(CurrentScale);
	SetRenderTranslation(CurrentTranslation);

	if (!World || Alpha >= 1.f)
	{
		CurrentOpacity = TargetOpacity;
		CurrentScale = TargetScale;
		CurrentTranslation = TargetTranslation;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
		SetRenderTranslation(CurrentTranslation);
		if (World)
		{
			World->GetTimerManager().ClearTimer(VisualTransitionTimer);
		}
	}
}

void UAvColorSwatchWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureBuilt();
	ApplyVisuals();
}

void UAvColorSwatchWidget::Setup(int32 InIndex, FLinearColor InColor, bool bInSelected)
{
	Index = InIndex;
	Color = InColor;
	bSelected = bInSelected;
	EnsureBuilt();
	ApplyVisuals();
}

void UAvColorSwatchWidget::HandleClicked()
{
	OnSwatchClicked.Broadcast(Index);
}

void UAvColorSwatchWidget::EnsureBuilt()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Button"));
		Fill = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Fill"));
		WidgetTree->RootWidget = Button;
		Button->AddChild(Fill);
		Button->OnClicked.AddDynamic(this, &UAvColorSwatchWidget::HandleClicked);
	}
}

void UAvColorSwatchWidget::ApplyVisuals()
{
	if (Fill)
	{
		Fill->SetBrushColor(Color);
		Fill->SetPadding(bSelected ? FMargin(4.f) : FMargin(0.f));
	}
}

void UAvActionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureBuilt();
	ApplyVisuals();
}

void UAvActionButtonWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(VisualTransitionTimer);
	}
	Super::NativeDestruct();
}

void UAvActionButtonWidget::Setup(const FText& InLabel, bool bInPrimary)
{
	Label = InLabel;
	bPrimary = bInPrimary;
	EnsureBuilt();
	if (LabelText)
	{
		LabelText->SetText(Label);
	}
	ApplyVisuals();
}

void UAvActionButtonWidget::HandleClicked()
{
	OnActionClicked.Broadcast();
}

void UAvActionButtonWidget::HandleHovered()
{
	bHovered = true;
	ApplyVisuals();
}

void UAvActionButtonWidget::HandleUnhovered()
{
	bHovered = false;
	bPressed = false;
	ApplyVisuals();
}

void UAvActionButtonWidget::HandlePressed()
{
	bPressed = true;
	ApplyVisuals();
}

void UAvActionButtonWidget::HandleReleased()
{
	bPressed = false;
	ApplyVisuals();
}

void UAvActionButtonWidget::SetIconBrush(const FSlateBrush& InBrush)
{
	EnsureBuilt();
	if (IconImage)
	{
		IconImage->SetBrush(InBrush);
		IconImage->SetVisibility(InBrush.GetResourceObject() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UAvActionButtonWidget::EnsureBuilt()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Button"));
		Fill = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Fill"));
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Row"));
		IconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Icon"));
		LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Label"));
		WidgetTree->RootWidget = Button;
		Button->AddChild(Fill);
		Fill->SetContent(Row);
		Fill->SetPadding(FMargin(16.f, 11.f));
		if (UHorizontalBoxSlot* IconSlot = Row->AddChildToHorizontalBox(IconImage))
		{
			IconSlot->SetPadding(FMargin(0.f, 0.f, 9.f, 0.f));
			IconSlot->SetVerticalAlignment(VAlign_Center);
		}
		Row->AddChildToHorizontalBox(LabelText)->SetVerticalAlignment(VAlign_Center);
		AvCustomizeStyle::HideEmptyIcon(IconImage);
		Button->OnClicked.AddDynamic(this, &UAvActionButtonWidget::HandleClicked);
		Button->OnHovered.AddDynamic(this, &UAvActionButtonWidget::HandleHovered);
		Button->OnUnhovered.AddDynamic(this, &UAvActionButtonWidget::HandleUnhovered);
		Button->OnPressed.AddDynamic(this, &UAvActionButtonWidget::HandlePressed);
		Button->OnReleased.AddDynamic(this, &UAvActionButtonWidget::HandleReleased);
		SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	}
}

void UAvActionButtonWidget::ApplyVisuals()
{
	if (Fill)
	{
		if (bPressed)
		{
			Fill->SetBrushColor(AvCustomizeStyle::Accent);
			Fill->SetPadding(FMargin(16.f, 11.f));
		}
		else if (bHovered)
		{
			Fill->SetBrushColor(bPrimary ? AvCustomizeStyle::AccentGlow : AvCustomizeStyle::PanelHover);
			Fill->SetPadding(FMargin(16.f, 11.f));
		}
		else
		{
			Fill->SetBrushColor(bPrimary ? AvCustomizeStyle::AccentDim : AvCustomizeStyle::PanelLight);
			Fill->SetPadding(FMargin(16.f, 11.f));
		}
	}
	if (LabelText)
	{
		LabelText->SetAutoWrapText(true);
		AvCustomizeStyle::StyleText(LabelText, bPrimary ? 19.f : 18.f, (bPrimary || bPressed) ? AvCustomizeStyle::Text : AvCustomizeStyle::Accent);
	}
	StartVisualTransition(
		bHovered || bPrimary ? 1.f : 0.86f,
		bPressed ? FVector2D(0.985f, 0.985f) : (bHovered ? FVector2D(1.025f, 1.025f) : FVector2D(1.f, 1.f)),
		bPressed ? 0.10f : 0.14f);
}

void UAvActionButtonWidget::StartVisualTransition(float InTargetOpacity, FVector2D InTargetScale, float InDuration)
{
	if (!bVisualInitialized)
	{
		bVisualInitialized = true;
		CurrentOpacity = TargetOpacity = InTargetOpacity;
		CurrentScale = TargetScale = InTargetScale;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
		return;
	}

	if (IsVisualTarget(InTargetOpacity, InTargetScale))
	{
		return;
	}

	StartOpacity = CurrentOpacity;
	TargetOpacity = InTargetOpacity;
	StartScale = CurrentScale;
	TargetScale = InTargetScale;
	TransitionDuration = FMath::Max(0.01f, InDuration);

	if (UWorld* World = GetWorld())
	{
		TransitionStartTime = World->GetTimeSeconds();
		World->GetTimerManager().ClearTimer(VisualTransitionTimer);
		World->GetTimerManager().SetTimer(VisualTransitionTimer, this, &UAvActionButtonWidget::StepVisualTransition, 1.f / 60.f, true);
		StepVisualTransition();
	}
	else
	{
		CurrentOpacity = TargetOpacity;
		CurrentScale = TargetScale;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
	}
}

bool UAvActionButtonWidget::IsVisualTarget(float InTargetOpacity, const FVector2D& InTargetScale) const
{
	const bool bSameTarget = FMath::IsNearlyEqual(TargetOpacity, InTargetOpacity, 0.005f) && TargetScale.Equals(InTargetScale, 0.005f);
	const bool bAtTarget = CurrentScale.Equals(InTargetScale, 0.005f) && FMath::IsNearlyEqual(CurrentOpacity, InTargetOpacity, 0.005f);
	const UWorld* World = GetWorld();
	return bSameTarget && (bAtTarget || (World && World->GetTimerManager().IsTimerActive(VisualTransitionTimer)));
}

void UAvActionButtonWidget::StepVisualTransition()
{
	UWorld* World = GetWorld();
	const float Alpha = World ? (World->GetTimeSeconds() - TransitionStartTime) / TransitionDuration : 1.f;
	const float Eased = AvCustomizeStyle::EaseUi(Alpha);
	CurrentOpacity = FMath::Lerp(StartOpacity, TargetOpacity, Eased);
	CurrentScale = FMath::Lerp(StartScale, TargetScale, Eased);
	SetRenderOpacity(CurrentOpacity);
	SetRenderScale(CurrentScale);

	if (!World || Alpha >= 1.f)
	{
		CurrentOpacity = TargetOpacity;
		CurrentScale = TargetScale;
		SetRenderOpacity(CurrentOpacity);
		SetRenderScale(CurrentScale);
		if (World)
		{
			World->GetTimerManager().ClearTimer(VisualTransitionTimer);
		}
	}
}

void UAvCustomizeRootWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	OpenedAtTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	EnsureBuiltForViewport();
	SyncStateFromCharacter();
	EnsurePreview();
	ApplyStateToPreview();
	RebuildCategories();
	RebuildOptions();
	SetKeyboardFocus();
	UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeInput] Root NativeConstruct, children built"));
}

void UAvCustomizeRootWidget::NativeDestruct()
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
	Super::NativeDestruct();
}

FReply UAvCustomizeRootWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::B || Key == EKeys::Escape)
	{
		const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		if (Key == EKeys::B && Now - OpenedAtTime < 0.25f)
		{
			return FReply::Handled();
		}
		HandleCloseClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UAvCustomizeRootWidget::EnsureBuiltForViewport()
{
	if (bLayoutBuilt)
	{
		return;
	}

	BuildLayout();
	bLayoutBuilt = true;
}

void UAvCustomizeRootWidget::BuildLayout()
{
	if (!WidgetTree)
	{
		return;
	}

	WidgetTree->RootWidget = nullptr;

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = Root;

	UBorder* Backdrop = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Backdrop"));
	Backdrop->SetBrushColor(AvCustomizeStyle::Bg);
	UCanvasPanelSlot* BackdropSlot = Root->AddChildToCanvas(Backdrop);
	BackdropSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	BackdropSlot->SetOffsets(FMargin(0.f));

	UVerticalBox* Main = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainLayout"));
	UCanvasPanelSlot* MainSlot = Root->AddChildToCanvas(Main);
	MainSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	MainSlot->SetOffsets(FMargin(44.f, 28.f, 44.f, 34.f));

	AddTopTabs(Main);

	UHorizontalBox* Body = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("BodyLayout"));
	UVerticalBoxSlot* BodySlot = Main->AddChildToVerticalBox(Body);
	BodySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BodySlot->SetPadding(FMargin(0.f, 24.f, 0.f, 18.f));

	UBorder* LeftPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("LeftCategoryPanel"));
	LeftPanel->SetBrushColor(AvCustomizeStyle::Panel);
	UVerticalBox* LeftContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("LeftContent"));
	LeftPanel->SetContent(LeftContent);
	UHorizontalBoxSlot* LeftSlot = Body->AddChildToHorizontalBox(LeftPanel);
	LeftSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	LeftSlot->SetPadding(FMargin(0.f, 0.f, 22.f, 0.f));

	USizeBox* LeftSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("LeftSize"));
	LeftSize->SetWidthOverride(330.f);
	LeftSize->AddChild(LeftContent);
	LeftPanel->SetContent(LeftSize);

	UTextBlock* CategoriesTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CategoriesTitle"));
	CategoriesTitle->SetText(FAvLoc::Text(TEXT("Customization.Legacy.CategoriesTitle")));
	AvCustomizeStyle::StyleText(CategoriesTitle, 26.f, AvCustomizeStyle::TextDim);
	LeftContent->AddChildToVerticalBox(CategoriesTitle)->SetPadding(FMargin(22.f, 22.f, 22.f, 16.f));

	CategoryScroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("CategoryScroll"));
	LeftContent->AddChildToVerticalBox(CategoryScroll)->SetPadding(FMargin(14.f, 0.f, 14.f, 0.f));

	BuildPreviewPanel(Body);

	RightPanel = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RightPanel"));
	UBorder* RightBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RightOptionsPanel"));
	RightBorder->SetBrushColor(AvCustomizeStyle::Panel);
	USizeBox* RightSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("RightSize"));
	RightSize->SetWidthOverride(620.f);
	RightSize->AddChild(RightPanel);
	RightBorder->SetContent(RightSize);
	UHorizontalBoxSlot* RightSlot = Body->AddChildToHorizontalBox(RightBorder);
	RightSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

	BuildEquipmentBar(Main);
}

void UAvCustomizeRootWidget::AddTopTabs(UVerticalBox* Parent)
{
	UHorizontalBox* Top = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("TopTabs"));
	Parent->AddChildToVerticalBox(Top)->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CustomizeTitle"));
	Title->SetText(FAvLoc::Text(TEXT("Customization.Tab.Character")));
	AvCustomizeStyle::StyleText(Title, 44.f, AvCustomizeStyle::Text);
	Top->AddChildToHorizontalBox(Title)->SetPadding(FMargin(0.f, 0.f, 42.f, 0.f));

	const TCHAR* Tabs[] = {
		TEXT("Customization.Tab.Wardrobe"),
		TEXT("Customization.Tab.Equipment"),
		TEXT("Customization.Legacy.Settings"),
		TEXT("Customization.Legacy.Poses"),
		TEXT("Customization.Legacy.Info") };
	for (int32 i = 0; i < UE_ARRAY_COUNT(Tabs); ++i)
	{
		UTextBlock* Tab = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Tab->SetText(FAvLoc::Text(Tabs[i]));
		AvCustomizeStyle::StyleText(Tab, 26.f, i == 0 ? AvCustomizeStyle::Accent : AvCustomizeStyle::TextDim);
		Top->AddChildToHorizontalBox(Tab)->SetPadding(FMargin(0.f, 10.f, 32.f, 0.f));
	}

	UAvActionButtonWidget* Close = WidgetTree->ConstructWidget<UAvActionButtonWidget>(UAvActionButtonWidget::StaticClass(), TEXT("CloseButton"));
	Close->Setup(FAvLoc::Text(TEXT("Customization.Action.Close")), false);
	Close->OnActionClicked.AddDynamic(this, &UAvCustomizeRootWidget::HandleCloseClicked);
	UHorizontalBoxSlot* CloseSlot = Top->AddChildToHorizontalBox(Close);
	CloseSlot->SetHorizontalAlignment(HAlign_Right);
	CloseSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
}

void UAvCustomizeRootWidget::BuildPreviewPanel(UHorizontalBox* Parent)
{
	UBorder* PreviewPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CenterPreviewPanel"));
	PreviewPanel->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.42f));
	UVerticalBox* PreviewContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PreviewContent"));
	PreviewPanel->SetContent(PreviewContent);
	UHorizontalBoxSlot* PreviewSlot = Parent->AddChildToHorizontalBox(PreviewPanel);
	PreviewSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	PreviewSlot->SetPadding(FMargin(0.f, 0.f, 22.f, 0.f));

	UTextBlock* PreviewTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PreviewTitle"));
	PreviewTitle->SetText(FAvLoc::Text(TEXT("Customization.Preview.LegacyTitle")));
	AvCustomizeStyle::StyleText(PreviewTitle, 24.f, AvCustomizeStyle::TextDim);
	PreviewContent->AddChildToVerticalBox(PreviewTitle)->SetPadding(FMargin(24.f, 20.f, 24.f, 8.f));

	PreviewImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("PreviewImage"));
	UVerticalBoxSlot* ImageSlot = PreviewContent->AddChildToVerticalBox(PreviewImage);
	ImageSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ImageSlot->SetPadding(FMargin(18.f, 0.f, 18.f, 18.f));
}

void UAvCustomizeRootWidget::BuildEquipmentBar(UVerticalBox* Parent)
{
	UBorder* Equipment = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("EquipmentBar"));
	Equipment->SetBrushColor(AvCustomizeStyle::Panel);
	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("EquipmentRow"));
	Equipment->SetContent(Row);
	Parent->AddChildToVerticalBox(Equipment)->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EquipmentTitle"));
	Title->SetText(FAvLoc::Text(TEXT("Customization.Legacy.EquipmentTitle")));
	AvCustomizeStyle::StyleText(Title, 26.f, AvCustomizeStyle::Text);
	Row->AddChildToHorizontalBox(Title)->SetPadding(FMargin(22.f, 20.f, 34.f, 20.f));

	const TCHAR* Items[] = {
		TEXT("Customization.Section.Jacket"),
		TEXT("Customization.Section.Legs"),
		TEXT("Customization.Section.Feet") };
	for (const TCHAR* Item : Items)
	{
		UTextBlock* Card = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Card->SetText(FAvLoc::Text(Item));
		AvCustomizeStyle::StyleText(Card, 24.f, AvCustomizeStyle::Accent);
		Row->AddChildToHorizontalBox(Card)->SetPadding(FMargin(18.f, 20.f));
	}
}

void UAvCustomizeRootWidget::RebuildCategories()
{
	if (!CategoryScroll)
	{
		return;
	}
	CategoryScroll->ClearChildren();
	CategoryButtons.Reset();
	for (int32 i = 0; i < UE_ARRAY_COUNT(CustomizeSlots); ++i)
	{
		UAvCategoryButtonWidget* Button = WidgetTree->ConstructWidget<UAvCategoryButtonWidget>(UAvCategoryButtonWidget::StaticClass());
		Button->Setup(i, GetCustomizeSlotLabel(i), i == SelectedCustomizationState.SelectedSlotIndex);
		Button->OnCategoryClicked.AddDynamic(this, &UAvCustomizeRootWidget::HandleCategoryClicked);
		CategoryButtons.Add(Button);
		if (UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(CategoryScroll->AddChild(Button)))
		{
			ScrollSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));
		}
	}
}

void UAvCustomizeRootWidget::RefreshCategoryFocus()
{
	for (int32 i = 0; i < CategoryButtons.Num(); ++i)
	{
		if (CategoryButtons[i])
		{
			CategoryButtons[i]->SetSelected(i == SelectedCustomizationState.SelectedSlotIndex);
		}
	}
}

void UAvCustomizeRootWidget::RebuildOptions()
{
	if (!RightPanel)
	{
		return;
	}
	RightPanel->ClearChildren();
	RightPanel->SetRenderOpacity(0.98f);

	CategoryTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CategoryTitle"));
	CategoryTitle->SetText(GetCustomizeSlotLabel(SelectedCustomizationState.SelectedSlotIndex));
	AvCustomizeStyle::StyleText(CategoryTitle, 36.f, AvCustomizeStyle::Accent);
	RightPanel->AddChildToVerticalBox(CategoryTitle)->SetPadding(FMargin(28.f, 28.f, 28.f, 10.f));

	UTextBlock* OptionsTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OptionsTitle"));
	OptionsTitle->SetText(FAvLoc::Text(TEXT("Customization.Legacy.OptionsTitle")));
	AvCustomizeStyle::StyleText(OptionsTitle, 26.f, AvCustomizeStyle::TextDim);
	RightPanel->AddChildToVerticalBox(OptionsTitle)->SetPadding(FMargin(28.f, 6.f, 28.f, 18.f));

	OptionGrid = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), TEXT("OptionGrid"));
	OptionGrid->SetMinDesiredSlotWidth(184.f);
	OptionGrid->SetMinDesiredSlotHeight(120.f);
	RightPanel->AddChildToVerticalBox(OptionGrid)->SetPadding(FMargin(28.f, 0.f, 28.f, 18.f));

	const int32 MaxPage = FMath::Max(0, (SelectedCustomizationState.CurrentOptions.Num() - 1) / OptionsPerPage);
	SelectedCustomizationState.OptionPage = FMath::Clamp(SelectedCustomizationState.OptionPage, 0, MaxPage);
	const int32 FirstOption = SelectedCustomizationState.OptionPage * OptionsPerPage;
	const int32 LastOption = FMath::Min(FirstOption + OptionsPerPage, SelectedCustomizationState.CurrentOptions.Num());
	const FString Worn = SelectedCustomizationState.Appearance.Get(GetSelectedSlot()).ToSoftObjectPath().GetAssetName();

	for (int32 i = FirstOption; i < LastOption; ++i)
	{
		const int32 LocalIndex = i - FirstOption;
		const FString& Option = SelectedCustomizationState.CurrentOptions[i];
		UAvOptionCardWidget* Card = WidgetTree->ConstructWidget<UAvOptionCardWidget>(UAvOptionCardWidget::StaticClass());
		Card->Setup(i, DisplayMeshName(Option), !Worn.IsEmpty() && Option == Worn);
		Card->OnOptionClicked.AddDynamic(this, &UAvCustomizeRootWidget::HandleOptionClicked);
		USizeBox* CardSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		CardSize->SetWidthOverride(176.f);
		CardSize->SetHeightOverride(112.f);
		CardSize->AddChild(Card);
		UUniformGridSlot* GridSlot = OptionGrid->AddChildToUniformGrid(CardSize, LocalIndex / 3, LocalIndex % 3);
		GridSlot->SetHorizontalAlignment(HAlign_Fill);
		GridSlot->SetVerticalAlignment(VAlign_Fill);
	}

	if (MaxPage > 0)
	{
		UHorizontalBox* Pages = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("OptionPages"));
		RightPanel->AddChildToVerticalBox(Pages)->SetPadding(FMargin(28.f, 0.f, 28.f, 18.f));

		UAvActionButtonWidget* Prev = WidgetTree->ConstructWidget<UAvActionButtonWidget>(UAvActionButtonWidget::StaticClass(), TEXT("PrevPageButton"));
		Prev->Setup(FText::FromString(TEXT("<")), false);
		Prev->OnActionClicked.AddDynamic(this, &UAvCustomizeRootWidget::HandlePrevPageClicked);
		Pages->AddChildToHorizontalBox(Prev)->SetPadding(FMargin(0.f, 0.f, 16.f, 0.f));

		UTextBlock* PageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PageText"));
		PageText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), SelectedCustomizationState.OptionPage + 1, MaxPage + 1)));
		AvCustomizeStyle::StyleText(PageText, 24.f, AvCustomizeStyle::TextDim);
		Pages->AddChildToHorizontalBox(PageText)->SetPadding(FMargin(0.f, 12.f, 16.f, 0.f));

		UAvActionButtonWidget* Next = WidgetTree->ConstructWidget<UAvActionButtonWidget>(UAvActionButtonWidget::StaticClass(), TEXT("NextPageButton"));
		Next->Setup(FText::FromString(TEXT(">")), false);
		Next->OnActionClicked.AddDynamic(this, &UAvCustomizeRootWidget::HandleNextPageClicked);
		Pages->AddChildToHorizontalBox(Next);
	}

	UTextBlock* ColorTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ColorTitle"));
	ColorTitle->SetText(FAvLoc::Text(TEXT("Customization.Legacy.ColorTitle")));
	AvCustomizeStyle::StyleText(ColorTitle, 26.f, AvCustomizeStyle::TextDim);
	RightPanel->AddChildToVerticalBox(ColorTitle)->SetPadding(FMargin(28.f, 8.f, 28.f, 12.f));

	UHorizontalBox* Swatches = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ColorSwatches"));
	RightPanel->AddChildToVerticalBox(Swatches)->SetPadding(FMargin(28.f, 0.f, 28.f, 28.f));
	const FLinearColor Colors[] = {
		FLinearColor(0.95f, 0.68f, 0.42f, 1.f),
		FLinearColor(0.80f, 0.42f, 0.18f, 1.f),
		FLinearColor(0.16f, 0.18f, 0.16f, 1.f),
		FLinearColor(0.82f, 0.88f, 0.08f, 1.f),
		FLinearColor(0.08f, 0.10f, 0.12f, 1.f)
	};
	for (int32 i = 0; i < UE_ARRAY_COUNT(Colors); ++i)
	{
		UAvColorSwatchWidget* Swatch = WidgetTree->ConstructWidget<UAvColorSwatchWidget>(UAvColorSwatchWidget::StaticClass());
		Swatch->Setup(i, Colors[i], i == SelectedCustomizationState.SelectedColorIndex);
		Swatch->OnSwatchClicked.AddDynamic(this, &UAvCustomizeRootWidget::HandleColorClicked);
		Swatches->AddChildToHorizontalBox(Swatch)->SetPadding(FMargin(0.f, 0.f, 12.f, 0.f));
	}

	USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), TEXT("RightSpacer"));
	RightPanel->AddChildToVerticalBox(Spacer)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	UAvActionButtonWidget* Save = WidgetTree->ConstructWidget<UAvActionButtonWidget>(UAvActionButtonWidget::StaticClass(), TEXT("SaveCharacterButton"));
	Save->Setup(FAvLoc::Text(TEXT("Customization.Action.SaveCharacter")), true);
	Save->OnActionClicked.AddDynamic(this, &UAvCustomizeRootWidget::HandleSaveClicked);
	RightPanel->AddChildToVerticalBox(Save)->SetPadding(FMargin(28.f, 0.f, 28.f, 12.f));

	UAvActionButtonWidget* Apply = WidgetTree->ConstructWidget<UAvActionButtonWidget>(UAvActionButtonWidget::StaticClass(), TEXT("ApplyCharacterButton"));
	Apply->Setup(FAvLoc::Text(TEXT("Customization.Action.Apply")), false);
	Apply->OnActionClicked.AddDynamic(this, &UAvCustomizeRootWidget::HandleApplyClicked);
	RightPanel->AddChildToVerticalBox(Apply)->SetPadding(FMargin(28.f, 0.f, 28.f, 28.f));
}

void UAvCustomizeRootWidget::RefreshFromCharacter()
{
	SyncStateFromCharacter();
	EnsurePreview();
	ApplyStateToPreview();
	RebuildCategories();
	RebuildOptions();
}

FText UAvCustomizeRootWidget::DisplayMeshName(const FString& Name) const
{
	// Legacy discovery has no curated StableId metadata. Never expose raw asset/package names.
	return FAvLoc::Text(TEXT("Customization.Legacy.Variant"));
}

AAvaryoCharacter* UAvCustomizeRootWidget::GetAvCharacter() const
{
	const APlayerController* PC = GetOwningPlayer();
	return PC ? Cast<AAvaryoCharacter>(PC->GetPawn()) : nullptr;
}

void UAvCustomizeRootWidget::SyncStateFromCharacter()
{
	AAvaryoCharacter* Character = GetAvCharacter();
	UWorkerAppearanceComponent* Appearance = Character ? Character->FindComponentByClass<UWorkerAppearanceComponent>() : nullptr;
	if (Appearance)
	{
		SelectedCustomizationState.Appearance = Appearance->GetAppearance();
		SelectedCustomizationState.CurrentOptions = Appearance->GetOptionsForSlot(GetSelectedSlot());
	}
}

void UAvCustomizeRootWidget::ApplyStateToPreview()
{
	if (PreviewActor)
	{
		PreviewActor->SetPreviewAppearance(SelectedCustomizationState.Appearance);
	}
}

void UAvCustomizeRootWidget::EnsurePreview()
{
	if (!PreviewRenderTarget)
	{
		PreviewRenderTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("AvCustomizePreviewRT"));
		PreviewRenderTarget->RenderTargetFormat = RTF_RGBA8;
		PreviewRenderTarget->ClearColor = FLinearColor(0.008f, 0.010f, 0.012f, 1.f);
		PreviewRenderTarget->InitAutoFormat(1024, 1024);
		PreviewRenderTarget->UpdateResourceImmediate(true);
	}

	if (PreviewImage && PreviewRenderTarget)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(PreviewRenderTarget);
		Brush.ImageSize = FVector2D(1024.f, 1024.f);
		PreviewImage->SetBrush(Brush);
	}

	if (!PreviewActor)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			PreviewActor = World->SpawnActor<AAvCustomizePreviewActor>(AAvCustomizePreviewActor::StaticClass(), FVector(0.f, 0.f, -5000.f), FRotator::ZeroRotator, Params);
			if (PreviewActor)
			{
				PreviewActor->InitializePreview(PreviewRenderTarget);
			}
		}
	}
}

bool UAvCustomizeRootWidget::ResolveMeshForSlot(EWorkerSlot WorkerSlot, const FString& Option, TSoftObjectPtr<USkeletalMesh>& OutMesh) const
{
	FString Folder, Include, Exclude;
	UWorkerAppearanceComponent::GetSlotSearch(WorkerSlot, Folder, Include, Exclude);
	if (Folder.IsEmpty())
	{
		return false;
	}

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	FARFilter Filter;
	Filter.PackagePaths.Add(FName(*Folder));
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(USkeletalMesh::StaticClass()->GetClassPathName());
	TArray<FAssetData> Assets;
	AssetRegistry.GetAssets(Filter, Assets);
	for (const FAssetData& Asset : Assets)
	{
		if (Asset.AssetName.ToString().Equals(Option, ESearchCase::IgnoreCase))
		{
			OutMesh = TSoftObjectPtr<USkeletalMesh>(Asset.ToSoftObjectPath());
			return true;
		}
	}
	return false;
}

EWorkerSlot UAvCustomizeRootWidget::GetSelectedSlot() const
{
	const int32 Index = FMath::Clamp(SelectedCustomizationState.SelectedSlotIndex, 0, static_cast<int32>(UE_ARRAY_COUNT(CustomizeSlots)) - 1);
	return CustomizeSlots[Index].Slot;
}

const TCHAR* UAvCustomizeRootWidget::GetSelectedCommand() const
{
	const int32 Index = FMath::Clamp(SelectedCustomizationState.SelectedSlotIndex, 0, static_cast<int32>(UE_ARRAY_COUNT(CustomizeSlots)) - 1);
	return CustomizeSlots[Index].Command;
}

void UAvCustomizeRootWidget::HandleCategoryClicked(int32 Index)
{
	SelectedCustomizationState.SelectedSlotIndex = FMath::Clamp(Index, 0, static_cast<int32>(UE_ARRAY_COUNT(CustomizeSlots)) - 1);
	SelectedCustomizationState.OptionPage = 0;
	SyncStateFromCharacter();
	RefreshCategoryFocus();
	RebuildOptions();
}

void UAvCustomizeRootWidget::HandleOptionClicked(int32 Index)
{
	if (!SelectedCustomizationState.CurrentOptions.IsValidIndex(Index))
	{
		return;
	}

	const FString Option = SelectedCustomizationState.CurrentOptions[Index];
	if (AAvaryoCharacter* Character = GetAvCharacter())
	{
		Character->RequestWorkerAppearanceSlot(GetSelectedCommand(), Option);
	}

	TSoftObjectPtr<USkeletalMesh> Mesh;
	if (ResolveMeshForSlot(GetSelectedSlot(), Option, Mesh))
	{
		SelectedCustomizationState.Appearance.Set(GetSelectedSlot(), Mesh);
	}
	ApplyStateToPreview();
	RebuildOptions();
}

void UAvCustomizeRootWidget::HandleColorClicked(int32 Index)
{
	SelectedCustomizationState.SelectedColorIndex = Index;
	const FLinearColor Colors[] = {
		FLinearColor(0.95f, 0.68f, 0.42f, 1.f),
		FLinearColor(0.80f, 0.42f, 0.18f, 1.f),
		FLinearColor(0.16f, 0.18f, 0.16f, 1.f),
		FLinearColor(0.82f, 0.88f, 0.08f, 1.f),
		FLinearColor(0.08f, 0.10f, 0.12f, 1.f)
	};
	if (PreviewActor && Index >= 0 && Index < UE_ARRAY_COUNT(Colors))
	{
		PreviewActor->SetPreviewSlotColor(GetSelectedSlot(), Colors[Index]);
	}
	if (Index >= 0 && Index < UE_ARRAY_COUNT(Colors))
	{
		if (AAvaryoCharacter* Character = GetAvCharacter())
		{
			Character->RequestWorkerAppearanceColor(GetSelectedCommand(), Colors[Index]);
		}
	}
	RebuildOptions();
}

void UAvCustomizeRootWidget::HandlePrevPageClicked()
{
	SelectedCustomizationState.OptionPage = FMath::Max(0, SelectedCustomizationState.OptionPage - 1);
	RebuildOptions();
}

void UAvCustomizeRootWidget::HandleNextPageClicked()
{
	const int32 MaxPage = FMath::Max(0, (SelectedCustomizationState.CurrentOptions.Num() - 1) / OptionsPerPage);
	SelectedCustomizationState.OptionPage = FMath::Min(MaxPage, SelectedCustomizationState.OptionPage + 1);
	RebuildOptions();
}

void UAvCustomizeRootWidget::HandleSaveClicked()
{
	if (AAvaryoCharacter* Character = GetAvCharacter())
	{
		Character->RequestSaveWorkerAppearance(SelectedCustomizationState.Appearance);
	}
}

void UAvCustomizeRootWidget::HandleApplyClicked()
{
	if (AAvaryoCharacter* Character = GetAvCharacter())
	{
		if (!Character->RequestApplyWorkerAppearance(SelectedCustomizationState.Appearance))
		{
			return;
		}
	}
	HandleCloseClicked();
}

void UAvCustomizeRootWidget::HandleCloseClicked()
{
	if (AAvaryoPlayerController* PC = Cast<AAvaryoPlayerController>(GetOwningPlayer()))
	{
		PC->ToggleCustomize();
	}
}
