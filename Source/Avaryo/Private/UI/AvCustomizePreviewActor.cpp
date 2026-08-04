#include "UI/AvCustomizePreviewActor.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "EngineUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "ImageUtils.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	constexpr float PreviewCaptureFovDegrees = 25.f;
	constexpr float PreviewCanonicalCameraDistance = 445.f;
	constexpr float PreviewFullBodyTargetHeightRatio = 0.858f;
	constexpr float PreviewZoomStep = 32.f;
	constexpr float PreviewCameraInterpSpeed = 9.f;
	constexpr float PreviewFocusBoundsExpansion = 8.f;
	constexpr float PreviewCursorTraceDistance = 2000.f;
	constexpr float PreviewTexturePrestreamSeconds = 600.f;
	constexpr float PreviewStreamingDistanceMultiplier = 20.f;
	const FVector PreviewCanonicalFocus(0.f, 0.f, 94.f);
	const FLinearColor ThumbnailBackdropClearColor(0.028f, 0.045f, 0.065f, 1.f);
	const FLinearColor ThumbnailBackdropMaterialColor(0.10f, 0.11f, 0.13f, 1.f);

	struct FThumbnailLightingProfile
	{
		float ExposureBias = 0.95f;
		float KeyIntensity = 2.65f * 8.f;
		float FillIntensity = 1500.f * 8.f;
		float SoftKeyIntensity = 9000.f * 8.f;
		float SoftFillIntensity = 3200.f * 8.f;
		float RimIntensity = 1800.f * 8.f;
		float AmbientIntensity = 0.55f;
		FRotator KeyRotation = FRotator(-32.f, -34.f, 0.f);
		FVector FillOffset = FVector(250.f, -190.f, 75.f);
		FVector SoftKeyOffset = FVector(250.f, -270.f, 185.f);
		FVector SoftKeyTargetOffset = FVector(0.f, 0.f, 55.f);
		FVector SoftFillOffset = FVector(240.f, 300.f, 125.f);
		FVector SoftFillTargetOffset = FVector(0.f, 0.f, 45.f);
		FVector RimOffset = FVector(-210.f, 160.f, 165.f);
		FVector RimTargetOffset = FVector(0.f, 0.f, 75.f);
	};

	struct FThumbnailCameraProfile
	{
		const TCHAR* Name = TEXT("StandardHead");
		float FovDegrees = 28.f;
		float CameraDistance = 112.f;
		FVector Focus = FVector(0.f, 0.f, 156.f);
	};

	const FThumbnailLightingProfile CommonThumbnailLightingProfile;

	struct FPreviewCameraProfile
	{
		float BaseDistance;
		float MinDistance;
		FVector BaseFocus;
	};

	FPreviewCameraProfile GetPreviewCameraProfile(EAvCustomizationCameraPreset Preset)
	{
		switch (Preset)
		{
		case EAvCustomizationCameraPreset::UpperBody:
			return { 260.f, 95.f, FVector(0.f, 0.f, 119.f) };
		case EAvCustomizationCameraPreset::Head:
			return { 170.f, 85.f, FVector(0.f, 0.f, 151.f) };
		case EAvCustomizationCameraPreset::Face:
			return { 125.f, 80.f, FVector(0.f, 0.f, 158.f) };
		case EAvCustomizationCameraPreset::Hands:
			return { 235.f, 95.f, FVector(0.f, 0.f, 108.f) };
		case EAvCustomizationCameraPreset::LowerBody:
			return { 300.f, 100.f, FVector(0.f, 0.f, 64.f) };
		case EAvCustomizationCameraPreset::FullBody:
		default:
			return { PreviewCanonicalCameraDistance, 115.f, PreviewCanonicalFocus };
		}
	}

	FThumbnailCameraProfile GetCategoryCameraProfile(EWorkerSlot Slot)
	{
		if (Slot == EWorkerSlot::Beard)
		{
			return {
				TEXT("BeardCamera"),
				27.f,
				92.f,
				FVector(0.f, 0.f, 156.5f)
			};
		}
		if (Slot == EWorkerSlot::Gloves)
		{
			return { TEXT("GlovesCamera"), 34.f, 150.f, FVector(0.f, 0.f, 105.f) };
		}
		if (Slot == EWorkerSlot::Watch)
		{
			return { TEXT("WatchCamera"), 34.f, 150.f, FVector(0.f, 0.f, 105.f) };
		}
		if (Slot == EWorkerSlot::Torso)
		{
			return { TEXT("TorsoCamera"), 34.f, 180.f, FVector(0.f, 0.f, 125.f) };
		}
		if (Slot == EWorkerSlot::Legs)
		{
			return { TEXT("LegsCamera"), 34.f, 235.f, FVector(0.f, 0.f, 64.f) };
		}
		if (Slot == EWorkerSlot::HipAccessory)
		{
			return { TEXT("HipCamera"), 34.f, 165.f, FVector(0.f, 0.f, 88.f) };
		}
		if (Slot == EWorkerSlot::FullOutfit || Slot == EWorkerSlot::Overalls)
		{
			return { TEXT("FullBodyEnsembleCamera"), 34.f, 305.f, FVector(0.f, 0.f, 88.f) };
		}

		return FThumbnailCameraProfile();
	}

	void AddBoxCorners(const FBox& WorldBox, const FTransform& ActorTransform, FBox& OutLocalBox)
	{
		const FVector Min = WorldBox.Min;
		const FVector Max = WorldBox.Max;
		const FVector Corners[] =
		{
			FVector(Min.X, Min.Y, Min.Z),
			FVector(Min.X, Min.Y, Max.Z),
			FVector(Min.X, Max.Y, Min.Z),
			FVector(Min.X, Max.Y, Max.Z),
			FVector(Max.X, Min.Y, Min.Z),
			FVector(Max.X, Min.Y, Max.Z),
			FVector(Max.X, Max.Y, Min.Z),
			FVector(Max.X, Max.Y, Max.Z)
		};

		for (const FVector& Corner : Corners)
		{
			OutLocalBox += ActorTransform.InverseTransformPosition(Corner);
		}
	}

	bool ProjectLocalBoundsToNormalizedViewport(
		const FBox& LocalBounds,
		const FVector& CameraLocal,
		const FVector& CameraForward,
		const FVector& CameraRight,
		const FVector& CameraUp,
		float HorizontalFovDegrees,
		float AspectRatio,
		FBox2D& OutBounds)
	{
		if (!LocalBounds.IsValid || AspectRatio <= KINDA_SMALL_NUMBER ||
			HorizontalFovDegrees <= KINDA_SMALL_NUMBER)
		{
			return false;
		}

		const float TanHalfHorizontalFov = FMath::Tan(
			FMath::DegreesToRadians(HorizontalFovDegrees * 0.5f));
		const float TanHalfVerticalFov = TanHalfHorizontalFov / AspectRatio;
		if (TanHalfHorizontalFov <= KINDA_SMALL_NUMBER ||
			TanHalfVerticalFov <= KINDA_SMALL_NUMBER)
		{
			return false;
		}

		OutBounds = FBox2D(ForceInit);
		const FVector Min = LocalBounds.Min;
		const FVector Max = LocalBounds.Max;
		for (int32 XIndex = 0; XIndex < 2; ++XIndex)
		{
			for (int32 YIndex = 0; YIndex < 2; ++YIndex)
			{
				for (int32 ZIndex = 0; ZIndex < 2; ++ZIndex)
				{
					const FVector Corner(
						XIndex == 0 ? Min.X : Max.X,
						YIndex == 0 ? Min.Y : Max.Y,
						ZIndex == 0 ? Min.Z : Max.Z);
					const FVector ToCorner = Corner - CameraLocal;
					const float Depth = FVector::DotProduct(ToCorner, CameraForward);
					if (Depth <= KINDA_SMALL_NUMBER)
					{
						return false;
					}

					const float NormalizedX = 0.5f + 0.5f *
						FVector::DotProduct(ToCorner, CameraRight) /
						(Depth * TanHalfHorizontalFov);
					const float NormalizedY = 0.5f - 0.5f *
						FVector::DotProduct(ToCorner, CameraUp) /
						(Depth * TanHalfVerticalFov);
					OutBounds += FVector2D(NormalizedX, NormalizedY);
				}
			}
		}
		return OutBounds.bIsValid;
	}

	bool ProjectLocalBoundsAtPresetDistance(
		const FBox& LocalBounds,
		const FVector& Focus,
		float CameraDistance,
		float HorizontalFovDegrees,
		float AspectRatio,
		FBox2D& OutBounds)
	{
		const FVector ViewDirection = FVector(-1.f, 0.08f, -0.01f).GetSafeNormal();
		const FVector CameraLocal = Focus - ViewDirection * CameraDistance;
		const FRotationMatrix CameraRotation(ViewDirection.Rotation());
		return ProjectLocalBoundsToNormalizedViewport(
			LocalBounds,
			CameraLocal,
			CameraRotation.GetUnitAxis(EAxis::X),
			CameraRotation.GetUnitAxis(EAxis::Y),
			CameraRotation.GetUnitAxis(EAxis::Z),
			HorizontalFovDegrees,
			AspectRatio,
			OutBounds);
	}

}

AAvCustomizePreviewActor::AAvCustomizePreviewActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	AppearanceComponent = CreateDefaultSubobject<UWorkerAppearanceComponent>(TEXT("PreviewAppearance"));

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(SceneRoot);
	SceneCapture->bCaptureEveryFrame = true;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->bAlwaysPersistRenderingState = false;
	SceneCapture->bIgnoreScreenPercentage = true;
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	SceneCapture->CompositeMode = ESceneCaptureCompositeMode::SCCM_Overwrite;
	SceneCapture->FOVAngle = PreviewCaptureFovDegrees;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCapture->ShowFlags.SetAntiAliasing(true);
	SceneCapture->ShowFlags.SetTemporalAA(false);
	SceneCapture->ShowFlags.SetMotionBlur(false);
	SceneCapture->ShowFlags.SetDepthOfField(false);
	SceneCapture->ShowFlags.SetScreenPercentage(false);
	SceneCapture->ShowFlags.SetEyeAdaptation(false);
	SceneCapture->ShowFlags.SetBloom(false);
	SceneCapture->ShowFlags.SetAtmosphere(false);
	SceneCapture->ShowFlags.SetFog(false);
	SceneCapture->ShowFlags.SetVolumetricFog(false);
	SceneCapture->PostProcessBlendWeight = 1.f;
	SceneCapture->PostProcessSettings.bOverride_AutoExposureMethod = true;
	SceneCapture->PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
	SceneCapture->PostProcessSettings.bOverride_AutoExposureBias = true;
	SceneCapture->PostProcessSettings.AutoExposureBias = 0.35f;
	SceneCapture->PostProcessSettings.bOverride_AutoExposureApplyPhysicalCameraExposure = true;
	SceneCapture->PostProcessSettings.AutoExposureApplyPhysicalCameraExposure = false;
	SceneCapture->PostProcessSettings.bOverride_BloomIntensity = true;
	SceneCapture->PostProcessSettings.BloomIntensity = 0.f;
	SceneCapture->PostProcessSettings.bOverride_ColorSaturation = true;
	SceneCapture->PostProcessSettings.ColorSaturation = FVector4(1.10f, 1.08f, 1.04f, 1.f);
	SceneCapture->PostProcessSettings.bOverride_ColorContrast = true;
	SceneCapture->PostProcessSettings.ColorContrast = FVector4(1.04f, 1.04f, 1.03f, 1.f);
	SceneCapture->PostProcessSettings.bOverride_ColorGamma = true;
	SceneCapture->PostProcessSettings.ColorGamma = FVector4(0.96f, 0.97f, 0.99f, 1.f);

	KeyLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("KeyLight"));
	KeyLight->SetupAttachment(SceneRoot);
	KeyLight->SetMobility(EComponentMobility::Movable);
	KeyLight->SetIntensity(2.65f);
	KeyLight->SetLightColor(FLinearColor(1.f, 0.90f, 0.78f));
	KeyLight->SetCastShadows(false);

	FillLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FillLight"));
	FillLight->SetupAttachment(SceneRoot);
	FillLight->SetMobility(EComponentMobility::Movable);
	FillLight->SetIntensity(1500.f);
	FillLight->SetAttenuationRadius(700.f);
	FillLight->SetLightColor(FLinearColor(1.f, 0.88f, 0.74f));
	FillLight->SetRelativeLocation(FVector(220.f, -130.f, 135.f));
	FillLight->SetCastShadows(false);

	SoftKeyLight = CreateDefaultSubobject<URectLightComponent>(TEXT("SoftKeyLight"));
	SoftKeyLight->SetupAttachment(SceneRoot);
	SoftKeyLight->SetMobility(EComponentMobility::Movable);
	SoftKeyLight->SetIntensity(9000.f);
	SoftKeyLight->SetSourceWidth(240.f);
	SoftKeyLight->SetSourceHeight(320.f);
	SoftKeyLight->SetAttenuationRadius(900.f);
	SoftKeyLight->SetLightColor(FLinearColor(1.f, 0.88f, 0.72f));
	SoftKeyLight->SetCastShadows(false);
	SoftKeyLight->SetShadowBias(0.8f);

	SoftFillLight = CreateDefaultSubobject<URectLightComponent>(TEXT("SoftFillLight"));
	SoftFillLight->SetupAttachment(SceneRoot);
	SoftFillLight->SetMobility(EComponentMobility::Movable);
	SoftFillLight->SetIntensity(3200.f);
	SoftFillLight->SetSourceWidth(320.f);
	SoftFillLight->SetSourceHeight(360.f);
	SoftFillLight->SetAttenuationRadius(900.f);
	SoftFillLight->SetLightColor(FLinearColor(0.98f, 0.91f, 0.82f));
	SoftFillLight->SetCastShadows(false);

	RimLight = CreateDefaultSubobject<URectLightComponent>(TEXT("RimLight"));
	RimLight->SetupAttachment(SceneRoot);
	RimLight->SetMobility(EComponentMobility::Movable);
	RimLight->SetIntensity(1800.f);
	RimLight->SetSourceWidth(260.f);
	RimLight->SetSourceHeight(300.f);
	RimLight->SetAttenuationRadius(850.f);
	RimLight->SetLightColor(FLinearColor(0.96f, 0.94f, 0.88f));
	RimLight->SetCastShadows(false);

	AmbientLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("AmbientLight"));
	AmbientLight->SetupAttachment(SceneRoot);
	AmbientLight->SetMobility(EComponentMobility::Movable);
	AmbientLight->SetIntensity(0.55f);
	AmbientLight->SetLightColor(FLinearColor(0.98f, 0.93f, 0.86f));
	AmbientLight->bRealTimeCapture = false;
	AmbientLight->SetCastShadows(false);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	ThumbnailBackdrop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThumbnailBackdrop"));
	ThumbnailBackdrop->SetupAttachment(SceneRoot);
	ThumbnailBackdrop->SetMobility(EComponentMobility::Movable);
	ThumbnailBackdrop->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ThumbnailBackdrop->SetGenerateOverlapEvents(false);
	ThumbnailBackdrop->SetCastShadow(false);
	ThumbnailBackdrop->bReceivesDecals = false;
	ThumbnailBackdrop->SetRelativeLocation(
		GetCategoryCameraProfile(EWorkerSlot::Head).Focus + FVector(-95.f, 0.f, 0.f));
	// /Engine/BasicShapes/Plane lies in XY. Rotate its normal onto the camera X axis;
	// yaw-only left it horizontal and the finite plane cut across portrait captures.
	ThumbnailBackdrop->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	ThumbnailBackdrop->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
	ThumbnailBackdrop->SetVisibility(false, true);
	ThumbnailBackdrop->SetHiddenInGame(true, true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BackdropMeshFinder(
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (BackdropMeshFinder.Succeeded())
	{
		ThumbnailBackdrop->SetStaticMesh(BackdropMeshFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BackdropMaterialFinder(
		TEXT("/Game/Avariika/UI/CharacterCustomization/Materials/MI_AvThumbnailBackdrop_Unlit.MI_AvThumbnailBackdrop_Unlit"));
	if (BackdropMaterialFinder.Succeeded())
	{
		ThumbnailBackdrop->SetMaterial(0, BackdropMaterialFinder.Object);
	}
#endif
}

void AAvCustomizePreviewActor::BeginPlay()
{
	Super::BeginPlay();
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	EnsureThumbnailBackdropMaterial();
#endif
	UpdateCaptureFraming();
}

void AAvCustomizePreviewActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	bool bThumbnailModeActive = false;
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	bThumbnailModeActive = bThumbnailCaptureMode;
#endif
	if (!bCameraTransitionActive || bThumbnailModeActive)
	{
		SetActorTickEnabled(false);
		return;
	}

	CurrentCameraDistance = FMath::FInterpTo(
		CurrentCameraDistance, TargetCameraDistance, DeltaSeconds, PreviewCameraInterpSpeed);
	CurrentCameraFocus = FMath::VInterpTo(
		CurrentCameraFocus, TargetCameraFocus, DeltaSeconds, PreviewCameraInterpSpeed);

	const bool bDistanceSettled =
		FMath::IsNearlyEqual(CurrentCameraDistance, TargetCameraDistance, 0.1f);
	const bool bFocusSettled = CurrentCameraFocus.Equals(TargetCameraFocus, 0.05f);
	if (bDistanceSettled && bFocusSettled)
	{
		CurrentCameraDistance = TargetCameraDistance;
		CurrentCameraFocus = TargetCameraFocus;
		bCameraTransitionActive = false;
		bCategoryTransitionActive = false;
		SetActorTickEnabled(false);
	}
	UpdateCaptureFraming();
}

void AAvCustomizePreviewActor::TransitionToCustomizationCameraPreset(
	EAvCustomizationCameraPreset InPreset,
	bool bImmediate,
	FName ResetReason)
{
	CameraPreset = InPreset;
	const FPreviewCameraProfile Profile = GetPreviewCameraProfile(InPreset);
	CurrentPresetMinDistance = Profile.MinDistance;
	CurrentPresetBaseFocus = Profile.BaseFocus;
	CurrentPresetBaseDistance = ResolvePresetBaseDistance(
		InPreset,
		Profile.BaseDistance,
		Profile.MinDistance,
		Profile.BaseFocus);
	ResetPreviewCameraToCurrentPresetBase(
		bImmediate,
		true,
		ResetReason.IsNone() ? FName(TEXT("CameraPresetApplied")) : ResetReason);
}

void AAvCustomizePreviewActor::RefreshCustomizationCameraPresetForViewport(bool bImmediate)
{
	TransitionToCustomizationCameraPreset(
		CameraPreset,
		bImmediate,
		FName(TEXT("ViewportResize")));
}

void AAvCustomizePreviewActor::SetCompositionCenterNormalizedX(
	float InCenterNormalizedX)
{
	const float ClampedCenter = FMath::Clamp(InCenterNormalizedX, 0.1f, 0.9f);
	if (FMath::IsNearlyEqual(
		CompositionCenterNormalizedX,
		ClampedCenter,
		0.0001f))
	{
		return;
	}

	CompositionCenterNormalizedX = ClampedCenter;
	UpdateCaptureFraming();
}

void AAvCustomizePreviewActor::ClearManualZoomState()
{
	ZoomAlpha = 0.f;
	bManualZoomActive = false;
	bFocusClampedToBounds = false;
	LastAnchorSource = EAvPreviewZoomAnchorSource::None;
	LastCursorScreenPosition = FVector2D::ZeroVector;
	LastCursorWorldAnchor = FVector::ZeroVector;
}

void AAvCustomizePreviewActor::ResetPreviewCameraToCurrentPresetBase(
	bool bImmediate,
	bool bCategoryTransition,
	FName ResetReason)
{
	TargetCameraDistance = CurrentPresetBaseDistance;
	TargetCameraFocus = CurrentPresetBaseFocus;
	ClearManualZoomState();
	bLastWheelHandled = false;
	LastCameraResetReason = ResetReason;
	if (bImmediate)
	{
		CurrentCameraDistance = TargetCameraDistance;
		CurrentCameraFocus = TargetCameraFocus;
		bCameraTransitionActive = false;
		bCategoryTransitionActive = false;
		SetActorTickEnabled(false);
		UpdateCaptureFraming();
		return;
	}

	bCameraTransitionActive =
		!FMath::IsNearlyEqual(CurrentCameraDistance, TargetCameraDistance, 0.1f) ||
		!CurrentCameraFocus.Equals(TargetCameraFocus, 0.05f);
	bCategoryTransitionActive = bCategoryTransition && bCameraTransitionActive;
	SetActorTickEnabled(bCameraTransitionActive);
}

bool AAvCustomizePreviewActor::AddPreviewZoom(
	float WheelDelta,
	const FVector2D& CursorScreenPosition,
	const FVector2D& CursorNormalizedPosition)
{
	bLastWheelHandled = false;
	bool bThumbnailModeActive = false;
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	bThumbnailModeActive = bThumbnailCaptureMode;
#endif
	if (FMath::IsNearlyZero(WheelDelta) || bThumbnailModeActive)
	{
		return false;
	}

	bLastWheelHandled = true;
	const bool bCanReuseSeriesAnchor = bManualZoomActive &&
		LastAnchorSource != EAvPreviewZoomAnchorSource::None &&
		FVector2D::DistSquared(CursorScreenPosition, LastCursorScreenPosition) <= 16.f;
	const float ZoomRange = FMath::Max(
		CurrentPresetBaseDistance - CurrentPresetMinDistance, 1.f);
	const float EffectiveZoomStep = FMath::Min(PreviewZoomStep, ZoomRange / 4.f);
	const float NewDistance = FMath::Clamp(
		TargetCameraDistance - WheelDelta * EffectiveZoomStep,
		CurrentPresetMinDistance,
		CurrentPresetBaseDistance);

	if (WheelDelta > 0.f && !FMath::IsNearlyEqual(NewDistance, TargetCameraDistance))
	{
		FVector RayOrigin = FVector::ZeroVector;
		FVector RayDirection = FVector::ForwardVector;
		FVector WorldAnchor = FVector::ZeroVector;
		EAvPreviewZoomAnchorSource AnchorSource = EAvPreviewZoomAnchorSource::None;
		bool bClampedToBounds = false;
		bool bResolvedAnchor = false;
		if (bCanReuseSeriesAnchor)
		{
			WorldAnchor = LastCursorWorldAnchor;
			AnchorSource = LastAnchorSource;
			bClampedToBounds = bFocusClampedToBounds;
			bResolvedAnchor = true;
		}
		else
		{
			bResolvedAnchor =
				BuildPreviewCursorRay(CursorNormalizedPosition, RayOrigin, RayDirection) &&
				ResolveCursorZoomAnchor(
					RayOrigin, RayDirection, WorldAnchor, AnchorSource, bClampedToBounds);
		}
		LastCursorScreenPosition = CursorScreenPosition;
		if (bResolvedAnchor)
		{
			LastCursorWorldAnchor = WorldAnchor;
			LastAnchorSource = AnchorSource;
			bFocusClampedToBounds = bClampedToBounds;
			TargetCameraDistance = NewDistance;
			ZoomAlpha = FMath::Clamp(
				(CurrentPresetBaseDistance - TargetCameraDistance) / ZoomRange, 0.f, 1.f);
			const FVector LocalAnchor = GetActorTransform().InverseTransformPosition(WorldAnchor);
			TargetCameraFocus = FMath::Lerp(CurrentPresetBaseFocus, LocalAnchor, ZoomAlpha);
			bManualZoomActive = ZoomAlpha > KINDA_SMALL_NUMBER;
			bCategoryTransitionActive = false;
			bCameraTransitionActive = true;
			SetActorTickEnabled(true);
		}
		return true;
	}

	LastCursorScreenPosition = CursorScreenPosition;
	TargetCameraDistance = NewDistance;
	ZoomAlpha = FMath::Clamp(
		(CurrentPresetBaseDistance - TargetCameraDistance) / ZoomRange, 0.f, 1.f);
	if (ZoomAlpha <= KINDA_SMALL_NUMBER)
	{
		ResetPreviewCameraToCurrentPresetBase(
			false,
			false,
			FName(TEXT("WheelDownToBase")));
		bLastWheelHandled = true;
	}
	else
	{
		const FVector LocalAnchor = GetActorTransform().InverseTransformPosition(
			LastCursorWorldAnchor);
		TargetCameraFocus = FMath::Lerp(CurrentPresetBaseFocus, LocalAnchor, ZoomAlpha);
		bManualZoomActive = true;
	}
	bCategoryTransitionActive = false;
	if (!FMath::IsNearlyEqual(CurrentCameraDistance, TargetCameraDistance, 0.1f) ||
		!CurrentCameraFocus.Equals(TargetCameraFocus, 0.05f))
	{
		bCameraTransitionActive = true;
		SetActorTickEnabled(true);
	}
	return true;
}

void AAvCustomizePreviewActor::InitializePreview(UTextureRenderTarget2D* InRenderTarget)
{
	SceneCapture->TextureTarget = InRenderTarget;
	ConfigurePreviewMeshes();
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	ApplyAppearanceInspectorState();
#endif
	UpdateCaptureFraming();
	RefreshCaptureShowOnly();
	SceneCapture->CaptureScene();
}

void AAvCustomizePreviewActor::SetPreviewAppearance(const FWorkerAppearance& InAppearance)
{
	if (AppearanceComponent)
	{
		AppearanceComponent->ApplyLocalPreviewAppearance(InAppearance);
	}
	ApplyIdleAnimation();
	ApplyPreviewYaw();
	ConfigurePreviewMeshes();
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	ApplyAppearanceInspectorState();
#endif
	UpdateCaptureFraming();
	RefreshCaptureShowOnly();
	if (SceneCapture)
	{
		SceneCapture->CaptureScene();
	}
}

void AAvCustomizePreviewActor::SetPreviewSlotColor(EWorkerSlot Slot, const FLinearColor& Color)
{
	if (AppearanceComponent)
	{
		AppearanceComponent->SetSlotColor(Slot, Color);
	}
	ConfigurePreviewMeshes();
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	ApplyAppearanceInspectorState();
#endif
	UpdateCaptureFraming();
	if (SceneCapture)
	{
		SceneCapture->CaptureScene();
	}
}

void AAvCustomizePreviewActor::SetPreviewYaw(float InYawDegrees)
{
	PreviewYawDegrees = FMath::UnwindDegrees(InYawDegrees);
	ApplyPreviewYaw();
	UpdateCaptureFraming();
	if (SceneCapture)
	{
		SceneCapture->CaptureScene();
	}
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
void AAvCustomizePreviewActor::EnsureThumbnailBackdropMaterial()
{
	if (!ThumbnailBackdrop || ThumbnailBackdropMaterial)
	{
		return;
	}

	UMaterialInterface* BackdropSourceMaterial = ThumbnailBackdrop->GetMaterial(0);
	if (!BackdropSourceMaterial)
	{
		return;
	}

	ThumbnailBackdropMaterial = UMaterialInstanceDynamic::Create(
		BackdropSourceMaterial,
		this,
		TEXT("MID_ThumbnailBackdrop"));
	if (!ThumbnailBackdropMaterial)
	{
		return;
	}

	ThumbnailBackdropMaterial->SetVectorParameterValue(TEXT("Color"), ThumbnailBackdropMaterialColor);
	ThumbnailBackdrop->SetMaterial(0, ThumbnailBackdropMaterial);
}

void AAvCustomizePreviewActor::SetThumbnailCaptureMode(bool bEnabled)
{
	bThumbnailCaptureMode = bEnabled;
	const FThumbnailCameraProfile CameraProfile =
		GetCategoryCameraProfile(ThumbnailCaptureItemSlot);
	EnsureThumbnailBackdropMaterial();
	if (ThumbnailBackdropMaterial)
	{
		ThumbnailBackdropMaterial->SetVectorParameterValue(
			TEXT("Color"),
			ThumbnailBackdropMaterialColor);
	}
	if (ThumbnailBackdrop)
	{
		ThumbnailBackdrop->SetVisibility(bEnabled, true);
		ThumbnailBackdrop->SetHiddenInGame(!bEnabled, true);
	}
	if (!SceneCapture || !SceneCapture->TextureTarget)
	{
		return;
	}
	if (bEnabled && !bHasStoredPreThumbnailPostProcess)
	{
		PreThumbnailClearColor = SceneCapture->TextureTarget->ClearColor;
		bPreThumbnailVignetteOverride =
			SceneCapture->PostProcessSettings.bOverride_VignetteIntensity;
		PreThumbnailVignetteIntensity =
			SceneCapture->PostProcessSettings.VignetteIntensity;
		bHasStoredPreThumbnailPostProcess = true;
	}
	SceneCapture->TextureTarget->ClearColor = bEnabled
		? ThumbnailBackdropClearColor
		: PreThumbnailClearColor;
	SceneCapture->TextureTarget->UpdateResourceImmediate(false);
	SceneCapture->PostProcessSettings.bOverride_VignetteIntensity = bEnabled
		? true
		: bPreThumbnailVignetteOverride;
	SceneCapture->PostProcessSettings.VignetteIntensity = bEnabled
		? 0.32f
		: PreThumbnailVignetteIntensity;

	// Prepared thumbnails are captured only on explicit state changes. Keep every-frame
	// capture disabled for the explicit CaptureScene below, including while restoring the
	// normal preview, so this helper never produces the expensive CaptureScene warning.
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->CaptureSource = bEnabled
		? ESceneCaptureSource::SCS_FinalColorLDR
		: ESceneCaptureSource::SCS_SceneColorHDR;
	SceneCapture->FOVAngle = bEnabled
		? CameraProfile.FovDegrees
		: PreviewCaptureFovDegrees;
	SceneCapture->PostProcessSettings.AutoExposureBias = bEnabled
		? CommonThumbnailLightingProfile.ExposureBias
		: 0.35f;
	if (bEnabled)
	{
		// Every Head category uses the exact accepted Headgear lighting/exposure.
		// Category-specific behavior is restricted to camera framing below.
		KeyLight->SetIntensity(CommonThumbnailLightingProfile.KeyIntensity);
		FillLight->SetIntensity(CommonThumbnailLightingProfile.FillIntensity);
		SoftKeyLight->SetIntensity(CommonThumbnailLightingProfile.SoftKeyIntensity);
		SoftFillLight->SetIntensity(CommonThumbnailLightingProfile.SoftFillIntensity);
		RimLight->SetIntensity(CommonThumbnailLightingProfile.RimIntensity);
		AmbientLight->SetIntensity(CommonThumbnailLightingProfile.AmbientIntensity);
	}
	else
	{
		KeyLight->SetIntensity(2.65f);
		FillLight->SetIntensity(1500.f);
		SoftKeyLight->SetIntensity(9000.f);
		SoftFillLight->SetIntensity(3200.f);
		RimLight->SetIntensity(1800.f);
		AmbientLight->SetIntensity(0.55f);
	}
	UpdateCaptureFraming();
	RefreshCaptureShowOnly();
	SceneCapture->CaptureScene();
	SceneCapture->bCaptureEveryFrame = !bEnabled;
	if (!bEnabled && bCameraTransitionActive)
	{
		SetActorTickEnabled(true);
	}
}

void AAvCustomizePreviewActor::SetThumbnailCapturePreview(
	const FString& HeadObjectPath,
	const FString& HeadgearObjectPath,
	const FLinearColor& SkinColor)
{
	SetThumbnailCaptureItemPreview(
		HeadObjectPath,
		EWorkerSlot::Headgear,
		HeadgearObjectPath,
		SkinColor);
}

void AAvCustomizePreviewActor::SetThumbnailCaptureItemPreview(
	const FString& HeadObjectPath,
	EWorkerSlot ItemSlot,
	const FString& ItemObjectPath,
	const FLinearColor& SkinColor)
{
	ThumbnailCaptureItemSlot = ItemSlot;
	SetThumbnailCaptureMode(true);
	FWorkerAppearance ThumbnailAppearance =
		UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
	if (!HeadObjectPath.IsEmpty() && HeadObjectPath != TEXT("None"))
	{
		ThumbnailAppearance.Set(
			EWorkerSlot::Head,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(HeadObjectPath)));
	}
	ThumbnailAppearance.Clear(EWorkerSlot::Headgear);
	ThumbnailAppearance.Clear(EWorkerSlot::Hair);
	ThumbnailAppearance.Clear(EWorkerSlot::Beard);
	ThumbnailAppearance.Clear(EWorkerSlot::FaceMask);
	ThumbnailAppearance.Clear(EWorkerSlot::Glasses);
	ThumbnailAppearance.Clear(EWorkerSlot::Headphones);
	ThumbnailAppearance.Clear(EWorkerSlot::Gloves);
	ThumbnailAppearance.Clear(EWorkerSlot::Watch);
	ThumbnailAppearance.Clear(EWorkerSlot::HipAccessory);
	ThumbnailAppearance.Clear(EWorkerSlot::FullOutfit);
	ThumbnailAppearance.Clear(EWorkerSlot::Overalls);
	if ((ItemSlot == EWorkerSlot::Headgear ||
		ItemSlot == EWorkerSlot::Hair ||
		ItemSlot == EWorkerSlot::Beard ||
		ItemSlot == EWorkerSlot::FaceMask ||
		ItemSlot == EWorkerSlot::Glasses ||
		ItemSlot == EWorkerSlot::Headphones ||
		ItemSlot == EWorkerSlot::Gloves ||
		ItemSlot == EWorkerSlot::Watch ||
		ItemSlot == EWorkerSlot::Torso ||
		ItemSlot == EWorkerSlot::Legs ||
		ItemSlot == EWorkerSlot::HipAccessory ||
		ItemSlot == EWorkerSlot::FullOutfit ||
		ItemSlot == EWorkerSlot::Overalls) &&
		!ItemObjectPath.IsEmpty() &&
		ItemObjectPath != TEXT("None"))
	{
		ThumbnailAppearance.Set(
			ItemSlot,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(ItemObjectPath)));
	}

	// The head mesh is the only product selector. Normalize its persisted endpoint, then let
	// the single canonical rebuild apply the matching Head/Arms/Body material family once.
	// The legacy argument remains API-compatible for existing capture scripts but cannot tint
	// or override the HeadType-owned profile.
	(void)SkinColor;
	UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(ThumbnailAppearance);
	SetPreviewAppearance(ThumbnailAppearance);
	SetPreviewYaw(0.f);
	const FThumbnailCameraProfile CameraProfile =
		GetCategoryCameraProfile(ItemSlot);
	UE_LOG(LogTemp, Display,
		TEXT("[AvThumbnailCapture] PreviewOnly=true LightingProfile=CommonHeadgear CameraProfile=%s Head=%s ItemSlot=%d Item=%s CaptureSource=FinalColorLDR ExposureMethod=Manual ExposureBias=%.2f FOV=%.1f CameraDistance=%.1f FocusZ=%.1f Key=%.1f Fill=%.1f SoftKey=%.1f SoftFill=%.1f Rim=%.1f Ambient=%.2f Backdrop=(%.3f,%.3f,%.3f) Skin=(%.3f,%.3f,%.3f) AppearanceMutation=false Autosave=false"),
		CameraProfile.Name,
		*HeadObjectPath,
		static_cast<int32>(ItemSlot),
		ItemObjectPath.IsEmpty() ? TEXT("None") : *ItemObjectPath,
		CommonThumbnailLightingProfile.ExposureBias,
		CameraProfile.FovDegrees,
		CameraProfile.CameraDistance,
		CameraProfile.Focus.Z,
		CommonThumbnailLightingProfile.KeyIntensity,
		CommonThumbnailLightingProfile.FillIntensity,
		CommonThumbnailLightingProfile.SoftKeyIntensity,
		CommonThumbnailLightingProfile.SoftFillIntensity,
		CommonThumbnailLightingProfile.RimIntensity,
		CommonThumbnailLightingProfile.AmbientIntensity,
		ThumbnailBackdropMaterialColor.R,
		ThumbnailBackdropMaterialColor.G,
		ThumbnailBackdropMaterialColor.B,
		ThumbnailAppearance.SkinColor.R,
		ThumbnailAppearance.SkinColor.G,
		ThumbnailAppearance.SkinColor.B);
}

bool AAvCustomizePreviewActor::ExportThumbnailRenderTargetOpaquePng(
	UTextureRenderTarget2D* RenderTarget,
	const FString& AbsoluteFilename,
	FString& OutError) const
{
	OutError.Reset();
	if (!RenderTarget)
	{
		OutError = TEXT("RenderTarget is null");
		return false;
	}
	if (RenderTarget->SizeX != 512 || RenderTarget->SizeY != 512)
	{
		OutError = FString::Printf(
			TEXT("RenderTarget must be 512x512, got %dx%d"),
			RenderTarget->SizeX,
			RenderTarget->SizeY);
		return false;
	}
	if (FPaths::IsRelative(AbsoluteFilename) ||
		!AbsoluteFilename.EndsWith(TEXT(".png"), ESearchCase::IgnoreCase))
	{
		OutError = TEXT("Absolute output filename must use the .png extension");
		return false;
	}

	FTextureRenderTargetResource* Resource =
		RenderTarget->GameThread_GetRenderTargetResource();
	if (!Resource)
	{
		OutError = TEXT("RenderTarget resource is unavailable");
		return false;
	}

	TArray<FColor> Pixels;
	FReadSurfaceDataFlags ReadFlags(RCM_UNorm);
	if (!Resource->ReadPixels(Pixels, ReadFlags) || Pixels.Num() != 512 * 512)
	{
		OutError = FString::Printf(
			TEXT("ReadPixels failed or returned %d pixels"), Pixels.Num());
		return false;
	}
	for (FColor& Pixel : Pixels)
	{
		Pixel.A = 255;
	}

	TArray64<uint8> CompressedPng;
	FImageUtils::PNGCompressImageArray(512, 512, Pixels, CompressedPng);
	if (CompressedPng.IsEmpty())
	{
		OutError = TEXT("PNG compression returned no data");
		return false;
	}

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(AbsoluteFilename), true);
	if (!FFileHelper::SaveArrayToFile(CompressedPng, *AbsoluteFilename))
	{
		OutError = TEXT("Failed to save PNG: ") + AbsoluteFilename;
		return false;
	}

	UE_LOG(LogTemp, Display,
		TEXT("[AvThumbnailCapture] SavedOpaquePng=%s Size=512x512 Alpha=255 Pixels=%d"),
		*AbsoluteFilename,
		Pixels.Num());
	return true;
}
#endif

void AAvCustomizePreviewActor::ApplyIdleAnimation()
{
	if (!AppearanceComponent)
	{
		return;
	}

	USkeletalMeshComponent* Body = AppearanceComponent->GetBodyComponent();
	UAnimSequence* IdleAnimation = LoadObject<UAnimSequence>(
		nullptr,
		TEXT("/Game/Modular_Workers/Demo/Animations/Male/A_MM_Idle.A_MM_Idle"));
	if (!Body || !IdleAnimation)
	{
		return;
	}

	Body->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Body->SetAnimation(IdleAnimation);
	Body->Play(true);
}

void AAvCustomizePreviewActor::ApplyPreviewYaw()
{
	if (!AppearanceComponent)
	{
		return;
	}

	if (USkeletalMeshComponent* Body = AppearanceComponent->GetBodyComponent())
	{
		Body->SetRelativeRotation(FRotator(0.f, -90.f + PreviewYawDegrees, 0.f));
	}
}

void AAvCustomizePreviewActor::ConfigurePreviewMeshes()
{
	TArray<USkeletalMeshComponent*> Meshes;
	GetComponents<USkeletalMeshComponent>(Meshes);
	for (USkeletalMeshComponent* Mesh : Meshes)
	{
		ConfigurePreviewMesh(Mesh);
	}
}

void AAvCustomizePreviewActor::ConfigurePreviewMesh(USkeletalMeshComponent* Mesh) const
{
	if (!Mesh)
	{
		return;
	}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	Mesh->SetForcedLOD(bAppearanceInspectorForceLod0 ? 1 : 0);
#else
	Mesh->SetForcedLOD(1);
#endif
	Mesh->bForceMipStreaming = true;
	Mesh->StreamingDistanceMultiplier = FMath::Max(
		Mesh->StreamingDistanceMultiplier,
		PreviewStreamingDistanceMultiplier);
	Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	Mesh->bEnableUpdateRateOptimizations = false;
	Mesh->SetComponentTickEnabled(true);
	Mesh->PrestreamTextures(PreviewTexturePrestreamSeconds, true, 0);
	Mesh->PrestreamMeshLODs(PreviewTexturePrestreamSeconds);
	Mesh->UpdateComponentToWorld();
	Mesh->RefreshBoneTransforms();
	Mesh->UpdateBounds();
	Mesh->MarkRenderStateDirty();
}

void AAvCustomizePreviewActor::RefreshPreviewMeshBounds() const
{
	TArray<USkeletalMeshComponent*> Meshes;
	GetComponents<USkeletalMeshComponent>(Meshes);
	for (USkeletalMeshComponent* Mesh : Meshes)
	{
		if (!Mesh || !Mesh->GetSkeletalMeshAsset() || !Mesh->IsVisible())
		{
			continue;
		}

		Mesh->UpdateComponentToWorld();
		Mesh->RefreshBoneTransforms();
		Mesh->UpdateBounds();
		Mesh->MarkRenderTransformDirty();
	}
}

bool AAvCustomizePreviewActor::GetPreviewLocalBounds(FBox& OutLocalBounds) const
{
	RefreshPreviewMeshBounds();

	FBox LocalBox(ForceInit);
	const FTransform ActorTransform = GetActorTransform();

	TArray<USkeletalMeshComponent*> Meshes;
	GetComponents<USkeletalMeshComponent>(Meshes);
	for (USkeletalMeshComponent* Mesh : Meshes)
	{
		if (!Mesh || !Mesh->GetSkeletalMeshAsset() || !Mesh->IsVisible())
		{
			continue;
		}

		const FBox WorldBox = Mesh->CalcBounds(Mesh->GetComponentTransform()).GetBox();
		if (WorldBox.IsValid)
		{
			AddBoxCorners(WorldBox, ActorTransform, LocalBox);
		}
	}

	if (!LocalBox.IsValid)
	{
		return false;
	}

	OutLocalBounds = LocalBox;
	return true;
}

bool AAvCustomizePreviewActor::GetCombinedVisibleBounds(FBox& OutLocalBounds) const
{
	return GetPreviewLocalBounds(OutLocalBounds);
}

float AAvCustomizePreviewActor::GetSceneCaptureFOV() const
{
	return SceneCapture ? SceneCapture->FOVAngle : 0.f;
}

float AAvCustomizePreviewActor::GetSceneCaptureAspectRatio() const
{
	if (!SceneCapture || !SceneCapture->TextureTarget ||
		SceneCapture->TextureTarget->SizeY <= 0)
	{
		return 0.f;
	}
	return static_cast<float>(SceneCapture->TextureTarget->SizeX) /
		static_cast<float>(SceneCapture->TextureTarget->SizeY);
}

bool AAvCustomizePreviewActor::IsSceneCaptureUsingCustomProjection() const
{
	return SceneCapture && SceneCapture->bUseCustomProjectionMatrix;
}

bool AAvCustomizePreviewActor::GetProjectedVisibleBounds(FBox2D& OutNormalizedBounds) const
{
	FBox LocalBounds(ForceInit);
	if (!SceneCapture || !GetPreviewLocalBounds(LocalBounds))
	{
		return false;
	}
	const FRotationMatrix CaptureRotation(SceneCapture->GetRelativeRotation());
	return ProjectLocalBoundsToNormalizedViewport(
		LocalBounds,
		SceneCapture->GetRelativeLocation(),
		CaptureRotation.GetUnitAxis(EAxis::X),
		CaptureRotation.GetUnitAxis(EAxis::Y),
		CaptureRotation.GetUnitAxis(EAxis::Z),
		SceneCapture->FOVAngle,
		GetSceneCaptureAspectRatio(),
		OutNormalizedBounds);
}

bool AAvCustomizePreviewActor::IsCameraAtBaseStateExact() const
{
	return !bCameraTransitionActive && !bManualZoomActive &&
		LastAnchorSource == EAvPreviewZoomAnchorSource::None &&
		FMath::IsNearlyEqual(CurrentCameraDistance, CurrentPresetBaseDistance, 0.01f) &&
		FMath::IsNearlyEqual(TargetCameraDistance, CurrentPresetBaseDistance, 0.01f) &&
		CurrentCameraFocus.Equals(CurrentPresetBaseFocus, 0.01f) &&
		TargetCameraFocus.Equals(CurrentPresetBaseFocus, 0.01f);
}

float AAvCustomizePreviewActor::ResolvePresetBaseDistance(
	EAvCustomizationCameraPreset InPreset,
	float AuthoredBaseDistance,
	float MinDistance,
	const FVector& BaseFocus) const
{
	FBox LocalBounds(ForceInit);
	const float CaptureAspect = GetSceneCaptureAspectRatio();
	if (!GetPreviewLocalBounds(LocalBounds) || CaptureAspect <= KINDA_SMALL_NUMBER)
	{
		return FMath::Max(MinDistance, AuthoredBaseDistance * FMath::Max(CaptureAspect, 1.f));
	}

	float TargetHeightRatio = PreviewFullBodyTargetHeightRatio;
	if (InPreset != EAvCustomizationCameraPreset::FullBody)
	{
		FBox2D ReferenceBounds(ForceInit);
		if (!ProjectLocalBoundsAtPresetDistance(
			LocalBounds,
			BaseFocus,
			AuthoredBaseDistance,
			PreviewCaptureFovDegrees,
			1.f,
			ReferenceBounds))
		{
			return FMath::Max(MinDistance, AuthoredBaseDistance * CaptureAspect);
		}
		TargetHeightRatio = ReferenceBounds.GetSize().Y;
	}

	const auto ProjectedHeightAtDistance = [&](float Distance)
	{
		FBox2D ProjectedBounds(ForceInit);
		return ProjectLocalBoundsAtPresetDistance(
			LocalBounds,
			BaseFocus,
			Distance,
			PreviewCaptureFovDegrees,
			CaptureAspect,
			ProjectedBounds)
			? ProjectedBounds.GetSize().Y
			: TNumericLimits<float>::Max();
	};

	float LowDistance = FMath::Max(MinDistance, 1.f);
	float HighDistance = FMath::Max(AuthoredBaseDistance, LowDistance + 1.f);
	for (int32 ExpansionIndex = 0;
		ExpansionIndex < 16 && ProjectedHeightAtDistance(HighDistance) > TargetHeightRatio;
		++ExpansionIndex)
	{
		HighDistance *= 1.5f;
	}

	if (ProjectedHeightAtDistance(LowDistance) <= TargetHeightRatio)
	{
		return LowDistance;
	}
	for (int32 Iteration = 0; Iteration < 28; ++Iteration)
	{
		const float MiddleDistance = (LowDistance + HighDistance) * 0.5f;
		if (ProjectedHeightAtDistance(MiddleDistance) > TargetHeightRatio)
		{
			LowDistance = MiddleDistance;
		}
		else
		{
			HighDistance = MiddleDistance;
		}
	}
	return HighDistance;
}

bool AAvCustomizePreviewActor::BuildPreviewCursorRay(
	const FVector2D& CursorNormalizedPosition,
	FVector& OutRayOrigin,
	FVector& OutRayDirection) const
{
	if (!SceneCapture || !FMath::IsFinite(CursorNormalizedPosition.X) ||
		!FMath::IsFinite(CursorNormalizedPosition.Y))
	{
		return false;
	}

	const FVector2D ClampedCursor(
		FMath::Clamp(CursorNormalizedPosition.X, 0.f, 1.f),
		FMath::Clamp(CursorNormalizedPosition.Y, 0.f, 1.f));
	const float NormalizedX = ClampedCursor.X * 2.f - 1.f;
	const float NormalizedY = 1.f - ClampedCursor.Y * 2.f;
	const float TanHalfHorizontalFov = FMath::Tan(
		FMath::DegreesToRadians(SceneCapture->FOVAngle * 0.5f));
	float CaptureAspectRatio = 1.f;
	if (SceneCapture->TextureTarget && SceneCapture->TextureTarget->SizeY > 0)
	{
		CaptureAspectRatio = static_cast<float>(SceneCapture->TextureTarget->SizeX) /
			static_cast<float>(SceneCapture->TextureTarget->SizeY);
	}
	const float TanHalfVerticalFov = TanHalfHorizontalFov /
		FMath::Max(CaptureAspectRatio, KINDA_SMALL_NUMBER);

	OutRayOrigin = SceneCapture->GetComponentLocation();
	OutRayDirection = (
		SceneCapture->GetForwardVector() +
		SceneCapture->GetRightVector() * (NormalizedX * TanHalfHorizontalFov) +
		SceneCapture->GetUpVector() * (NormalizedY * TanHalfVerticalFov)).GetSafeNormal();
	return !OutRayDirection.IsNearlyZero();
}

bool AAvCustomizePreviewActor::ResolveCursorZoomAnchor(
	const FVector& RayOrigin,
	const FVector& RayDirection,
	FVector& OutWorldAnchor,
	EAvPreviewZoomAnchorSource& OutSource,
	bool& bOutClampedToBounds) const
{
	OutSource = EAvPreviewZoomAnchorSource::None;
	bOutClampedToBounds = false;
	FBox LocalBounds(ForceInit);
	if (!GetPreviewLocalBounds(LocalBounds))
	{
		return false;
	}

	const FVector RayEnd = RayOrigin + RayDirection * PreviewCursorTraceDistance;
	float ClosestHitDistanceSquared = TNumericLimits<float>::Max();
	FVector ClosestHitPoint = FVector::ZeroVector;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(AvPreviewCursorZoom), true);
	QueryParams.bReturnFaceIndex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	TArray<USkeletalMeshComponent*> Meshes;
	GetComponents<USkeletalMeshComponent>(Meshes);
	for (USkeletalMeshComponent* Mesh : Meshes)
	{
		if (!Mesh || !Mesh->GetSkeletalMeshAsset() || !Mesh->IsVisible() ||
			!AppearanceComponent || !AppearanceComponent->IsManagedVisualComponent(Mesh))
		{
			continue;
		}

		FHitResult Hit;
		if (Mesh->LineTraceComponent(Hit, RayOrigin, RayEnd, QueryParams))
		{
			const float HitDistanceSquared = FVector::DistSquared(RayOrigin, Hit.ImpactPoint);
			if (HitDistanceSquared < ClosestHitDistanceSquared)
			{
				ClosestHitDistanceSquared = HitDistanceSquared;
				ClosestHitPoint = Hit.ImpactPoint;
			}
		}
	}

	FVector CandidateWorld = ClosestHitPoint;
	if (ClosestHitDistanceSquared < TNumericLimits<float>::Max())
	{
		OutSource = EAvPreviewZoomAnchorSource::MeshHit;
	}
	else
	{
		const FVector PlaneOrigin = GetActorTransform().TransformPosition(CurrentCameraFocus);
		const FVector PlaneNormal = SceneCapture->GetForwardVector().GetSafeNormal();
		const float Denominator = FVector::DotProduct(RayDirection, PlaneNormal);
		if (FMath::IsNearlyZero(Denominator))
		{
			return false;
		}
		const float RayDistance = FVector::DotProduct(PlaneOrigin - RayOrigin, PlaneNormal) /
			Denominator;
		if (!FMath::IsFinite(RayDistance) || RayDistance <= 0.f)
		{
			return false;
		}
		CandidateWorld = RayOrigin + RayDirection * RayDistance;
		OutSource = EAvPreviewZoomAnchorSource::FocusPlaneFallback;
	}

	const FBox AllowedFocusBounds = LocalBounds.ExpandBy(PreviewFocusBoundsExpansion);
	const FVector CandidateLocal = GetActorTransform().InverseTransformPosition(CandidateWorld);
	const FVector ClampedLocal(
		FMath::Clamp(CandidateLocal.X, AllowedFocusBounds.Min.X, AllowedFocusBounds.Max.X),
		FMath::Clamp(CandidateLocal.Y, AllowedFocusBounds.Min.Y, AllowedFocusBounds.Max.Y),
		FMath::Clamp(CandidateLocal.Z, AllowedFocusBounds.Min.Z, AllowedFocusBounds.Max.Z));
	bOutClampedToBounds = !CandidateLocal.Equals(ClampedLocal, 0.01f);
	OutWorldAnchor = GetActorTransform().TransformPosition(ClampedLocal);
	return !OutWorldAnchor.ContainsNaN();
}

void AAvCustomizePreviewActor::RefreshCaptureShowOnly()
{
	if (!SceneCapture)
	{
		return;
	}

	SceneCapture->ClearShowOnlyComponents();
	TArray<USkeletalMeshComponent*> Meshes;
	GetComponents<USkeletalMeshComponent>(Meshes);
	for (USkeletalMeshComponent* Mesh : Meshes)
	{
		// Head cards deliberately retain the canonical Body. The fixed portrait crop
		// shows only the upper chest, but the real neck/shoulders provide an integrated
		// silhouette instead of a floating Head mesh.
		if (Mesh && Mesh->GetSkeletalMeshAsset() && Mesh->IsVisible())
		{
			SceneCapture->ShowOnlyComponent(Mesh);
		}
	}
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (bThumbnailCaptureMode && ThumbnailBackdrop && ThumbnailBackdrop->IsVisible())
	{
		SceneCapture->ShowOnlyComponent(ThumbnailBackdrop);
	}
#endif
}

void AAvCustomizePreviewActor::UpdateCaptureFraming()
{
	if (!SceneCapture || !KeyLight)
	{
		return;
	}

	FBox LocalBounds(ForceInit);
	GetPreviewLocalBounds(LocalBounds);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const FThumbnailCameraProfile CameraProfile =
		GetCategoryCameraProfile(ThumbnailCaptureItemSlot);
	const bool bThumbnailCompositionActive = bThumbnailCaptureMode;
	const FVector Focus = bThumbnailCaptureMode
		? CameraProfile.Focus
		: CurrentCameraFocus;
	const float CameraDistance = bThumbnailCaptureMode
		? CameraProfile.CameraDistance
		: CurrentCameraDistance;
#else
	const bool bThumbnailCompositionActive = false;
	const FVector Focus = CurrentCameraFocus;
	const float CameraDistance = CurrentCameraDistance;
#endif
	const FVector ViewDirection = FVector(-1.f, 0.08f, -0.01f).GetSafeNormal();
	const float CompositionCenterX = bThumbnailCompositionActive
		? 0.5f
		: CompositionCenterNormalizedX;
	const float HalfViewWidthAtFocus = CameraDistance * FMath::Tan(
		FMath::DegreesToRadians(SceneCapture->FOVAngle * 0.5f));
	const float CompositionOffset =
		(0.5f - CompositionCenterX) * 2.f * HalfViewWidthAtFocus;
	const FVector CameraRight = FRotationMatrix(
		ViewDirection.Rotation()).GetUnitAxis(EAxis::Y);
	const FVector CompositionFocus = Focus + CameraRight * CompositionOffset;
	const FVector CameraLocal = CompositionFocus - ViewDirection * CameraDistance;

	SceneCapture->SetRelativeLocation(CameraLocal);
	SceneCapture->SetRelativeRotation((CompositionFocus - CameraLocal).Rotation());
	KeyLight->SetRelativeRotation(CommonThumbnailLightingProfile.KeyRotation);
	if (FillLight)
	{
		FillLight->SetRelativeLocation(
			Focus + CommonThumbnailLightingProfile.FillOffset);
	}
	if (SoftKeyLight)
	{
		const FVector KeyPosition =
			Focus + CommonThumbnailLightingProfile.SoftKeyOffset;
		SoftKeyLight->SetRelativeLocation(KeyPosition);
		const FVector KeyTarget =
			Focus + CommonThumbnailLightingProfile.SoftKeyTargetOffset;
		SoftKeyLight->SetRelativeRotation((KeyTarget - KeyPosition).Rotation());
	}
	if (SoftFillLight)
	{
		const FVector FillPosition =
			Focus + CommonThumbnailLightingProfile.SoftFillOffset;
		SoftFillLight->SetRelativeLocation(FillPosition);
		const FVector FillTarget =
			Focus + CommonThumbnailLightingProfile.SoftFillTargetOffset;
		SoftFillLight->SetRelativeRotation((FillTarget - FillPosition).Rotation());
	}
	if (RimLight)
	{
		const FVector RimPosition =
			Focus + CommonThumbnailLightingProfile.RimOffset;
		RimLight->SetRelativeLocation(RimPosition);
		const FVector RimTarget =
			Focus + CommonThumbnailLightingProfile.RimTargetOffset;
		RimLight->SetRelativeRotation((RimTarget - RimPosition).Rotation());
	}
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
namespace
{
	bool IsInspectorClothingSlot(EWorkerSlot Slot)
	{
		return Slot != EWorkerSlot::Body && Slot != EWorkerSlot::Head;
	}
}

void AAvCustomizePreviewActor::ApplyAppearanceInspectorState()
{
	if (!AppearanceComponent)
	{
		return;
	}

	for (int32 SlotIndex = static_cast<int32>(EWorkerSlot::Body);
		SlotIndex <= static_cast<int32>(EWorkerSlot::Overalls);
		++SlotIndex)
	{
		const EWorkerSlot Slot = static_cast<EWorkerSlot>(SlotIndex);
		USkeletalMeshComponent* Mesh = AppearanceComponent->GetAppearanceInspectorSlotComponent(Slot);
		if (!Mesh)
		{
			continue;
		}

		const bool bCanonicalSuppressed =
			AppearanceComponent->IsAppearanceInspectorSlotSuppressed(Slot);
		const bool bVisible = !bCanonicalSuppressed &&
			!DebugHiddenSlots.Contains(Slot) &&
			!(Slot == EWorkerSlot::Body && bDebugHideBody);
		Mesh->SetVisibility(bVisible, true);
		Mesh->SetHiddenInGame(!bVisible, true);
		Mesh->SetForcedLOD(bAppearanceInspectorForceLod0 ? 1 : 0);
		Mesh->MarkRenderStateDirty();
	}
}

void AAvCustomizePreviewActor::RefreshAppearanceInspectorCapture()
{
	ApplyAppearanceInspectorState();
	UpdateCaptureFraming();
	RefreshCaptureShowOnly();
	if (SceneCapture)
	{
		SceneCapture->CaptureScene();
	}
}

void AAvCustomizePreviewActor::AppearanceInspectorToggleSlot(EWorkerSlot Slot)
{
	DebugSoloSlot.Reset();
	if (Slot == EWorkerSlot::Body)
	{
		bDebugHideBody = !bDebugHideBody;
	}
	else if (DebugHiddenSlots.Contains(Slot))
	{
		DebugHiddenSlots.Remove(Slot);
	}
	else
	{
		DebugHiddenSlots.Add(Slot);
	}
	RefreshAppearanceInspectorCapture();
}

void AAvCustomizePreviewActor::AppearanceInspectorSoloSlot(EWorkerSlot Slot)
{
	DebugHiddenSlots.Reset();
	for (int32 SlotIndex = static_cast<int32>(EWorkerSlot::Body);
		SlotIndex <= static_cast<int32>(EWorkerSlot::Overalls);
		++SlotIndex)
	{
		const EWorkerSlot CandidateSlot = static_cast<EWorkerSlot>(SlotIndex);
		if (CandidateSlot != EWorkerSlot::Body && CandidateSlot != EWorkerSlot::Head && CandidateSlot != Slot)
		{
			DebugHiddenSlots.Add(CandidateSlot);
		}
	}
	bDebugHideBody = false;
	DebugSoloSlot = Slot;
	RefreshAppearanceInspectorCapture();
}

void AAvCustomizePreviewActor::AppearanceInspectorShowAll()
{
	DebugHiddenSlots.Reset();
	DebugSoloSlot.Reset();
	bDebugHideBody = false;
	RefreshAppearanceInspectorCapture();
}

void AAvCustomizePreviewActor::AppearanceInspectorHideAllClothing()
{
	DebugHiddenSlots.Reset();
	for (int32 SlotIndex = static_cast<int32>(EWorkerSlot::Body);
		SlotIndex <= static_cast<int32>(EWorkerSlot::Overalls);
		++SlotIndex)
	{
		const EWorkerSlot Slot = static_cast<EWorkerSlot>(SlotIndex);
		if (IsInspectorClothingSlot(Slot))
		{
			DebugHiddenSlots.Add(Slot);
		}
	}
	bDebugHideBody = false;
	DebugSoloSlot.Reset();
	RefreshAppearanceInspectorCapture();
}

void AAvCustomizePreviewActor::AppearanceInspectorBodyOnly()
{
	AppearanceInspectorHideAllClothing();
}

void AAvCustomizePreviewActor::AppearanceInspectorHideBody()
{
	bDebugHideBody = true;
	RefreshAppearanceInspectorCapture();
}

void AAvCustomizePreviewActor::AppearanceInspectorShowBody()
{
	bDebugHideBody = false;
	DebugHiddenSlots.Remove(EWorkerSlot::Body);
	RefreshAppearanceInspectorCapture();
}

void AAvCustomizePreviewActor::AppearanceInspectorClearSolo()
{
	AppearanceInspectorShowAll();
}

void AAvCustomizePreviewActor::AppearanceInspectorSetForceLod0(bool bForceLod0)
{
	bAppearanceInspectorForceLod0 = bForceLod0;
	ConfigurePreviewMeshes();
	RefreshAppearanceInspectorCapture();
}

FAvAppearanceInspectorSlotInfo AAvCustomizePreviewActor::GetAppearanceInspectorSlotInfo(EWorkerSlot Slot) const
{
	FAvAppearanceInspectorSlotInfo Info;
	Info.Slot = Slot;
	if (!AppearanceComponent)
	{
		return Info;
	}

	USkeletalMeshComponent* Component = AppearanceComponent->GetAppearanceInspectorSlotComponent(Slot);
	USkeletalMesh* Mesh = Component ? Component->GetSkeletalMeshAsset() : nullptr;
	if (!Component || !Mesh)
	{
		return Info;
	}

	Info.bPresent = true;
	Info.bVisible = Component->IsVisible() && !Component->bHiddenInGame;
	Info.MeshName = Mesh->GetName();
	Info.MeshObjectPath = Mesh->GetPathName();
	Info.MeshPackagePath = Mesh->GetOutermost()->GetName();
	Info.MaterialSlotCount = Component->GetNumMaterials();
	Info.ForcedLodModel = Component->GetForcedLOD();
	Info.PredictedLodLevel = Component->GetPredictedLODLevel();

	const TArray<FSkeletalMaterial>& AssetMaterials = Mesh->GetMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < Info.MaterialSlotCount; ++MaterialIndex)
	{
		UMaterialInterface* RuntimeMaterial = Component->GetMaterial(MaterialIndex);
		Info.RuntimeMaterialPaths.Add(RuntimeMaterial ? RuntimeMaterial->GetPathName() : TEXT("None"));
		UMaterialInterface* AssetMaterial = AssetMaterials.IsValidIndex(MaterialIndex)
			? AssetMaterials[MaterialIndex].MaterialInterface
			: nullptr;
		Info.MaterialAssetPaths.Add(AssetMaterial ? AssetMaterial->GetPathName() : TEXT("None"));
	}

	return Info;
}

namespace
{
	AAvCustomizePreviewActor* FindAppearanceAuditPreviewActor(UWorld* World)
	{
		if (!World)
		{
			return nullptr;
		}
		for (TActorIterator<AAvCustomizePreviewActor> It(World); It; ++It)
		{
			return *It;
		}
		return nullptr;
	}

	FSoftObjectPath MakeAppearanceAuditExactPath(const FString& PackageOrObjectPath)
	{
		if (PackageOrObjectPath.IsEmpty() ||
			PackageOrObjectPath.Equals(TEXT("None"), ESearchCase::IgnoreCase))
		{
			return FSoftObjectPath();
		}
		if (PackageOrObjectPath.Contains(TEXT(".")))
		{
			return FSoftObjectPath(PackageOrObjectPath);
		}
		return FSoftObjectPath(PackageOrObjectPath + TEXT(".") +
			FPackageName::GetLongPackageAssetName(PackageOrObjectPath));
	}

	void SetAppearanceAuditSlot(
		FWorkerAppearance& Appearance,
		EWorkerSlot Slot,
		const FString& PackageOrObjectPath)
	{
		const FSoftObjectPath ExactPath = MakeAppearanceAuditExactPath(PackageOrObjectPath);
		if (ExactPath.IsNull())
		{
			Appearance.Clear(Slot);
		}
		else
		{
			Appearance.Set(Slot, TSoftObjectPtr<USkeletalMesh>(ExactPath));
		}
	}

	void RunAppearanceAuditSetPreview(const TArray<FString>& Args, UWorld* World)
	{
		AAvCustomizePreviewActor* PreviewActor = FindAppearanceAuditPreviewActor(World);
		if (!PreviewActor || Args.Num() < 3)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvAppearanceAudit] SetPreview failed. Usage: Av.AppearanceAudit.SetPreview <Torso|None> <Gloves|None> <FullOutfit|None> [Lower|None]"));
			return;
		}

		FWorkerAppearance Appearance = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
		SetAppearanceAuditSlot(Appearance, EWorkerSlot::Torso, Args[0]);
		SetAppearanceAuditSlot(Appearance, EWorkerSlot::Gloves, Args[1]);
		SetAppearanceAuditSlot(Appearance, EWorkerSlot::FullOutfit, Args[2]);
		SetAppearanceAuditSlot(
			Appearance,
			EWorkerSlot::Legs,
			Args.Num() >= 4 ? Args[3] : TEXT("None"));

		PreviewActor->SetPreviewAppearance(Appearance);
		PreviewActor->SetPreviewYaw(0.f);
		PreviewActor->AppearanceInspectorShowAll();
		PreviewActor->AppearanceInspectorSetForceLod0(true);
		UE_LOG(LogTemp, Warning,
			TEXT("[AvAppearanceAudit] SetPreview PreviewOnly=true AppearanceMutation=false Autosave=false Stable=%s"),
			*Appearance.ToStableDebugString());
	}

	void RunAppearanceAuditSetIdleTime(const TArray<FString>& Args, UWorld* World)
	{
		AAvCustomizePreviewActor* PreviewActor = FindAppearanceAuditPreviewActor(World);
		UWorkerAppearanceComponent* Appearance =
			PreviewActor ? PreviewActor->FindComponentByClass<UWorkerAppearanceComponent>() : nullptr;
		USkeletalMeshComponent* Body = Appearance ? Appearance->GetBodyComponent() : nullptr;
		if (!PreviewActor || !Body || Args.IsEmpty())
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvAppearanceAudit] SetIdleTime failed. Usage: Av.AppearanceAudit.SetIdleTime <Seconds>"));
			return;
		}

		const float TimeSeconds = FMath::Max(0.f, FCString::Atof(*Args[0]));
		Body->bPauseAnims = true;
		Body->SetPosition(TimeSeconds, false);
		Body->RefreshBoneTransforms();
		Body->UpdateBounds();
		Body->MarkRenderStateDirty();
		PreviewActor->AppearanceInspectorShowAll();
		UE_LOG(LogTemp, Warning,
			TEXT("[AvAppearanceAudit] IdleTime=%.3f Paused=true PreviewOnly=true AppearanceMutation=false Autosave=false"),
			TimeSeconds);
	}

	void RunAppearanceAuditResumeIdle(const TArray<FString>&, UWorld* World)
	{
		AAvCustomizePreviewActor* PreviewActor = FindAppearanceAuditPreviewActor(World);
		UWorkerAppearanceComponent* Appearance =
			PreviewActor ? PreviewActor->FindComponentByClass<UWorkerAppearanceComponent>() : nullptr;
		USkeletalMeshComponent* Body = Appearance ? Appearance->GetBodyComponent() : nullptr;
		if (!Body)
		{
			UE_LOG(LogTemp, Error, TEXT("[AvAppearanceAudit] ResumeIdle failed: preview Body not found"));
			return;
		}
		Body->bPauseAnims = false;
		Body->Play(true);
		UE_LOG(LogTemp, Warning,
			TEXT("[AvAppearanceAudit] IdleResumed=true PreviewOnly=true AppearanceMutation=false Autosave=false"));
	}

	FAutoConsoleCommandWithWorldAndArgs GAppearanceAuditSetPreviewCommand(
		TEXT("Av.AppearanceAudit.SetPreview"),
		TEXT("Development-only deterministic preview appearance: Torso Gloves FullOutfit [Lower]. Use None for an empty slot."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&RunAppearanceAuditSetPreview));

	FAutoConsoleCommandWithWorldAndArgs GAppearanceAuditSetIdleTimeCommand(
		TEXT("Av.AppearanceAudit.SetIdleTime"),
		TEXT("Development-only deterministic preview idle time in seconds."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&RunAppearanceAuditSetIdleTime));

	FAutoConsoleCommandWithWorldAndArgs GAppearanceAuditResumeIdleCommand(
		TEXT("Av.AppearanceAudit.ResumeIdle"),
		TEXT("Resume the development-only preview idle animation after deterministic sampling."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&RunAppearanceAuditResumeIdle));
}
#endif
