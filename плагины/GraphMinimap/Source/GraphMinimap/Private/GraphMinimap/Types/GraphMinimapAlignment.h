// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "GraphMinimapAlignment.generated.h"

/**
 * An enum that defines kind of position on which the minimap is displayed in the graph editor.
 */
UENUM()
enum class EGraphMinimapAlignment : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
};

namespace GraphMinimap
{
	namespace GraphMinimapAlignment
	{
		static EHorizontalAlignment ToHorizontalAlignment(const EGraphMinimapAlignment GraphMinimapAlignment)
		{
			switch (GraphMinimapAlignment)
			{
			case EGraphMinimapAlignment::TopLeft:
			case EGraphMinimapAlignment::BottomLeft:
				return HAlign_Left;

			case EGraphMinimapAlignment::TopRight:
			case EGraphMinimapAlignment::BottomRight:
				return HAlign_Right;

			default:
				return HAlign_Fill;
			}
		}

		static EVerticalAlignment ToVerticalAlignment(const EGraphMinimapAlignment GraphMinimapAlignment)
		{
			switch (GraphMinimapAlignment)
			{
			case EGraphMinimapAlignment::TopLeft:
			case EGraphMinimapAlignment::TopRight:
				return VAlign_Top;

			case EGraphMinimapAlignment::BottomLeft:
			case EGraphMinimapAlignment::BottomRight:
				return VAlign_Bottom;

			default:
				return VAlign_Fill;
			}
		}
	}
}
