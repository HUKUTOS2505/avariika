// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphMinimapRequestAction.generated.h"

/**
 * Kind of action sent to the http server.
 */
UENUM()
enum class EGraphMinimapRequestAction : uint8
{
	Change,
	Check,
};

namespace GraphMinimap
{
	namespace GraphMinimapRequestAction
	{
		static FString ToString(const EGraphMinimapRequestAction Value)
		{
			const UEnum* EnumPtr = StaticEnum<EGraphMinimapRequestAction>();
			check(IsValid(EnumPtr));
			return EnumPtr->GetValueAsString(Value);
		}

		static TOptional<EGraphMinimapRequestAction> FromString(const FString& ValueString)
		{
			const UEnum* EnumPtr = StaticEnum<EGraphMinimapRequestAction>();
			check(IsValid(EnumPtr));
			const int64 Index = EnumPtr->GetValueByNameString(ValueString);
			if (Index == INDEX_NONE)
			{
				return {};
			}

			return static_cast<EGraphMinimapRequestAction>(Index);
		}
	}
}
