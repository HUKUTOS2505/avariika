// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Images/SImage.h"
#include "UObject/StrongObjectPtr.h"
#include "Engine/TextureRenderTarget2D.h"

class FWidgetRenderer;
class SGraphEditor;
class SGraphPanel;

namespace GraphMinimap
{
	struct FMinimapArea;
	
	/**
	 * A widget class to draw the graph editor.
	 */
	class GRAPHMINIMAP_API SGraphRenderer : public SImage
	{
    public:
    	// Kind of drawing preparation error.
		enum class EDrawPrepareError : uint8
		{
			NoError,
			NoNode,
			TooBigGraph,
		};
	
	public:
		SLATE_BEGIN_ARGS(SGraphRenderer)
			: _SourceGraphEditor(nullptr)
			, _ColorAndOpacity(FLinearColor::White)
			, _RenderingScale(1.f)
			, _DrawCameraBounds(true)
		{}

		// Sets the graph editor to draw.
		SLATE_ARGUMENT(TSharedPtr<SGraphEditor>, SourceGraphEditor)

		// Sets the color and opacity.
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

		// Sets the final size to be drawn by widgets that use this widget.
		// You need to set this value if you want to specify the size while preserving the ratio of the graph.
		SLATE_ATTRIBUTE(FVector2D, OwnerDesiredSize)

		// Sets the size of the margin applied when drawing the graph.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(float, Padding)

		// Sets the scale for drawing the graph.
		SLATE_ARGUMENT(float, RenderingScale)

		// Sets whether to draw the camera border on the graph.
		SLATE_ARGUMENT(bool, DrawCameraBounds)

		// Sets the color of camera bounds.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(FLinearColor, CameraBoundsColor)

		// Sets the thickness of camera bounds.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(float, CameraBoundsThickness)
		
		// Called before and after rendering.
		SLATE_EVENT(FSimpleDelegate, OnPreRendering)
		SLATE_EVENT(FSimpleDelegate, OnPostRendering)
		
		SLATE_END_ARGS()

		// Constructor.
		void Construct(const FArguments& InArgs);

		// Destructor.
		virtual ~SGraphRenderer() override;
		
		// Returns the graph editor to draw.
		TSharedPtr<SGraphEditor> GetSourceGraphEditor() const;

		// Sets the graph editor to draw.
		void SetSourceGraphEditor(const TSharedPtr<SGraphEditor>& InSourceGraphEditor);
	
		// Sets the final size to be drawn by widgets that use this widget.
		void SetOwnerDesiredSize(const TAttribute<FVector2D>& NewOwnerDesiredSize);

		// Returns the size of the graph.
		FVector2D GetGraphSize() const;

		// Returns the scaled size of the graph.
		FVector2D GetScaledGraphSize() const;
		
		// Returns the size of the graph on this renderer.
		FVector2D GetDesiredGraphSize() const;
		
		// Returns the camera position on the graph editor from the coordinates on the specified minimap.
		FVector2D GetViewLocation(const FVector2D& CameraCenterPosition) const;

		// Returns the scale for drawing the graph.
		float GetRenderingScale() const;

		// Sets the scale for drawing the graph.
		void SetRenderingScale(float NewRenderingScale);

		// Sets the minimap area currently drawing.
		void SetSelectedMinimapArea(const TSharedPtr<FMinimapArea>& MinimapArea);
		
		// Returns the error type of drawing preparation.
		EDrawPrepareError GetDrawPrepareError() const;
		
	private:
		// Ensures the render target is initialized and updates it if needed.
		void UpdateRenderTarget(const FVector2D& RenderTargetSize);
		
		// Draws a graph containing all the nodes on the render target.
		void DrawGraphToRenderTarget(float DeltaTime);
		
		// Returns the size of the graph if all nodes are included
		// and the position of the camera that can show all the nodes.
		bool CalculateGraphSizeAndOrigin();

		// Returns bounds about the range shown by the current camera.
		bool GetGraphViewBounds(FSlateRect& Bounds) const;
		
        // SWidget interface.
        virtual int32 OnPaint(
            const FPaintArgs& Args,
            const FGeometry& AllottedGeometry,
            const FSlateRect& MyCullingRect,
            FSlateWindowElementList& OutDrawElements,
            int32 LayerId,
            const FWidgetStyle& InWidgetStyle,
            bool bParentEnabled)
        const override;
        // End of SWidget interface.

		// Callback functions that are called when the editor settings change.
		void HandleOnPaddingChanged(const float NewPadding);
		void HandleOnCameraBoundsColorChanged(const FLinearColor& NewCameraBoundsColor);
		void HandleOnCameraBoundsThicknessChanged(const float NewCameraBoundsThickness);
		
	private:
		// The graph editor widget to draw.
		TSharedPtr<SGraphEditor> SourceGraphEditor;

		// The graph panel widget included in the graph editor to draw.
		TSharedPtr<SGraphPanel> SourceGraphPanel;
		
		// The helper class for drawing widgets to a render target.
		FWidgetRenderer* WidgetRenderer = nullptr;
		
		// The render target on which the graph is drawn.
		TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget;
		
		// The brush used to draw the drawn render target in SImage.
		FSlateBrush RenderedGraphBrush;

		// The final size to be drawn by widgets that use this widget.
		TAttribute<FVector2D> OwnerDesiredSize;

		// The actual size of the graph cached when CalculateGraphSizeAndOrigin.
		FVector2D GraphSize;

		// The origin when the entire graph cached at the time of CalculateGraphSizeAndOrigin is stored.
		// It is also the upper left coordinate of the minimap.
		FVector2D GraphOrigin;

		// The current camera position cached when DrawGraphToRenderTarget.
		FVector2D CachedViewLocation;

		// The current zoom amount cached when DrawGraphToRenderTarget.
		float CachedZoomAmount = 1.f;

		// The size of the margin applied when drawing the graph.
		float Padding = 0.f;

		// The minimap drawing scale.
		// If the scale for drawing the graph is 0.5, it will be drawn at half the resolution.
		float RenderingScale = 1.f;
		
		// Whether to draw the camera border on the graph.
		bool bDrawCameraBounds = true;

		// Camera bounds color and thickness.
		FLinearColor CameraBoundsColor;
		float CameraBoundsThickness = 1.f;

		// Events called before and after rendering.
		FSimpleDelegate OnPreRendering;
		FSimpleDelegate OnPostRendering;

		// The minimap area currently drawing.
		TSharedPtr<FMinimapArea> SelectedMinimapArea;
		
		// The error type of drawing preparation.
		EDrawPrepareError DrawPrepareError = EDrawPrepareError::NoError;

		// The waiting time for normal zooming after the node has finished focusing.
		float DeferredObjectFocusTimer = 0.f;
	};
}
