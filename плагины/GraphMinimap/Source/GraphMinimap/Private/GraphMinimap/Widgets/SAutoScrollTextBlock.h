// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/STextBlock.h"

class SScrollBox;

namespace GraphMinimap
{
	/**
	 * A text block that automatically scrolls when it doesn't fit in the display range.
	 */
	class GRAPHMINIMAP_API SAutoScrollTextBlock : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SAutoScrollTextBlock)
			: _ScrollSpeed(20.f)
			, _WaitTimeOnEndOfScroll(3.f)
		{}

		// Sets the slate arguments for the inner text block.
		SLATE_ARGUMENT(STextBlock::FArguments, TextBlockArgs)

		// Sets the autoscroll speed.
		SLATE_ARGUMENT(float, ScrollSpeed)

		// Sets the number of seconds to stop at the beginning and end of the string.
		SLATE_ARGUMENT(float, WaitTimeOnEndOfScroll)
		
		SLATE_END_ARGS()

		// Constructor.
		void Construct(const FArguments& InArgs);

		// SWidget interface.
		virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
		// End of SWidget interface.

		// Resets state changed by autoscroll.
		void ResetScrollState();
		
	private:
		// The scrollbox widget for internal use.
		TSharedPtr<SScrollBox> ScrollBox;

		// The autoscroll speed.
		float ScrollSpeed = 20.f;

		// The number of seconds to stop at the beginning and end of the string.
		float WaitTimeOnStartAndEnd = 3.f;
		
		// The timer that measures the latency at the beginning and end of a string.
		float WaitTimer = 0.f;

		// Kind of autoscroll state.
		enum class EScrollState : uint8
		{
			WaitOnStart,
			Scrolling,
			WaitOnEnd,
			BackToStart,
		};
		EScrollState ScrollState = EScrollState::WaitOnStart;
	};
}
