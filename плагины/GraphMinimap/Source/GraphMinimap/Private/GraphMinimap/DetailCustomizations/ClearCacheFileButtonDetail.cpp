// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/DetailCustomizations/ClearCacheFileButtonDetail.h"
#include "GraphMinimap/Types/ClearCacheFileButton.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "DetailWidgetRow.h"
#include "GraphMinimap/Utilities/GraphMinimapSpawner.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "ClearHistoriesButtonDetail"

namespace GraphMinimap
{
	void FClearCacheFileButtonDetail::Register()
	{
		CachedPropertyTypeName = GetNameSafe(FClearCacheFileButton::StaticStruct());
		
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName,
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FClearCacheFileButtonDetail::MakeInstance)
		);
	}

	void FClearCacheFileButtonDetail::Unregister()
	{
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName
		);
	}

	TSharedRef<IPropertyTypeCustomization> FClearCacheFileButtonDetail::MakeInstance()
	{
		return MakeShared<FClearCacheFileButtonDetail>();
	}

	void FClearCacheFileButtonDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
		HeaderRow.ValueContent()
		.MinDesiredWidth(130.f)
		[
			SNew(SButton)
			.OnPressed_Static(&FClearCacheFileButtonDetail::OnButtonPressed)
			.IsEnabled_Static(&FClearCacheFileButtonDetail::CanButtonPress)
			.Text(LOCTEXT("ButtonText", "Clear Cached Files"))
			.ToolTipText(LOCTEXT("ButtonTooltip", "Delete cached minimap data and json data on disk."))
		];
	}

	void FClearCacheFileButtonDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
	}

	void FClearCacheFileButtonDetail::OnButtonPressed()
	{
		FGraphMinimapSpawner::FCachedDataAccessor::ClearCachedData();
	}

	bool FClearCacheFileButtonDetail::CanButtonPress()
	{
		return FGraphMinimapSpawner::FCachedDataAccessor::HasCachedData();
	}

	FString FClearCacheFileButtonDetail::CachedPropertyTypeName;
}

#undef LOCTEXT_NAMESPACE
