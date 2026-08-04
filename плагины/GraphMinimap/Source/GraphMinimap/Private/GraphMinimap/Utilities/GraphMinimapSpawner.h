// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphMinimap/Types/GraphMinimapConfig.h"

struct FUserActivity;

namespace GraphMinimap
{
	/**
	 * A spawner class to add a graph minimap to a graph editor.
	 */
	class GRAPHMINIMAP_API FGraphMinimapSpawner
	{
	public:
		// Registers-Unregisters graph minimap spawner.
		static void Register();
		static void Unregister();
		
		// A function that can only be accessed from the details panel extension that deletes the cache file.
		struct FCachedDataAccessor
		{
		private:
			friend class FClearCacheFileButtonDetail;

			static bool HasCachedData();
			static void ClearCachedData();
		};
		
	private:
		// Called when the active tab is switched.
		static void HandleOnActivityChanged(const FUserActivity& UserActivity);

		// Called at the next frame when the active tab is switched.
		static void HandleOnPostTick(float DeltaSeconds);

		// Called when the config of the minimap changes.
		static void HandleOnGraphMinimapConfigChanged(const FString& GraphId, const FGraphMinimapConfig& GraphMinimapConfig);
		
		// Returns the config data for the specified graph.
		// If not found, return the default value based on the editor settings.
		static FGraphMinimapConfig FindGraphMinimapConfig(const FString& GraphId);
		
		// Returns the path to the json file in the state of the graph minimap saved on disk.
		static FString GetConfigFilePath();
		
		// Serializes the state of the graph minimap.
		static bool SerializeGraphMinimapStates();

		// Deserializes the state of the graph minimap.
		static bool DeserializeGraphMinimapStates();

		// Deletes the state of the graph minimap.
		static void ClearGraphMinimapStates();
	
	private:
		// The handle of the event called when the active tab is switched.
		static FDelegateHandle OnActivityChangedHandle;

		// The handle of the event called in the next frame when the active tab switches.
		static FDelegateHandle OnPostTickHandle;
		
		// The state of the minimap for each graph.
		static TMap<FString, FGraphMinimapConfig> CachedGraphMinimapConfigs;
	};
}
