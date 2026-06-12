#include "Game/ARunState.h"

#include "AvaryoCharacter.h"
#include "Components/VitalsComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "World/AExitZone.h"
#include "World/ARepairable.h"

// Реплики диспетчера: сухой сарказм уставшего начальника смены. «{X}» — имя/название/число.
namespace DispatcherLines
{
	const TArray<FString> Greeting = {
		TEXT("Бригада, приём. Заявка плёвая: объектов всего {X}. Через полчаса жду в гараже."),
		TEXT("Так, мужики. Поломок — {X}, на дворе ночь, премия под вопросом. Работаем."),
		TEXT("Диспетчерская — бригаде: по заявке «обычная поломка минут на пять». По факту — {X}. Удачи."),
		TEXT("Принята заявка №47: объектов {X}. Жильцы уже звонят. Не позорьте контору."),
	};
	const TArray<FString> RepairDone = {
		TEXT("«{X}» — принято. Неужели сами справились."),
		TEXT("Отметил: «{X}» готов. Продолжаем не ломать остальное."),
		TEXT("«{X}» починен. Записал в акт, не благодарите."),
	};
	const TArray<FString> AllDone = {
		TEXT("Всё?! Так, быстро все в ГАЗель, пока опять не заискрило."),
		TEXT("Заявка закрыта. Сбор у машины, перекличка."),
		TEXT("Принято, всё работает. В ГАЗель шагом марш. Курить — НЕ в машине."),
	};
	const TArray<FString> GasExplosion = {
		TEXT("КТО КУРИЛ НА ГАЗОВОЙ ЗАЯВКЕ?! {X}, я ведь слышал щелчок зажигалки!"),
		TEXT("Взрыв на объекте. {X}, это твоя зона ответственности. Премии не будет."),
		TEXT("Диспетчерская фиксирует хлопок газа. Жильцам скажем — салют в честь дня монтажника."),
	};
	const TArray<FString> ShortCircuit = {
		TEXT("Щиток замкнуло. {X}, тестером надо ТЫКАТЬ, а не ЛУПИТЬ."),
		TEXT("Слышу, заискрило. Минута на остывание — и на пересдачу, {X}."),
		TEXT("{X} устроил иллюминацию. В акте напишу «плановое отключение»."),
	};
	const TArray<FString> Wounded = {
		TEXT("{X} ранен. Бывает. Кто-нибудь, поднимите его, он мне акт не подписал."),
		TEXT("Минус один: {X} отдыхает на земле. Аптечка в зубы — и работать."),
		TEXT("{X}, лежать на смене запрещено инструкцией. Подъём."),
	};
	const TArray<FString> Incident = {
		TEXT("{X}... до биотуалета было сто метров. СТО. МЕТРОВ."),
		TEXT("Фиксирую санитарный инцидент. {X}, химчистку вычту из зарплаты."),
		TEXT("{X}, это пойдёт в акт отдельной строкой. С формулировкой."),
	};
	const TArray<FString> ToiletVisit = {
		TEXT("{X} отошёл по регламенту. Не отвлекаем человека."),
		TEXT("Зафиксировал технологический перерыв у {X}. Дисциплина!"),
	};
	const TArray<FString> Victory = {
		TEXT("Объект сдан. Жалоб много, премии не будет, но все живы — уже праздник."),
		TEXT("Заявка закрыта. По домам. Завтра в то же время, и не опаздывать."),
	};
	const TArray<FString> Defeat = {
		TEXT("Бригада, приём... Приём!.. Так. Высылаю вторую бригаду. За вами."),
		TEXT("Вся бригада лежит. В акте напишу «технический перерыв». Длинный."),
	};
	// Крики паникующих монтёров (концепт §18): сами лезут в эфир и шумят
	const TArray<FString> PanicCries = {
		TEXT("Мужики, я не пойду туда!"),
		TEXT("Я слышал шаги! Точно слышал!"),
		TEXT("Мне надо в туалет... срочно..."),
		TEXT("Кто-нибудь, посветите сюда!!"),
		TEXT("Давайте быстрее, а?! Очень страшно!"),
		TEXT("Что это был за звук?!"),
		TEXT("Я домой хочу. Официально заявляю."),
	};
}

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
	NextChatterTime = 0.f;
}

void ARunState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	// Рандомизация поломок: каждый забег ломается случайное подмножество
	// объектов карты (минимум 2, либо все, если их меньше)
	TArray<ARepairable*> AllRepairables;
	for (TActorIterator<ARepairable> It(GetWorld()); It; ++It)
	{
		AllRepairables.Add(*It);
	}
	for (int32 i = AllRepairables.Num() - 1; i > 0; --i)
	{
		AllRepairables.Swap(i, FMath::RandRange(0, i)); // Фишер-Йетс
	}
	const int32 NumBroken = AllRepairables.Num() <= 2
		? AllRepairables.Num()
		: FMath::RandRange(2, AllRepairables.Num());

	for (int32 i = 0; i < AllRepairables.Num(); ++i)
	{
		ARepairable* Repairable = AllRepairables[i];
		Repairable->SetBroken(i < NumBroken);
		if (i < NumBroken)
		{
			Objectives.Add(Repairable);
			Repairable->OnRepairFinished.AddDynamic(this, &ARunState::OnObjectiveRepaired);
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

	// Приветствие с задержкой: мультикаст в первый кадр клиенты ещё не получат
	GetWorldTimerManager().SetTimer(GreetingTimer, this, &ARunState::SendGreeting, 6.f, false);
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
	const float Now = GetWorld()->GetTimeSeconds();
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
			TickPanicCries(*It, Now);
		}
		else
		{
			NextPanicCryTime.Remove(*It); // успокоился — следующая паника заново отсчитает крик
		}
		if (Vitals->IsWounded() && !Stats.bWasWounded)
		{
			++Stats.TimesWounded;
			DispatcherSay(DispatcherLines::Wounded, CrewName(*It));
		}
		Stats.bWasWounded = Vitals->IsWounded();
		if (Vitals->IsSoiled() && !Stats.bWasSoiled)
		{
			++Stats.Incidents;
			DispatcherSay(DispatcherLines::Incident, CrewName(*It), /*bImportant=*/true);
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

void ARunState::AddToiletVisit(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Who)
	{
		++FindOrAddStats(Who).ToiletVisits;
		if (FMath::FRand() < 0.35f) // комментирует не каждый визит — туалетный юмор дозируем
		{
			DispatcherSay(DispatcherLines::ToiletVisit, CrewName(Who));
		}
	}
}

void ARunState::NotifyGasExplosion(AAvaryoCharacter* Culprit)
{
	if (HasAuthority())
	{
		DispatcherSay(DispatcherLines::GasExplosion, CrewName(Culprit), /*bImportant=*/true);
	}
}

void ARunState::NotifyShortCircuit(AAvaryoCharacter* Culprit)
{
	if (HasAuthority())
	{
		DispatcherSay(DispatcherLines::ShortCircuit, CrewName(Culprit), /*bImportant=*/true);
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
	else if (AreAllObjectivesComplete())
	{
		DispatcherSay(DispatcherLines::AllDone, FString(), /*bImportant=*/true);
	}
	else if (Repairable)
	{
		DispatcherSay(DispatcherLines::RepairDone, Repairable->DisplayName.ToString());
	}
}

void ARunState::NotifyTeamAtExit()
{
	if (HasAuthority() && Phase == ERunPhase::InProgress && AreAllObjectivesComplete())
	{
		FinishRun(ERunPhase::Won);
	}
}

void ARunState::RequestRestart()
{
	if (!HasAuthority() || Phase == ERunPhase::InProgress)
	{
		return; // рестарт только с финального экрана
	}
	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
	{
		GameMode->ProcessServerTravel(TEXT("?restart")); // новая смена, новые поломки
	}
}

void ARunState::FinishRun(ERunPhase NewPhase)
{
	Phase = NewPhase;
	if (const AGameStateBase* GS = GetWorld()->GetGameState())
	{
		RunEndServerTime = GS->GetServerWorldTimeSeconds();
	}
	DispatcherSay(NewPhase == ERunPhase::Won ? DispatcherLines::Victory : DispatcherLines::Defeat,
		FString(), /*bImportant=*/true);
}

// ---------- Диспетчер ----------

void ARunState::SendGreeting()
{
	DispatcherSay(DispatcherLines::Greeting, FString::FromInt(Objectives.Num()), /*bImportant=*/true);
}

void ARunState::DispatcherSay(const TArray<FString>& Pool, const FString& Param, bool bImportant, const FString& Speaker)
{
	if (!HasAuthority() || Pool.Num() == 0)
	{
		return;
	}
	const float Now = GetWorld()->GetTimeSeconds();
	if (!bImportant && Now < NextChatterTime)
	{
		return; // эфир занят — неважное глотаем
	}
	NextChatterTime = Now + 6.f;

	FString Line = Pool[FMath::RandRange(0, Pool.Num() - 1)];
	Line.ReplaceInline(TEXT("{X}"), *Param);
	MulticastDispatcherSay(Speaker.IsEmpty() ? TEXT("ДИСПЕТЧЕР") : Speaker, Line);
}

void ARunState::TickPanicCries(AAvaryoCharacter* Who, float Now)
{
	float& CryAt = NextPanicCryTime.FindOrAdd(Who);
	if (CryAt <= 0.f)
	{
		CryAt = Now + FMath::FRandRange(4.f, 10.f); // паника началась — крик зреет
		return;
	}
	if (Now < CryAt)
	{
		return;
	}
	CryAt = Now + FMath::FRandRange(12.f, 25.f);

	DispatcherSay(DispatcherLines::PanicCries, FString(), /*bImportant=*/false,
		CrewName(Who) + TEXT(" (паника)"));
	Who->MakeNoise(0.6f, Who, Who->GetActorLocation()); // крик слышно — монстру понравится
}

void ARunState::MulticastDispatcherSay_Implementation(const FString& Speaker, const FString& Line)
{
	FDispatcherLine& Entry = DispatcherLines.AddDefaulted_GetRef();
	Entry.Speaker = Speaker;
	Entry.Text = Line;
	Entry.ReceivedAt = GetWorld()->GetTimeSeconds();
	while (DispatcherLines.Num() > 3) // на экране держим максимум три плашки
	{
		DispatcherLines.RemoveAt(0);
	}
}

FString ARunState::CrewName(const AAvaryoCharacter* Who)
{
	const APlayerState* PS = Who ? Who->GetPlayerState() : nullptr;
	return PS ? PS->GetPlayerName() : TEXT("Монтёр");
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
