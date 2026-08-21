// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

namespace GraphMinimap
{
	/**
	 * A detail customization class for the structure of the button to clear the cached data.
	 */
	class FClearCacheFileButtonDetail : public IPropertyTypeCustomization
	{
	public:
		// Registers-Unregisters and instantiate this customization.
		static void Register();
		static void Unregister();
		static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
		// IPropertyTypeCustomization interface.
		virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
		virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
		// End of IPropertyTypeCustomization interface.

	private:
		// Called when the clear button is pressed.
		static void OnButtonPressed();

		// Returns whether the clear button can be pressed.
		static bool CanButtonPress();

	private:
		// The cache of type names for properties that utilize this details panel.
		static FString CachedPropertyTypeName;
	};
}
