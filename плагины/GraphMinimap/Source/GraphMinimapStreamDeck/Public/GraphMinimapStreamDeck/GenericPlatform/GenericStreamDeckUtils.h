// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace GraphMinimap
{
	/**
	 * A class that defines the processing related to the Stream Deck.
	 */
	class GRAPHMINIMAPSTREAMDECK_API FGenericStreamDeckUtils
	{
	public:
		// Returns whether the Stream Deck application is installed.
		static bool IsStreamDeckInstalled() { return false; }

		// Installs the Stream Deck plugin that works with this plugin on the Stream Deck.
		static void InstallStreamDeckPlugin() {}

		// Returns whether the Stream Deck plugin is installed.
		static bool IsInstalledStreamDeckPlugin() { return false; }
	};
}
