#include "Game/AvariikaOnlineSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

const FName UAvariikaOnlineSubsystem::SessionName = NAME_GameSession;

void UAvariikaOnlineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
	{
		Sessions = OSS->GetSessionInterface();
	}
}

void UAvariikaOnlineSubsystem::Deinitialize()
{
	Sessions.Reset();
	Super::Deinitialize();
}

static bool IsNullOnline()
{
	const IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	return !OSS || OSS->GetSubsystemName() == FName(TEXT("NULL"));
}

void UAvariikaOnlineSubsystem::HostGame(int32 MaxPlayers, FString MapName)
{
	if (!Sessions.IsValid())
	{
		return;
	}
	PendingMap = MapName;
	PendingMaxPlayers = MaxPlayers;
	if (Sessions->GetNamedSession(SessionName))
	{
		// DestroySession асинхронен на EOS/Steam — создавать новую надо ИЗ коллбэка, иначе CreateSession
		// с тем же именем падает «session already exists» и хост молча не уезжает (на NULL работало случайно).
		DestroyHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UAvariikaOnlineSubsystem::HandleDestroyForRecreate));
		Sessions->DestroySession(SessionName);
		return;
	}
	DoCreateSession();
}

void UAvariikaOnlineSubsystem::HandleDestroyForRecreate(FName InName, bool bWasSuccessful)
{
	if (Sessions.IsValid())
	{
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
	}
	DoCreateSession(); // прошлая сессия снесена — теперь безопасно создать
}

void UAvariikaOnlineSubsystem::DoCreateSession()
{
	if (!Sessions.IsValid())
	{
		return;
	}
	const bool bLan = IsNullOnline();
	FOnlineSessionSettings S;
	S.bIsLANMatch = bLan;
	S.NumPublicConnections = FMath::Max(1, PendingMaxPlayers);
	S.NumPrivateConnections = 0;
	S.bShouldAdvertise = true;
	S.bAllowJoinInProgress = true;
	S.bAllowJoinViaPresence = true;
	S.bUsesPresence = !bLan;            // presence — для EOS/Steam, не для NULL
	S.bUseLobbiesIfAvailable = !bLan;
	S.bIsDedicated = false;

	CreateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UAvariikaOnlineSubsystem::HandleCreateComplete));
	Sessions->CreateSession(0, SessionName, S);
}

void UAvariikaOnlineSubsystem::HandleCreateComplete(FName InName, bool bWasSuccessful)
{
	if (Sessions.IsValid())
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
	}
	UE_LOG(LogTemp, Display, TEXT("[Avariika] CreateSession %s success=%d"), *InName.ToString(), bWasSuccessful);
	if (bWasSuccessful)
	{
		if (UWorld* W = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr)
		{
			W->ServerTravel(PendingMap + TEXT("?listen"));
		}
	}
}

void UAvariikaOnlineSubsystem::FindGames()
{
	if (!Sessions.IsValid())
	{
		return;
	}
	SearchSettings = MakeShareable(new FOnlineSessionSearch());
	SearchSettings->bIsLanQuery = IsNullOnline();
	SearchSettings->MaxSearchResults = 20;
	FindHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UAvariikaOnlineSubsystem::HandleFindComplete));
	Sessions->FindSessions(0, SearchSettings.ToSharedRef());
}

void UAvariikaOnlineSubsystem::HandleFindComplete(bool bWasSuccessful)
{
	if (Sessions.IsValid())
	{
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
	}
	const int32 Num = SearchSettings.IsValid() ? SearchSettings->SearchResults.Num() : 0;
	UE_LOG(LogTemp, Display, TEXT("[Avariika] FindSessions success=%d, найдено: %d"), bWasSuccessful, Num);
}

void UAvariikaOnlineSubsystem::JoinGameByIndex(int32 Index)
{
	if (!Sessions.IsValid() || !SearchSettings.IsValid() || !SearchSettings->SearchResults.IsValidIndex(Index))
	{
		return;
	}
	JoinHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UAvariikaOnlineSubsystem::HandleJoinComplete));
	Sessions->JoinSession(0, SessionName, SearchSettings->SearchResults[Index]);
}

void UAvariikaOnlineSubsystem::HandleJoinComplete(FName InName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Sessions.IsValid())
	{
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
	}
	if (Result != EOnJoinSessionCompleteResult::Success || !Sessions.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Avariika] JoinSession не удалось (%d)"), (int32)Result);
		return;
	}
	FString ConnectStr;
	if (Sessions->GetResolvedConnectString(SessionName, ConnectStr))
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (APlayerController* PC = GI->GetFirstLocalPlayerController())
			{
				UE_LOG(LogTemp, Display, TEXT("[Avariika] ClientTravel → %s"), *ConnectStr);
				PC->ClientTravel(ConnectStr, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UAvariikaOnlineSubsystem::LeaveGame()
{
	if (Sessions.IsValid() && Sessions->GetNamedSession(SessionName))
	{
		Sessions->DestroySession(SessionName);
	}
}

int32 UAvariikaOnlineSubsystem::GetNumFoundSessions() const
{
	return SearchSettings.IsValid() ? SearchSettings->SearchResults.Num() : 0;
}
