//
// rdSpline_Split.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 11th May 2023
//

#include "Misc/OutputDeviceNull.h"
#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// SplitSpline
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SplitSpline(FrdSplineComponentVisualizer* vis,bool round) {

	if(!vis || actorList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SplitSpline"));
	UE_LOG(LogTemp,Display,TEXT("SplitSpline()"));

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer

	FOutputDeviceNull ar;
	actor->CallFunctionByNameWithArguments(TEXT("rdClearSpline"),ar,NULL,true);

	int meshIndex=0;
	float meshOffset=BuildBPSplineSection(actor,0.0f,meshIndex);

	if(SplineActorHasSplits(actor)) {
		InsertAttachActorIndexes(actor,1); // insert 1 actor at the begining
	}

	AActor* newActor=DuplicateActor(actor);
	newActor->Tags.Add(FName(TEXT("split0")));
	newActor->SetActorLabel(TEXT("SubSpline0"));

	TArray<UActorComponent*> comps;
	actor->GetComponents(USplineComponent::StaticClass(),comps);
	for(auto comp:comps) {
		USplineComponent* spline1=Cast<USplineComponent>(comp);
		USplineComponent* spline2=GetSpline(newActor);
		if(spline1 && spline2) {

			int32 numSel=vis->GetSelectedKeys().Num();
			TArray<int32> keys=vis->GetSelectedKeys().Array();
			if(numSel==1) {

				int32 index=keys[0];

				spline2->ClearSplinePoints(false);

				copySpline(spline1,spline2,index,spline1->GetNumberOfSplinePoints());
				trimSpline(spline1,0,index);
				break;
			}
		}
	}

	meshOffset=BuildBPSplineSection(newActor,meshOffset,meshIndex);

	doDraw=true;
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE	