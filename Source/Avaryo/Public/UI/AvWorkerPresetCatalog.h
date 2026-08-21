#pragma once

#include "CoreMinimal.h"

struct AVARYO_API FAvWorkerPresetLoadoutSummary
{
	FText Head;
	FText Face;
	FText Top;
	FText Hands;
	FText Bottom;
};

namespace AvWorkerPresetCatalog
{
	inline constexpr int32 PresetCount = 25;

	AVARYO_API bool TryGetLoadoutSummary(
		const FSoftObjectPath& PresetMeshPath,
		FAvWorkerPresetLoadoutSummary& OutSummary);
}
