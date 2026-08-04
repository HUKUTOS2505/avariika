// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimapStreamDeck/DetailCustomizations/StreamDeckInstallButtonDetail.h"
#include "GraphMinimapStreamDeck/Types/StreamDeckInstallButton.h"
#ifdef WITH_STREAM_DECK
#include "GraphMinimapStreamDeck/HAL/StreamDeckUtils.h"
#endif
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "StreamDeckInstallButtonDetail"

namespace GraphMinimap
{
	void FStreamDeckInstallButtonDetail::Register()
	{
		CachedPropertyTypeName = GetNameSafe(FStreamDeckInstallButton::StaticStruct());
		
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName,
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStreamDeckInstallButtonDetail::MakeInstance)
		);
	}

	void FStreamDeckInstallButtonDetail::Unregister()
	{
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName
		);
	}

	TSharedRef<IPropertyTypeCustomization> FStreamDeckInstallButtonDetail::MakeInstance()
	{
		return MakeShared<FStreamDeckInstallButtonDetail>();
	}

	void FStreamDeckInstallButtonDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
		HeaderRow.ValueContent()
		.MinDesiredWidth(130.f)
		[
			SNew(SButton)
			.Text_Static(&FStreamDeckInstallButtonDetail::GetButtonDisplayText)
			.ToolTipText_Static(&FStreamDeckInstallButtonDetail::GetButtonTooltipText)
			.OnPressed_Static(&FStreamDeckInstallButtonDetail::OnButtonPressed)
			.IsEnabled_Static(&FStreamDeckInstallButtonDetail::CanButtonPress)
		];
	}

	void FStreamDeckInstallButtonDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
	}

	FText FStreamDeckInstallButtonDetail::GetButtonDisplayText()
	{
#ifdef WITH_STREAM_DECK
		if (!FStreamDeckUtils::IsStreamDeckInstalled())
		{
			return LOCTEXT("RequiredStreamDeckLabel", "Stream Deck Application is not installed."); 
		}
		
		if (FStreamDeckUtils::IsInstalledStreamDeckPlugin())
		{
			return LOCTEXT("AlreadyInstalledLabel", "Stream Deck Plugin is already installed.");
		}
		
		return LOCTEXT("InstallLabel", "Install Stream Deck Plugin");
#else
		return LOCTEXT("NotSupportLabel", "Not Available");
#endif
	}

	FText FStreamDeckInstallButtonDetail::GetButtonTooltipText()
	{
#ifdef WITH_STREAM_DECK
		if (!FStreamDeckUtils::IsStreamDeckInstalled())
		{
			return LOCTEXT("RequiredStreamDeckTooltip", "You need to have the Stream Deck application installed in your OS to use this feature."); 
		}
		
		if (FStreamDeckUtils::IsInstalledStreamDeckPlugin())
		{
			return LOCTEXT("AlreadyInstalledTooltip", "The plugin is already installed in the Stream Deck application.");
		}
		
		return LOCTEXT("InstallTooltip", "Install this plugin for Stream Deck into the Stream Deck application.");
#else
		return LOCTEXT("NotSupportTooltip", "This platform does not support features related to Stream Deck.");
#endif
	}

	void FStreamDeckInstallButtonDetail::OnButtonPressed()
	{
#ifdef WITH_STREAM_DECK
		FStreamDeckUtils::InstallStreamDeckPlugin();
#endif
	}

	bool FStreamDeckInstallButtonDetail::CanButtonPress()
	{
#ifdef WITH_STREAM_DECK
		return (
			FStreamDeckUtils::IsStreamDeckInstalled() &&
			!FStreamDeckUtils::IsInstalledStreamDeckPlugin()
		);
#else
		return false;
#endif
	}

	FString FStreamDeckInstallButtonDetail::CachedPropertyTypeName;
}

#undef LOCTEXT_NAMESPACE
