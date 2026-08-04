//
// rdSpline_SetHeight.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 27th August 2023
// Last Modified: 13th November 2024

#include "rdSplineTools.h"
#include "Misc/OutputDeviceNull.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightToFirst - with level tangents
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SetPointsHeightToFirst(FrdSplineComponentVisualizer* vis) {

	if(!vis || actorList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SetPointsHeightToFirst"));
	UE_LOG(LogTemp,Display,TEXT("SetPointsHeightToFirst()"));

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	USplineComponent* spline=splineList[0];

	TArray<int32> keys=vis->GetSelectedKeys().Array();
	int32 numSel=keys.Num();
	if(numSel<2) return;

	int32 index=keys[0];
	float zpos=spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::Local).Z;
	FVector v,t1,t2;

	for(auto key:keys) {

		index=key;
		v=spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::Local);
		v.Z=zpos;
		spline->SetLocationAtSplinePoint(index,v,ESplineCoordinateSpace::Local);
		t1=spline->GetArriveTangentAtSplinePoint(index,ESplineCoordinateSpace::Local);
		t2=spline->GetLeaveTangentAtSplinePoint(index,ESplineCoordinateSpace::Local);
		t1.Z=zpos;
		t2.Z=zpos;
		spline->SetTangentsAtSplinePoint(index,t1,t2,ESplineCoordinateSpace::Local);
	}
	dirtyActorList.AddUnique(actor);
	doDraw=true;
}

//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightToLast - with level tangents
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SetPointsHeightToLast(FrdSplineComponentVisualizer* vis) {

	if(!vis || actorList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SetPointsHeightToLast"));
	UE_LOG(LogTemp,Display,TEXT("SetPointsHeightToLast()"));

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	USplineComponent* spline=splineList[0];

	TArray<int32> keys=vis->GetSelectedKeys().Array();
	int32 numSel=keys.Num();
	if(numSel<2) return;

	int32 index=keys[keys.Num()-1];
	float zpos=spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::Local).Z;
	FVector v,t1,t2;

	for(auto key:keys) {

		v=spline->GetLocationAtSplinePoint(key,ESplineCoordinateSpace::Local);
		v.Z=zpos;
		spline->SetLocationAtSplinePoint(key,v,ESplineCoordinateSpace::Local);
		t1=spline->GetArriveTangentAtSplinePoint(key,ESplineCoordinateSpace::Local);
		t2=spline->GetLeaveTangentAtSplinePoint(key,ESplineCoordinateSpace::Local);
		t1.Z=0;//zpos;
		t2.Z=0;//zpos;
		spline->SetTangentsAtSplinePoint(key,t1,t2,ESplineCoordinateSpace::Local);
	}
	dirtyActorList.AddUnique(actor);
	doDraw=true;
}

//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightToLandscape - with offset
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SetPointsHeightToLandscape(FrdSplineComponentVisualizer* vis,const FVector offset) {


	if(!vis || actorList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SetPointsHeightToLast"));
	UE_LOG(LogTemp,Display,TEXT("SetPointsHeightToLast()"));

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	USplineComponent* spline=splineList[0];

	TArray<int32> keys=vis->GetSelectedKeys().Array();
	int32 numSel=keys.Num();
	if(numSel<1) return;

	int32 index=keys[keys.Num()-1];
	float zpos=spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::Local).Z;
	FVector v;

	FHitResult hit;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(actor);

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();

	for(auto key:keys) {

		v=spline->GetLocationAtSplinePoint(key,ESplineCoordinateSpace::World);
		float z2=v.Z-50000;
		float z1=v.Z+50000;
		bool ret=World->LineTraceSingleByChannel(hit,FVector(v.X,v.Y,z1),FVector(v.X,v.Y,z2),ECollisionChannel::ECC_WorldStatic,queryParams); // ECC_Visibility
		if(ret && hit.bBlockingHit) {
			v.Z=hit.ImpactPoint.Z;
			spline->SetLocationAtSplinePoint(key,v,ESplineCoordinateSpace::World);
		}
	}

	dirtyActorList.AddUnique(actor);
	doDraw=true;
}

//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightAdjacent - with level tangents
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SetPointsHeightAdjacent(FrdSplineComponentVisualizer* vis,int32 num) {

	if(!vis || actorList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SetPointsHeightAdjacent"));
	UE_LOG(LogTemp,Display,TEXT("SetPointsAdjacent()"));

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	USplineComponent* spline=splineList[0];

	TArray<int32> keys=vis->GetSelectedKeys().Array();
	int32 numSel=keys.Num();
	if(numSel!=1) return;

	int32 index=keys[0];
	if(index<num) {
		UE_LOG(LogTemp,Display,TEXT("SetPointsHeightAdjacent() - spline point limits breached"));
		return;
	}

	float zpos=spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::Local).Z;
	FVector v,t1,t2;

	for(int32 i=(index-num);i<=(index+num);i++) {

		v=spline->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::Local);
		v.Z=zpos;
		spline->SetLocationAtSplinePoint(i,v,ESplineCoordinateSpace::Local);
		t1=spline->GetArriveTangentAtSplinePoint(i,ESplineCoordinateSpace::Local);
		t2=spline->GetLeaveTangentAtSplinePoint(i,ESplineCoordinateSpace::Local);
		t1.Z=0;
		t2.Z=0;
		spline->SetTangentsAtSplinePoint(i,t1,t2,ESplineCoordinateSpace::Local);
	}

	dirtyActorList.AddUnique(actor);
	doDraw=true;
}

//----------------------------------------------------------------------------------------------------------------
// CreateLeftIntersection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::CreateLeftIntersection(FrdSplineComponentVisualizer* vis) {

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	if(!actor) return;

	if(FSlateApplication::Get().GetModifierKeys().IsShiftDown()) {
		SetPointsHeightAdjacent(vis,1);
	}

	UFunction* func=actor->FindFunction(FName(TEXT("rdCreateIntersection")));
	if(func) {

		struct cnvParms {
			int32	splineIndex;
			bool	onLeft;
		};

		cnvParms parms;
		TArray<int32> keys=vis->GetSelectedKeys().Array();
		parms.splineIndex=keys[0];
		parms.onLeft=true;
		actor->ProcessEvent(func,(void*)&parms);
	}
}

//----------------------------------------------------------------------------------------------------------------
// ClearLeftIntersection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::ClearLeftIntersection(FrdSplineComponentVisualizer* vis) {

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	if(!actor) return;

	UFunction* func=actor->FindFunction(FName(TEXT("rdClearIntersection")));
	if(func) {

		struct cnvParms {
			int32	splineIndex;
			bool	onLeft;
		};

		cnvParms parms;
		TArray<int32> keys=vis->GetSelectedKeys().Array();
		parms.splineIndex=keys[0];
		parms.onLeft=true;
		actor->ProcessEvent(func,(void*)&parms);
	}
}

//----------------------------------------------------------------------------------------------------------------
// CreateRightIntersection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::CreateRightIntersection(FrdSplineComponentVisualizer* vis) {

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	if(!actor) return;

	if(FSlateApplication::Get().GetModifierKeys().IsShiftDown()) {
		SetPointsHeightAdjacent(vis,1);
	}

	UFunction* func=actor->FindFunction(FName(TEXT("rdCreateIntersection")));
	if(func) {

		struct cnvParms {
			int32	splineIndex;
			bool	onLeft;
		};

		cnvParms parms;
		TArray<int32> keys=vis->GetSelectedKeys().Array();
		parms.splineIndex=keys[0];
		parms.onLeft=false;
		actor->ProcessEvent(func,(void*)&parms);
	}
}

//----------------------------------------------------------------------------------------------------------------
// ClearRightIntersection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::ClearRightIntersection(FrdSplineComponentVisualizer* vis) {

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	if(!actor) return;

	UFunction* func=actor->FindFunction(FName(TEXT("rdClearIntersection")));
	if(func) {

		struct cnvParms {
			int32	splineIndex;
			bool	onLeft;
		};

		cnvParms parms;
		TArray<int32> keys=vis->GetSelectedKeys().Array();
		parms.splineIndex=keys[0];
		parms.onLeft=false;
		actor->ProcessEvent(func,(void*)&parms);
	}
}

//----------------------------------------------------------------------------------------------------------------
// MoveAllSplinePointsToGround
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::MoveAllSplinePointsToGround() {

	FHitResult hit;
	FCollisionQueryParams queryParams;
	for(auto actor:actorList) {
		queryParams.AddIgnoredActor(actor);
	}

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();
	FVector v;

	int32 ind=0;
	for(auto actor:actorList) {

		USplineComponent* spline=splineList[ind++];
		int32 numPoints=spline->GetNumberOfSplinePoints();
		for(int32 i=0;i<numPoints;i++) {

			v=spline->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::World);
			float z2=v.Z-50000;
			float z1=v.Z+50000;
			bool ret=World->LineTraceSingleByChannel(hit,FVector(v.X,v.Y,z1),FVector(v.X,v.Y,z2),ECollisionChannel::ECC_WorldStatic,queryParams); // ECC_Visibility
			if(ret && hit.bBlockingHit) {
				v.Z=hit.ImpactPoint.Z;
				spline->SetLocationAtSplinePoint(i,v,ESplineCoordinateSpace::World);
			}
		}
		UFunction* func=actor->FindFunction(FName(TEXT("rdBuildRoad")));
		if(func) {
			actor->ProcessEvent(func,nullptr);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
	