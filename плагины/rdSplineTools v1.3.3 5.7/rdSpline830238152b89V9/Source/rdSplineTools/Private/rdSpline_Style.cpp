//
// rdSpline_Style
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 23rd March 2024

#include "rdSpline_Style.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr<FSlateStyleSet> FPluginStyle::StyleSet=nullptr;

//----------------------------------------------------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------------------------------------------------
void FPluginStyle::Initialize() {

	if(StyleSet.IsValid()) {
		return;
	}

	StyleSet=MakeShared<FSlateStyleSet>(GetStyleSetName());
	StyleSet->SetContentRoot(FPaths::EngineContentDir()/TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir()/TEXT("Slate"));

	const FVector2D Icon20x20(20.0f,20.0f);
	const FVector2D Icon40x40(40.0f,40.0f);

	StyleSet->Set("rdTools.Icon",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdTools_20px"),Icon20x20));
	StyleSet->Set("rdSplineTools",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdSplineTools_40px"),Icon40x40));
	StyleSet->Set("rdSplineTools.Small",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdSplineTools_40px"),Icon20x20));
	StyleSet->Set("rdSplineTools.Logo",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdSplineTools_350px"),FVector2D(140.0,140.0)));
	StyleSet->Set("rdSplineTools.Icon",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdSplineTools_20px"),Icon20x20));

	StyleSet->Set("rdSplineTools.SubMenu",new IMAGE_BRUSH(TEXT("Icons/ActorIcons/Spline_16px"), Icon20x20));
	StyleSet->Set("rdSplineTools.SplitSpline",new IMAGE_BRUSH(TEXT("Icons/icon_scaleb_16x"), Icon20x20));
	StyleSet->Set("rdSplineTools.SplitSplineNum",new IMAGE_BRUSH(TEXT("Icons/icon_scaleb_16x"), Icon20x20));
	StyleSet->Set("rdSplineTools.JoinSplines",new IMAGE_BRUSH(TEXT("Icons/icon_Landscape_Tool_Splines_20x"), Icon20x20));
	StyleSet->Set("rdSplineTools.JoinAllSplines",new IMAGE_BRUSH(TEXT("Icons/icon_Landscape_Tool_Splines_20x"), Icon20x20));
	StyleSet->Set("rdSplineTools.NewSplineHere",new IMAGE_BRUSH(TEXT("Icons/PlusSymbol_12x"), Icon20x20));
	StyleSet->Set("rdSplineTools.StraightenPoints",new IMAGE_BRUSH(TEXT("Icons/FitHorz_16x"), Icon20x20));
	StyleSet->Set("rdSplineTools.CopySplineDataToClipboard",new IMAGE_BRUSH(TEXT("Icons/icon_Asset_Duplicate_16x"), Icon20x20));
	StyleSet->Set("rdSplineTools.CreateSplineDataFromClipboard",new IMAGE_BRUSH(TEXT("Icons/icon_Import_16x"), Icon20x20));
	StyleSet->Set("rdSplineTools.SubdivideSplines",new IMAGE_BRUSH(TEXT("Icons/icon_scaleb_16x"), Icon20x20));
	StyleSet->Set("rdSplineTools.UnSubdivideSplines",new IMAGE_BRUSH(TEXT("Icons/icon_Landscape_Tool_Splines_20x"), Icon20x20));
	StyleSet->Set("rdSplineTools.About",new IMAGE_BRUSH(TEXT("Icons/AssetIcons/DocumentationActor_16x"), Icon20x20));
	StyleSet->Set("rdSplineTools.Settings",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdSplineTools_20px"), Icon20x20));
	StyleSet->Set("rdSplineTools.TransformLandscapeSplines",new IMAGE_BRUSH(TEXT("Icons/FitHorz_16x"), Icon20x20));
	StyleSet->Set("rdSplineTools.ConvertLandscapeSplines",new IMAGE_BRUSH(TEXT("Icons/ActorIcons/Spline_16px"), Icon20x20));
	StyleSet->Set("rdSplineTools.MoveAllToGround",new IMAGE_BRUSH(TEXT("Icons/icon_Landscape_Tool_Splines_20x"), Icon20x20));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());

}

//----------------------------------------------------------------------------------------------------------------
// Shutdown
//----------------------------------------------------------------------------------------------------------------
void FPluginStyle::Shutdown() {

	if(StyleSet.IsValid()) {

		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}

}

//----------------------------------------------------------------------------------------------------------------
// InContent
//----------------------------------------------------------------------------------------------------------------
FString FPluginStyle::InContent(const FString& RelativePath,const ANSICHAR* Extension) { 
	
	static FString ContentDir=IPluginManager::Get().FindPlugin(TEXT("rdSplineTools"))->GetContentDir(); 
	return (ContentDir/RelativePath)+Extension; 
}

//----------------------------------------------------------------------------------------------------------------
	