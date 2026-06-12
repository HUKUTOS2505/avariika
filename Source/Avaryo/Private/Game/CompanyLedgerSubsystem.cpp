#include "Game/CompanyLedgerSubsystem.h"

void UCompanyLedgerSubsystem::CommitShift(int32 ShiftNet, bool bWon)
{
	CompanyBalance += ShiftNet;

	// Сдал объект — репутация чуть вверх; провалил — заметно вниз; ушёл в минус — ещё штраф
	ReputationPoints += bWon ? 1 : -2;
	if (ShiftNet < 0)
	{
		ReputationPoints -= 1;
	}
	ReputationPoints = FMath::Clamp(ReputationPoints, -10, 10);

	++ShiftNumber;
}
