#include "Game/CompanyLedgerSubsystem.h"

#include "AvariikaLoc.h"
#include "Game/AvariikaSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UCompanyLedgerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadUserSettings();
	Load();
}

void UCompanyLedgerSubsystem::LoadUserSettings()
{
	bSuppressRaisedHoodHeadgearConflictWarning = false;
	bSuppressHeadgearHeadphonesConflictWarning = false;
	if (!UGameplayStatics::DoesSaveGameExist(UserSettingsSlotName(), 0))
	{
		return;
	}
	if (const UAvariikaUserSettingsSaveGame* Settings =
		Cast<UAvariikaUserSettingsSaveGame>(
			UGameplayStatics::LoadGameFromSlot(UserSettingsSlotName(), 0)))
	{
		bSuppressRaisedHoodHeadgearConflictWarning =
			Settings->bSuppressRaisedHoodHeadgearConflictWarning;
		bSuppressHeadgearHeadphonesConflictWarning =
			Settings->bSuppressHeadgearHeadphonesConflictWarning;
	}
}

void UCompanyLedgerSubsystem::SaveUserSettings() const
{
	// Preserve project-owned Main Menu settings added to this same canonical slot.
	// Creating a fresh object here would silently erase display/audio/language values.
	UAvariikaUserSettingsSaveGame* Settings = Cast<UAvariikaUserSettingsSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UserSettingsSlotName(), 0));
	if (!Settings)
	{
		Settings = Cast<UAvariikaUserSettingsSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UAvariikaUserSettingsSaveGame::StaticClass()));
	}
	if (!Settings)
	{
		return;
	}
	Settings->bSuppressRaisedHoodHeadgearConflictWarning =
		bSuppressRaisedHoodHeadgearConflictWarning;
	Settings->bSuppressHeadgearHeadphonesConflictWarning =
		bSuppressHeadgearHeadphonesConflictWarning;
	if (!UGameplayStatics::SaveGameToSlot(Settings, UserSettingsSlotName(), 0))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvEquipmentConflictPreference] Failed to save slot %s"),
			UserSettingsSlotName());
	}
}

void UCompanyLedgerSubsystem::Load()
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName(), 0))
	{
		CreateDefaultCharacterRecord();
		UE_LOG(LogTemp, Log,
			TEXT("[AvCompanySave] No company save exists; prepared in-memory defaults without creating a disk slot."));
		return;
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
		bHasSavedWorkerAppearance = S->bHasSavedWorkerAppearance;
		SavedWorkerAppearance = S->SavedWorkerAppearance;
		CharacterSchemaVersion = S->SchemaVersion;
		ActiveCharacterId = S->ActiveCharacterId;
		CharacterRecords = S->CharacterRecords;
		bLoadedActiveCharacterIdWasInvalid = !CharacterRecords.IsEmpty() &&
			(ActiveCharacterId.IsNone() || !CharacterRecords.ContainsByPredicate(
				[this](const FAvCharacterRecord& Record)
				{
					return Record.CharacterId == ActiveCharacterId;
				}));

		const bool bNeedsMigration = CharacterRecords.IsEmpty();
		bool bMigratedObsoleteFactoryBase = false;
		bool bMigratedAppearanceOrigins = false;
		bool bMigratedRosterMetadata = false;
		if (bNeedsMigration)
		{
			MigrateLegacyCharacterAppearance();
		}
		else
		{
			bMigratedRosterMetadata = MigrateRosterMetadata(S->SchemaVersion);
			bMigratedAppearanceOrigins = MigrateAppearanceOrigins(S->SchemaVersion);
			if (const FAvCharacterRecord* ResolvedActiveCharacter = GetActiveCharacter())
			{
				ActiveCharacterId = ResolvedActiveCharacter->CharacterId;
			}
			else
			{
				ActiveCharacterId = CharacterRecords[0].CharacterId;
			}
			for (FAvCharacterRecord& Record : CharacterRecords)
			{
				const bool bIsActive = Record.CharacterId == ActiveCharacterId;
				Record.bIsActive = bIsActive;
				Record.bIsSelected = bIsActive;
			}
			CharacterSchemaVersion = CurrentCharacterSchemaVersion;
			SynchronizeLegacyAppearanceCache();
			bMigratedObsoleteFactoryBase = MigrateObsoleteFactoryBaseAppearance();
		}
		const bool bMigratedHeadTypeSkinPresentation =
			MigrateHeadTypeSkinPresentation(S->SchemaVersion);
		CharacterSchemaVersion = CurrentCharacterSchemaVersion;
		SynchronizeLegacyAppearanceCache();

		if (bNeedsMigration || bMigratedObsoleteFactoryBase || bMigratedAppearanceOrigins ||
			bMigratedHeadTypeSkinPresentation || bMigratedRosterMetadata ||
			S->SchemaVersion < CurrentCharacterSchemaVersion)
		{
			UE_LOG(LogTemp, Log,
				TEXT("[AvCharacterSave] In-memory migration prepared without autosave; it will persist on the next explicit user change."));
		}
		return;
	}

	CreateDefaultCharacterRecord();
	UE_LOG(LogTemp, Error,
		TEXT("[AvCompanySave] Existing company slot could not be loaded; preserving it and using in-memory defaults."));
}

void UCompanyLedgerSubsystem::Save() const
{
	UAvariikaSaveGame* S = Cast<UAvariikaSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UAvariikaSaveGame::StaticClass()));
	if (!S)
	{
		return;
	}
	S->SchemaVersion = CurrentCharacterSchemaVersion;
	S->ActiveCharacterId = ActiveCharacterId;
	S->CharacterRecords = CharacterRecords;
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
	if (const FAvCharacterRecord* ActiveCharacter = GetActiveCharacter())
	{
		S->bHasSavedWorkerAppearance = true;
		S->SavedWorkerAppearance = ActiveCharacter->Appearance;
	}
	else
	{
		S->bHasSavedWorkerAppearance = bHasSavedWorkerAppearance;
		S->SavedWorkerAppearance = SavedWorkerAppearance;
	}
	if (!UGameplayStatics::SaveGameToSlot(S, SlotName(), 0))
	{
		UE_LOG(LogTemp, Error, TEXT("[AvCharacterSave] Failed to save slot %s"), SlotName());
	}
}

namespace
{
	FString AvMakeLocalizedDefaultCharacterName(int32 Number)
	{
		FNumberFormattingOptions NumberOptions;
		NumberOptions.SetMinimumIntegralDigits(2);
		NumberOptions.SetMaximumIntegralDigits(2);
		return FText::Format(
			FAvLoc::Text(TEXT("Customization.Character.DefaultNameFormat")),
			FText::AsNumber(Number, &NumberOptions)).ToString();
	}
}

void UCompanyLedgerSubsystem::CreateDefaultCharacterRecord()
{
	CharacterSchemaVersion = CurrentCharacterSchemaVersion;
	ActiveCharacterId = FName(TEXT("Character_01"));
	CharacterRecords.Reset();

	FAvCharacterRecord& Record = CharacterRecords.AddDefaulted_GetRef();
	Record.CharacterId = ActiveCharacterId;
	// Resolve once in the creation culture, then persist the resulting literal FString forever.
	Record.DisplayName = AvMakeLocalizedDefaultCharacterName(1);
	Record.Appearance = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
	Record.BasePresetId = FName(TEXT("BaseMaleUnderwear"));
	Record.bHasMeaningfulAppearance = false;
	Record.AppearanceOrigin = EAvAppearanceOrigin::Factory;
	Record.bIsActive = true;
	Record.bIsSelected = true;
	Record.SortOrder = 0;
	Record.CreatedTimestamp = 0;
	SynchronizeLegacyAppearanceCache();
}

void UCompanyLedgerSubsystem::MigrateLegacyCharacterAppearance()
{
	const bool bHasUsableLegacyAppearance = bHasSavedWorkerAppearance && !SavedWorkerAppearance.Slots.IsEmpty();
	const FWorkerAppearance LegacyAppearance = SavedWorkerAppearance;
	CreateDefaultCharacterRecord();
	if (bHasUsableLegacyAppearance)
	{
		FAvCharacterRecord& Record = CharacterRecords[0];
		Record.Appearance = LegacyAppearance;
		Record.BasePresetId = NAME_None;
		Record.bHasMeaningfulAppearance = true;
		Record.AppearanceOrigin = EAvAppearanceOrigin::ManualCustomized;
		SynchronizeLegacyAppearanceCache();
		UE_LOG(LogTemp, Log, TEXT("[AvCharacterSave] Migrated legacy worker appearance into Character_01."));
	}
}

bool UCompanyLedgerSubsystem::MigrateObsoleteFactoryBaseAppearance()
{
	FAvCharacterRecord* Record = GetActiveCharacterMutable();
	if (!Record || Record->bHasMeaningfulAppearance || Record->BasePresetId != FName(TEXT("BaseMaleUnderwear")) ||
		!UWorkerAppearanceComponent::IsObsoleteBaseMaleUnderwearAppearance(Record->Appearance))
	{
		return false;
	}

	Record->Appearance = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
	Record->AppearanceOrigin = EAvAppearanceOrigin::Factory;
	SynchronizeLegacyAppearanceCache();
	UE_LOG(LogTemp, Log, TEXT("[AvCharacterSave] Migrated exact obsolete modular factory base to the complete FaceRig body."));
	return true;
}

bool UCompanyLedgerSubsystem::MigrateAppearanceOrigins(int32 LoadedSchemaVersion)
{
	if (LoadedSchemaVersion >= 3)
	{
		return false;
	}

	for (FAvCharacterRecord& Record : CharacterRecords)
	{
		if (!Record.bHasMeaningfulAppearance)
		{
			Record.AppearanceOrigin = EAvAppearanceOrigin::Factory;
		}
		else if (Record.BasePresetId == FName(TEXT("Randomized")))
		{
			// This ID was written only by the old Random path, so the inference is unambiguous.
			Record.AppearanceOrigin = EAvAppearanceOrigin::RandomGenerated;
		}
		else if (Record.BasePresetId.ToString().StartsWith(TEXT("WorkerPreset_")))
		{
			Record.AppearanceOrigin = EAvAppearanceOrigin::PresetApplied;
		}
		else
		{
			// Preserve and protect an ambiguous legacy custom look.
			Record.AppearanceOrigin = EAvAppearanceOrigin::ManualCustomized;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[AvCharacterSave] Migrated appearance origins to schema v3."));
	return true;
}

bool UCompanyLedgerSubsystem::MigrateHeadTypeSkinPresentation(int32 LoadedSchemaVersion)
{
	bool bChanged = false;
	for (FAvCharacterRecord& Record : CharacterRecords)
	{
		FString Details;
		if (UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(
			Record.Appearance,
			&Details))
		{
			bChanged = true;
			UE_LOG(LogTemp, Log,
				TEXT("[AvCharacterSave] Head skin normalized Character=%s %s"),
				*Record.CharacterId.ToString(),
				*Details);
		}
	}
	if (bHasSavedWorkerAppearance &&
		UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(SavedWorkerAppearance))
	{
		bChanged = true;
	}

	if (bChanged)
	{
		SynchronizeLegacyAppearanceCache();
	}
	if (LoadedSchemaVersion < 4 || bChanged)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[AvCharacterSave] HeadType/SkinColor presentation migration v%d -> v4 Changed=%s Profiles=HeadType01/Light,HeadType02/Dark."),
			LoadedSchemaVersion,
			bChanged ? TEXT("true") : TEXT("false"));
	}
	return bChanged;
}

bool UCompanyLedgerSubsystem::MigrateRosterMetadata(int32 LoadedSchemaVersion)
{
	bool bChanged = false;
	for (int32 Index = 0; Index < CharacterRecords.Num(); ++Index)
	{
		FAvCharacterRecord& Record = CharacterRecords[Index];
		if (LoadedSchemaVersion < 5)
		{
			Record.SortOrder = Index;
			bChanged = true;
		}
	}
	return bChanged;
}

const FAvCharacterRecord* UCompanyLedgerSubsystem::GetActiveCharacter() const
{
	if (!ActiveCharacterId.IsNone())
	{
		for (const FAvCharacterRecord& Record : CharacterRecords)
		{
			if (Record.CharacterId == ActiveCharacterId)
			{
				return &Record;
			}
		}
	}
	for (const FAvCharacterRecord& Record : CharacterRecords)
	{
		if (Record.bIsActive)
		{
			return &Record;
		}
	}
	return CharacterRecords.IsEmpty() ? nullptr : &CharacterRecords[0];
}

FAvCharacterRecord* UCompanyLedgerSubsystem::GetActiveCharacterMutable()
{
	if (!ActiveCharacterId.IsNone())
	{
		for (FAvCharacterRecord& Record : CharacterRecords)
		{
			if (Record.CharacterId == ActiveCharacterId)
			{
				return &Record;
			}
		}
	}
	for (FAvCharacterRecord& Record : CharacterRecords)
	{
		if (Record.bIsActive)
		{
			return &Record;
		}
	}
	return CharacterRecords.IsEmpty() ? nullptr : &CharacterRecords[0];
}

bool UCompanyLedgerSubsystem::HasActiveCharacter() const
{
	return GetActiveCharacter() != nullptr;
}

const FWorkerAppearance& UCompanyLedgerSubsystem::GetActiveCharacterAppearance() const
{
	if (const FAvCharacterRecord* Record = GetActiveCharacter())
	{
		return Record->Appearance;
	}
	return SavedWorkerAppearance;
}

FString UCompanyLedgerSubsystem::MakeNextCharacterDisplayName() const
{
	for (int32 Number = 1; Number < MAX_int32; ++Number)
	{
		const FString NumberSuffix = FString::Printf(TEXT(" %02d"), Number);
		const FString Candidate = AvMakeLocalizedDefaultCharacterName(Number);
		if (!CharacterRecords.ContainsByPredicate(
			[&Candidate, &NumberSuffix](const FAvCharacterRecord& Record)
			{
				return Record.DisplayName.Equals(Candidate, ESearchCase::IgnoreCase) ||
					Record.DisplayName.EndsWith(NumberSuffix, ESearchCase::CaseSensitive);
			}))
		{
			return Candidate;
		}
	}
	return AvMakeLocalizedDefaultCharacterName(CharacterRecords.Num() + 1);
}

FName UCompanyLedgerSubsystem::MakeUniqueCharacterId() const
{
	FName Candidate;
	do
	{
		Candidate = FName(*FString::Printf(
			TEXT("Character_%s"),
			*FGuid::NewGuid().ToString(EGuidFormats::Digits)));
	}
	while (CharacterRecords.ContainsByPredicate(
		[Candidate](const FAvCharacterRecord& Record)
		{
			return Record.CharacterId == Candidate;
		}));
	return Candidate;
}

FName UCompanyLedgerSubsystem::CreateCharacter()
{
	const FName NewCharacterId = MakeUniqueCharacterId();
	int32 NextSortOrder = 0;
	for (FAvCharacterRecord& Record : CharacterRecords)
	{
		Record.bIsActive = false;
		Record.bIsSelected = false;
		NextSortOrder = FMath::Max(NextSortOrder, Record.SortOrder + 1);
	}

	FAvCharacterRecord& Record = CharacterRecords.AddDefaulted_GetRef();
	Record.CharacterId = NewCharacterId;
	Record.DisplayName = MakeNextCharacterDisplayName();
	Record.Appearance = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
	Record.BasePresetId = FName(TEXT("BaseMaleUnderwear"));
	Record.bHasMeaningfulAppearance = false;
	Record.AppearanceOrigin = EAvAppearanceOrigin::Factory;
	Record.bIsActive = true;
	Record.bIsSelected = true;
	Record.SortOrder = NextSortOrder;
	Record.CreatedTimestamp = FDateTime::UtcNow().ToUnixTimestamp();
	ActiveCharacterId = NewCharacterId;
	bLoadedActiveCharacterIdWasInvalid = false;
	CharacterSchemaVersion = CurrentCharacterSchemaVersion;
	SynchronizeLegacyAppearanceCache();
	Save();
	UE_LOG(LogTemp, Log,
		TEXT("[AvCharacterRoster] Create CharacterId=%s Name=%s Count=%d SaveGameCommits=1"),
		*NewCharacterId.ToString(), *Record.DisplayName, CharacterRecords.Num());
	return NewCharacterId;
}

bool UCompanyLedgerSubsystem::SetActiveCharacter(FName CharacterId)
{
	if (CharacterId.IsNone())
	{
		return false;
	}
	const bool bRepairingInvalidLoadedActiveId =
		CharacterId == ActiveCharacterId && bLoadedActiveCharacterIdWasInvalid;
	if (CharacterId == ActiveCharacterId && !bRepairingInvalidLoadedActiveId)
	{
		return false;
	}
	if (!CharacterRecords.ContainsByPredicate(
		[CharacterId](const FAvCharacterRecord& Record)
		{
			return Record.CharacterId == CharacterId;
		}))
	{
		return false;
	}
	ActiveCharacterId = CharacterId;
	for (FAvCharacterRecord& Record : CharacterRecords)
	{
		const bool bActive = Record.CharacterId == ActiveCharacterId;
		Record.bIsActive = bActive;
		Record.bIsSelected = bActive;
	}
	bLoadedActiveCharacterIdWasInvalid = false;
	SynchronizeLegacyAppearanceCache();
	Save();
	UE_LOG(LogTemp, Log,
		TEXT("[AvCharacterRoster] Select CharacterId=%s SaveGameCommits=1"),
		*CharacterId.ToString());
	return true;
}

bool UCompanyLedgerSubsystem::DeleteCharacter(FName CharacterId)
{
	if (CharacterRecords.Num() <= 1 || CharacterId.IsNone())
	{
		return false;
	}
	const int32 DeleteIndex = CharacterRecords.IndexOfByPredicate(
		[CharacterId](const FAvCharacterRecord& Record)
		{
			return Record.CharacterId == CharacterId;
		});
	if (DeleteIndex == INDEX_NONE)
	{
		return false;
	}

	const bool bDeletingActive = CharacterRecords[DeleteIndex].CharacterId == ActiveCharacterId;
	const int32 DeletedSortOrder = CharacterRecords[DeleteIndex].SortOrder;
	FName ReplacementId = ActiveCharacterId;
	if (bDeletingActive)
	{
		const FAvCharacterRecord* Next = nullptr;
		const FAvCharacterRecord* Previous = nullptr;
		for (int32 CandidateIndex = 0; CandidateIndex < CharacterRecords.Num(); ++CandidateIndex)
		{
			if (CandidateIndex == DeleteIndex)
			{
				continue;
			}
			const FAvCharacterRecord& Candidate = CharacterRecords[CandidateIndex];
			if (Candidate.SortOrder > DeletedSortOrder &&
				(!Next || Candidate.SortOrder < Next->SortOrder))
			{
				Next = &Candidate;
			}
			if (Candidate.SortOrder <= DeletedSortOrder &&
				(!Previous || Candidate.SortOrder > Previous->SortOrder))
			{
				Previous = &Candidate;
			}
		}
		ReplacementId = Next ? Next->CharacterId :
			(Previous ? Previous->CharacterId : NAME_None);
		if (ReplacementId.IsNone())
		{
			return false;
		}
	}

	CharacterRecords.RemoveAt(DeleteIndex);
	ActiveCharacterId = ReplacementId;
	for (FAvCharacterRecord& Record : CharacterRecords)
	{
		const bool bActive = Record.CharacterId == ActiveCharacterId;
		Record.bIsActive = bActive;
		Record.bIsSelected = bActive;
	}
	SynchronizeLegacyAppearanceCache();
	Save();
	UE_LOG(LogTemp, Log,
		TEXT("[AvCharacterRoster] Delete CharacterId=%s NewActive=%s Count=%d SaveGameCommits=1"),
		*CharacterId.ToString(), *ActiveCharacterId.ToString(), CharacterRecords.Num());
	return true;
}

int32 UCompanyLedgerSubsystem::GetDuplicateCharacterIdCount() const
{
	TSet<FName> Seen;
	int32 DuplicateCount = 0;
	for (const FAvCharacterRecord& Record : CharacterRecords)
	{
		if (Seen.Contains(Record.CharacterId))
		{
			++DuplicateCount;
		}
		Seen.Add(Record.CharacterId);
	}
	return DuplicateCount;
}

bool UCompanyLedgerSubsystem::IsLegacyAppearanceMirrorSynchronized() const
{
	const FAvCharacterRecord* Active = GetActiveCharacter();
	return Active && bHasSavedWorkerAppearance &&
		Active->Appearance.IsEquivalentTo(SavedWorkerAppearance);
}

void UCompanyLedgerSubsystem::SynchronizeLegacyAppearanceCache()
{
	if (const FAvCharacterRecord* Record = GetActiveCharacter())
	{
		SavedWorkerAppearance = Record->Appearance;
		bHasSavedWorkerAppearance = true;
	}
}

void UCompanyLedgerSubsystem::SetActiveCharacterAppearance(
	const FWorkerAppearance& NewAppearance,
	bool bHasMeaningfulAppearance,
	FName BasePresetId,
	EAvAppearanceOrigin AppearanceOrigin)
{
	if (!GetActiveCharacterMutable())
	{
		CreateDefaultCharacterRecord();
	}
	if (FAvCharacterRecord* Record = GetActiveCharacterMutable())
	{
		Record->Appearance = NewAppearance;
		UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(Record->Appearance);
		Record->BasePresetId = BasePresetId;
		Record->bHasMeaningfulAppearance = bHasMeaningfulAppearance;
		Record->AppearanceOrigin = AppearanceOrigin;
		Record->bIsActive = true;
		Record->bIsSelected = true;
	}
	SynchronizeLegacyAppearanceCache();
	Save();
}

void UCompanyLedgerSubsystem::SetActiveCharacterDisplayName(const FString& NewDisplayName)
{
	if (!GetActiveCharacterMutable())
	{
		CreateDefaultCharacterRecord();
	}
	if (FAvCharacterRecord* Record = GetActiveCharacterMutable())
	{
		Record->DisplayName = NewDisplayName;
	}
	Save();
}

bool UCompanyLedgerSubsystem::SetCharacterDisplayName(
	FName CharacterId,
	const FString& NewDisplayName)
{
	FAvCharacterRecord* Record = CharacterRecords.FindByPredicate(
		[CharacterId](const FAvCharacterRecord& Candidate)
		{
			return !CharacterId.IsNone() && Candidate.CharacterId == CharacterId;
		});
	if (!Record)
	{
		return false;
	}
	if (Record->DisplayName.Equals(NewDisplayName, ESearchCase::CaseSensitive))
	{
		return true;
	}
	Record->DisplayName = NewDisplayName;
	Save();
	return true;
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
	SynchronizeLegacyAppearanceCache();
	Save();
}

void UCompanyLedgerSubsystem::SetSavedWorkerAppearance(const FWorkerAppearance& NewAppearance)
{
	SetActiveCharacterAppearance(
		NewAppearance,
		true,
		NAME_None,
		EAvAppearanceOrigin::ManualCustomized);
}

void UCompanyLedgerSubsystem::SetSuppressRaisedHoodHeadgearConflictWarning(bool bSuppress)
{
	if (bSuppressRaisedHoodHeadgearConflictWarning == bSuppress)
	{
		return;
	}
	bSuppressRaisedHoodHeadgearConflictWarning = bSuppress;
	SaveUserSettings();
	UE_LOG(LogTemp, Log,
		TEXT("[AvEquipmentConflictPreference] SuppressRaisedHoodEquipmentWarning=%s Saved=true LegacySerializedField=Headgear"),
		bSuppress ? TEXT("true") : TEXT("false"));
}

void UCompanyLedgerSubsystem::SetSuppressHeadgearHeadphonesConflictWarning(bool bSuppress)
{
	if (bSuppressHeadgearHeadphonesConflictWarning == bSuppress)
	{
		return;
	}
	bSuppressHeadgearHeadphonesConflictWarning = bSuppress;
	SaveUserSettings();
	UE_LOG(LogTemp, Log,
		TEXT("[AvEquipmentConflictPreference] SuppressHeadgearHeadphonesWarning=%s Saved=true"),
		bSuppress ? TEXT("true") : TEXT("false"));
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
