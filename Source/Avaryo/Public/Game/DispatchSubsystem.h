#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DispatchSubsystem.generated.h"

/**
 * Память диспетчерской петли между картами. Живёт в GameInstance, поэтому
 * переживает ServerTravel (как UCompanyLedgerSubsystem). Доска заявок
 * (ACallBoard) в хабе пишет сюда «куда едем и куда вернуться», объектная
 * карта читает заявку для брифинга диспетчера, а по завершении забега
 * ARunState возвращает бригаду в хаб.
 *
 * Цикл: ХАБ (доска заявок) → ServerTravel на объект (запомнили хаб+заявку)
 *       → забег → «Акт» → [R] → ServerTravel обратно в хаб.
 */
UCLASS()
class AVARYO_API UDispatchSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Long-package-name хаба, куда вернуться после забега. Пусто = карту тестим напрямую (без хаба) → рестарт. */
	const FString& GetHomeHubMap() const { return HomeHubMap; }
	bool HasHomeHub() const { return !HomeHubMap.IsEmpty(); }

	/** Id принятой заявки (для брифинга диспетчера на объекте). */
	FName GetActiveCallId() const { return ActiveCallId; }
	const FString& GetActiveCallTitle() const { return ActiveCallTitle; }
	bool HasActiveCall() const { return !ActiveCallId.IsNone(); }

	/** Хаб принял заявку: запомнить, куда вернуться и что за вызов. Зовётся перед ServerTravel на объект. */
	void BeginJob(const FString& InHubMap, FName InCallId, const FString& InCallTitle)
	{
		HomeHubMap = InHubMap;
		ActiveCallId = InCallId;
		ActiveCallTitle = InCallTitle;
	}

	/** Забег закрыт, вернулись на базу — снять активную заявку (адрес хаба остаётся как «дом»). */
	void ClearActiveCall()
	{
		ActiveCallId = NAME_None;
		ActiveCallTitle.Reset();
	}

protected:
	/** Куда вернуться (хаб). */
	FString HomeHubMap;

	/** Что за заявка сейчас выполняется. */
	FName ActiveCallId = NAME_None;
	FString ActiveCallTitle;
};
