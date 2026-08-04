// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GraphMinimapStreamDeck/Types/StreamDeckInstallButton.h"
#include "GraphMinimapStreamDeckSettings.generated.h"

/**
 * A editor preferences class related to the stream deck of this plugin.
 */
UCLASS(Config = Editor)
class GRAPHMINIMAPSTREAMDECK_API UGraphMinimapStreamDeckSettings : public UObject
{
	GENERATED_BODY()

protected:
	// A dummy struct to display the Stream Deck install button in the details panel.
	UPROPERTY(EditAnywhere, Category = "Stream Deck")
	FStreamDeckInstallButton Dummy;

public:
	// Registers-Unregisters in the editor setting item.
	static void Register();
	static void Unregister();
};
