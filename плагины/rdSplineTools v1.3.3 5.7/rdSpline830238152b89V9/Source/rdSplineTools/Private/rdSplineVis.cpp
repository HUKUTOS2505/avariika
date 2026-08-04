//
// rdSplineVis.cpp
//
// Copyright Recourse Design ltd, 2023, Inc. All Rights Reserved.
//
// Version 1.30
//
// Creation Date: 6th January 2025
// Last Modified: 9th January 2025
//
#include "rdSplineTools.h"
#include "Materials/MaterialInstanceDynamic.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"
 
//----------------------------------------------------------------------------------------------------------------
// rdInitVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdInitVis() {

	if(!matVisParent) {
		TSoftObjectPtr<UMaterialInterface> smat(FSoftObjectPath(TEXT("/rdSplineTools/Materials/M_rdSplineVis.M_rdSplineVis")));
		matVisParent=smat.LoadSynchronous();
	}
}
 
//----------------------------------------------------------------------------------------------------------------
// rdAddIntersectionVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdAddIntersectionVis(const FVector& loc,float zrot,float distance,AActor* owner,UrdSplineToolsOptions* opt) {

	if(!visMeshIntersection) return;

	FTransform t(FRotator(0,zrot,0),loc+FVector(0,0,0),FVector(opt->visMeshIntersectionScale/100.0f));
	FLinearColor col=FMath::Lerp(opt->visIntersectionCol2,opt->visIntersectionCol1,FMath::Clamp(distance/(opt->visIntersectionRadius-opt->snapIntersectionRadius),0.0f,1.0f));
	if(distance<opt->snapIntersectionRadius) col.A=opt->visIntersectionSnappedOpacity;
	UStaticMeshComponent** psmc=visIntersectionList.Find(owner);
	if(psmc) {
		(*psmc)->SetWorldTransform(t);
		UMaterialInstanceDynamic* mi=(UMaterialInstanceDynamic*)(*psmc)->GetMaterial(0);
		mi->SetVectorParameterValue(TEXT("VisColor"),col);
		return;
	}
	
	UStaticMeshComponent* smc=(UStaticMeshComponent*)owner->AddComponentByClass(UStaticMeshComponent::StaticClass(),false,t,false);
	smc->SetStaticMesh(visMeshIntersection);
	UMaterialInstanceDynamic* mi=UMaterialInstanceDynamic::Create(matVisParent,nullptr);
	mi->SetVectorParameterValue(TEXT("VisColor"),col);
	visMeshIntersection->SetMaterial(0,mi);
	visMeshIntersection->GetOutermost()->SetDirtyFlag(false);
	smc->bSelectable=false;
	smc->CastShadow=false;
	visIntersectionList.Add(owner,smc);
}
 
//----------------------------------------------------------------------------------------------------------------
// rdRemIntersectionVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdRemIntersectionVis(AActor* owner) {

	UStaticMeshComponent** smc=visIntersectionList.Find(owner);
	if(smc) {
		(*smc)->DestroyComponent();
	}
	visIntersectionList.Remove(owner);
}
 
//----------------------------------------------------------------------------------------------------------------
// rdAddSocketVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdAddSocketVis(const FVector& loc,float zrot,float distance,AActor* owner,UrdSplineToolsOptions* opt) {

	if(!visMeshSocket) return;

	FTransform t(FRotator(0,zrot,0),loc+FVector(0,0,0),FVector(opt->visMeshSocketScale/100.0f));
	FLinearColor col=FMath::Lerp(opt->visSocketCol2,opt->visSocketCol1,FMath::Clamp(distance/(opt->visSocketRadius-opt->snapSocketRadius),0.0f,1.0f));
	if(distance<opt->snapSocketRadius) col.A=opt->visSocketSnappedOpacity;
	UStaticMeshComponent** psmc=visSocketList.Find(owner);
	if(psmc) {
		(*psmc)->SetWorldTransform(t);
		UMaterialInstanceDynamic* mi=(UMaterialInstanceDynamic*)(*psmc)->GetMaterial(0);
		mi->SetVectorParameterValue(TEXT("VisColor"),col);
		return;
	}
	
	UStaticMeshComponent* smc=(UStaticMeshComponent*)owner->AddComponentByClass(UStaticMeshComponent::StaticClass(),false,t,false);
	smc->SetStaticMesh(visMeshSocket);
	UMaterialInstanceDynamic* mi=UMaterialInstanceDynamic::Create(matVisParent,nullptr);
	mi->SetVectorParameterValue(TEXT("VisColor"),col);
	visMeshSocket->SetMaterial(0,mi);
	visMeshSocket->GetOutermost()->SetDirtyFlag(false);
	smc->bSelectable=false;
	smc->CastShadow=false;
	visSocketList.Add(owner,smc);
}
 
//----------------------------------------------------------------------------------------------------------------
// rdRemSocketVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdRemSocketVis(AActor* owner) {

	UStaticMeshComponent** smc=visSocketList.Find(owner);
	if(smc) {
		(*smc)->DestroyComponent();
	}
	visSocketList.Remove(owner);
}
 
//----------------------------------------------------------------------------------------------------------------
// rdAddSplinePointVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdAddSplinePointVis(const FVector& loc,float zrot,float distance,AActor* owner,UrdSplineToolsOptions* opt) {

	if(!visMeshSplinePoint) return;

	FTransform t(FRotator(0,zrot,0),loc+FVector(0,0,0),FVector(opt->visMeshSplinePointScale/100.0f));
	FLinearColor col=FMath::Lerp(opt->visSplinePointCol2,opt->visSplinePointCol1,FMath::Clamp(distance/(opt->visSplinePointRadius-opt->snapSplinePointRadius),0.0f,1.0f));
	if(distance<opt->snapSplinePointRadius) col.A=opt->visSplinePointSnappedOpacity;
	UStaticMeshComponent** psmc=visSplinePointList.Find(owner);
	if(psmc) {
		(*psmc)->SetWorldTransform(t);
		UMaterialInstanceDynamic* mi=(UMaterialInstanceDynamic*)(*psmc)->GetMaterial(0);
		mi->SetVectorParameterValue(TEXT("VisColor"),col);
		return;
	}
	
	UStaticMeshComponent* smc=(UStaticMeshComponent*)owner->AddComponentByClass(UStaticMeshComponent::StaticClass(),false,t,false);
	smc->SetStaticMesh(visMeshSplinePoint);
	UMaterialInstanceDynamic* mi=UMaterialInstanceDynamic::Create(matVisParent,nullptr);
	mi->SetVectorParameterValue(TEXT("VisColor"),col);
	visMeshSplinePoint->SetMaterial(0,mi);
	visMeshSplinePoint->GetOutermost()->SetDirtyFlag(false);
	smc->bSelectable=false;
	smc->CastShadow=false;
	visSplinePointList.Add(owner,smc);
}
 
//----------------------------------------------------------------------------------------------------------------
// rdRemSplinePointVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdRemSplinePointVis(AActor* owner) {

	UStaticMeshComponent** smc=visSplinePointList.Find(owner);
	if(smc) {
		(*smc)->DestroyComponent();
	}
	visSplinePointList.Remove(owner);
}
 
//----------------------------------------------------------------------------------------------------------------
// rdRemAllIntersectionVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdRemAllIntersectionVis() {

	for(auto& it:visIntersectionList) {
		it.Value->DestroyComponent();
	}
	visIntersectionList.Empty();
}

//----------------------------------------------------------------------------------------------------------------
// rdRemAllSocketVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdRemAllSocketVis() {

	for(auto& it:visSocketList) {
		it.Value->DestroyComponent();
	}
	visSocketList.Empty();
}

//----------------------------------------------------------------------------------------------------------------
// rdRemAllSplinePointVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdRemAllSplinePointVis() {

	for(auto& it:visSplinePointList) {
		it.Value->DestroyComponent();
	}
	visSplinePointList.Empty();
}

//----------------------------------------------------------------------------------------------------------------
// rdRemAllVis
//----------------------------------------------------------------------------------------------------------------
void FrdSplineComponentVisualizer::rdRemAllVis() {

	for(auto& it:visIntersectionList) {
		it.Value->DestroyComponent();
	}
	visIntersectionList.Empty();
	for(auto& it:visSocketList) {
		it.Value->DestroyComponent();
	}
	visSocketList.Empty();
	for(auto& it:visSplinePointList) {
		it.Value->DestroyComponent();
	}
	visSplinePointList.Empty();
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
