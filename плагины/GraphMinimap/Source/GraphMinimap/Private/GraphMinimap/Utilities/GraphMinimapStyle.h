// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "GraphMinimapStyle.generated.h"

struct FSlateIcon;

// Kind of icon registered in this style set.
UENUM()
enum class EGraphMinimapStyleIconType : uint8
{
	Resizable,
	Visible,
	Controllable,
};

namespace GraphMinimap
{
	/**
	 * A class that manages the slate icon used by this plugin.
	 */
	class GRAPHMINIMAP_API FGraphMinimapStyle : public FSlateStyleSet
	{
	public:
		// Constructor.
		FGraphMinimapStyle();

	private:
		// The actual registration process for this class.
		void RegisterInternal();

	public:
		// Registers-Unregisters and instance getter this class.
		static void Register();
		static void Unregister();
		static const ISlateStyle& Get();

		// Returns slate brush of specified icon type.
		static const FSlateBrush* GetBrushFromIconType(const EGraphMinimapStyleIconType IconType);

		// Returns slate icon of specified icon type.
		static FSlateIcon GetSlateIconFromIconType(const EGraphMinimapStyleIconType IconType);
		
		// Returns property name of specified icon type.
		static FName GetPropertyNameFromIconType(const EGraphMinimapStyleIconType IconType);
		
	private:
		// The instance of this style class.
		static TUniquePtr<FGraphMinimapStyle> Instance;
	};
}
