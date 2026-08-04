// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphMinimap/Types/MinimapArea.h"

enum class EGraphMinimapState : uint8;

namespace GraphMinimap
{
	/**
	 *  A class that defines the function executed from the shortcut key.
	 */
	class GRAPHMINIMAP_API FGraphMinimapCommandActions
	{
	public:
		// Returns the current minimap state.
		static TOptional<EGraphMinimapState> GetMinimapState(); 
		
		// Changes the state of the minimap in the currently active graph editor.
		// Switches from the current state to the next state if no new state is specified.
		static void ChangeMinimapState();
		static void ChangeMinimapState(const TOptional<EGraphMinimapState>& NewState = {});

		// Returns whether the minimap state can be toggled.
		static bool CanChangeMinimapState();

		// Returns the number of minimap areas.
		static int32 GetNumOfMinimapArea();

		// Returns a list of identifier for the minimap area.
		static TArray<FMinimapAreaIdentifier> GetMinimapAreaIdentifiers();

		// Changes the minimap area in the currently active graph editor.
		static void ChangeMinimapArea();
		static void ChangeMinimapArea(const int32 Index);
		static void ChangeMinimapArea(const FMinimapAreaIdentifier& Identifier);

		// Returns whether the minimap area can be toggled.
		static bool CanChangeMinimapArea();
	};
}
