// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/IGraphMinimap.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "GraphMinimap/CommandActions/GraphMinimapCommands.h"
#include "GraphMinimap/CommandActions/GraphMinimapCommandActions.h"
#include "GraphMinimap/Utilities/GraphMinimapStyle.h"
#include "GraphMinimap/Utilities/GraphMinimapSettings.h"
#include "GraphMinimap/Utilities/GraphMinimapSpawner.h"
#include "GraphMinimap/DetailCustomizations/ClearCacheFileButtonDetail.h"

DEFINE_LOG_CATEGORY(LogGraphMinimap);

namespace GraphMinimap
{
	const FName IGraphMinimap::PluginModuleName = TEXT("GraphMinimap");
	
	class FGraphMinimapModule : public IGraphMinimap
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.

		// IGraphMinimap interface.
		virtual TOptional<EGraphMinimapState> GetMinimapState() const override;
		virtual void ChangeMinimapState(const TOptional<EGraphMinimapState>& NewState) override;
		virtual bool CanChangeMinimapState() const override;
		virtual int32 GetNumOfMinimapArea() const override;
		virtual TArray<FMinimapAreaIdentifier> GetMinimapAreaIdentifiers() const override;
		virtual void ChangeMinimapArea() override;
		virtual void ChangeMinimapArea(const int32 Index) override;
		virtual void ChangeMinimapArea(const FMinimapAreaIdentifier& Identifier) override;
		virtual bool CanChangeMinimapArea() const override;
		// End of IGraphMinimap interface.
	};

	void FGraphMinimapModule::StartupModule()
	{
		// Register style set.
		FGraphMinimapStyle::Register();
		
		// Register command actions.
		FGraphMinimapCommands::Register();
		FGraphMinimapCommands::Bind();
		
		// Register settings.
		UGraphMinimapSettings::Register();
		
		// Register graph minimap spawner.
		FGraphMinimapSpawner::Register();

		// Register detail customizations.
		FClearCacheFileButtonDetail::Register();
	}

	void FGraphMinimapModule::ShutdownModule()
	{
		// Unregister detail customizations.
		FClearCacheFileButtonDetail::Unregister();
		
		// Unregister graph minimap spawner.
		FGraphMinimapSpawner::Unregister();

		// Unregister settings.
		UGraphMinimapSettings::Unregister();
		
		// Unregister command actions.
		if (FGraphMinimapCommands::IsRegistered())
		{
			FGraphMinimapCommands::Unregister();
		}

		// Unregister style set.
		FGraphMinimapStyle::Unregister();
	}

	TOptional<EGraphMinimapState> FGraphMinimapModule::GetMinimapState() const
	{
		return FGraphMinimapCommandActions::GetMinimapState();
	}
	
	void FGraphMinimapModule::ChangeMinimapState(const TOptional<EGraphMinimapState>& NewState)
	{
		FGraphMinimapCommandActions::ChangeMinimapState(NewState);
	}

	bool FGraphMinimapModule::CanChangeMinimapState() const
	{
		return FGraphMinimapCommandActions::CanChangeMinimapState();
	}

	int32 FGraphMinimapModule::GetNumOfMinimapArea() const
	{
		return FGraphMinimapCommandActions::GetNumOfMinimapArea();
	}

	TArray<FMinimapAreaIdentifier> FGraphMinimapModule::GetMinimapAreaIdentifiers() const
	{
		return FGraphMinimapCommandActions::GetMinimapAreaIdentifiers();
	}

	void FGraphMinimapModule::ChangeMinimapArea()
	{
		FGraphMinimapCommandActions::ChangeMinimapArea();
	}

	void FGraphMinimapModule::ChangeMinimapArea(const int32 Index)
	{
		FGraphMinimapCommandActions::ChangeMinimapArea(Index);
	}

	void FGraphMinimapModule::ChangeMinimapArea(const FMinimapAreaIdentifier& Identifier)
	{
		FGraphMinimapCommandActions::ChangeMinimapArea(Identifier);
	}

	bool FGraphMinimapModule::CanChangeMinimapArea() const
	{
		return FGraphMinimapCommandActions::CanChangeMinimapArea();
	}
}

IMPLEMENT_MODULE(GraphMinimap::FGraphMinimapModule, GraphMinimap)
