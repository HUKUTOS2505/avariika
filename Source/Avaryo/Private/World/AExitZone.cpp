#include "World/AExitZone.h"

#include "AvaryoCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

AExitZone::AExitZone()
{
	PrimaryActorTick.bCanEverTick = true; // периодическая проверка "вся команда внутри"
	PrimaryActorTick.TickInterval = 0.5f;
	bReplicates = true;

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	SetRootComponent(Zone);
	Zone->SetBoxExtent(FVector(300.f, 300.f, 150.f));
	Zone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Zone->SetCollisionResponseToAllChannels(ECR_Overlap);

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(Zone);
	Label->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetWorldSize(40.f);
	Label->SetText(NSLOCTEXT("ExitZone", "Label", "ГАЗель\n(сначала почините всё)"));
	Label->SetTextRenderColor(FColor(255, 140, 0)); // оранжевый акцент проекта
	static ConstructorHelpers::FObjectFinder<UFont> CyrFont(TEXT("/Engine/EngineFonts/Roboto.Roboto"));
	if (CyrFont.Succeeded()) { Label->SetFont(CyrFont.Object); } // рантайм-шрифт с кириллицей

	// Маяк готовности (Movable — работает без билда света)
	Beacon = CreateDefaultSubobject<UPointLightComponent>(TEXT("Beacon"));
	Beacon->SetupAttachment(Zone);
	Beacon->SetRelativeLocation(FVector(0.f, 0.f, 260.f));
	Beacon->SetMobility(EComponentMobility::Movable);
	Beacon->SetAttenuationRadius(900.f);
	Beacon->SetLightColor(FLinearColor(1.f, 0.3f, 0.1f)); // тускло-оранжевый, пока не готово
	Beacon->SetIntensity(300.f);

	static ConstructorHelpers::FObjectFinder<USoundBase> Snd(TEXT("/Game/Audio/SFX/RepairDone.RepairDone"));
	if (Snd.Succeeded()) { ReadySound = Snd.Object; }

	bTeamInside = false;
	bReadyToLeave = false;
}

void AExitZone::BeginPlay()
{
	Super::BeginPlay();
	RefreshBeacon(); // начальный вид (ещё не готово)
}

void AExitZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AExitZone, bTeamInside);
	DOREPLIFETIME(AExitZone, bReadyToLeave);
}

void AExitZone::RefreshBeacon()
{
	if (Beacon)
	{
		Beacon->SetLightColor(bReadyToLeave ? FLinearColor(0.1f, 1.f, 0.2f) : FLinearColor(1.f, 0.3f, 0.1f));
		Beacon->SetIntensity(bReadyToLeave ? 6000.f : 300.f);
	}
	if (Label)
	{
		Label->SetText(bReadyToLeave
			? NSLOCTEXT("ExitZone", "Ready", "✓ ВСЁ ПОЧИНЕНО\nК ГАЗели — на базу")
			: NSLOCTEXT("ExitZone", "NotReady", "ГАЗель\n(сначала почините всё)"));
		Label->SetTextRenderColor(bReadyToLeave ? FColor(60, 255, 90) : FColor(255, 140, 0));
	}
}

void AExitZone::OnRep_Ready()
{
	RefreshBeacon();
	if (bReadyToLeave && ReadySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReadySound, GetActorLocation());
	}
}

void AExitZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	// Вся команда в зоне? Раненых тоже считаем — их надо дотащить
	int32 NumPlayers = 0;
	bool bAllInside = true;
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		++NumPlayers;
		if (!IsOverlappingActor(*It))
		{
			bAllInside = false;
			break;
		}
	}
	bTeamInside = NumPlayers > 0 && bAllInside;

	ARunState* RunState = ARunState::Get(GetWorld());

	// Готовность к возврату на базу: всё починено → маяк зелёный + звук (один раз)
	const bool bAllDone = RunState && RunState->GetTotalObjectives() > 0 && RunState->AreAllObjectivesComplete();
	if (bAllDone != bReadyToLeave)
	{
		bReadyToLeave = bAllDone;
		RefreshBeacon(); // визуал на сервере (хост)
		if (bReadyToLeave && ReadySound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ReadySound, GetActorLocation());
		}
	}

	if (bTeamInside && RunState)
	{
		RunState->NotifyTeamAtExit();
	}
}
