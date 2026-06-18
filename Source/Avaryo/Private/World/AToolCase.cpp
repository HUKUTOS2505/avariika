#include "World/AToolCase.h"

#include "AvaryoCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"
#include "Engine/GameInstance.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Game/ARunState.h"
#include "Game/DispatchSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

AToolCase::AToolCase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	SetRootComponent(Zone);
	Zone->SetBoxExtent(FVector(120.f, 120.f, 120.f));
	Zone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Zone->SetCollisionResponseToAllChannels(ECR_Overlap);

	Case = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Case"));
	Case->SetupAttachment(Zone);
	Case->SetRelativeScale3D(FVector(0.8f, 0.5f, 0.35f)); // ~ящик 0.8×0.5×0.35 м
	Case->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
	Case->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Case->SetCollisionResponseToAllChannels(ECR_Block); // прицельный свип E попадает
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded()) { Case->SetStaticMesh(CubeMesh.Object); }

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(Zone);
	Label->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetWorldSize(14.f);
	Label->SetTextRenderColor(FColor(255, 140, 0));
	static ConstructorHelpers::FObjectFinder<UFont> CyrFont(TEXT("/Engine/EngineFonts/Roboto.Roboto"));
	if (CyrFont.Succeeded()) { Label->SetFont(CyrFont.Object); } // рантайм-шрифт с кириллицей

	static ConstructorHelpers::FObjectFinder<USoundBase> Snd(TEXT("/Game/Audio/SFX/FlashClick.FlashClick"));
	if (Snd.Succeeded()) { LoadSound = Snd.Object; }
}

void AToolCase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AToolCase, bLoaded);
}

void AToolCase::OnRep_Loaded()
{
	RefreshLabel();
}

void AToolCase::BeginPlay()
{
	Super::BeginPlay();
	// собранность сбрасывается на каждую базу — читаем из диспетч-подсистемы
	if (const UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (const UDispatchSubsystem* D = GI->GetSubsystem<UDispatchSubsystem>())
		{
			bLoaded = D->IsKitLoaded();
		}
	}
	RefreshLabel();
}

void AToolCase::RefreshLabel()
{
	if (Label)
	{
		Label->SetText(bLoaded
			? NSLOCTEXT("ToolCase", "Done", "ЯЩИК СОБРАН ✓")
			: NSLOCTEXT("ToolCase", "Take", "ЯЩИК ИНСТРУМЕНТА\n[E] собрать кит"));
	}
}

void AToolCase::UseBy(AAvaryoCharacter* Who)
{
	if (!HasAuthority() || bLoaded)
	{
		return;
	}
	UWorld* W = GetWorld();
	if (!W)
	{
		return;
	}
	bLoaded = true;
	if (UGameInstance* GI = W->GetGameInstance())
	{
		if (UDispatchSubsystem* D = GI->GetSubsystem<UDispatchSubsystem>())
		{
			D->SetKitLoaded(true);
		}
	}
	if (LoadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(W, LoadSound, GetActorLocation());
	}
	if (ARunState* Run = ARunState::Get(W))
	{
		Run->AnnounceKitLoaded();
	}
	RefreshLabel();
}
