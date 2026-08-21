#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WorkerAppearanceComponent.h"
#include "AvCustomizePreviewActor.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UDirectionalLightComponent;
class UPointLightComponent;
class URectLightComponent;
class USkyLightComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UENUM()
enum class EAvCustomizationCameraPreset : uint8
{
	FullBody,
	UpperBody,
	Head,
	Face,
	Hands,
	LowerBody
};

enum class EAvPreviewZoomAnchorSource : uint8
{
	None,
	MeshHit,
	FocusPlaneFallback
};

#if WITH_EDITOR || !UE_BUILD_SHIPPING
struct FAvAppearanceInspectorSlotInfo
{
	EWorkerSlot Slot = EWorkerSlot::Body;
	bool bPresent = false;
	bool bVisible = false;
	FString MeshName;
	FString MeshObjectPath;
	FString MeshPackagePath;
	TArray<FString> MaterialAssetPaths;
	TArray<FString> RuntimeMaterialPaths;
	int32 MaterialSlotCount = 0;
	int32 ForcedLodModel = 0;
	int32 PredictedLodLevel = INDEX_NONE;
};
#endif

UCLASS()
class AVARYO_API AAvCustomizePreviewActor : public AActor
{
	GENERATED_BODY()

public:
	AAvCustomizePreviewActor();

	void InitializePreview(UTextureRenderTarget2D* InRenderTarget);
	void SetPreviewAppearance(const FWorkerAppearance& InAppearance);
	void SetPreviewSlotColor(EWorkerSlot Slot, const FLinearColor& Color);

	UFUNCTION(BlueprintCallable, Category="AvCustomize")
	void SetPreviewYaw(float InYawDegrees);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	/** Development-only deterministic portrait source for prepared UI thumbnail generation. */
	UFUNCTION(BlueprintCallable, Category="AvCustomize|ThumbnailCapture")
	void SetThumbnailCapturePreview(
		const FString& HeadObjectPath,
		const FString& HeadgearObjectPath,
		const FLinearColor& SkinColor);

	/** Development-only exact-slot portrait composition used by the real thumbnail pipeline. */
	UFUNCTION(BlueprintCallable, Category="AvCustomize|ThumbnailCapture")
	void SetThumbnailCaptureItemPreview(
		const FString& HeadObjectPath,
		EWorkerSlot ItemSlot,
		const FString& ItemObjectPath,
		const FLinearColor& SkinColor);

	/** Read a 512x512 color render target and write an opaque PNG without alpha premultiplication. */
	UFUNCTION(BlueprintCallable, Category="AvCustomize|ThumbnailCapture")
	bool ExportThumbnailRenderTargetOpaquePng(
		UTextureRenderTarget2D* RenderTarget,
		const FString& AbsoluteFilename,
		FString& OutError) const;

	UFUNCTION(BlueprintCallable, Category="AvCustomize|ThumbnailCapture")
	void SetThumbnailCaptureMode(bool bEnabled);

	void AppearanceInspectorToggleSlot(EWorkerSlot Slot);
	void AppearanceInspectorSoloSlot(EWorkerSlot Slot);
	void AppearanceInspectorShowAll();
	void AppearanceInspectorHideAllClothing();
	void AppearanceInspectorBodyOnly();
	void AppearanceInspectorHideBody();
	void AppearanceInspectorShowBody();
	void AppearanceInspectorClearSolo();
	void AppearanceInspectorSetForceLod0(bool bForceLod0);
	FAvAppearanceInspectorSlotInfo GetAppearanceInspectorSlotInfo(EWorkerSlot Slot) const;
	bool IsAppearanceInspectorForceLod0() const { return bAppearanceInspectorForceLod0; }
	TOptional<EWorkerSlot> GetAppearanceInspectorSoloSlot() const { return DebugSoloSlot; }
#endif

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	void TransitionToCustomizationCameraPreset(
		EAvCustomizationCameraPreset InPreset,
		bool bImmediate = false,
		FName ResetReason = NAME_None);
	void RefreshCustomizationCameraPresetForViewport(bool bImmediate = true);
	void SetCompositionCenterNormalizedX(float InCenterNormalizedX);
	bool AddPreviewZoom(
		float WheelDelta,
		const FVector2D& CursorScreenPosition,
		const FVector2D& CursorNormalizedPosition);
	EAvCustomizationCameraPreset GetCameraPreset() const { return CameraPreset; }
	float GetCameraBaseDistance() const { return CurrentPresetBaseDistance; }
	float GetCameraTargetDistance() const { return TargetCameraDistance; }
	float GetCameraCurrentDistance() const { return CurrentCameraDistance; }
	float GetCameraMinDistance() const { return CurrentPresetMinDistance; }
	float GetCameraZoomAlpha() const { return ZoomAlpha; }
	const FVector& GetCameraBaseFocus() const { return CurrentPresetBaseFocus; }
	const FVector& GetCameraCurrentFocus() const { return CurrentCameraFocus; }
	const FVector& GetCameraTargetFocus() const { return TargetCameraFocus; }
	float GetCameraFocusZ() const { return CurrentCameraFocus.Z; }
	float GetPreviewYaw() const { return PreviewYawDegrees; }
	bool IsCameraTransitionActive() const { return bCameraTransitionActive; }
	bool IsCategoryTransitionActive() const { return bCategoryTransitionActive; }
	bool IsManualZoomActive() const { return bManualZoomActive; }
	const FVector2D& GetLastCursorScreenPosition() const { return LastCursorScreenPosition; }
	const FVector& GetLastCursorWorldAnchor() const { return LastCursorWorldAnchor; }
	EAvPreviewZoomAnchorSource GetLastAnchorSource() const { return LastAnchorSource; }
	bool WasFocusClampedToBounds() const { return bFocusClampedToBounds; }
	bool WasLastWheelHandled() const { return bLastWheelHandled; }
	bool IsAutoFitActive() const { return false; }
	bool IsScreenSpaceCharacterClampActive() const { return false; }
	bool GetCombinedVisibleBounds(FBox& OutLocalBounds) const;
	bool GetProjectedVisibleBounds(FBox2D& OutNormalizedBounds) const;
	float GetSceneCaptureFOV() const;
	float GetSceneCaptureAspectRatio() const;
	bool IsSceneCaptureAspectConstrained() const { return false; }
	bool IsSceneCaptureUsingCustomProjection() const;
	bool IsCameraAtBaseStateExact() const;
	FName GetLastCameraResetReason() const { return LastCameraResetReason; }
	float GetCompositionCenterNormalizedX() const
	{
		return CompositionCenterNormalizedX;
	}

private:
	void ApplyIdleAnimation();
	void ApplyPreviewYaw();
	void ConfigurePreviewMeshes();
	void ConfigurePreviewMesh(USkeletalMeshComponent* Mesh) const;
	void RefreshPreviewMeshBounds() const;
	bool GetPreviewLocalBounds(FBox& OutLocalBounds) const;
	bool BuildPreviewCursorRay(
		const FVector2D& CursorNormalizedPosition,
		FVector& OutRayOrigin,
		FVector& OutRayDirection) const;
	bool ResolveCursorZoomAnchor(
		const FVector& RayOrigin,
		const FVector& RayDirection,
		FVector& OutWorldAnchor,
		EAvPreviewZoomAnchorSource& OutSource,
		bool& bOutClampedToBounds) const;
	float ResolvePresetBaseDistance(
		EAvCustomizationCameraPreset InPreset,
		float AuthoredBaseDistance,
		float MinDistance,
		const FVector& BaseFocus) const;
	void ResetPreviewCameraToCurrentPresetBase(
		bool bImmediate,
		bool bCategoryTransition,
		FName ResetReason);
	void ClearManualZoomState();
	void RefreshCaptureShowOnly();
	void UpdateCaptureFraming();

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	void EnsureThumbnailBackdropMaterial();
	void ApplyAppearanceInspectorState();
	void RefreshAppearanceInspectorCapture();
#endif

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<UWorkerAppearanceComponent> AppearanceComponent;

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<UDirectionalLightComponent> KeyLight;

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<UPointLightComponent> FillLight;

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<URectLightComponent> SoftKeyLight;

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<URectLightComponent> SoftFillLight;

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<URectLightComponent> RimLight;

	UPROPERTY(VisibleAnywhere, Category="AvCustomize")
	TObjectPtr<USkyLightComponent> AmbientLight;

	/** Opaque, shared portrait background rendered only by prepared thumbnail captures. */
	UPROPERTY(VisibleAnywhere, Category="AvCustomize|ThumbnailCapture")
	TObjectPtr<UStaticMeshComponent> ThumbnailBackdrop;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ThumbnailBackdropMaterial;

	float PreviewYawDegrees = 0.f;
	float CompositionCenterNormalizedX = 0.5f;
	EAvCustomizationCameraPreset CameraPreset = EAvCustomizationCameraPreset::FullBody;
	FVector CurrentPresetBaseFocus = FVector(0.f, 0.f, 94.f);
	FVector CurrentCameraFocus = FVector(0.f, 0.f, 94.f);
	FVector TargetCameraFocus = FVector(0.f, 0.f, 94.f);
	FVector LastCursorWorldAnchor = FVector::ZeroVector;
	FVector2D LastCursorScreenPosition = FVector2D::ZeroVector;
	float CurrentPresetBaseDistance = 445.f;
	float CurrentPresetMinDistance = 115.f;
	float CurrentCameraDistance = 445.f;
	float TargetCameraDistance = 445.f;
	float ZoomAlpha = 0.f;
	bool bCameraTransitionActive = false;
	bool bCategoryTransitionActive = false;
	bool bManualZoomActive = false;
	bool bFocusClampedToBounds = false;
	bool bLastWheelHandled = false;
	EAvPreviewZoomAnchorSource LastAnchorSource = EAvPreviewZoomAnchorSource::None;
	FName LastCameraResetReason = NAME_None;

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	TSet<EWorkerSlot> DebugHiddenSlots;
	TOptional<EWorkerSlot> DebugSoloSlot;
	bool bDebugHideBody = false;
	bool bAppearanceInspectorForceLod0 = true;
	bool bThumbnailCaptureMode = false;
	EWorkerSlot ThumbnailCaptureItemSlot = EWorkerSlot::Head;
	bool bHasStoredPreThumbnailPostProcess = false;
	bool bPreThumbnailVignetteOverride = false;
	float PreThumbnailVignetteIntensity = 0.f;
	FLinearColor PreThumbnailClearColor = FLinearColor::Transparent;
#endif
};
