#include "UI/AvAppearanceInspectorSlotButton.h"

#include "UI/AvCharacterCustomizationRootWidget.h"

void UAvAppearanceInspectorSlotButton::InitializeInspectorButton(
	UAvCharacterCustomizationRootWidget* InOwner,
	EWorkerSlot InSlot,
	EAvAppearanceInspectorSlotAction InAction)
{
	InspectorOwner = InOwner;
	InspectorSlot = InSlot;
	InspectorAction = InAction;
	OnClicked.RemoveAll(this);
	OnClicked.AddDynamic(this, &UAvAppearanceInspectorSlotButton::HandleInspectorClicked);
}

void UAvAppearanceInspectorSlotButton::HandleInspectorClicked()
{
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (UAvCharacterCustomizationRootWidget* Owner = InspectorOwner.Get())
	{
		Owner->HandleAppearanceInspectorSlotAction(InspectorSlot, InspectorAction);
	}
#endif
}
