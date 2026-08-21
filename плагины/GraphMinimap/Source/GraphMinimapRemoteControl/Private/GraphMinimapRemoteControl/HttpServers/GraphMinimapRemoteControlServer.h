// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IHttpRouter;
struct FHttpRouteHandleInternal;
struct FHttpServerRequest;
struct FHttpServerResponse;

namespace GraphMinimap
{
	/**
	 * A server class that utilizes the functionality of this plugin externally via a http server.
	 * 
	 * The format of the request from the server is as follows:
	 * UnrealEngine-GraphMinimap-[GraphMinimapState]
	 * 
	 * GraphMinimapState is the name of the state defined in EGraphMinimapState.
	 */
	class GRAPHMINIMAPREMOTECONTROL_API FGraphMinimapRemoteControlServer
	{
	public:
		// Registers-Unregisters the remote control receiver.
		static void Register();
		static void Unregister();
		
		// Called when remote control is enabled.
		void ConnectToServer(const FString& HttpPath, const uint32 PortNumber);

		// Called when remote control is disabled.
		void DisconnectFromServer();

	private:
		// Called when a request is made to the http server.
		static bool RequestHandler(
			const FHttpServerRequest& Request,
			const TFunction<void(TUniquePtr<FHttpServerResponse>&& Response)>& OnComplete
		);
		
	private:
		// The instance of currently running http server router.
		TSharedPtr<IHttpRouter> Router;

		// The handle to a route bound to the server.
		TSharedPtr<const FHttpRouteHandleInternal> RouteHandle;

		// The unique instance of this class.
		static TUniquePtr<FGraphMinimapRemoteControlServer> Instance;
	};
}
