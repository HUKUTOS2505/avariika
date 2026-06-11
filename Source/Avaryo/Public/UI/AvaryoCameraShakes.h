#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "AvaryoCameraShakes.generated.h"

/** Лёгкая дрожь камеры при высокой панике (перезапускается, пока паника держится). */
UCLASS()
class AVARYO_API UPanicCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UPanicCameraShake(const FObjectInitializer& ObjectInitializer);
};

/** Сильный толчок при взрыве газа рядом. */
UCLASS()
class AVARYO_API UExplosionCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UExplosionCameraShake(const FObjectInitializer& ObjectInitializer);
};
