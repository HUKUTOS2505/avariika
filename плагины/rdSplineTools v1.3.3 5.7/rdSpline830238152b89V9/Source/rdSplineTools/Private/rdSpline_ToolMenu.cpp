//
// rdSpline_ToolMenu.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 27th March 2024
// Last Modified: 22nd March 2025

#include "rdSplineTools.h"
#include "Engine/Selection.h"
#include "ToolMenu.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// ToolMenuSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::ToolMenuSubMenu(UToolMenu* menu) {

	FToolMenuSection& section=menu->AddSection("rdSplineToolsSec",LOCTEXT("rdSplineTools_ToolMenu_Label1","rdSplineTools"));

	FSlateIcon icon1=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.UnSubdivideSplines","rdSplineTools.UnSubdivideSplines");
	section.AddEntry(FToolMenuEntry::InitSubMenu("rdSplineToolsTool1",LOCTEXT("rdSplineTools_ToolMenu1","Landscape Splines"),LOCTEXT("rdSplineTools_ToolMenu_Tooltip1","Tools for Landscape Splines"),
												FNewToolMenuDelegate::CreateRaw(this,&FrdSplineToolsModule::ToolsLandscapeSubMenu),false,icon1));

	FUIAction action2=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::Settings));
	FSlateIcon icon2=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.Settings","rdSplineTools.Settings");
	section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsTool2",LOCTEXT("rdSplineTools_ToolMenu2","Settings"),LOCTEXT("rdSplineTools_ToolMenu_Tooltip2","Opens the Settings Window"),icon2,action2));

	FUIAction Action4=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::ShowAbout));
	FSlateIcon Icon4=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.About","rdSplineTools.About");
	section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsTool4",LOCTEXT("rdSplineTools_LevelMenu4","About"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip4","Shows the About Window"),Icon4,Action4));
}

//----------------------------------------------------------------------------------------------------------------
// ToolsLandscapeSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::ToolsLandscapeSubMenu(UToolMenu* menu) {

	FToolMenuSection& section=menu->AddSection("rdSplineToolsTMSec",LOCTEXT("rdSplineTools_ToolsMenu_Label2","rdSplineTools"));

	FUIAction action2=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::TransformLandscapeSplines));
	FSlateIcon icon2=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.TransformLandscapeSplines","rdSplineTools.TransformLandscapeSplines");
	section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineTools5",LOCTEXT("rdSplineTools_ToolMenu3","Transform"),LOCTEXT("rdSplineTools_ToolMenu_Tooltip3","Transform all Landscape Splines"),icon2,action2));

	FUIAction Action7=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::ConvertLandscapeSplines));
	FSlateIcon Icon7=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.ConvertLandscapeSplines","rdSplineTools.ConvertLandscapeSplines");
	section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineTools7",LOCTEXT("rdSplineTools_LevelMenu7","Convert Landscape Splines"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip7","Converts the Landscape Splines into BP Splines"),Icon7,Action7));

}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	