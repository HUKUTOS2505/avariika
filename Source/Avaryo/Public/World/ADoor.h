#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ADoor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USoundBase;
class AAvaryoCharacter;

/**
 * Распашная дверь. E у двери — распахнуть/закрыть (полотно поворачивается на петле).
 * Заменяет статичные двери дома (SM_Door_*), которые иначе перекрывают проход.
 * Логика на сервере, состояние (bOpen) реплицируется; поворот плавно лерпится у всех.
 */
UCLASS()
class AVARYO_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	ADoor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Петля — ось вращения (корень). Ставится на край дверного проёма. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door")
	TObjectPtr<USceneComponent> Hinge;

	/** Полотно двери (меш). Привязано к петле; вращается вместе с ней. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	/** Зона взаимодействия (E рядом/под прицелом). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door")
	TObjectPtr<UBoxComponent> Zone;

	/** На сколько градусов распахивается. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	float OpenAngle = 95.f;

	/** Скорость распахивания (плавность). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	float OpenSpeed = 4.5f;

	/** Смещение полотна от петли по Y (полудлина двери, чтобы вращалось вокруг края). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	float LeafOffsetY = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	TObjectPtr<USoundBase> OpenSound;

	UFUNCTION(BlueprintPure, Category="Door")
	bool IsOpen() const { return bOpen; }

	/** Игрок нажал E у двери — распахнуть/закрыть. Только сервер. */
	void ToggleBy(AAvaryoCharacter* Who);

protected:
	/** Открыта ли (реплицируется). */
	UPROPERTY(ReplicatedUsing=OnRep_Open, BlueprintReadOnly, Category="Door")
	bool bOpen = false;

	UFUNCTION()
	void OnRep_Open();

	/** Текущий угол распахивания (лерпится в Tick на всех машинах). */
	float CurrentAngle = 0.f;

	float LastToggleTime = 0.f; // дебаунс E (анти-спам распахивания/звука)
};
