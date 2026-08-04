// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"

namespace GraphMinimap
{
	namespace Global
	{
		// The name of this plugin.
		static const FName PluginName = TEXT("GraphMinimap");
	
		// The minimum size of the minimap.
		static const FVector2D MinimumMinimapSize = FVector2D(100.f, 100.f);
	}
}

/**
 * Macros to support each engine version.
 */
#ifndef UE_5_01_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 1, 0)
#define UE_5_01_OR_LATER 1
#else
#define UE_5_01_OR_LATER 0
#endif
#endif

#ifndef UE_5_00_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 0, 0)
#define UE_5_00_OR_LATER 1
#else
#define UE_5_00_OR_LATER 0
#endif
#endif

#ifndef UE_4_26_OR_LATER
#if !UE_VERSION_OLDER_THAN(4, 26, 0)
#define UE_4_26_OR_LATER 1
#else
#define UE_4_26_OR_LATER 0
#endif
#endif

/**
 * Categories used for log output with this plugin.
 */
GRAPHMINIMAP_API DECLARE_LOG_CATEGORY_EXTERN(LogGraphMinimap, Log, All);
