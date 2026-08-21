#pragma once

#include "CoreMinimal.h"
#include "Components/WorkerAppearanceComponent.h"
#include "GameFramework/SaveGame.h"
#include "AvariikaSaveGame.generated.h"

/** Why the active appearance exists. Reset availability is stored separately. */
UENUM(BlueprintType)
enum class EAvAppearanceOrigin : uint8
{
	Factory,
	RandomGenerated,
	PresetApplied,
	ManualCustomized
};

/** Уровни апгрейдов инструментов бригады (см. SPEC_Shop_Progression.md §3). */
USTRUCT(BlueprintType)
struct FEquipmentLevels
{
	GENERATED_BODY()

	UPROPERTY() int32 Flashlight   = 1;  // 1..4
	UPROPERTY() int32 Tester       = 1;  // 1..3
	UPROPERTY() int32 Extinguisher = 1;  // 1..3
	UPROPERTY() int32 Radio        = 1;  // 1..3
	UPROPERTY() int32 Welder       = 1;  // 1..3
	UPROPERTY() int32 Cameras      = 0;  // 0 нет, 1 переносные, 2 +слоты
	UPROPERTY() bool  bHelmet  = false;
	UPROPERTY() bool  bGloves  = false;
	UPROPERTY() bool  bGasMask = false;
};

/** Склад расходников бригады (выдаётся на старте смены). */
USTRUCT(BlueprintType)
struct FConsumableStock
{
	GENERATED_BODY()

	UPROPERTY() int32 Batteries    = 0;
	UPROPERTY() int32 Fuses        = 0;
	UPROPERTY() int32 Medkits      = 0;
	UPROPERTY() int32 AmmoniaSalts = 0; // нашатырь — сбить панику
	UPROPERTY() int32 FuelCans     = 0;
	UPROPERTY() int32 FoamRefill   = 0;
};

/** Карьерная статистика конторы за всё время (для «диспетчер помнит» + экрана карьеры). */
USTRUCT(BlueprintType)
struct FCareerStats
{
	GENERATED_BODY()

	UPROPERTY() int32 ShiftsWon       = 0;
	UPROPERTY() int32 ShiftsLost      = 0;
	UPROPERTY() int32 TotalRepairs    = 0;
	UPROPERTY() int32 BuildingsBlownUp = 0; // взорвали дом → диспетчер припомнит
	UPROPERTY() int32 CigsSmoked      = 0;
	UPROPERTY() int32 TotalIncidents  = 0;
	UPROPERTY() bool  bInsured        = false; // куплена ли страховка инструмента на текущую смену
	UPROPERTY() TArray<FName> Cosmetics;       // купленные наклейки/цвета касок
};

/** One project-owned character record. FName IDs are retained for serialized compatibility. */
USTRUCT(BlueprintType)
struct FAvCharacterRecord
{
	GENERATED_BODY()

	UPROPERTY() FName CharacterId;
	UPROPERTY() FString DisplayName;
	UPROPERTY() FWorkerAppearance Appearance;
	UPROPERTY() FName BasePresetId;
	UPROPERTY() bool bHasMeaningfulAppearance = false;
	/** Defaults conservatively for records serialized before schema v3. Load migration refines it. */
	UPROPERTY() EAvAppearanceOrigin AppearanceOrigin = EAvAppearanceOrigin::ManualCustomized;
	UPROPERTY() bool bIsActive = false;
	UPROPERTY() bool bIsSelected = false;
	UPROPERTY() int32 SortOrder = 0;
	/** Unix UTC seconds. Zero means an older record with no authored creation timestamp. */
	UPROPERTY() int64 CreatedTimestamp = 0;
};

/**
 * Сейв конторы на диск (авторитет хоста): баланс, репутация, апгрейды, склад,
 * карьера и квота переживают выход из игры. Слот "AvariikaCompany".
 * См. SPEC_Shop_Progression.md.
 */
UCLASS()
class AVARYO_API UAvariikaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Zero means a legacy save created before character records were introduced. */
	UPROPERTY() int32 SchemaVersion = 0;
	UPROPERTY() FName ActiveCharacterId;
	UPROPERTY() TArray<FAvCharacterRecord> CharacterRecords;

	UPROPERTY() int32 CompanyBalance   = 0;
	UPROPERTY() int32 ShiftNumber      = 1;
	UPROPERTY() int32 ReputationPoints = 0;
	UPROPERTY() FEquipmentLevels Equipment;
	UPROPERTY() FConsumableStock Stock;
	UPROPERTY() FCareerStats     Career;
	// Квота: сколько ₽ сдать и до какой смены (0 = квоты нет / песочница).
	UPROPERTY() int32 QuotaTarget        = 0;
	UPROPERTY() int32 QuotaDeadlineShift = 0;
	UPROPERTY() int32 QuotaPaidSoFar     = 0;
	UPROPERTY() int32 QuotaWindowShifts  = 3;     // на сколько смён даётся очередная квота
	UPROPERTY() bool  bQuotaFailed       = false; // квота провалена → контора закрыта
	UPROPERTY() bool bHasSavedWorkerAppearance = false;
	UPROPERTY() FWorkerAppearance SavedWorkerAppearance;
};

/** Global, character-independent warning preferences. Kept out of appearance saves. */
UCLASS()
class AVARYO_API UAvariikaUserSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Legacy serialized name retained; now covers Raised Hood versus Headgear and Headphones. */
	UPROPERTY() bool bSuppressRaisedHoodHeadgearConflictWarning = false;

	/** Independent warning preference for the Headgear <-> Headphones product rule. */
	UPROPERTY() bool bSuppressHeadgearHeadphonesConflictWarning = false;

	/** Project-owned Main Menu settings schema. Missing/zero means a pre-menu settings save. */
	UPROPERTY() int32 MenuSettingsSchemaVersion = 0;
	UPROPERTY() int32 ResolutionX = 0;
	UPROPERTY() int32 ResolutionY = 0;
	UPROPERTY() int32 WindowMode = -1;
	UPROPERTY() int32 AspectRatio = 0;
	UPROPERTY() bool bVSync = false;
	UPROPERTY() float FrameRateLimit = 0.f;
	UPROPERTY() float Brightness = 1.f;
	UPROPERTY() int32 ColorDeficiencyType = 0;
	UPROPERTY() int32 ColorDeficiencyStrength = 0;
	UPROPERTY() int32 OverallQuality = -1;
	UPROPERTY() int32 GlobalIlluminationQuality = -1;
	UPROPERTY() int32 ShadowQuality = -1;
	UPROPERTY() int32 AntiAliasingQuality = -1;
	UPROPERTY() int32 ViewDistanceQuality = -1;
	UPROPERTY() int32 TextureQuality = -1;
	UPROPERTY() int32 EffectsQuality = -1;
	UPROPERTY() int32 ReflectionQuality = -1;
	UPROPERTY() int32 FoliageQuality = -1;
	UPROPERTY() int32 ShadingQuality = -1;
	UPROPERTY() int32 PostProcessQuality = -1;
	UPROPERTY() int32 GlobalIlluminationMethod = 1;
	UPROPERTY() int32 ReflectionMethod = 1;
	UPROPERTY() int32 AntiAliasingMethod = 4;
	UPROPERTY() bool bMotionBlur = false;
	UPROPERTY() bool bLensFlares = true;
	UPROPERTY() bool bBloom = true;
	UPROPERTY() bool bAmbientOcclusion = true;
	UPROPERTY() float ResolutionScale = 100.f;
	UPROPERTY() float MasterVolume = 1.f;
	UPROPERTY() float SFXVolume = 1.f;
	UPROPERTY() float MusicVolume = 1.f;
	UPROPERTY() float VoiceVolume = 1.f;
	UPROPERTY() FString Culture;
};
