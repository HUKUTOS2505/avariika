#include "UI/AvaryoCameraShakes.h"

#include "Shakes/PerlinNoiseCameraShakePattern.h"

UPanicCameraShake::UPanicCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UPerlinNoiseCameraShakePattern* Pattern = ObjectInitializer.CreateDefaultSubobject<UPerlinNoiseCameraShakePattern>(this, TEXT("Pattern"));
	// Бесконечная плавная дрожь: запускается один раз при входе в панику,
	// останавливается при выходе. Рестарты каждые N секунд давали рывок вбок.
	Pattern->Duration = 0.f;
	Pattern->BlendInTime = 0.6f;
	Pattern->BlendOutTime = 0.5f;
	Pattern->Pitch.Amplitude = 0.35f;
	Pattern->Pitch.Frequency = 8.f;
	Pattern->Yaw.Amplitude = 0.25f;
	Pattern->Yaw.Frequency = 6.f;
	Pattern->Roll.Amplitude = 0.15f;
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
