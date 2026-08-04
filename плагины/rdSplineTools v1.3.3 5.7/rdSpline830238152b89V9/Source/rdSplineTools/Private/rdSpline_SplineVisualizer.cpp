//
// rdSpline_SplineVisualizer.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 22nd March 2025

#include "rdSpline_SplineVisualizer.h"
#include "rdSplineTools.h"
#include "LevelEditorActions.h"
#include "Misc/OutputDeviceNull.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshSocket.h"
#include "Landscape.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------
FrdSplineComponentVisualizer::FrdSplineComponentVisualizer() : FSplineComponentVisualizer(),splineTools(nullptr),prevSubSpline(nullptr),nextSubSpline(nullptr) {

	FrdSplineToolsMenuCommands::Register();
	rdSplineComponentVisualizerActions=MakeShareable(new FUICommandList);


}

FrdSplineComponentVisualizer::FrdSplineComponentVisualizer(FrdSplineToolsModule* mod) : FSplineComponentVisualizer(),splineTools(mod),prevSubSpline(nullptr),nextSubSpline(nullptr) {

	FrdSplineToolsMenuCommands::Register();
	rdSplineComponentVisualizerActions=MakeShareable(new FUICommandList);
}

//----------------------------------------------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------------------------------------------
FrdSplineComponentVisualizer::~FrdSplineComponentVisualizer() {
}

//----------------------------------------------------------------------------------------------------------------
// OnRegister
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::OnRegister() {
}

//----------------------------------------------------------------------------------------------------------------
// RegisterCommands
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::RegisterCommands(TSharedPtr<class FUICommandList> rdLevelEditorCommands) {
}

//----------------------------------------------------------------------------------------------------------------
// HandleFrustumSelect
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineComponentVisualizer::HandleFrustumSelect(const FConvexVolume& InFrustum, FEditorViewportClient* InViewportClient, FViewport* InViewport) {

	return FSplineComponentVisualizer::HandleFrustumSelect(InFrustum,InViewportClient,InViewport);
}

//----------------------------------------------------------------------------------------------------------------
// HandleBoxSelect
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineComponentVisualizer::HandleBoxSelect(const FBox& InBox,FEditorViewportClient* InViewportClient,FViewport* InViewport) {

	return FSplineComponentVisualizer::HandleBoxSelect(InBox,InViewportClient,InViewport);
}

//----------------------------------------------------------------------------------------------------------------
// SetupSplinePtrs
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SetupSplinePtrs() {

	prevSubSpline=nullptr;
	nextSubSpline=nullptr;

	rdInitVis();

	USplineComponent* spline=GetEditedSplineComponent();
	if(spline && spline!=lastSpline) {
		if(!rdSplineToolsoptions.Get()) {
			rdSplineToolsoptions=DuplicateObject(GetMutableDefault<UrdSplineToolsOptions>(),GetTransientPackage());
			rdSplineToolsoptions->splineModule=splineTools;
			splineTools->FillFromOptionDefaults(TEXT("Defaults"),rdSplineToolsoptions.Get());
		}
		visMeshIntersection=rdSplineToolsoptions->visMeshIntersection.Get();
		if(!visMeshIntersection) visMeshIntersection=rdSplineToolsoptions->visMeshIntersection.LoadSynchronous();

		visMeshSocket=rdSplineToolsoptions->visMeshSocket.Get();
		if(!visMeshSocket) visMeshSocket=rdSplineToolsoptions->visMeshSocket.LoadSynchronous();

		visMeshSplinePoint=rdSplineToolsoptions->visMeshSplinePoint.Get();
		if(!visMeshSplinePoint) visMeshSplinePoint=rdSplineToolsoptions->visMeshSplinePoint.LoadSynchronous();

		AActor* actor=spline->GetOwner();
		if(actor) {

			if(landscapes.Num()==0) {
				UGameplayStatics::GetAllActorsOfClass(actor->GetWorld(),ALandscape::StaticClass(),landscapes);
			}

			if(actor->Tags.Contains(FName(TEXT("SubSpline")))) {
				AActor* pa=actor->GetAttachParentActor();
				if(pa) {
					int32 ind=splineTools->GetSubSplineIndex(actor),cnt=0;
					if(ind==0) {
						prevSubSpline=splineTools->GetSpline(pa);
						AActor* sub=splineTools->FindSubSpline(pa,1,cnt);
						if(sub) nextSubSpline=splineTools->GetSpline(sub);

					} else {
						AActor* sub=splineTools->FindSubSpline(pa,ind-1,cnt);
						if(sub) prevSubSpline=splineTools->GetSpline(sub);

						sub=splineTools->FindSubSpline(pa,-1,cnt);
						if(ind>=(cnt-1)) {
							nextSubSpline=splineTools->GetSpline(pa);
						} else {
							sub=splineTools->FindSubSpline(pa,ind+1,cnt);
							if(sub) nextSubSpline=splineTools->GetSpline(sub);
						}
					}
				}
			} else if(splineTools->SplineActorHasSplits(actor)) {

				int32 cnt=0;
				AActor* sub=splineTools->FindSubSpline(actor,0,cnt);
				if(sub) nextSubSpline=splineTools->GetSpline(sub);
				sub=splineTools->FindSubSpline(actor,-1,cnt);
				if(sub) prevSubSpline=splineTools->GetSpline(sub);
			}
		}

		lastSpline=spline;
	}
}

//----------------------------------------------------------------------------------------------------------------
// SnapToIntersections
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineComponentVisualizer::SnapToIntersections(AActor* actor,AActor* otherActor,FVector& DeltaTranslate,USplineComponent* spline,int32 index,FVector loc,FViewport* Viewport) {

	if(!otherActor) {
		rdRemAllIntersectionVis();
		return false;
	}

	if(!((rdSplineToolsoptions->snapIntersectionsType==rdSnapType::RDSNAP_ALWAYS && !Viewport->KeyState(rdSplineToolsoptions->hotkeySnap)) || (rdSplineToolsoptions->snapIntersectionsType==rdSnapType::RDSNAP_WITHKEY && Viewport->KeyState(rdSplineToolsoptions->hotkeySnap)))) {
		return false;
	}

	if(!otherActor) {
		UFunction* func=spline->GetOwner()->FindFunction(FName(TEXT("rdClearIntersection")));
		if(func) {
			struct intsctnParms { bool atStart; };
			intsctnParms parms;
			parms.atStart=(index==0);
			spline->GetOwner()->ProcessEvent(func,(void*)&parms);
		}
		return false;
	}

	FVector closestSocketPoint=FVector();
	float closestSocketDist=999999.9f;
	float closestPointDist=999999.9f;
	float closestZRot=0.0f;
	bool closestOnLeft=true;

	TArray<UActorComponent*> splineList;
	otherActor->GetComponents(USplineComponent::StaticClass(),splineList);
	TArray<UActorComponent*> splineComps;
	otherActor->GetComponents(USplineMeshComponent::StaticClass(),splineComps);

	USplineComponent* otherSpline=nullptr;
	if(splineList.Num()>0) otherSpline=(USplineComponent*)splineList[0];

	for(auto c:splineComps) {
		USplineMeshComponent* sc=(USplineMeshComponent*)c;
		UStaticMesh* mesh=sc->GetStaticMesh();
		if(mesh) {
#if ENGINE_MAJOR_VERSION<5
			TArray<UStaticMeshSocket*>& sockets=mesh->Sockets;
#else
			TArray<TObjectPtr<class UStaticMeshSocket>>& sockets=mesh->Sockets;
#endif
			for(auto s:sockets) {
				if(s->SocketName.ToString().Contains(TEXT("Intersection"))) {
					FTransform t=sc->GetSocketTransform(s->SocketName,RTS_World); 
					FVector sockLocation=t.GetLocation();
					sockLocation.Z=loc.Z;
					float dist=FVector::Distance(sockLocation,loc);
					if(dist<closestSocketDist) {
						closestSocketPoint=t.GetLocation();
						closestSocketDist=dist;
						closestOnLeft=s->SocketName.ToString().Contains(TEXT("Left"));
						closestZRot=t.Rotator().Yaw;
					}
				}
			}
		}
	}

	if(closestSocketDist<rdSplineToolsoptions->visIntersectionRadius) {
		rdAddIntersectionVis(closestSocketPoint,closestZRot,closestSocketDist,otherActor,rdSplineToolsoptions.Get());
	} else {
		rdRemIntersectionVis(otherActor);
	}

	if(!originalTangentSet) {
		originalTangent=spline->GetTangentAtSplinePoint(index,ESplineCoordinateSpace::Local);
		originalTangentSet=true;
	}

	if(closestSocketDist<rdSplineToolsoptions->snapIntersectionRadius) {

		DeltaTranslate=closestSocketPoint-spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::World);

		if(rdSplineToolsoptions->snapIntersectionsTangent) {
			float pdist=(index==0)?(spline->GetDistanceAlongSplineAtSplinePoint(1)-spline->GetDistanceAlongSplineAtSplinePoint(0)):(spline->GetDistanceAlongSplineAtSplinePoint(index)-spline->GetDistanceAlongSplineAtSplinePoint(index-1));
			float oy=-actor->GetActorRotation().Yaw;
			FRotator rot(0,oy+closestZRot+((index==spline->GetNumberOfSplinePoints()-1)?180:0),0);
			FVector tg=UKismetMathLibrary::GetRightVector(rot);
			tg*=(pdist*0.5f);
			spline->SetTangentAtSplinePoint(index,tg,ESplineCoordinateSpace::Local);
		}

		UFunction* func=otherActor->FindFunction(FName(TEXT("rdCreateIntersection")));
		if(func) {

			struct intsctnParms {
				FVector location;
				bool	leftSide;
				bool	atStart;
				AActor* otherRoad;
			};

			intsctnParms parms;
			parms.location=closestSocketPoint;
			parms.leftSide=closestOnLeft;
			parms.atStart=(index==0);
			parms.otherRoad=spline->GetOwner();

			otherActor->ProcessEvent(func,(void*)&parms);
		}

		return true;

	} else {

		if(originalTangentSet) {
			spline->SetTangentAtSplinePoint(index,originalTangent,ESplineCoordinateSpace::Local);
			spline->SetSplinePointType(index,ESplinePointType::Curve,true);
		}

		UFunction* func=spline->GetOwner()->FindFunction(FName(TEXT("rdClearIntersection")));
		if(func) {
			struct intsctnParms { bool atStart; };
			intsctnParms parms;
			parms.atStart=(index==0);
			spline->GetOwner()->ProcessEvent(func,(void*)&parms);
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------------------
// SnapToSockets
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineComponentVisualizer::SnapToSockets(AActor* actor,AActor* otherActor,FVector& DeltaTranslate,USplineComponent* spline,int32 index,FVector loc,FViewport* Viewport) {

	if(!otherActor) {
		rdRemAllSocketVis();
		return false;
	}

	bool bSophisticatedSnap=Viewport->KeyState(rdSplineToolsoptions->hotkeySophisticatedSnap);
	if(!((rdSplineToolsoptions->snapSocketsType==rdSnapType::RDSNAP_ALWAYS && !Viewport->KeyState(rdSplineToolsoptions->hotkeySnap))|| (rdSplineToolsoptions->snapSocketsType==rdSnapType::RDSNAP_WITHKEY && (Viewport->KeyState(rdSplineToolsoptions->hotkeySnap) || bSophisticatedSnap)))) {
		return false;
	}

	float oy=-actor->GetActorRotation().Yaw;
	if(bSophisticatedSnap) { // Sophisticated way, handled by spline blueprint
					
		UFunction* func=otherActor->FindFunction(FName(TEXT("rdGetClosestSocket")));
		if(func) {

			uint8* parms=(uint8*)FMemory_Alloca(func->ParmsSize);
			FMemory::Memzero(parms,func->ParmsSize);
			float* floatParms=(float*)parms;

			floatParms[0]=loc.X; // x
			floatParms[1]=loc.Y; // y
			floatParms[2]=loc.Z; // z

			otherActor->ProcessEvent(func,parms);

			FVector vec(floatParms[3],floatParms[4],floatParms[5]); // amount to modify to snap to closest socket

			if(fabs(vec.X)>0.0001f || fabs(vec.Y)>0.0001f || fabs(vec.Z)>0.0001f) {
				DeltaTranslate=vec-spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::World);
			}
		}

	} else {

		// Scan through all sockets of parent/children spline mesh components.
		// Note that sockets can be on top of each other (e.g. left and right lane sockets).
		// To find the correct socket, the mouse cursor is translated into a distance along the spline and 
		// the correct socket is selected by whether the spline distance is before or after the socket point.
		TArray<AActor*> actors;
		otherActor->GetAttachedActors(actors);
		actors.Add(otherActor);
		FVector closestPoint;
		float closestDistance=999999.9f;
		float closestZRot=0.0f;

		if(otherActor->GetAttachParentActor()) {
			actors.AddUnique(otherActor->GetAttachParentActor());
			TArray<AActor*> sactors;
			otherActor->GetAttachParentActor()->GetAttachedActors(sactors);
			for(auto a:sactors) {
				if(a!=spline->GetOwner()) { // don't snap to our own sockets
					actors.AddUnique(a);
				}
			}
		}

		struct sminfo {
			sminfo(const FString& n,USplineComponent* s,float d) : name(n),spline(s),distance(d) {}
			FString name;
			USplineComponent* spline;
			float distance;
		};

		TMap<USplineMeshComponent*,sminfo> potentialCloseSockets;
		for(auto a:actors) {

			TArray<UActorComponent*> splineList;
			a->GetComponents(USplineComponent::StaticClass(),splineList);
			USplineComponent* otherSpline=nullptr;
			if(splineList.Num()>0) otherSpline=(USplineComponent*)splineList[0];

			TArray<UActorComponent*> comps;
			a->GetComponents(USplineMeshComponent::StaticClass(),comps);
			for(auto c:comps) {
				USplineMeshComponent* sc=(USplineMeshComponent*)c;
				UStaticMesh* mesh=sc->GetStaticMesh();
				if(mesh) {
#if ENGINE_MAJOR_VERSION<5
					TArray<UStaticMeshSocket*>& sockets=mesh->Sockets;
#else
					TArray<TObjectPtr<class UStaticMeshSocket>>& sockets=mesh->Sockets;
#endif
					FString closestSocketName;
					for(auto s:sockets) {
						if(s->SocketName.ToString().Contains(TEXT("Lane"))) {
							FTransform t=sc->GetSocketTransform(s->SocketName,RTS_World); 
							float dist=FVector::Distance(t.GetLocation(),loc);
							if(dist<=closestDistance && dist<rdSplineToolsoptions->visSocketRadius) {
								closestDistance=dist;
								closestSocketName=s->SocketName.ToString();
							}
						}
					}

					if(!closestSocketName.IsEmpty() && closestDistance<rdSplineToolsoptions->visSocketRadius) {
						potentialCloseSockets.Add(sc,sminfo(closestSocketName,otherSpline,closestDistance));
					}
				}
			}
		}

		TMap<USplineMeshComponent*,sminfo> closeSockets;
		float minDist=9999999.9f;
		for(auto& it:potentialCloseSockets) {
			if(minDist>it.Value.distance) minDist=it.Value.distance;
		}

		for(auto& it:potentialCloseSockets) {
			if(it.Value.distance<=(minDist+2.0f)) {
				closeSockets.Add(it.Key,it.Value);
			}
		}

		if(closeSockets.Num()==1) {
			for(auto& it:closeSockets) {
				FTransform t=it.Key->GetSocketTransform(FName(*it.Value.name),RTS_World); 
				closestPoint=t.GetLocation();
				bool closestOnLeft=it.Value.name.Contains(TEXT("Left"));
				closestZRot=t.Rotator().Yaw+closestOnLeft?0:180;
			}

		} else {

			for(auto& it:closeSockets) {

				if(!it.Value.spline) continue;

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<3
				float dist1=it.Value.spline->FindInputKeyClosestToWorldLocation(loc);
#else
				float dist1=it.Value.spline->GetDistanceAlongSplineAtLocation(loc,ESplineCoordinateSpace::World);
#endif
				bool closestOnLeft=it.Value.name.Contains(TEXT("Left"));
				FTransform t=it.Key->GetSocketTransform(FName(*it.Value.name),RTS_World); 
#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<3
				float dist2=it.Value.spline->FindInputKeyClosestToWorldLocation(t.GetLocation());
#else
				float dist2=it.Value.spline->GetDistanceAlongSplineAtLocation(t.GetLocation(),ESplineCoordinateSpace::World);
#endif
				if(closestOnLeft && dist1<dist2) {
					closestPoint=t.GetLocation();
					closestZRot=t.Rotator().Yaw;
					break;
				} else if(!closestOnLeft && dist1>dist2) {
					closestPoint=t.GetLocation();
					closestZRot=t.Rotator().Yaw+180;
					break;
				}
			}
		}

		if(closestDistance<rdSplineToolsoptions->visSocketRadius) {
			rdAddSocketVis(closestPoint,closestZRot,closestDistance,otherActor,rdSplineToolsoptions.Get());
		} else {
			rdRemSocketVis(otherActor);
		}

		if(!originalTangentSet) {
			originalTangent=spline->GetTangentAtSplinePoint(index,ESplineCoordinateSpace::Local);
			originalTangentSet=true;
		}

		if(closestDistance<rdSplineToolsoptions->snapSocketRadius) {
			DeltaTranslate=closestPoint-spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::World);
			if(rdSplineToolsoptions->snapSocketsTangent) {
				float pdist=(index==0)?(spline->GetDistanceAlongSplineAtSplinePoint(1)-spline->GetDistanceAlongSplineAtSplinePoint(0)):(spline->GetDistanceAlongSplineAtSplinePoint(index)-spline->GetDistanceAlongSplineAtSplinePoint(index-1));
				FRotator rot(0,oy+closestZRot,0);
				FVector tg=UKismetMathLibrary::GetForwardVector(rot);
				tg*=(pdist*0.5f);
				spline->SetTangentAtSplinePoint(index,tg,ESplineCoordinateSpace::Local);
			}
			return true;
		} else {

			if(originalTangentSet) {
				spline->SetTangentAtSplinePoint(index,originalTangent,ESplineCoordinateSpace::Local);
				spline->SetSplinePointType(index,ESplinePointType::Curve,true);
			}
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------------------
// SnapToSplinePoints
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineComponentVisualizer::SnapToSplinePoints(AActor* actor,AActor* otherActor,FVector& DeltaTranslate,USplineComponent* spline,int32 index,FVector loc,FViewport* Viewport) {

	if(!((rdSplineToolsoptions->snapSplinePointsType==rdSnapType::RDSNAP_ALWAYS && !Viewport->KeyState(rdSplineToolsoptions->hotkeySnap)) || (rdSplineToolsoptions->snapSplinePointsType==rdSnapType::RDSNAP_WITHKEY && Viewport->KeyState(rdSplineToolsoptions->hotkeySnap)))) {
		return false;
	}

	float closestDistance=9999999.0f;
	FVector closestPoint=FVector(0,0,0);
	float closestZRot=0.0f;
	FVector closestTangent=FVector();
	AActor* closestActor=nullptr;
	FVector newLoc=loc,t1;
	bool firstPoint=index==0,lastPoint=index==spline->GetNumberOfSplinePoints()-1;
	TArray<AActor*> otherBPs;

	TArray<UActorComponent*> splineComps;
	UGameplayStatics::GetAllActorsOfClass(spline->GetOwner()->GetWorld(),AActor::StaticClass(),otherBPs);

	float oy=-actor->GetActorRotation().Yaw;
	for(auto oa:otherBPs) {
		if(oa==spline->GetOwner()) continue;

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>24
		oa->GetComponents(USplineComponent::StaticClass(),splineComps);
#else
		splineComps=oa->GetComponentsByClass(USplineComponent::StaticClass());
#endif
		for(auto c:splineComps) {
			USplineComponent* sc=(USplineComponent*)c;
			if(sc) {
				int32 key=(int32)(sc->FindInputKeyClosestToWorldLocation(loc)+0.5f);
				FVector sloc=sc->GetLocationAtSplinePoint(key,ESplineCoordinateSpace::World);
				float pointDistanceAlongSpline=sc->GetDistanceAlongSplineAtSplinePoint(key);
#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<3
				float mouseDistanceAlongSpline=sc->FindInputKeyClosestToWorldLocation(loc);
#else
				float mouseDistanceAlongSpline=sc->GetDistanceAlongSplineAtLocation(loc,ESplineCoordinateSpace::World);
#endif
				float dist=FVector::Distance(loc,sloc);
				if(dist<closestDistance) {

					FVector cv=sloc-loc;
					cv.Normalize();
					bool up=cv.Y>0.0f;
					bool left=cv.X>0.0f;
					bool tg=fabs(cv.X)>fabs(cv.Y);
					float yaw=tg?(firstPoint?(left?0.0f:180.0f):(left?180.0f:0.0f)):(firstPoint?(up?90.0f:-90.0f):(up?-90.0f:90.0f));
					//if(!key) yaw=180.0f; // first point

					newLoc=sloc;
					t1=sc->GetArriveTangentAtSplinePoint(key,ESplineCoordinateSpace::Local);
					closestDistance=dist;
					closestPoint=sloc;
					closestZRot=sc->GetTransformAtSplinePoint(key,ESplineCoordinateSpace::World).Rotator().Yaw;
					float pdist=(index==0)?spline->GetDistanceAlongSplineAtSplinePoint(1):(spline->GetDistanceAlongSplineAtSplinePoint(index)-spline->GetDistanceAlongSplineAtSplinePoint(index-1));
					FRotator rot(0,oy+closestZRot+yaw,0);
					closestTangent=UKismetMathLibrary::GetForwardVector(rot);
					closestTangent*=(pdist*0.5f);
					closestActor=oa;
				}
			}
		}
	}

	if(closestDistance<rdSplineToolsoptions->visSplinePointRadius) {
		rdAddSplinePointVis(closestPoint,closestZRot,closestDistance,closestActor,rdSplineToolsoptions.Get());
	} else {
		rdRemSplinePointVis(closestActor);
	}

	if(!originalTangentSet) {
		originalTangent=spline->GetTangentAtSplinePoint(index,ESplineCoordinateSpace::Local);
		originalTangentSet=true;
	}

	if(closestDistance<rdSplineToolsoptions->snapSplinePointRadius) {
		DeltaTranslate=closestPoint-spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::World);
		if(rdSplineToolsoptions->snapSplinePointsTangent) {
			spline->SetTangentAtSplinePoint(index,closestTangent,ESplineCoordinateSpace::Local);
		}
		return true;
	} else {

		if(originalTangentSet) {
			spline->SetTangentAtSplinePoint(index,originalTangent,ESplineCoordinateSpace::Local);
			spline->SetSplinePointType(index,ESplinePointType::Curve,true);
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------------------
// VisProxyHandleClick
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineComponentVisualizer::VisProxyHandleClick(FEditorViewportClient* InViewportClient,HComponentVisProxy* VisProxy,const FViewportClick& Click) {

//	if(splineTools) SetupSplinePtrs();
	return FSplineComponentVisualizer::VisProxyHandleClick(InViewportClient,VisProxy,Click);
}

//----------------------------------------------------------------------------------------------------------------
// EndEditing
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::EndEditing() {

	if(GEditor->GetTimerManager()->IsTimerActive(splineVisTimerHandle)) {
		GEditor->GetTimerManager()->ClearTimer(splineVisTimerHandle);
	}
	rdRemAllVis();
	originalTangentSet=false;
	FSplineComponentVisualizer::EndEditing();
}

//----------------------------------------------------------------------------------------------------------------
// TrackingStopped
//----------------------------------------------------------------------------------------------------------------
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
void FrdSplineComponentVisualizer::TrackingStopped(FEditorViewportClient* InViewportClient,bool bInDidMove) {

	if(GEditor->GetTimerManager()->IsTimerActive(splineVisTimerHandle)) {
		GEditor->GetTimerManager()->ClearTimer(splineVisTimerHandle);
	}
	rdRemAllVis();
	originalTangentSet=false;
	FSplineComponentVisualizer::TrackingStopped(InViewportClient,bInDidMove);
}
#endif

void FrdSplineComponentVisualizer::doFullDraw() {

	if(!drawActor) return;

	UFunction* func=drawActor->FindFunction(FName(TEXT("rdBuild")));
	if(func) drawActor->ProcessEvent(func,nullptr);
	else drawActor->RerunConstructionScripts();

	drawActor=nullptr;
}

//----------------------------------------------------------------------------------------------------------------
// HandleInputDelta
//----------------------------------------------------------------------------------------------------------------
bool FrdSplineComponentVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient,FViewport* Viewport,FVector& DeltaTranslate,FRotator& DeltaRotate,FVector& DeltaScale) {

	if(splineTools) SetupSplinePtrs();

	USplineComponent* spline=GetEditedSplineComponent();
	if(!spline) {
		return FSplineComponentVisualizer::HandleInputDelta(ViewportClient,Viewport,DeltaTranslate,DeltaRotate,DeltaScale);
	}

	AActor* actor=spline->GetOwner();

	if(spline && actor) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
		if(rdSplineToolsoptions->doFullDraw) {

			drawActor=actor;
			FTimerDelegate doFullDrawDelegate;
			doFullDrawDelegate.BindRaw(this,&FrdSplineComponentVisualizer::doFullDraw);

			GEditor->GetTimerManager()->SetTimer(splineVisTimerHandle,doFullDrawDelegate,rdSplineToolsoptions->fullDrawTime,false);
		}
#endif

		int32 numPoints=spline->GetNumberOfSplinePoints();
#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
		const TSet<int32>& SelectedKeys=SelectionState->GetSelectedKeys();
#endif
		int32 index=0;
		for(int32 i:SelectedKeys) index=i;

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
		int32 tangT=SelectionState?((int32)SelectionState->GetSelectedTangentHandleType()):0; // 0=normal point, 1=arrive tangent point, 2=leave tangent point
#else
		int32 tangT=SelectedTangentHandleType; // 0=normal point, 1=arrive tangent point, 2=leave tangent point
#endif

		if(tangT==0 && SelectedKeys.Num()==1 && (index==0 || index==numPoints-1)) {

			FVector loc=spline->GetLocationAtSplinePoint(index,ESplineCoordinateSpace::World)+DeltaTranslate;
			AActor* otherActor=nullptr;
			FCollisionQueryParams queryParams;
			queryParams.AddIgnoredActor(actor);
			for(auto ls:landscapes) queryParams.AddIgnoredActor(ls);
			FHitResult hit;
			actor->GetWorld()->LineTraceSingleByChannel(hit,FVector(loc.X,loc.Y,loc.Z+2000),FVector(loc.X,loc.Y,loc.Z-2000),ECollisionChannel::ECC_WorldStatic,queryParams); // ECC_Visibility

#if ENGINE_MAJOR_VERSION>4
			otherActor=hit.GetHitObjectHandle().FetchActor();
#else
			otherActor=hit.Actor.Get();
#endif

			bool ret=SnapToIntersections(actor,otherActor,DeltaTranslate,spline,index,loc,Viewport);
			if(!ret) {
				ret=SnapToSockets(actor,otherActor,DeltaTranslate,spline,index,loc,Viewport);
				if(!ret) {
					ret=SnapToSplinePoints(actor,otherActor,DeltaTranslate,spline,index,loc,Viewport);
				}
			}
		}

		if(splineTools && (prevSubSpline || nextSubSpline)) {

			for(int32 i:SelectedKeys) {

				if(i==0) {
					if(prevSubSpline) {
						splineTools->CopySplinePoint(tangT,spline,0,prevSubSpline,prevSubSpline->GetNumberOfSplinePoints()-1,DeltaTranslate,DeltaRotate,DeltaScale);
						prevSubSpline->UpdateSpline();
						FOutputDeviceNull ar;
						prevSubSpline->GetOwner()->CallFunctionByNameWithArguments(TEXT("rdBuildSpline"),ar,NULL,true);
					}
				}
				if(i==numPoints-1) {
					if(nextSubSpline) {
						splineTools->CopySplinePoint(tangT,spline,numPoints-1,nextSubSpline,0,DeltaTranslate,DeltaRotate,DeltaScale);
						nextSubSpline->UpdateSpline();
						FOutputDeviceNull ar;
						nextSubSpline->GetOwner()->CallFunctionByNameWithArguments(TEXT("rdBuildSpline"),ar,NULL,true);
					}
				}
			}
		}
	}

	return FSplineComponentVisualizer::HandleInputDelta(ViewportClient,Viewport,DeltaTranslate,DeltaRotate,DeltaScale);
}

//----------------------------------------------------------------------------------------------------------------
// GenerateContextMenu
//----------------------------------------------------------------------------------------------------------------
TSharedPtr<SWidget> FrdSplineComponentVisualizer::GenerateContextMenu() const {

	if(splineTools) {
		splineTools->actorList.Empty();
		splineTools->splineList.Empty();
#if ENGINE_MAJOR_VERSION>4
		UActorComponent* comp=GetEditedComponent();
#else
		UActorComponent* comp=GetEditedSplineComponent();
#endif
		USplineComponent* splineComp=Cast<USplineComponent>(comp);
		if(splineComp) {
			splineTools->splineList.Add(splineComp);
			splineTools->actorList.Add(splineComp->GetOwner());
		}
	}

	FMenuBuilder MenuBuilder(true,rdSplineComponentVisualizerActions);
						
	GenerateContextMenuSections(MenuBuilder);

	TSharedPtr<SWidget> MenuWidget=MenuBuilder.MakeWidget();
	return MenuWidget;
}

//----------------------------------------------------------------------------------------------------------------
// Menu Entry Macros
//----------------------------------------------------------------------------------------------------------------
#define rdAddVisMenuEntry(l,t,f) InMenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(t,t),FSlateIcon(),FUIAction(FExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f),FCanExecuteAction::CreateLambda([] { return true; })))
#define rdAddVisMenuEntry2(l,t,f1,f2) InMenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(t,t),FSlateIcon(),FUIAction(FExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f1),FCanExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f2)))
#define rdAddVisMenuEntry3(l,t,f1,b1,b2,f2) InMenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(t,t),FSlateIcon(),FUIAction(FExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f1,b1,b2),FCanExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f2)))
#define rdAddVisMenuEntry4(l,t,f1,b,f2) InMenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(t,t),FSlateIcon(),FUIAction(FExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f1,b),FCanExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f2,b)))
#define rdAddVisMenuEntry5(l,t,f1,b,f2) InMenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(t,t),FSlateIcon(),FUIAction(FExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f1,b),FIsActionChecked::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f2,b)))
#define rdAddVisMenuEntry6(l,t,f,s) InMenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(t,t),FSlateIcon(),FUIAction(FExecuteAction::CreateStatic(f,FString(TEXT(s)))))
#define rdAddVisMenuEntry7(l,t,f1,b,f2) InMenuBuilder.AddMenuEntry(LOCTEXT(l,l),LOCTEXT(t,t),FSlateIcon(),FUIAction(FExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f1,b),FCanExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this),f2)))
#define rdAddVisSubMenu(l,t,f) InMenuBuilder.AddSubMenu(LOCTEXT(l,l),LOCTEXT(t,t),FNewMenuDelegate::CreateSP(this,f))
#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
#define rdAddVisSeparator() InMenuBuilder.AddSeparator()
#else
#define rdAddVisSeparator() InMenuBuilder.AddMenuSeparator()
#endif

//----------------------------------------------------------------------------------------------------------------
// GenerateContextMenuSections
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::GenerateContextMenuSections(FMenuBuilder& InMenuBuilder) const {

	InMenuBuilder.BeginSection("SplinePointEdit", LOCTEXT("SplinePoint", "Spline Point"));

	const auto& Commands = FrdSplineToolsMenuCommands::Get();

	USplineComponent* SplineComp=GetEditedSplineComponent();
	if(SplineComp==nullptr) return;

	
#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	check(SelectionState);
	if(SelectionState->GetSelectedSegmentIndex()!=INDEX_NONE) {
#else
	if(SelectedSegmentIndex!=INDEX_NONE) {
#endif
		rdAddVisMenuEntry2("Add Spline Point Here","Add a new spline point at the cursor location.",&FrdSplineComponentVisualizer::OnAddKeyToSegment,&FrdSplineComponentVisualizer::CanAddKeyToSegment);

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	} else if(SelectionState->GetLastKeyIndexSelected()!=INDEX_NONE) {
#else
	} else if(LastKeyIndexSelected!=INDEX_NONE) {
#endif
		rdAddVisMenuEntry2("Delete Spline Point","Delete the currently selected spline point.",&FrdSplineComponentVisualizer::OnDeleteKey,&FrdSplineComponentVisualizer::CanDeleteKey);
		rdAddVisMenuEntry2("Duplicate Spline Point","Duplicate the currently selected spline point.",&FrdSplineComponentVisualizer::OnDuplicateKey,&FrdSplineComponentVisualizer::IsKeySelectionValid);

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>26
		rdAddVisSubMenu("Select Spline Points","Select spline point.",&FrdSplineComponentVisualizer::GenerateSelectSplinePointsSubMenu);
#else
#if ENGINE_MINOR_VERSION>25
		rdAddVisMenuEntry2("Select All Spline Points","Select All Spline Points",&FrdSplineComponentVisualizer::OnSelectAllSplinePoints,&FrdSplineComponentVisualizer::CanSelectSplinePoints);
#else
		rdAddVisMenuEntry2("Select All Spline Points","Select All Spline Points",&FrdSplineComponentVisualizer::OnSelectAllSplinePoints,&FrdSplineComponentVisualizer::CanSelectAllSplinePoints);
#endif
#endif

		rdAddVisSubMenu("Spline Point Type","Define the type of the spline point.",&FrdSplineComponentVisualizer::GenerateSplinePointTypeSubMenu);

		// Only add the Automatic Tangents submenu if any of the keys is a curve type
#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
		const TSet<int32>& SelectedKeys=SelectionState->GetSelectedKeys();
#endif
		for(int32 SelectedKeyIndex:SelectedKeys) {
			check(SelectedKeyIndex>=0);
			check(SelectedKeyIndex<SplineComp->GetNumberOfSplinePoints());
			const auto& Point=SplineComp->SplineCurves.Position.Points[SelectedKeyIndex];
			if(Point.IsCurveKey()) {
				rdAddVisSubMenu("Reset to Automatic Tangent","Reset the spline point tangent to an automatically generated value.",&FrdSplineComponentVisualizer::GenerateTangentTypeSubMenu);
				break;
			}
		}

		InMenuBuilder.AddMenuEntry(
			LOCTEXT("SplineGenerate", "Spline Generation Panel"),
			LOCTEXT("SplineGenerateTooltip", "Opens up a spline generation panel to easily create basic shapes with splines"),
			FSlateIcon(),
			FUIAction( 
				FExecuteAction::CreateSP(const_cast<FrdSplineComponentVisualizer*>(this), &FrdSplineComponentVisualizer::CreateSplineGeneratorPanel),
				FCanExecuteAction::CreateLambda([] { return true; })
			)
		);

	}
	InMenuBuilder.EndSection();

	InMenuBuilder.BeginSection("Transform");
	{
		rdAddVisMenuEntry6("Focus Selected","Moves the camera in front of the selection",&FLevelEditorActionCallbacks::ExecuteExecCommand,"CAMERA ALIGN ACTIVEVIEWPORTONLY");
		rdAddVisSubMenu("Snap and Align","Snap align options.",&FrdSplineComponentVisualizer::GenerateSnapAlignSubMenu);
	}
	InMenuBuilder.EndSection();

	InMenuBuilder.BeginSection("Spline",LOCTEXT("Spline","Spline"));
	{
		rdAddVisMenuEntry2("Reset to Default","Reset this spline to its archetype default.",&FrdSplineComponentVisualizer::OnResetToDefault,&FrdSplineComponentVisualizer::CanResetToDefault);
	}
	InMenuBuilder.EndSection();

	InMenuBuilder.BeginSection("Visualization",LOCTEXT("Visualization","Visualization"));
	{
		rdAddVisMenuEntry2("Visualize Roll and Scale","Whether the visualization should show roll and scale on this spline.",&FrdSplineComponentVisualizer::OnSetVisualizeRollAndScale,&FrdSplineComponentVisualizer::IsVisualizingRollAndScale);
		rdAddVisMenuEntry2("Allow Discontinuous Splines","Whether the visualization allows Arrive and Leave tangents to be set separately.",&FrdSplineComponentVisualizer::OnSetDiscontinuousSpline,&FrdSplineComponentVisualizer::IsDiscontinuousSpline);
	}
	InMenuBuilder.EndSection();

	// rdSplineTools Menu Section
	if(splineTools && splineTools->splineList.Num()>0) {

		USplineComponent* spline=splineTools->splineList[0];
		int32 pnts=spline->GetNumberOfSplinePoints();

		InMenuBuilder.BeginSection("rdSplineTools",LOCTEXT("rdSplineToolsSec1","rdSplineTools"));
		
		int32 numSel=GetSelectedKeys().Num();
		TArray<int32> keys=GetSelectedKeys().Array();
		if(numSel==1) {
			
			int32 key=keys[0];
			if(pnts>1 && key>1 && key<spline->GetNumberOfSplinePoints()-1) {
				rdAddVisMenuEntry("Split Spline Here","Split the Spline at the selected point",&FrdSplineComponentVisualizer::SplitSpline);
				rdAddVisMenuEntry("Split Spline Nearest Section","Split the Spline at the selected point, offset to the nearest section end",&FrdSplineComponentVisualizer::SplitSplineRound);
			}
			rdAddVisMenuEntry("New Spline Here","Create a new Spline at the selected point",&FrdSplineComponentVisualizer::NewSplineHere);

		} else if(numSel>1 && !(numSel==2 && abs(keys[0]-keys[1])==1)) { // at least 2 points selected for a straighten
			
			rdAddVisMenuEntry("Straighten Points","Straightens the selected points in the spline",&FrdSplineComponentVisualizer::StraightenPoints);

			if(pnts>5 && keys[0]>1 && keys[1]<(spline->GetNumberOfSplinePoints()-2)) {
				rdAddVisMenuEntry("Split Spline Section","Split the Spline into 3 around the selected section",&FrdSplineComponentVisualizer::SplitSplineSection);
			}
		}

		if(numSel>1) {

			rdAddVisMenuEntry("Set Height to First (level tangent)","Sets the selected points to the height of the first selected point, leveling all tangents",&FrdSplineComponentVisualizer::SetPointsHeightToFirst);
			rdAddVisMenuEntry("Set Height to Last (level tangent)","Sets the selected points to the height of the last selected point, leveling all tangents",&FrdSplineComponentVisualizer::SetPointsHeightToLast);

		} else if(spline->GetNumberOfSplinePoints()>2) {

			rdAddVisMenuEntry("Set Height of 2 adjacent points","Sets the 2 adjacent points to the height of the selected point, leveling all tangents",&FrdSplineComponentVisualizer::SetPointsHeightAdjacent2);

			if(spline->GetNumberOfSplinePoints()>4) {
				rdAddVisMenuEntry("Set Height of 4 adjacent points","Sets the 4 adjacent points to the height of the selected point, leveling all tangents",&FrdSplineComponentVisualizer::SetPointsHeightAdjacent4);
			}
		}

		rdAddVisMenuEntry("Set Height to Landscape (offset)","Sets the selected points to the height of the Landscape at that point with the applied offset",&FrdSplineComponentVisualizer::SetPointsHeightToLandscape);

		rdAddVisMenuEntry("Snap Point to closest neighbor point","Snaps the SplinePoint to the location of the closest Neighboring Splines point",&FrdSplineComponentVisualizer::SnapPointToClosestNeighbor);

		//if(numSel==1) {
			//rdAddVisMenuEntry("Create Intersection on Left","Turns the selected point into an intersection with the road picked in the blueprint",&FrdSplineComponentVisualizer::CreateLeftIntersection);
			//rdAddVisMenuEntry("Remove Intersection on Left","Removes any existing Left intersection with the road picked in the blueprint",&FrdSplineComponentVisualizer::ClearLeftIntersection);
			//rdAddVisMenuEntry("Create Intersection on Right","Turns the selected point into an intersection with the road picked in the blueprint",&FrdSplineComponentVisualizer::CreateRightIntersection);
			//rdAddVisMenuEntry("Remove Intersection on Right","Removes any existing Right intersection with the road picked in the blueprint",&FrdSplineComponentVisualizer::ClearRightIntersection);
		//}

		rdAddVisMenuEntry("Copy SplineData to Clipboard","Copies the spline point data to the clipboard as text",&FrdSplineComponentVisualizer::CopySplineDataToClipboard);
		rdAddVisMenuEntry("Create SplineData from Clipboard","Creates the spline point data from text in the clipboard",&FrdSplineComponentVisualizer::CreateSplineDataFromClipboard);

		InMenuBuilder.EndSection();
	}
}

//----------------------------------------------------------------------------------------------------------------
// GenerateSelectSplinePointsSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::GenerateSelectSplinePointsSubMenu(FMenuBuilder& InMenuBuilder) const {
#if ENGINE_MAJOR_VERSION>4
	rdAddVisMenuEntry2("Select All Spline Points","Select All Spline Points",&FrdSplineComponentVisualizer::OnSelectAllSplinePoints,&FrdSplineComponentVisualizer::CanSelectSplinePoints);

	rdAddVisMenuEntry3("Select Next Spline Point","Select Next Spline Point",&FrdSplineComponentVisualizer::OnSelectPrevNextSplinePoint,true,false,&FrdSplineComponentVisualizer::CanSelectSplinePoints);
	rdAddVisMenuEntry3("Select Prev Spline Point","Select Prev Spline Point",&FrdSplineComponentVisualizer::OnSelectPrevNextSplinePoint,false,false,&FrdSplineComponentVisualizer::CanSelectSplinePoints);

	rdAddVisMenuEntry3("Add Next Spline Point","Add Next Spline Point",&FrdSplineComponentVisualizer::OnSelectPrevNextSplinePoint,true,true,&FrdSplineComponentVisualizer::CanSelectSplinePoints);
	rdAddVisMenuEntry3("Add Prev Spline Point","Add Prev Spline Point",&FrdSplineComponentVisualizer::OnSelectPrevNextSplinePoint,false,true,&FrdSplineComponentVisualizer::CanSelectSplinePoints);
#endif
}

//----------------------------------------------------------------------------------------------------------------
// GenerateSplinePointTypeSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::GenerateSplinePointTypeSubMenu(FMenuBuilder& InMenuBuilder) const {
	rdAddVisMenuEntry5("Curve","Set spline point to Curve type",&FrdSplineComponentVisualizer::OnSetKeyType,CIM_CurveAuto,&FrdSplineComponentVisualizer::IsKeyTypeSet);
	rdAddVisMenuEntry5("Linear","Set spline point to Linear type",&FrdSplineComponentVisualizer::OnSetKeyType,CIM_Linear,&FrdSplineComponentVisualizer::IsKeyTypeSet);
	rdAddVisMenuEntry5("Constant","Set spline point to Constant type",&FrdSplineComponentVisualizer::OnSetKeyType,CIM_Constant,&FrdSplineComponentVisualizer::IsKeyTypeSet);
}

//----------------------------------------------------------------------------------------------------------------
// GenerateTangentTypeSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::GenerateTangentTypeSubMenu(FMenuBuilder& InMenuBuilder) const {
	rdAddVisMenuEntry4("Unclamped Tangent","Reset the tangent for this spline point to its default unclamped value.",&FrdSplineComponentVisualizer::OnResetToAutomaticTangent,CIM_CurveAuto,&FrdSplineComponentVisualizer::CanResetToAutomaticTangent);
	rdAddVisMenuEntry4("Clamped Tangent","Reset the tangent for this spline point to its default clamped value.",&FrdSplineComponentVisualizer::OnResetToAutomaticTangent,CIM_CurveAutoClamped,&FrdSplineComponentVisualizer::CanResetToAutomaticTangent);
}

//----------------------------------------------------------------------------------------------------------------
// GenerateSnapAlignSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::GenerateSnapAlignSubMenu(FMenuBuilder& InMenuBuilder) const {

	InMenuBuilder.AddMenuEntry(FLevelEditorCommands::Get().SnapToFloor);
	InMenuBuilder.AddMenuEntry(FLevelEditorCommands::Get().AlignToFloor);

#if ENGINE_MAJOR_VERSION>4 
	rdAddVisMenuEntry7("Snap to Nearest Spline Point","Snap selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapKeyToNearestSplinePoint,ESplineComponentSnapMode::Snap,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Align to Nearest Spline Point","Align selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapKeyToNearestSplinePoint,ESplineComponentSnapMode::AlignToTangent,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Align Perpendicular to Nearest Spline Point","Align perpendicular selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapKeyToNearestSplinePoint,ESplineComponentSnapMode::AlignPerpendicularToTangent,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisSeparator();
	rdAddVisMenuEntry7("Snap to Actor","Snap selected spline point to actor, Ctrl-LMB to select the actor after choosing this option.",&FrdSplineComponentVisualizer::OnSnapKeyToActor,ESplineComponentSnapMode::Snap,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Align to Actor","Align selected spline point to actor, Ctrl-LMB to select the actor after choosing this option.",&FrdSplineComponentVisualizer::OnSnapKeyToActor,ESplineComponentSnapMode::AlignToTangent,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Align Perpendicular to Actor","Align perpendicular  selected spline point to actor, Ctrl-LMB to select the actor after choosing this option.",&FrdSplineComponentVisualizer::OnSnapKeyToActor,ESplineComponentSnapMode::AlignPerpendicularToTangent,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisSeparator();
	rdAddVisMenuEntry7("Snap All To Selected X","Snap all spline points to selected spline point world X position.",&FrdSplineComponentVisualizer::OnSnapAllToAxis,EAxis::X,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Snap All To Selected Y","Snap all spline points to selected spline point world Y position.",&FrdSplineComponentVisualizer::OnSnapAllToAxis,EAxis::Y,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Snap All To Selected Z","Snap all spline points to selected spline point world Z position.",&FrdSplineComponentVisualizer::OnSnapAllToAxis,EAxis::Z,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisSeparator();
	rdAddVisMenuEntry7("Snap To Last Selected X","Snap selected spline points to world X position of last selected spline point.",&FrdSplineComponentVisualizer::OnSnapSelectedToAxis,EAxis::X,&FrdSplineComponentVisualizer::AreMultipleKeysSelected);
	rdAddVisMenuEntry7("Snap To Last Selected Y","Snap selected spline points to world Y position of last selected spline point.",&FrdSplineComponentVisualizer::OnSnapSelectedToAxis,EAxis::Y,&FrdSplineComponentVisualizer::AreMultipleKeysSelected);
	rdAddVisMenuEntry7("Snap To Last Selected Z","Snap selected spline points to world Z position of last selected spline point.",&FrdSplineComponentVisualizer::OnSnapSelectedToAxis,EAxis::Z,&FrdSplineComponentVisualizer::AreMultipleKeysSelected);
#else
#if ENGINE_MINOR_VERSION>25
	rdAddVisMenuEntry7("Snap to Nearest Spline Point","Snap selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapKeyToNearestSplinePoint,ESplineComponentSnapMode::Snap,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Align to Nearest Spline Point","Align selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapKeyToNearestSplinePoint,ESplineComponentSnapMode::AlignToTangent,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Align Perpendicular to Nearest Spline Point","Align perpendicular selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapKeyToNearestSplinePoint,ESplineComponentSnapMode::AlignPerpendicularToTangent,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Snap All To Selected X","Snap all spline points to selected spline point world X position.",&FrdSplineComponentVisualizer::OnSnapAllToAxis,EAxis::X,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Snap All To Selected Y","Snap all spline points to selected spline point world Y position.",&FrdSplineComponentVisualizer::OnSnapAllToAxis,EAxis::Y,&FrdSplineComponentVisualizer::IsSingleKeySelected);
	rdAddVisMenuEntry7("Snap All To Selected Z","Snap all spline points to selected spline point world Z position.",&FrdSplineComponentVisualizer::OnSnapAllToAxis,EAxis::Z,&FrdSplineComponentVisualizer::IsSingleKeySelected);
#else
	rdAddVisMenuEntry7("Snap to Nearest Spline Point","Snap selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapToNearestSplinePoint,ESplineComponentSnapMode::Snap,&FrdSplineComponentVisualizer::CanSnapToNearestSplinePoint);
	rdAddVisMenuEntry7("Align to Nearest Spline Point","Align selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapToNearestSplinePoint,ESplineComponentSnapMode::AlignToTangent,&FrdSplineComponentVisualizer::CanSnapToNearestSplinePoint);
	rdAddVisMenuEntry7("Align Perpendicular to Nearest Spline Point","Align perpendicular selected spline point to nearest non-adjacent spline point on current or nearby spline.",&FrdSplineComponentVisualizer::OnSnapToNearestSplinePoint,ESplineComponentSnapMode::AlignPerpendicularToTangent,&FrdSplineComponentVisualizer::CanSnapToNearestSplinePoint);
	rdAddVisMenuEntry7("Snap All To Selected X","Snap all spline points to selected spline point world X position.",&FrdSplineComponentVisualizer::OnSnapAll,EAxis::X,&FrdSplineComponentVisualizer::CanSnapAll);
	rdAddVisMenuEntry7("Snap All To Selected Y","Snap all spline points to selected spline point world Y position.",&FrdSplineComponentVisualizer::OnSnapAll,EAxis::Y,&FrdSplineComponentVisualizer::CanSnapAll);
	rdAddVisMenuEntry7("Snap All To Selected Z","Snap all spline points to selected spline point world Z position.",&FrdSplineComponentVisualizer::OnSnapAll,EAxis::Z,&FrdSplineComponentVisualizer::CanSnapAll);
#endif
#endif
}

//----------------------------------------------------------------------------------------------------------------
// GenerateLockAxisSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::GenerateLockAxisSubMenu(FMenuBuilder& InMenuBuilder) const {
	rdAddVisMenuEntry4("None","New spline point axis is not fixed.",&FrdSplineComponentVisualizer::OnLockAxis,EAxis::None,&FrdSplineComponentVisualizer::IsLockAxisSet);
	rdAddVisMenuEntry4("X","Fix X axis when adding new spline points.",&FrdSplineComponentVisualizer::OnLockAxis,EAxis::X,&FrdSplineComponentVisualizer::IsLockAxisSet);
	rdAddVisMenuEntry4("Y","Fix Y axis when adding new spline points.",&FrdSplineComponentVisualizer::OnLockAxis,EAxis::Y,&FrdSplineComponentVisualizer::IsLockAxisSet);
	rdAddVisMenuEntry4("Z","Fix Z axis when adding new spline points.",&FrdSplineComponentVisualizer::OnLockAxis,EAxis::Z,&FrdSplineComponentVisualizer::IsLockAxisSet);
}

//----------------------------------------------------------------------------------------------------------------
// SplitSpline
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SplitSpline() {
	if(splineTools) splineTools->SplitSpline(this,false);
}

//----------------------------------------------------------------------------------------------------------------
// SplitSplineSection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SplitSplineSection() {
	if(splineTools) splineTools->SplitSplineSection(this);
}

//----------------------------------------------------------------------------------------------------------------
// SplitSplineRound
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SplitSplineRound() {
	if(splineTools) splineTools->SplitSpline(this,true);
}

//----------------------------------------------------------------------------------------------------------------
// NewSplineHere
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::NewSplineHere() {
	if(splineTools) splineTools->NewSplineHere(this);
}

//----------------------------------------------------------------------------------------------------------------
// StraightenPoints
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::StraightenPoints() {
	if(splineTools) splineTools->StraightenPoints(this);
}


//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightToFirst
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SetPointsHeightToFirst() {
	if(splineTools) splineTools->SetPointsHeightToFirst(this);
}

//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightToLast
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SetPointsHeightToLast() {
	if(splineTools) splineTools->SetPointsHeightToLast(this);
}

//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightAdjacent2
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SetPointsHeightAdjacent2() {
	if(splineTools) splineTools->SetPointsHeightAdjacent(this,1);
}

//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightAdjacent4
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SetPointsHeightAdjacent4() {
	if(splineTools) splineTools->SetPointsHeightAdjacent(this,2);
}

//----------------------------------------------------------------------------------------------------------------
// SetPointsHeightToLandscape
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SetPointsHeightToLandscape() {
	if(!splineTools) return;
	FVector offset(0,0,0);
	// showOffsetWindow
	
	splineTools->SetPointsHeightToLandscape(this,offset);
}

//----------------------------------------------------------------------------------------------------------------
// SnapPointToClosestNeighbor
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::SnapPointToClosestNeighbor() {
	if(!splineTools) return;
	splineTools->SnapPointToClosestNeighbor(this);
}

//----------------------------------------------------------------------------------------------------------------
// CreateLeftIntersection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::CreateLeftIntersection() {
	if(!splineTools) return;
	splineTools->CreateLeftIntersection(this);
}

//----------------------------------------------------------------------------------------------------------------
// ClearLeftIntersection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::ClearLeftIntersection() {
	if(!splineTools) return;
	splineTools->ClearLeftIntersection(this);
}

//----------------------------------------------------------------------------------------------------------------
// CreateRightIntersection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::CreateRightIntersection() {
	if(!splineTools) return;
	splineTools->CreateRightIntersection(this);
}

//----------------------------------------------------------------------------------------------------------------
// ClearRightIntersection
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::ClearRightIntersection() {
	if(!splineTools) return;
	splineTools->ClearRightIntersection(this);
}

//----------------------------------------------------------------------------------------------------------------
// CopySplineDataToClipboard
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::CopySplineDataToClipboard() {
	if(splineTools) splineTools->CopySplineDataToClipboard(this);
}

//----------------------------------------------------------------------------------------------------------------
// CreateSplineDataFromClipboard
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::CreateSplineDataFromClipboard() {
	if(splineTools) splineTools->CreateSplineDataFromClipboard(this);
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
