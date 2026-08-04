// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "GraphEditor.h"
#include "Widgets/SOverlay.h"
#include "SGraphPanel.h"

namespace GraphMinimap
{
	class SGraphMinimap;
	
	/**
	 * A utility class that defines utility functions used in this plugin.
	 */
	class GRAPHMINIMAP_API FGraphMinimapUtils
	{
	public:
		// Enumerates all widgets that are children of SearchTarget.
		// if Predicate returns false, stop recursive process.
		static void EnumerateChildWidgets(
			const TSharedPtr<SWidget>& SearchTarget,
			const TFunction<bool(const TSharedPtr<SWidget>& ChildWidget)>& Predicate
		);

		// Enumerates all widgets that are parents of SearchTarget.
		// if Predicate returns false, stop recursive process.
		static void EnumerateParentWidgets(
			const TSharedPtr<SWidget>& SearchTarget,
			const TFunction<bool(const TSharedPtr<SWidget>& ParentWidget)>& Predicate
		);

		// Finds the docking tab stack that contains the specified dock tab.
		static TSharedPtr<SDockingTabStack> FindNearestParentDockingTabStack(const TSharedPtr<SDockTab>& SearchTarget);
		
		// Finds the widget that is the graph editor and nearest child of SearchTarget.
		static TSharedPtr<SGraphEditor> FindNearestChildGraphEditor(const TSharedPtr<SWidget>& SearchTarget);

		// Finds the widget that is the overlay and nearest parent of SearchTarget.
		static TSharedPtr<SOverlay> FindNearestChildOverlay(const TSharedPtr<SWidget>& SearchTarget);

		// Finds the widget that is the graph panel and nearest child of SearchTarget.
		static TSharedPtr<SGraphPanel> FindNearestChildGraphPanel(const TSharedPtr<SWidget>& SearchTarget);
		
		// Returns the graph editor contained in the currently active tab.
		static TSharedPtr<SGraphEditor> GetActiveGraphEditor();
		
		// Returns an identification string that identifies the graph editor.
		static TOptional<FString> GetGraphIdentificationString(const TSharedPtr<SGraphEditor>& GraphEditor);

		// Returns the graph minimap contained in the specified overlay.
		static TSharedPtr<SGraphMinimap> FindGraphMinimap(const TSharedPtr<SOverlay>& SearchTarget);

		// Returns the graph minimap contained in the currently active tab.
		static TSharedPtr<SGraphMinimap> GetActiveGraphMinimap();

		// Returns the displayed text blocks that are children of SearchTarget.
		static TArray<TSharedPtr<STextBlock>> GetVisibleChildTextBlocks(const TSharedPtr<SWidget>& SearchTarget);

		// Creates an FKeyEvent from FUICommandInfo.
		static bool GetKeyEventFromUICommandInfo(
			FKeyEvent& OutKeyEvent,
			const TSharedPtr<FUICommandInfo>& UICommandInfo,
			TOptional<FModifierKeysState> ModifierKeysOverride = {}
		);
	};
}
