#include "Game/CompanyLedgerSubsystem.h"

#include "Game/AvariikaSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UCompanyLedgerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Load();
}

void UCompanyLedgerSubsystem::Load()
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName(), 0))
	{
		return; // первый запуск — дефолты
	}
	if (UAvariikaSaveGame* S = Cast<UAvariikaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName(), 0)))
	{
		CompanyBalance     = S->CompanyBalance;
		ShiftNumber        = S->ShiftNumber;
		ReputationPoints   = S->ReputationPoints;
		Equipment          = S->Equipment;
		Stock              = S->Stock;
		Career             = S->Career;
		QuotaTarget        = S->QuotaTarget;
		QuotaDeadlineShift = S->QuotaDeadlineShift;
		QuotaPaidSoFar     = S->QuotaPaidSoFar;
	}
}

void UCompanyLedgerSubsystem::Save() const
{
	UAvariikaSaveGame* S = Cast<UAvariikaSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UAvariikaSaveGame::StaticClass()));
	if (!S)
	{
		return;
	}
	S->CompanyBalance     = CompanyBalance;
	S->ShiftNumber        = ShiftNumber;
	S->ReputationPoints   = ReputationPoints;
	S->Equipment          = Equipment;
	S->Stock              = Stock;
	S->Career             = Career;
	S->QuotaTarget        = QuotaTarget;
	S->QuotaDeadlineShift = QuotaDeadlineShift;
	S->QuotaPaidSoFar     = QuotaPaidSoFar;
	UGameplayStatics::SaveGameToSlot(S, SlotName(), 0);
}

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

	if (bWon) { ++Career.ShiftsWon; } else { ++Career.ShiftsLost; }

	Save();
}

bool UCompanyLedgerSubsystem::TrySpend(int32 Cost)
{
	if (Cost <= 0 || CompanyBalance < Cost)
	{
		return false;
	}
	CompanyBalance -= Cost;
	Save();
	return true;
}

void UCompanyLedgerSubsystem::AddBalance(int32 Amount)
{
	CompanyBalance += Amount;
	Save();
}

void UCompanyLedgerSubsystem::ResetCompany()
{
	CompanyBalance = 0;
	ShiftNumber = 1;
	ReputationPoints = 0;
	Equipment = FEquipmentLevels();
	Stock = FConsumableStock();
	Career = FCareerStats();
	QuotaTarget = 0;
	QuotaDeadlineShift = 0;
	QuotaPaidSoFar = 0;
	Save();
}
