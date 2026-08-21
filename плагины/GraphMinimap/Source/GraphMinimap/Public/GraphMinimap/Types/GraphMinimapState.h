// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphMinimapState.generated.h"

/**
 * An enum that defines state of graph minimap.
 */
UENUM()
enum class EGraphMinimapState : uint8
{
	Hidden,
	Resizable,
	Visible,
	Controllable,
};

namespace GraphMinimap
{
	namespace GraphMinimapState
	{
		static FString ToString(const EGraphMinimapState Value)
		{
			const UEnum* EnumPtr = StaticEnum<EGraphMinimapState>();
			check(IsValid(EnumPtr));
			return EnumPtr->GetValueAsString(Value);
		}

		static TOptional<EGraphMinimapState> FromString(const FString& ValueString)
		{
			const UEnum* EnumPtr = StaticEnum<EGraphMinimapState>();
			check(IsValid(EnumPtr));
			const int64 Index = EnumPtr->GetValueByNameString(ValueString);
			if (Index == INDEX_NONE)
			{
				return {};
			}

			return static_cast<EGraphMinimapState>(Index);
		}
		
		static EGraphMinimapState NextState(const EGraphMinimapState CurrentState)
		{
			switch (CurrentState)
			{
			case EGraphMinimapState::Hidden:
				return EGraphMinimapState::Resizable;

			case EGraphMinimapState::Resizable:
				return EGraphMinimapState::Visible;

			case EGraphMinimapState::Visible:
				return EGraphMinimapState::Controllable;

			case EGraphMinimapState::Controllable:
				return EGraphMinimapState::Hidden;

			default:
				return EGraphMinimapState::Hidden;
			}
		}
	}
}
