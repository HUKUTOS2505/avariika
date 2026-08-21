// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimapRemoteControl/HttpServers/GraphMinimapRemoteControlServer.h"
#include "GraphMinimapRemoteControl/Utilities/GraphMinimapRemoteControlSettings.h"
#include "GraphMinimapRemoteControl/Types/GraphMinimapRequestAction.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "GraphMinimap/IGraphMinimap.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerConstants.h"
#include "HttpServerResponse.h"

namespace GraphMinimap
{
	void FGraphMinimapRemoteControlServer::Register()
	{
		Instance = MakeUnique<FGraphMinimapRemoteControlServer>();
		check(Instance.IsValid());
		
		UGraphMinimapRemoteControlSettings::OnRemoteControlEnabled.AddRaw(
			Instance.Get(), &FGraphMinimapRemoteControlServer::ConnectToServer
		);
		UGraphMinimapRemoteControlSettings::OnRemoteControlDisabled.AddRaw(
			Instance.Get(), &FGraphMinimapRemoteControlServer::DisconnectFromServer
		);

		const auto& Settings = UGraphMinimapRemoteControlSettings::Get();
		if (Settings.bEnableRemoteControl)
		{
			Instance->ConnectToServer(Settings.HttpPath, Settings.PortNumber);
		}
	}

	void FGraphMinimapRemoteControlServer::Unregister()
	{
		UGraphMinimapRemoteControlSettings::OnRemoteControlEnabled.RemoveAll(Instance.Get());
		UGraphMinimapRemoteControlSettings::OnRemoteControlDisabled.RemoveAll(Instance.Get());

		Instance.Reset();
	}

	void FGraphMinimapRemoteControlServer::ConnectToServer(const FString& HttpPath, const uint32 PortNumber)
	{
		DisconnectFromServer();

		auto& HttpServerModule = FHttpServerModule::Get();

		const FHttpPath Path(HttpPath);
		if (!Path.IsValidPath())
		{
			UE_LOG(LogGraphMinimap, Error, TEXT("The specified path is invalid (Http Path : %s)"), *Path.GetPath());
		}
		
		Router = HttpServerModule.GetHttpRouter(PortNumber);
		if (!Router.IsValid())
		{
			UE_LOG(LogGraphMinimap, Error, TEXT("Failed to build server (Port Number : %u)"), PortNumber);
			return;
		}

		RouteHandle = Router->BindRoute(
			Path,
			EHttpServerRequestVerbs::VERB_GET | EHttpServerRequestVerbs::VERB_POST,
			&FGraphMinimapRemoteControlServer::RequestHandler
		);
		if (!RouteHandle.IsValid())
		{
			UE_LOG(LogGraphMinimap, Error, TEXT("Failed to establish route to server"));
			return;
		}

		HttpServerModule.StartAllListeners();
		
		UE_LOG(LogGraphMinimap, Log, TEXT("Connected to server (URL: %s)"), *RouteHandle->Path);
	}

	void FGraphMinimapRemoteControlServer::DisconnectFromServer()
	{
		if (Router.IsValid())
		{
			FHttpServerModule::Get().StopAllListeners();
			Router->UnbindRoute(RouteHandle);
			Router.Reset();

			UE_LOG(LogGraphMinimap, Log, TEXT("Dissconnected from server"));
		}
	}

	bool FGraphMinimapRemoteControlServer::RequestHandler(
		const FHttpServerRequest& Request,
		const TFunction<void(TUniquePtr<FHttpServerResponse>&& Response)>& OnComplete
	)
	{
		const FString* GraphMinimapStateString = Request.QueryParams.Find(TEXT("state"));
		const FString* RequestActionString = Request.QueryParams.Find(TEXT("action"));
		if (GraphMinimapStateString == nullptr || RequestActionString == nullptr)
		{
			const FString ErrorMessage = FString::Printf(TEXT("Invalid query parameter. Specify state and action parameters."));
			UE_LOG(LogGraphMinimap, Error, TEXT("%s"), *ErrorMessage);
			OnComplete(
				FHttpServerResponse::Error(
					EHttpServerResponseCodes::BadRequest,
					{},
					ErrorMessage
				)
			);
			return true;
		}
		
		const TOptional<EGraphMinimapState> GraphMinimapState = GraphMinimapState::FromString(*GraphMinimapStateString);
		const TOptional<EGraphMinimapRequestAction> RequestAction = GraphMinimapRequestAction::FromString(*RequestActionString);
		if (GraphMinimapState.IsSet() && RequestAction.IsSet())
		{
			if (RequestAction.GetValue() == EGraphMinimapRequestAction::Change)
			{
				IGraphMinimap::Get().ChangeMinimapState(GraphMinimapState);
				OnComplete(FHttpServerResponse::Ok());
			}
			else if (RequestAction.GetValue() == EGraphMinimapRequestAction::Check)
			{
				const TOptional<EGraphMinimapState> CurrentGraphMinimapState = IGraphMinimap::Get().GetMinimapState();
				const bool bCanChangeMinimapState = (CurrentGraphMinimapState.IsSet() && CurrentGraphMinimapState != GraphMinimapState);
				const FString Content = (bCanChangeMinimapState ? TEXT("true") : TEXT("false"));

				TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(
					Content,
					TEXT("text/plain")
				);
				Response->Code = EHttpServerResponseCodes::Ok;
				OnComplete(MoveTemp(Response));
			}
			
			UE_LOG(LogGraphMinimap, Log, TEXT("Received request: state = %s action = %s"), **GraphMinimapStateString, **RequestActionString);
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Received invalid request : state = %s action = %s"), **GraphMinimapStateString, **RequestActionString);
			OnComplete(
				FHttpServerResponse::Error(
					EHttpServerResponseCodes::BadRequest,
					{},
					ErrorMessage
				)
			);

			UE_LOG(LogGraphMinimap, Error, TEXT("%s"), *ErrorMessage);
		}

		return true;
	}

	TUniquePtr<FGraphMinimapRemoteControlServer> FGraphMinimapRemoteControlServer::Instance;
}
