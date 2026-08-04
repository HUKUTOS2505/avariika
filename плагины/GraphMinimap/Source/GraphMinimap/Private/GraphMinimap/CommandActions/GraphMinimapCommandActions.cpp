// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/CommandActions/GraphMinimapCommandActions.h"
#include "GraphMinimap/Types/GraphMinimapState.h"
#include "GraphMinimap/Utilities/GraphMinimapUtils.h"
#include "GraphMinimap/Widgets/SGraphMinimap.h"

namespace GraphMinimap
{
	TOptional<EGraphMinimapState> FGraphMinimapCommandActions::GetMinimapState()
	{
		const TSharedPtr<SGraphMinimap> GraphMinimap = FGraphMinimapUtils::GetActiveGraphMinimap();
		if (!GraphMinimap.IsValid())
		{
			return {};
		}

		return GraphMinimap->GetState();
	}

	void FGraphMinimapCommandActions::ChangeMinimapState()
	{
		ChangeMinimapState({});
	}

	void FGraphMinimapCommandActions::ChangeMinimapState(const TOptional<EGraphMinimapState>& NewState)
	{
		const TSharedPtr<SGraphMinimap> GraphMinimap = FGraphMinimapUtils::GetActiveGraphMinimap();
		if (!GraphMinimap.IsValid())
		{
			return;
		}

		EGraphMinimapState NextState;
		if (NewState.IsSet())
		{
			NextState = NewState.GetValue();
		}
		else
		{
			const EGraphMinimapState CurrentState = GraphMinimap->GetState();
			NextState = GraphMinimapState::NextState(CurrentState);
		}
		
		GraphMinimap->ChangeState(NextState);
	}

	bool FGraphMinimapCommandActions::CanChangeMinimapState()
	{
		const TSharedPtr<SGraphMinimap> GraphMinimap = FGraphMinimapUtils::GetActiveGraphMinimap();
		return GraphMinimap.IsValid();
	}

	int32 FGraphMinimapCommandActions::GetNumOfMinimapArea()
	{
		const TArray<FMinimapAreaIdentifier>& MinimapAreaIdentifiers = GetMinimapAreaIdentifiers();
		return MinimapAreaIdentifiers.Num();
	}

	TArray<FMinimapAreaIdentifier> FGraphMinimapCommandActions::GetMinimapAreaIdentifiers()
	{
		const TSharedPtr<SGraphMinimap> GraphMinimap = FGraphMinimapUtils::GetActiveGraphMinimap();
		if (!GraphMinimap.IsValid())
		{
			return {};
		}

		return GraphMinimap->GetMinimapAreaIdentifiers();
	}

	void FGraphMinimapCommandActions::ChangeMinimapArea()
	{
		const TSharedPtr<SGraphMinimap> GraphMinimap = FGraphMinimapUtils::GetActiveGraphMinimap();
        if (!GraphMinimap.IsValid())
        {
        	return;
        }

		const TArray<FMinimapAreaIdentifier>& MinimapAreaIdentifiers = GraphMinimap->GetMinimapAreaIdentifiers();
		const FMinimapAreaIdentifier& SelectedMinimapAreaIdentifier = GraphMinimap->GetSelectedMinimapAreaIdentifier();
		const int32 CurrentIndex = MinimapAreaIdentifiers.IndexOfByKey(SelectedMinimapAreaIdentifier);
		if (CurrentIndex == INDEX_NONE)
		{
			return;
		}

		int32 NextIndex = (CurrentIndex + 1);
		if (NextIndex == MinimapAreaIdentifiers.Num())
		{
			NextIndex = 0;
		}

		const FMinimapAreaIdentifier& NextMinimapAreaIdentifier = MinimapAreaIdentifiers[NextIndex];
		GraphMinimap->SetSelectedMinimapAreaIdentifier(NextMinimapAreaIdentifier);
	}

	void FGraphMinimapCommandActions::ChangeMinimapArea(const int32 Index)
	{
		const TSharedPtr<SGraphMinimap> GraphMinimap = FGraphMinimapUtils::GetActiveGraphMinimap();
		if (!GraphMinimap.IsValid())
		{
			return;
		}

		const TArray<FMinimapAreaIdentifier>& MinimapAreaIdentifiers = GraphMinimap->GetMinimapAreaIdentifiers();
		if (!MinimapAreaIdentifiers.IsValidIndex(Index))
		{
			return;
		}

		const FMinimapAreaIdentifier& NextMinimapAreaIdentifier = MinimapAreaIdentifiers[Index];
		GraphMinimap->SetSelectedMinimapAreaIdentifier(NextMinimapAreaIdentifier);
	}

	void FGraphMinimapCommandActions::ChangeMinimapArea(const FMinimapAreaIdentifier& Identifier)
	{
		const TSharedPtr<SGraphMinimap> GraphMinimap = FGraphMinimapUtils::GetActiveGraphMinimap();
		if (!GraphMinimap.IsValid())
		{
			return;
		}
		
		GraphMinimap->SetSelectedMinimapAreaIdentifier(Identifier);
	}

	bool FGraphMinimapCommandActions::CanChangeMinimapArea()
	{
		const TSharedPtr<SGraphMinimap> GraphMinimap = FGraphMinimapUtils::GetActiveGraphMinimap();
		if (!GraphMinimap.IsValid())
		{
			return false;
		}

		return GraphMinimap->HasMultipleMinimapArea();
	}
}
