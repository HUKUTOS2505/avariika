#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARepairable.generated.h"

class AAvaryoCharacter;
class UPointLightComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRepairFinished, ARepairable*, Repairable, AAvaryoCharacter*, FinishedBy);

/** Тип мини-игры починки. None — обычное удержание E. */
UENUM(BlueprintType)
enum class ERepairMinigameType : uint8
{
	None,    // держать E, прогресс растёт сам
	Cursor,  // щиток: E по бегающему курсору, промах бьёт током
	Valve,   // труба: размеренные тыки E докручивают вентиль, частить = срыв резьбы
	Starter  // генератор: держать E (натяжение шнура), отпустить в зелёном окне
};

/**
 * Ремонтируемый объект (щиток, труба, генератор) — суть работы бригады.
 * Игрок смотрит на сломанный объект и держит E: прогресс растёт, по завершении
 * объект починен. Прогресс не сбрасывается при срыве — можно вернуться и дочинить.
 * Вся логика на сервере, состояние реплицируется. Починка шумит (задел под монстра).
 */
UCLASS()
class AVARYO_API ARepairable : public AActor
{
	GENERATED_BODY()

public:
	ARepairable();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Визуал объекта. Меш-заглушка назначается в Blueprint или скриптом размещения. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Repair")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Табличка статуса над объектом: "СЛОМАНО" / "ОК" + проценты починки. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Repair")
	TObjectPtr<UTextRenderComponent> StatusText;

	/** Аварийная лампа: красная пульсация пока сломан — видно в темноте. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Repair")
	TObjectPtr<UPointLightComponent> AlarmLight;

	/** Название для HUD и таблички ("Щиток", "Газовая труба"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair")
	FText DisplayName;

	/** Сколько секунд суммарно держать E. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair")
	float RepairDuration;

	/** Какой инструмент нужен в руках (ToolTag предмета). NAME_None — чинится руками. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair")
	FName RequiredTool;

	/** Дальше этой дистанции починка срывается, см. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair")
	float RepairRange;

	/** Пока сломан — травит газ: открытый огонь рядом (курение) вызывает взрыв. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Gas")
	bool bLeaksGasWhenBroken;

	/** Радиус газового облака, см. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Gas")
	float GasRadius;

	/** Урон взрыва в эпицентре. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Gas")
	float ExplosionDamage;

	/** Травит ли газ прямо сейчас (для HUD «пахнет газом»). */
	UFUNCTION(BlueprintPure, Category="Repair|Gas")
	bool IsLeakingGas() const { return bBroken && bLeaksGasWhenBroken; }

	/** Сбить облако газа струёй огнетушителя: пока сбито, поджечь его нельзя. Только сервер. */
	void SuppressGas(float Duration);
	bool IsGasSuppressed() const { return GasSuppressedTime > 0.f; }

	// ---------- Мини-игры ----------

	/** Какой мини-игрой чинится: None (держать E) / Cursor (щиток) / Valve (труба) / Starter (генератор). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	ERepairMinigameType MinigameType;

	/** Сколько попаданий в зелёную зону нужно для починки. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	int32 HitsToRepair;

	/** Скорость курсора, проходов полоски в секунду. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	float MinigameCursorSpeed;

	/** Полуширина зелёной зоны (доля полоски). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	float MinigameGreenHalfWidth;

	/** Насколько паника ремонтника усложняет мини-игру (0 — не влияет). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	float PanicHardenScale;

	/** Урон током за промах (ремонтнику). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	float ShockDamage;

	/** Урон током по площади после серии промахов (всем рядом). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	float ShockAoEDamage;

	/** Сколько промахов до короткого замыкания и блокировки. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	int32 MissesBeforeLockout;

	/** На сколько секунд щиток блокируется после замыкания. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Minigame")
	float LockoutDuration;

	// ---------- Вентиль (труба) ----------

	/** Докрутка вентиля за один тык E (доля прогресса). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Valve")
	float ValveTurnAmount;

	/** Тыкать E реже этого интервала (сек), иначе «срыв резьбы». */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Valve")
	float ValveMinInterval;

	/** Сколько прогресса сгорает при срыве резьбы. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Valve")
	float ValveSlipPenalty;

	// ---------- Стартер (генератор) ----------

	/** За сколько секунд натяжение шнура растёт 0→100% при зажатом E. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Starter")
	float StarterChargeTime;

	/** Начало зелёного окна (доля натяжения): отпускать здесь. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Starter")
	float StarterWindowStart;

	/** Конец зелёного окна. Дотянул до 100% — обратный удар сам по себе. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Starter")
	float StarterWindowEnd;

	/** Сколько успешных рывков нужно для запуска. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Starter")
	int32 StarterPullsToFix;

	/** Урон от обратного удара шнура (рано отпустил / перетянул). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Starter")
	float StarterKickDamage;

	/** Паника от обратного удара. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Starter")
	float StarterKickPanic;

	/** Ниже этого натяжения отпускание — просто перехват, без наказания. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Starter")
	float StarterGraceTension;

	// ---------- «Не тот инструмент» — колхозный ремонт (§18) ----------

	/** Можно ли чинить объект «на коленке» без нужного инструмента (только если RequiredTool задан). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Botch")
	bool bAllowBotch;

	/** Во сколько раз дольше держать E при колхозе (мини-игра при этом не работает). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Botch")
	float BotchDurationMultiplier;

	/** Шанс косяка в секунду при колхозе. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Botch")
	float BotchMishapChancePerSecond;

	/** Сколько прогресса сгорает при косяке. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Botch")
	float BotchMishapProgressLoss;

	/** Урон ремонтнику при косяке. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Botch")
	float BotchMishapDamage;

	/** Паника при косяке. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Repair|Botch")
	float BotchMishapPanic;

	UFUNCTION(BlueprintPure, Category="Repair|Minigame") bool IsMinigameRepair() const { return MinigameType != ERepairMinigameType::None; }
	UFUNCTION(BlueprintPure, Category="Repair|Minigame") ERepairMinigameType GetMinigameType() const { return MinigameType; }
	UFUNCTION(BlueprintPure, Category="Repair|Minigame") float GetCursorPos() const { return CursorPos; }
	UFUNCTION(BlueprintPure, Category="Repair|Minigame") float GetGreenCenter() const { return GreenCenter; }
	UFUNCTION(BlueprintPure, Category="Repair|Minigame") int32 GetMissCount() const { return MissCount; }
	UFUNCTION(BlueprintPure, Category="Repair|Minigame") float GetLockoutRemaining() const { return LockoutRemaining; }
	UFUNCTION(BlueprintPure, Category="Repair|Valve") float GetValveCooldown() const { return ValveCooldown; }
	UFUNCTION(BlueprintPure, Category="Repair|Starter") bool IsStarterPulling() const { return bStarterPulling; }
	UFUNCTION(BlueprintPure, Category="Repair|Starter") float GetStarterTension() const { return StarterTension; }
	UFUNCTION(BlueprintPure, Category="Repair|Botch") bool IsBotching() const { return bBotching; }

	/** Можно ли колхозить: сломан, свободен, не ранен, нужен инструмент, а в руках не он. */
	bool CanBotchBy(const AAvaryoCharacter* Who) const;

	/** Нажатие E во время мини-игры: курсор — попадание, вентиль — докрутка, стартер — начать тянуть. Только сервер. */
	void TryHitBy(AAvaryoCharacter* Who);

	/** Отпускание E во время мини-игры: стартер оценивает рывок. Только сервер. */
	void TryReleaseBy(AAvaryoCharacter* Who);

	/** Объект починен (для RunState и списка задач). Срабатывает на сервере. */
	UPROPERTY(BlueprintAssignable, Category="Repair")
	FOnRepairFinished OnRepairFinished;

	// ---------- API (вызывать на сервере) ----------

	/** Начать починку. false — нельзя (не сломан, занят другим, нет инструмента...). */
	bool BeginRepairBy(AAvaryoCharacter* Who);

	/** Прекратить починку (отпустил E). Прогресс сохраняется. */
	void EndRepairBy(AAvaryoCharacter* Who);

	/** Можно ли игроку чинить: сломан, никем не занят, не ранен, инструмент подходит. */
	bool CanBeRepairedBy(const AAvaryoCharacter* Who) const;

	// ---------- Геттеры ----------

	UFUNCTION(BlueprintPure, Category="Repair") bool IsBroken() const { return bBroken; }
	UFUNCTION(BlueprintPure, Category="Repair") bool IsBeingRepaired() const { return Repairer != nullptr; }
	UFUNCTION(BlueprintPure, Category="Repair") float GetRepairProgress() const { return RepairProgress; }
	UFUNCTION(BlueprintPure, Category="Repair") AAvaryoCharacter* GetRepairer() const { return Repairer; }

	/** Сломать объект (рандомизация поломок на старте забега, аварии). Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Repair")
	void SetBroken(bool bNewBroken);

	/** Паника ремонтника 0..1 (трясущиеся руки усложняют мини-игру). */
	float RepairerPanic01() const;

protected:
	/** Сломан ли. Выставляется в редакторе/скриптом; чинится игроками. */
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Broken, BlueprintReadOnly, Category="Repair")
	bool bBroken;

	/** Прогресс починки 0..1, реплицируется для HUD и таблички. */
	UPROPERTY(Replicated)
	float RepairProgress;

	/** Кто сейчас держит E (один ремонтник на объект). */
	UPROPERTY(Replicated)
	TObjectPtr<AAvaryoCharacter> Repairer;

	/** Таймер шума починки (стук/сварка слышны — задел под монстра-слухача). */
	float NoiseAccum;

	/** Пауза между взрывами, чтобы не рвало каждый тик. */
	float ExplosionCooldown;

	/** Сколько ещё секунд газ сбит пеной (поджечь нельзя). */
	float GasSuppressedTime;

	/** Мини-игра: позиция курсора, центр зелёной зоны, промахи, блокировка. */
	UPROPERTY(Replicated)
	float CursorPos;

	UPROPERTY(Replicated)
	float GreenCenter;

	UPROPERTY(Replicated)
	int32 MissCount;

	UPROPERTY(Replicated)
	float LockoutRemaining;

	float CursorPhase;
	float MinigameSpeedMult;

	/** Вентиль: сколько осталось до «безопасно тыкать» (HUD рисует индикатор ритма). */
	UPROPERTY(Replicated)
	float ValveCooldown;

	/** Стартер: тянет ли шнур прямо сейчас (E зажат). */
	UPROPERTY(Replicated)
	bool bStarterPulling;

	/** Стартер: текущее натяжение шнура 0..1. */
	UPROPERTY(Replicated)
	float StarterTension;

	/** Текущая починка идёт колхозом (без инструмента). Реплицируется для HUD и маршрутизации E. */
	UPROPERTY(Replicated)
	bool bBotching;

	/** Докрутка вентиля одним тыком: попал в ритм или сорвал резьбу. Только сервер. */
	void HandleValveTurn(AAvaryoCharacter* Who);

	/** Обратный удар шнура стартера: урон, паника, шум, попытка не засчитана. Только сервер. */
	void StarterKickback(AAvaryoCharacter* Who);

	/** Успешное завершение: снять лок, погасить лампу, разослать событие. Только сервер. */
	void FinishRepair(AAvaryoCharacter* Who);

	/** Замыкание: ток по всем рядом, блокировка щитка. Только сервер. */
	void ShortCircuit(AAvaryoCharacter* Culprit);

	/** Газ рванул: урон и паника по радиусу, очень громко, прогресс починки сгорает. */
	void ExplodeGas(AAvaryoCharacter* Culprit);

	/** Тряска камеры от взрыва у всех машин (затухает с расстоянием). */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastExplosionShake();

	/** Последний показанный на табличке процент — чтобы не перерисовывать текст каждый кадр. */
	int32 LastShownPercent;

	UFUNCTION()
	void OnRep_Broken();

	/** Обновить табличку (текст + цвет) из текущего состояния. */
	void RefreshStatusVisual();

	/** Серверная проверка, что починку можно продолжать (дистанция, ранение, инструмент). */
	bool CanContinueRepair() const;
};
