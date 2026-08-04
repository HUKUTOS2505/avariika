// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/Widgets/SAutoScrollTextBlock.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "Widgets/Layout/SScrollBox.h"

namespace GraphMinimap
{
	void SAutoScrollTextBlock::Construct(const FArguments& InArgs)
	{
		ScrollSpeed = InArgs._ScrollSpeed;
		WaitTimeOnStartAndEnd = InArgs._WaitTimeOnEndOfScroll;
		WaitTimer = WaitTimeOnStartAndEnd;
		
		ChildSlot
		[
			SAssignNew(ScrollBox, SScrollBox)
			.Orientation(Orient_Horizontal)
			.ScrollBarVisibility(EVisibility::Collapsed)
			+ SScrollBox::Slot()
			[
#if UE_5_00_OR_LATER
				SArgumentNew(InArgs._TextBlockArgs, STextBlock)
#else
				SNew(STextBlock)
				.Text(InArgs._TextBlockArgs._Text)
#endif
			]
		];
	}

	void SAutoScrollTextBlock::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
	{
		SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
		
		if (!ScrollBox.IsValid())
		{
			return;
		}

		if (ScrollState == EScrollState::WaitOnStart || ScrollState == EScrollState::WaitOnEnd)
		{
			if (WaitTimer > 0.f)
			{
				WaitTimer -= InDeltaTime;
			}
			else
			{
				const bool bIsInStart = (ScrollState == EScrollState::WaitOnStart);
				ScrollState = (bIsInStart ? EScrollState::Scrolling : EScrollState::BackToStart);
			}
		}
		else if (ScrollState == EScrollState::Scrolling || ScrollState == EScrollState::BackToStart)
		{
			const bool bStartToEnd = (ScrollState == EScrollState::Scrolling);
			
			const float CurrentScrollOffset = ScrollBox->GetScrollOffset();
			const float TargetScrollOffset = (bStartToEnd ? ScrollBox->GetScrollOffsetOfEnd() : 0.f);
			const float DeltaScrollOffset = (InDeltaTime * ScrollSpeed * (bStartToEnd ? 1.f : -15.f));
			
			if (!FMath::IsNearlyEqual(CurrentScrollOffset, TargetScrollOffset))
			{
				DECLARE_DELEGATE_RetVal_TwoParams(float, FMinOrMax, float, float);
				const FMinOrMax Function = (
					bStartToEnd ?
					FMinOrMax::CreateStatic(&FMath::Min) :
					FMinOrMax::CreateStatic(&FMath::Max)
				);
				
				const float ModifiedScrollOffset = Function.Execute(
					CurrentScrollOffset + DeltaScrollOffset,
					TargetScrollOffset
				);
				ScrollBox->SetScrollOffset(ModifiedScrollOffset);
			}
			else
			{
				ScrollState = (bStartToEnd ? EScrollState::WaitOnEnd : EScrollState::WaitOnStart);
				WaitTimer = WaitTimeOnStartAndEnd;
			}
		}
	}

	void SAutoScrollTextBlock::ResetScrollState()
	{
		if (ScrollBox.IsValid())
		{
			ScrollBox->ScrollToStart();
		}

		ScrollState = EScrollState::WaitOnStart;
		WaitTimer = WaitTimeOnStartAndEnd;
	}
}
