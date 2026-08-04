//
// rdSpline_LandscapeSplinesTransforms.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 16th November 2024
// Last Modified: 29th March 2024

#include "rdSplineTools.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#include "LandscapeSplineSegment.h"
#include "LandscapeSplineActor.h"
#include "LandscapeSplineControlPoint.h"
#endif
#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// TransformLandscapeSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::TransformLandscapeSplines() {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0

	if(!ShowLandscapeSplineTransformsSettings()) {
		return;
	}

	FVector& loc=rdSplineToolsoptions->transformLocation;
	FRotator& rot=rdSplineToolsoptions->transformRotation;
	FVector& scale=rdSplineToolsoptions->transformScale;

	TArray<ULandscapeSplineControlPoint*> cpoints;

	TArray<ULandscapeSplineSegment*> segments;
	for(TObjectIterator<ULandscapeSplineSegment> it;it;++it) {
		ULandscapeSplineSegment* seg=*it;
		for(auto& c:seg->Connections) {
			ULandscapeSplineControlPoint* cp=c.ControlPoint;
			if(cpoints.Find(cp)!=INDEX_NONE) continue;
			cp->Location+=loc;
			cpoints.Add(cp);
			cp->UpdateSplinePoints();
		}
		seg->UpdateSplinePoints(true,true);
	}
#endif
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	