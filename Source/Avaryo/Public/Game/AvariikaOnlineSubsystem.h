#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "AvariikaOnlineSubsystem.generated.h"

/**
 * Кооп-сессии бригады поверх UE OnlineSubsystem.
 * Сейчас работает на NULL-подсистеме (LAN) — тест через 2 инстанса / PIE-2.
 * EOS/Steam подключается БЕЗ правки кода — только конфиг DefaultEngine.ini
 * ([OnlineSubsystem] DefaultPlatformService=EOS) + плагин.
 * Хост: HostGame → CreateSession → ServerTravel(map?listen).
 * Клиент: FindGames → JoinGameByIndex → ClientTravel по resolved-адресу.
 */
UCLASS()
class AVARYO_API UAvariikaOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Создать сессию и уехать на карту листен-сервером. */
	void HostGame(int32 MaxPlayers = 6, FString MapName = TEXT("/Game/FirstPerson/Lvl_FirstPerson"));

	/** Найти открытые сессии (результаты — в SearchSettings). */
	void FindGames();

	/** Подключиться к найденной сессии по индексу. */
	void JoinGameByIndex(int32 Index);

	/** Выйти/закрыть сессию. */
	void LeaveGame();

	/** Сколько сессий найдено последним поиском. */
	int32 GetNumFoundSessions() const;

protected:
	IOnlineSessionPtr Sessions;
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	FString PendingMap;

	void HandleCreateComplete(FName SessionName, bool bWasSuccessful);
	void HandleFindComplete(bool bWasSuccessful);
	void HandleJoinComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	FDelegateHandle CreateHandle;
	FDelegateHandle FindHandle;
	FDelegateHandle JoinHandle;

	static const FName SessionName; // NAME_GameSession
};
