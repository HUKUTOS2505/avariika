#include "Game/ARunState.h"

#include "AvaryoCharacter.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "World/AExitZone.h"
#include "World/ARepairable.h"

ARunState::ARunState()
{
	PrimaryActorTick.bCanEverTick = true; // серверная проверка поражения
	PrimaryActorTick.TickInterval = 0.5f;
	bReplicates = true;
	bAlwaysRelevant = true; // состояние забега нужно всем клиентам всегда

	Phase = ERunPhase::InProgress;
	RepairedCount = 0;
	RunStartServerTime = 0.f;
	RunEndServerTime = 0.f;
	bHasExitZone = false;
}

void ARunState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	// Собираем задачи: все сломанные объекты карты на момент старта
	for (TActorIterator<ARepairable> It(GetWorld()); It; ++It)
	{
		if (It->IsBroken())
		{
			Objectives.Add(*It);
			It->OnRepairFinished.AddDynamic(this, &ARunState::OnObjectiveRepaired);
		}
	}

	for (TActorIterator<AExitZone> It(GetWorld()); It; ++It)
	{
		bHasExitZone = true;
		break;
	}

	if (const AGameStateBase* GS = GetWorld()->GetGameState())
	{
		RunStartServerTime = GS->GetServerWorldTimeSeconds();
	}
}

void ARunState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARunState, Phase);
	DOREPLIFETIME(ARunState, Objectives);
	DOREPLIFETIME(ARunState, RepairedCount);
	DOREPLIFETIME(ARunState, RunStartServerTime);
	DOREPLIFETIME(ARunState, RunEndServerTime);
	DOREPLIFETIME(ARunState, PlayerStats);
}

ARunState* ARunState::Get(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}
	for (TActorIterator<ARunState> It(World); It; ++It)
	{
		return *It;
	}
	return nullptr;
}

float ARunState::GetElapsedSeconds() const
{
	const AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (!GS || RunStartServerTime <= 0.f)
	{
		return 0.f;
	}
	const float EndTime = (Phase == ERunPhase::InProgress) ? GS->GetServerWorldTimeSeconds() : RunEndServerTime;
	return FMath::Max(EndTime - RunStartServerTime, 0.f);
}

void ARunState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || Phase != ERunPhase::InProgress)
	{
		return;
	}

	// Статистика + поражение (вся бригада ранена — поднимать некому)
	int32 NumPlayers = 0;
	int32 NumWounded = 0;
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		UVitalsComponent* Vitals = It->VitalsComponent;
		if (!Vitals)
		{
			continue;
		}
		++NumPlayers;
		if (Vitals->IsWounded())
		{
			++NumWounded;
		}

		FPlayerRunStats& Stats = FindOrAddStats(*It);
		if (Vitals->IsPanicking())
		{
			Stats.PanicSeconds += DeltaSeconds;
		}
		if (Vitals->IsWounded() && !Stats.bWasWounded)
		{
			++Stats.TimesWounded;
		}
		Stats.bWasWounded = Vitals->IsWounded();
		if (Vitals->IsSoiled() && !Stats.bWasSoiled)
		{
			++Stats.Incidents;
		}
		Stats.bWasSoiled = Vitals->IsSoiled();
	}
	if (NumPlayers > 0 && NumWounded == NumPlayers)
	{
		FinishRun(ERunPhase::Lost);
	}
}

FPlayerRunStats& ARunState::FindOrAddStats(AAvaryoCharacter* Who)
{
	for (FPlayerRunStats& Stats : PlayerStats)
	{
		if (Stats.Character == Who)
		{
			return Stats;
		}
	}
	FPlayerRunStats& Stats = PlayerStats.AddDefaulted_GetRef();
	Stats.Character = Who;
	return Stats;
}

void ARunState::AddRevive(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Who)
	{
		++FindOrAddStats(Who).Revives;
	}
}

void ARunState::AddDrag(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Who)
	{
		++FindOrAddStats(Who).Drags;
	}
}

void ARunState::OnObjectiveRepaired(ARepairable* Repairable, AAvaryoCharacter* FinishedBy)
{
	if (!HasAuthority() || Phase != ERunPhase::InProgress)
	{
		return;
	}

	if (FinishedBy)
	{
		++FindOrAddStats(FinishedBy).Repairs;
	}

	RepairedCount = 0;
	for (const ARepairable* Objective : Objectives)
	{
		if (Objective && !Objective->IsBroken())
		{
			++RepairedCount;
		}
	}

	// Без зоны выхода на карте побеждаем сразу после последней починки
	if (AreAllObjectivesComplete() && !bHasExitZone)
	{
		FinishRun(ERunPhase::Won);
	}
}

void ARunState::NotifyTeamAtExit()
{
	if (HasAuthority() && Phase == ERunPhase::InProgress && AreAllObjectivesComplete())
	{
		FinishRun(ERunPhase::Won);
	}
}

void ARunState::FinishRun(ERunPhase NewPhase)
{
	Phase = NewPhase;
	if (const AGameStateBase* GS = GetWorld()->GetGameState())
	{
		RunEndServerTime = GS->GetServerWorldTimeSeconds();
	}
}

// ---------- URunStateSubsystem ----------

bool URunStateSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URunStateSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Спавним только на сервере; клиентам RunState приедет по репликации
	if (InWorld.GetNetMode() != NM_Client && !ARunState::Get(&InWorld))
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		InWorld.SpawnActor<ARunState>(Params);
	}
}
