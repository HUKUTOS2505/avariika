// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/Utilities/GraphMinimapUtils.h"
#include "GraphMinimap/Widgets/SGraphMinimap.h"
#include "SGraphEditorImpl.h"
#include "EdGraph/EdGraph.h"
#include "Framework/Application/SlateApplication.h"
#include "Input/Events.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "MaterialGraph/MaterialGraph.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"

namespace GraphMinimap
{
	namespace Private
	{
		/**
		 * Cast function for classes that inherit from SWidget.
		 */
		template<class To, class From>
		TSharedPtr<To> CastSlateWidget(TSharedPtr<From> FromPtr, const FName& ToClassName)
		{
			static_assert(TIsDerivedFrom<From, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");
			static_assert(TIsDerivedFrom<To, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");

			if (FromPtr.IsValid())
			{
				if (FromPtr->GetType() == ToClassName)
				{
					return StaticCastSharedPtr<To>(FromPtr);
				}
			}

			return nullptr;
		}
	}
	
#define GM_CAST_SLATE_WIDGET(ToClass, FromPtr) GraphMinimap::Private::CastSlateWidget<ToClass>(FromPtr, #ToClass)
	
	void FGraphMinimapUtils::EnumerateChildWidgets(
		const TSharedPtr<SWidget>& SearchTarget,
		const TFunction<bool(const TSharedPtr<SWidget>& ChildWidget)>& Predicate
	)
	{
		if (!SearchTarget.IsValid())
		{
			return;
		}

		FChildren* Children = SearchTarget->GetChildren();
		if (Children == nullptr)
		{
			return;
		}

		for (int32 Index = 0; Index < Children->Num(); Index++)
		{
			TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
			if (ChildWidget.IsValid())
			{
				if (Predicate(ChildWidget))
				{
					EnumerateChildWidgets(ChildWidget, Predicate);
				}
			}
		}
	}

	void FGraphMinimapUtils::EnumerateParentWidgets(
		const TSharedPtr<SWidget>& SearchTarget,
		const TFunction<bool(const TSharedPtr<SWidget>& ParentWidget)>& Predicate
	)
	{
		if (!SearchTarget.IsValid())
		{
			return;
		}
			
		const TSharedPtr<SWidget> ParentWidget = SearchTarget->GetParentWidget();
		if (!ParentWidget.IsValid())
		{
			return;
		}

		if (Predicate(ParentWidget))
		{
			EnumerateParentWidgets(ParentWidget, Predicate);
		}
	}

	TSharedPtr<SDockingTabStack> FGraphMinimapUtils::FindNearestParentDockingTabStack(const TSharedPtr<SDockTab>& SearchTarget)
	{
		TSharedPtr<SDockingTabStack> FoundDockingTabStack = nullptr;
		
		EnumerateParentWidgets(
			SearchTarget,
			[&FoundDockingTabStack](const TSharedPtr<SWidget>& ParentWidget) -> bool
			{
				const TSharedPtr<SDockingTabStack> DockingTabStack = GM_CAST_SLATE_WIDGET(SDockingTabStack, ParentWidget);
				if (DockingTabStack.IsValid())
				{
					FoundDockingTabStack = DockingTabStack;
					return false;
				}

				return true;
			}
		);

		return FoundDockingTabStack;
	}

	TSharedPtr<SGraphEditor> FGraphMinimapUtils::FindNearestChildGraphEditor(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SGraphEditor> FoundGraphEditor = nullptr;
		
		EnumerateChildWidgets(
			SearchTarget,
			[&FoundGraphEditor](const TSharedPtr<SWidget>& ChildWidget) -> bool
			{
				const TSharedPtr<SGraphEditor> GraphEditor = GM_CAST_SLATE_WIDGET(SGraphEditorImpl, ChildWidget);
				if (GraphEditor.IsValid())
				{
					FoundGraphEditor = GraphEditor;
					return false;
				}

				return true;
			}
		);

		return FoundGraphEditor;
	}

	TSharedPtr<SOverlay> FGraphMinimapUtils::FindNearestChildOverlay(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SOverlay> FoundOverlay = nullptr;
		
		EnumerateChildWidgets(
			SearchTarget,
			[&FoundOverlay](const TSharedPtr<SWidget>& ChildWidget) -> bool
			{
				const TSharedPtr<SOverlay> Overlay = GM_CAST_SLATE_WIDGET(SOverlay, ChildWidget);
				if (Overlay.IsValid())
				{
					FoundOverlay = Overlay;
					return false;
				}

				return true;
			}
		);

		return FoundOverlay;
	}

	TSharedPtr<SGraphPanel> FGraphMinimapUtils::FindNearestChildGraphPanel(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SGraphPanel> FoundGraphPanel = nullptr;
		
		EnumerateChildWidgets(
			SearchTarget,
			[&FoundGraphPanel](const TSharedPtr<SWidget>& ChildWidget) -> bool
			{
				const TSharedPtr<SGraphPanel> GraphPanel = GM_CAST_SLATE_WIDGET(SGraphPanel, ChildWidget);
				if (GraphPanel.IsValid())
				{
					FoundGraphPanel = GraphPanel;
					return false;
				}

				return true;
			}
		);

		return FoundGraphPanel;
	}

	TSharedPtr<SGraphEditor> FGraphMinimapUtils::GetActiveGraphEditor()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return nullptr;
		}
	
		const TSharedPtr<SDockingTabStack> DockingTabStack = FindNearestParentDockingTabStack(ActiveTab);
		if (!DockingTabStack.IsValid())
		{
			return nullptr;
		}

		return FindNearestChildGraphEditor(DockingTabStack);
	}

	TOptional<FString> FGraphMinimapUtils::GetGraphIdentificationString(const TSharedPtr<SGraphEditor>& GraphEditor)
	{
		if (!GraphEditor.IsValid())
		{
			return {};
		}

		const UEdGraph* Graph = GraphEditor->GetCurrentGraph();
		if (!IsValid(Graph))
		{
			return {};
		}

		// Materials require special handling as the instances used in the graph are separate objects with transients.
		if (auto* MaterialGraph = Cast<UMaterialGraph>(Graph))
		{
			const FString& OriginalMaterialName = MaterialGraph->OriginalMaterialFullName;
			
			check(IsValid(GEditor));
			auto* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
			check(IsValid(AssetEditorSubsystem));
			const TArray<UObject*>& EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();
			UObject* const* FoundAssetPtr = EditedAssets.FindByPredicate(
				[&OriginalMaterialName](const UObject* EditedAsset) -> bool
				{
					if (IsValid(EditedAsset))
					{
						return (OriginalMaterialName == EditedAsset->GetName());
					}

					return false;
				}
			);
			if (FoundAssetPtr != nullptr)
			{
				if (const UObject* FoundAsset = *FoundAssetPtr)
				{
					return FoundAsset->GetPathName();
				}
			}
		}
		
		return Graph->GetPathName();
	}

	TSharedPtr<SGraphMinimap> FGraphMinimapUtils::FindGraphMinimap(const TSharedPtr<SOverlay>& SearchTarget)
	{
		if (!SearchTarget.IsValid())
		{
			return nullptr;
		}

		FChildren* Children = SearchTarget->GetChildren();
		if (Children == nullptr)
		{
			return nullptr;
		}

		for (int32 Index = 0; Index < Children->Num(); Index++)
		{
			const TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
			const TSharedPtr<SGraphMinimap> GraphMinimap = GM_CAST_SLATE_WIDGET(SGraphMinimap, ChildWidget);
			if (GraphMinimap.IsValid())
			{
				return GraphMinimap;
			}
		}

		return nullptr;
	}

	TSharedPtr<SGraphMinimap> FGraphMinimapUtils::GetActiveGraphMinimap()
	{
		const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
		if (!GraphEditor.IsValid())
		{
			return nullptr;
		}

		const TSharedPtr<SOverlay> Overlay = FindNearestChildOverlay(GraphEditor);
		if (!Overlay.IsValid())
		{
			return nullptr;
		}

		return FindGraphMinimap(Overlay);
	}

	TArray<TSharedPtr<STextBlock>> FGraphMinimapUtils::GetVisibleChildTextBlocks(const TSharedPtr<SWidget>& SearchTarget)
	{
		if (!SearchTarget.IsValid())
		{
			return {};
		}

		FChildren* Children = SearchTarget->GetChildren();
		if (Children == nullptr)
		{
			return {};
		}

		TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks;
		for (int32 Index = 0; Index < Children->Num(); Index++)
		{
			const TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
			TSharedPtr<STextBlock> TextBlock = GM_CAST_SLATE_WIDGET(STextBlock, ChildWidget);
			if (TextBlock.IsValid())
			{
				if (TextBlock->GetVisibility().IsVisible())
				{
					VisibleChildTextBlocks.Add(TextBlock);
				}
			}
		}
		
		return VisibleChildTextBlocks;
	}

	bool FGraphMinimapUtils::GetKeyEventFromUICommandInfo(
		FKeyEvent& OutKeyEvent,
		const TSharedPtr<FUICommandInfo>& UICommandInfo,
		TOptional<FModifierKeysState> ModifierKeysOverride /* = {} */
	)
	{
		check(UICommandInfo.IsValid());
	
		const TSharedRef<const FInputChord>& Chord = UICommandInfo->GetFirstValidChord();

		FModifierKeysState ModifierKeys(
			Chord->bShift, Chord->bShift,
			Chord->bCtrl, Chord->bCtrl,
			Chord->bAlt, Chord->bAlt,
			Chord->bCmd, Chord->bCmd,
			false
		);
		if (ModifierKeysOverride.IsSet())
		{
			ModifierKeys = ModifierKeysOverride.GetValue();
		}
		
		const uint32* CharacterCodePtr;
		const uint32* KeyCodePtr;
		FInputKeyManager::Get().GetCodesFromKey(Chord->Key, CharacterCodePtr, KeyCodePtr);
		const uint32 CharacterCode = (CharacterCodePtr != nullptr ? *CharacterCodePtr : 0);
		const uint32 KeyCode = (KeyCodePtr != nullptr ? *KeyCodePtr : 0);
		const FKeyEvent KeyEvent(Chord->Key, ModifierKeys, FSlateApplication::Get().GetUserIndexForKeyboard(), false, CharacterCode, KeyCode);
		OutKeyEvent = KeyEvent;

		return true;
	}

#undef GM_CAST_SLATE_WIDGET
}
