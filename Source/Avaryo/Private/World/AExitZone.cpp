#include "World/AExitZone.h"

#include "AvaryoCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "Net/UnrealNetwork.h"

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
	Label->SetText(NSLOCTEXT("ExitZone", "Label", "ГАЗель — выход"));
	Label->SetTextRenderColor(FColor(255, 140, 0)); // оранжевый акцент проекта

	bTeamInside = false;
}

void AExitZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AExitZone, bTeamInside);
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

	if (bTeamInside)
	{
		if (ARunState* RunState = ARunState::Get(GetWorld()))
		{
			RunState->NotifyTeamAtExit();
		}
	}
}
