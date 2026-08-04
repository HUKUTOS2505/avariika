// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/UICommandInfo.h"

namespace GraphMinimap
{
	/**
	 * A class to register the shortcut key used in this plugin.
	 */
	class GRAPHMINIMAP_API FGraphMinimapCommands : public TCommands<FGraphMinimapCommands>
	{
	public:
		// Constructor.
		FGraphMinimapCommands();

		// TCommands interface.
		virtual void RegisterCommands() override;
		// End of TCommands interface.

		// Returns whether the commands registered here are bound.
		static bool IsBound();

		// Binds commands registered here.
		static void Bind();
	
	protected:
		// Binds commands actually registered here.
		virtual void BindCommands();

	public:
		// The list of shortcut keys used by this plugin.
		TSharedRef<FUICommandList> CommandBindings;
		
		// Instances of bound commands.
		TSharedPtr<FUICommandInfo> ChangeMinimapState;
		TSharedPtr<FUICommandInfo> ChangeMinimapArea;

	private:
		// Whether the commands registered here are bound.
		bool bIsBound;
	};
}
