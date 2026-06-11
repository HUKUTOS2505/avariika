#include "UI/AvaryoCameraShakes.h"

#include "Shakes/PerlinNoiseCameraShakePattern.h"

UPanicCameraShake::UPanicCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UPerlinNoiseCameraShakePattern* Pattern = ObjectInitializer.CreateDefaultSubobject<UPerlinNoiseCameraShakePattern>(this, TEXT("Pattern"));
	Pattern->Duration = 1.1f;
	Pattern->BlendInTime = 0.2f;
	Pattern->BlendOutTime = 0.3f;
	Pattern->Pitch.Amplitude = 0.5f;
	Pattern->Pitch.Frequency = 9.f;
	Pattern->Yaw.Amplitude = 0.45f;
	Pattern->Yaw.Frequency = 7.f;
	Pattern->Roll.Amplitude = 0.25f;
	Pattern->Roll.Frequency = 5.f;
	SetRootShakePattern(Pattern);
	bSingleInstance = true; // ретриггер обновляет, а не стакает
}

UExplosionCameraShake::UExplosionCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UPerlinNoiseCameraShakePattern* Pattern = ObjectInitializer.CreateDefaultSubobject<UPerlinNoiseCameraShakePattern>(this, TEXT("Pattern"));
	Pattern->Duration = 0.7f;
	Pattern->BlendInTime = 0.02f;
	Pattern->BlendOutTime = 0.45f;
	Pattern->Pitch.Amplitude = 6.f;
	Pattern->Pitch.Frequency = 18.f;
	Pattern->Yaw.Amplitude = 4.f;
	Pattern->Yaw.Frequency = 15.f;
	Pattern->Roll.Amplitude = 3.f;
	Pattern->Roll.Frequency = 12.f;
	Pattern->LocationAmplitudeMultiplier = 1.f;
	Pattern->Z.Amplitude = 8.f;
	Pattern->Z.Frequency = 14.f;
	SetRootShakePattern(Pattern);
	bSingleInstance = true;
}
