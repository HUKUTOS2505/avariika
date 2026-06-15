#include "World/ACallBoard.h"

#include "AvaryoCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/Level.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Game/ARunState.h"
#include "Game/DispatchSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Package.h"

ACallBoard::ACallBoard()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	SetRootComponent(Zone);
	Zone->SetBoxExtent(FVector(160.f, 160.f, 160.f));
	Zone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Zone->SetCollisionResponseToAllChannels(ECR_Overlap);

	Board = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Board"));
	Board->SetupAttachment(Zone);
	// Плоская «доска объявлений» ~1.6×0.1×1.0 м из движкового куба (меш заменит сетап-скрипт/арт)
	Board->SetRelativeScale3D(FVector(1.6f, 0.1f, 1.0f));
	Board->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	Board->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Board->SetCollisionResponseToAllChannels(ECR_Block); // чтобы прицельный свип E попадал по доске
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Board->SetStaticMesh(CubeMesh.Object);
	}

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(Zone);
	Label->SetRelativeLocation(FVector(0.f, -12.f, 200.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetWorldSize(18.f);
	Label->SetText(NSLOCTEXT("CallBoard", "Label", "ДОСКА ЗАЯВОК"));
	Label->SetTextRenderColor(FColor(255, 140, 0)); // оранжевый акцент проекта

	// Звуки по умолчанию (переопределяемы на инстансе/в Blueprint)
	static ConstructorHelpers::FObjectFinder<USoundBase> AcceptSnd(TEXT("/Game/Audio/SFX/RadioComm.RadioComm"));
	if (AcceptSnd.Succeeded()) { AcceptSound = AcceptSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> EngineSnd(TEXT("/Game/Audio/SFX/EngineStart.EngineStart"));
	if (EngineSnd.Succeeded()) { EngineStartSound = EngineSnd.Object; }
}

void ACallBoard::BeginPlay()
{
	Super::BeginPlay();

	// Дефолтные заявки, если на инстансе ничего не задано (Дом готов; Завод/Больница — «скоро»).
	if (Calls.Num() == 0)
	{
		FCallListing Dom;
		Dom.Id = TEXT("Dom");
		Dom.Title = TEXT("Дом — частный сектор");
		Dom.Brief = TEXT("Бытовая авария: электрика, газ.");
		// ВРЕМЕННО: едем на рабочую Lvl_FirstPerson — там весь геймплей (поломки, зона
		// выхода, предметы). Переключить на /Game/Avariika/Maps/L_Dom, когда дом-карта
		// будет наполнена ремонтируемыми + зоной выхода.
		Dom.ObjectMap = TEXT("/Game/FirstPerson/Lvl_FirstPerson");
		Dom.bAvailable = true;
		Calls.Add(Dom);

		FCallListing Zavod;
		Zavod.Id = TEXT("Zavod");
		Zavod.Title = TEXT("Завод — цех (скоро)");
		Zavod.Brief = TEXT("Промышленные системы, опасные зоны.");
		Zavod.ObjectMap = TEXT("/Game/Avariika/Maps/L_Zavod");
		Zavod.bAvailable = false;
		Calls.Add(Zavod);

		FCallListing Bol;
		Bol.Id = TEXT("Bolnitsa");
		Bol.Title = TEXT("Больница (скоро)");
		Bol.Brief = TEXT("Людная тревожная среда.");
		Bol.ObjectMap = TEXT("/Game/Avariika/Maps/L_Bolnitsa");
		Bol.bAvailable = false;
		Calls.Add(Bol);
	}

	// Выбрать первую доступную заявку
	const int32 Avail = FirstAvailable();
	SelectedIndex = (Avail != INDEX_NONE) ? Avail : 0;

	RefreshLabel();
}

int32 ACallBoard::FirstAvailable() const
{
	for (int32 i = 0; i < Calls.Num(); ++i)
	{
		if (Calls[i].bAvailable && !Calls[i].ObjectMap.IsEmpty())
		{
			return i;
		}
	}
	return INDEX_NONE;
}

void ACallBoard::RefreshLabel()
{
	if (!Label)
	{
		return;
	}
	FString Text = TEXT("ДОСКА ЗАЯВОК\n");
	for (int32 i = 0; i < Calls.Num(); ++i)
	{
		const FCallListing& C = Calls[i];
		const TCHAR* Mark = C.bAvailable ? TEXT("> ") : TEXT("  ");
		Text += FString::Printf(TEXT("%s%s%s\n"), Mark, *C.Title, C.bAvailable ? TEXT("") : TEXT(""));
	}
	Text += TEXT("\n[E] — взять заявку");
	Label->SetText(FText::FromString(Text));
}

void ACallBoard::AcceptBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || bTraveling)
	{
		return;
	}
	UWorld* W = GetWorld();
	if (!W)
	{
		return;
	}

	int32 Idx = (Calls.IsValidIndex(SelectedIndex) && Calls[SelectedIndex].bAvailable)
		? SelectedIndex
		: FirstAvailable();
	if (!Calls.IsValidIndex(Idx))
	{
		return;
	}
	const FCallListing Call = Calls[Idx]; // копия — переживёт таймер до выезда
	if (Call.ObjectMap.IsEmpty() || !Call.bAvailable)
	{
		return;
	}

	// Запомнить хаб (куда вернуться) + заявку в GameInstance — переживёт ServerTravel
	FString HubMap = HubMapOverride;
	if (HubMap.IsEmpty() && W->PersistentLevel)
	{
		HubMap = W->PersistentLevel->GetOutermost()->GetName();
	}
	bool bKitLoaded = false;
	if (UGameInstance* GI = W->GetGameInstance())
	{
		if (UDispatchSubsystem* D = GI->GetSubsystem<UDispatchSubsystem>())
		{
			bKitLoaded = D->IsKitLoaded(); // собрали ли ящик перед выездом
			D->BeginJob(HubMap, Call.Id, Call.Title);
		}
	}

	// Диспетчер на базе подтверждает заявку (если RunState поднят) и ребёт за забытый ящик
	if (ARunState* Run = ARunState::Get(W))
	{
		Run->AnnounceCallAccepted(Call.Title, bKitLoaded);
	}

	if (AcceptSound)
	{
		UGameplayStatics::PlaySound2D(W, AcceptSound);
	}
	if (EngineStartSound)
	{
		// двигатель «поехали» — от места доски/гаража (3D)
		UGameplayStatics::PlaySoundAtLocation(W, EngineStartSound, GetActorLocation());
	}

	// Небольшая пауза на брифинг, затем выезд
	PendingTravelURL = Call.ObjectMap + TEXT("?listen");
	bTraveling = true;
	W->GetTimerManager().SetTimer(TravelTimer, this, &ACallBoard::DoTravel, FMath::Max(TravelDelay, 0.1f), false);
}

void ACallBoard::DoTravel()
{
	if (HasAuthority() && GetWorld() && !PendingTravelURL.IsEmpty())
	{
		GetWorld()->ServerTravel(PendingTravelURL);
	}
}
