#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Subsystems/WorldSubsystem.h"
#include "ARunState.generated.h"

class ARepairable;

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

	UFUNCTION()
	void OnObjectiveRepaired(ARepairable* Repairable);

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
