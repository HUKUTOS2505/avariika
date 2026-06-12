#include "Game/ARunState.h"

#include "AvaryoCharacter.h"
#include "Components/UFlashlightComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Items/APickupItem.h"
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
	// Чужой голос в эфире дешёвой рации (§18 «рация ловит чужой голос»): двусмысленно, жутко
	const TArray<FString> RadioGhost = {
		TEXT("...ш-ш-ш... за спиной... не оборачивайся..."),
		TEXT("...я уже внутри... вы меня впустили..."),
		TEXT("...слышу вас... иду на голос..."),
		TEXT("...кто выключил свет... кто выключил све-е..."),
		TEXT("...помогите... я застрял в подвале... с две тысячи третьего..."),
		TEXT("...не туда чините... совсем не туда..."),
		TEXT("...ещё один... нас тут уже семеро..."),
		TEXT("...тёпленькие... приходите..."),
	};
	// Колхозный ремонт без инструмента (§18 «не тот инструмент»)
	const TArray<FString> BotchRepair = {
		TEXT("{X}, это не ремонт, это художественная самодеятельность. Но раз держится — молчу."),
		TEXT("Чем ты это чинил, {X}, коленкой? В акте так и напишу: «восстановлено народными методами»."),
		TEXT("{X} закрыл объект без инструмента. Гарантия — до выхода из подъезда."),
		TEXT("Принято от {X}. Колхоз, но работает. Премию урежу за стиль."),
	};
	// Реплики начальника про выданный дешёвый комплект (§18 «дешёвое оборудование»)
	const TArray<FString> CheapGearGreeting = {
		TEXT("Да, и комплект вам выдали бюджетный — фонари с рынка, рация с помехами. Экономия, мужики."),
		TEXT("Снаряга сегодня дешёвая: будет моргать и шипеть. Бухгалтерия так решила, не я."),
		TEXT("Предупреждаю: оборудование уценённое. Глючит — это не монстр, это смета."),
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
	bCheapGear = false;
	NextRadioGhostTime = 0.f;
	ShiftNumber = 1;
	CompanyBalanceStart = 0;
	Reputation = 0;
	ShiftNet = 0;
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

	// Бухгалтерия конторы (§19): подтягиваем баланс/смену/репутацию из леджера, переживающего рестарт
	if (const UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (const UCompanyLedgerSubsystem* Ledger = GI->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			ShiftNumber = Ledger->GetShiftNumber();
			CompanyBalanceStart = Ledger->GetBalance();
			Reputation = Ledger->GetReputation();
		}
	}

	// Косяки оборудования (§18): шанс дешёвого комплекта зависит от репутации
	// (хорошая контора реже получает рыночный хлам, плохая — чаще).
	const float CheapChance = FMath::Clamp(0.4f - 0.03f * Reputation, 0.1f, 0.7f);
	bCheapGear = FMath::FRand() < CheapChance;

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
	DOREPLIFETIME(ARunState, bCheapGear);
	DOREPLIFETIME(ARunState, ShiftNumber);
	DOREPLIFETIME(ARunState, CompanyBalanceStart);
	DOREPLIFETIME(ARunState, Reputation);
	DOREPLIFETIME(ARunState, ShiftNet);
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

		ApplyCheapGear(*It);

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

	TickRadioInterference(Now);

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

void ARunState::AddBotchedRepair(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Who)
	{
		++FindOrAddStats(Who).BotchedRepairs;
		DispatcherSay(DispatcherLines::BotchRepair, CrewName(Who), /*bImportant=*/true);
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

	// Бухгалтерия (§19): сумма по бригаде → итог смены, фиксируем в леджере на следующую смену
	ShiftNet = 0;
	for (const FPlayerRunStats& S : PlayerStats)
	{
		ShiftNet += ComputePlayerBalance(S);
	}
	const bool bWon = NewPhase == ERunPhase::Won;
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger = GI->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			Ledger->CommitShift(ShiftNet, bWon);
		}
	}

	DispatcherSay(bWon ? DispatcherLines::Victory : DispatcherLines::Defeat,
		FString(), /*bImportant=*/true);
}

int32 ARunState::ComputePlayerBalance(const FPlayerRunStats& S)
{
	return S.Repairs * 1500 + S.Revives * 1000 + S.Drags * 500 + S.ToiletVisits * 300
		- S.TimesWounded * 1000 - S.Incidents * 2000 - S.BotchedRepairs * 800
		- FMath::RoundToInt(S.PanicSeconds) * 10;
}

FString ARunState::ReputationTitle(int32 Points)
{
	if (Points >= 6)  return TEXT("Контора на хорошем счету");
	if (Points >= 2)  return TEXT("Репутация так себе, но берут");
	if (Points >= -1) return TEXT("Серая контора без лица");
	if (Points >= -5) return TEXT("Жалоб больше, чем заявок");
	return TEXT("На грани отзыва лицензии");
}

// ---------- Диспетчер ----------

void ARunState::SendGreeting()
{
	DispatcherSay(DispatcherLines::Greeting, FString::FromInt(Objectives.Num()), /*bImportant=*/true);
	if (bCheapGear)
	{
		DispatcherSay(DispatcherLines::CheapGearGreeting, FString(), /*bImportant=*/true);
	}
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

void ARunState::TickRadioInterference(float Now)
{
	// Ищем включённую рацию: чужой голос лезет только в живой эфир
	APickupItem* ActiveRadio = nullptr;
	for (TActorIterator<APickupItem> It(GetWorld()); It; ++It)
	{
		if (It->ItemEffect == EItemEffect::Radio && It->IsToggledOn())
		{
			ActiveRadio = *It;
			break;
		}
	}

	if (!ActiveRadio)
	{
		NextRadioGhostTime = 0.f; // тумблер выключен — таймер сбрасываем
		return;
	}

	// Рацию только что включили — заводим отсчёт (дешёвая ловит чужой голос заметно чаще)
	if (NextRadioGhostTime <= 0.f)
	{
		NextRadioGhostTime = Now + (bCheapGear ? FMath::FRandRange(12.f, 25.f) : FMath::FRandRange(25.f, 50.f));
		return;
	}
	if (Now < NextRadioGhostTime)
	{
		return;
	}
	NextRadioGhostTime = Now + (bCheapGear ? FMath::FRandRange(18.f, 35.f) : FMath::FRandRange(35.f, 70.f));

	// Чужой голос прорывается в эфир — важная реплика, анти-спам её не глотает
	DispatcherSay(DispatcherLines::RadioGhost, FString(), /*bImportant=*/true, TEXT("···помехи···"));

	// Эфир шипит на всю округу — задел под монстра-слухача
	ActiveRadio->MakeNoise(0.9f, Cast<APawn>(ActiveRadio->GetOwner()), ActiveRadio->GetActorLocation());

	// Кто рядом с рацией — у того по спине пробегает холодок
	const FVector RadioLoc = ActiveRadio->GetActorLocation();
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (UVitalsComponent* Vitals = It->VitalsComponent)
		{
			if (FVector::Dist(It->GetActorLocation(), RadioLoc) <= 900.f)
			{
				Vitals->AddPanic(8.f);
			}
		}
	}
}

void ARunState::ApplyCheapGear(AAvaryoCharacter* Who)
{
	if (!bCheapGear || !Who || CheapGearApplied.Contains(Who))
	{
		return;
	}
	CheapGearApplied.Add(Who);
	if (UFlashlightComponent* Flashlight = Who->FlashlightComponent)
	{
		Flashlight->SetCheapUnit(true); // дешёвый фонарь будет моргать и при полном заряде
	}
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
