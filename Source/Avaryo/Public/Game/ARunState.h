#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Subsystems/WorldSubsystem.h"
#include "ARunState.generated.h"

class AAvaryoCharacter;
class ARepairable;

/** Статистика монтёра за забег — кормит «Акт выполненных работ». */
USTRUCT(BlueprintType)
struct FPlayerRunStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Run")
	TObjectPtr<AAvaryoCharacter> Character = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 Repairs = 0;          // закончил починок

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 TimesWounded = 0;     // сколько раз падал

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 Revives = 0;          // поднял тиммейтов аптечкой

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 Drags = 0;            // хватался тащить раненых

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 Incidents = 0;        // санитарные инциденты

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 ToiletVisits = 0;     // дисциплинированные визиты в биотуалет

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 BotchedRepairs = 0;   // починки «на коленке» без инструмента (§18)

	UPROPERTY(BlueprintReadOnly, Category="Run")
	float PanicSeconds = 0.f;   // времени в панике

	UPROPERTY(BlueprintReadOnly, Category="Run")
	float SmellSeconds = 0.f;   // времени «воняет» (§16)

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 ShovedOthers = 0;     // сколько раз толкнул товарищей (§18)

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 TimesTripped = 0;     // сколько раз споткнулся (§18)

	UPROPERTY(BlueprintReadOnly, Category="Run")
	float SlipSeconds = 0.f;    // времени катался по пене (§18)

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 ExplosionsCaused = 0; // устроил взрыв газа на объекте

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 ShortsCaused = 0;     // замкнул щиток серией промахов

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 Throws = 0;           // метал предметы (§18)

	UPROPERTY(BlueprintReadOnly, Category="Run")
	int32 Coffees = 0;          // хлебал кофе из термоса

	// Для детекции переходов на серверном тике (реплицируются заодно — безвредно)
	UPROPERTY()
	bool bWasWounded = false;

	UPROPERTY()
	bool bWasSoiled = false;

	UPROPERTY()
	bool bWasSmelly = false;
};

/** Реплика в эфире рации, как её хранит клиент: кто + текст + момент получения (HUD гасит по возрасту). */
struct FDispatcherLine
{
	FString Speaker; // «ДИСПЕТЧЕР» или «Серёга (паника)»
	FString Text;
	float ReceivedAt = 0.f;
};

/** Фаза забега. */
UENUM(BlueprintType)
enum class ERunPhase : uint8
{
	InProgress	UMETA(DisplayName = "Работаем"),
	Won			UMETA(DisplayName = "Победа"),
	Lost		UMETA(DisplayName = "Поражение")
};

/**
 * Состояние забега: список задач (сломанные объекты), счёт починок, таймер, фаза.
 * Спавнится сервером через URunStateSubsystem — BP GameMode править не нужно.
 * Победа: всё починено и вся команда в зоне выхода (Газель). Если зоны выхода
 * на карте нет — победа сразу после последней починки.
 * Поражение: вся команда ранена (некому поднимать).
 */
UCLASS()
class AVARYO_API ARunState : public AInfo
{
	GENERATED_BODY()

public:
	ARunState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Найти RunState в мире (для HUD и зоны выхода). */
	static ARunState* Get(UWorld* World);

	UFUNCTION(BlueprintPure, Category="Run") ERunPhase GetPhase() const { return Phase; }
	UFUNCTION(BlueprintPure, Category="Run") int32 GetTotalObjectives() const { return Objectives.Num(); }
	UFUNCTION(BlueprintPure, Category="Run") int32 GetRepairedCount() const { return RepairedCount; }
	UFUNCTION(BlueprintPure, Category="Run") bool AreAllObjectivesComplete() const { return RepairedCount >= Objectives.Num(); }

	/** Задачи забега (для списка на HUD). */
	UFUNCTION(BlueprintPure, Category="Run")
	const TArray<ARepairable*>& GetObjectives() const { return Objectives; }

	/** Сколько секунд идёт забег (на финише — итоговое время). */
	UFUNCTION(BlueprintPure, Category="Run")
	float GetElapsedSeconds() const;

	/** Зона выхода сообщает: вся команда у Газели. Только сервер. */
	void NotifyTeamAtExit();

	/** Перезапуск смены с экрана «Акта» (клавиша R). Только сервер. */
	void RequestRestart();

	// ---------- Статистика (сервер пишет, клиенты читают для «Акта») ----------

	UFUNCTION(BlueprintPure, Category="Run")
	const TArray<FPlayerRunStats>& GetPlayerStats() const { return PlayerStats; }

	/** Поднял раненого тиммейта аптечкой. Только сервер. */
	void AddRevive(AAvaryoCharacter* Who);

	/** Схватился тащить раненого. Только сервер. */
	void AddDrag(AAvaryoCharacter* Who);

	/** Толкнул товарища (§18). Только сервер. */
	void AddShove(AAvaryoCharacter* Who);

	/** Споткнулся на бегу (§18). Только сервер. */
	void AddTrip(AAvaryoCharacter* Who);

	/** Дошёл до биотуалета. Только сервер. */
	void AddToiletVisit(AAvaryoCharacter* Who);

	/** Закончил колхозный ремонт без инструмента (§18). Только сервер. */
	void AddBotchedRepair(AAvaryoCharacter* Who);

	// ---------- Диспетчер (комментирует хаос по рации) ----------

	/** Последние реплики диспетчера на этой машине (HUD рисует и гасит сам). */
	const TArray<FDispatcherLine>& GetDispatcherLines() const { return DispatcherLines; }

	/** Газ рванул на объекте. Только сервер. */
	void NotifyGasExplosion(AAvaryoCharacter* Culprit);

	/** Щиток замкнуло после серии промахов. Только сервер. */
	void NotifyShortCircuit(AAvaryoCharacter* Culprit);

	/** Сработала растяжка-ловушка (§18). Только сервер. */
	void NotifyTrapTriggered(AAvaryoCharacter* TriggeredBy);

	/** Шлёпнул биологический снаряд (§15). TriggeredBy — прямая жертва или null. Только сервер. */
	void NotifyBioSplat(AAvaryoCharacter* DirectHit);

	/** Поскользнулся на пене (§18). Who — поскользнувшийся. Только сервер. */
	void NotifySlipped(AAvaryoCharacter* Who);

	/** Толкнули товарища (§18). Victim — кого толкнули. Только сервер. */
	void NotifyShoved(AAvaryoCharacter* Victim);

	/** Споткнулся на бегу (§18). Who — споткнувшийся. Только сервер. */
	void NotifyTripped(AAvaryoCharacter* Who);

	/** Метнул предмет (§18). Только сервер. */
	void NotifyThrow(AAvaryoCharacter* Who);

	/** Хлебнул кофе из термоса. Только сервер. */
	void NotifyCoffee(AAvaryoCharacter* Who);

	/** Достался ли бригаде дешёвый комплект оборудования (косяки, §18). */
	UFUNCTION(BlueprintPure, Category="Run")
	bool HasCheapGear() const { return bCheapGear; }

	/** Дев-режим: принудительно включить дешёвый комплект и раздать фонарям. Только сервер. */
	void DebugForceCheapGear();

	/** Дев-режим: завершить забег (показать «Акт») победой/поражением. Только сервер. */
	void DebugFinishRun(bool bWon);

	/** Дев-режим: включить квоту на Target ₽ (0 = выключить). Только сервер. */
	void DebugSetQuota(int32 Target);

	// ---------- Бухгалтерия конторы (§19, переживает смены через UCompanyLedgerSubsystem) ----------

	/** Премии/штрафы одного монтёра за смену — единая формула для сервера и HUD. */
	static int32 ComputePlayerBalance(const FPlayerRunStats& S);

	UFUNCTION(BlueprintPure, Category="Run") int32 GetShiftNumber() const { return ShiftNumber; }
	UFUNCTION(BlueprintPure, Category="Run") int32 GetCompanyBalanceStart() const { return CompanyBalanceStart; }
	UFUNCTION(BlueprintPure, Category="Run") int32 GetShiftNet() const { return ShiftNet; }
	UFUNCTION(BlueprintPure, Category="Run") int32 GetReputation() const { return Reputation; }

	// Квота диспетчера (реплицируется для «Акта»; 0 = выключена/песочница)
	UFUNCTION(BlueprintPure, Category="Run") int32 GetQuotaTarget() const { return QuotaTarget; }
	UFUNCTION(BlueprintPure, Category="Run") int32 GetQuotaPaid() const { return QuotaPaid; }
	UFUNCTION(BlueprintPure, Category="Run") int32 GetQuotaDeadlineShift() const { return QuotaDeadlineShift; }
	UFUNCTION(BlueprintPure, Category="Run") bool IsQuotaActive() const { return QuotaTarget > 0; }
	UFUNCTION(BlueprintPure, Category="Run") bool IsQuotaFailed() const { return bQuotaFailed; }

	/** Название репутационного статуса конторы по очкам. */
	static FString ReputationTitle(int32 Points);

protected:
	UPROPERTY(Replicated)
	ERunPhase Phase;

	/** Сломанные на этот забег объекты (порядок = порядок в списке задач HUD). */
	UPROPERTY(Replicated)
	TArray<TObjectPtr<ARepairable>> Objectives;

	UPROPERTY(Replicated)
	int32 RepairedCount;

	/** Серверное время старта/конца (через GetServerWorldTimeSeconds — честно у клиентов). */
	UPROPERTY(Replicated)
	float RunStartServerTime;

	UPROPERTY(Replicated)
	float RunEndServerTime;

	/** Есть ли на карте зона выхода (если нет — побеждаем сразу после починок). */
	bool bHasExitZone;

	/** Перегрузка сети: старое здание изредка снова выбивает починенный щиток. */
	UPROPERTY(EditAnywhere, Category="Run|Overload")
	bool bElectricalOverload;

	/** Шанс/сек выбить щиток заново (только пока он починен и его никто не чинит). */
	UPROPERTY(EditAnywhere, Category="Run|Overload")
	float OverloadChancePerSecond;

	float OverloadCooldown; // пауза после выбивания, чтобы не зациклить

	/** Дешёвый комплект на этот забег: фонари моргают, рация ловит чужой голос чаще. Реплицируется для HUD. */
	UPROPERTY(Replicated)
	bool bCheapGear;

	/** Номер смены и баланс конторы на её начало (прочитаны из леджера в BeginPlay). Реплицируются для «Акта». */
	UPROPERTY(Replicated)
	int32 ShiftNumber;

	UPROPERTY(Replicated)
	int32 CompanyBalanceStart;

	/** Очки репутации конторы на начало смены. */
	UPROPERTY(Replicated)
	int32 Reputation;

	/** Итог смены (сумма по бригаде), считается на финише. Реплицируется для «Акта». */
	UPROPERTY(Replicated)
	int32 ShiftNet;

	/** Квота диспетчера — копии из леджера для «Акта» (0 = выключена). */
	UPROPERTY(Replicated)
	int32 QuotaTarget = 0;

	UPROPERTY(Replicated)
	int32 QuotaPaid = 0;

	UPROPERTY(Replicated)
	int32 QuotaDeadlineShift = 0;

	UPROPERTY(Replicated)
	bool bQuotaFailed = false;

	/** Сервер: когда в эфир в следующий раз прорвётся чужой голос (только пока включена рация). */
	float NextRadioGhostTime;

	/** Сервер: фонарям каких монтёров уже выдали дешёвый статус (раздаём один раз). */
	TSet<TWeakObjectPtr<AAvaryoCharacter>> CheapGearApplied;

	/** Статистика всех монтёров (по мере появления персонажей). */
	UPROPERTY(Replicated)
	TArray<FPlayerRunStats> PlayerStats;

	/** Найти/завести запись статистики. Только сервер. */
	FPlayerRunStats& FindOrAddStats(AAvaryoCharacter* Who);

	/** Реплики диспетчера на этой машине (заполняется мультикастом, не реплицируется). */
	TArray<FDispatcherLine> DispatcherLines;

	/** Сервер: раньше этого времени неважные реплики глотаются (диспетчер не тараторит). */
	float NextChatterTime;

	/** Приветствие с задержкой: даём клиентам получить RunState, потом здороваемся. */
	FTimerHandle GreetingTimer;

	/** Сервер: когда кому из паникующих крикнуть в эфир в следующий раз. */
	TMap<TWeakObjectPtr<AAvaryoCharacter>, float> NextPanicCryTime;

	/** Сервер: случайная реплика из пула, «{X}» заменяется на Param, рассылка всем. */
	void DispatcherSay(const TArray<FString>& Pool, const FString& Param = FString(), bool bImportant = false,
		const FString& Speaker = FString());

	/** Сервер: паникующий монтёр сам кричит в рацию (и шумит — монстр оценит). */
	void TickPanicCries(AAvaryoCharacter* Who, float Now);

	/** Сервер: пока хоть одна рация включена, в эфир изредка прорывается чужой голос (паника + шум). */
	void TickRadioInterference(float Now);

	/** Сервер: перегрузка сети — старое здание может снова выбить починенный щиток (§18). */
	void TickOverload(float DeltaSeconds);

	/** Сервер: фоновая жуть — редкий скрип/шум здания + реплика «показалось?». */
	void TickAmbient(float Now);

	float NextCreakTime; // когда следующий скрип

	UPROPERTY(EditAnywhere, Category="Run|Ambient")
	float CreakIntervalMin;

	UPROPERTY(EditAnywhere, Category="Run|Ambient")
	float CreakIntervalMax;

	/** Страх темноты: пока питание вырублено (щиток сломан) и игрок без света — паника растёт. */
	UPROPERTY(EditAnywhere, Category="Run|Darkness")
	bool bDarknessFear;

	/** Сколько паники/сек набегает в темноте без света. */
	UPROPERTY(EditAnywhere, Category="Run|Darkness")
	float DarknessPanicPerSecond;

	/** Радиус «у прожектора не страшно», см (хранится в квадрате — см. .cpp). */
	UPROPERTY(EditAnywhere, Category="Run|Darkness")
	float DarknessSafeRadius;

	/** Сервер: выдать монтёру дешёвый фонарь, если на забег выпал дешёвый комплект (один раз). */
	void ApplyCheapGear(AAvaryoCharacter* Who);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDispatcherSay(const FString& Speaker, const FString& Line);

	/** Приветственная реплика по таймеру после старта смены. */
	void SendGreeting();

	/** Имя монтёра для реплик («Монтёр», пока PlayerState не приехал). */
	static FString CrewName(const AAvaryoCharacter* Who);

	UFUNCTION()
	void OnObjectiveRepaired(ARepairable* Repairable, AAvaryoCharacter* FinishedBy);

	/** Завершить забег. Только сервер. */
	void FinishRun(ERunPhase NewPhase);
};

/**
 * Спавнит ARunState на сервере при старте мира, чтобы не трогать Blueprint GameMode
 * (работаем с закрытым редактором).
 */
UCLASS()
class AVARYO_API URunStateSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
};
