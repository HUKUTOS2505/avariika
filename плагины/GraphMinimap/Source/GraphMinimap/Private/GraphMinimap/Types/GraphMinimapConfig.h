// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphMinimap/Types/GraphMinimapState.h"
#include "GraphMinimap/Types/MinimapArea.h"

namespace GraphMinimap
{
	/**
	 * A struct of minimap data stored on disk.
	 */
	struct FGraphMinimapConfig
	{
	public:
		EGraphMinimapState State;
		FVector2D Size;
		float RenderingScale;
		FMinimapAreaIdentifier SelectedMinimapArea;
	};
}
