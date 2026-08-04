//
// rdSpline_NewSplineHere.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 19th May 2023

#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// NewSplineHere
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::NewSplineHere(FrdSplineComponentVisualizer* vis) {

	if(!vis || actorList.Num()<1 || splineList.Num()<1) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","NewSplineHere"));
	UE_LOG(LogTemp,Display,TEXT("NewSplineHere()"));

	AActor* actor=actorList[0];
	USplineComponent* spline=splineList[0];

	int32 cnt=1;
	TArray<AActor*> aList;
	actor->GetAttachedActors(aList,true);
	for(auto a:aList) {
		if(a->Tags.Contains("NewSpline")) {
			int32 ind=GetNewSplineIndex(a);
			if(ind>cnt) cnt=ind;
		}
	}
	
	int32 numSel=vis->GetSelectedKeys().Num();
	TArray<int32> keys=vis->GetSelectedKeys().Array();
	for(auto key:keys) {

		FTransform tran=spline->GetTransformAtSplinePoint(key,ESplineCoordinateSpace::Local,false);
		tran.Accumulate(FTransform(FRotator(0,90,0),FVector(0,0,0),FVector(1,1,1)));
		AActor* newActor=NewActor(actor,tran);
		newActor->SetActorLabel(*FString::Printf(TEXT("NewSpline%d"),cnt));
		newActor->Tags.Add(*FString::Printf(TEXT("newSpline%d"),cnt));

		USplineComponent* nspline=GetSpline(newActor);

		nspline->ClearSplinePoints();
		FSplinePoint pt=FSplinePoint(	0.0f,
										FVector(0,0,0),
										FVector(0,0,0),
										FVector(0,0,0),
										FRotator(0,0,0),
										FVector(1,1,1),
										ESplinePointType::Curve
										);
		nspline->AddPoint(pt,false);
		pt.InputKey=1.0f;
		pt.Position.X=1000.0f;
		nspline->AddPoint(pt,true);
		nspline->bSplineHasBeenEdited=true;

		cnt++;
	}
	
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	