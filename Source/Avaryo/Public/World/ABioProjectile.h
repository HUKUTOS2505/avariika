#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABioProjectile.generated.h"

class AAvaryoCharacter;
class UStaticMeshComponent;

/**
 * Биологический снаряд (§15 «биологический снаряд»): метается из рук физикой.
 * При попадании в монтёра или о землю «шлёпает»: запах + паника по радиусу,
 * громкий шум (отвлекает будущего монстра на запах), и несколько секунд
 * «амбре-зоны». Двусмысленное оружие: легко зацепить своих.
 */
UCLASS()
class AVARYO_API ABioProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABioProjectile();

	virtual void Tick(float DeltaSeconds) override;

	/** Корпус-комок (физика). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Bio")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Начальная скорость броска, см/с. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bio")
	float ThrowSpeed;

	/** Радиус прямого попадания в монтёра, см. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bio")
	float HitRadius;

	/** Радиус «шлепка»: кому достанется запах и паника, см. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bio")
	float SplatRadius;

	/** Запах прямой жертве / в радиусе. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bio")
	float SplatSmell;

	/** Паника в радиусе шлепка. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bio")
	float SplatPanic;

	/** Сколько секунд «амбре-зона» живёт после шлепка. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bio")
	float LingerTime;

	/** Запустить снаряд из точки по направлению (сервер). */
	void Launch(const FVector& Direction);

protected:
	bool bSplatted;
	float SettledTime;   // сколько времени снаряд почти не движется
	float LingerRemaining;
	float StinkNoiseAccum;

	/** Шлёпок: запах + паника по радиусу, шум, реплика диспетчера. */
	void Splat(AAvaryoCharacter* DirectHit);
};
