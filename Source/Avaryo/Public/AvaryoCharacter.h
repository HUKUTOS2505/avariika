#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AvaryoCharacter.generated.h"

class APickupItem;
class ARepairable;
class AToilet;
class UCameraComponent;
class UFlashlightComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
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

	/** Метнуть предмет из рук по прицелу (передать через провал / запулить). */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Inventory")
	void ThrowItem();

	/** G нажата: в мини-игре/туалете — выйти; иначе начать «зарядку» броска. */
	void OnThrowKeyPressed();
	/** G отпущена: метнуть с силой по времени удержания (короткий тап — слабо, удержание — сильно). */
	void OnThrowKeyReleased();

	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory") bool IsChargingThrow() const { return bChargingThrow; }
	/** Прогресс зарядки броска 0..1 (для HUD-полоски). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Inventory") float GetThrowChargeAlpha() const;

	/** Зарегистрировать «свой шум» (для шумомера HUD): громкость 0..1, затухает за ~0.8с. */
	UFUNCTION(BlueprintCallable, Category="Avaryo|Noise") void RegisterSelfNoise(float Loudness);
	/** Текущий уровень своего шума 0..1 (с затуханием) — для HUD-шумомера. */
	UFUNCTION(BlueprintPure, Category="Avaryo|Noise") float GetSelfNoise01() const;

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

	/** Биотуалет под прицелом (для подсказки в HUD). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Interact")
	AToilet* GetFocusedToilet() const { return FocusedToilet; }

	/** Идёт ли «процесс» в биотуалете (мини-игра). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Interact")
	bool IsUsingToilet() const { return CurrentToilet != nullptr; }

	UFUNCTION(BlueprintPure, Category="Avaryo|Interact")
	AToilet* GetCurrentToilet() const { return CurrentToilet; }

	/** Заблокирован ли ввод (сидим в туалете / чиним щиток в мини-игре). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Interact")
	bool IsInteractionLocked() const { return bInteractionLocked; }

	/** Заблокировать/освободить движение и камеру на время мини-игры. Только сервер. */
	void SetInteractionLocked(bool bNewLocked);

	/** Удар (толчок/взрыв) вышибает тяжёлый предмет из рук и роняет несомого раненого. Только сервер. */
	void FumbleHeavy();

	/** Скользит ли сейчас по пене (для «Акта» и т.п.). */
	bool IsSlipping() const { return bSlipping; }

	/** Повернуть камеру владельца (сесть спиной в туалет). */
	UFUNCTION(Client, Reliable)
	void ClientSetControlYaw(float NewYaw);

	// ---------- Оператор: нагрудные камеры ----------

	/** Открыть/закрыть монитор оператора (Tab). Работает только в зоне ГАЗели. */
	void ToggleMonitor();

	UFUNCTION(BlueprintPure, Category="Avaryo|Operator")
	bool IsMonitorOpen() const { return bMonitorOpen; }

	/** Можно ли смотреть камеры: стоим в зоне ГАЗели и не ранены. */
	UFUNCTION(BlueprintPure, Category="Avaryo|Operator")
	bool CanUseMonitor() const;

	/** Картинка с нагрудной камеры этого монтёра (для монитора оператора). */
	UFUNCTION(BlueprintPure, Category="Avaryo|Operator")
	UTextureRenderTarget2D* GetChestCamTarget() const { return ChestCamTarget; }

	/** Нагрудная камера. Захват включается локально, только пока кто-то смотрит монитор. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Avaryo|Operator")
	TObjectPtr<USceneCaptureComponent2D> ChestCamera;

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

	/** Потратить заряд предмета в руках (этапы ремонта: вставить кабель/предохранитель, залить канистру). Только сервер. */
	void ConsumeHeldItemCharge();

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

	/** Биотуалет под прицелом. Считается локально в Tick. */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Avaryo|Interact")
	TObjectPtr<AToilet> FocusedToilet;

	/** Где идёт «процесс» (сервер пишет, реплицируется для HUD). */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Avaryo|Interact")
	TObjectPtr<AToilet> CurrentToilet;

	/** Рендер-таргет нагрудной камеры (создаётся в BeginPlay, не реплицируется). */
	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> ChestCamTarget;

	/** Открыт ли монитор оператора (локальное состояние, не реплицируется). */
	bool bMonitorOpen;

	/** Таймер шороха волочения (слышно — задел под монстра). */
	float DragNoiseAccum;

	/** Включён ли сейчас бесконечный паник-шейк (локально). */
	bool bPanicShakeActive;

	/** Ввод заблокирован мини-игрой (сервер пишет, реплицируется). */
	UPROPERTY(Replicated)
	bool bInteractionLocked;

	/** Применён ли лок к контроллеру (локальное состояние для переходов). */
	bool bAppliedInputLock;

	/** Камера персонажа (из Blueprint), к ней крепится предмет в руках. */
	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> ViewCamera;

	/** Был ли ранен в прошлый кадр (для авто-сброса тяжёлого при ранении). */
	bool bWasWounded;

	/** Идёт ли распыление огнетушителя (только сервер). */
	bool bSprayingHeld;
	float SprayDrainAccum;  // накопитель дробного расхода зарядов
	float SprayNoiseAccum;  // таймер шума для будущего монстра-слухача
	float SprayFoamAccum;   // таймер спавна луж пены при распылении
	float FootstepNoiseAccum; // таймер шума шагов при беге

	/** Скользит ли сейчас по пене (реплицируется — клиент тоже снижает трение). */
	UPROPERTY(ReplicatedUsing=OnRep_Slipping)
	bool bSlipping;

	UFUNCTION()
	void OnRep_Slipping();

	// Дефолты движения, чтобы вернуть сцепление после пены
	bool bSlipDefaultsSaved;
	float SlipDefaultGroundFriction;
	float SlipDefaultBrakingDecel;

	/** Сервер: проверяет, стоит ли монтёр в луже пены, и переключает скольжение. */
	void UpdateFoamSlip(float DeltaSeconds);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Foam")
	float FoamSlipPanic;            // паника при попадании на пену

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Foam")
	float FoamFallChancePerSecond;  // шанс/сек навернуться, пока быстро скользишь

	/** Применить/снять «скользкое» трение на CharacterMovement (сервер и клиент). */
	void ApplySlipFriction(bool bOn);

	// ---------- Толчок/пинок товарища (§18 кооп-хаос) ----------
	/** Толкнуть того, на кого смотришь: в газ, на пену, с края, в растяжку. */
	void Shove();

	UFUNCTION(Server, Reliable)
	void ServerShove();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Shove")
	float ShoveRange;        // дальность толчка, см

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Shove")
	float ShoveForce;        // горизонтальный импульс толкаемому

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Shove")
	float ShoveUp;           // подброс вверх

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Shove")
	float ShovePanic;        // паника толкнутому

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Shove")
	float ShoveCooldownTime; // перезарядка толчка, сек

	float ShoveReadyTime;    // серверное время, когда снова можно толкать

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Shove")
	float ShoveFumbleChance; // шанс выбить тяжёлый предмет из рук толкнутого

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Shove")
	float HeavyBonkChance;   // шанс уронить тяжёлое себе на ногу при фумбле

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Shove")
	float HeavyBonkDamage;   // урон от удара по ноге

	// ---------- Споткнуться (§18 хаос/хоррор) ----------
	/** Спотыкается ли сейчас (реплицируется — клиент тоже сбрасывает скорость). */
	UPROPERTY(Replicated)
	bool bStumbling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Trip")
	float TripChancePerSecond; // базовый шанс споткнуться при беге, в секунду

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Trip")
	float TripDarkMultiplier;  // во сколько раз чаще без фонаря (в темноте)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Trip")
	float TripPanicMultiplier; // надбавка к шансу при полной панике

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Trip")
	float TripRecoverTime;     // длительность спотыкания, сек

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Trip")
	float TripSlowSpeed;       // скорость во время спотыкания

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Trip")
	float TripFumbleChance;    // шанс выронить активный лёгкий предмет при спотыкании

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Trip")
	float TripTiredScale;      // насколько усталость (низкая выносливость) повышает шанс споткнуться

	/** Запустить спотыкание: стан скорости, шум, реплики, фумбл. Только сервер. */
	void TriggerStumble();

	// ---------- Паника: тряска прицела + адреналин ----------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Panic")
	float PanicSwayAmount;       // амплитуда дрожи прицела при панике

	/** Камера-эффекты паники (шейк камеры + дрожь прицела). Сейчас ВЫКЛ для тестов; true — вернуть. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Panic")
	bool bPanicCameraEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Panic")
	float AdrenalineHealthThreshold; // ниже этого HP включается адреналин

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Panic")
	float AdrenalineSpeedMult;   // прибавка к скорости на адреналине

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Panic")
	float AdrenalinePanicPerSecond; // паника растёт, пока на адреналине

	float StumbleUntil;        // серверное время конца спотыкания

	/** Сервер: розыгрыш спотыкания при беге (темнота/паника повышают шанс). */
	void UpdateTrip(float DeltaSeconds);

	/** Каст применения предмета: осталось/всего, реплицируется для HUD. */
	UPROPERTY(Replicated)
	float UseCastRemaining;

	UPROPERTY(Replicated)
	float UseCastDuration;

	/** Насколько паника удлиняет применение предметов (1 = при 100% паники время ×2). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avaryo|Inventory")
	float ItemUsePanicScale = 1.f;

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

	/** Найти биотуалет под прицелом. */
	AToilet* FindFocusedToilet() const;

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

	/** Выложить активный предмет в мир: bThrown=false — уронить/поставить, true — метнуть. ChargeAlpha 0..1 множит силу. */
	void ReleaseHeldItem(bool bThrown, float ChargeAlpha = 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Inventory")
	float ThrowImpulseLight;  // сила броска лёгкого предмета (база, тап без зарядки)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Inventory")
	float ThrowImpulseHeavy;  // сила броска тяжёлого (летит слабее)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Inventory")
	float ThrowMaxSpreadDeg;  // макс. разброс броска при полной панике, градусы

	/** Зарядка броска: удержание дольше Min начинает копить силу, к Max — полная (×ThrowChargeMaxMult). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Inventory")
	float ThrowChargeMinTime = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Inventory")
	float ThrowChargeMaxTime = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avaryo|Inventory")
	float ThrowChargeMaxMult = 3.f; // во столько раз сильнее при полной зарядке

	bool bChargingThrow = false; // локально: зажата ли G для зарядки броска
	float ThrowPressTime = 0.f;  // локально: момент нажатия G

	float SelfNoiseLevel = 0.f;  // последний пик «своего шума» 0..1 (для шумомера)
	float SelfNoiseTime = 0.f;   // когда был пик (для затухания)
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
	void ServerThrowItem();

	UFUNCTION(Server, Reliable)
	void ServerThrowCharged(float ChargeAlpha);

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

	/** R на экране «Акта»: перезапуск смены. */
	void TryRestartRun();

	UFUNCTION(Server, Reliable)
	void ServerRequestRestart();

	// ---------- Дев-консоль: быстрый тест модулей ----------

	/** Выставить шкалу: `AvVital panic 100` (health/panic/stamina/bladder/smell, 0..100). */
	UFUNCTION(Exec)
	void AvVital(const FString& Which, float Value);

	/** Мгновенный санитарный инцидент (роняет биоснаряд, скачок паники/запаха). */
	UFUNCTION(Exec)
	void AvIncident();

	/** Дать «подозрительный комок» в руки (для теста биоснаряда). */
	UFUNCTION(Exec)
	void AvGiveBio();

	/** Принудительно включить дешёвый комплект (моргающий фонарь + помехи рации). */
	UFUNCTION(Exec)
	void AvCheapGear();

	/** Завершить забег и показать «Акт»: `AvFinish win` или `AvFinish lose`. */
	UFUNCTION(Exec)
	void AvFinish(const FString& Outcome);

	/** Квота диспетчера: `AvQuota 8000` включить, `AvQuota 0` выключить (песочница). */
	UFUNCTION(Exec)
	void AvQuota(int32 Target);

	/** Купить апгрейд инструмента: `AvUpgrade Welder` (Flashlight/Tester/Welder/Extinguisher/Radio). */
	UFUNCTION(Exec)
	void AvUpgrade(const FString& Tool);

	/** Открыть/закрыть экран магазина снаряжения (локально). */
	UFUNCTION(Exec)
	void AvShop();

	/** Дев-режим: вкл/выкл неуязвимость (для тестов — не умереть). Доливает здоровье при включении. */
	UFUNCTION(Exec)
	void AvGod();

	/** Дев: выдать предмет в свободный слот для теста: `AvGive coffee` (heal/calm/ext/radio/recharge/trap/light/bio). */
	UFUNCTION(Exec)
	void AvGive(const FString& What);

	/** Дев: качество предмета В РУКАХ (0.25..3): `AvToolQ 3` легче чинить, `AvToolQ 0.3` тяжелее. */
	UFUNCTION(Exec)
	void AvToolQ(float Scale);

	/** Дев: выдать ₽ в кассу конторы: `AvMoney 50000` (для теста магазина/апгрейдов/квоты). */
	UFUNCTION(Exec)
	void AvMoney(int32 Amount);

	/** Дев: задать уровень апгрейда напрямую (сравнить до/после, без денег): `AvSetGear Welder 1` / `AvSetGear Welder 3`. */
	UFUNCTION(Exec)
	void AvSetGear(const FString& Tool, int32 Level);

	/** Дев: задать заряд фонаря 0..100: `AvBattery 2` → быстро сядет (тест испуга при севшей батарее). */
	UFUNCTION(Exec)
	void AvBattery(float Pct);

	UFUNCTION(Server, Reliable)
	void ServerAvVital(const FString& Which, float Value);

	UFUNCTION(Server, Reliable)
	void ServerAvGiveBio();

	UFUNCTION(Server, Reliable)
	void ServerAvCheapGear();

	UFUNCTION(Server, Reliable)
	void ServerAvFinish(const FString& Outcome);

	UFUNCTION(Server, Reliable)
	void ServerAvQuota(int32 Target);

	UFUNCTION(Server, Reliable)
	void ServerAvUpgrade(const FString& Tool);

	UFUNCTION(Server, Reliable)
	void ServerAvGod();

	UFUNCTION(Server, Reliable)
	void ServerAvGive(const FString& What);

	UFUNCTION(Server, Reliable)
	void ServerAvToolQ(float Scale);

	UFUNCTION(Server, Reliable)
	void ServerAvMoney(int32 Amount);

	UFUNCTION(Server, Reliable)
	void ServerAvSetGear(const FString& Tool, int32 Level);

	UFUNCTION(Server, Reliable)
	void ServerAvBattery(float Pct);
};
