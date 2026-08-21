//
// rdSpline_Subdivide.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 27th December 2023
// Last Modified: 27th December 2023

#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// SubdivideSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SubdivideSplines(int32 dv) {

	if(actorList.Num()==0 || splineList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SubdivideSplines"));
	UE_LOG(LogTemp,Display,TEXT("SubdivideSplines()"));

	for(int32 i=0;i<actorList.Num();i++) {

		AActor* actor=actorList[i];
		USplineComponent* spline=splineList[i];

		int32 numPoints=spline->GetNumberOfSplinePoints();
		TArray<FVector> locs;
		for(int32 j=0;j<numPoints-1;j++) {

			float dist1=spline->GetDistanceAlongSplineAtSplinePoint(j);
			float dist2=spline->GetDistanceAlongSplineAtSplinePoint(j+1);
			float secDiv=(dist2-dist1)/(float)dv;
			float offset=secDiv;

			for(int32 s=0;s<dv-1;s++) {
				locs.Add(spline->GetLocationAtDistanceAlongSpline(dist1+offset,ESplineCoordinateSpace::Local));
				offset+=secDiv;
			}
		}
		int32 ind=0;
		for(int32 j=0;j<numPoints-1;j++) {
			for(int32 s=0;s<dv-1;s++) {
				spline->AddSplinePointAtIndex(locs[ind++],(j*dv)+s+1,ESplineCoordinateSpace::Local,true);
			}
		}
	}

}

//----------------------------------------------------------------------------------------------------------------
// UnSubdivideSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::UnSubdivideSplines(int32 dv) {

	if(actorList.Num()==0 || splineList.Num()==0) return;

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","UnSubdivideSplines"));
	UE_LOG(LogTemp,Display,TEXT("UnSubdivideSplines()"));

	for(int32 i=0;i<actorList.Num();i++) {

		AActor* actor=actorList[i];
		USplineComponent* spline=splineList[i];

		int32 numPoints=spline->GetNumberOfSplinePoints();
		TArray<int32> indexes;
		if(dv>=numPoints) {
			for(int32 j=1;j<numPoints-2;j++) {
				indexes.Add(j);
			}
		} else {
			for(int32 j=1;j<numPoints-1;j+=dv) {
				for(int32 s=0;s<dv-1;s++) {
					indexes.Add(j+s);
				}
			}
		}
		for(int32 j=indexes.Num()-1;j>=0;j--) {
			spline->RemoveSplinePoint(indexes[j],false);
		}
		spline->UpdateSpline();
	}
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	