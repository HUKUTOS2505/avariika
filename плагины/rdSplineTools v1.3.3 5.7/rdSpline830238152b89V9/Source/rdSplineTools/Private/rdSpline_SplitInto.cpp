//
// rdSpline_SplitInto.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 29th May 2023

#include "Misc/OutputDeviceNull.h"
#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// SplitSplineInto
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SplitSplineInto(int32 num) {

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SplitSplineInto"));
	UE_LOG(LogTemp,Display,TEXT("SplitSplineInto()"));

	for(auto actor:actorList) {

		actor->Modify(true);

		FOutputDeviceNull ar;
		actor->CallFunctionByNameWithArguments(TEXT("rdClearSpline"),ar,NULL,true);

		int meshIndex=0;
		float meshOffset=BuildBPSplineSection(actor,0.0f,meshIndex);

		TArray<UActorComponent*> comps1,comps2;
		actor->GetComponents(USplineComponent::StaticClass(),comps1);
		for(auto comp:comps1) {

			USplineComponent* spline1=Cast<USplineComponent>(comp);
			if(spline1) {
				int32 numPoints=spline1->GetNumberOfSplinePoints();
				if(num<=(numPoints/3)) { // only split splines with num*3 points or greater

					int32 numSplits=num;
					int32 pntsPerSplit=numPoints/numSplits;
					int32 splitInd=pntsPerSplit;
					for(int32 i=0;i<numSplits-1;i++) {

						AActor* newActor=DuplicateActor(actor);
						newActor->Modify(true);

						FString nm=FString::Printf(TEXT("split%d"),i);
						FString nm2=FString::Printf(TEXT("SubSpline%d"),i);
						newActor->Tags.Add(FName(*nm));
						newActor->SetActorLabel(*nm2);

						USplineComponent* spline2=nullptr;
						newActor->GetComponents(USplineComponent::StaticClass(),comps2);
						for(auto comp2:comps2) {
							if(comp2->GetName()==comp->GetName()) {
								spline2=Cast<USplineComponent>(comp2);
								break;
							}
						}
						if(spline2) {

							spline2->ClearSplinePoints(false);
							copySpline(spline1,spline2,splitInd,(i==(numSplits-2))?spline1->GetNumberOfSplinePoints():splitInd+pntsPerSplit+1);
							splitInd+=pntsPerSplit;
						}

						meshOffset=BuildBPSplineSection(newActor,meshOffset,meshIndex);

					}
					trimSpline(spline1,0,pntsPerSplit);
					break;
				}
			}
		}
	}
	doDraw=true;
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE	