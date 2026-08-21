// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimapRemoteControl/Utilities/GraphMinimapRemoteControlSettings.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "GraphMinimapRemoteControlSettings"

namespace GraphMinimap
{
	namespace RemoteControlSettings
	{
		static const FName ContainerName			= TEXT("Editor");
		static const FName CategoryName				= TEXT("Plugins");
		static const FName SectionName				= TEXT("GraphMinimapRemoteControlSettings");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
		}
	}
}

UGraphMinimapRemoteControlSettings::FOnRemoteControlEnabled UGraphMinimapRemoteControlSettings::OnRemoteControlEnabled;
UGraphMinimapRemoteControlSettings::FOnRemoteControlDisabled UGraphMinimapRemoteControlSettings::OnRemoteControlDisabled;

UGraphMinimapRemoteControlSettings::UGraphMinimapRemoteControlSettings()
	: bEnableRemoteControl(false)
	, HttpPath(TEXT("/unrealengine_graphminimap"))
	, PortNumber(1080)
{
}

void UGraphMinimapRemoteControlSettings::Register()
{
	if (ISettingsModule* SettingsModule = GraphMinimap::RemoteControlSettings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			GraphMinimap::RemoteControlSettings::ContainerName,
			GraphMinimap::RemoteControlSettings::CategoryName,
			GraphMinimap::RemoteControlSettings::SectionName,
			LOCTEXT("SettingName", "Graph Minimap - Remote Control"),
			LOCTEXT("SettingDescription", "Settings related to the remote control of this plugin"),
			GetMutableDefault<UGraphMinimapRemoteControlSettings>()
		);
	}
}

void UGraphMinimapRemoteControlSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = GraphMinimap::RemoteControlSettings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			GraphMinimap::RemoteControlSettings::ContainerName,
			GraphMinimap::RemoteControlSettings::CategoryName,
			GraphMinimap::RemoteControlSettings::SectionName
		);
	}
}

const UGraphMinimapRemoteControlSettings& UGraphMinimapRemoteControlSettings::Get()
{
	const auto* Settings = GetDefault<UGraphMinimapRemoteControlSettings>();
	check(IsValid(Settings));
	return *Settings;
}

void UGraphMinimapRemoteControlSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapRemoteControlSettings, bEnableRemoteControl))
	{
		if (bEnableRemoteControl)
		{
			OnRemoteControlEnabled.Broadcast(HttpPath, PortNumber);
		}
		else
		{
			OnRemoteControlDisabled.Broadcast();
		}
	}
}

#undef LOCTEXT_NAMESPACE
