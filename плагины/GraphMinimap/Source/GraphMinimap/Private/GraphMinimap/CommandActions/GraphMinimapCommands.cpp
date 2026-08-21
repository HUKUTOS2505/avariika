// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/CommandActions/GraphMinimapCommands.h"
#include "GraphMinimap/CommandActions/GraphMinimapCommandActions.h"
#include "GraphMinimap/Utilities/GraphMinimapStyle.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "GraphMinimapCommands"

namespace GraphMinimap
{
	FGraphMinimapCommands::FGraphMinimapCommands()
		: TCommands<FGraphMinimapCommands>
		(
			TEXT("GraphMinimap"),
			LOCTEXT("Contexts", "Graph Minimap"),
			NAME_None,
			FGraphMinimapStyle::Get().GetStyleSetName()
		)
		, CommandBindings(MakeShared<FUICommandList>())
		, bIsBound(false)
	{
	}

	void FGraphMinimapCommands::RegisterCommands()
	{
		UI_COMMAND(
			ChangeMinimapState,
			"Change Minimap State",
			"Changes the state of the minimap in the currently active graph editor.",
			EUserInterfaceActionType::None,
			FInputChord(EKeys::M, false, true, false, false)
		);

		UI_COMMAND(
			ChangeMinimapArea,
			"Change Minimap Area",
			"Changes the minimap area in the currently active graph editor.",
			EUserInterfaceActionType::None,
			FInputChord(EKeys::N, false, true, false, false)
		);
	}

	bool FGraphMinimapCommands::IsBound()
	{
		return Instance.Pin()->bIsBound;
	}

	void FGraphMinimapCommands::Bind()
	{
		Instance.Pin()->BindCommands();
	}

	void FGraphMinimapCommands::BindCommands()
	{
		if (!IsRegistered())
		{
			UE_LOG(LogGraphMinimap, Fatal, TEXT("Bound before UI Command was registered.\nPlease be sure to bind after registration."));
		}

		if (IsBound())
		{
			UE_LOG(LogGraphMinimap, Warning, TEXT("The binding process has already been completed."));
			return;
		}
		bIsBound = true;
		
		const TSharedRef<FUICommandList>& MainFrameCommandBindings = IMainFrameModule::Get().GetMainFrameCommandBindings();
		MainFrameCommandBindings->Append(CommandBindings);
		
		CommandBindings->MapAction(
			ChangeMinimapState,
			FExecuteAction::CreateStatic(&FGraphMinimapCommandActions::ChangeMinimapState),
			FCanExecuteAction::CreateStatic(&FGraphMinimapCommandActions::CanChangeMinimapState)
		);

		CommandBindings->MapAction(
			ChangeMinimapArea,
			FExecuteAction::CreateStatic(&FGraphMinimapCommandActions::ChangeMinimapArea),
			FCanExecuteAction::CreateStatic(&FGraphMinimapCommandActions::CanChangeMinimapArea)
		);
	}
}

#undef LOCTEXT_NAMESPACE
