#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CompanyLedgerSubsystem.generated.h"

/**
 * Бухгалтерия конторы (§19): баланс, номер смены и репутация переживают
 * перезапуск уровня (ProcessServerTravel «?restart» не трогает GameInstance).
 * Живёт на сервере; клиентам нужные числа отдаёт ARunState через репликацию.
 */
UCLASS()
class AVARYO_API UCompanyLedgerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Текущий баланс конторы, ₽ (может быть отрицательным — долг). */
	int32 GetBalance() const { return CompanyBalance; }

	/** Номер текущей смены (с 1). */
	int32 GetShiftNumber() const { return ShiftNumber; }

	/** Очки репутации, зажаты в [-10; 10]. */
	int32 GetReputation() const { return ReputationPoints; }

	/** Зафиксировать итог смены: net к балансу, репутация вверх/вниз, +1 к номеру смены. */
	void CommitShift(int32 ShiftNet, bool bWon);

protected:
	int32 CompanyBalance = 0;
	int32 ShiftNumber = 1;
	int32 ReputationPoints = 0;
};
