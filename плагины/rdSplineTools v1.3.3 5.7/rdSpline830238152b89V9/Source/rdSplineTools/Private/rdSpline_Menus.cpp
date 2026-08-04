//
// rdSpline_Menus.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 13th November 2024

#include "rdSplineTools.h"
#include "Engine/Selection.h"
#include "ToolMenu.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// LevelMenuSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::LevelMenuSubMenu(UToolMenu* menu) {

	actorList.Empty();
	splineList.Empty();
	int32 numSplines=0;
	bool canJoin=false;
	for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {

		AActor* actor=Cast<AActor>(*it);

		bool hasSplines=false;
		for(auto comp:actor->GetComponents()) {
			USplineComponent* splineComp=Cast<USplineComponent>(comp);
			if(splineComp) {
				splineList.Add(splineComp);
				numSplines++;
				hasSplines=true;
			}
		}
		if(hasSplines) {
			actorList.Add(actor);

			TArray<AActor*> aList;
			actor->GetAttachedActors(aList,true);
			for(auto a:aList) {
				if(a->Tags.Contains(TEXT("SubSpline"))) {
					canJoin=true;
					break;
				}
			}
		}
	}

	FToolMenuSection& Section=menu->AddSection("rdSplineToolsSec",LOCTEXT("rdSplineTools_LevelMenu_Label","rdSplineTools"));

	if(numSplines>0) {
	
		FSlateIcon Icon1=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SplitSplineNum","rdSplineTools.SplitSplineNum");
		Section.AddEntry(FToolMenuEntry::InitSubMenu("rdSplineTools1",LOCTEXT("rdSplineTools_LevelMenu1a","Split Spline into X parts"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip1a","Split the Spline into X equal parts"),
													FNewToolMenuDelegate::CreateRaw(this,&FrdSplineToolsModule::SplitSplineIntoSubMenu),false,Icon1));

		if(canJoin) {
			FUIAction Action2=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::JoinSplines));
			FSlateIcon Icon2=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.JoinSplines","rdSplineTools.JoinSpline");
			Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineTools2",LOCTEXT("rdSplineTools_LevelMenu2","Join Splines"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip2","Join the selected Splines at their closest end points"),Icon2,Action2));
		}

		FUIAction Action5=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::CopySplineDataToClipboard2));
		FSlateIcon Icon5=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.CopySplineDataToClipboard","rdSplineTools.CopySplineDataToClipboard");
		Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineTools5",LOCTEXT("rdSplineTools_LevelMenu5","Copy Spline to Clipboard"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip5","Copy the Spline data to Clipboard as text"),Icon5,Action5));

		FUIAction Action6=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::CreateSplineDataFromClipboard2));
		FSlateIcon Icon6=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.CreateSplineDataFromClipboard","rdSplineTools.CreateSplineDataFromClipboard");
		Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineTools6",LOCTEXT("rdSplineTools_LevelMenu6","Create Spline from Clipboard"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip6","Recreate the Spline from text data in the Clipboard"),Icon6,Action6));
	}

	FSlateIcon Icon8=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SubdivideSplines","rdSplineTools.SubdivideSplines");
	Section.AddEntry(FToolMenuEntry::InitSubMenu("rdSplineTools8",LOCTEXT("rdSplineTools_LevelMenu8","Subdivide Splines"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip8","Adds more Spline points along the spline"),
												FNewToolMenuDelegate::CreateRaw(this,&FrdSplineToolsModule::SubdivideSubMenu),false,Icon8));
	
	FSlateIcon Icon9=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.UnSubdivideSplines","rdSplineTools.UnSubdivideSplines");
	Section.AddEntry(FToolMenuEntry::InitSubMenu("rdSplineTools9",LOCTEXT("rdSplineTools_LevelMenu9","UnSubdivide Splines"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip9","Removes Spline points from the spline"),
												FNewToolMenuDelegate::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSubMenu),false,Icon9));

	FUIAction Action10=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::MoveAllSplinePointsToGround));
	FSlateIcon Icon10=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.MoveAllToGround","rdSplineTools.MoveAllToGround");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineTools10",LOCTEXT("rdSplineTools_LevelMenu10","Move All points to Ground"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip10","Moves all the points in the spline to the ground (if found)"),Icon10,Action10));

	FUIAction Action4=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::ShowAbout));
	FSlateIcon Icon4=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.About","rdSplineTools.About");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineTools4",LOCTEXT("rdSplineTools_LevelMenu4","About"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip4","Shows the About Window"),Icon4,Action4));
}

//----------------------------------------------------------------------------------------------------------------
// SubdivideSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SubdivideSubMenu(UToolMenu* menu) {

	FToolMenuSection& Section=menu->AddSection("rdSplineToolsSubdivide",LOCTEXT("rdSplineTools_Subdivide_Label","Subdivide Splines"));

	FUIAction Action2=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines2));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide2",LOCTEXT("rdSplineTools_SubdivideMenu2","2"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip2","double the points"),FSlateIcon(),Action2));
	FUIAction Action3=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines3));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide3",LOCTEXT("rdSplineTools_SubdivideMenu3","3"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip3","3x the points"),FSlateIcon(),Action3));
	FUIAction Action4=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines4));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide4",LOCTEXT("rdSplineTools_SubdivideMenu4","4"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip4","4x the points"),FSlateIcon(),Action4));
	FUIAction Action5=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines5));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide5",LOCTEXT("rdSplineTools_SubdivideMenu5","5"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip5","5x the points"),FSlateIcon(),Action5));
	FUIAction Action6=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines6));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide6",LOCTEXT("rdSplineTools_SubdivideMenu6","6"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip6","6x the points"),FSlateIcon(),Action6));
	FUIAction Action7=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines7));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide7",LOCTEXT("rdSplineTools_SubdivideMenu7","7"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip7","7x the points"),FSlateIcon(),Action7));
	FUIAction Action8=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines8));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide8",LOCTEXT("rdSplineTools_SubdivideMenu8","8"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip8","8x the points"),FSlateIcon(),Action8));
	FUIAction Action9=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines9));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide9",LOCTEXT("rdSplineTools_SubdivideMenu9","9"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip9","9x the points"),FSlateIcon(),Action9));
	FUIAction Action10=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SubdivideSplines10));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSubdivide10",LOCTEXT("rdSplineTools_SubdivideMenu10","10"),LOCTEXT("rdSplineTools_SubdivideMenu_Tooltip10","10x the points"),FSlateIcon(),Action10));
}

//----------------------------------------------------------------------------------------------------------------
// UnSubdivideSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::UnSubdivideSubMenu(UToolMenu* menu) {

	FToolMenuSection& Section=menu->AddSection("rdSplineToolsUnSubdivide",LOCTEXT("rdSplineTools_UnSubdivide_Label","UnSubdivide Splines"));

	FUIAction Action2=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines2));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide2",LOCTEXT("rdSplineTools_UnSubdivideMenu2","2"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip2","half the points"),FSlateIcon(),Action2));
	FUIAction Action3=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines3));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide3",LOCTEXT("rdSplineTools_UnSubdivideMenu3","3"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip3","1/3 the points"),FSlateIcon(),Action3));
	FUIAction Action4=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines4));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide4",LOCTEXT("rdSplineTools_UnSubdivideMenu4","4"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip4","1/4 the points"),FSlateIcon(),Action4));
	FUIAction Action5=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines5));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide5",LOCTEXT("rdSplineTools_UnSubdivideMenu5","5"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip5","1/5 the points"),FSlateIcon(),Action5));
	FUIAction Action6=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines6));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide6",LOCTEXT("rdSplineTools_UnSubdivideMenu6","6"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip6","1/6 the points"),FSlateIcon(),Action6));
	FUIAction Action7=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines7));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide7",LOCTEXT("rdSplineTools_UnSubdivideMenu7","7"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip7","1/7 the points"),FSlateIcon(),Action7));
	FUIAction Action8=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines8));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide8",LOCTEXT("rdSplineTools_UnSubdivideMenu8","8"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip8","1/8 the points"),FSlateIcon(),Action8));
	FUIAction Action9=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines9));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide9",LOCTEXT("rdSplineTools_UnSubdivideMenu9","9"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip9","1/9 the points"),FSlateIcon(),Action9));
	FUIAction Action10=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::UnSubdivideSplines10));
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdUnSplineToolsSubdivide10",LOCTEXT("rdSplineTools_UnSubdivideMenu10","10"),LOCTEXT("rdSplineTools_UnSubdivideMenu_Tooltip10","1/10 the points"),FSlateIcon(),Action10));
}

//----------------------------------------------------------------------------------------------------------------
// SplitSplineIntoSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SplitSplineIntoSubMenu(UToolMenu* menu) {

	if(splineList.Num()<1) return;
	USplineComponent* spline=splineList[0];
	int32 numPoints=spline->GetNumberOfSplinePoints();
	if(numPoints<6) return;

	FToolMenuSection& Section=menu->AddSection("rdSplineToolsSplitSec",LOCTEXT("rdSplineTools_SplitMenu_Label","Split Spline Into"));

	FUIAction Action1=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SplitSplineInto2));
	FSlateIcon Icon1=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SplitSplineNum","rdSplineTools.SplitSplineNum");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSplit1",LOCTEXT("rdSplineTools_SplitMenu1","2 Parts"),LOCTEXT("rdSplineTools_SplitMenu_Tooltip1","Split into 2 parts"),Icon1,Action1));

	if(numPoints<9) return;

	FUIAction Action2=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SplitSplineInto3));
	FSlateIcon Icon2=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SplitSplineNum","rdSplineTools.SplitSplineNum");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSplit2",LOCTEXT("rdSplineTools_SplitMenu2","3 Parts"),LOCTEXT("rdSplineTools_SplitMenu_Tooltip2","Split into 3 parts"),Icon2,Action2));

	if(numPoints<12) return;

	FUIAction Action3=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SplitSplineInto4));
	FSlateIcon Icon3=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SplitSplineNum","rdSplineTools.SplitSplineNum");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSplit3",LOCTEXT("rdSplineTools_SplitMenu3","4 Parts"),LOCTEXT("rdSplineTools_SplitMenu_Tooltip3","Split into 4 parts"),Icon3,Action3));

	if(numPoints<24) return;

	FUIAction Action4=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SplitSplineInto8));
	FSlateIcon Icon4=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SplitSplineNum","rdSplineTools.SplitSplineNum");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSplit4",LOCTEXT("rdSplineTools_SplitMenu4","8 Parts"),LOCTEXT("rdSplineTools_SplitMenu_Tooltip4","Split into 8 parts"),Icon4,Action4));

	if(numPoints<48) return;

	FUIAction Action5=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SplitSplineInto16));
	FSlateIcon Icon5=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SplitSplineNum","rdSplineTools.SplitSplineNum");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSplit5",LOCTEXT("rdSplineTools_SplitMenu5","16 Parts"),LOCTEXT("rdSplineTools_SplitMenu_Tooltip5","Split into 16 parts"),Icon5,Action5));

	if(numPoints<96) return;

	FUIAction Action6=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SplitSplineInto32));
	FSlateIcon Icon6=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SplitSplineNum","rdSplineTools.SplitSplineNum");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSplit6",LOCTEXT("rdSplineTools_SplitMenu6","32 Parts"),LOCTEXT("rdSplineTools_SplitMenu_Tooltip6","Split into 32 parts"),Icon6,Action6));

	if(numPoints<192) return;

	FUIAction Action7=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::SplitSplineInto64));
	FSlateIcon Icon7=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.SplitSplineNum","rdSplineTools.SplitSplineNum");
	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineToolsSplit7",LOCTEXT("rdSplineTools_SplitMenu7","64 Parts"),LOCTEXT("rdSplineTools_SplitMenu_Tooltip7","Split into 64 parts"),Icon7,Action7));
}

//----------------------------------------------------------------------------------------------------------------
// RegisterMenus
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::RegisterMenus() {

	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	// LevelEditor Context Menu
	UToolMenu* menu=UToolMenus::Get()->ExtendMenu("LevelEditor.ActorContextMenu");
#if ENGINE_MAJOR_VERSION>4			
	FToolMenuSection& section=menu->FindOrAddSection("ActorGeneral");
#else
	FToolMenuSection& section=menu->FindOrAddSection("ActorControl");
#endif
	section.AddEntry(FToolMenuEntry::InitSubMenu("rdSplineTools",LOCTEXT("rdSplineTools_LevelMenu","rdSplineTools"),LOCTEXT("rdSplineTools_LevelMenu_Main_Tooltip1","rdSplineTools SubMenu"),FNewToolMenuDelegate::CreateRaw(this,&FrdSplineToolsModule::LevelMenuSubMenu),false,FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.Icon","rdSplineTools.Icon")));

	// Main Editor Tool Menu
#if ENGINE_MAJOR_VERSION<5
	menu=UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Edit");
	FToolMenuSection& section2=menu->FindOrAddSection("Level");
#else
	menu=UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
	FToolMenuSection& section2=menu->FindOrAddSection("PROGRAMMING");
#else
	FToolMenuSection& section2=menu->FindOrAddSection("Tools");
#endif
#endif
	FToolMenuEntry* rdToolsToolMenu=section2.FindEntry("rdToolsToolMenu");
	if(!rdToolsToolMenu) {
		rdToolsToolMenu=&section2.AddEntry(FToolMenuEntry::InitSubMenu("rdToolsToolMenu",LOCTEXT("rdTools_ToolMenu","rdTools"),LOCTEXT("rdTools_ToolMenu_Tooltip1","rdTools SubMenu"),FNewToolMenuChoice(),false,FSlateIcon(FPluginStyle::GetStyleSetName(),"rdTools.Icon","rdTools.Icon")));
	}
#if ENGINE_MAJOR_VERSION<5
	UToolMenu* tmenu=UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Edit.rdToolsToolMenu");
#else
	UToolMenu* tmenu=UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools.rdToolsToolMenu");
#endif
	if(tmenu) {
		FToolMenuSection& tsec=tmenu->FindOrAddSection("rdTools");
		tsec.AddEntry(FToolMenuEntry::InitSubMenu("rdToolsToolMenu",LOCTEXT("rdSplineTools_ToolMenu","rdSplineTools"),LOCTEXT("rdSplineTools_ToolMenu_Tooltip1","rdSplineTools SubMenu"),FNewToolMenuDelegate::CreateRaw(this,&FrdSplineToolsModule::ToolMenuSubMenu),false,FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.Icon","rdSplineTools.Icon")));
	}
/*
	// World Outliner Menu
	menu=UToolMenus::Get()->ExtendMenu("LevelEditor.SceneOutlinerContextMenu");
#if ENGINE_MAJOR_VERSION>4			
	FToolMenuSection& Section3=Menu->FindOrAddSection("ActorGeneral");
#else
	FToolMenuSection& Section3=Menu->FindOrAddSection("ActorControl");
#endif
	section3.AddEntry(FToolMenuEntry::InitSubMenu("rdSplineTools",LOCTEXT("rdSplineTools_LevelMenu","rdSplineTools"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip1a","rdSplineTools Outline SubMenu"),FNewToolMenuDelegate::CreateRaw(this,&FrdSplineToolsModule::OutlinerMenuSubMenu),false,FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.Icon","rdSplineTools.Icon")));
*/
}

//----------------------------------------------------------------------------------------------------------------
// OutlinerMenuSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::OutlinerMenuSubMenu(UToolMenu* menu) {

//	FToolMenuSection& Section=menu->AddSection("rdSplineToolsSec",LOCTEXT("rdSplineTools_LevelMenu_Label","rdSplineTools"));

//	FUIAction Action2a=FUIAction(FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::JoinAllSplines));
//	FSlateIcon Icon2a=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdSplineTools.JoinAllSplines","rdSplineTools.JoinAllSplines");
//	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdSplineTools2a",LOCTEXT("rdSplineTools_LevelMenu2a","Join All Splines"),LOCTEXT("rdSplineTools_LevelMenu_Tooltip2a","Join All associated Splines at their closest end points"),Icon2a,Action2a));

//	FUIAction Action1=FUIAction(FExecuteAction::CreateRaw(this,&FrdBPTools::CollapseBrowserAllButThis));
//	FSlateIcon Icon1=FSlateIcon(FPluginStyle::GetStyleSetName(),"rdBPTools.Collapse","rdBPTools.Collapse");
//	Section.AddEntry(FToolMenuEntry::InitMenuEntry("rdBPtools_sub1",LOCTEXT("rdBPtools_BPOutlineMenu1","Collapse all but this"),LOCTEXT("rdBPtools_BPOutlineMenu1_Tooltip1","Collapse All Folders except for this one"),Icon1,Action1));
}

//----------------------------------------------------------------------------------------------------------------
// RegisterCommands
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsMenuCommands::RegisterCommands() {

	UI_COMMAND(SplitSpline,"Split Spline Here","Split the Spline at the selected point",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(SplitSplineSection,"Split Spline Section","Split the Spline into 3 around the selected section",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(JoinSplines,"Join Splines","Join the Splines at their closest end points",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(JoinAllSplines,"Join All Splines","Join All the Splines at their closest end points",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(NewSplineHere,"New Spline Here","Create a new Spline at the selected point",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CopySplineDataToClipboard,"Copy SplineData to Clipboard","Copies all the spline point data to the clipboard in text format",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CreateSplineDataFromClipboard,"Create SplineData from Clipboard","Creates the spline point data from text in the clipboard",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(StraightenPoints,"Straighten Points","Straightens the selected points in the spline",EUserInterfaceActionType::Button,FInputChord());
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	