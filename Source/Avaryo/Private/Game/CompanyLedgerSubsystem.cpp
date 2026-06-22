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
		QuotaWindowShifts  = S->QuotaWindowShifts;
		bQuotaFailed       = S->bQuotaFailed;
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
	S->QuotaWindowShifts  = QuotaWindowShifts;
	S->bQuotaFailed       = bQuotaFailed;
	UGameplayStatics::SaveGameToSlot(S, SlotName(), 0);
}

void UCompanyLedgerSubsystem::CommitShift(int32 ShiftNet, bool bWon)
{
	const int32 PlayedShift = ShiftNumber; // смена, которая только что закончилась

	CompanyBalance += ShiftNet;

	// Сдал объект — репутация чуть вверх; провалил — заметно вниз; ушёл в минус — ещё штраф
	ReputationPoints += bWon ? 1 : -2;
	if (ShiftNet < 0)
	{
		ReputationPoints -= 1;
	}
	ReputationPoints = FMath::Clamp(ReputationPoints, -10, 10);

	if (bWon) { ++Career.ShiftsWon; } else { ++Career.ShiftsLost; }

	// Квота диспетчера: копим сданное, на дедлайне — проверка (выполнил → новая выше; нет → крах)
	if (QuotaTarget > 0 && !bQuotaFailed)
	{
		QuotaPaidSoFar += FMath::Max(0, ShiftNet);
		if (PlayedShift >= QuotaDeadlineShift)
		{
			if (QuotaPaidSoFar >= QuotaTarget)
			{
				QuotaTarget = FMath::RoundToInt(QuotaTarget * 1.6f); // следующая квота жёстче
				QuotaDeadlineShift = (PlayedShift + 1) + QuotaWindowShifts - 1;
				QuotaPaidSoFar = 0;
			}
			else
			{
				bQuotaFailed = true; // контора закрыта — ARunState предложит новую карьеру
			}
		}
	}

	++ShiftNumber;

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
	QuotaWindowShifts = 0; // свежая карьера не наследует длину окна прошлой квоты (CODE_AUDIT3 #12)
	bQuotaFailed = false;
	Save();
}

void UCompanyLedgerSubsystem::StartQuota(int32 Target, int32 WindowShifts)
{
	QuotaTarget = FMath::Max(0, Target);
	QuotaWindowShifts = FMath::Max(1, WindowShifts);
	QuotaDeadlineShift = ShiftNumber + QuotaWindowShifts - 1;
	QuotaPaidSoFar = 0;
	bQuotaFailed = false;
	Save();
}

void UCompanyLedgerSubsystem::StopQuota()
{
	QuotaTarget = 0;
	bQuotaFailed = false;
	Save();
}

void UCompanyLedgerSubsystem::SetEquipmentLevel(FName Tool, int32 Level)
{
	const int32 MaxLvl = (Tool == TEXT("Flashlight")) ? 4 : 3;
	const int32 L = FMath::Clamp(Level, (Tool == TEXT("Cameras")) ? 0 : 1, MaxLvl);
	if      (Tool == TEXT("Flashlight"))   { Equipment.Flashlight   = L; }
	else if (Tool == TEXT("Tester"))       { Equipment.Tester       = L; }
	else if (Tool == TEXT("Welder"))       { Equipment.Welder       = L; }
	else if (Tool == TEXT("Extinguisher")) { Equipment.Extinguisher = L; }
	else if (Tool == TEXT("Radio"))        { Equipment.Radio        = L; }
	else { return; }
	Save();
}

int32 UCompanyLedgerSubsystem::GetEquipmentLevel(FName Tool) const
{
	if (Tool == TEXT("Flashlight"))   { return Equipment.Flashlight; }
	if (Tool == TEXT("Tester"))       { return Equipment.Tester; }
	if (Tool == TEXT("Welder"))       { return Equipment.Welder; }
	if (Tool == TEXT("Extinguisher")) { return Equipment.Extinguisher; }
	if (Tool == TEXT("Radio"))        { return Equipment.Radio; }
	return 1;
}

bool UCompanyLedgerSubsystem::BuyUpgrade(FName Tool)
{
	// Валидируем инструмент ДО списания: иначе неизвестное имя (напр. "Cameras" или опечатка из AvUpgrade)
	// проходило как Cur=1 → TrySpend списывал и сохранял, потом рефанд+сохранял — деньги дёргались зря (CODE_AUDIT3 #2/#13).
	const bool bKnownTool = Tool == TEXT("Flashlight") || Tool == TEXT("Tester")
		|| Tool == TEXT("Welder") || Tool == TEXT("Extinguisher") || Tool == TEXT("Radio");
	if (!bKnownTool)
	{
		return false;
	}
	const int32 Cur = GetEquipmentLevel(Tool);
	const int32 MaxLvl = (Tool == TEXT("Flashlight")) ? 4 : 3;
	if (Cur >= MaxLvl)
	{
		return false; // уже максимум
	}
	const int32 Price = 3000 * Cur; // дороже с каждым уровнем
	if (!TrySpend(Price))           // спишет и сохранит при успехе
	{
		return false; // не хватает денег
	}
	if      (Tool == TEXT("Flashlight"))   { Equipment.Flashlight   = Cur + 1; }
	else if (Tool == TEXT("Tester"))       { Equipment.Tester       = Cur + 1; }
	else if (Tool == TEXT("Welder"))       { Equipment.Welder       = Cur + 1; }
	else if (Tool == TEXT("Extinguisher")) { Equipment.Extinguisher = Cur + 1; }
	else if (Tool == TEXT("Radio"))        { Equipment.Radio        = Cur + 1; }
	else { AddBalance(Price); return false; } // неизвестный инструмент — вернуть деньги
	Save();
	return true;
}
