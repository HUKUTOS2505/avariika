//
// rdSpline_ConvertLandscapeSplines.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 16th November 2023
// Last Modified: 4th October 2025

#include "rdSplineTools.h"
#include "Landscape.h"
#include "LandscapeSplineSegment.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#include "LandscapeSplineActor.h"
#include "WorldPartition/WorldPartition.h"
#endif
#include "LandscapeSplineControlPoint.h"
#include "Kismet/GameplayStatics.h"
#include "LandscapeProxy.h"
#include "LandscapeSplinesComponent.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// ConvertLandscapeSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::ConvertLandscapeSplines() {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	UClass* splineClass=ShowLandscapeSplineSettings();
	if(!splineClass) {
		return; // cancelled
	}

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdSplineTools","ConvertLandscapeSplines"));
	UE_LOG(LogTemp,Display,TEXT("ConvertLandscapeSplines()"));

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();
	if(World->IsPartitionedWorld()) {
		Convert_WP_LandscapeSplines(splineClass);
	} else {
		Convert_NonWP_LandscapeSplines(splineClass);
	}
#endif
}

//----------------------------------------------------------------------------------------------------------------
// Convert_WP_LandscapeSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::Convert_WP_LandscapeSplines(UClass* splineClass) {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();

	if(UWorldPartition* WorldPartition=World->GetWorldPartition()) {
		//WorldPartition->LoadAllPartitionCells();
	}

	TArray<AActor*> landscapes;
	UGameplayStatics::GetAllActorsOfClass(World,ALandscape::StaticClass(),landscapes);
	ALandscape* landscape=nullptr;
	if(landscapes.Num()>0) landscape=(ALandscape*)landscapes[0]; // just first one for now
	if(!landscape) {
		return;
	}

	TMap<ULandscapeSplineControlPoint*, TArray<ULandscapeSplineControlPoint*>> adjacency;
	TArray<ULandscapeSplineControlPoint*> allPoints;

	TArray<ALandscapeSplineActor*> splineActors;
	UGameplayStatics::GetAllActorsOfClass(World,ALandscapeSplineActor::StaticClass(),(TArray<AActor*>&)splineActors);

	for(ALandscapeSplineActor* splineActor:splineActors) {
		ULandscapeSplinesComponent* splines=splineActor->GetSplinesComponent();
		if(splines) {

			TArray<ULandscapeSplineControlPoint*> controlPoints=splines->GetControlPoints();
			
			allPoints.Append(controlPoints);
			for(ULandscapeSplineSegment* segment:splines->GetSegments()) {
				ULandscapeSplineControlPoint* point0=segment->Connections[0].ControlPoint;
				ULandscapeSplineControlPoint* point1=segment->Connections[1].ControlPoint;
				if(point0 && point1) {
					adjacency.FindOrAdd(point0).Add(point1);
					adjacency.FindOrAdd(point1).Add(point0);
				}
			}
		}
	}

    // Find all chains (starting from endpoints with degree 1)
    TArray<FSplineChain> chains;
    TSet<ULandscapeSplineControlPoint*> visited;

    for(ULandscapeSplineControlPoint* startPoint:allPoints) {

        if(visited.Contains(startPoint)) continue;

        TArray<ULandscapeSplineControlPoint*>* neighbors=adjacency.Find(startPoint);
        int degree=neighbors?neighbors->Num():0;
        if(degree==1) { // Start from endpoints

            FSplineChain chain;
            ULandscapeSplineControlPoint* current=startPoint;
            ULandscapeSplineControlPoint* previous=nullptr;

            while(true) {

				// Check to see if we've completed a closed loop
				if(chain.points.Contains(current)) {
					chain.bClosedLoop=true;
					break;
				}

                visited.Add(current);
                chain.points.Add(current);

                TArray<ULandscapeSplineControlPoint*>* currentNeighbors=adjacency.Find(current);
                if(!currentNeighbors||currentNeighbors->Num()==0) break;

                ULandscapeSplineControlPoint* next=nullptr;
                for(ULandscapeSplineControlPoint* neighbor:*currentNeighbors) {
                    if(neighbor!=previous) {
                        next=neighbor;
                        break;
                    }
                }

                if(!next) break;

                previous=current;
                current=next;
            }

            chains.Add(chain);
        }
    }

    // Handle remaining points (cycles or isolated) - for simplicity, treat as open chains starting from any point
    for(ULandscapeSplineControlPoint* point:allPoints) {

        if(visited.Contains(point)) continue;

        FSplineChain chain;
        ULandscapeSplineControlPoint* current=point;
        ULandscapeSplineControlPoint* previous=nullptr;

        while(true) {

			// Check to see if we've completed a closed loop
			if(chain.points.Contains(current)) {
				chain.bClosedLoop=true;
				break;
			}

            visited.Add(current);
            chain.points.Add(current);
            TArray<ULandscapeSplineControlPoint*>* currentNeighbors=adjacency.Find(current);
            if(!currentNeighbors||currentNeighbors->Num()==0) break;

            ULandscapeSplineControlPoint* next=nullptr;
            for(ULandscapeSplineControlPoint* neighbor:*currentNeighbors) {
                if(neighbor!=previous) {
                    next=neighbor;
                    break;
                }
            }
            if(!next) break;
            previous=current;
            current=next;
        }

        if(chain.points.Num()>1) chains.Add(chain);
    }

	CreateSplinesFromChains(splineClass,chains);
#endif
}

//----------------------------------------------------------------------------------------------------------------
// CreateSplinesFromChains
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::CreateSplinesFromChains(UClass* splineClass,TArray<FSplineChain>& chains) {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();

    int32 chainIndex=0;
    for(const FSplineChain& chain:chains) {

		if(chain.points.Num()<1) {
			chainIndex++;
			continue;
		}

		// Spawn a new actor with one spline component per chain
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.NameMode=FActorSpawnParameters::ESpawnActorNameMode::Requested;
		SpawnParams.ObjectFlags=RF_Public|RF_Transactional;
		SpawnParams.bDeferConstruction=false;
		
		ULandscapeSplineControlPoint* firstPoint=chain.points[0]; // get the location of the first control point
		ULandscapeSplinesComponent* firstOwner=Cast<ULandscapeSplinesComponent>(firstPoint->GetOuter());
		FVector firstPos=firstOwner->GetComponentLocation();
		FVector aloc=firstPoint->Location+firstPos;

		AActor* splineActor=World->SpawnActor<AActor>(splineClass,FTransform::Identity,SpawnParams);
		splineActor->PostEditMove(true);
		splineActor->MarkPackageDirty();
		ULevel::LevelDirtiedEvent.Broadcast();

		TArray<UActorComponent*> comps;
		splineActor->GetComponents(USplineComponent::StaticClass(),comps);

		splineActor->Modify();
		USplineComponent* sc=nullptr;
		bool hadExistingSpline=false;
		if(comps.Num()==0) {

			sc=NewObject<USplineComponent>(splineActor,*FString::Printf(TEXT("ImportedSpline_%d"),chainIndex));
			sc->RegisterComponent();
			sc->AttachToComponent(splineActor->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
			splineActor->AddInstanceComponent(sc);
			splineActor->SetRootComponent(sc);

		} else {
			
			sc=(USplineComponent*)comps[0];
			hadExistingSpline=true;

//			bool isRoot=splineActor->GetRootComponent()==sc;
			sc->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			sc->UnregisterComponent();
			splineActor->AddInstanceComponent(sc);

//			if(isRoot) {
				splineActor->SetRootComponent(sc);
//			} else {
//				sc->AttachToComponent(splineActor->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
//			}
			//sc->ComponentFlags;
			//FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(Blueprint, VariableName, !bInstanceEditable);

		}
		if(!sc) return;

		sc->Modify();
		sc->ClearSplinePoints(true);

		int32 i=0;
		for(ULandscapeSplineControlPoint* point:chain.points) {

			FVector currPos=point->Location;

			// Process segments to get TangentLen
			FVector arriveTangent=FVector::ZeroVector;
			FVector leaveTangent=FVector::ZeroVector;
			bool bValidRotation=!point->Rotation.ContainsNaN()&&!point->Rotation.Vector().IsNearlyZero();
			FVector rotationNormal=bValidRotation?point->Rotation.Vector():FVector::ForwardVector;
			float arriveTangentLen=10.0f;
			float leaveTangentLen=10.0f;

			for(const FLandscapeSplineConnection& connection:point->ConnectedSegments) {

				if(!connection.Segment) continue;

	            ULandscapeSplineSegment* segment=connection.Segment;
				bool bIsStartPoint=(segment->Connections[0].ControlPoint==point);
				ULandscapeSplineControlPoint* otherPoint=bIsStartPoint?segment->Connections[1].ControlPoint:segment->Connections[0].ControlPoint;
				if(!otherPoint||otherPoint->Location.ContainsNaN()) continue;

				float tangentLength=bIsStartPoint?segment->Connections[0].TangentLen:segment->Connections[1].TangentLen;
				FVector tangentDirection=(tangentLength>=0.0f?rotationNormal:-rotationNormal);
				if(bIsStartPoint) {
					leaveTangentLen=fabs(tangentLength);
					leaveTangent=-tangentDirection*leaveTangentLen;
				} else {
					arriveTangentLen=fabs(tangentLength);
					arriveTangent=tangentDirection*arriveTangentLen;
				}
	        }

			ULandscapeSplinesComponent* owner=Cast<ULandscapeSplinesComponent>(point->GetOuter());
			FVector pos=owner->GetComponentLocation();
			FVector loc=(point->Location+pos)-aloc;

			sc->AddSplinePointAtIndex(loc,i,ESplineCoordinateSpace::World,false);
			sc->SetSplinePointType(i,ESplinePointType::Curve,false);
			sc->SetRotationAtSplinePoint(i,point->Rotation,ESplineCoordinateSpace::World,false);
			sc->SetTangentsAtSplinePoint(i,arriveTangent,leaveTangent,ESplineCoordinateSpace::Local,false);
			i++;
		}

		FVector point0=sc->GetLocationAtSplinePoint(0,ESplineCoordinateSpace::Local);
		FVector point1=sc->GetLocationAtSplinePoint(1,ESplineCoordinateSpace::Local);
		FVector arriveTangent=sc->GetArriveTangentAtSplinePoint(1,ESplineCoordinateSpace::Local); // always 0,0,0
		FVector leaveTangent=sc->GetLeaveTangentAtSplinePoint(1,ESplineCoordinateSpace::Local);
		float dist1=FVector::Distance(point0,point1+arriveTangent);
		float dist2=FVector::Distance(point0,point1-leaveTangent);
		if(dist1<=dist2) {
			for(int32 ii=0;ii<i;ii++) {

				FVector aTan=sc->GetArriveTangentAtSplinePoint(ii,ESplineCoordinateSpace::Local);
				FVector lTan=sc->GetLeaveTangentAtSplinePoint(ii,ESplineCoordinateSpace::Local);
				sc->SetTangentsAtSplinePoint(ii,aTan*-1.0f,lTan*-1.0f,ESplineCoordinateSpace::Local,false);
			}
		}

		if(chain.bClosedLoop) {

			//sc->RemoveSplinePoint(sc->GetNumberOfSplinePoints()-1,false);
			sc->SetClosedLoop(true,true);
			/*
			// Ensure smooth tangents for closed loops
			int32 LastIndex=sc->GetNumberOfSplinePoints()-1;
			FVector FirstArriveTangent=sc->GetArriveTangentAtSplinePoint(0,ESplineCoordinateSpace::Local);
			FVector FirstLeaveTangent=sc->GetLeaveTangentAtSplinePoint(0,ESplineCoordinateSpace::Local);
			FVector LastArriveTangent=sc->GetArriveTangentAtSplinePoint(LastIndex,ESplineCoordinateSpace::Local);
			//FVector LastLeaveTangent=sc->GetLeaveTangentAtSplinePoint(LastIndex,ESplineCoordinateSpace::Local);
			//sc->SetTangentsAtSplinePoint(0,FirstArriveTangent,LastLeaveTangent,ESplineCoordinateSpace::Local,true);
			//sc->SetTangentsAtSplinePoint(LastIndex,LastLeaveTangent,FirstArriveTangent,ESplineCoordinateSpace::Local,true);
			sc->SetTangentsAtSplinePoint(LastIndex,LastArriveTangent,FirstLeaveTangent,ESplineCoordinateSpace::Local,true);
			*/
		}
		sc->UpdateSpline();
		sc->Modify();

		sc->MarkRenderStateDirty();
        sc->MarkPackageDirty();
		sc->UpdateComponentToWorld();

		splineActor->PostActorCreated();
		splineActor->PostEditChange();
		splineActor->RerunConstructionScripts();
		splineActor->SetActorLocation(aloc);

		chainIndex++;
	}
#endif
}

//----------------------------------------------------------------------------------------------------------------
// Convert_NonWP_LandscapeSplines
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::Convert_NonWP_LandscapeSplines(UClass* splineClass) {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();

	TArray<AActor*> landscapes;
	UGameplayStatics::GetAllActorsOfClass(World,ALandscape::StaticClass(),landscapes);
	ALandscape* landscape=nullptr;
	if(landscapes.Num()>0) landscape=(ALandscape*)landscapes[0]; // just first one for now
	if(!landscape) {
		return;
	}

	TArray<ALandscapeProxy*> lsProxies;
	UGameplayStatics::GetAllActorsOfClass(World,ALandscapeProxy::StaticClass(),(TArray<AActor*>&)lsProxies);

	// Collect all control points and build adjacency graph (undirected)
	TMap<ULandscapeSplineControlPoint*, TArray<ULandscapeSplineControlPoint*>> adjacency;
	TArray<ULandscapeSplineControlPoint*> allPoints;
	for(AActor* actor:landscapes) {
		ALandscapeProxy* ls=Cast<ALandscapeProxy>(actor);
		if(!ls) continue;
		ULandscapeSplinesComponent* splines=ls->GetSplinesComponent();
		if(splines) {
			allPoints.Append(splines->GetControlPoints());
			for(ULandscapeSplineSegment* segment:splines->GetSegments())  {
				ULandscapeSplineControlPoint* point0=segment->Connections[0].ControlPoint;
				ULandscapeSplineControlPoint* point1=segment->Connections[1].ControlPoint;
				if(point0 && point1) {
					adjacency.FindOrAdd(point0).Add(point1);
					adjacency.FindOrAdd(point1).Add(point0);
				}
			}
		}
	}

    // Find all chains (starting from endpoints with degree 1)
    TArray<FSplineChain> chains;
    TSet<ULandscapeSplineControlPoint*> visited;
    for(ULandscapeSplineControlPoint* startPoint:allPoints) {

        if(visited.Contains(startPoint)) continue;

        TArray<ULandscapeSplineControlPoint*>* neighbors=adjacency.Find(startPoint);
        int32 degree=neighbors?neighbors->Num():0;
        if(degree==1) { // Start from endpoints
	        FSplineChain chain;
            ULandscapeSplineControlPoint* current=startPoint;
            ULandscapeSplineControlPoint* previous=nullptr;

            while(true) {

				// Check to see if we've completed a closed loop
				if(chain.points.Contains(current)) {
					chain.bClosedLoop=true;
					break;
				}

                visited.Add(current);
                chain.points.Add(current);

                TArray<ULandscapeSplineControlPoint*>* currentNeighbors=adjacency.Find(current);
                if(!currentNeighbors||currentNeighbors->Num()==0) break;

                ULandscapeSplineControlPoint* next=nullptr;
                for(ULandscapeSplineControlPoint* neighbor:*currentNeighbors) {
                    if(neighbor!=previous) {
                        next=neighbor;
                        break;
                    }
                }

                if(!next) break;

                previous=current;
                current=next;
            }

            chains.Add(chain);
        }
    }

    // Handle remaining points (cycles or isolated) - for simplicity, treat as open chains starting from any point
    for(ULandscapeSplineControlPoint* point:allPoints) {

        if(visited.Contains(point)) continue;

        FSplineChain chain;
        ULandscapeSplineControlPoint* current=point;
        ULandscapeSplineControlPoint* previous=nullptr;

        while(true) {

			// Check to see if we've completed a closed loop
			if(chain.points.Contains(current)) {
				chain.bClosedLoop=true;
				break;
			}

            visited.Add(current);
            chain.points.Add(current);

            TArray<ULandscapeSplineControlPoint*>* currentNeighbors=adjacency.Find(current);
            if(!currentNeighbors||currentNeighbors->Num()==0) break;

            ULandscapeSplineControlPoint* next=nullptr;
            for(ULandscapeSplineControlPoint* neighbor:*currentNeighbors) {
                if(neighbor!=previous) {
                    next=neighbor;
                    break;
                }
            }
            if(!next) break;

            previous=current;
            current=next;
        }

        if(chain.points.Num()>1) chains.Add(chain);
    }

	CreateSplinesFromChains(splineClass,chains);
#endif
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	