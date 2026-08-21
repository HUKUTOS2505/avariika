// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SplineComponentVisualizer.h"

class FrdSplineToolsModule;
class USplineComponent;
class UrdSplineToolsOptions;

UENUM()
enum rdSnapType {
	RDSNAP_NONE		UMETA(DisplayName="None"),
	RDSNAP_ALWAYS	UMETA(DisplayName="Always"),
	RDSNAP_WITHKEY	UMETA(DisplayName="With Hotkey"),
};

class FrdSplineComponentVisualizer : public FSplineComponentVisualizer {
public:
					FrdSplineComponentVisualizer();
					FrdSplineComponentVisualizer(FrdSplineToolsModule* mod);
	virtual			~FrdSplineComponentVisualizer();

	virtual void	OnRegister() override;
	virtual TSharedPtr<SWidget> GenerateContextMenu() const override;
	virtual bool	HandleInputDelta(FEditorViewportClient* ViewportClient,FViewport* Viewport,FVector& DeltaTranslate,FRotator& DeltaRotate,FVector& DeltaScale) override;

	virtual bool	VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	virtual void	EndEditing() override;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	virtual void	TrackingStopped(FEditorViewportClient* InViewportClient, bool bInDidMove) override;
#endif
	virtual bool  	HandleBoxSelect(const FBox& InBox,FEditorViewportClient* InViewportClient,FViewport* InViewport) override;
	virtual bool	HandleFrustumSelect(const FConvexVolume& InFrustum, FEditorViewportClient* InViewportClient, FViewport* InViewport) override;
	void			SetupSplinePtrs();

	void			GenerateContextMenuSections(FMenuBuilder& InMenuBuilder) const;
	void			GenerateSelectSplinePointsSubMenu(FMenuBuilder& MenuBuilder) const;
	void			GenerateSplinePointTypeSubMenu(FMenuBuilder& MenuBuilder) const;
	void			GenerateTangentTypeSubMenu(FMenuBuilder& MenuBuilder) const;
	void			GenerateSnapAlignSubMenu(FMenuBuilder& MenuBuilder) const;
	void			GenerateLockAxisSubMenu(FMenuBuilder& MenuBuilder) const;

	void			RegisterCommands(TSharedPtr<class FUICommandList> rdLevelEditorCommands);

	void			SplitSpline();
	void			SplitSplineSection();
	void			SplitSplineRound();
	void			NewSplineHere();
	void			StraightenPoints();

	void			SetPointsHeightToFirst();
	void			SetPointsHeightToLast();
	void			SetPointsHeightAdjacent2();
	void			SetPointsHeightAdjacent4();
	void			SetPointsHeightToLandscape();

	void			SnapPointToClosestNeighbor();

	void			CreateLeftIntersection();
	void			ClearLeftIntersection();
	void			CreateRightIntersection();
	void			ClearRightIntersection();

	void			CopySplineDataToClipboard();
	void			CreateSplineDataFromClipboard();

	bool			SnapToIntersections(AActor* actor,AActor* otherActor,FVector& DeltaTranslate,USplineComponent* spline,int32 index,FVector loc,FViewport* Viewport);
	bool			SnapToSockets(AActor* actor,AActor* otherActor,FVector& DeltaTranslate,USplineComponent* spline,int32 index,FVector loc,FViewport* Viewport);
	bool			SnapToSplinePoints(AActor* actor,AActor* otherActor,FVector& DeltaTranslate,USplineComponent* spline,int32 index,FVector loc,FViewport* Viewport);

	TSharedPtr<FUICommandList>			rdSplineComponentVisualizerActions=nullptr;
	FrdSplineToolsModule*				splineTools=nullptr;
	TWeakObjectPtr<UrdSplineToolsOptions>	rdSplineToolsoptions=nullptr;
	
private:
	USplineComponent*					prevSubSpline=nullptr;
	USplineComponent*					nextSubSpline=nullptr;

	USplineComponent*					lastSpline=nullptr;
	TArray<AActor*>						landscapes;
public:
	void					rdInitVis();
	void					rdAddIntersectionVis(const FVector& loc,float zrot,float distance,AActor* owner,UrdSplineToolsOptions* opt);
	void					rdRemIntersectionVis(AActor* owner);
	void					rdAddSocketVis(const FVector& loc,float zrot,float distance,AActor* owner,UrdSplineToolsOptions* opt);
	void					rdRemSocketVis(AActor* owner);
	void					rdAddSplinePointVis(const FVector& loc,float zrot,float distance,AActor* owner,UrdSplineToolsOptions* opt);
	void					rdRemSplinePointVis(AActor* owner);
	void					rdRemAllIntersectionVis();
	void					rdRemAllSocketVis();
	void					rdRemAllSplinePointVis();
	void					rdRemAllVis();

	UMaterialInterface*					matVisParent=nullptr;

	UStaticMesh*						visMeshIntersection=nullptr;
	UStaticMesh*						visMeshSocket=nullptr;
	UStaticMesh*						visMeshSplinePoint=nullptr;

	FVector								originalTangent;
	bool								originalTangentSet=false;

private:
	TMap<AActor*,UStaticMeshComponent*>	visIntersectionList;
	TMap<AActor*,UStaticMeshComponent*>	visSocketList;
	TMap<AActor*,UStaticMeshComponent*>	visSplinePointList;

	FTimerHandle			splineVisTimerHandle;
public:
	void					doFullDraw();
	AActor*					drawActor=nullptr;
};
