#include "World/APowerSwitch.h"

#include "AvaryoCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
#include "World/ARepairable.h"

APowerSwitch::APowerSwitch()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	SetRootComponent(Zone);
	Zone->SetBoxExtent(FVector(110.f, 110.f, 110.f));
	Zone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Zone->SetCollisionResponseToAllChannels(ECR_Overlap);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Zone);
	Body->SetRelativeScale3D(FVector(0.3f, 0.5f, 0.7f)); // щиток-коробка на стене
	Body->SetRelativeLocation(FVector(0.f, 0.f, 35.f));
	Body->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Body->SetCollisionResponseToAllChannels(ECR_Block); // прицельный свип E попадает
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded()) { Body->SetStaticMesh(CubeMesh.Object); }

	StatusLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("StatusLight"));
	StatusLight->SetupAttachment(Body);
	StatusLight->SetRelativeLocation(FVector(40.f, 0.f, 30.f));
	StatusLight->SetIntensity(1500.f);
	StatusLight->SetAttenuationRadius(220.f);
	StatusLight->SetCastShadows(false);

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(Zone);
	Label->SetRelativeLocation(FVector(0.f, 0.f, 95.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetWorldSize(13.f);
	Label->SetTextRenderColor(FColor(255, 140, 0));
	static ConstructorHelpers::FObjectFinder<UFont> CyrFont(TEXT("/Engine/EngineFonts/Roboto.Roboto"));
	if (CyrFont.Succeeded()) { Label->SetFont(CyrFont.Object); } // рантайм-шрифт с кириллицей

	static ConstructorHelpers::FObjectFinder<USoundBase> Snd(TEXT("/Game/Audio/Lib/repair_tool/Ghosthack-H_Impact_Metal_Klonk.Ghosthack-H_Impact_Metal_Klonk"));
	if (Snd.Succeeded()) { ClickSound = Snd.Object; }
}

void APowerSwitch::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APowerSwitch, bPowerOn);
}

void APowerSwitch::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		ApplyToFloods(); // синхронизировать зоны с начальным состоянием питания
	}
	RefreshVisual();
}

void APowerSwitch::ToggleBy(AAvaryoCharacter* /*Who*/)
{
	if (!HasAuthority())
	{
		return;
	}
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (Now - LastToggleTime < 0.4f) { return; } // дебаунс: анти-спам строб света / ре-электрификация воды мешингом E
	LastToggleTime = Now;
	bPowerOn = !bPowerOn;
	ApplyToFloods();
	if (ClickSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ClickSound, GetActorLocation());
	}
	RefreshVisual(); // сервер тоже обновляет свой визуал (клиенты — через OnRep)
}

void APowerSwitch::ApplyToFloods()
{
	// Рубильник снимает/подаёт напряжение на ВСЕ объекты (для слайса 1 — глобально):
	// и электрификацию залитых зон (вода), и «живой провод» (электрика). SetPowered ставит оба флага.
	UWorld* W = GetWorld();
	if (!W)
	{
		return;
	}
	for (TActorIterator<ARepairable> It(W); It; ++It)
	{
		It->SetPowered(bPowerOn);
	}
	// Свет на «домовом» питании (тег "PoweredLight") теперь в RefreshVisual() — чтобы лампы гасли
	// на ВСЕХ машинах (сервер из BeginPlay/ToggleBy, клиенты через OnRep_Power). Раньше было здесь,
	// в server-only ApplyToFloods → у клиентов свет не гас (кооп-десинк механики «свет↔ток»).
}

void APowerSwitch::OnRep_Power()
{
	RefreshVisual();
}

void APowerSwitch::RefreshVisual()
{
	if (StatusLight)
	{
		// красный = под напряжением (опасно лезть в воду), зелёный = обесточено (безопасно)
		StatusLight->SetLightColor(bPowerOn ? FLinearColor(1.f, 0.05f, 0.05f) : FLinearColor(0.05f, 1.f, 0.1f));
	}
	if (Label)
	{
		Label->SetText(bPowerOn
			? NSLOCTEXT("PowerSwitch", "On", "РУБИЛЬНИК: ПИТАНИЕ ВКЛ\n[E] обесточить")
			: NSLOCTEXT("PowerSwitch", "Off", "РУБИЛЬНИК: ОБЕСТОЧЕНО ✓\n[E] включить"));
		Label->SetTextRenderColor(bPowerOn ? FColor(255, 60, 40) : FColor(80, 255, 120));
	}

	// Лампы на «домовом» питании (тег "PoweredLight"): гасим/зажигаем на ВСЕХ машинах.
	// Здесь, а не в server-only ApplyToFloods, иначе у клиентов свет не гас — кооп-десинк.
	// Размен: ВКЛ → светло, но провод/вода под током; ВЫКЛ → безопасно чинить, но темно (фонарь).
	if (UWorld* W = GetWorld())
	{
		for (TActorIterator<AActor> It(W); It; ++It)
		{
			if (!It->Tags.Contains(FName(TEXT("PoweredLight"))))
			{
				continue;
			}
			TArray<ULightComponent*> Lights;
			It->GetComponents<ULightComponent>(Lights);
			for (ULightComponent* L : Lights)
			{
				if (L)
				{
					L->SetVisibility(bPowerOn);
				}
			}
		}
	}
}
