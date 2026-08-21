//
// rdSpline_JoinAll.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 7th August 2023

#include "rdSplineTools.h"
#include "Misc/OutputDeviceNull.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// JoinAllSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::JoinAllSplines() {

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","JoinAllSplines"));
	UE_LOG(LogTemp,Display,TEXT("JoinAllSplines()"));

	// remove actors that are sub splines attached to parents in the selected list
	TArray<AActor*> aList;
	for(auto actor:actorList) {
		for(auto actor2:actorList) {
			if(actor2->GetAttachParentActor()==actor) {
				aList.Add(actor2);
			}
		}
	}
	for(auto actor:aList) {
		actorList.Remove(actor);
	}

	// Now merge attach actors subsplines to their parents (only one level deep to preserve sub editing)
	for(auto actor:actorList) {
		MergeActorsSubSplines(actor);
		dirtyActorList.AddUnique(actor);
	}

	doDraw=true;
}

//----------------------------------------------------------------------------------------------------------------
// MergeActorsSubSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::MergeActorsSubSplines(AActor* actor) {

	TArray<AActor*> aList;
	actor->GetAttachedActors(aList,true);

	TArray<AActor*> sortedList;
	sortedList.SetNum(aList.Num());
	for(auto subActor:aList) {
		if(subActor->Tags.Contains(FName(TEXT("SubSpline")))) {
			int32 ind=GetSubSplineIndex(subActor);
			if(ind>=0) sortedList[ind]=subActor;
		}
	}

	USplineComponent* spline=GetSpline(actor);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	float key=spline->GetInputKeyValueAtDistanceAlongSpline(spline->GetDistanceAlongSplineAtSplinePoint(spline->GetNumberOfSplinePoints()-1));
#else
	float key=spline->SplineCurves.ReparamTable.Eval((spline->GetDistanceAlongSplineAtSplinePoint(spline->GetNumberOfSplinePoints()-1)),0.0f);
#endif
	for(auto a:sortedList) {
		if(IsValid(a)) {
			MergeActorsSubSplines(a);

			USplineComponent* sub=GetSpline(a);

			UFunction* func=actor->FindFunction(FName(TEXT("rdMergeSubSplineData")));
			if(func) {

				struct cnvParms {
					AActor* subspline;
					int32	startIndex;
					int32	endIndex;
				};

				cnvParms parms;
				parms.subspline=a;
				parms.startIndex=(int32)key;
				parms.endIndex=(int32)key+sub->GetNumberOfSplinePoints()-2;
				actor->ProcessEvent(func,(void*)&parms);
			}

			for(int32 i=1;i<sub->GetNumberOfSplinePoints();i++) {

				FSplinePoint pt=FillSplinePoint(sub,i,key);
				spline->AddPoint(pt,true);
			}
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
			key+=sub->GetInputKeyValueAtDistanceAlongSpline(sub->GetDistanceAlongSplineAtSplinePoint(sub->GetNumberOfSplinePoints()-1));
#else
			key+=sub->SplineCurves.ReparamTable.Eval((sub->GetDistanceAlongSplineAtSplinePoint(sub->GetNumberOfSplinePoints()-1)),0.0f);
#endif

			a->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			a->Destroy();
		}
	}

	// Reset the main spline
	UFunction* func=actor->FindFunction(FName(TEXT("rdMergeSubSplineData")));
	if(func) {

		struct cnvParms {
			AActor* subspline;
			int32	startIndex;
			int32	endIndex;
		};

		cnvParms parms;
		parms.subspline=nullptr;
		parms.startIndex=0;
		parms.endIndex=0;
		actor->ProcessEvent(func,(void*)&parms);
	}
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	