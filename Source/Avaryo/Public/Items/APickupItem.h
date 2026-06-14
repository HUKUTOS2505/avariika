#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APickupItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/** Размер предмета: тяжёлый занимает руки, лёгкие лежат в четырёх быстрых слотах. */
UENUM(BlueprintType)
enum class EItemSize : uint8
{
	Light	UMETA(DisplayName = "Лёгкий"),
	Heavy	UMETA(DisplayName = "Тяжёлый")
};

/** Что делает предмет при использовании (ЛКМ / R). */
UENUM(BlueprintType)
enum class EItemEffect : uint8
{
	None		UMETA(DisplayName = "Ничего"),
	Heal		UMETA(DisplayName = "Лечит (себя или раненого рядом)"),
	Calm		UMETA(DisplayName = "Снижает панику"),
	Extinguish	UMETA(DisplayName = "Огнетушитель (распыление, пока зажата кнопка)"),
	Recharge	UMETA(DisplayName = "Заряжает налобный фонарь"),
	Radio		UMETA(DisplayName = "Рация (тумблер вкл/выкл, шумит)"),
	DeployTrap	UMETA(DisplayName = "Ставит растяжку-шумелку"),
	ThrowBio	UMETA(DisplayName = "Метает биологический снаряд"),
	DeployLight	UMETA(DisplayName = "Ставит переносной прожектор"),
	Drink		UMETA(DisplayName = "Кофе/термос: восстанавливает выносливость")
};

/**
 * Подбираемый предмет. Сам ничего не подбирает:
 * персонаж находит его трейсом из камеры (или по радиусу) и берёт по кнопке E.
 */
UCLASS()
class AVARYO_API APickupItem : public AActor
{
	GENERATED_BODY()

public:
	APickupItem();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Визуал предмета. Корневой компонент, чтобы физика при сбросе двигала весь актор. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Радиус подбора — фолбэк, если игрок стоит рядом, но не смотрит прямо на предмет. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<USphereComponent> TriggerSphere;

	/** Визуал струи огнетушителя (конус). Меш назначается в Blueprint, по умолчанию скрыт. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup|Use")
	TObjectPtr<UStaticMeshComponent> SprayCone;

	/** Название предмета для HUD ("[E] Подобрать: ..."). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	FText DisplayName;

	/** Тяжёлый или лёгкий. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	EItemSize ItemSize;

	/** Эффект при использовании (ЛКМ). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup|Use")
	EItemEffect ItemEffect;

	/** Тег инструмента ("Wrench", "Welder"...). Ремонтируемые объекты сверяют его с RequiredTool. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup|Use")
	FName ToolTag;

	/** Сила эффекта (сколько HP лечит / паники снимает). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup|Use")
	float EffectMagnitude;

	/** Качество инструмента для мини-игр починки: >1 легче (шире зона, медленнее курсор),
	 *  <1 тяжелее. 1 — нейтрально. Апгрейды магазина поднимают, дешёвый комплект — опускает.
	 *  Реплицируется: сервер может менять в рантайме, клиент должен видеть то же. */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Pickup|Use", meta=(ClampMin="0.25", ClampMax="3.0"))
	float ToolQualityScale = 1.f;

	/** Запас использований. -1 — бесконечно. При нуле предмет исчезает. */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Pickup|Use")
	int32 Charges;

	/** Время применения, сек (кнопку нужно держать). 0 — мгновенно. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup|Use")
	float UseCastTime;

	/** Множитель скорости ходьбы во время применения (0.5 — вдвое медленнее). Бег и прыжок запрещены всегда. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup|Use", meta=(ClampMin="0.0", ClampMax="1.0"))
	float CastSpeedMultiplier;

	/** Положение предмета в руках (активный слот) относительно камеры. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	FVector HoldOffset;

	/** Доворот предмета в руках (некоторые модели смотрят не туда). По умолчанию 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	FRotator HoldRotation;

	/** Положение тяжёлого предмета, когда он "опущен" (активен лёгкий слот). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	FVector CarryOffset;

	/** Скорость декоративного вращения, град/сек. 0 — не вращать. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	float RotationSpeed;

	/** Включить/выключить визуал распыления (сервер; клиентам придёт через репликацию). */
	UFUNCTION(BlueprintCallable, Category="Pickup|Use")
	void SetSpraying(bool bNewSpraying);

	UFUNCTION(BlueprintPure, Category="Pickup|Use")
	bool IsSpraying() const { return bSpraying; }

	/** Тумблер (рация): включить/выключить. Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Pickup|Use")
	void SetToggledOn(bool bNewOn);

	UFUNCTION(BlueprintPure, Category="Pickup|Use")
	bool IsToggledOn() const { return bToggledOn; }

protected:
	/** Идёт ли распыление (для огнетушителя), реплицируется. */
	UPROPERTY(ReplicatedUsing=OnRep_Spraying)
	bool bSpraying;

	/** Включён ли тумблер (рация), реплицируется. */
	UPROPERTY(Replicated)
	bool bToggledOn;

	/** Таймер шума включённой рации (сервер). */
	float ToggleNoiseAccum;

	UFUNCTION()
	void OnRep_Spraying();
};
