// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ComponentVisualizer.h"
#include "Components/SplineComponent.h"
#include "SplineComponentVisualizer.h"
#include "rdSpline_Style.h"
#include "rdSpline_SplineVisualizer.h"
#include "IDetailRootObjectCustomization.h"
#include "rdSplineToolsOptions.h"

#define RDSPLINETOOLS_MAJOR_VERSION 1
#define RDSPLINETOOLS_MINOR_VERSION 33
#define RDSPLINETOOLS_UPDATE_VERSION 0
#define RDSPLINETOOLS_RELEASEDATE "5th October 2025"

class ULandscapeSplineControlPoint;

struct FSplineChain {
	TArray<ULandscapeSplineControlPoint*> points;
	bool bClosedLoop=false;
};


class FrdSplineToolsMenuCommands : public TCommands<FrdSplineToolsMenuCommands> {
public:
	FrdSplineToolsMenuCommands() : TCommands<FrdSplineToolsMenuCommands>(TEXT("rdSplineTools"),NSLOCTEXT("Contexts","rdSplineTools","rdSplineTools Plugin"),NAME_None,FPluginStyle::GetStyleSetName()) {}

	virtual void RegisterCommands() override;
public:
	TSharedPtr<FUICommandInfo> SplitSpline;
	TSharedPtr<FUICommandInfo> SplitSplineSection;
	TSharedPtr<FUICommandInfo> SplitSplineInto;
	TSharedPtr<FUICommandInfo> JoinSplines;
	TSharedPtr<FUICommandInfo> JoinAllSplines;
	TSharedPtr<FUICommandInfo> NewSplineHere;
	TSharedPtr<FUICommandInfo> CopySplineDataToClipboard;
	TSharedPtr<FUICommandInfo> CreateSplineDataFromClipboard;
	TSharedPtr<FUICommandInfo> StraightenPoints;
};

class FrdSplineToolsModule : public IModuleInterface {

public:
	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;

	bool			SetOptionDefaults(const FString& name,UrdSplineToolsOptions* Options);
	bool			FillFromOptionDefaults(const FString& name,UrdSplineToolsOptions* Options);

	void			RegisterMenus();

	void			ToolMenuSubMenu(UToolMenu* menu);
	void			ToolsLandscapeSubMenu(UToolMenu* menu);

	void			LevelMenuSubMenu(UToolMenu* menu);
	void			OutlinerMenuSubMenu(UToolMenu* menu);

	void			SubdivideSubMenu(UToolMenu* menu);
	void			UnSubdivideSubMenu(UToolMenu* menu);

	void			SplitSplineIntoSubMenu(UToolMenu* menu);

	void			ShowAbout();
	bool			ShowSettings();
	void			Settings();
	UClass*			ShowLandscapeSplineSettings();

	bool			ShowLandscapeSplineTransformsSettings();

	void			MoveAllSplinePointsToGround();

	bool			SplineActorHasSplits(AActor* actor);
	bool			SplineActorIsSplit(AActor* actor);
	AActor*			DuplicateActor(AActor* actor);
	AActor*			NewActor(AActor* actor,const FTransform& tran);
	void			copySpline(USplineComponent* spline1,USplineComponent* spline2,int32 start,int32 len);
	void			trimSpline(USplineComponent* spline,int32 start,int32 len);
	int32			GetSubSplineIndex(AActor* actor);
	int32			GetNewSplineIndex(AActor* actor);
	AActor*			FindSubSpline(AActor* actor,int32 ind,int32& cnt);
	USplineComponent* GetSpline(AActor* actor);
	void			MergeActorsSubSplines(AActor* actor);
	void			CopySplinePoint(int32 tangT,USplineComponent* s1,int32 i1,USplineComponent* s2,int32 i2,FVector& deltaTranslate,FRotator& deltaRotate,FVector& deltaScale);
	void			rdTick(float deltaSeconds);
	void			OnActorMoving(AActor* actor);
	bool			IsMainSplineSelected(AActor* actor);
	void			InsertAttachActorIndexes(AActor* actor,int32 num);
	FSplinePoint	FillSplinePoint(USplineComponent* spline,int32 point,float startKey=0.0f);

	void			SplitSpline(FrdSplineComponentVisualizer* vis,bool rounded);
	void			SplitSplineSection(FrdSplineComponentVisualizer* vis);
	void			NewSplineHere(FrdSplineComponentVisualizer* vis);
	void			StraightenPoints(FrdSplineComponentVisualizer* vis);

	void			SetPointsHeightToFirst(FrdSplineComponentVisualizer* vis);
	void			SetPointsHeightToLast(FrdSplineComponentVisualizer* vis);
	void			SetPointsHeightAdjacent(FrdSplineComponentVisualizer* vis,int32 num);
	void			SetPointsHeightToLandscape(FrdSplineComponentVisualizer* vis,const FVector offset);

	void			CreateLeftIntersection(FrdSplineComponentVisualizer* vis);
	void			ClearLeftIntersection(FrdSplineComponentVisualizer* vis);
	void			CreateRightIntersection(FrdSplineComponentVisualizer* vis);
	void			ClearRightIntersection(FrdSplineComponentVisualizer* vis);

	void			CopySplineDataToClipboard(FrdSplineComponentVisualizer* vis);
	void			CreateSplineDataFromClipboard(FrdSplineComponentVisualizer* vis);

	void			CopySplineDataToClipboard2();
	void			CreateSplineDataFromClipboard2();

	void			SplitSplineInto2() { SplitSplineInto(2); }
	void			SplitSplineInto3() { SplitSplineInto(3); }
	void			SplitSplineInto4() { SplitSplineInto(4); }
	void			SplitSplineInto8() { SplitSplineInto(8); }
	void			SplitSplineInto16() { SplitSplineInto(16); }
	void			SplitSplineInto32() { SplitSplineInto(32); }
	void			SplitSplineInto64() { SplitSplineInto(64); }
	void			SplitSplineInto(int32 num);

	void			JoinSplines();
	void			JoinAllSplines();

	void			SnapPointToClosestNeighbor(FrdSplineComponentVisualizer* vis);

	float			BuildBPSplineSection(AActor* actor,float startOffset,int32& index);

	void			ConvertLandscapeSplines();
	void			CreateSplinesFromChains(UClass* splineClass,TArray<FSplineChain>& chains);
	void			Convert_NonWP_LandscapeSplines(UClass* splineClass);
	void			Convert_WP_LandscapeSplines(UClass* splineClass);
	void			TransformLandscapeSplines();

	void			SubdivideSplines2() { SubdivideSplines(2); } 
	void			SubdivideSplines3() { SubdivideSplines(3); } 
	void			SubdivideSplines4() { SubdivideSplines(4); } 
	void			SubdivideSplines5() { SubdivideSplines(5); } 
	void			SubdivideSplines6() { SubdivideSplines(6); } 
	void			SubdivideSplines7() { SubdivideSplines(7); } 
	void			SubdivideSplines8() { SubdivideSplines(8); } 
	void			SubdivideSplines9() { SubdivideSplines(9); } 
	void			SubdivideSplines10() { SubdivideSplines(10); } 
	void			SubdivideSplines(int32 dv);

	void			UnSubdivideSplines2() { UnSubdivideSplines(2); } 
	void			UnSubdivideSplines3() { UnSubdivideSplines(3); } 
	void			UnSubdivideSplines4() { UnSubdivideSplines(4); } 
	void			UnSubdivideSplines5() { UnSubdivideSplines(5); } 
	void			UnSubdivideSplines6() { UnSubdivideSplines(6); } 
	void			UnSubdivideSplines7() { UnSubdivideSplines(7); } 
	void			UnSubdivideSplines8() { UnSubdivideSplines(8); } 
	void			UnSubdivideSplines9() { UnSubdivideSplines(9); } 
	void			UnSubdivideSplines10() { UnSubdivideSplines(10); } 
	void			UnSubdivideSplines(int32 dv);

	TArray<USplineComponent*>					splineList;
	TArray<AActor*>								actorList;
	TArray<AActor*>								dirtyActorList;
	TSharedPtr<class FUICommandList>			rdLevelEditorCommands;
	TSharedPtr<FrdSplineComponentVisualizer>	rdSplineVis;

	bool										ignoreMove,doDraw;

	TObjectPtr<UrdSplineToolsOptions>			rdSplineToolsoptions;
	FString										configIni;

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

class FSimpleRootObjectCustomization : public IDetailRootObjectCustomization {
public:
#if ENGINE_MAJOR_VERSION<5 && ENGINE_MINOR_VERSION<25
	virtual TSharedPtr<SWidget> CustomizeObjectHeader(const UObject* InRootObject) override;
	virtual bool IsObjectVisible(const UObject* InRootObject) const override { return true; }
	virtual bool ShouldDisplayHeader(const UObject* InRootObject) const override { return false; }
#else
	virtual TSharedPtr<SWidget> CustomizeObjectHeader(const FDetailsObjectSet& InRootObjectSet) override;
	virtual bool AreObjectsVisible(const FDetailsObjectSet& InRootObjectSet) const override { return true; }
	virtual bool ShouldDisplayHeader(const FDetailsObjectSet& InRootObjectSet) const override { return false; }
#endif	
};

