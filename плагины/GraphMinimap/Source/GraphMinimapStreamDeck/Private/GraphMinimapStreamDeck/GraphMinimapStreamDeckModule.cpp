// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphMinimapStreamDeck/Utilities/GraphMinimapStreamDeckSettings.h"
#include "GraphMinimapStreamDeck/DetailCustomizations/StreamDeckInstallButtonDetail.h"

namespace GraphMinimap
{
	class FGraphMinimapStreamDeckModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FGraphMinimapStreamDeckModule::StartupModule()
	{
#ifdef WITH_STREAM_DECK
		// Register settings.
		UGraphMinimapStreamDeckSettings::Register();

		// Register detail customizations.
		FStreamDeckInstallButtonDetail::Register();
#endif
	}

	void FGraphMinimapStreamDeckModule::ShutdownModule()
	{
#ifdef WITH_STREAM_DECK
		// Unregister detail customizations.
		FStreamDeckInstallButtonDetail::Unregister();
		
		// Unregister settings.
		UGraphMinimapStreamDeckSettings::Unregister();
#endif
	}
}

IMPLEMENT_MODULE(GraphMinimap::FGraphMinimapStreamDeckModule, GraphMinimapStreamDeck)
