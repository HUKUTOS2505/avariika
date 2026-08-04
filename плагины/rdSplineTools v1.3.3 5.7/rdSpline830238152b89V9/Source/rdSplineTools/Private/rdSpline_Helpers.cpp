//
// rdSpline_Helpers.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 9th May 2023
// Last Modified: 7th August 2023

#include "rdSplineTools.h"
#include "Misc/OutputDeviceNull.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// FillSplinePoint
//----------------------------------------------------------------------------------------------------------------
FSplinePoint FrdSplineToolsModule::FillSplinePoint(USplineComponent* spline,int32 point,float startKey) {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return FSplinePoint(startKey+spline->GetInputKeyValueAtDistanceAlongSpline(spline->GetDistanceAlongSplineAtSplinePoint(point)),
#else
	return FSplinePoint(startKey+spline->SplineCurves.Position.Points[point].InVal,
#endif
					spline->GetLocationAtSplinePoint(point,ESplineCoordinateSpace::Local),
					spline->GetArriveTangentAtSplinePoint(point,ESplineCoordinateSpace::Local),
					spline->GetLeaveTangentAtSplinePoint(point,ESplineCoordinateSpace::Local),
					spline->GetRotationAtSplinePoint(point,ESplineCoordinateSpace::Local),
					spline->GetScaleAtSplinePoint(point),
					spline->GetSplinePointType(point)
					);
}

//----------------------------------------------------------------------------------------------------------------
// copySpline
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::copySpline(USplineComponent* spline1,USplineComponent* spline2,int32 start,int32 end) {

	if(end>spline1->GetNumberOfSplinePoints()) return;

	FSplinePoint pt1=FillSplinePoint(spline1,start);
	FSplinePoint pt2=FillSplinePoint(spline1,end-1);

	for(int32 i=start;i<end;i++) {
		FSplinePoint spt=FillSplinePoint(spline1,i);
		spt.InputKey-=pt1.InputKey;
		spline2->AddPoint(spt);
	}

	spline2->UpdateSpline();

	spline2->SetTangentsAtSplinePoint(0,pt1.ArriveTangent,pt1.LeaveTangent,ESplineCoordinateSpace::Local,true);
	spline2->SetTangentsAtSplinePoint(spline2->GetNumberOfSplinePoints()-1,pt2.ArriveTangent,pt2.LeaveTangent,ESplineCoordinateSpace::Local,true);
}

//----------------------------------------------------------------------------------------------------------------
// trimSpline
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::trimSpline(USplineComponent* spline,int32 start,int32 len) {

	if(start+len>spline->GetNumberOfSplinePoints()) return;

	FSplinePoint pt1=FillSplinePoint(spline,start);

	int32 end=(len==0)?spline->GetNumberOfSplinePoints()-1:start+len;
	FSplinePoint pt2=FillSplinePoint(spline,end);

	// remove from 0 to start
	for(int32 i=0;i<start;i++) {
		spline->RemoveSplinePoint(0,false);
	}

	// remove left overs
	int32 left=(len==0)?0:spline->GetNumberOfSplinePoints()-len;
	for(int32 i=0;i<left;i++) {
		spline->RemoveSplinePoint(len+1,false);
	}

	spline->UpdateSpline();

	spline->SetTangentsAtSplinePoint(0,pt1.ArriveTangent,pt1.LeaveTangent,ESplineCoordinateSpace::Local,true);
	spline->SetTangentsAtSplinePoint(spline->GetNumberOfSplinePoints()-1,pt2.ArriveTangent,pt2.LeaveTangent,ESplineCoordinateSpace::Local,true);
}

//----------------------------------------------------------------------------------------------------------------
// GetSubSplineIndex
//----------------------------------------------------------------------------------------------------------------
int32 FrdSplineToolsModule::GetSubSplineIndex(AActor* actor) {

	int32 ind=-1;
	for(auto& ftag:actor->Tags) {
		FString tag(*ftag.ToString());
		if(tag.StartsWith(TEXT("split"))) {
			ind=FCString::Atoi(*tag.RightChop(5));
			break;
		}
	}
	return ind;
}

//----------------------------------------------------------------------------------------------------------------
// GetNewSplineIndex
//----------------------------------------------------------------------------------------------------------------
int32 FrdSplineToolsModule::GetNewSplineIndex(AActor* actor) {

	int32 ind=-1;
	for(auto& ftag:actor->Tags) {
		FString tag(*ftag.ToString());
		if(tag.StartsWith(TEXT("newSpline"))) {
			ind=FCString::Atoi(*tag.RightChop(9));
			break;
		}
	}
	return ind;
}

//----------------------------------------------------------------------------------------------------------------
// GetSpline
//----------------------------------------------------------------------------------------------------------------
USplineComponent* FrdSplineToolsModule::GetSpline(AActor* actor) {

	TArray<UActorComponent*> comps;
	actor->GetComponents(USplineComponent::StaticClass(),comps);
	for(auto comp:comps) {
		USplineComponent* sc=Cast<USplineComponent>(comp);
		if(sc) return sc; // just return the first one for now
	}
	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------
// CopySplinePoint
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::CopySplinePoint(int32 tangT,USplineComponent* s1,int32 i1,USplineComponent* s2,int32 i2,FVector& deltaTranslate,FRotator& deltaRotate,FVector& deltaScale) {

	if(tangT) { // one of the tangent points
		s2->SetTangentsAtSplinePoint(i2,s1->GetArriveTangentAtSplinePoint(i1,ESplineCoordinateSpace::Local),s1->GetTangentAtSplinePoint(i1,ESplineCoordinateSpace::Local),ESplineCoordinateSpace::Local,true);
		return;
	}

	s2->SetLocationAtSplinePoint(i2,s1->GetLocationAtSplinePoint(i1,ESplineCoordinateSpace::Local)+deltaTranslate,ESplineCoordinateSpace::Local,false);

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	s2->SetRotationAtSplinePoint(i2,s1->GetRotationAtSplinePoint(i1,ESplineCoordinateSpace::Local),ESplineCoordinateSpace::Local,false);
	s2->SetScaleAtSplinePoint(i2,s1->GetScaleAtSplinePoint(i1),false);
#else
	if(s2->SplineCurves.Rotation.Points.IsValidIndex(i2)) {
		const FQuat quat=s1->GetRotationAtSplinePoint(i1,ESplineCoordinateSpace::Local).Quaternion();
		FVector upVec=quat.GetUpVector();
		s2->SetUpVectorAtSplinePoint(i2,upVec,ESplineCoordinateSpace::Local,false);
		FVector dir=quat.GetForwardVector();
		s2->SetTangentAtSplinePoint(i2,dir,ESplineCoordinateSpace::Local,false);
	}
	if(s2->SplineCurves.Scale.Points.IsValidIndex(i2)) {
		s2->SplineCurves.Scale.Points[i2].OutVal=s1->GetScaleAtSplinePoint(i1);
	}
#endif
	if(i1==0) {
		FVector prevLoc=s2->GetLocationAtSplinePoint(i2-1,ESplineCoordinateSpace::Local);
		FVector nextLoc=s1->GetLocationAtSplinePoint(1,ESplineCoordinateSpace::Local);
		float dist1=s2->GetDistanceAlongSplineAtSplinePoint(i2)-s2->GetDistanceAlongSplineAtSplinePoint(i2-1);
		float dist2=s1->GetDistanceAlongSplineAtSplinePoint(1);
		FVector inc=(nextLoc-prevLoc)/(dist1+dist2);
		FVector tangent=(inc*dist1*0.5f);
		s1->SetTangentsAtSplinePoint(i1,-tangent,tangent,ESplineCoordinateSpace::Local,true);
		s2->SetTangentsAtSplinePoint(i2,tangent,-tangent,ESplineCoordinateSpace::Local,true);
	} else {
		FVector prevLoc=s1->GetLocationAtSplinePoint(i1-1,ESplineCoordinateSpace::Local);
		FVector nextLoc=s2->GetLocationAtSplinePoint(1,ESplineCoordinateSpace::Local);
		float dist1=s1->GetDistanceAlongSplineAtSplinePoint(i1)-s1->GetDistanceAlongSplineAtSplinePoint(i1-1);
		float dist2=s2->GetDistanceAlongSplineAtSplinePoint(1);
		FVector inc=(nextLoc-prevLoc)/(dist1+dist2);
		FVector tangent=(inc*dist1*0.5f);
		s1->SetTangentsAtSplinePoint(i1,tangent,-tangent,ESplineCoordinateSpace::Local,true);
		s2->SetTangentsAtSplinePoint(i2,-tangent,tangent,ESplineCoordinateSpace::Local,true);
	}
}

//----------------------------------------------------------------------------------------------------------------
// SplineActorHasSplits
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineToolsModule::SplineActorHasSplits(AActor* actor) {

	TArray<AActor*> aList;
	actor->GetAttachedActors(aList,true);
	for(auto subActor:aList) {
		if(subActor->Tags.Contains(FName(TEXT("SubSpline")))) return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------
// SplineActorIsSplit
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineToolsModule::SplineActorIsSplit(AActor* actor) {

	return actor->Tags.Contains(FName(TEXT("SubSpline")));
}

//----------------------------------------------------------------------------------------------------------------
// FindSubSpline
//----------------------------------------------------------------------------------------------------------------
AActor* FrdSplineToolsModule::FindSubSpline(AActor* actor,int32 ind,int32& cnt) {

	TArray<AActor*> aList;
	actor->GetAttachedActors(aList,true);
	int32 lastInd=-1;
	cnt=0;
	AActor* lastValidActor=nullptr;
	for(auto subActor:aList) {
		if(subActor->Tags.Contains(FName(TEXT("SubSpline")))) {
			cnt++;
			int32 aind=GetSubSplineIndex(subActor);
			if(aind==ind) return subActor;
			if(aind>lastInd) {
				lastValidActor=subActor;
				lastInd=aind;
			}
		}
	}

	return lastValidActor;
}

//----------------------------------------------------------------------------------------------------------------
// DuplicateActor
//----------------------------------------------------------------------------------------------------------------
AActor* FrdSplineToolsModule::DuplicateActor(AActor* actor) {

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags=RF_Transactional;
	SpawnParams.Template=actor;

	AActor* newActor=World->SpawnActor<AActor>(actor->GetClass(),FTransform(FRotator(0,0,0),FVector(0,0,0),FVector(1,1,1)),SpawnParams);
	if(newActor) {

		if(newActor->GetRootComponent()==nullptr) {
			for(auto c:newActor->GetComponents()) {
				USceneComponent* sc=Cast<USceneComponent>(c);
				if(sc && (sc->GetName().StartsWith(TEXT("Default")) || sc->GetName().StartsWith(TEXT("shared")))) {
					newActor->SetRootComponent(sc);
					break;
				}
			}
		}

		// Set the edited flag to True to override construction
		for(auto c:newActor->GetComponents()) {
			USplineComponent* sc=Cast<USplineComponent>(c);
			if(sc) {
				sc->bSplineHasBeenEdited=true;
			}
		}

		newActor->AttachToActor(actor,FAttachmentTransformRules::KeepRelativeTransform);
		newActor->SetActorRelativeTransform(FTransform(FRotator(0,0,0),FVector(0,0,0),FVector(1,1,1)));
#if ENGINE_MAJOR_VERSION>4
		newActor->SetIsSpatiallyLoaded(false);
#endif
		if(newActor->Tags.Contains(TEXT("SubSpline"))) {
			TArray<FName> delList;
			for(auto& ftag:newActor->Tags) {
				FString tag(*ftag.ToString());
				if(tag.StartsWith(TEXT("split"))) {
					delList.Add(ftag);
				}
			}
			for(auto& tag:delList) newActor->Tags.Remove(tag);

		} else {
			newActor->Tags.Add(FName(TEXT("SubSpline")));
		}
	}

	return newActor;
}

//----------------------------------------------------------------------------------------------------------------
// NewActor
//----------------------------------------------------------------------------------------------------------------
AActor* FrdSplineToolsModule::NewActor(AActor* actor,const FTransform& tran) {

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags=RF_Transactional;
	SpawnParams.Template=actor;

	AActor* newActor=World->SpawnActor<AActor>(actor->GetClass(),FTransform(FRotator(0,0,0),FVector(0,0,0),FVector(1,1,1)),SpawnParams);

	if(newActor) {

		if(newActor->GetRootComponent()==nullptr) {
			for(auto c:newActor->GetComponents()) {
				USceneComponent* sc=Cast<USceneComponent>(c);
				if(sc && (sc->GetName().StartsWith(TEXT("Default")) || sc->GetName().StartsWith(TEXT("shared")))) {
					newActor->SetRootComponent(sc);
					break;
				}
			}
		}

		newActor->AttachToActor(actor,FAttachmentTransformRules::KeepRelativeTransform);
		newActor->SetActorRelativeTransform(tran);

		// remove any un-needed tags
		TArray<FName> delList;
		for(auto& ftag:newActor->Tags) {
			FString tag=ftag.ToString();
			if(ftag==TEXT("SubSpline")) delList.Add(ftag);
			else if(tag.StartsWith(TEXT("split"))) delList.Add(ftag);
		}
		for(auto& tag:delList) newActor->Tags.Remove(tag);

		if(!newActor->Tags.Contains(TEXT("NewSpline"))) newActor->Tags.Add(FName(TEXT("NewSpline")));

		// Set the edited flag to True to override construction
		for(auto c:newActor->GetComponents()) {
			USplineComponent* sc=Cast<USplineComponent>(c);
			if(sc) {
				sc->ClearSplinePoints();
			}
		}
	}

	return newActor;
}

//----------------------------------------------------------------------------------------------------------------
// IsMainSplineSelected
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineToolsModule::IsMainSplineSelected(AActor* actor) {

	while(actor) {
		if(actor->GetAttachParentActor()) {
			actor=actor->GetAttachParentActor();
		} else {
			if(actor->IsSelected()) return true;
			return false;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------
// InsertAttachActorIndexes
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::InsertAttachActorIndexes(AActor* actor,int32 num) {

	TArray<AActor*> aList;
	actor->GetAttachedActors(aList,true);
	for(auto subActor:aList) {
		if(subActor->Tags.Contains(FName(TEXT("SubSpline")))) {

			int32 ind;
			for(auto& ftag:subActor->Tags) {
				FString tag(*ftag.ToString());
				if(tag.StartsWith(TEXT("split"))) {
					ind=FCString::Atoi(*tag.RightChop(5));
					FString nm=FString::Printf(TEXT("split%d"),ind+num);
					ftag=FName(*nm);
					break;
				}
			}
			FString nm2=FString::Printf(TEXT("SubSpline%d"),ind+num);
			subActor->SetActorLabel(nm2);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
// BuildBPSplineSection
//----------------------------------------------------------------------------------------------------------------
float FrdSplineToolsModule::BuildBPSplineSection(AActor* actor,float startOffset,int32& index) {

	float endOffset=0.0f;

	UFunction* func=actor->FindFunction(FName(TEXT("rdBuildSection")));
	if(func) {

		struct cnvParms {
#if ENGINE_MAJOR_VERSION>4
			double	startMeshOffset;
#else
			float	startMeshOffset;
#endif
			int32	startIndex;
#if ENGINE_MAJOR_VERSION>4
			double	endMeshOffset;
#else
			float	endMeshOffset;
#endif
			int32	endIndex;
		};

		cnvParms parms;
		parms.startMeshOffset=startOffset;
		parms.startIndex=index;
		actor->ProcessEvent(func,(void*)&parms);
		endOffset=parms.endMeshOffset;
		index=parms.endIndex;
	}

	return endOffset;
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	