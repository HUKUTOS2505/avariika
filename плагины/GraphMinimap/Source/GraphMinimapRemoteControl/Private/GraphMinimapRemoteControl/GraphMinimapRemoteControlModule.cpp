// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphMinimapRemoteControl/Utilities/GraphMinimapRemoteControlSettings.h"
#include "GraphMinimapRemoteControl/HttpServers/GraphMinimapRemoteControlServer.h"

namespace GraphMinimap
{
	class FGraphMinimapRemoteControlModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FGraphMinimapRemoteControlModule::StartupModule()
	{
		// Register settings.
		UGraphMinimapRemoteControlSettings::Register();
		
		// Register remote control server.
		FGraphMinimapRemoteControlServer::Register();
	}

	void FGraphMinimapRemoteControlModule::ShutdownModule()
	{
		// Unregister remote control server.
		FGraphMinimapRemoteControlServer::Unregister();

		// Unregister settings.
		UGraphMinimapRemoteControlSettings::Unregister();
	}
}

IMPLEMENT_MODULE(GraphMinimap::FGraphMinimapRemoteControlModule, GraphMinimapRemoteControl)
