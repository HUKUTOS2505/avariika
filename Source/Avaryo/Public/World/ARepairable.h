#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARepairable.generated.h"

class AAvaryoCharacter;
class UStaticMeshComponent;
class UTextRenderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRepairFinished, ARepairable*, Repairable);

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

	/** Последний показанный на табличке процент — чтобы не перерисовывать текст каждый кадр. */
	int32 LastShownPercent;

	UFUNCTION()
	void OnRep_Broken();

	/** Обновить табличку (текст + цвет) из текущего состояния. */
	void RefreshStatusVisual();

	/** Серверная проверка, что починку можно продолжать (дистанция, ранение, инструмент). */
	bool CanContinueRepair() const;
};
