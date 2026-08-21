//
// rdSplineTools.cpp
//
// Copyright Recourse Design ltd, 2023, Inc. All Rights Reserved.
//
// Version 1.31
//
// Creation Date: 6th May 2023
// Last Modified: 9th September 2025
//
//  * TODO:   Split up splines per WP tile tool
//  * TODO:   Option to round to the nearest section with splitting splines
//  * TODO:   BP Library exposing tools to Blueprint
//  * TODO:   Convert BP spline to LandscapeSpline tool
//
//
// Version 1.31
//	* FIXED:  Crash when trying to convert a landscape spline that contains a mesh
//	* FIXED:  rdSplineTools options were not being saved in the spline visualizer
//	* FIXED:  Rewrote the LandscapeSpline Import routines, now work with WP and non-WP
//  * CHANGED:Fixed size icons (zoom for camera) for snapping points
//
// Version 1.30
//  * ADDED:  Auto Draw when dragging splines
//  * ADDED:  Snapping to Spline Points (with icons)
//  * ADDED:  Icons for snapping to sockets
//  * ADDED:  New options for socket snapping to default to on
//  * ADDED:  Snapping Settings window
//  * ADDED:  Snap/Perpendicular mode
//  * ADDED:  Move All Spline Points to Ground tool
//
// Version 1.20
//  * ADDED:  Tool to set all spline points to landscape
//  * ADDED:  Tool to set current point to closest point of neighboring splines
//  * FIXED:  Set Height to landscape was not working for single or multiple points
//  * FIXED:  Set Height to First/Last points not working
//
// Version 1.10
//	* ADDED:  Intersection detection (socket based)
//  * ADDED:  Snap spline points to same x,y or z -set from first or last selected point (or surrounding)
//  * ADDED:  Place spline on ground
//  * ADDED:  Tool to convert landscape splines to blueprint splines (experimental)
//  * ADDED:  Transform all landscape splines tool
//  * ADDED:  Main Tools menu additions
//  * ADDED:  Add subdivide spline tool
//  * ADDED:  Add un-subdivide spline tool
//  * ADDED:  Add Settings window for snapping radius etc
//  * FIXED:  Pasting a spline would set the curve type to constant
//  * FIXED:  Shortcuts
//
// Version 1.0
//
//	* ADDED: Tool to Split Spline from current point
//	* ADDED: Tool to Split Spline into 3 from selected Range
//	* ADDED: Tool to Split Spline into "x" amount of sub splines
//	* ADDED: Tool to recombine split splines into one
//	* ADDED: Tool to maintain end points of "sub" splines from a split when edited
//	* ADDED: Tools to copy spline data to clipboard, and recreate spline from that clipboard data
//	* ADDED: Snapping system to snap to any Socket on SplineMeshes
//
//rdSplineData|1|49|0:-64753.358157:8035.225245:370:34750:-4460:0:34750:-4460:0:3:0:0:0:1:1:1|1:-26149.999999:2123.017741:370:16947.97359:-3579.208923:0:16947.97359:-3579.208923:0:3:0:0:0:1:1:1|2:-12596.998956:-192.337149:369.999995:13075.000112:-598.366711:-70.000002:13075.000112:-598.366711:-70.000002:3:0:0:0:1:1:1|3:-3796.08247:-649.985261:230:12070.838562:-615.596458:-185:12070.838562:-615.596458:-185:3:0:0:0:1:1:1|4:25567.458947:1100.506971:0:24315.846399:3103.670688:-281.354319:24315.846399:3103.670688:-281.354319:1:-0.000407:0.065453:0.006208:1:1:1|5:44835.610327:5557.356115:-332.708638:19740.41017:4945.660939:69.021945:19740.41017:4945.660939:69.021945:1:0.000194:0.122433:-0.001685:1:1:1|6:65048.279287:10991.82885:138.04389:18365.144217:5207.773207:346.493819:18365.144217:5207.773207:346.493819:3:0.00125:0.137713:-0.008988:1:1:1|7:81565.898762:15972.902528:360.279001:16785.860357:5284.085575:205.978055:16785.860357:5284.085575:205.978055:1:0.000889:0.151894:-0.005784:1:1:1|8:98620:21560:550:17377.318409:6080.179525:96.385286:17377.318409:6080.179525:96.385286:1:0.000414:0.155532:-0.002632:1:1:1|9:116320.535581:28133.261579:553.049574:17947.183837:7464.977415:1.524787:17947.183837:7464.977415:1.524787:3:0.000008:0.195813:-0.000038:1:1:1|10:134242.825889:38521.483958:553.049574:26247.816162:20060.022585:58.475213:26247.816162:20060.022585:58.475213:3:0.000284:0.320522:-0.000838:1:1:1|11:148191.211494:57901.530948:670:11061.602892:18523.346541:60.036141:11061.602892:18523.346541:60.036141:1:0.000633:0.473556:-0.001178:1:1:1|12:156366.031674:75568.177041:673.121855:8545:18555:0:8545:18555:0:3:0:0.539306:0:1:1:1|13:164389.325404:94283.979482:670:6279.365447:15032.59305:-1.560928:6279.365447:15032.59305:-1.560928:1:-0.000028:0.557279:0.000041:1:1:1|14:168924.762568:105633.363142:670:4725.551085:12762.306422:-10.005645:4725.551085:12762.306422:-10.005645:1:-0.00022:0.578243:0.000311:1:1:1|15:173840.427575:119808.592325:649.98871:4633.871632:15221.651574:-10.005645:4633.871632:15221.651574:-10.005645:3:-0.000187:0.595302:0.000253:1:1:1|16:178156.545674:135580.714873:649.98871:3318.041763:14512.071562:-29.723828:3318.041763:14512.071562:-29.723828:1:-0.000613:0.614077:0.000788:1:1:1|17:180476.5111:148832.735448:590.541054:1859.461288:12979.409797:-29.723828:1859.461288:12979.409797:-29.723828:1:-0.000666:0.651388:0.000776:1:1:1|18:181875.468249:161539.534466:590.541054:385.199241:15907.440658:4.729473:385.199241:15907.440658:4.729473:3:0.000104:0.698496:-0.000106:1:1:1|19:180767.585244:176515.062454:600:-2701.255893:13543.209673:114.729473:-2701.255893:13543.209673:114.729473:1:0.003291:0.762978:-0.002788:1:1:1|20:176472.956463:188625.953812:820:-6479.519896:12750.613408:225:-6479.519896:12750.613408:225:1:0.007771:0.856118:-0.00469:1:1:1|21:167808.545453:202016.289269:1050:-10695:12050:365:-10695:12050:365:3:0.010329:0.912028:-0.004643:1:1:1|22:158226.386076:212302.861597:1550:-8518.409616:8938.000772:381.134708:-8518.409616:8938.000772:381.134708:1:0.012154:0.927026:-0.004914:1:1:1|23:150771.72622:219892.290812:1812.269416:-9297.682379:8042.346922:334.524626:-9297.682379:8042.346922:334.524626:1:0.012878:0.92854:-0.005145:1:1:1|24:139631.021317:228387.555441:2219.049251:-12144.044684:10459.315308:368.389917:-12144.044684:10459.315308:368.389917:3:0.010772:0.93741:-0.003999:1:1:1|25:125246.966044:239521.941287:2549.049251:-14588.326296:12033.649862:255.475374:-14588.326296:12033.649862:255.475374:1:0.00594:0.93649:-0.002224:1:1:1|26:110454.368725:252454.855164:2730:-15355.514631:16971.883808:-74.524626:-15355.514631:16971.883808:-74.524626:1:-0.001484:0.913052:0.000663:1:1:1|27:94535.936781:273465.708904:2400:-15565:22775:-648.096319:-15565:22775:-648.096319:3:-0.010387:0.884315:0.005482:1:1:1|28:77729.730248:299168.64707:1433.807363:-12917.018695:20439.356978:-1045:-12917.018695:20439.356978:-1045:1:-0.018127:0.866795:0.010419:1:1:1|29:68701.899391:314344.42286:310:-7875:23995:-1420:-7875:23995:-1420:3:-0.022742:0.809566:0.016472:1:1:1|30:54966.649397:335499.937015:-3430:-28820:13555:-1870:-28820:13555:-1870:3:-0.028614:0.975518:0.006393:1:1:1|31:38620.163433:342276.538305:-3430:-27564.07396:7485.894318:-450:-27564.07396:7485.894318:-450:1:-0.007617:0.992445:0.00094:1:1:1|32:-161.498522:350471.725651:-4330:-32840.081716:6651.730847:-575:-32840.081716:6651.730847:-575:1:-0.010566:0.995761:0.000969:1:1:1|33:-27060:355580:-4580:-27398.141545:6556.999607:-320:-27398.141545:6556.999607:-320:3:-0.00564:0.993094:0.000665:1:1:1|34:-57308.834978:364135.314824:-4970:-26942.260952:419.873886:575:-26942.260952:419.873886:575:1:-0.010689:-0.999894:-0.000105:1:1:1|35:-80944.521904:356419.747771:-3430:-22509.508227:-11283.904761:2250:-22509.508227:-11283.904761:2250:1:-0.0446:-0.973528:-0.010259:1:1:1|36:-102327.851431:341567.505301:-470:-9586.132678:-16898.901677:2518.074249:-9586.132678:-16898.901677:2518.074249:3:-0.055649:-0.862326:-0.032412:1:1:1|37:-103598.20852:319984.163022:1606.148499:891.154716:-23745.116709:1038.074249:891.154716:-23745.116709:1038.074249:1:-0.014513:-0.690673:-0.01519:1:1:1|38:-100545.542:294077.271883:1606.148499:3216.022492:-25239.909893:0:3216.022492:-25239.909893:0:1:0:-0.660209:0:1:1:1|39:-97166.163536:269504.343236:1606.148499:3385:-28135:0:3385:-28135:0:3:0:-0.663532:0:1:1:1|40:-93348.3141:237152.196538:1606.148499:3116.993337:-31392.171617:36.925751:3116.993337:-31392.171617:36.925751:1:-0.0004:-0.670438:-0.000443:1:1:1|41:-90932.176861:206720.000003:1680:603.049049:-25896.725236:36.925751:603.049049:-25896.725236:36.925751:1:-0.000485:-0.693767:-0.000504:1:1:1|42:-92142.216003:185358.746065:1680:-2880:-22480:-220:-2880:-22480:-220:3:0.003643:-0.750683:0.003206:1:1:1|43:-95547.238822:161759.999984:1240:-4848.985248:-29912.099662:-220:-4848.985248:-29912.099662:-220:3:0.002183:-0.789728:0.001695:1:1:1|44:-105495.41744:138729.999991:1240:-14475.177551:-24404.999951:-0:-14475.177551:-24404.999951:-0:3:0:-0.837495:0:1:1:1|45:-117448.371629:112949.999796:1240:-5790:-31660:0:-5790:-31660:0:3:0:-0.768081:0:1:1:1|46:-117776.902784:75410.000649:1240:8947.122162:-36382.710611:0:8947.122162:-36382.710611:0:1:0:-0.617663:0:1:1:1|47:-99554.127305:40184.578574:1240:26512.253841:-33687.550858:-434.291971:26512.253841:-33687.550858:-434.291971:1:0.001808:-0.438363:0.003708:1:1:1|48:-64752.395102:8034.898932:371.416057:49522.461514:-3417.002431:0:49522.461514:-3417.002431:0:3:0:-0.034438:0:1:1:1|
//
#include "rdSplineTools.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"
#include "ToolMenuDelegates.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/Selection.h"
#include "LevelEditorActions.h"
#include "Misc/OutputDeviceNull.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"
 
//----------------------------------------------------------------------------------------------------------------
// StartupModule
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::StartupModule() {

	ignoreMove=false;
	FPluginStyle::Initialize();
	FPluginStyle::ReloadTextures();

	// Commands List and Hotkeys
	FrdSplineToolsMenuCommands::Register();
	auto const cmdList=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame").GetMainFrameCommandBindings();
	cmdList->MapAction(FrdSplineToolsMenuCommands::Get().JoinSplines,FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::JoinSplines));
	cmdList->MapAction(FrdSplineToolsMenuCommands::Get().JoinAllSplines,FExecuteAction::CreateRaw(this,&FrdSplineToolsModule::JoinAllSplines));

//	FLevelEditorModule& LevelEditorModule=FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,&FrdSplineToolsModule::RegisterMenus));

	if(GUnrealEd) {

		//rdSplineToolsoptions=DuplicateObject(GetMutableDefault<UrdSplineToolsOptions>(),GetTransientPackage());
		//rdSplineToolsoptions->splineModule=this;
		//FillFromOptionDefaults(TEXT("Defaults"),rdSplineToolsoptions);

		rdSplineVis=MakeShareable(new FrdSplineComponentVisualizer(this));
		rdSplineVis->OnRegister();
		//rdSplineVis->rdSplineToolsoptions->splineModule=this;
		//FillFromOptionDefaults(TEXT("Defaults"),rdSplineVis->rdSplineToolsoptions);
		rdSplineVis->RegisterCommands(rdLevelEditorCommands);
		GUnrealEd->RegisterComponentVisualizer(USplineComponent::StaticClass()->GetFName(),rdSplineVis);
	}

	GEngine->OnActorMoving().AddRaw(this,&FrdSplineToolsModule::OnActorMoving);
	doDraw=false;
	GEngine->OnPostEditorTick().AddRaw(this,&FrdSplineToolsModule::rdTick);

}

//----------------------------------------------------------------------------------------------------------------
// rdTick
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::rdTick(float deltaSeconds) {

	if(!doDraw) return;

	FOutputDeviceNull ar;
	for(auto a:dirtyActorList) {
		a->CallFunctionByNameWithArguments(TEXT("rdBuildSpline"),ar,NULL,true);
		UFunction* func=a->FindFunction(FName(TEXT("rdBuildRoad")));
		if(func) {
			a->ProcessEvent(func,nullptr);
		}

	}

	dirtyActorList.Empty();
	doDraw=false;
}

//----------------------------------------------------------------------------------------------------------------
// OnActorMoved
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::OnActorMoving(AActor* actor) {

	if(ignoreMove) return;

	if(actor->Tags.Contains(FName(TEXT("SubSpline"))) && !IsMainSplineSelected(actor)) {

		AActor* par=actor->GetAttachParentActor();
		if(par) {
			FVector loc=actor->GetActorLocation()-par->GetActorLocation();
			if(fabs(loc.X)>0.001 || fabs(loc.Y)>0.001 || fabs(loc.Z)>0.001) {
				ignoreMove=true;
				par->SetActorLocation(par->GetActorLocation()+loc);
				actor->SetActorRelativeTransform(FTransform(FRotator(0,0,0),FVector(0,0,0),FVector(1,1,1)));
				ignoreMove=false;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
// ShutdownModule
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::ShutdownModule() {

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FPluginStyle::Shutdown();
}

//----------------------------------------------------------------------------------------------------------------
// SetOptionDefaults
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineToolsModule::SetOptionDefaults(const FString& name,UrdSplineToolsOptions* Options) {

	FConfigCacheIni::LoadGlobalIniFile(configIni,TEXT("rdSplineToolsConfig"));

	FString str=*Options->ToString();

	FConfigFile cfg; 
	cfg.Read(configIni);
	cfg.SetString(TEXT("CurrentSettings"),*name,*str);
	//cfg.UpdateSinglePropertyInSection(*configIni,TEXT("CurrentSettings"),*name);
	cfg.Write(configIni,false,TEXT(""));

	//check(GConfig && GConfig->IsReadyForUse());
	//GConfig->SetString(TEXT("CurrentSettings"),*name,*str,configIni);
	//GConfig->Flush(0);

	return true;
}

//----------------------------------------------------------------------------------------------------------------
// FillFromOptionDefaults
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineToolsModule::FillFromOptionDefaults(const FString& name,UrdSplineToolsOptions* Options) {

	FString str;

	FConfigCacheIni::LoadGlobalIniFile(configIni,TEXT("rdSplineToolsConfig"));
	//check(GConfig && GConfig->IsReadyForUse());

	//if(!GConfig->GetString(TEXT("CurrentSettings"),*name,str,configIni)) {
		FConfigFile cfg; 
		cfg.Read(configIni);
		if(!cfg.GetString(TEXT("CurrentSettings"),*name,str)) {
			return false;
		}
	//}

	Options->FromString(str);

	return true;
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FrdSplineToolsModule, rdSplineTools)