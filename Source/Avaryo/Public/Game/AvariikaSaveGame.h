#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AvariikaSaveGame.generated.h"

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
};
