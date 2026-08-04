#include "UI/AvMyCharacterRowWidget.h"

#include "Engine/Texture2D.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "Rendering/SlateRenderer.h"
#include "Styling/CoreStyle.h"
#include "UI/AvCharacterCustomizationRootWidget.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

namespace AvMyCharacterRow
{
	constexpr int32 MaxDisplayNameLength = 20;
	const FLinearColor SelectedAccent(0.95f, 0.42f, 0.04f, 1.f);
	const FLinearColor InactiveFrame(0.025f, 0.035f, 0.045f, 0.85f);
	const FLinearColor InactiveText(0.82f, 0.83f, 0.85f, 1.f);
	const FLinearColor PortraitBackground(0.0025f, 0.004f, 0.0055f, 0.98f);
	const FLinearColor PortraitTint(0.68f, 0.70f, 0.73f, 1.f);
	const FLinearColor ButtonNormal(0.003347f, 0.005182f, 0.007499f, 0.98f);
	const FLinearColor ButtonHovered(0.005182f, 0.006995f, 0.008568f, 1.f);
	const FLinearColor ButtonPressed(0.012f, 0.006f, 0.002f, 1.f);
	const FLinearColor Transparent(0.f, 0.f, 0.f, 0.f);
	constexpr float RenameHitTolerance = 3.f;
	const FString OverflowEllipsis(TEXT("\u2026"));
}

void UAvMyCharacterRowWidget::InitializeMyCharacterRow(
	UAvCharacterCustomizationRootWidget* InOwner,
	FName InCharacterId,
	UTexture2D* InSilhouetteTexture,
	const FString& InDisplayName,
	bool bInSelected)
{
	CustomizationOwner = InOwner;
	CharacterId = InCharacterId;
	SilhouetteTexture = InSilhouetteTexture;
	DisplayName = InDisplayName;
	bSelected = bInSelected;
	SetToolTipText(FText::FromString(DisplayName));
	RefreshSlatePresentation();
}

TSharedRef<SWidget> UAvMyCharacterRowWidget::RebuildWidget()
{
	auto ConfigureButtonBrush = [](FSlateBrush& Brush, const FLinearColor& Tint)
	{
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		Brush.TintColor = FSlateColor(Tint);
		Brush.OutlineSettings.CornerRadii = FVector4(3.f, 3.f, 3.f, 3.f);
		Brush.OutlineSettings.Width = 0.f;
		Brush.OutlineSettings.Color = FSlateColor(AvMyCharacterRow::Transparent);
		Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	};

	FSlateBrush NormalButtonBrush;
	FSlateBrush HoveredButtonBrush;
	FSlateBrush PressedButtonBrush;
	FSlateBrush DisabledButtonBrush;
	FSlateBrush NoDrawBrush;
	ConfigureButtonBrush(NormalButtonBrush, AvMyCharacterRow::ButtonNormal);
	ConfigureButtonBrush(HoveredButtonBrush, AvMyCharacterRow::ButtonHovered);
	ConfigureButtonBrush(PressedButtonBrush, AvMyCharacterRow::ButtonPressed);
	DisabledButtonBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NoDrawBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	RowButtonStyle.SetNormal(NormalButtonBrush);
	RowButtonStyle.SetHovered(HoveredButtonBrush);
	RowButtonStyle.SetPressed(PressedButtonBrush);
	RowButtonStyle.SetDisabled(DisabledButtonBrush);
	RowButtonStyle.SetNormalForeground(AvMyCharacterRow::InactiveText);
	RowButtonStyle.SetHoveredForeground(AvMyCharacterRow::InactiveText);
	RowButtonStyle.SetPressedForeground(AvMyCharacterRow::InactiveText);
	RowButtonStyle.SetNormalPadding(FMargin(0.f));
	RowButtonStyle.SetPressedPadding(FMargin(0.f));

	NameEditorStyle = FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>(
		TEXT("NormalEditableTextBox"));
	NameEditorStyle.BackgroundImageNormal = NoDrawBrush;
	NameEditorStyle.BackgroundImageHovered = NoDrawBrush;
	NameEditorStyle.BackgroundImageFocused = NoDrawBrush;
	NameEditorStyle.BackgroundImageReadOnly = NoDrawBrush;
	NameEditorStyle.Padding = FMargin(0.f);
	DisplayNameFont = FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 18);
	NameEditorStyle.TextStyle.Font = DisplayNameFont;
	NameEditorStyle.ForegroundColor = FSlateColor(AvMyCharacterRow::InactiveText);
	NameEditorStyle.ReadOnlyForegroundColor = FSlateColor(AvMyCharacterRow::InactiveText);

	BaseFrameBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	BaseFrameBrush.TintColor = FSlateColor(AvMyCharacterRow::Transparent);
	BaseFrameBrush.OutlineSettings.CornerRadii = FVector4(3.f, 3.f, 3.f, 3.f);
	BaseFrameBrush.OutlineSettings.Width = 1.f;
	BaseFrameBrush.OutlineSettings.Color = FSlateColor(AvMyCharacterRow::InactiveFrame);
	BaseFrameBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;

	SelectedFrameBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	SelectedFrameBrush.TintColor = FSlateColor(AvMyCharacterRow::Transparent);
	SelectedFrameBrush.OutlineSettings.CornerRadii = FVector4(3.f, 3.f, 3.f, 3.f);
	SelectedFrameBrush.OutlineSettings.Width = 2.f;
	SelectedFrameBrush.OutlineSettings.Color = FSlateColor(AvMyCharacterRow::SelectedAccent);
	SelectedFrameBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;

	SilhouetteFrameBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	SilhouetteFrameBrush.TintColor = FSlateColor(AvMyCharacterRow::PortraitBackground);
	SilhouetteFrameBrush.OutlineSettings.CornerRadii = FVector4(2.f, 2.f, 2.f, 2.f);
	SilhouetteFrameBrush.OutlineSettings.Width = 1.f;
	SilhouetteFrameBrush.OutlineSettings.Color = FSlateColor(AvMyCharacterRow::InactiveFrame);
	SilhouetteFrameBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;

	SilhouetteBrush.DrawAs = ESlateBrushDrawType::Image;
	SilhouetteBrush.TintColor = FSlateColor(FLinearColor::White);
	SilhouetteBrush.ImageSize = FVector2D::ZeroVector;
	SilhouetteBrush.SetResourceObject(SilhouetteTexture.Get());

	TSharedRef<SWidget> Result =
		SNew(SBox)
		.HeightOverride(92.f)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SButton)
				.ButtonStyle(&RowButtonStyle)
				.IsFocusable(true)
				.ToolTipText(FText::FromString(DisplayName))
				.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, HandleRowClicked))
				[
					SNew(SOverlay)
					.Visibility(EVisibility::SelfHitTestInvisible)
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(&BaseFrameBrush)
					.Padding(FMargin(4.f, 2.f))
					.RenderOpacity(0.35f)
					.Visibility(EVisibility::HitTestInvisible)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(SelectedFrame, SBorder)
					.BorderImage(&SelectedFrameBrush)
					.Padding(FMargin(4.f, 2.f))
					.Visibility(bSelected ? EVisibility::HitTestInvisible : EVisibility::Hidden)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(13.f))
				[
					SNew(SHorizontalBox)
					.Visibility(EVisibility::SelfHitTestInvisible)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(FMargin(0.f, 0.f, 14.f, 0.f))
					[
						SNew(SBox)
						.WidthOverride(66.f)
						.HeightOverride(66.f)
						.Visibility(EVisibility::HitTestInvisible)
						[
							SNew(SBorder)
							.BorderImage(&SilhouetteFrameBrush)
							.Padding(FMargin(4.f))
							[
								SNew(SImage)
								.Image(&SilhouetteBrush)
								.ColorAndOpacity(AvMyCharacterRow::PortraitTint)
							]
						]
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					.VAlign(VAlign_Center)
					.Padding(FMargin(12.f, 0.f, 8.f, 0.f))
					[
						SNew(SBox)
						.Visibility(EVisibility::HitTestInvisible)
					]
				]
			]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(FMargin(105.f, 13.f, 21.f, 13.f))
			[
				SNew(SOverlay)
				.Visibility(EVisibility::SelfHitTestInvisible)
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					SAssignNew(NameText, STextBlock)
					.Text(FText::FromString(DisplayName))
					.Font(DisplayNameFont)
					.ColorAndOpacity(AvMyCharacterRow::InactiveText)
					.AutoWrapText(false)
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
					.Clipping(EWidgetClipping::ClipToBounds)
					.ToolTipText(FText::FromString(DisplayName))
					.Visibility(EVisibility::HitTestInvisible)
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(NameEditor, SEditableTextBox)
					.Style(&NameEditorStyle)
					.Text(FText::FromString(DisplayName))
					.SelectAllTextWhenFocused(true)
					.RevertTextOnEscape(true)
					.OnKeyDownHandler(BIND_UOBJECT_DELEGATE(
						FOnKeyDown, HandleNameKeyDown))
					.OnTextChanged(BIND_UOBJECT_DELEGATE(
						FOnTextChanged, HandleNameTextChanged))
					.OnTextCommitted(BIND_UOBJECT_DELEGATE(
						FOnTextCommitted, HandleNameTextCommitted))
					.Visibility(EVisibility::Collapsed)
				]
			]
		];

	RefreshSlatePresentation();
	return Result;
}

FReply UAvMyCharacterRowWidget::NativeOnPreviewMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	}

	const FVector2D ScreenPosition = InMouseEvent.GetScreenSpacePosition();
	FVector2D GeometrySize = FVector2D::ZeroVector;
	FVector2D MeasuredTextSize = FVector2D::ZeroVector;
	float VisibleWidth = 0.f;
	FVector4 RenameHitRect(0.f, 0.f, 0.f, 0.f);
	float LayoutScale = 1.f;
	float ApplicationScale = 1.f;
	const bool bHasMetrics = CalculateRenameHitTestMetrics(
		GeometrySize,
		MeasuredTextSize,
		VisibleWidth,
		RenameHitRect,
		LayoutScale,
		ApplicationScale);
	const FGeometry NameGeometry = NameText.IsValid()
		? NameText->GetCachedGeometry()
		: FGeometry();
	const FVector2D LocalPosition = NameText.IsValid()
		? NameGeometry.AbsoluteToLocal(ScreenPosition)
		: FVector2D::ZeroVector;

	if (bRenaming && NameEditor.IsValid() &&
		NameEditor->GetCachedGeometry().IsUnderLocation(ScreenPosition))
	{
		if (UAvCharacterCustomizationRootWidget* Owner = CustomizationOwner.Get())
		{
			Owner->RecordMyCharacterRowPointerDiagnostic(
				CharacterId, ScreenPosition, LocalPosition, GeometrySize,
				MeasuredTextSize, VisibleWidth, RenameHitRect, false,
				TEXT("EditField"), LayoutScale, ApplicationScale);
		}
		return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	}

	const bool bInsideRenameTextRect = bHasMetrics && !bRenaming &&
		LocalPosition.X >= RenameHitRect.X && LocalPosition.X <= RenameHitRect.Z &&
		LocalPosition.Y >= RenameHitRect.Y && LocalPosition.Y <= RenameHitRect.W;
	if (UAvCharacterCustomizationRootWidget* Owner = CustomizationOwner.Get())
	{
		Owner->RecordMyCharacterRowPointerDiagnostic(
			CharacterId, ScreenPosition, LocalPosition, GeometrySize,
			MeasuredTextSize, VisibleWidth, RenameHitRect,
			bInsideRenameTextRect,
			bInsideRenameTextRect ? TEXT("Rename") : TEXT("Select"),
			LayoutScale, ApplicationScale);
	}

	if (bInsideRenameTextRect)
	{
		BeginRename();
		return FReply::Handled();
	}
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

bool UAvMyCharacterRowWidget::CalculateRenameHitTestMetrics(
	FVector2D& OutGeometrySize,
	FVector2D& OutMeasuredTextSize,
	float& OutVisibleWidth,
	FVector4& OutRenameHitRect,
	float& OutLayoutScale,
	float& OutApplicationScale) const
{
	OutGeometrySize = FVector2D::ZeroVector;
	OutMeasuredTextSize = FVector2D::ZeroVector;
	OutVisibleWidth = 0.f;
	OutRenameHitRect = FVector4(0.f, 0.f, 0.f, 0.f);
	OutLayoutScale = 1.f;
	OutApplicationScale = 1.f;
	if (!NameText.IsValid() || !FSlateApplication::IsInitialized())
	{
		return false;
	}

	const FGeometry NameGeometry = NameText->GetCachedGeometry();
	OutGeometrySize = NameGeometry.GetLocalSize();
	if (OutGeometrySize.X <= KINDA_SMALL_NUMBER ||
		OutGeometrySize.Y <= KINDA_SMALL_NUMBER)
	{
		return false;
	}

	FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();
	if (!Renderer)
	{
		return false;
	}
	const TSharedRef<FSlateFontMeasure> FontMeasure = Renderer->GetFontMeasureService();
	OutLayoutScale = FMath::Max(
		static_cast<float>(NameGeometry.GetAccumulatedLayoutTransform().GetScale()),
		KINDA_SMALL_NUMBER);
	OutApplicationScale = FSlateApplication::Get().GetApplicationScale();

	const auto MeasuredScreen = FontMeasure->Measure(
		FStringView(DisplayName), DisplayNameFont, OutLayoutScale);
	OutMeasuredTextSize = FVector2D(
		static_cast<double>(MeasuredScreen.X) / OutLayoutScale,
		static_cast<double>(MeasuredScreen.Y) / OutLayoutScale);
	const float AvailableScreenWidth = OutGeometrySize.X * OutLayoutScale;
	float VisibleScreenWidth = static_cast<float>(MeasuredScreen.X);
	if (VisibleScreenWidth > AvailableScreenWidth)
	{
		const auto EllipsisScreen = FontMeasure->Measure(
			FStringView(AvMyCharacterRow::OverflowEllipsis),
			DisplayNameFont,
			OutLayoutScale);
		const float PrefixBudget = FMath::Max(
			0.f,
			AvailableScreenWidth - static_cast<float>(EllipsisScreen.X));
		float VisiblePrefixWidth = 0.f;
		for (int32 PrefixLength = 1; PrefixLength <= DisplayName.Len(); ++PrefixLength)
		{
			const FString Prefix = DisplayName.Left(PrefixLength);
			const auto PrefixScreen = FontMeasure->Measure(
				FStringView(Prefix), DisplayNameFont, OutLayoutScale);
			if (static_cast<float>(PrefixScreen.X) > PrefixBudget)
			{
				break;
			}
			VisiblePrefixWidth = static_cast<float>(PrefixScreen.X);
		}
		VisibleScreenWidth = FMath::Min(
			AvailableScreenWidth,
			VisiblePrefixWidth + static_cast<float>(EllipsisScreen.X));
	}
	OutVisibleWidth = FMath::Min(
		OutGeometrySize.X,
		VisibleScreenWidth / OutLayoutScale);
	const float VisibleTextHeight = FMath::Min(
		OutGeometrySize.Y,
		static_cast<float>(OutMeasuredTextSize.Y));
	const float TextTop = FMath::Max(
		0.f,
		(OutGeometrySize.Y - VisibleTextHeight) * 0.5f);
	OutRenameHitRect = FVector4(
		-AvMyCharacterRow::RenameHitTolerance,
		TextTop - AvMyCharacterRow::RenameHitTolerance,
		FMath::Min(
			OutGeometrySize.X,
			OutVisibleWidth + AvMyCharacterRow::RenameHitTolerance),
		TextTop + VisibleTextHeight + AvMyCharacterRow::RenameHitTolerance);
	return OutVisibleWidth > 0.f && VisibleTextHeight > 0.f;
}

FString UAvMyCharacterRowWidget::GetRenameHitTestGeometryForAutomation() const
{
	FVector2D GeometrySize;
	FVector2D MeasuredTextSize;
	float VisibleWidth = 0.f;
	FVector4 RenameHitRect;
	float LayoutScale = 1.f;
	float ApplicationScale = 1.f;
	if (!CalculateRenameHitTestMetrics(
		GeometrySize, MeasuredTextSize, VisibleWidth, RenameHitRect,
		LayoutScale, ApplicationScale) || !NameText.IsValid())
	{
		return TEXT("{}");
	}
	const FGeometry RowGeometry = GetCachedGeometry();
	const FGeometry NameGeometry = NameText->GetCachedGeometry();
	const FVector2D RowTopLeft = RowGeometry.LocalToAbsolute(FVector2D::ZeroVector);
	const FVector2D NameTopLeft = NameGeometry.LocalToAbsolute(FVector2D::ZeroVector);
	const FVector2D HitMinScreen = NameGeometry.LocalToAbsolute(
		FVector2D(RenameHitRect.X, RenameHitRect.Y));
	const FVector2D HitMaxScreen = NameGeometry.LocalToAbsolute(
		FVector2D(RenameHitRect.Z, RenameHitRect.W));
	return FString::Printf(
		TEXT("{\"character_id\":\"%s\",\"row_top_left\":[%.3f,%.3f],\"row_size\":[%.3f,%.3f],\"name_top_left\":[%.3f,%.3f],\"name_geometry_size\":[%.3f,%.3f],\"measured_text_size\":[%.3f,%.3f],\"visible_width\":%.3f,\"rename_hit_rect\":[%.3f,%.3f,%.3f,%.3f],\"rename_hit_screen\":[%.3f,%.3f,%.3f,%.3f],\"layout_scale\":%.4f,\"application_scale\":%.4f}"),
		*CharacterId.ToString(),
		RowTopLeft.X, RowTopLeft.Y,
		RowGeometry.GetLocalSize().X, RowGeometry.GetLocalSize().Y,
		NameTopLeft.X, NameTopLeft.Y,
		GeometrySize.X, GeometrySize.Y,
		MeasuredTextSize.X, MeasuredTextSize.Y,
		VisibleWidth,
		RenameHitRect.X, RenameHitRect.Y, RenameHitRect.Z, RenameHitRect.W,
		HitMinScreen.X, HitMinScreen.Y, HitMaxScreen.X, HitMaxScreen.Y,
		LayoutScale, ApplicationScale);
}

void UAvMyCharacterRowWidget::SetSelectedState(bool bInSelected)
{
	bSelected = bInSelected;
	RefreshSlatePresentation();
}

void UAvMyCharacterRowWidget::SetDisplayName(const FString& InDisplayName)
{
	DisplayName = InDisplayName;
	SetToolTipText(FText::FromString(DisplayName));
	RefreshSlatePresentation();
}

bool UAvMyCharacterRowWidget::HasRenameKeyboardFocus() const
{
	return NameEditor.IsValid() &&
		(NameEditor->HasKeyboardFocus() || NameEditor->HasFocusedDescendants());
}

bool UAvMyCharacterRowWidget::BeginRename()
{
	if (bRenaming || CharacterId.IsNone() || !NameEditor.IsValid())
	{
		return false;
	}

	if (UAvCharacterCustomizationRootWidget* Owner = CustomizationOwner.Get())
	{
		Owner->HandleMyCharacterRenameStarted(this);
	}
	else
	{
		return false;
	}

	bRenaming = true;
	DisplayNameBeforeRename = DisplayName;
	NameEditor->SetText(FText::FromString(DisplayName));
	RefreshSlatePresentation();
	FSlateApplication::Get().SetKeyboardFocus(NameEditor, EFocusCause::SetDirectly);
	NameEditor->SelectAllText();
	return true;
}

bool UAvMyCharacterRowWidget::CommitPendingRename()
{
	return bRenaming && NameEditor.IsValid()
		? FinishRename(true, NameEditor->GetText().ToString())
		: false;
}

void UAvMyCharacterRowWidget::CancelRename()
{
	if (bRenaming)
	{
		FinishRename(false, DisplayNameBeforeRename);
	}
}

void UAvMyCharacterRowWidget::SetPendingRenameText(const FString& InDisplayName)
{
	if (bRenaming && NameEditor.IsValid())
	{
		NameEditor->SetText(FText::FromString(
			InDisplayName.Left(AvMyCharacterRow::MaxDisplayNameLength)));
	}
}

FString UAvMyCharacterRowWidget::GetPendingRenameText() const
{
	return NameEditor.IsValid() ? NameEditor->GetText().ToString() : FString();
}

void UAvMyCharacterRowWidget::RefreshSlatePresentation()
{
	if (SelectedFrame)
	{
		SelectedFrame->SetVisibility(
			bSelected ? EVisibility::HitTestInvisible : EVisibility::Hidden);
	}
	if (NameText)
	{
		NameText->SetText(FText::FromString(DisplayName));
		NameText->SetColorAndOpacity(FSlateColor(AvMyCharacterRow::InactiveText));
		NameText->SetToolTipText(FText::FromString(DisplayName));
		NameText->SetVisibility(
			bRenaming ? EVisibility::Collapsed : EVisibility::HitTestInvisible);
	}
	if (NameEditor)
	{
		NameEditor->SetVisibility(
			bRenaming ? EVisibility::Visible : EVisibility::Collapsed);
	}
}

FReply UAvMyCharacterRowWidget::HandleRowClicked()
{
	if (!CharacterId.IsNone())
	{
		if (UAvCharacterCustomizationRootWidget* Owner = CustomizationOwner.Get())
		{
			Owner->HandleMyCharacterRowClicked(CharacterId);
		}
	}
	return FReply::Handled();
}

FReply UAvMyCharacterRowWidget::HandleNameKeyDown(
	const FGeometry& MyGeometry,
	const FKeyEvent& KeyEvent)
{
	if (bRenaming && KeyEvent.GetKey() == EKeys::Escape)
	{
		CancelRename();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UAvMyCharacterRowWidget::HandleNameTextChanged(const FText& NewText)
{
	if (bIsClampingName || !NameEditor.IsValid())
	{
		return;
	}
	const FString Value = NewText.ToString();
	if (Value.Len() <= AvMyCharacterRow::MaxDisplayNameLength)
	{
		return;
	}
	TGuardValue<bool> Guard(bIsClampingName, true);
	NameEditor->SetText(FText::FromString(
		Value.Left(AvMyCharacterRow::MaxDisplayNameLength)));
}

void UAvMyCharacterRowWidget::HandleNameTextCommitted(
	const FText& Text,
	ETextCommit::Type CommitMethod)
{
	if (!bRenaming)
	{
		return;
	}
	if (CommitMethod == ETextCommit::OnCleared)
	{
		FinishRename(false, DisplayNameBeforeRename);
		return;
	}
	if (CommitMethod == ETextCommit::OnEnter ||
		CommitMethod == ETextCommit::OnUserMovedFocus)
	{
		FinishRename(true, Text.ToString());
	}
}

bool UAvMyCharacterRowWidget::FinishRename(
	bool bCommit,
	const FString& CandidateName)
{
	if (!bRenaming)
	{
		return false;
	}

	const FString TrimmedName = CandidateName.TrimStartAndEnd().Left(
		AvMyCharacterRow::MaxDisplayNameLength);
	const FString RestoredName = DisplayNameBeforeRename;
	bRenaming = false;
	bool bAccepted = false;
	if (bCommit && !TrimmedName.IsEmpty())
	{
		if (UAvCharacterCustomizationRootWidget* Owner = CustomizationOwner.Get())
		{
			bAccepted = Owner->HandleMyCharacterRenameCommitted(CharacterId, TrimmedName);
		}
	}
	DisplayName = bAccepted ? TrimmedName : RestoredName;
	if (NameEditor)
	{
		NameEditor->SetText(FText::FromString(DisplayName));
	}
	RefreshSlatePresentation();
	return bAccepted;
}

void UAvMyCharacterRowWidget::TriggerSelectionForAutomation()
{
	HandleRowClicked();
}

bool UAvMyCharacterRowWidget::TriggerRenameForAutomation()
{
	return BeginRename();
}
