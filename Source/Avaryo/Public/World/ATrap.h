#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATrap.generated.h"

class AAvaryoCharacter;
class UPointLightComponent;
class USphereComponent;
class UStaticMeshComponent;
class USoundBase;

/**
 * Растяжка-шумелка (§18 «предметы-ловушки»): монтёр ставит её из быстрого слота.
 * Через ArmDelay взводится (мигает), и при пересечении кем угодно — включая своих —
 * громко срабатывает: шум на всю карту, паника по радиусу, вспышка и тряска камеры.
 * Одноразовая. Поставивший защищён первые PlacerGraceTime секунд, чтобы успеть отойти.
 */
UCLASS()
class AVARYO_API ATrap : public AActor
{
	GENERATED_BODY()

public:
	ATrap();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Корпус-заглушка (банка/коробок). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trap")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Зона срабатывания. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trap")
	TObjectPtr<USphereComponent> TriggerSphere;

	/** Лампочка: мигает на взводе, ярко вспыхивает при срабатывании. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trap")
	TObjectPtr<UPointLightComponent> IndicatorLight;

	/** Через сколько секунд ловушка встаёт на взвод. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trap")
	float ArmDelay;

	/** Радиус зоны срабатывания, см. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trap")
	float TriggerRadius;

	/** Громкость шума при срабатывании (для будущего монстра-слухача). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trap")
	float NoiseLoudness;

	/** Скачок паники у всех в радиусе. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trap")
	float PanicAmount;

	/** Поставивший не подрывает ловушку первые столько секунд после взвода. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trap")
	float PlacerGraceTime;

	UFUNCTION(BlueprintPure, Category="Trap") bool IsArmed() const { return bArmed; }

	/** Звук установки растяжки. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trap|Audio")
	TObjectPtr<USoundBase> PlaceSound;

	/** Звук срабатывания растяжки. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trap|Audio")
	TObjectPtr<USoundBase> SnapSound;

protected:
	/** Взведена (реплицируется для мигания индикатора). */
	UPROPERTY(Replicated)
	bool bArmed;

	bool bTriggered;
	bool bFlashing; // локально на каждой машине: вспышка отыгрывает — Tick не перетирает её блинком (кооп-фикс)
	float ArmedAtTime; // момент взвода (для grace поставившего)

	/** Сработать: шум, паника, вспышка, тряска. Только сервер. */
	void Spring(AAvaryoCharacter* TriggeredBy);

	/** Вспышка индикатора у всех машин. Reliable: одношот-фидбэк (вспышка/тряска/щелчок), актор гибнет через 0.3с —
	 *  Unreliable-дроп лишал клиента всей реакции ловушки без второго шанса (CODE_AUDIT3 #16). */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFlash();
};
