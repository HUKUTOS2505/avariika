#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACallBoard.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UBoxComponent;
class USoundBase;
class AAvaryoCharacter;

/** Одна заявка на доске диспетчера. */
USTRUCT(BlueprintType)
struct FCallListing
{
	GENERATED_BODY()

	/** Короткий id заявки («Dom»/«Zavod»/«Bolnitsa») — диспетчер и леджер ссылаются на него. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Call")
	FName Id = NAME_None;

	/** Заголовок для HUD/таблички. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Call")
	FString Title;

	/** Короткое описание заявки (тип аварии). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Call")
	FString Brief;

	/** Карта объекта — long package name, напр. «/Game/Avariika/Maps/L_Dom». */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Call")
	FString ObjectMap;

	/** false = «скоро» (завод/больница ещё не готовы) — на доске видно, но взять нельзя. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Call")
	bool bAvailable = true;
};

/**
 * «Стена с вызовами» в хабе. Игрок нажимает E → бригада принимает заявку и
 * выезжает (ServerTravel на карту объекта). Куда вернуться запоминается в
 * UDispatchSubsystem, поэтому после «Акта» бригада возвращается на базу.
 *
 * Присутствие ACallBoard на карте — признак ХАБА для ARunState (там нет забега:
 * ни поломок, ни победы/поражения; диспетчер только приветствует и выдаёт заявку).
 */
UCLASS()
class AVARYO_API ACallBoard : public AActor
{
	GENERATED_BODY()

public:
	ACallBoard();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CallBoard")
	TObjectPtr<UStaticMeshComponent> Board;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CallBoard")
	TObjectPtr<UTextRenderComponent> Label;

	/** Зона взаимодействия (фолбэк-оверлап, если игрок не смотрит точно в доску). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CallBoard")
	TObjectPtr<UBoxComponent> Zone;

	/** Заявки на доске. Если пусто — BeginPlay заполнит дефолтом (Дом доступен, Завод/Больница «скоро»). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CallBoard")
	TArray<FCallListing> Calls;

	/** Куда вернуться после забега (long package name хаба). Пусто → берём текущую карту автоматически. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CallBoard")
	FString HubMapOverride;

	/** Сколько секунд держать брифинг перед выездом (диспетчер + завод двигателя успевают прозвучать). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CallBoard")
	float TravelDelay = 2.2f;

	/** Бип/щелчок рации при приёме заявки (дефолт — RadioComm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CallBoard")
	TObjectPtr<USoundBase> AcceptSound;

	/** Звук заводящегося двигателя «поехали» при приёме заявки (дефолт — EngineStart). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CallBoard")
	TObjectPtr<USoundBase> EngineStartSound;

	/** Выбранная заявка (для подсказки HUD). */
	UFUNCTION(BlueprintPure, Category="CallBoard")
	int32 GetSelectedIndex() const { return SelectedIndex; }

	UFUNCTION(BlueprintPure, Category="CallBoard")
	const TArray<FCallListing>& GetCalls() const { return Calls; }

	/** Индекс первой доступной заявки или INDEX_NONE. */
	int32 FirstAvailable() const;

	/** Есть ли доступная заявка (для подсказки HUD «[E] Взять заявку»). */
	UFUNCTION(BlueprintPure, Category="CallBoard")
	bool HasAvailableCall() const { return FirstAvailable() != INDEX_NONE; }

	/** Игрок нажал E у доски — принять заявку и выехать. Только сервер. */
	void AcceptBy(AAvaryoCharacter* Who);

protected:
	int32 SelectedIndex = 0;

	/** Защита от двойного приёма, пока идёт отсчёт до выезда. */
	bool bTraveling = false;

	/** URL для отложенного ServerTravel. */
	FString PendingTravelURL;

	FTimerHandle TravelTimer;

	/** Серверный выезд по таймеру (после брифинга). */
	void DoTravel();

	/** Обновить текст на табличке по списку заявок. */
	void RefreshLabel();
};
