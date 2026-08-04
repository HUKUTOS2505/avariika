// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SOverlay.h"
#include "Types/SlateStructs.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "GraphMinimap/Types/GraphMinimapState.h"
#include "GraphMinimap/Types/GraphMinimapConfig.h"
#include "GraphMinimap/Types/MinimapArea.h"

class FWidgetRenderer;
class UEdGraphNode_Comment;
class SBox;
class SImage;
class STextBlock;
class SGraphEditor;
class SGraphPanel;

namespace GraphMinimap
{
	class SGraphRenderer;
	class SAutoScrollTextBlock;
	
	/**
	 * A widget class that displays a minimap for the specified graph editor.
	 */
	class GRAPHMINIMAP_API SGraphMinimap : public SCompoundWidget
	{
	public:
		// Defines an event that will be called when the config of the minimap changes.
		DECLARE_DELEGATE_TwoParams(FOnGraphMinimapConfigChanged, const FString& /* GraphId */, const FGraphMinimapConfig& /* GraphMinimapConfig */);
	
	public:
		SLATE_BEGIN_ARGS(SGraphMinimap)
			: _InitialState(EGraphMinimapState::Hidden)
			, _MinimapSize(Global::MinimumMinimapSize)
			, _RenderingScale(1.f)
		{}

		// Sets the state of the initial minimap.
		SLATE_ARGUMENT(EGraphMinimapState, InitialState)

		// Sets the size of the minimap to draw.
		SLATE_ARGUMENT(FVector2D, MinimapSize)

		// Sets the scale for drawing the graph.
		SLATE_ARGUMENT(float, RenderingScale)

		// Sets the identifier for the selected minimap area.
		SLATE_ATTRIBUTE(FMinimapAreaIdentifier, SelectedMinimapArea)

		// Sets the alignment of the minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(EHorizontalAlignment, HAlign)
		SLATE_ATTRIBUTE(EVerticalAlignment, VAlign)

		// Sets the opacity of the minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(float, MinimapOpacity)

		// Sets the tint color of the minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(FSlateColor, MinimapTintColor)

		// Sets the size of the mode icon displayed in the upper left of the minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(float, ModeIconSize)

		// Sets the tint color of the mode icon displayed in the upper left of the minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(FSlateColor, ModeIconTintColor)

		// Sets the mouse sensitivity when dragging.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(float, DragSensitivity)

		// Sets whether to draw the drawing size and scale of the graph on the graph minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(bool, bDrawSizeAndScale)

		// Sets the tint color of the text of size and scale of the graph on the graph minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(FLinearColor, SizeAndScaleTintColor)

		// Sets whether to show the name of the currently displayed minimap area on the graph minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(bool, bShowMinimapArea)

		// Sets the opacity of the name of the currently displayed minimap area on the graph minimap.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(float, MinimapAreaOpacity)
		
		// Called when the config of the minimap changes.
		SLATE_EVENT(FOnGraphMinimapConfigChanged, OnGraphMinimapConfigChanged)

		SLATE_END_ARGS()

		// Constructor.
		void Construct(
			const FArguments& InArgs,
			const TSharedPtr<SGraphEditor>& InOwnerGraphEditor,
#if UE_5_00_OR_LATER
			SOverlay::FScopedWidgetSlotArguments* InSlot
#else
			SOverlay::FOverlaySlot* InSlot
#endif
		);

		// SWidget interface.
		virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
		virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
		virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
		virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
		virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
		// End of SWidget interface.
		
		// Returns the graph editor to draw.
		TSharedPtr<SGraphEditor> GetOwnerGraphEditor() const;

		// Sets the graph editor to draw.
		void SetOwnerGraphEditor(const TSharedPtr<SGraphEditor>& GraphEditor);
		
		// Returns current minimap state.
		EGraphMinimapState GetState() const;
		
		// Changes current minimap state to new state.
		void ChangeState(const EGraphMinimapState NewState);

		// Returns current minimap size.
		FVector2D GetMinimapSize() const;
		
		// Sets the size of the minimap to draw.
		void SetMinimapSize(const FVector2D& NewSize);

		// Returns the size of the graph.
		FVector2D GetGraphSize() const;

		// Returns the scaled size of the graph.
		FVector2D GetScaledGraphSize() const;
		
		// Returns the scale for drawing the graph.
		float GetRenderingScale() const;

		// Sets the scale for drawing the graph.
		void SetRenderingScale(const float NewRenderingScale);
		
 		// Sets the opacity of the minimap.
		void SetMinimapOpacity(const float NewOpacity);
		
		// Updates the visibility of the minimap.
		void UpdateMinimapVisibility();
		
		// Update sthe opacity of the minimap.
		void UpdateMinimapOpacity();
		
		// Returns whether the graph being drawn has multiple minimap areas.
		bool HasMultipleMinimapArea() const;

		// Returns a list of identifier for the minimap area.
		TArray<FMinimapAreaIdentifier> GetMinimapAreaIdentifiers() const;

		// Returns the minimap area identifier currently drawing.
		FMinimapAreaIdentifier GetSelectedMinimapAreaIdentifier() const;

		// Sets the minimap area identifier currently drawing.
		void SetSelectedMinimapAreaIdentifier(const FMinimapAreaIdentifier& Identifier);
	
	private:
		// Sets the minimap area currently drawing.
		void SetSelectedMinimapArea(const TSharedPtr<FMinimapArea>& MinimapArea);
		
		// Resizes processing for scaling the minimap.
		void ControlSizeProcess(const FVector2D& AdditionalSize);
		void ControlRenderingScaleProcess(const float WheelDelta);

		// Dragging processing for control view location or zoom amount.
		void ControlViewLocationProcess(const FVector2D& RelativePosition);
		void ControlZoomAmountProcess(const float WheelDelta);
		
		// Collects minimap areas in the graph to be drawn.
		void CollectMinimapAreas();
		
		// Returns the icon according to the current minimap mode.
		const FSlateBrush* GetModeIconImage() const;

		// Returns the size and scale text and its visibility.
		FText GetSizeAndScaleText() const;
		EVisibility GetSizeAndScaleTextVisibility() const;

		// Returns the width of the minimap area panel widget.
		FOptionalSize GetMinimapAreaPanelWidth() const;
		
		// Returns the visibility of the minimap area panel widget.
		EVisibility GetMinimapAreaPanelVisibility() const;
		
		// Called when the selected minimap area has changed.
		void HandleOnSelectionChanged(TSharedPtr<FMinimapArea> InSelectedItem, ESelectInfo::Type SelectInfo);

		// Called when creating the minimap area selection widget.
		TSharedRef<SWidget> HandleOnGenerateWidget(TSharedPtr<FMinimapArea> InItem) const;
		
		// Returns the name of the minimap area.
		static FText GetSelectedMinimapAreaText(TSharedPtr<FMinimapArea> InItem);
		FText GetSelectedMinimapAreaText() const;
		
		// Returns the draw prepare error text and its visibility.
		FText GetDrawPrepareErrorText() const;
		EVisibility GetDrawPrepareErrorTextVisibility() const;
		
		// Callback functions that are called when the editor settings change.
		void HandleOnMinimapAlignmentChanged(const EHorizontalAlignment HAlign, const EVerticalAlignment VAlign);
		void HandleOnMinimapOpacityChanged(const float NewMinimapOpacity);
		void HandleOnMinimapTintColorChanged(const FLinearColor& NewMinimapTintColor);
		void HandleOnModeIconSizeChanged(const float NewModeIconSize);
		void HandleOnModeIconTintColorChanged(const FLinearColor& NewModeIconTintColor);
		void HandleOnDragSensitivityChanged(const float NewDragSensitivity);
		void HandleOnDrawSizeAndScaleChanged(const bool bNewDrawSizeAndScale);
		void HandleOnSizeAndScaleTintColorChanged(const FLinearColor& NewSizeAndScaleTintColor);
		void HandleOnShowMinimapAreaChanged(const bool bNewShowMinimapArea);
		void HandleOnMinimapAreaOpacityChanged(const float NewMinimapAreaOpacity);

		// Called when before and after rendering the graph renderer.
		void HandleOnPreRendering();
		void HandleOnPostRendering();
		
		// Notifies that changes have been made to the data saved in the minimap.
		void NotifyGraphMinimapConfigChanged();
		
	private:
		// The overlay slot to which this widget is applied.
		SOverlay::FOverlaySlot* Slot = nullptr;
		
		// The panel that determines the size of the minimap used internally.
		TSharedPtr<SBox> MinimapPanel;
		
		// The widget that draws the graph editor used internally.
		TSharedPtr<SGraphRenderer> GraphRenderer;

		// The panel that determines the size of the mode icon used internally.
		TSharedPtr<SBox> ModeIconPanel;

		// The widget that draws the mode icon used internally.
		TSharedPtr<SImage> ModeIcon;

		// The widget that draws the text of size and scale of the graph used internally.
		TSharedPtr<STextBlock> SizeAndScaleText;
		
		// The panel that determines the size of the minimap area used internally.
		TSharedPtr<SBox> MinimapAreaPanel;

		// The widget that displays the name of the minimap area.
		TSharedPtr<SAutoScrollTextBlock> MinimapAreaName;
		
		// The graph editor to draw.
		TSharedPtr<SGraphEditor> OwnerGraphEditor;

		// The graph panel included in the owner graph editor.
		TSharedPtr<SGraphPanel> OwnerGraphPanel;
		
		// The current state of the minimap.
		TOptional<EGraphMinimapState> State;

		// The current size of the minimap.
		FVector2D Size;
		
		// The opacity of the minimap.
		float MinimapOpacity = 0.f;
		
		// The event called when the config of the minimap changes.
		FOnGraphMinimapConfigChanged OnGraphMinimapConfigChanged;

		// Whether you are dragging.
		bool bIsDragging = false;

		// The mouse sensitivity when dragging.
		float DragSensitivity = 1.f;

		// Whether to draw the drawing size and scale of the graph on the graph minimap.
		bool bDrawSizeAndScale = true;

		// Whether to show the name of the currently displayed minimap area on the graph minimap.
		bool bShowMinimapArea = true;
		
		// The list of minimap areas in the graph to draw.
		TArray<TSharedPtr<FMinimapArea>> MinimapAreas;

		// The minimap area currently drawing.
		TSharedPtr<FMinimapArea> SelectedMinimapArea;
	};
}
