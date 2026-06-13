#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Game/AvariikaSaveGame.h"
#include "CompanyLedgerSubsystem.generated.h"

/**
 * Бухгалтерия конторы (§19): баланс, номер смены и репутация переживают
 * перезапуск уровня (ProcessServerTravel «?restart» не трогает GameInstance)
 * И выход из игры — пишутся на диск в слот "AvariikaCompany" (UAvariikaSaveGame).
 * Живёт на сервере; клиентам нужные числа отдаёт ARunState через репликацию.
 * Фундамент магазина/прогрессии — см. SPEC_Shop_Progression.md.
 */
UCLASS()
class AVARYO_API UCompanyLedgerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Текущий баланс конторы, ₽ (может быть отрицательным — долг). */
	int32 GetBalance() const { return CompanyBalance; }

	/** Номер текущей смены (с 1). */
	int32 GetShiftNumber() const { return ShiftNumber; }

	/** Очки репутации, зажаты в [-10; 10]. */
	int32 GetReputation() const { return ReputationPoints; }

	/** Зафиксировать итог смены: net к балансу, репутация вверх/вниз, +1 к номеру смены, сейв. */
	void CommitShift(int32 ShiftNet, bool bWon);

	/** Списать ₽ на покупку (атомарно). false если не хватает. Сейвит при успехе. */
	bool TrySpend(int32 Cost);

	/** Начислить ₽ (например, бонус) + сейв. */
	void AddBalance(int32 Amount);

	/** Сбросить контору в ноль (новая карьера / провал квоты) + сейв. */
	void ResetCompany();

	/** Уровень апгрейда инструмента по имени категории (Flashlight/Tester/Welder/Extinguisher/Radio). 1 — базовый. */
	int32 GetEquipmentLevel(FName Tool) const;

	/** Купить +1 уровень инструмента: списывает цену (растёт с уровнем), поднимает уровень, сейв. false если не хватает/макс. */
	bool BuyUpgrade(FName Tool);

	// ---------- Квота диспетчера (game-over крючок). QuotaTarget==0 = выкл/песочница. ----------

	/** Запустить квоту: сдать Target ₽ за WindowShifts смен (с текущей). + сейв. */
	void StartQuota(int32 Target, int32 WindowShifts = 3);

	/** Выключить квоту (режим песочницы). + сейв. */
	void StopQuota();

	int32 GetQuotaTarget() const { return QuotaTarget; }
	int32 GetQuotaPaidSoFar() const { return QuotaPaidSoFar; }
	int32 GetQuotaDeadlineShift() const { return QuotaDeadlineShift; }
	bool  IsQuotaActive() const { return QuotaTarget > 0; }
	bool  IsQuotaFailed() const { return bQuotaFailed; }

	/** Сохранить состояние на диск немедленно. */
	void Save() const;

	// Прогрессия (магазин читает/пишет; после записи вызвать Save()).
	const FEquipmentLevels& GetEquipment() const { return Equipment; }
	FEquipmentLevels&       GetEquipmentMutable() { return Equipment; }
	const FConsumableStock& GetStock() const { return Stock; }
	FConsumableStock&       GetStockMutable() { return Stock; }
	const FCareerStats&     GetCareer() const { return Career; }
	FCareerStats&           GetCareerMutable() { return Career; }

protected:
	void Load();
	static const TCHAR* SlotName() { return TEXT("AvariikaCompany"); }

	int32 CompanyBalance = 0;
	int32 ShiftNumber = 1;
	int32 ReputationPoints = 0;

	FEquipmentLevels Equipment;
	FConsumableStock Stock;
	FCareerStats     Career;
	int32 QuotaTarget = 0;
	int32 QuotaDeadlineShift = 0;
	int32 QuotaPaidSoFar = 0;
	int32 QuotaWindowShifts = 3;
	bool  bQuotaFailed = false;
};
