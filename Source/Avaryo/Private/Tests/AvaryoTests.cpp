#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AvaryoCharacter.h"
#include "Components/VitalsComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Game/ARunState.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/Package.h"
#include "World/AFloodlight.h"
#include "World/ARepairable.h"
#include "World/ATrap.h"

// Формула премий/штрафов «Акта» (единая для сервера и HUD)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAvaryoBalanceTest, "Avariika.PlayerBalance",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FAvaryoBalanceTest::RunTest(const FString&)
{
	{
		FPlayerRunStats S;
		TestEqual(TEXT("пустой монтёр = 0 ₽"), ARunState::ComputePlayerBalance(S), 0);
	}
	{
		FPlayerRunStats S; S.Repairs = 2;
		TestEqual(TEXT("2 починки = 3000 ₽"), ARunState::ComputePlayerBalance(S), 3000);
	}
	{
		// 1500 +1000 +500 +300 -1000 -2000 -800 -100 = -600
		FPlayerRunStats S;
		S.Repairs = 1; S.Revives = 1; S.Drags = 1; S.ToiletVisits = 1;
		S.TimesWounded = 1; S.Incidents = 1; S.BotchedRepairs = 1; S.PanicSeconds = 10.f;
		TestEqual(TEXT("смешанная смена = -600 ₽"), ARunState::ComputePlayerBalance(S), -600);
	}
	{
		FPlayerRunStats S; S.BotchedRepairs = 3; // штраф 800 за колхоз
		TestEqual(TEXT("3 колхоза = -2400 ₽"), ARunState::ComputePlayerBalance(S), -2400);
	}
	return true;
}

// Названия репутационных статусов по очкам
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAvaryoReputationTitleTest, "Avariika.ReputationTitle",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FAvaryoReputationTitleTest::RunTest(const FString&)
{
	TestEqual(TEXT("+10"), ARunState::ReputationTitle(10), FString(TEXT("Контора на хорошем счету")));
	TestEqual(TEXT("+3"),  ARunState::ReputationTitle(3),  FString(TEXT("Репутация так себе, но берут")));
	TestEqual(TEXT("0"),   ARunState::ReputationTitle(0),  FString(TEXT("Серая контора без лица")));
	TestEqual(TEXT("-3"),  ARunState::ReputationTitle(-3), FString(TEXT("Жалоб больше, чем заявок")));
	TestEqual(TEXT("-8"),  ARunState::ReputationTitle(-8), FString(TEXT("На грани отзыва лицензии")));
	return true;
}

// Бухгалтерия конторы: накопление баланса, репутация, номер смены
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAvaryoLedgerTest, "Avariika.CompanyLedger",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FAvaryoLedgerTest::RunTest(const FString&)
{
	// Сабсистема ClassWithin=GameInstance — нужен GameInstance-овнер
	UGameInstance* GI = NewObject<UGameInstance>(GetTransientPackage());
	UCompanyLedgerSubsystem* Ledger = NewObject<UCompanyLedgerSubsystem>(GI);
	if (!TestNotNull(TEXT("леджер создан"), Ledger))
	{
		return false;
	}

	TestEqual(TEXT("старт: баланс 0"), Ledger->GetBalance(), 0);
	TestEqual(TEXT("старт: смена 1"), Ledger->GetShiftNumber(), 1);
	TestEqual(TEXT("старт: репутация 0"), Ledger->GetReputation(), 0);

	// Победа в плюс: +5000, репутация +1, смена → 2
	Ledger->CommitShift(5000, /*bWon=*/true);
	TestEqual(TEXT("после победы: баланс 5000"), Ledger->GetBalance(), 5000);
	TestEqual(TEXT("после победы: репутация 1"), Ledger->GetReputation(), 1);
	TestEqual(TEXT("после победы: смена 2"), Ledger->GetShiftNumber(), 2);

	// Провал в минус: -3000, репутация -2 (провал) -1 (минус) = было 1 → -2
	Ledger->CommitShift(-3000, /*bWon=*/false);
	TestEqual(TEXT("после провала: баланс 2000"), Ledger->GetBalance(), 2000);
	TestEqual(TEXT("после провала: репутация -2"), Ledger->GetReputation(), -2);
	TestEqual(TEXT("после провала: смена 3"), Ledger->GetShiftNumber(), 3);

	// Репутация зажата сверху +10
	for (int32 i = 0; i < 30; ++i) { Ledger->CommitShift(100, true); }
	TestEqual(TEXT("репутация не выше +10"), Ledger->GetReputation(), 10);

	// ...и снизу -10
	for (int32 i = 0; i < 30; ++i) { Ledger->CommitShift(-100, false); }
	TestEqual(TEXT("репутация не ниже -10"), Ledger->GetReputation(), -10);

	return true;
}

// Шкалы Vitals: зажимы и сеттеры
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAvaryoVitalsTest, "Avariika.Vitals",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FAvaryoVitalsTest::RunTest(const FString&)
{
	UVitalsComponent* V = NewObject<UVitalsComponent>(GetTransientPackage());
	if (!TestNotNull(TEXT("Vitals создан"), V))
	{
		return false;
	}

	// Запах: зажим 0..100 и порог «воняет»
	TestFalse(TEXT("на старте не воняет"), V->IsSmelly());
	V->AddSmell(120.f);
	TestEqual(TEXT("запах зажат сверху 100"), V->GetSmell(), 100.f);
	TestTrue(TEXT("при 100 — воняет"), V->IsSmelly());
	V->AddSmell(-200.f);
	TestEqual(TEXT("запах зажат снизу 0"), V->GetSmell(), 0.f);
	TestFalse(TEXT("при 0 — не воняет"), V->IsSmelly());

	// Паника: зажим
	V->AddPanic(250.f);
	TestEqual(TEXT("паника зажата 100"), V->GetPanic(), 100.f);
	V->ReducePanic(40.f);
	TestEqual(TEXT("паника 60 после -40"), V->GetPanic(), 60.f);

	// Дев-сеттер
	V->DebugSetVital(TEXT("smell"), 75.f);
	TestEqual(TEXT("DebugSetVital smell=75"), V->GetSmell(), 75.f);
	V->DebugSetVital(TEXT("stamina"), 10.f);
	TestEqual(TEXT("DebugSetVital stamina=10"), V->GetStamina(), 10.f);

	return true;
}

// --- Рантайм-акторы в живом тест-мире (то, что не покрыть смоуком) ---

static UWorld* AvaryoMakeTestWorld()
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld=*/false);
	FWorldContext& Ctx = GEngine->CreateNewWorldContext(EWorldType::Game);
	Ctx.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();
	return World;
}

static void AvaryoTickWorld(UWorld* World, float Seconds)
{
	const float Step = 0.05f;
	for (float t = 0.f; t < Seconds; t += Step)
	{
		World->Tick(LEVELTICK_All, Step);
	}
}

static void AvaryoDestroyTestWorld(UWorld* World)
{
	GEngine->DestroyWorldContext(World);
	World->DestroyWorld(false);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAvaryoDeployablesTest, "Avariika.Deployables",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FAvaryoDeployablesTest::RunTest(const FString&)
{
	UWorld* World = AvaryoMakeTestWorld();
	if (!TestNotNull(TEXT("тест-мир создан"), World))
	{
		return false;
	}

	FActorSpawnParameters Always;
	Always.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAvaryoCharacter* Char = World->SpawnActor<AAvaryoCharacter>(AAvaryoCharacter::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Always);
	if (!TestNotNull(TEXT("монтёр заспавнен"), Char) || !TestNotNull(TEXT("есть Vitals"), Char ? (UObject*)Char->VitalsComponent : nullptr))
	{
		AvaryoDestroyTestWorld(World);
		return false;
	}
	Char->SetActorTickEnabled(false);
	if (Char->GetCharacterMovement())
	{
		Char->GetCharacterMovement()->SetMovementMode(MOVE_None);
	}

	// Ставимые акторы должны безопасно спавниться, взводиться и тикать в живом мире (без падений).
	// Поведение их Tick (гашение паники / срабатывание) проверяется в PIE — в ручном World->Tick
	// хедлесс-мира тики акторов не диспатчатся надёжно.
	AFloodlight* Lamp = World->SpawnActor<AFloodlight>(AFloodlight::StaticClass(), FVector(50.f, 0.f, 0.f), FRotator::ZeroRotator, Always);
	TestNotNull(TEXT("прожектор заспавнен"), Lamp);

	ATrap* Trap = World->SpawnActorDeferred<ATrap>(ATrap::StaticClass(), FTransform(FVector(60.f, 0.f, 0.f)),
		nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	TestNotNull(TEXT("растяжка заспавнена"), Trap);
	if (Trap)
	{
		Trap->ArmDelay = 0.1f;
		Trap->PlacerGraceTime = 0.f;
		Trap->FinishSpawning(FTransform(FVector(60.f, 0.f, 0.f)));
	}

	AvaryoTickWorld(World, 0.6f); // не должно упасть на BeginPlay/Tick/таймере взвода

	// --- Колхоз: ремонт инструментального объекта без инструмента (прямые вызовы, без зависимости от Tick) ---
	ARepairable* Rep = World->SpawnActor<ARepairable>(ARepairable::StaticClass(), FVector(100.f, 0.f, 0.f), FRotator::ZeroRotator, Always);
	if (TestNotNull(TEXT("объект заспавнен"), Rep))
	{
		Rep->RequiredTool = TEXT("Welder");
		Rep->bAllowBotch = true;
		TestTrue(TEXT("без инструмента можно колхозить"), Rep->CanBotchBy(Char));
		TestTrue(TEXT("починка стартует колхозом"), Rep->BeginRepairBy(Char));
		TestTrue(TEXT("идёт колхоз"), Rep->IsBotching());

		// С правильным инструментом колхоз НЕ предлагается (нужна обычная мини-игра)
		Rep->EndRepairBy(Char);
		Rep->bAllowBotch = false;
		TestFalse(TEXT("при bAllowBotch=false колхоз запрещён"), Rep->CanBotchBy(Char));
	}

	AvaryoDestroyTestWorld(World);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
