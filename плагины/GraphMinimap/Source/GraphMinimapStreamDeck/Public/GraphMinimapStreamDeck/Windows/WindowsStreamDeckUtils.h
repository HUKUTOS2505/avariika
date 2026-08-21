// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphMinimapStreamDeck/GenericPlatform/GenericStreamDeckUtils.h"

namespace GraphMinimap
{
	/**
	 * A class that defines the processing related to Stream Deck on Windows.
	 */
	class GRAPHMINIMAPSTREAMDECK_API FWindowsStreamDeckUtils : public FGenericStreamDeckUtils
	{
	public:
		// FGenericStreamDeckUtils interface.
		static bool IsStreamDeckInstalled();
		static void InstallStreamDeckPlugin();
		static bool IsInstalledStreamDeckPlugin();
		// End of FGenericStreamDeckUtils interface.
	};

#if PLATFORM_WINDOWS
	using FStreamDeckUtils = FWindowsStreamDeckUtils;
#endif
}
