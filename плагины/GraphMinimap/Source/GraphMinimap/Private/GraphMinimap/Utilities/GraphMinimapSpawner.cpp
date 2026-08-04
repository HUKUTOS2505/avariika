// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/Utilities/GraphMinimapSpawner.h"
#include "GraphMinimap/IGraphMinimap.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "GraphMinimap/Utilities/GraphMinimapSettings.h"
#include "GraphMinimap/Utilities/GraphMinimapUtils.h"
#include "GraphMinimap/Widgets/SGraphMinimap.h"
#include "UserActivityTracking.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Framework/Application/SlateApplication.h"

namespace GraphMinimap
{
	void FGraphMinimapSpawner::Register()
	{
		if (!DeserializeGraphMinimapStates())
		{
			UE_LOG(LogGraphMinimap, Error, TEXT("Failed to deserialize the state of the graph minimap."));
		}
		
		OnActivityChangedHandle = FUserActivityTracking::OnActivityChanged.AddStatic(
			&FGraphMinimapSpawner::HandleOnActivityChanged
		);
	}

	void FGraphMinimapSpawner::Unregister()
	{
		FUserActivityTracking::OnActivityChanged.Remove(OnActivityChangedHandle);
	}

	bool FGraphMinimapSpawner::FCachedDataAccessor::HasCachedData()
	{
		return (CachedGraphMinimapConfigs.Num() > 0);
	}

	void FGraphMinimapSpawner::FCachedDataAccessor::ClearCachedData()
	{
		ClearGraphMinimapStates();
	}

	void FGraphMinimapSpawner::HandleOnActivityChanged(const FUserActivity& UserActivity)
	{
		if (OnPostTickHandle.IsValid())
		{
			return;
		}
		
		// If we do GetActiveTab at this timing, we can only get the previous active tab,
		// so process it in the next frame.
		OnPostTickHandle = FSlateApplication::Get().OnPostTick().AddStatic(&FGraphMinimapSpawner::HandleOnPostTick);
	}

	void FGraphMinimapSpawner::HandleOnPostTick(float DeltaSeconds)
	{
		const TSharedPtr<SGraphEditor> GraphEditor = FGraphMinimapUtils::GetActiveGraphEditor();
		if (!GraphEditor.IsValid())
		{
			return;
		}

		const TSharedPtr<SOverlay> Overlay = FGraphMinimapUtils::FindNearestChildOverlay(GraphEditor);
		if (!Overlay.IsValid())
		{
			return;
		}

		// Check if overlay already has a graph minimap.
		if (FGraphMinimapUtils::FindGraphMinimap(Overlay))
		{
			return;
		}
		
		const TOptional<FString> GraphId = FGraphMinimapUtils::GetGraphIdentificationString(GraphEditor);
		if (!GraphId.IsSet())
		{
			return;
		}

#if UE_5_00_OR_LATER
		SOverlay::FScopedWidgetSlotArguments ExtensionPoint = Overlay->AddSlot(TNumericLimits<int32>::Max());
#else
		SOverlay::FOverlaySlot& ExtensionPoint = Overlay->AddSlot(TNumericLimits<int32>::Max());
#endif
		const FGraphMinimapConfig Config = FindGraphMinimapConfig(GraphId.GetValue());
		
		const TSharedPtr<SGraphMinimap> GraphMinimap =
			SNew(SGraphMinimap, GraphEditor, &ExtensionPoint)
			.InitialState(Config.State)
			.MinimapSize(Config.Size)
			.RenderingScale(Config.RenderingScale)
			.SelectedMinimapArea(Config.SelectedMinimapArea)
			.OnGraphMinimapConfigChanged_Static(&FGraphMinimapSpawner::HandleOnGraphMinimapConfigChanged);
		
		ExtensionPoint
		[
			GraphMinimap.ToSharedRef()
		];
	}

	void FGraphMinimapSpawner::HandleOnGraphMinimapConfigChanged(const FString& GraphId, const FGraphMinimapConfig& GraphMinimapConfig)
	{
		if (!UGraphMinimapSettings::Get().bKeepGraphMinimapState)
		{
			return;
		}

		FGraphMinimapConfig& Config = CachedGraphMinimapConfigs.FindOrAdd(GraphId);
		Config = GraphMinimapConfig;
		
		if (!SerializeGraphMinimapStates())
		{
			UE_LOG(LogGraphMinimap, Error, TEXT("Failed to serialize the state of the graph minimap."));
		}
	}

	FGraphMinimapConfig FGraphMinimapSpawner::FindGraphMinimapConfig(const FString& GraphId)
	{
		const auto& Settings = UGraphMinimapSettings::Get();
		if (Settings.bKeepGraphMinimapState)
		{
			if (const FGraphMinimapConfig* FoundConfig = CachedGraphMinimapConfigs.Find(GraphId))
			{
				return *FoundConfig;
			}
		}
		
		FGraphMinimapConfig DefaultConfig;
		DefaultConfig.State = Settings.DefaultGraphMinimapState;
		DefaultConfig.Size = Settings.DefaultGraphMinimapSize;
		DefaultConfig.RenderingScale = Settings.DefaultRenderingScale;
		return DefaultConfig;
	}
	
	FString FGraphMinimapSpawner::GetConfigFilePath()
	{
		return FPaths::ProjectSavedDir() / IGraphMinimap::PluginModuleName.ToString() / TEXT("GraphMinimapStates.json");
	}

	bool FGraphMinimapSpawner::SerializeGraphMinimapStates()
	{
		if (CachedGraphMinimapConfigs.Num() == 0)
		{
			ClearGraphMinimapStates();
			return true;
		}

		const TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		check(JsonObject.IsValid());

		for (const auto& CachedGraphMinimapConfig : CachedGraphMinimapConfigs)
		{
			TSharedPtr<FJsonObject> ConfigJson = MakeShared<FJsonObject>();
			check(ConfigJson.IsValid());
			ConfigJson->SetStringField(
				GET_MEMBER_NAME_STRING_CHECKED(FGraphMinimapConfig, State),
				GraphMinimapState::ToString(CachedGraphMinimapConfig.Value.State)
			);
			ConfigJson->SetStringField(
				GET_MEMBER_NAME_STRING_CHECKED(FGraphMinimapConfig, Size),
				CachedGraphMinimapConfig.Value.Size.ToString()
			);
			ConfigJson->SetNumberField(
				GET_MEMBER_NAME_STRING_CHECKED(FGraphMinimapConfig, RenderingScale),
				CachedGraphMinimapConfig.Value.RenderingScale
			);
			ConfigJson->SetStringField(
				GET_MEMBER_NAME_STRING_CHECKED(FGraphMinimapConfig, SelectedMinimapArea),
				CachedGraphMinimapConfig.Value.SelectedMinimapArea.ToString()
			);
			
			JsonObject->SetObjectField(
				CachedGraphMinimapConfig.Key,
				ConfigJson
			);
		}

		FString JsonString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
		{
			return false;
		}
		
		return FFileHelper::SaveStringToFile(JsonString, *GetConfigFilePath());
	}

	bool FGraphMinimapSpawner::DeserializeGraphMinimapStates()
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		{
			FString JsonString;
			if (!FFileHelper::LoadFileToString(JsonString, *GetConfigFilePath()))
			{
				return !IFileManager::Get().FileExists(*GetConfigFilePath());
			}
	
			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
			if (!FJsonSerializer::Deserialize(JsonReader, JsonObject))
			{
				return false;
			}
		}
		check(JsonObject.IsValid());

		TArray<FString> FieldNames;
		JsonObject->Values.GenerateKeyArray(FieldNames);

		for (const FString& FieldName : FieldNames)
		{
			const TSharedPtr<FJsonObject> ConfigJson = JsonObject->GetObjectField(FieldName);
			check(ConfigJson.IsValid());

			FString StateString;
			if (!ConfigJson->TryGetStringField(GET_MEMBER_NAME_STRING_CHECKED(FGraphMinimapConfig, State), StateString))
			{
				continue;
			}
			const TOptional<EGraphMinimapState> GraphMinimapState = GraphMinimapState::FromString(StateString);
			if (!GraphMinimapState.IsSet())
			{
				continue;
			}
			
			FString SizeString;
			if (!ConfigJson->TryGetStringField(GET_MEMBER_NAME_STRING_CHECKED(FGraphMinimapConfig, Size), SizeString))
			{
				continue;
			}
			FVector2D GraphMinimapSize;
			if (!GraphMinimapSize.InitFromString(SizeString))
			{
				continue;
			}
			
			double GraphMinimapRenderingScale;
			if (!ConfigJson->TryGetNumberField(GET_MEMBER_NAME_STRING_CHECKED(FGraphMinimapConfig, RenderingScale), GraphMinimapRenderingScale))
			{
				continue;
			}

			FString SelectedMinimapAreaString;
			if (!ConfigJson->TryGetStringField(GET_MEMBER_NAME_STRING_CHECKED(FGraphMinimapConfig, SelectedMinimapArea), SelectedMinimapAreaString))
            {
            	continue;
            }
			
			FGraphMinimapConfig Config;
			Config.State = GraphMinimapState.GetValue();
			Config.Size = GraphMinimapSize;
			Config.RenderingScale = GraphMinimapRenderingScale;
			Config.SelectedMinimapArea = FMinimapAreaIdentifier(SelectedMinimapAreaString);

			CachedGraphMinimapConfigs.Add(FieldName, Config);
		}

		return true;
	}

	void FGraphMinimapSpawner::ClearGraphMinimapStates()
	{
		CachedGraphMinimapConfigs.Reset();
		IFileManager::Get().Delete(*GetConfigFilePath(), false, true, false);
	}

	FDelegateHandle FGraphMinimapSpawner::OnActivityChangedHandle;
	FDelegateHandle FGraphMinimapSpawner::OnPostTickHandle;
	TMap<FString, FGraphMinimapConfig> FGraphMinimapSpawner::CachedGraphMinimapConfigs;
}
