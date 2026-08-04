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

	/** Дев: задать уровень инструмента напрямую (без денег) — для сравнения. Клампится по максимуму. + сейв. */
	void SetEquipmentLevel(FName Tool, int32 Level);

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
	bool HasSavedWorkerAppearance() const { return bHasSavedWorkerAppearance; }
	const FWorkerAppearance& GetSavedWorkerAppearance() const { return SavedWorkerAppearance; }
	void SetSavedWorkerAppearance(const FWorkerAppearance& NewAppearance);
	bool GetSuppressRaisedHoodHeadgearConflictWarning() const
	{
		return bSuppressRaisedHoodHeadgearConflictWarning;
	}
	/** Saves only when the global warning preference actually changes. */
	void SetSuppressRaisedHoodHeadgearConflictWarning(bool bSuppress);
	/** Universal API name; the legacy serialized field is retained for backward compatibility. */
	bool GetSuppressRaisedHoodEquipmentConflictWarning() const
	{
		return GetSuppressRaisedHoodHeadgearConflictWarning();
	}
	void SetSuppressRaisedHoodEquipmentConflictWarning(bool bSuppress)
	{
		SetSuppressRaisedHoodHeadgearConflictWarning(bSuppress);
	}
	bool GetSuppressHeadgearHeadphonesConflictWarning() const
	{
		return bSuppressHeadgearHeadphonesConflictWarning;
	}
	/** Saves only when the independent Headgear/Headphones preference changes. */
	void SetSuppressHeadgearHeadphonesConflictWarning(bool bSuppress);

	bool HasActiveCharacter() const;
	const FAvCharacterRecord* GetActiveCharacter() const;
	const TArray<FAvCharacterRecord>& GetCharacterRecords() const { return CharacterRecords; }
	FName GetActiveCharacterId() const { return ActiveCharacterId; }
	const FWorkerAppearance& GetActiveCharacterAppearance() const;
	/** Factory-create, activate and atomically save one unique roster record. */
	FName CreateCharacter();
	/** Phase 1 rule: selection immediately becomes active. No-op for the current ID. */
	bool SetActiveCharacter(FName CharacterId);
	/** Deletes one record and selects next SortOrder (or previous). The last record is protected. */
	bool DeleteCharacter(FName CharacterId);
	void SetActiveCharacterAppearance(
		const FWorkerAppearance& NewAppearance,
		bool bHasMeaningfulAppearance,
		FName BasePresetId = NAME_None,
		EAvAppearanceOrigin AppearanceOrigin = EAvAppearanceOrigin::ManualCustomized);
	void SetActiveCharacterDisplayName(const FString& NewDisplayName);
	/** Renames exactly one existing roster record and saves once when the name changes. */
	bool SetCharacterDisplayName(FName CharacterId, const FString& NewDisplayName);
	int32 GetCharacterSchemaVersion() const { return CharacterSchemaVersion; }
	bool WasLoadedActiveCharacterIdInvalid() const
	{
		return bLoadedActiveCharacterIdWasInvalid;
	}
	int32 GetDuplicateCharacterIdCount() const;
	bool IsLegacyAppearanceMirrorSynchronized() const;

protected:
	void Load();
	void LoadUserSettings();
	void SaveUserSettings() const;
	void CreateDefaultCharacterRecord();
	void MigrateLegacyCharacterAppearance();
	bool MigrateObsoleteFactoryBaseAppearance();
	bool MigrateAppearanceOrigins(int32 LoadedSchemaVersion);
	bool MigrateHeadTypeSkinPresentation(int32 LoadedSchemaVersion);
	bool MigrateRosterMetadata(int32 LoadedSchemaVersion);
	FAvCharacterRecord* GetActiveCharacterMutable();
	void SynchronizeLegacyAppearanceCache();
	FString MakeNextCharacterDisplayName() const;
	FName MakeUniqueCharacterId() const;
	static const TCHAR* SlotName() { return TEXT("AvariikaCompany"); }
	static const TCHAR* UserSettingsSlotName() { return TEXT("AvariikaUserSettings"); }
	static constexpr int32 CurrentCharacterSchemaVersion = 5;

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
	bool bHasSavedWorkerAppearance = false;
	FWorkerAppearance SavedWorkerAppearance;
	bool bSuppressRaisedHoodHeadgearConflictWarning = false;
	bool bSuppressHeadgearHeadphonesConflictWarning = false;
	int32 CharacterSchemaVersion = 0;
	FName ActiveCharacterId;
	TArray<FAvCharacterRecord> CharacterRecords;
	bool bLoadedActiveCharacterIdWasInvalid = false;
};
