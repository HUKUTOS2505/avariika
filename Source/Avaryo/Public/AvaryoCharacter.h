#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AvaryoCharacter.generated.h"

class APickupItem;
class ARepairable;
class UCameraComponent;
class UFlashlightComponent;
class UVitalsComponent;

/**
 * Базовый персонаж "Аварийки": налобный фонарик, шкалы (HP/паника/выносливость/туалет)
 * и инвентарь по концепту: 1 тяжёлый предмет в руках + 4 быстрых лёгких слота.
 *
 * Правило сварочника: тяжёлый предмет занимает руки. При переключении на лёгкий
 * слот он НЕ убирается в рюкзак, а опускается вниз (виден). Второй тяжёлый взять
 * нельзя — сначала поставь текущий (G). Тяжёлый груз замедляет и жрёт выносливость.
 */
UCLASS()
class AVARYO_API AAvaryoCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAvaryoCharacter();

	/** Количество слотов: 0 — тяжёлый (руки), 1-4 — лёгкие. */
	static constexpr int32 NumLightSlots = 4;
	static constexpr int32 NumSlots = NumLightSlots + 1;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool CanJumpInternal_Implementation() const override;

	// ---------- Инвентарь ----------

	/** Положить предмет в подходящий слот и взять в руки. Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void PickupItem(APickupItem* Item);

	/** Бросить/поставить предмет из рук (клавиша G). Тяжёлый ставится аккуратно. */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void DropItem();

	/** Переключить активный слот (клавиши 1-5). 0 — тяжёлый, 1-4 — лёгкие. */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void EquipSlot(int32 SlotIndex);

	/** Подобрать предмет под прицелом / рядом (клавиша E). */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void TryPickupNearbyItem();

	// ---------- Взаимодействие (E) ----------

	/** Нажатие E: подобрать предмет, иначе начать чинить объект под прицелом. */
	void OnInteractPressed();

	/** Отпускание E: прекратить починку (прогресс сохраняется). */
	void OnInteractReleased();

	/** Ремонтируемый объект под прицелом (для подсказки в HUD). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Repair")
	ARepairable* GetFocusedRepairable() const { return FocusedRepairable; }

	/** Чинит ли сейчас (держит E у объекта). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Repair")
	bool IsRepairing() const { return CurrentRepairable != nullptr; }

	UFUNCTION(BlueprintPure, Category="Avaryo|Repair")
	ARepairable* GetCurrentRepairable() const { return CurrentRepairable; }

	// ---------- Перетаскивание раненого ----------

	/** Кого тащу (E на раненом — схватил, повторное E — отпустил). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Drag")
	AAvaryoCharacter* GetDraggedTeammate() const { return DraggedTeammate; }

	/** Кто тащит меня (я ранен). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Drag")
	AAvaryoCharacter* GetDraggedBy() const { return DraggedBy; }

	/** Раненый тиммейт под прицелом/рядом (для подсказки в HUD). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Drag")
	AAvaryoCharacter* GetFocusedWounded() const { return FocusedWounded; }

	UFUNCTION(BlueprintPure, Category="Avaryo|Drag")
	bool IsDragging() const { return DraggedTeammate != nullptr; }

	/** Использовать предмет в руках (мгновенные эффекты): аптечка лечит, сигареты успокаивают. */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void UseHeldItem();

	/** Нажатие кнопки использования (ЛКМ / R): мгновенный эффект или начало распыления. */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void BeginUseHeldItem();

	/** Отпускание кнопки использования — отмена применения / конец распыления. */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void EndUseHeldItem();

	/** Идёт ли сейчас применение предмета (для HUD-прогресса). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	bool IsUsingItem() const { return UseCastRemaining > 0.f; }

	/** Прогресс применения 0..1. */
	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	float GetUseProgress() const { return UseCastDuration > 0.f ? 1.f - UseCastRemaining / UseCastDuration : 0.f; }

	/** Начать передачу (ПКМ): вытянуть предмет вперёд, тиммейт заберёт его по E. */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void BeginOfferItem();

	/** Закончить передачу (отпустил ПКМ). */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void EndOfferItem();

	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	bool IsOffering() const { return bOffering; }

	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	APickupItem* GetHeldItem() const;

	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	APickupItem* GetItemInSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	int32 GetActiveSlot() const { return ActiveSlot; }

	/** Предмет, на который игрок сейчас смотрит (для подсказки в HUD). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	APickupItem* GetFocusedItem() const { return FocusedItem; }

	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	bool HasItem() const { return GetHeldItem() != nullptr; }

	/** Можно ли сейчас подобрать предмет: есть место, тяжёлый не берётся раненым/вторым. */
	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	bool CanPickupItem(APickupItem* Item) const;

	/** Несёт ли тяжёлый предмет (в любом состоянии — в руках или опущен). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory")
	bool IsCarryingHeavy() const { return HeavySlot != nullptr; }

	// ---------- Прочее ----------

	UFUNCTION(BlueprintCallable, Category="Avaryo|Flashlight")
	void ToggleFlashlight();

	void StartSprint();
	void StopSprint();
	void StartCrouchInput();
	void StopCrouchInput();

	/** Налобный фонарик. Источник света (SpotLight) добавляется в Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Avaryo")
	TObjectPtr<UFlashlightComponent> FlashlightComponent;

	/** Шкалы: HP, паника, выносливость, туалет. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Avaryo")
	TObjectPtr<UVitalsComponent> VitalsComponent;

	/** Дальность подбора (трейс из камеры), см. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Inventory")
	float PickupRange;

	// Скорости движения
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Movement")
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Movement")
	float SprintSpeed;

	/** Множитель скорости с тяжёлым предметом. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Movement")
	float HeavyCarryMultiplier;

	/** Скорость ползания раненым. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Movement")
	float CrawlSpeed;

	/** Множитель скорости после "санитарного инцидента". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Movement")
	float IncidentSlowMultiplier;

	/** Радиус, в котором аптечкой можно поднять раненого тиммейта. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Inventory")
	float ReviveRange;

	/** Множитель скорости, пока тащишь раненого. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Movement")
	float DragSpeedMultiplier;

protected:
	/** Тяжёлый слот (один предмет), реплицируется. */
	UPROPERTY(ReplicatedUsing=OnRep_Inventory, BlueprintReadOnly, Category="Avaryo|Inventory")
	TObjectPtr<APickupItem> HeavySlot;

	/** Четыре лёгких слота, реплицируются. */
	UPROPERTY(ReplicatedUsing=OnRep_Inventory, BlueprintReadOnly, Category="Avaryo|Inventory")
	TArray<TObjectPtr<APickupItem>> LightSlots;

	/** Активный слот (что в руках): 0 — тяжёлый, 1-4 — лёгкие. */
	UPROPERTY(ReplicatedUsing=OnRep_Inventory, BlueprintReadOnly, Category="Avaryo|Inventory")
	int32 ActiveSlot;

	/** Предмет под прицелом. Считается локально в Tick, не реплицируется. */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Avaryo|Inventory")
	TObjectPtr<APickupItem> FocusedItem;

	/** Ремонтируемый объект под прицелом. Считается локально в Tick. */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Avaryo|Repair")
	TObjectPtr<ARepairable> FocusedRepairable;

	/** Что чиню сейчас (сервер пишет, реплицируется для HUD). */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Avaryo|Repair")
	TObjectPtr<ARepairable> CurrentRepairable;

	/** Раненый, которого тащу (сервер пишет, реплицируется). */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Avaryo|Drag")
	TObjectPtr<AAvaryoCharacter> DraggedTeammate;

	/** Кто тащит меня (обратная ссылка, реплицируется для HUD раненого). */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Avaryo|Drag")
	TObjectPtr<AAvaryoCharacter> DraggedBy;

	/** Раненый под прицелом. Считается локально в Tick. */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Avaryo|Drag")
	TObjectPtr<AAvaryoCharacter> FocusedWounded;

	/** Таймер шороха волочения (слышно — задел под монстра). */
	float DragNoiseAccum;

	/** Камера персонажа (из Blueprint), к ней крепится предмет в руках. */
	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> ViewCamera;

	/** Был ли ранен в прошлый кадр (для авто-сброса тяжёлого при ранении). */
	bool bWasWounded;

	/** Идёт ли распыление огнетушителя (только сервер). */
	bool bSprayingHeld;
	float SprayDrainAccum;  // накопитель дробного расхода зарядов
	float SprayNoiseAccum;  // таймер шума для будущего монстра-слухача
	float FootstepNoiseAccum; // таймер шума шагов при беге

	/** Каст применения предмета: осталось/всего, реплицируется для HUD. */
	UPROPERTY(Replicated)
	float UseCastRemaining;

	UPROPERTY(Replicated)
	float UseCastDuration;

	/** Вытянул ли предмет вперёд для передачи. */
	UPROPERTY(ReplicatedUsing=OnRep_Offering, BlueprintReadOnly, Category="Avaryo|Inventory")
	bool bOffering;

	UFUNCTION()
	void OnRep_Offering();

	/** Остановить распыление и погасить визуал струи. */
	void StopSpraying();

	/** Серверная часть распыления: расход зарядов, отталкивание предметов, шум. */
	void TickSpray(float DeltaSeconds);

	/** Отмена текущего применения (отпустил кнопку, сменил слот, ранен). */
	void CancelUseCast();

	/** Тик применения: по завершении срабатывает эффект. */
	void TickUseCast(float DeltaSeconds);

	/** Сработавший эффект предмета (вынесен из UseHeldItem). */
	void ApplyItemEffect(APickupItem* Item);

	/** Есть ли смысл применять (кого лечить, есть ли паника) — иначе каст не начинаем. */
	bool CanApplyEffect(APickupItem* Item) const;

	/** Раненый тиммейт рядом, иначе сам (если не здоров), иначе nullptr. */
	AAvaryoCharacter* FindHealTarget() const;

	UFUNCTION()
	void OnRep_Inventory();

	/** Найти предмет под прицелом (трейс из камеры) или ближайший рядом. */
	APickupItem* FindFocusedItem() const;

	/** Найти сломанный ремонтируемый объект под прицелом (трейс из камеры). */
	ARepairable* FindFocusedRepairable() const;

	/** Серверная логика нажатия/отпускания E. */
	void InteractPressedAuth();
	void InteractReleasedAuth();

	/** Найти раненого тиммейта под прицелом или рядом (для драга). */
	AAvaryoCharacter* FindFocusedWoundedTeammate() const;

	/** Можно ли начать тащить этого раненого. */
	bool CanDrag(const AAvaryoCharacter* Wounded) const;

	/** Отпустить раненого. Только сервер. */
	void ReleaseDraggedTeammate();

	/** Серверный тик волочения: тянем раненого за собой, шуршим. */
	void TickDrag(float DeltaSeconds);

	/** Можно ли продолжать тащить (встал, сам ранен, взял тяжёлое, застрял). */
	bool CanDragContinue() const;

	/** Применить состояние инвентаря: активный в руки, тяжёлый опустить, лёгкие спрятать. */
	void RefreshHeldItem();
	void HoldItem(APickupItem* Item);
	void CarryHeavyLowered(APickupItem* Item);
	void StashItem(APickupItem* Item);

	/** Потратить заряд; при нуле предмет исчезает из мира и слота. */
	void ConsumeCharge(APickupItem* Item);
	void ClearSlotFor(APickupItem* Item);

	/** Пересчитать скорость движения из состояния (тяжесть, бег, ранение, дебафы). */
	void RefreshMoveSpeed();

	// Обёртки для биндов клавиш 1-5
	void EquipSlot1() { EquipSlot(0); }
	void EquipSlot2() { EquipSlot(1); }
	void EquipSlot3() { EquipSlot(2); }
	void EquipSlot4() { EquipSlot(3); }
	void EquipSlot5() { EquipSlot(4); }

	// Серверные RPC: ввод происходит на клиенте, а реплицируемое состояние меняет сервер
	UFUNCTION(Server, Reliable)
	void ServerToggleFlashlight();

	UFUNCTION(Server, Reliable)
	void ServerTryPickupNearbyItem();

	UFUNCTION(Server, Reliable)
	void ServerDropItem();

	UFUNCTION(Server, Reliable)
	void ServerEquipSlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerUseHeldItem();

	UFUNCTION(Server, Reliable)
	void ServerBeginUseHeldItem();

	UFUNCTION(Server, Reliable)
	void ServerEndUseHeldItem();

	UFUNCTION(Server, Reliable)
	void ServerBeginOfferItem();

	UFUNCTION(Server, Reliable)
	void ServerEndOfferItem();

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bNewSprinting);

	UFUNCTION(Server, Reliable)
	void ServerInteractPressed();

	UFUNCTION(Server, Reliable)
	void ServerInteractReleased();
};
