#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/WorkerAppearanceComponent.h"
#include "AvAppearanceInspectorSlotButton.generated.h"

class UAvCharacterCustomizationRootWidget;

enum class EAvAppearanceInspectorSlotAction : uint8
{
	Toggle,
	Solo
};

/** Small payload-aware button used by the development-only dynamic inspector rows. */
UCLASS()
class AVARYO_API UAvAppearanceInspectorSlotButton : public UButton
{
	GENERATED_BODY()

public:
	void InitializeInspectorButton(
		UAvCharacterCustomizationRootWidget* InOwner,
		EWorkerSlot InSlot,
		EAvAppearanceInspectorSlotAction InAction);

private:
	UFUNCTION()
	void HandleInspectorClicked();

	TWeakObjectPtr<UAvCharacterCustomizationRootWidget> InspectorOwner;
	EWorkerSlot InspectorSlot = EWorkerSlot::Body;
	EAvAppearanceInspectorSlotAction InspectorAction = EAvAppearanceInspectorSlotAction::Toggle;
};
