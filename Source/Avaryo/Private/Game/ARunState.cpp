#include "Game/ARunState.h"

#include "AvaryoCharacter.h"
#include "Components/UFlashlightComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "Game/DispatchSubsystem.h"
#include "World/ACallBoard.h"
#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Items/ABioPickup.h"
#include "Items/APickupItem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
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
	// «Диспетчер помнит» — реплики на основе карьеры конторы (переживает выход из игры)
	const TArray<FString> MemoryBlewUp = {
		TEXT("О, опять вы. На вашем счету уже {X} спалённых объектов. Сегодня — хотя бы без пожара?"),
		TEXT("Бригада-поджигатели снова в деле. {X} зданий на вашей совести. Зажигалки оставьте дома."),
		TEXT("Напоминаю: вы взрывали объекты {X} раз. Страховая нас уже по имени знает."),
	};
	const TArray<FString> MemoryLoser = {
		TEXT("Честно? После ваших прошлых смен я заявку давал с неохотой. Докажите, что зря."),
		TEXT("Контора на грани. Ещё один провал — и пишем заявления. Без давления, работайте."),
	};
	const TArray<FString> MemoryVeteran = {
		TEXT("А, ветераны. Заявки вы закрываете — за это держим. Не зазнавайтесь."),
		TEXT("Опытная бригада на смене. Жильцы попросили именно вас. Не подведите."),
	};
	// ХАБ: приветствие на базе (диспетчер ждёт, пока бригада возьмёт заявку с доски)
	const TArray<FString> HubWelcome = {
		TEXT("Бригада на базе. На доске висят заявки — берите и выезжайте, смена не резиновая."),
		TEXT("Утро, мужики. Кофе допили — к доске заявок, жильцы ждать не любят."),
		TEXT("Диспетчерская — бригаде: заявки на стене. Выбирайте объект и в ГАЗель."),
	};
	// ХАБ: подтверждение принятой заявки («{X}» — заголовок заявки)
	const TArray<FString> CallBriefing = {
		TEXT("Принял. Выезд на объект: {X}. По коням, фонари проверьте."),
		TEXT("Заявка «{X}» ваша. Грузимся, выезжаем. Не растеряйте инструмент."),
		TEXT("Есть «{X}». Адрес в навигаторе, погнали. Жду с победой."),
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
	// Биологический снаряд (§15): прямое попадание в своего / падение мимо
	const TArray<FString> BioHit = {
		TEXT("{X} поймал биологический снаряд лицом. Бригада, я в восторге от уровня."),
		TEXT("Прямое попадание в {X}. Это, конечно, в акт. С фотографией."),
		TEXT("{X}, тебя только что... кхм. Соболезную. И штрафую — кого-нибудь."),
	};
	const TArray<FString> BioMiss = {
		TEXT("Что-то шлёпнулось в темноте. Надеюсь, это была не еда."),
		TEXT("Промах. Зато теперь там... ароматно. Монстр оценит."),
		TEXT("Снаряд ушёл в молоко. Санитарной службе привет."),
	};
	// Амбре: монтёр провонял (§16 «запах»)
	const TArray<FString> SmellJab = {
		TEXT("{X}, от тебя за версту несёт. Открой хоть окно... а, тут нет окон. Терпите, мужики."),
		TEXT("Кто там надушился? {X}, это уже не амбре, это химоружие."),
		TEXT("{X}, после смены — в душ. Приказ. Бригада задыхается."),
	};
	// Сработавшая растяжка (§18 «предметы-ловушки» — часто бьёт по своим)
	const TArray<FString> TrapTriggered = {
		TEXT("Это что за грохот?! {X}, ты что, в свою же растяжку влетел?"),
		TEXT("Растяжка сработала. {X}, ловушки — для монстра, а не для бригады."),
		TEXT("Бах! {X}, надеюсь, хоть штаны сухие. Хотя у нас и на это есть графа."),
		TEXT("Шумелка отработала на отлично. Вся карта в курсе, где вы. Молодцы."),
	};
	// Колхозный ремонт без инструмента (§18 «не тот инструмент»)
	const TArray<FString> BotchRepair = {
		TEXT("{X}, это не ремонт, это художественная самодеятельность. Но раз держится — молчу."),
		TEXT("Чем ты это чинил, {X}, коленкой? В акте так и напишу: «восстановлено народными методами»."),
		TEXT("{X} закрыл объект без инструмента. Гарантия — до выхода из подъезда."),
		TEXT("Принято от {X}. Колхоз, но работает. Премию урежу за стиль."),
	};
	// Скользкая пена (§18): кто-то поехал по разлитой пене
	const TArray<FString> SlipFoam = {
		TEXT("{X} поехал по пене. Это огнетушитель, а не каток, мужики."),
		TEXT("Опять кто-то катается. Кто залил пол — с того и химчистка."),
		TEXT("{X} красиво проскользил. На акт не тянет, но я заценил."),
	};
	// Толчок (§18 кооп-хаос): прилетело от своих
	const TArray<FString> Shoved = {
		TEXT("Кто там толкается?! Бригада, мы вроде одна команда."),
		TEXT("{X} словил плечо от своих же. Запишу как «производственная гимнастика»."),
		TEXT("Толкотня на смене. {X}, держись от коллег подальше — целее будешь."),
	};
	// Споткнулся (§18): навернулся на бегу
	const TArray<FString> Tripped = {
		TEXT("{X}, под ноги смотри! Хотя тут темно, как у меня в премии."),
		TEXT("Кто-то навернулся. Пол подлый, не спорю."),
		TEXT("{X} собрал все провода разом. Изящно, но в акт."),
	};
	// Фоновая жуть: скрипы старого здания (без монстра — тревожность звуком/репликой)
	const TArray<FString> Creak = {
		TEXT("...слышу скрип где-то наверху. Показалось, наверное."),
		TEXT("Кто-то ходит? ...Нет? Ну ладно. Здание старое, оседает."),
		TEXT("Опять стукнуло в стене. Мужики, это трубы. Точно трубы."),
		TEXT("Чувствуете? ...Нет, ничего. Работаем, работаем."),
	};
	// Метнул предмет (§18)
	const TArray<FString> ThrowJab = {
		TEXT("{X} что-то метнул. Бейсбол на смене — новаторски, в акт."),
		TEXT("Летающий инструмент зафиксирован. Технику безопасности оформлю отдельно."),
		TEXT("{X}, кидаться — это не передача по регламенту. Но красиво."),
	};
	// Кофе-брейк (термос)
	const TArray<FString> CoffeeBreak = {
		TEXT("{X} на кофе. Правильно, на ногах держаться надо."),
		TEXT("Кофеёк? Мне б тоже, но диспетчерам нельзя. Завидую, {X}."),
		TEXT("{X} заправляется. Только баллон с кофе не перепутай с огнетушителем."),
	};
	// Перегрузка сети (§18): старое здание снова выбило щиток
	const TArray<FString> Overload = {
		TEXT("Опять выбило! Старая проводка, мужики. Кто-то снова к щитку."),
		TEXT("Свет моргнул — перегрузка. Щиток обесточился, чините заново."),
		TEXT("Здание девятьсот лохматого года: автомат выбило сам по себе. Бегом к ГРЩ."),
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
	bElectricalOverload = true;
	OverloadChancePerSecond = 0.015f; // ~раз в минуту, пока щиток под напряжением
	OverloadCooldown = 0.f;
	NextCreakTime = 0.f;
	CreakIntervalMin = 22.f;
	CreakIntervalMax = 50.f;
	ShiftNumber = 1;
	CompanyBalanceStart = 0;
	Reputation = 0;
	ShiftNet = 0;

	// Звуки по умолчанию (переопределяемы в Blueprint)
	// Диспетчер: «эфир рации» вместо прежнего глитч-стэтика (тот не подошёл)
	static ConstructorHelpers::FObjectFinder<USoundBase> RadioSnd(TEXT("/Game/Audio/SFX/RadioComm.RadioComm"));
	if (RadioSnd.Succeeded()) { RadioBlipSound = RadioSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> AmbSnd(TEXT("/Game/Audio/SFX/Ambient_Boiler.Ambient_Boiler"));
	if (AmbSnd.Succeeded()) { AmbientLoopSound = AmbSnd.Object; }
}

void ARunState::BeginPlay()
{
	Super::BeginPlay();

	// Фоновый эмбиент — локально на каждой машине (ARunState есть у всех клиентов)
	if (AmbientLoopSound && !AmbientAudio)
	{
		AmbientAudio = UGameplayStatics::SpawnSound2D(this, AmbientLoopSound, 0.6f);
	}

	if (!HasAuthority())
	{
		return;
	}

	// ХАБ? Если на карте есть доска заявок — это база, а не объект: ни поломок,
	// ни победы/поражения. Диспетчер только приветствует, дальше ждём приёма заявки.
	for (TActorIterator<ACallBoard> It(GetWorld()); It; ++It)
	{
		bHubMode = true;
		break;
	}
	if (bHubMode)
	{
		// Вернулись с забега — снять активную заявку (хаб как «дом»)
		if (UGameInstance* GI = GetWorld()->GetGameInstance())
		{
			if (UDispatchSubsystem* D = GI->GetSubsystem<UDispatchSubsystem>())
			{
				D->ClearActiveCall();
			}
		}
		GetWorldTimerManager().SetTimer(GreetingTimer, this, &ARunState::SendGreeting, 4.f, false);
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
			QuotaTarget = Ledger->GetQuotaTarget();
			QuotaPaid = Ledger->GetQuotaPaidSoFar();
			QuotaDeadlineShift = Ledger->GetQuotaDeadlineShift();
			bQuotaFailed = Ledger->IsQuotaFailed();
		}
	}
	RefreshCompanyMirror(); // живой баланс/уровни/карьера → клиентам

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
	DOREPLIFETIME(ARunState, QuotaTarget);
	DOREPLIFETIME(ARunState, QuotaPaid);
	DOREPLIFETIME(ARunState, QuotaDeadlineShift);
	DOREPLIFETIME(ARunState, bQuotaFailed);
	DOREPLIFETIME(ARunState, CompanyBalanceLive);
	DOREPLIFETIME(ARunState, EquipWelder);
	DOREPLIFETIME(ARunState, EquipTester);
	DOREPLIFETIME(ARunState, EquipFlashlight);
	DOREPLIFETIME(ARunState, EquipExtinguisher);
	DOREPLIFETIME(ARunState, EquipRadio);
	DOREPLIFETIME(ARunState, CareerRepairs);
	DOREPLIFETIME(ARunState, CareerBlownUp);
	DOREPLIFETIME(ARunState, CareerIncidents);
}

void ARunState::RefreshCompanyMirror()
{
	if (!HasAuthority())
	{
		return;
	}
	const UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	const UCompanyLedgerSubsystem* Ledger = GI ? GI->GetSubsystem<UCompanyLedgerSubsystem>() : nullptr;
	if (!Ledger)
	{
		return;
	}
	CompanyBalanceLive = Ledger->GetBalance();
	EquipWelder       = Ledger->GetEquipmentLevel(FName(TEXT("Welder")));
	EquipTester       = Ledger->GetEquipmentLevel(FName(TEXT("Tester")));
	EquipFlashlight   = Ledger->GetEquipmentLevel(FName(TEXT("Flashlight")));
	EquipExtinguisher = Ledger->GetEquipmentLevel(FName(TEXT("Extinguisher")));
	EquipRadio        = Ledger->GetEquipmentLevel(FName(TEXT("Radio")));
	const FCareerStats& C = Ledger->GetCareer();
	CareerRepairs   = C.TotalRepairs;
	CareerBlownUp   = C.BuildingsBlownUp;
	CareerIncidents = C.TotalIncidents;
}

int32 ARunState::GetEquipmentLevelRep(FName Tool) const
{
	if (Tool == FName(TEXT("Welder")))       { return EquipWelder; }
	if (Tool == FName(TEXT("Tester")))       { return EquipTester; }
	if (Tool == FName(TEXT("Flashlight")))   { return EquipFlashlight; }
	if (Tool == FName(TEXT("Extinguisher"))) { return EquipExtinguisher; }
	if (Tool == FName(TEXT("Radio")))        { return EquipRadio; }
	return 1;
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

	// ХАБ — не забег: никакой логики поломок/перегрузки/победы (иначе 0 задач = мгновенная «победа»)
	if (bHubMode)
	{
		return;
	}

	RefreshCompanyMirror(); // живой баланс/уровни (магазин/мини-игра у клиента) — дёшево

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

			// Инцидент «произвёл» биологический снаряд — лежит у ног, кто рискнёт — подберёт (§15)
			const FVector BioLoc = It->GetActorLocation() + It->GetActorForwardVector() * 60.f - FVector(0.f, 0.f, 40.f);
			FActorSpawnParameters BioParams;
			BioParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			GetWorld()->SpawnActor<ABioPickup>(ABioPickup::StaticClass(), BioLoc, FRotator::ZeroRotator, BioParams);
		}
		Stats.bWasSoiled = Vitals->IsSoiled();

		if (Vitals->IsSmelly())
		{
			Stats.SmellSeconds += DeltaSeconds;
			if (!Stats.bWasSmelly)
			{
				DispatcherSay(DispatcherLines::SmellJab, CrewName(*It)); // неважная — глотается анти-спамом
			}
		}
		Stats.bWasSmelly = Vitals->IsSmelly();

		if (It->IsSlipping())
		{
			Stats.SlipSeconds += DeltaSeconds; // катается по пене
		}
	}

	TickRadioInterference(Now);
	TickOverload(DeltaSeconds);
	TickAmbient(Now);

	if (NumPlayers > 0 && NumWounded == NumPlayers)
	{
		FinishRun(ERunPhase::Lost);
	}
}

void ARunState::TickOverload(float DeltaSeconds)
{
	if (!bElectricalOverload)
	{
		return;
	}
	if (OverloadCooldown > 0.f)
	{
		OverloadCooldown = FMath::Max(0.f, OverloadCooldown - DeltaSeconds);
		return;
	}

	// Щиток — объект с курсорной мини-игрой; выбиваем только починенный и который сейчас никто не чинит
	ARepairable* Breaker = nullptr;
	for (const TObjectPtr<ARepairable>& Obj : Objectives)
	{
		if (Obj && Obj->GetMinigameType() == ERepairMinigameType::Cursor
			&& !Obj->IsBroken() && !Obj->IsBeingRepaired())
		{
			Breaker = Obj;
			break;
		}
	}
	if (!Breaker)
	{
		return; // щиток сломан/чинится/его нет — перегружать нечего
	}

	if (FMath::FRand() < OverloadChancePerSecond * DeltaSeconds)
	{
		Breaker->SetBroken(true); // снова обесточились (OnRep_Broken оживит аварийные лампы)

		// Пересчитать прогресс задач — иначе победа сработает с выбитым щитком
		RepairedCount = 0;
		for (const ARepairable* Objective : Objectives)
		{
			if (Objective && !Objective->IsBroken())
			{
				++RepairedCount;
			}
		}

		OverloadCooldown = 12.f; // не выбивать снова сразу
		DispatcherSay(DispatcherLines::Overload, FString(), /*bImportant=*/true);
		MakeNoise(0.9f, nullptr, Breaker->GetActorLocation());
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

void ARunState::AddShove(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Who)
	{
		++FindOrAddStats(Who).ShovedOthers;
	}
}

void ARunState::AddTrip(AAvaryoCharacter* Who)
{
	if (HasAuthority() && Who)
	{
		++FindOrAddStats(Who).TimesTripped;
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
		if (Culprit) { ++FindOrAddStats(Culprit).ExplosionsCaused; }
		DispatcherSay(DispatcherLines::GasExplosion, CrewName(Culprit), /*bImportant=*/true);
	}
}

void ARunState::NotifyShortCircuit(AAvaryoCharacter* Culprit)
{
	if (HasAuthority())
	{
		if (Culprit) { ++FindOrAddStats(Culprit).ShortsCaused; }
		DispatcherSay(DispatcherLines::ShortCircuit, CrewName(Culprit), /*bImportant=*/true);
	}
}

void ARunState::NotifyTrapTriggered(AAvaryoCharacter* TriggeredBy)
{
	if (HasAuthority())
	{
		DispatcherSay(DispatcherLines::TrapTriggered, CrewName(TriggeredBy), /*bImportant=*/true);
	}
}

void ARunState::NotifyBioSplat(AAvaryoCharacter* DirectHit)
{
	if (!HasAuthority())
	{
		return;
	}
	if (DirectHit)
	{
		DispatcherSay(DispatcherLines::BioHit, CrewName(DirectHit), /*bImportant=*/true);
	}
	else
	{
		DispatcherSay(DispatcherLines::BioMiss, FString(), /*bImportant=*/false);
	}
}

void ARunState::NotifySlipped(AAvaryoCharacter* Who)
{
	if (HasAuthority())
	{
		DispatcherSay(DispatcherLines::SlipFoam, CrewName(Who), /*bImportant=*/false);
	}
}

void ARunState::NotifyShoved(AAvaryoCharacter* Victim)
{
	if (HasAuthority())
	{
		DispatcherSay(DispatcherLines::Shoved, CrewName(Victim), /*bImportant=*/false);
	}
}

void ARunState::NotifyTripped(AAvaryoCharacter* Who)
{
	if (HasAuthority())
	{
		DispatcherSay(DispatcherLines::Tripped, CrewName(Who), /*bImportant=*/false);
	}
}

void ARunState::NotifyThrow(AAvaryoCharacter* Who)
{
	if (HasAuthority())
	{
		if (Who) { ++FindOrAddStats(Who).Throws; }
		DispatcherSay(DispatcherLines::ThrowJab, CrewName(Who), /*bImportant=*/false);
	}
}

void ARunState::NotifyCoffee(AAvaryoCharacter* Who)
{
	if (HasAuthority())
	{
		if (Who) { ++FindOrAddStats(Who).Coffees; }
		DispatcherSay(DispatcherLines::CoffeeBreak, CrewName(Who), /*bImportant=*/false);
	}
}

void ARunState::TickAmbient(float Now)
{
	if (NextCreakTime <= 0.f)
	{
		NextCreakTime = Now + FMath::FRandRange(CreakIntervalMin, CreakIntervalMax);
		return;
	}
	if (Now < NextCreakTime)
	{
		return;
	}
	NextCreakTime = Now + FMath::FRandRange(CreakIntervalMin, CreakIntervalMax);

	// Скрип/стук где-то рядом со случайным монтёром — шумом seed под монстра + жуткая реплика
	AAvaryoCharacter* Anyone = nullptr;
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		Anyone = *It;
		break;
	}
	if (Anyone)
	{
		const FVector Around = Anyone->GetActorLocation() + FVector(FMath::FRandRange(-600.f, 600.f), FMath::FRandRange(-600.f, 600.f), 0.f);
		MakeNoise(0.4f, nullptr, Around);
	}
	DispatcherSay(DispatcherLines::Creak, FString(), /*bImportant=*/false);
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

	// Командное «уф, починили!» — живая бригада рядом чуть выдыхает (гасит панику в пиковый момент)
	{
		const float ReliefRadiusSq = FMath::Square(1500.f);
		const FVector Where = Repairable ? Repairable->GetActorLocation() : FVector::ZeroVector;
		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			if (It->VitalsComponent && !It->VitalsComponent->IsWounded()
				&& (!Repairable || FVector::DistSquared(It->GetActorLocation(), Where) < ReliefRadiusSq))
			{
				It->VitalsComponent->ReducePanic(8.f);
			}
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
	UGameInstance* GI = GetWorld()->GetGameInstance();

	// Квота провалена — контора закрыта: [R] начинает новую карьеру (сброс леджера)
	if (UCompanyLedgerSubsystem* Ledger = GI ? GI->GetSubsystem<UCompanyLedgerSubsystem>() : nullptr)
	{
		if (Ledger->IsQuotaFailed())
		{
			Ledger->ResetCompany();
		}
	}

	// Выехали с базы по заявке? Тогда после «Акта» возвращаемся в ХАБ к доске.
	// Иначе (тестим карту напрямую, без хаба) — рестарт той же карты, как раньше.
	if (UDispatchSubsystem* D = GI ? GI->GetSubsystem<UDispatchSubsystem>() : nullptr)
	{
		if (D->HasHomeHub())
		{
			const FString Hub = D->GetHomeHubMap();
			D->ClearActiveCall();
			GetWorld()->ServerTravel(Hub + TEXT("?listen"));
			return;
		}
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
	int32 SumRepairs = 0, SumIncidents = 0, SumExplosions = 0;
	for (const FPlayerRunStats& S : PlayerStats)
	{
		ShiftNet += ComputePlayerBalance(S);
		SumRepairs += S.Repairs;
		SumIncidents += S.Incidents;
		SumExplosions += S.ExplosionsCaused;
	}
	const bool bWon = NewPhase == ERunPhase::Won;
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger = GI->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			// Накопить карьеру (переживёт выход — под «диспетчер помнит» / экран карьеры)
			FCareerStats& Career = Ledger->GetCareerMutable();
			Career.TotalRepairs    += SumRepairs;
			Career.TotalIncidents  += SumIncidents;
			Career.BuildingsBlownUp += SumExplosions;
			Ledger->CommitShift(ShiftNet, bWon); // сохранит и карьеру на диск
			// Обновить копии квоты для «Акта» (CommitShift мог продвинуть/провалить её)
			QuotaTarget = Ledger->GetQuotaTarget();
			QuotaPaid = Ledger->GetQuotaPaidSoFar();
			QuotaDeadlineShift = Ledger->GetQuotaDeadlineShift();
			bQuotaFailed = Ledger->IsQuotaFailed();
		}
	}
	RefreshCompanyMirror(); // карьера/баланс/уровни для «Акта» у клиента

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

void ARunState::AnnounceCallAccepted(const FString& CallTitle)
{
	DispatcherSay(DispatcherLines::CallBriefing, CallTitle, /*bImportant=*/true);
}

void ARunState::SendGreeting()
{
	// На базе — своё приветствие, без числа поломок (их тут нет)
	if (bHubMode)
	{
		DispatcherSay(DispatcherLines::HubWelcome, FString(), /*bImportant=*/true);
		return;
	}

	DispatcherSay(DispatcherLines::Greeting, FString::FromInt(Objectives.Num()), /*bImportant=*/true);
	if (bCheapGear)
	{
		DispatcherSay(DispatcherLines::CheapGearGreeting, FString(), /*bImportant=*/true);
	}

	// «Диспетчер помнит»: реплика по карьере конторы (переживает выход из игры)
	if (const UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (const UCompanyLedgerSubsystem* Ledger = GI->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			const FCareerStats& C = Ledger->GetCareer();
			const int32 Played = C.ShiftsWon + C.ShiftsLost;
			if (C.BuildingsBlownUp > 0)
			{
				DispatcherSay(DispatcherLines::MemoryBlewUp, FString::FromInt(C.BuildingsBlownUp), /*bImportant=*/true);
			}
			else if (Played >= 2 && C.ShiftsLost > C.ShiftsWon)
			{
				DispatcherSay(DispatcherLines::MemoryLoser, FString(), /*bImportant=*/true);
			}
			else if (C.ShiftsWon >= 3)
			{
				DispatcherSay(DispatcherLines::MemoryVeteran, FString(), /*bImportant=*/true);
			}
		}
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

void ARunState::DebugForceCheapGear()
{
	if (!HasAuthority())
	{
		return;
	}
	bCheapGear = true;
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		ApplyCheapGear(*It);
	}
}

void ARunState::DebugFinishRun(bool bWon)
{
	if (HasAuthority() && Phase == ERunPhase::InProgress)
	{
		FinishRun(bWon ? ERunPhase::Won : ERunPhase::Lost);
	}
}

void ARunState::DebugSetQuota(int32 Target)
{
	if (!HasAuthority())
	{
		return;
	}
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger = GI->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			if (Target > 0) { Ledger->StartQuota(Target); } else { Ledger->StopQuota(); }
			QuotaTarget = Ledger->GetQuotaTarget();
			QuotaPaid = Ledger->GetQuotaPaidSoFar();
			QuotaDeadlineShift = Ledger->GetQuotaDeadlineShift();
			bQuotaFailed = Ledger->IsQuotaFailed();
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

	// (звук реплики диспетчера убран — раздражал; при желании вернём тихий короткий бип)
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
