// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GraphMinimapRemoteControlSettings.generated.h"

/**
 * A editor preferences class related to the remote control of this plugin.
 */
UCLASS(Config = Editor, GlobalUserConfig)
class GRAPHMINIMAPREMOTECONTROL_API UGraphMinimapRemoteControlSettings : public UObject
{
	GENERATED_BODY()

public:
	// Whether remote control via HTTP server is enabled.
	// Please check again when the server is rebuilt.
	UPROPERTY(EditAnywhere, Config, Category = "Remote Control")
	bool bEnableRemoteControl;
	
	// The URL to connect to HTTP server.
	// Disable remote control once to edit.
	UPROPERTY(EditAnywhere, Config, Category = "Remote Control", meta = (EditCondition = "!bEnableRemoteControl"))
	FString HttpPath;

	// The port number used to connect to the HTTP server.
	UPROPERTY(EditAnywhere, Config, Category = "Remote Control", meta = (EditCondition = "!bEnableRemoteControl"))
	uint32 PortNumber;

public:
	// The event called when remote control is enabled.
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRemoteControlEnabled, const FString& /* HttpPath */, const uint32 /* PortNumber */);
	static FOnRemoteControlEnabled OnRemoteControlEnabled;

	// The event called when remote control is disabled.
	DECLARE_MULTICAST_DELEGATE(FOnRemoteControlDisabled);
	static FOnRemoteControlDisabled OnRemoteControlDisabled;
	
public:
	// Constructor.
	UGraphMinimapRemoteControlSettings();

	// Registers-Unregisters in the editor setting item.
	static void Register();
	static void Unregister();
	
	// Returns reference of this settings.
	static const UGraphMinimapRemoteControlSettings& Get();

	// UObject interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
};
