//
// rdSpline_SplitSplineSection.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 9th May 2023
// Last Modified: 19th May 2023

#include "Misc/OutputDeviceNull.h"
#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// SplitSplineSection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SplitSplineSection(FrdSplineComponentVisualizer* vis) {

	if(!vis || actorList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SplitSplineSection"));
	UE_LOG(LogTemp,Display,TEXT("SplitSplineSection()"));

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer

	FString str=FString::Printf(TEXT("rdClearSpline"));
	FOutputDeviceNull ar;
	actor->CallFunctionByNameWithArguments(*str,ar,NULL,true);

	if(SplineActorHasSplits(actor)) {
		InsertAttachActorIndexes(actor,2); // insert 2 actors at the begining
	}

	AActor* newActor1=DuplicateActor(actor);
	newActor1->Tags.Add(FName(TEXT("split0")));
	newActor1->SetActorLabel(TEXT("SubSpline0"));

	AActor* newActor2=DuplicateActor(actor);
	newActor2->Tags.Add(FName(TEXT("split1")));
	newActor2->SetActorLabel(TEXT("SubSpline1"));

	TArray<UActorComponent*> comps;
	actor->GetComponents(USplineComponent::StaticClass(),comps);
	for(auto comp:comps) {
		USplineComponent* spline1=Cast<USplineComponent>(comp);
		USplineComponent* spline2=GetSpline(newActor1);
		USplineComponent* spline3=GetSpline(newActor2);
		if(spline1 && spline2 && spline3) {

			int32 numSel=vis->GetSelectedKeys().Num();
			TArray<int32> keys=vis->GetSelectedKeys().Array();
			if(numSel==2) {

				int32 index1=keys[0];
				int32 index2=keys[1];
				if(index1>index2) {
					index1=keys[1];
					index2=keys[0];
				}

				spline2->ClearSplinePoints(false);
				spline3->ClearSplinePoints(false);

				copySpline(spline1,spline2,index1,index2+1);
				copySpline(spline1,spline3,index2,spline1->GetNumberOfSplinePoints());
				trimSpline(spline1,0,index1);
				break;
			}
		}
	}

	dirtyActorList.AddUnique(newActor1);
	dirtyActorList.AddUnique(newActor2);
	dirtyActorList.AddUnique(actor);
	doDraw=true;
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	