#pragma once

#include "CoreMinimal.h"
#include "Items/APickupItem.h"
#include "ABioPickup.generated.h"

/**
 * «Подозрительный комок» — лёгкий предмет, который роняет санитарный инцидент (§15).
 * Меш и параметры заданы в конструкторе (CDO), поэтому спавненный в рантайме
 * экземпляр виден и подбираем у всех клиентов без отдельной репликации меша.
 * Эффект ThrowBio: ЛКМ метает биологический снаряд.
 */
UCLASS()
class AVARYO_API ABioPickup : public APickupItem
{
	GENERATED_BODY()

public:
	ABioPickup();
};
