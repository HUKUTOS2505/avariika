// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/Widgets/SGraphRenderer.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "GraphMinimap/Utilities/GraphMinimapSettings.h"
#include "GraphMinimap/Utilities/GraphMinimapUtils.h"
#include "GraphMinimap/Types/MinimapArea.h"
#include "Slate/WidgetRenderer.h"
#include "RenderingThread.h"
#include "Framework/Application/SlateApplication.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphUtilities.h"
#include "EdGraphNode_Comment.h"

#define LOCTEXT_NAMESPACE "GraphRenderer"

namespace GraphMinimap
{
	namespace GraphRendererDefine
	{
		// The number of attempts to draw the widget on the render target.
		// If the number of drawing attempts is one,
		// the drawing result will be corrupted, so draw multiple times.
		static constexpr int32 DrawTimes = 2;
	}
	
	void SGraphRenderer::Construct(const FArguments& InArgs)
	{
		SetSourceGraphEditor(InArgs._SourceGraphEditor);
		OwnerDesiredSize = InArgs._OwnerDesiredSize;
		OnPreRendering = InArgs._OnPreRendering;
		OnPostRendering = InArgs._OnPostRendering;

		if (InArgs._Padding.IsSet())
		{
			Padding = InArgs._Padding.Get();
		}
		else
		{
			Padding = UGraphMinimapSettings::Get().Padding;
			UGraphMinimapSettings::Get().OnPaddingChanged.AddRaw(this, &SGraphRenderer::HandleOnPaddingChanged);
		}

		SetRenderingScale(InArgs._RenderingScale);
		bDrawCameraBounds = InArgs._DrawCameraBounds;
		
		if (InArgs._CameraBoundsColor.IsSet())
		{
			CameraBoundsColor = InArgs._CameraBoundsColor.Get();
		}
		else
		{
			CameraBoundsColor = UGraphMinimapSettings::Get().CameraBoundsColor;
			UGraphMinimapSettings::Get().OnCameraBoundsColorChanged.AddRaw(this, &SGraphRenderer::HandleOnCameraBoundsColorChanged);
		}
		
		if (InArgs._CameraBoundsThickness.IsSet())
		{
			CameraBoundsThickness = InArgs._CameraBoundsThickness.Get();
		}
		else
		{
			CameraBoundsThickness = UGraphMinimapSettings::Get().CameraBoundsThickness;
			UGraphMinimapSettings::Get().OnCameraBoundsThicknessChanged.AddRaw(this, &SGraphRenderer::HandleOnCameraBoundsThicknessChanged);
		}
		
		check(WidgetRenderer == nullptr);
		WidgetRenderer = new FWidgetRenderer();
		
		SImage::Construct(
			SImage::FArguments()
			.Image(&RenderedGraphBrush)
			.ColorAndOpacity(InArgs._ColorAndOpacity)
		);

		UpdateRenderTarget(FVector2D(1.f, 1.f));
		
		// In SWidget::Tick and FSlateApplication::OnPostTick, drawing is done with OnPreTick
		// because the processing conflicts in the thread and drawing cannot be done well.
		FSlateApplication::Get().OnPreTick().AddRaw(this, &SGraphRenderer::DrawGraphToRenderTarget);
	}

	SGraphRenderer::~SGraphRenderer()
	{
		FSlateApplication::Get().OnPreTick().RemoveAll(this);
		
		if (WidgetRenderer != nullptr)
		{
			BeginCleanup(WidgetRenderer);
			WidgetRenderer = nullptr;
		}
	}

	TSharedPtr<SGraphEditor> SGraphRenderer::GetSourceGraphEditor() const
	{
		return SourceGraphEditor;
	}

	void SGraphRenderer::SetSourceGraphEditor(const TSharedPtr<SGraphEditor>& InSourceGraphEditor)
	{
		SourceGraphEditor = InSourceGraphEditor;
		SourceGraphPanel = FGraphMinimapUtils::FindNearestChildGraphPanel(SourceGraphEditor);
	}

	void SGraphRenderer::SetOwnerDesiredSize(const TAttribute<FVector2D>& NewOwnerDesiredSize)
	{
		OwnerDesiredSize = NewOwnerDesiredSize;
	}

	FVector2D SGraphRenderer::GetGraphSize() const
	{
		return GraphSize;
	}

	FVector2D SGraphRenderer::GetScaledGraphSize() const
	{
		return (GetGraphSize() * GetRenderingScale());
	}

	FVector2D SGraphRenderer::GetDesiredGraphSize() const
	{
		return OwnerDesiredSize.Get(GetGraphSize());
	}

	FVector2D SGraphRenderer::GetViewLocation(const FVector2D& CameraCenterPosition) const
	{
		const FVector2D NormalizedPosition = CameraCenterPosition / GetDesiredGraphSize();
		const FVector2D PositionOnGraph = GraphSize * NormalizedPosition;
		const FVector2D ViewLocation = GraphOrigin + PositionOnGraph;
		FVector2D CenterOffset = FVector2D::ZeroVector;
		if (SourceGraphEditor.IsValid())
		{
			const FVector2D ViewSize = SourceGraphEditor->GetTickSpaceGeometry().GetLocalSize() / CachedZoomAmount;
			CenterOffset = ViewSize / 2.f;
		}
		return (ViewLocation - CenterOffset);
	}

	float SGraphRenderer::GetRenderingScale() const
	{
		return RenderingScale;
	}

	void SGraphRenderer::SetRenderingScale(const float NewRenderingScale)
	{
		RenderingScale = FMath::Clamp(NewRenderingScale, 0.1f, 1.f);
	}

	void SGraphRenderer::SetSelectedMinimapArea(const TSharedPtr<FMinimapArea>& MinimapArea)
	{
		SelectedMinimapArea = MinimapArea;
	}

	SGraphRenderer::EDrawPrepareError SGraphRenderer::GetDrawPrepareError() const
	{
		return DrawPrepareError;
	}

	void SGraphRenderer::UpdateRenderTarget(const FVector2D& RenderTargetSize)
	{
		if (RenderTargetSize.X == 0.f || RenderTargetSize.Y == 0.f)
		{
			return;
		}

		const uint32 RenderTargetSizeX = FMath::Max(RenderTargetSize.X, 1.f);
		const uint32 RenderTargetSizeY = FMath::Max(RenderTargetSize.Y, 1.f);

		EPixelFormat RequestedFormat = PF_B8G8R8A8;
#if UE_4_26_OR_LATER
		if (FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer())
		{
			RequestedFormat = SlateRenderer->GetSlateRecommendedColorFormat();
		}
#endif
		
		if (!RenderTarget.IsValid())
		{
			RenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(NewObject<UTextureRenderTarget2D>(GetTransientPackage()));
			RenderTarget->ClearColor = FLinearColor::Black;
			RenderTarget->InitCustomFormat(RenderTargetSizeX, RenderTargetSizeY, RequestedFormat, false);
		}
		else
		{
			if (RenderTarget->SizeX != RenderTargetSizeX || RenderTarget->SizeY != RenderTargetSizeY)
			{
				RenderTarget->ClearColor = FLinearColor::Black;
				RenderTarget->InitCustomFormat(RenderTargetSizeX, RenderTargetSizeY, RequestedFormat, false);
				RenderTarget->UpdateResourceImmediate(true);
			}
		}

		RenderedGraphBrush.SetResourceObject(RenderTarget.Get());
		RenderedGraphBrush.SetImageSize(RenderTargetSize);
	}
	
	void SGraphRenderer::DrawGraphToRenderTarget(const float DeltaTime)
	{
		if (!SourceGraphEditor.IsValid())
		{
			return;
		}

		auto& SlateApplication = FSlateApplication::Get();
		
		// The graph is not drawn while the widget is not displayed.
		FWidgetPath WidgetPath;
		if (!SlateApplication.FindPathToWidget(AsShared(), WidgetPath, EVisibility::All))
		{
			return;
		}

		// The graph is not drawn while the graph editor is not active.
		if (WidgetPath.GetWindow() != SlateApplication.GetActiveTopLevelWindow())
		{
			return;
		}
		if (SourceGraphEditor != FGraphMinimapUtils::GetActiveGraphEditor())
		{
			return;	
		}

		// The graph is not drawn while focusing on a particular node.
		// Wait for the automatic zoom to occur after the focus is finished.
		if (SourceGraphPanel->HasDeferredObjectFocus())
		{
			DeferredObjectFocusTimer = 1.0f;
			return;
		}
		if (DeferredObjectFocusTimer > 0.f)
		{
			DeferredObjectFocusTimer -= DeltaTime;
			return;
		}
		
		// Since the position of camera will be changed, save the original value so that it can be restored.
		FVector2D PreviousViewLocation;
		float PreviousZoomAmount;
		SourceGraphEditor->GetViewLocation(PreviousViewLocation, PreviousZoomAmount);
		
		CachedViewLocation = PreviousViewLocation;
		CachedZoomAmount = PreviousZoomAmount;

		// Move the camera to a position where all nodes can be drawn, and calculate the drawing size.
		const bool bDoesGraphHaveAnyGraph = CalculateGraphSizeAndOrigin();
		const FVector2D ScaledDrawSize = GraphSize * RenderingScale;
		if (!bDoesGraphHaveAnyGraph)
		{
			DrawPrepareError = EDrawPrepareError::NoNode;
		}
#if UE_5_01_OR_LATER
		else if (!ScaledDrawSize.ComponentwiseAllLessOrEqual(UGraphMinimapSettings::Get().MaxGraphSize))
#else
		else if (!(ScaledDrawSize <= UGraphMinimapSettings::Get().MaxGraphSize))
#endif
		{
			DrawPrepareError = EDrawPrepareError::TooBigGraph;
		}
		else
		{
			DrawPrepareError = EDrawPrepareError::NoError;
		}
		
		if (DrawPrepareError != EDrawPrepareError::NoError)
		{
			// If it can't draw, clear the render target.
			if (RenderTarget.IsValid())
			{
				RenderTarget->UpdateResourceImmediate(true);
			}
			
			return;
		}

		SourceGraphEditor->SetViewLocation(GraphOrigin, 1.f);

		// Draw the entire graph on the render target.
		UpdateRenderTarget(GraphSize * RenderingScale);
		if (RenderTarget.IsValid() && WidgetRenderer != nullptr)
		{
			// Hide this minimap only while drawing.
			const EVisibility PreviousMinimapVisibility = GetVisibility();
			SetVisibility(EVisibility::Collapsed);

			// If there is a title bar, hide it only while drawing.
			const TSharedPtr<SWidget> TitleBar = SourceGraphEditor->GetTitleBar();
			TOptional<EVisibility> PreviousTitleBarVisibility;
			if (TitleBar.IsValid())
			{
				PreviousTitleBarVisibility = TitleBar->GetVisibility();
				TitleBar->SetVisibility(EVisibility::Collapsed);
			}

			// Hide zoom magnification and graph type text while drawing.
			TMap<TSharedPtr<STextBlock>, EVisibility> PreviousChildTextBlockVisibilities;
			{
				const TSharedPtr<SOverlay> Overlay = FGraphMinimapUtils::FindNearestChildOverlay(SourceGraphEditor);
				TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks = FGraphMinimapUtils::GetVisibleChildTextBlocks(Overlay);
				for (const TSharedPtr<STextBlock>& VisibleChildTextBlock : VisibleChildTextBlocks)
				{
					if (VisibleChildTextBlock.IsValid())
					{
						PreviousChildTextBlockVisibilities.Add(VisibleChildTextBlock, VisibleChildTextBlock->GetVisibility());
						VisibleChildTextBlock->SetVisibility(EVisibility::Collapsed);
					}
				}
			}
			
			OnPreRendering.ExecuteIfBound();
			
			for (int32 Count = 0; Count < GraphRendererDefine::DrawTimes; Count++)
			{
				WidgetRenderer->DrawWidget(
					RenderTarget.Get(),
					SourceGraphEditor.ToSharedRef(),
					RenderingScale,
					GraphSize,
					DeltaTime
				);
				FlushRenderingCommands();
			}

			OnPostRendering.ExecuteIfBound();

			// Restores the visibility of the title bar,
			// zoom magnification text, and graph type text.
			SetVisibility(PreviousMinimapVisibility);
			
			if (TitleBar.IsValid() && PreviousTitleBarVisibility.IsSet())
			{
				TitleBar->SetVisibility(PreviousTitleBarVisibility.GetValue());
			}

			for (const auto& PreviousChildTextBlockVisibility : PreviousChildTextBlockVisibilities)
			{
				TSharedPtr<STextBlock> TextBlock = PreviousChildTextBlockVisibility.Key;
				EVisibility PreviousVisibility = PreviousChildTextBlockVisibility.Value;
				if (TextBlock.IsValid())
				{
					TextBlock->SetVisibility(PreviousVisibility);
				}
			}
		}
		
		// Restore the camera position.
		SourceGraphEditor->SetViewLocation(PreviousViewLocation, PreviousZoomAmount);
	}
	
	bool SGraphRenderer::CalculateGraphSizeAndOrigin()
	{
		if (!SourceGraphEditor.IsValid())
		{
			return false;
		}
		
		const UEdGraph* Graph = SourceGraphEditor->GetCurrentGraph();
		if (!IsValid(Graph))
		{
			return false;
		}
		
		TWeakObjectPtr<UEdGraphNode_Comment> CommentNode = nullptr;
		if (SelectedMinimapArea.IsValid())
		{
			CommentNode = SelectedMinimapArea->GetCommentNode();
		}

		TArray<UEdGraphNode*> Nodes;
		if (CommentNode.IsValid())
		{
			Nodes.Add(CommentNode.Get());
		}
		else
		{
			Nodes = Graph->Nodes;
			if (Nodes.Num() == 0)
			{
				return false;
			}
		}
		
		// Calculate the range including all nodes or selected minimap area and add the padding.
		// If you use SGraphEditor::SelectAllNodes etc., the graph editor will be focused and
		// the details panel etc. will disappear, so do not use SGraphEditor::GetBoundsForSelectedNodes.
		FIntRect Bounds = FEdGraphUtilities::CalculateApproximateNodeBoundaries(Nodes);
		{
			const float HalfPadding = Padding / 2.f;
			const FIntPoint Margin(HalfPadding, HalfPadding);
			Bounds.Min -= Margin;
			Bounds.Max += Margin;
		}

		GraphSize = Bounds.Size();
		GraphOrigin = Bounds.Min;

		// Adjust the size and camera position to match the OwnerDesiredSize ratio.
		if (OwnerDesiredSize.IsSet())
		{
			const float GraphSizeRatio = GraphSize.X / GraphSize.Y;
			const float DesiredSizeRatio = OwnerDesiredSize.Get().X / OwnerDesiredSize.Get().Y;

			float Multiplier = 1.f;
			if (GraphSizeRatio < DesiredSizeRatio)
			{
				Multiplier = GraphSize.Y / OwnerDesiredSize.Get().Y;
			}
			else if (GraphSizeRatio > DesiredSizeRatio)
			{
				Multiplier = GraphSize.X / OwnerDesiredSize.Get().X;
			}

			const FVector2D OriginalDrawSize = GraphSize;
			GraphSize = OwnerDesiredSize.Get() * Multiplier;
				
			const FVector2D OriginalDrawSizeDiff = GraphSize - OriginalDrawSize;
			GraphOrigin -= OriginalDrawSizeDiff / 2.f;
		}

		return true;
	}

	bool SGraphRenderer::GetGraphViewBounds(FSlateRect& Bounds) const
	{
		if (!SourceGraphEditor.IsValid())
		{
			return false;
		}
		
		FVector2D OwnerDesiredSizeMultiplier = FVector2D(1.f, 1.f);
		if (OwnerDesiredSize.IsSet() && GraphSize != FVector2D::ZeroVector)
		{
			OwnerDesiredSizeMultiplier = OwnerDesiredSize.Get() / GraphSize;
		}
		
		const FVector2D RelativeViewLocation = (CachedViewLocation - GraphOrigin) * OwnerDesiredSizeMultiplier;
		const FVector2D ViewSize = SourceGraphEditor->GetTickSpaceGeometry().GetLocalSize() * OwnerDesiredSizeMultiplier / CachedZoomAmount;
		Bounds = FSlateRect::FromPointAndExtent(RelativeViewLocation, ViewSize);
		
		return true;
	}

	int32 SGraphRenderer::OnPaint(
    	const FPaintArgs& Args,
    	const FGeometry& AllottedGeometry,
    	const FSlateRect& MyCullingRect,
    	FSlateWindowElementList& OutDrawElements,
    	int32 LayerId,
    	const FWidgetStyle& InWidgetStyle,
    	bool bParentEnabled
    ) const
    {
    	const int32 SuperLayerId = SImage::OnPaint(
    		Args,
    		AllottedGeometry,
    		MyCullingRect,
    		OutDrawElements,
    		LayerId,
    		InWidgetStyle,
    		bParentEnabled
    	);
		
		if (DrawPrepareError == EDrawPrepareError::NoError)
		{
			FSlateRect GraphViewBounds;
			if (bDrawCameraBounds && GetGraphViewBounds(GraphViewBounds))
			{
				auto ClampPosition = [&](const FVector2D& Point) -> FVector2D
				{
					const FVector2D DesiredGraphSize = GetDesiredGraphSize();
					FVector2D Result;
					Result.X = FMath::Clamp(Point.X, 0.f, DesiredGraphSize.X);
					Result.Y = FMath::Clamp(Point.Y, 0.f, DesiredGraphSize.Y);
					return Result;
				};
			
				const TArray<FVector2D> Points = {
					ClampPosition(GraphViewBounds.GetTopLeft()),
					ClampPosition(GraphViewBounds.GetTopRight()),
					ClampPosition(GraphViewBounds.GetBottomRight()),
					ClampPosition(GraphViewBounds.GetBottomLeft()),
					ClampPosition(GraphViewBounds.GetTopLeft()),
				};

				const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
				const ESlateDrawEffect DrawEffects = (bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect);
				
				FSlateDrawElement::MakeLines(
					OutDrawElements,
					SuperLayerId,
					AllottedGeometry.ToPaintGeometry(),
					Points,
					DrawEffects,
					CameraBoundsColor,
					true,
					CameraBoundsThickness
				);
			}
		}

    	return SuperLayerId;
    }

	void SGraphRenderer::HandleOnPaddingChanged(const float NewPadding)
	{
		Padding = NewPadding;
	}
	
	void SGraphRenderer::HandleOnCameraBoundsColorChanged(const FLinearColor& NewCameraBoundsColor)
	{
		CameraBoundsColor = NewCameraBoundsColor;
	}

	void SGraphRenderer::HandleOnCameraBoundsThicknessChanged(const float NewCameraBoundsThickness)
	{
		CameraBoundsThickness = NewCameraBoundsThickness;
	}
}

#undef LOCTEXT_NAMESPACE
