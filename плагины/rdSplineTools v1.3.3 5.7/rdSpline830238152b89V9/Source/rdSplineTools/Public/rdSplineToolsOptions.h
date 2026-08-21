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
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "SClassViewer.h"

#include "rdSplineToolsOptions.generated.h"

class FrdSplineToolsModule;

class FClassFilter : public IClassViewerFilter {
public:
	FClassFilter(UClass* addClass)	{ classList.Add(addClass); }
	TSet<const UClass*> classList;
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& initOptions,const UClass* tstClass,TSharedRef<FClassViewerFilterFuncs> filterFuncs) override { 
		return filterFuncs->IfInChildOfClassesSet(classList,tstClass)==EFilterReturn::Passed;
	}
	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& initOptions,const TSharedRef<const IUnloadedBlueprintData> unloadedClassData,TSharedRef<FClassViewerFilterFuncs> filterFuncs) override { 
		return filterFuncs->IfInChildOfClassesSet(classList,unloadedClassData)==EFilterReturn::Passed;
	}
};

UCLASS(ComponentWrapperClass)
class UrdSplineToolsOptions : public UObject {
	GENERATED_BODY()
public:
	UrdSplineToolsOptions() :	classType(nullptr),
								snapIntersectionRadius(65.0f),visIntersectionRadius(3000.0f),visMeshIntersection(FSoftObjectPath(TEXT("/rdSplineTools/Meshes/SM_SnapIntersectionIcon.SM_SnapIntersectionIcon"))),visMeshIntersectionScale(1.0f),visIntersectionCol1(1,1,.773,0),visIntersectionCol2(10,200,10,1),visIntersectionSnappedOpacity(0.1),
								snapSocketRadius(45.0f),visSocketRadius(3000.0f),visMeshSocket(FSoftObjectPath(TEXT("/rdSplineTools/Meshes/SM_SnapSocketIcon.SM_SnapSocketIcon"))),visMeshSocketScale(1.0f),visSocketCol1(1,1,.773,0),visSocketCol2(200,200,10,1),visSocketSnappedOpacity(0.1),
								snapSplinePointRadius(65.0f),visSplinePointRadius(3000.0f),visMeshSplinePoint(FSoftObjectPath(TEXT("/rdSplineTools/Meshes/SM_SnapSplinePointIcon.SM_SnapSplinePointIcon"))),visMeshSplinePointScale(1.0f),visSplinePointCol1(1,1,.773,0),visSplinePointCol2(10,200,200,1),visSplinePointSnappedOpacity(0.1),
								doFullDraw(true),fullDrawTime(2.0f),
								splineModule(nullptr) {};

	FString						ToString();
	bool						FromString(const FString& str);

	UClass*						classType=nullptr;
	FVector						transformLocation=FVector(0,0,0);
	FRotator					transformRotation=FRotator(0,0,0);
	FVector						transformScale=FVector(1,1,1);

	rdSnapType					snapIntersectionsType=rdSnapType::RDSNAP_ALWAYS;
	FString						snapIntersectionsHotkey;
	bool						snapIntersectionsTangent=true;
	float						snapIntersectionRadius=65.0f;
	float						visIntersectionRadius=800.0f;
	TSoftObjectPtr<UStaticMesh>	visMeshIntersection=nullptr;
	float						visMeshIntersectionScale=100.0f;
	FLinearColor				visIntersectionCol1=FLinearColor(1,0,0,0.5);
	FLinearColor				visIntersectionCol2=FLinearColor(0,1,0,1);
	float						visIntersectionSnappedOpacity=0.1f;

	rdSnapType					snapSocketsType=rdSnapType::RDSNAP_ALWAYS;
	FString						snapSocketsHotkey;
	bool						snapSocketsTangent=true;
	float						snapSocketRadius=65.0f;
	float						visSocketRadius=800.0f;
	TSoftObjectPtr<UStaticMesh>	visMeshSocket=nullptr;
	float						visMeshSocketScale=100.0f;
	FLinearColor				visSocketCol1=FLinearColor(1,0,0,0.5);
	FLinearColor				visSocketCol2=FLinearColor(0,1,0,1);
	float						visSocketSnappedOpacity=0.1f;

	rdSnapType					snapSplinePointsType=rdSnapType::RDSNAP_ALWAYS;
	FString						snapSplinePointsHotkey;
	bool						snapSplinePointsTangent=true;
	float						snapSplinePointRadius=65.0f;
	float						visSplinePointRadius=800.0f;
	TSoftObjectPtr<UStaticMesh>	visMeshSplinePoint=nullptr;
	float						visMeshSplinePointScale=100.0f;
	FLinearColor				visSplinePointCol1=FLinearColor(1,0,0,0.5);
	FLinearColor				visSplinePointCol2=FLinearColor(0,1,0,1);
	float						visSplinePointSnappedOpacity=0.1f;

	bool						doFullDraw=false;
	float						fullDrawTime=2.0f;

	FKey						hotkeySnap=EKeys::W;
	FKey						hotkeySophisticatedSnap=EKeys::X;

	bool						removeLandscapeSplines=false;
	FrdSplineToolsModule*		splineModule=nullptr;
};
