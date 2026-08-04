//
// rdSpline_Join.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 13th November 2024

#include "Kismet/GameplayStatics.h"
#include "Landscape.h"
#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// JoinSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::JoinSplines() {

	// Add Undo point
	//const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","JoinSplines"));
	//UE_LOG(LogTemp,Display,TEXT("JoinSplines()"));

	JoinAllSplines();
}

//----------------------------------------------------------------------------------------------------------------
// SnapPointToClosestNeighbor
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::SnapPointToClosestNeighbor(FrdSplineComponentVisualizer* vis) {

	if(!vis || actorList.Num()==0) return;

	AActor* actor=actorList[0];// only one actor and spline, set from the SplineVisualizer
	USplineComponent* spline=splineList[0];

	TArray<int32> keys=vis->GetSelectedKeys().Array();
	int32 numSel=keys.Num();
	if(numSel>1) return;
	int32 index=keys[0];

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","SnapPointToClosestNeighbor"));
	UE_LOG(LogTemp,Display,TEXT("SnapPointToClosestNeighbor()"));

	FVector loc=spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::World);
	float closestDistance=9999999.0f;
	FVector newLoc=loc,t1,t2;
	bool firstPoint=index==0,lastPoint=index==spline->GetNumberOfSplinePoints()-1,oFirstPoint=false,oLastPoint=false;

	TArray<AActor*> otherBPs;
	TArray<UActorComponent*> splineComps;
	UGameplayStatics::GetAllActorsOfClass(actor->GetWorld(),actor->GetClass(),otherBPs);

	for(auto otherActor:otherBPs) {
		if(otherActor==actor) continue;

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>24
		otherActor->GetComponents(USplineComponent::StaticClass(),splineComps);
#else
		splineComps=otherActor->GetComponentsByClass(USplineComponent::StaticClass());
#endif
		for(auto c:splineComps) {
			USplineComponent* sc=(USplineComponent*)c;
			if(sc) {
				int32 key=(int32)sc->FindInputKeyClosestToWorldLocation(loc);
				FVector sloc=sc->GetLocationAtSplinePoint(key,ESplineCoordinateSpace::World);
				float dist=FVector::Distance(loc,sloc);
				if(dist<closestDistance) {
					newLoc=sloc;
					oFirstPoint=key==0;
					oLastPoint=key==sc->GetNumberOfSplinePoints()-1;
					t1=sc->GetArriveTangentAtSplinePoint(key,ESplineCoordinateSpace::Local);
					t2=sc->GetLeaveTangentAtSplinePoint(key,ESplineCoordinateSpace::Local);
					closestDistance=dist;
				}
			}
		}
	}
	if(closestDistance<5000.0f) {
		spline->SetLocationAtSplinePoint(index,newLoc,ESplineCoordinateSpace::World);

		if(firstPoint) {

			spline->SetTangentsAtSplinePoint(index,t1,t2,ESplineCoordinateSpace::Local);

		} else if(lastPoint) {

			spline->SetTangentsAtSplinePoint(index,t1,t2,ESplineCoordinateSpace::Local);

		}

		UFunction* func=actor->FindFunction(FName(TEXT("rdBuildRoad")));
		if(func) {
			actor->ProcessEvent(func,nullptr);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	