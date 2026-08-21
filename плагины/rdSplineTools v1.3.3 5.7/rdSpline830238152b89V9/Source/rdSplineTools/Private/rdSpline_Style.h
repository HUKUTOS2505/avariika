// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Interfaces/IPluginManager.h"

#define IMAGE_BRUSH(RelativePath,...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath,TEXT(".png")),__VA_ARGS__)
#define IMAGE_PLUGIN_BRUSH(RelativePath,...) FSlateImageBrush(FPluginStyle::InContent(RelativePath,".png"),__VA_ARGS__)

class FPluginStyle {
public:
	static void Initialize();
	static void Shutdown();
	static TSharedPtr<class ISlateStyle> Get() { return StyleSet; }
	static FName GetStyleSetName() { static FName StyleName("rdSplineToolsStyle"); return StyleName; }
	static void ReloadTextures() { if(FSlateApplication::IsInitialized()) FSlateApplication::Get().GetRenderer()->ReloadTextureResources(); }
	static FString InContent(const FString& RelativePath,const ANSICHAR* Extension);
private:
	static TSharedPtr<class FSlateStyleSet> StyleSet;
};

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#define GetAppFontStyle FAppStyle::GetFontStyle
#define GetAppStyle FAppStyle::Get
#define GetAppBrush FAppStyle::GetBrush
#else
#define GetAppFontStyle FEditorStyle::GetFontStyle
#define GetAppStyle FEditorStyle::Get
#define GetAppBrush FEditorStyle::GetBrush
#endif
