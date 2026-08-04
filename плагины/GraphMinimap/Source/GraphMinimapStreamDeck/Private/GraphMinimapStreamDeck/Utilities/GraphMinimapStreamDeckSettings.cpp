// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimapStreamDeck/Utilities/GraphMinimapStreamDeckSettings.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "GraphMinimapStreamDeckSettings"

namespace GraphMinimap
{
	namespace StreamDeckSettings
	{
		static const FName ContainerName			= TEXT("Editor");
		static const FName CategoryName				= TEXT("Plugins");
		static const FName SectionName				= TEXT("GraphMinimapStreamDeckSettings");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
		}
	}
}


void UGraphMinimapStreamDeckSettings::Register()
{
	if (ISettingsModule* SettingsModule = GraphMinimap::StreamDeckSettings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			GraphMinimap::StreamDeckSettings::ContainerName,
			GraphMinimap::StreamDeckSettings::CategoryName,
			GraphMinimap::StreamDeckSettings::SectionName,
			LOCTEXT("SettingName", "Graph Minimap - Stream Deck"),
			LOCTEXT("SettingDescription", "Settings related to the stream deck of this plugin"),
			GetMutableDefault<UGraphMinimapStreamDeckSettings>()
		);
	}
}

void UGraphMinimapStreamDeckSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = GraphMinimap::StreamDeckSettings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			GraphMinimap::StreamDeckSettings::ContainerName,
			GraphMinimap::StreamDeckSettings::CategoryName,
			GraphMinimap::StreamDeckSettings::SectionName
		);
	}
}
