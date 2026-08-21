// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/Widgets/SGraphMinimap.h"
#include "GraphMinimap/Widgets/SGraphRenderer.h"
#include "GraphMinimap/Widgets/SAutoScrollTextBlock.h"
#include "GraphMinimap/Utilities/GraphMinimapSettings.h"
#include "GraphMinimap/Utilities/GraphMinimapStyle.h"
#include "GraphMinimap/Utilities/GraphMinimapUtils.h"
#include "GraphEditorActions.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphNode_Comment.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "GraphMinimap"

namespace GraphMinimap
{
	void SGraphMinimap::Construct(
		const FArguments& InArgs,
		const TSharedPtr<SGraphEditor>& InOwnerGraphEditor,
#if UE_5_00_OR_LATER
		SOverlay::FScopedWidgetSlotArguments* InSlot
#else
		SOverlay::FOverlaySlot* InSlot
#endif
	)
	{
		SetOwnerGraphEditor(InOwnerGraphEditor);
#if UE_5_00_OR_LATER
		check(InSlot != nullptr);
		InSlot->Expose(Slot);
#else
		Slot = InSlot;
#endif
		check(Slot != nullptr);
		OnGraphMinimapConfigChanged = InArgs._OnGraphMinimapConfigChanged;

		if (InArgs._HAlign.IsSet() && InArgs._VAlign.IsSet())
		{
#if UE_5_00_OR_LATER
			Slot->SetHorizontalAlignment(InArgs._HAlign.Get());
			Slot->SetVerticalAlignment(InArgs._VAlign.Get());
#else
			Slot->HAlign(InArgs._HAlign.Get());
			Slot->VAlign(InArgs._VAlign.Get());
#endif
		}
		else
		{
			const EHorizontalAlignment HAlign = GraphMinimapAlignment::ToHorizontalAlignment(UGraphMinimapSettings::Get().MinimapAlignment);
			const EVerticalAlignment VAlign = GraphMinimapAlignment::ToVerticalAlignment(UGraphMinimapSettings::Get().MinimapAlignment);
#if UE_5_00_OR_LATER
			Slot->SetHorizontalAlignment(HAlign);
			Slot->SetVerticalAlignment(VAlign);
#else
			Slot->HAlign(HAlign);
			Slot->VAlign(VAlign);
#endif
			UGraphMinimapSettings::Get().OnMinimapAlignmentChanged.AddRaw(this, &SGraphMinimap::HandleOnMinimapAlignmentChanged);
		}
		
		if (InArgs._MinimapOpacity.IsSet())
		{
			MinimapOpacity = InArgs._MinimapOpacity.Get();
		}
		else
		{
			MinimapOpacity = UGraphMinimapSettings::Get().MinimapOpacity;
			UGraphMinimapSettings::Get().OnMinimapOpacityChanged.AddRaw(this, &SGraphMinimap::HandleOnMinimapOpacityChanged);
		}
		
		TAttribute<FSlateColor> MinimapTintColor;
		if (InArgs._MinimapTintColor.IsSet())
		{
			MinimapTintColor = InArgs._MinimapTintColor;
		}
		else
		{
			MinimapTintColor = UGraphMinimapSettings::Get().MinimapTintColor;
			UGraphMinimapSettings::Get().OnMinimapTintColorChanged.AddRaw(this, &SGraphMinimap::HandleOnMinimapTintColorChanged);
		}

		float ModeIconSize;
		if (InArgs._ModeIconSize.IsSet())
		{
			ModeIconSize = InArgs._ModeIconSize.Get();
		}
		else
		{
			ModeIconSize = UGraphMinimapSettings::Get().ModeIconSize;
			UGraphMinimapSettings::Get().OnModeIconSizeChanged.AddRaw(this, &SGraphMinimap::HandleOnModeIconSizeChanged);
		}

		FSlateColor ModeIconTintColor;
		if (InArgs._ModeIconTintColor.IsSet())
		{
			ModeIconTintColor = InArgs._ModeIconTintColor.Get();
		}
		else
		{
			ModeIconTintColor = UGraphMinimapSettings::Get().ModeIconTintColor;
			UGraphMinimapSettings::Get().OnModeIconTintColorChanged.AddRaw(this, &SGraphMinimap::HandleOnModeIconTintColorChanged);
		}
		
		if (InArgs._DragSensitivity.IsSet())
		{
			DragSensitivity = InArgs._DragSensitivity.Get();
		}
		else
		{
			DragSensitivity = UGraphMinimapSettings::Get().DragSensitivity;
			UGraphMinimapSettings::Get().OnDragSensitivityChanged.AddRaw(this, &SGraphMinimap::HandleOnDragSensitivityChanged);
		}

		if (InArgs._bDrawSizeAndScale.IsSet())
		{
			bDrawSizeAndScale = InArgs._bDrawSizeAndScale.Get();
		}
		else
		{
			bDrawSizeAndScale = UGraphMinimapSettings::Get().bDrawSizeAndScale;
			UGraphMinimapSettings::Get().OnDrawSizeAndScaleChanged.AddRaw(this, &SGraphMinimap::HandleOnDrawSizeAndScaleChanged);
		}

		FSlateColor SizeAndScaleTintColor;
		if (InArgs._SizeAndScaleTintColor.IsSet())
		{
			SizeAndScaleTintColor = InArgs._SizeAndScaleTintColor.Get();
		}
		else
		{
			SizeAndScaleTintColor = UGraphMinimapSettings::Get().SizeAndScaleTintColor;
			UGraphMinimapSettings::Get().OnSizeAndScaleTintColorChanged.AddRaw(this, &SGraphMinimap::HandleOnSizeAndScaleTintColorChanged);
		}

		if (InArgs._bShowMinimapArea.IsSet())
		{
			bShowMinimapArea = InArgs._bShowMinimapArea.Get();
		}
		else
		{
			bShowMinimapArea = UGraphMinimapSettings::Get().bShowMinimapArea;
			UGraphMinimapSettings::Get().OnShowMinimapAreaChanged.AddRaw(this, &SGraphMinimap::HandleOnShowMinimapAreaChanged);
		}

		float MinimapAreaOpacity;
		if (InArgs._MinimapAreaOpacity.IsSet())
		{
			MinimapAreaOpacity = InArgs._MinimapAreaOpacity.Get();
		}
		else
		{
			MinimapAreaOpacity = UGraphMinimapSettings::Get().MinimapAreaOpacity;
			UGraphMinimapSettings::Get().OnMinimapAreaOpacityChanged.AddRaw(this, &SGraphMinimap::HandleOnMinimapAreaOpacityChanged);
		}
		
		CollectMinimapAreas();
		if (InArgs._SelectedMinimapArea.IsSet())
		{
			SetSelectedMinimapAreaIdentifier(InArgs._SelectedMinimapArea.Get());
		}
		if (!SelectedMinimapArea.IsValid() && MinimapAreas.IsValidIndex(0))
		{
			SelectedMinimapArea = MinimapAreas[0];
		}
		
		ChildSlot
		[
			SNew(SOverlay)
			// Graph renderer.
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(MinimapPanel, SBox)
				[
					SAssignNew(GraphRenderer, SGraphRenderer)
					.SourceGraphEditor(OwnerGraphEditor)
					.ColorAndOpacity(MinimapTintColor)
					.RenderingScale(InArgs._RenderingScale)
					.OnPreRendering(this, &SGraphMinimap::HandleOnPreRendering)
					.OnPostRendering(this, &SGraphMinimap::HandleOnPostRendering)
				]
			]
			// Mode icon.
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(3.f)
			[
				SAssignNew(ModeIconPanel, SBox)
				.WidthOverride(ModeIconSize)
				.HeightOverride(ModeIconSize)
				[
					SAssignNew(ModeIcon, SImage)
					.ColorAndOpacity(ModeIconTintColor)
					.Image(this, &SGraphMinimap::GetModeIconImage)
				]
			]
			// Size and scale text.
			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Top)
			.Padding(3.f)
			[
				SAssignNew(SizeAndScaleText, STextBlock)
				.Text(this, &SGraphMinimap::GetSizeAndScaleText)
				.Visibility(this, &SGraphMinimap::GetSizeAndScaleTextVisibility)
				.ColorAndOpacity(SizeAndScaleTintColor)
			]
			// Minimap area widget.
			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(3.f)
			[
				SAssignNew(MinimapAreaPanel, SBox)
				.RenderOpacity(MinimapAreaOpacity)
				.WidthOverride(this, &SGraphMinimap::GetMinimapAreaPanelWidth)
				.Visibility(this, &SGraphMinimap::GetMinimapAreaPanelVisibility)
				[
					SNew(SComboBox<TSharedPtr<FMinimapArea>>)
					.OptionsSource(&MinimapAreas)
					.OnSelectionChanged(this, &SGraphMinimap::HandleOnSelectionChanged)
					.OnGenerateWidget(this, &SGraphMinimap::HandleOnGenerateWidget)
					.OnComboBoxOpening(this, &SGraphMinimap::CollectMinimapAreas)
					.InitiallySelectedItem(SelectedMinimapArea)
					[
						SAssignNew(MinimapAreaName, SAutoScrollTextBlock)
						.TextBlockArgs(
							STextBlock::FArguments()
							.Text(this, &SGraphMinimap::GetSelectedMinimapAreaText)
						)
					]
				]
			]
			// Draw prepare error text.
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(this, &SGraphMinimap::GetDrawPrepareErrorText)
				.Visibility(this, &SGraphMinimap::GetDrawPrepareErrorTextVisibility)
				.ColorAndOpacity(FLinearColor::Red)
				.Justification(ETextJustify::Center)
				.Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 13))
			]
		];
		
		ChangeState(InArgs._InitialState);
		SetMinimapSize(InArgs._MinimapSize);
		SetSelectedMinimapArea(SelectedMinimapArea);
	}
	
	FReply SGraphMinimap::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		const EGraphMinimapState CurrentState = GetState();
		if (CurrentState == EGraphMinimapState::Resizable)
		{
			ControlRenderingScaleProcess(MouseEvent.GetWheelDelta());
		}
		else if (CurrentState == EGraphMinimapState::Controllable)
		{
			ControlZoomAmountProcess(MouseEvent.GetWheelDelta());
		}
		
		return SCompoundWidget::OnMouseWheel(MyGeometry, MouseEvent);
	}

	FReply SGraphMinimap::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		const EGraphMinimapState CurrentState = GetState();
		if (CurrentState == EGraphMinimapState::Resizable ||
			CurrentState == EGraphMinimapState::Controllable)
		{
			bIsDragging = true;
		}
		
		return SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
	}
	
	FReply SGraphMinimap::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		if (bIsDragging)
		{
			const EGraphMinimapState CurrentState = GetState();
			if (CurrentState == EGraphMinimapState::Resizable)
			{
				FVector2D AdditionalSize = MouseEvent.GetCursorDelta();
				if (Slot != nullptr)
				{
#if UE_5_00_OR_LATER
					const EHorizontalAlignment HorizontalAlignment = Slot->GetHorizontalAlignment();
					const EVerticalAlignment VerticalAlignment = Slot->GetVerticalAlignment();
#else
					const EHorizontalAlignment HorizontalAlignment = Slot->HAlignment;
					const EVerticalAlignment VerticalAlignment = Slot->VAlignment;
#endif
					if (HorizontalAlignment == HAlign_Left)
					{
						AdditionalSize.X *= -1.f;
					}
					if (VerticalAlignment == VAlign_Top)
					{
						AdditionalSize.Y *= -1.f;
					}
				}
				
				ControlSizeProcess(AdditionalSize);
			}
			else if (CurrentState == EGraphMinimapState::Controllable)
			{
				FDisplayMetrics DisplayMetrics;
				FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);
				const float DPIScale = FPlatformApplicationMisc::GetDPIScaleFactorAtPoint(
					DisplayMetrics.PrimaryDisplayWorkAreaRect.Left,
					DisplayMetrics.PrimaryDisplayWorkAreaRect.Top
				);
				const FVector2D RelativePosition = (
					(MouseEvent.GetScreenSpacePosition() - MyGeometry.GetAbsolutePosition()) / DPIScale
				);
				ControlViewLocationProcess(RelativePosition);
			}
		}
		
		return SCompoundWidget::OnMouseMove(MyGeometry, MouseEvent);
	}
	
	FReply SGraphMinimap::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		bIsDragging = false;
		
		return SCompoundWidget::OnMouseButtonUp(MyGeometry, MouseEvent);
	}

	void SGraphMinimap::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
		
		if (MouseEvent.GetPressedButtons().Num() == 0)
		{
			bIsDragging = false;
		}
	}

	TSharedPtr<SGraphEditor> SGraphMinimap::GetOwnerGraphEditor() const
	{
		return OwnerGraphEditor;
	}

	void SGraphMinimap::SetOwnerGraphEditor(const TSharedPtr<SGraphEditor>& GraphEditor)
	{
		OwnerGraphEditor = GraphEditor;
		OwnerGraphPanel = FGraphMinimapUtils::FindNearestChildGraphPanel(OwnerGraphEditor);
		if (GraphRenderer.IsValid())
		{
			GraphRenderer->SetSourceGraphEditor(OwnerGraphEditor);
		}
	}

	EGraphMinimapState SGraphMinimap::GetState() const
	{
		return State.Get(EGraphMinimapState::Hidden);
	}

	void SGraphMinimap::ChangeState(const EGraphMinimapState NewState)
	{
		if (!State.IsSet() || State.GetValue() != NewState)
		{
			State = NewState;
			
			NotifyGraphMinimapConfigChanged();
			
			UpdateMinimapVisibility();
			UpdateMinimapOpacity();

			if (GetState() != EGraphMinimapState::Resizable)
			{
				bIsDragging = false;
			}
		}
	}

	FVector2D SGraphMinimap::GetMinimapSize() const
	{
		return Size;
	}

	void SGraphMinimap::SetMinimapSize(const FVector2D& NewSize)
	{
		const FVector2D ClampedMinimapSize(
			FMath::Max(NewSize.X, Global::MinimumMinimapSize.X),
			FMath::Max(NewSize.Y, Global::MinimumMinimapSize.Y)
		);
		
		Size = ClampedMinimapSize;
		if (MinimapPanel.IsValid())
		{
			MinimapPanel->SetHeightOverride(Size.Y);
			MinimapPanel->SetWidthOverride(Size.X);
		}
		if (GraphRenderer.IsValid())
		{
			GraphRenderer->SetOwnerDesiredSize(Size);
		}

		NotifyGraphMinimapConfigChanged();
	}

	FVector2D SGraphMinimap::GetGraphSize() const
	{
		if (GraphRenderer.IsValid())
		{
			return GraphRenderer->GetGraphSize();
		}

		return FVector2D::ZeroVector;
	}

	FVector2D SGraphMinimap::GetScaledGraphSize() const
	{
		if (GraphRenderer.IsValid())
		{
			return GraphRenderer->GetScaledGraphSize();
		}

		return FVector2D::ZeroVector;
	}

	float SGraphMinimap::GetRenderingScale() const
	{
		if (GraphRenderer.IsValid())
		{
			return GraphRenderer->GetRenderingScale();
		}

		return -1.f;
	}

	void SGraphMinimap::SetRenderingScale(const float NewRenderingScale)
	{
		if (GraphRenderer.IsValid())
		{
			GraphRenderer->SetRenderingScale(NewRenderingScale);
			NotifyGraphMinimapConfigChanged();
		}
	}

	void SGraphMinimap::SetMinimapOpacity(const float NewOpacity)
	{
		MinimapOpacity = NewOpacity;
		UpdateMinimapOpacity();
	}

	void SGraphMinimap::UpdateMinimapVisibility()
	{
		EVisibility NewVisibility;
		switch (GetState())
		{
		case EGraphMinimapState::Controllable:
			NewVisibility = EVisibility::Visible;
			break;

		case EGraphMinimapState::Visible:
			NewVisibility = EVisibility::HitTestInvisible;
			break;

		case EGraphMinimapState::Resizable:
			NewVisibility = EVisibility::Visible;
			break;
			
		case EGraphMinimapState::Hidden:
		default:
			NewVisibility = EVisibility::Collapsed;
			break;
		}

		SetVisibility(NewVisibility);
	}

	void SGraphMinimap::UpdateMinimapOpacity()
	{
		SetRenderOpacity(MinimapOpacity);
	}
	
	bool SGraphMinimap::HasMultipleMinimapArea() const
	{
		return (MinimapAreas.Num() >= 2);
	}

	TArray<FMinimapAreaIdentifier> SGraphMinimap::GetMinimapAreaIdentifiers() const
	{
		TArray<FMinimapAreaIdentifier> Identifiers;
		Identifiers.Reserve(MinimapAreas.Num());
		for (const TSharedPtr<FMinimapArea>& MinimapArea : MinimapAreas)
		{
			if (!MinimapArea.IsValid())
			{
				continue;
			}

			Identifiers.Add(MinimapArea->ToIdentifier());
		}
		
		return Identifiers;
	}

	FMinimapAreaIdentifier SGraphMinimap::GetSelectedMinimapAreaIdentifier() const
	{
		if (!SelectedMinimapArea.IsValid())
		{
			return FMinimapAreaIdentifier();
		}

		return SelectedMinimapArea->ToIdentifier();
	}

	void SGraphMinimap::SetSelectedMinimapAreaIdentifier(const FMinimapAreaIdentifier& Identifier)
	{
		const TSharedPtr<FMinimapArea>* FoundMinimapArea = MinimapAreas.FindByPredicate(
			[&Identifier](const TSharedPtr<FMinimapArea>& MinimapArea) -> bool
			{
				if (!MinimapArea.IsValid())
				{
					return false;
				}
					
				return (Identifier == MinimapArea->ToIdentifier());
			}
		);
		if (FoundMinimapArea != nullptr)
		{
			SetSelectedMinimapArea(*FoundMinimapArea);
		}
	}

	void SGraphMinimap::SetSelectedMinimapArea(const TSharedPtr<FMinimapArea>& MinimapArea)
	{
		if (!MinimapArea.IsValid())
		{
			return;
		}
		
		SelectedMinimapArea = MinimapArea;
		if (GraphRenderer.IsValid())
		{
			GraphRenderer->SetSelectedMinimapArea(SelectedMinimapArea);
		}
		
		NotifyGraphMinimapConfigChanged();
	}

	void SGraphMinimap::ControlSizeProcess(const FVector2D& AdditionalSize)
	{
		if (MinimapPanel.IsValid() && OwnerGraphEditor.IsValid())
		{
			const FVector2D NewMinimapSize = MinimapPanel->GetDesiredSize() - AdditionalSize * DragSensitivity;
			const FVector2D OwnerGraphSize = OwnerGraphEditor->GetTickSpaceGeometry().GetLocalSize();
			const FVector2D ClampedMinimapSize(
				FMath::Clamp(NewMinimapSize.X, Global::MinimumMinimapSize.X, OwnerGraphSize.X),
				FMath::Clamp(NewMinimapSize.Y, Global::MinimumMinimapSize.Y, OwnerGraphSize.Y)
			);
			SetMinimapSize(ClampedMinimapSize);
		}
	}

	void SGraphMinimap::ControlRenderingScaleProcess(const float WheelDelta)
	{
		if (GraphRenderer.IsValid())
		{
			const float CurrentRenderingScale = GraphRenderer->GetRenderingScale();
			SetRenderingScale(CurrentRenderingScale + WheelDelta * 0.01f);
		}
	}

	void SGraphMinimap::ControlViewLocationProcess(const FVector2D& RelativePosition)
	{
		if (GraphRenderer.IsValid() && OwnerGraphEditor.IsValid())
		{
			FVector2D ViewLocation;
			float ZoomAmount;
			OwnerGraphEditor->GetViewLocation(ViewLocation, ZoomAmount);
			
			ViewLocation = GraphRenderer->GetViewLocation(RelativePosition);
			OwnerGraphEditor->SetViewLocation(ViewLocation, ZoomAmount);
		}
	}

	void SGraphMinimap::ControlZoomAmountProcess(const float WheelDelta)
	{
		TSharedPtr<FUICommandInfo> Command;
		if (WheelDelta > 0.f)
		{
			Command = FGraphEditorCommands::Get().ZoomIn;
		}
		else if (WheelDelta < 0.f)
		{
			Command = FGraphEditorCommands::Get().ZoomOut;
		}
		else
		{
			return;
		}

		const FModifierKeysState ModifierKeysOverride(
			false, false,
			true, true,
			false, false,
			false, false,
			false
		);
		FKeyEvent KeyEvent;
		if (!FGraphMinimapUtils::GetKeyEventFromUICommandInfo(KeyEvent, Command, ModifierKeysOverride))
		{
			return;
		}

		if (OwnerGraphPanel.IsValid())
		{
			OwnerGraphPanel->OnKeyDown(OwnerGraphPanel->GetCachedGeometry(), KeyEvent);
		}
	}

	void SGraphMinimap::CollectMinimapAreas()
	{
		MinimapAreas.Empty();
		
		if (!OwnerGraphEditor.IsValid())
		{
			return;
		}

		const UEdGraph* Graph = OwnerGraphEditor->GetCurrentGraph();
		if (!IsValid(Graph))
		{
			return;
		}

		const TArray<UEdGraphNode*>& Nodes = Graph->Nodes;
		if (Nodes.Num() == 0)
		{
			return;
		}

		MinimapAreas.Add(FMinimapArea::WholeArea());
		for (auto* Node : Nodes)
		{
			if (auto* CommentNode = Cast<UEdGraphNode_Comment>(Node))
			{
				MinimapAreas.Add(MakeShared<FMinimapArea>(CommentNode));
			}
		}
	}

	const FSlateBrush* SGraphMinimap::GetModeIconImage() const
	{
		switch (GetState())
		{
		case EGraphMinimapState::Controllable:
			return FGraphMinimapStyle::GetBrushFromIconType(EGraphMinimapStyleIconType::Controllable);

		case EGraphMinimapState::Visible:
			return FGraphMinimapStyle::GetBrushFromIconType(EGraphMinimapStyleIconType::Visible);

		case EGraphMinimapState::Resizable:
			return FGraphMinimapStyle::GetBrushFromIconType(EGraphMinimapStyleIconType::Resizable);
		
		default:
			return nullptr;
		}
	}

	FText SGraphMinimap::GetSizeAndScaleText() const
	{
		const FVector2D& ScaledGraphSize = GetScaledGraphSize();
		const FString SizeAndScaleString = FString::Printf(
			TEXT("Scale %.02f\r\nSize %.02f : %.02f"),
			GetRenderingScale(),
			ScaledGraphSize.X, ScaledGraphSize.Y
		);
		
		return FText::FromString(SizeAndScaleString);
	}

	EVisibility SGraphMinimap::GetSizeAndScaleTextVisibility() const
	{
		return (
			bDrawSizeAndScale ?
			EVisibility::SelfHitTestInvisible :
			EVisibility::Hidden
		);
	}

	FOptionalSize SGraphMinimap::GetMinimapAreaPanelWidth() const
	{
		return (Size.X / 3.f);
	}

	EVisibility SGraphMinimap::GetMinimapAreaPanelVisibility() const
	{
		if (HasMultipleMinimapArea() && UGraphMinimapSettings::Get().bShowMinimapArea)
		{
			return EVisibility::Visible;
		}

		return EVisibility::Collapsed;
	}

	void SGraphMinimap::HandleOnSelectionChanged(TSharedPtr<FMinimapArea> InSelectedItem, ESelectInfo::Type SelectInfo)
	{
		SetSelectedMinimapArea(InSelectedItem);
		
		if (MinimapAreaName.IsValid())
		{
			MinimapAreaName->ResetScrollState();
		}
	}

	TSharedRef<SWidget> SGraphMinimap::HandleOnGenerateWidget(TSharedPtr<FMinimapArea> InItem) const
	{
		return SNew(STextBlock)
			.Text(GetSelectedMinimapAreaText(InItem));
	}

	FText SGraphMinimap::GetSelectedMinimapAreaText(TSharedPtr<FMinimapArea> InItem)
	{
		if (InItem.IsValid())
		{
			return InItem->GetName();
		}

		return LOCTEXT("InvalidMinimapAreaTextError", "Invalid Data");
	}

	FText SGraphMinimap::GetSelectedMinimapAreaText() const
	{
		return GetSelectedMinimapAreaText(SelectedMinimapArea);
	}

	FText SGraphMinimap::GetDrawPrepareErrorText() const
	{
		SGraphRenderer::EDrawPrepareError DrawPrepareError = SGraphRenderer::EDrawPrepareError::NoError;
		if (GraphRenderer.IsValid())
		{
			DrawPrepareError = GraphRenderer->GetDrawPrepareError();
		}
		
		FText ErrorMessage;
		if (DrawPrepareError == SGraphRenderer::EDrawPrepareError::NoNode)
		{
			ErrorMessage = LOCTEXT("NoNodeError", "There are no nodes.");
		}
		else if (DrawPrepareError == SGraphRenderer::EDrawPrepareError::TooBigGraph)
		{
			ErrorMessage = LOCTEXT("TooBigGraphError", "The graph is too big.");
		}

		return ErrorMessage;
	}

	EVisibility SGraphMinimap::GetDrawPrepareErrorTextVisibility() const
	{
		SGraphRenderer::EDrawPrepareError DrawPrepareError = SGraphRenderer::EDrawPrepareError::NoError;
		if (GraphRenderer.IsValid())
		{
			DrawPrepareError = GraphRenderer->GetDrawPrepareError();
		}
		
		return (
			DrawPrepareError != SGraphRenderer::EDrawPrepareError::NoError ?
			EVisibility::SelfHitTestInvisible :
			EVisibility::Hidden
		);
	}

	void SGraphMinimap::HandleOnMinimapAlignmentChanged(const EHorizontalAlignment HAlign, const EVerticalAlignment VAlign)
	{
		if (Slot != nullptr)
		{
#if UE_5_00_OR_LATER
			Slot->SetHorizontalAlignment(HAlign);
			Slot->SetVerticalAlignment(VAlign);
#else
			Slot->HAlign(HAlign);
			Slot->VAlign(VAlign);
#endif
		}
	}

	void SGraphMinimap::HandleOnMinimapOpacityChanged(const float NewMinimapOpacity)
	{
		SetMinimapOpacity(NewMinimapOpacity);
	}

	void SGraphMinimap::HandleOnMinimapTintColorChanged(const FLinearColor& NewMinimapTintColor)
	{
		if (GraphRenderer.IsValid())
		{
			GraphRenderer->SetColorAndOpacity(NewMinimapTintColor);
		}
	}

	void SGraphMinimap::HandleOnModeIconSizeChanged(const float NewModeIconSize)
	{
		if (ModeIconPanel.IsValid())
		{
			ModeIconPanel->SetHeightOverride(NewModeIconSize);
			ModeIconPanel->SetWidthOverride(NewModeIconSize);
		}
	}

	void SGraphMinimap::HandleOnModeIconTintColorChanged(const FLinearColor& NewModeIconTintColor)
	{
		if (ModeIcon.IsValid())
		{
			ModeIcon->SetColorAndOpacity(NewModeIconTintColor);
		}
	}

	void SGraphMinimap::HandleOnDragSensitivityChanged(const float NewDragSensitivity)
	{
		DragSensitivity = NewDragSensitivity;
	}

	void SGraphMinimap::HandleOnDrawSizeAndScaleChanged(const bool bNewDrawSizeAndScale)
	{
		bDrawSizeAndScale = bNewDrawSizeAndScale;
	}

	void SGraphMinimap::HandleOnSizeAndScaleTintColorChanged(const FLinearColor& NewSizeAndScaleTintColor)
	{
		if (SizeAndScaleText.IsValid())
		{
			SizeAndScaleText->SetColorAndOpacity(NewSizeAndScaleTintColor);
		}
	}

	void SGraphMinimap::HandleOnShowMinimapAreaChanged(const bool bNewShowMinimapArea)
	{
		bShowMinimapArea = bNewShowMinimapArea;
	}

	void SGraphMinimap::HandleOnMinimapAreaOpacityChanged(const float NewMinimapAreaOpacity)
	{
		if (MinimapAreaPanel.IsValid())
		{
			MinimapAreaPanel->SetRenderOpacity(NewMinimapAreaOpacity);
		}
	}

	void SGraphMinimap::HandleOnPreRendering()
	{
		SetVisibility(EVisibility::Collapsed);
	}

	void SGraphMinimap::HandleOnPostRendering()
	{
		UpdateMinimapVisibility();
	}

	void SGraphMinimap::NotifyGraphMinimapConfigChanged()
	{
		const TOptional<FString> GraphId = FGraphMinimapUtils::GetGraphIdentificationString(OwnerGraphEditor);
		if (OnGraphMinimapConfigChanged.IsBound() && GraphId.IsSet())
		{
			FGraphMinimapConfig Config;
			Config.State = GetState();
			Config.Size = GetMinimapSize();
			Config.RenderingScale = GetRenderingScale();
			if (SelectedMinimapArea.IsValid())
			{
				Config.SelectedMinimapArea = SelectedMinimapArea->ToIdentifier();
			}
				
			OnGraphMinimapConfigChanged.Execute(GraphId.GetValue(), Config);
		}
	}
}

#undef LOCTEXT_NAMESPACE
