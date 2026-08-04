//
// rdSpline_StraightenPoints.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 8th May 2023

#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// StraightenPoints
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::StraightenPoints(FrdSplineComponentVisualizer* vis) {

	if(!vis || actorList.Num()==0 || splineList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","StraightenPoints"));
	UE_LOG(LogTemp,Display,TEXT("StraightenPoints()"));

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	USplineComponent* spline=splineList[0];

	int32 numSel=vis->GetSelectedKeys().Num();
	TArray<int32> keys=vis->GetSelectedKeys().Array();
	if(numSel>2) {

		int32 index1=keys[0];
		int32 index2=keys[keys.Num()-1];
		if(index1>index2) {
			index1=keys[keys.Num()-1];
			index2=keys[0];
		}

		FVector startLoc=spline->GetLocationAtSplinePoint(index1,ESplineCoordinateSpace::Local);
		FVector endLoc=spline->GetLocationAtSplinePoint(index2,ESplineCoordinateSpace::Local);
		float numSteps=(index2-index1)+1;
		FVector locInc=(endLoc-startLoc)/numSteps;
		FVector loc=startLoc;

		for(int32 i=index1;i<index2;i++) {
			spline->SetLocationAtSplinePoint(i,loc,ESplineCoordinateSpace::Local,false);
#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
			spline->SetRotationAtSplinePoint(i,FRotator(0,0,0),ESplineCoordinateSpace::Local,false);
#else
			if(spline->SplineCurves.Rotation.Points.IsValidIndex(i)) {
				const FQuat quat=FRotator(0,0,0).Quaternion();
				FVector upVec=quat.GetUpVector();
				spline->SetUpVectorAtSplinePoint(i,upVec,ESplineCoordinateSpace::Local,false);
				FVector dir=quat.GetForwardVector();
				spline->SetTangentAtSplinePoint(i,dir,ESplineCoordinateSpace::Local,false);
			}
#endif
			loc+=locInc;
		}
		spline->SetTangentsAtSplinePoint(index1,FVector(0,0,0),FVector(0,0,0),ESplineCoordinateSpace::Local,false);
		spline->SetTangentsAtSplinePoint(index2,FVector(0,0,0),FVector(0,0,0),ESplineCoordinateSpace::Local,false);
		spline->UpdateSpline();

		//FString str=FString::Printf(TEXT("rdBuildSpline"));
		//FOutputDeviceNull ar;
		//actor->CallFunctionByNameWithArguments(*str,ar,NULL,true);
		dirtyActorList.AddUnique(actor);
		doDraw=true;
	}
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	