// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphMinimap/Types/GraphMinimapState.h"
#include "GraphMinimap/Types/MinimapArea.h"

namespace GraphMinimap
{
	/**
	 * The public interface to the GraphMinimap module.
	 */
	class IGraphMinimap : public IModuleInterface
	{
	public:
		// The name of the module for this plugin.
		GRAPHMINIMAP_API static const FName PluginModuleName;
		
	public:
		// Returns singleton instance, loading the module on demand if needed.
		static IGraphMinimap& Get()
		{
			return FModuleManager::LoadModuleChecked<IGraphMinimap>(PluginModuleName);
		}

		// Returns whether the module is loaded and ready to use.
		static bool IsAvailable()
		{
			return FModuleManager::Get().IsModuleLoaded(PluginModuleName);
		}

		// Returns the current minimap state.
		virtual TOptional<EGraphMinimapState> GetMinimapState() const = 0; 
		
		// Changes the state of the minimap in the currently active graph editor.
		// Switches from the current state to the next state if no new state is specified.
		virtual void ChangeMinimapState(const TOptional<EGraphMinimapState>& NewState = {}) = 0;

		// Returns whether the minimap state can be toggled.
		virtual bool CanChangeMinimapState() const = 0;

		// Returns the number of minimap areas.
		virtual int32 GetNumOfMinimapArea() const = 0;

		// Returns a list of identifier for the minimap area.
		virtual TArray<FMinimapAreaIdentifier> GetMinimapAreaIdentifiers() const = 0;

		// Changes the minimap area in the currently active graph editor.
		virtual void ChangeMinimapArea() = 0;
		virtual void ChangeMinimapArea(const int32 Index) = 0;
		virtual void ChangeMinimapArea(const FMinimapAreaIdentifier& Identifier) = 0;

		// Returns whether the minimap area can be toggled.
		virtual bool CanChangeMinimapArea() const = 0;
	};
}
