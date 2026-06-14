#include "AvaryoCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/UFlashlightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/TextureRenderTarget2D.h"
#include "World/AExitZone.h"
#include "Components/VitalsComponent.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/GameInstance.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputCoreTypes.h"
#include "Game/ARunState.h"
#include "Game/AvariikaOnlineSubsystem.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "Items/ABioPickup.h"
#include "Items/APickupItem.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "UI/AvaryoCameraShakes.h"
#include "UI/AvaryoHUD.h"
#include "World/ABioProjectile.h"
#include "World/AFloodlight.h"
#include "World/AFoamPatch.h"
#include "World/ARepairable.h"
#include "World/AToilet.h"
#include "World/ATrap.h"

AAvaryoCharacter::AAvaryoCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // фокус предмета + пересчёт скорости
	bReplicates = true;

	// Налобный фонарик. Сам свет (SpotLight) добавляется в Blueprint
	// и назначается в FlashlightComponent->AttachedLight (или найдётся автоматически).
	FlashlightComponent = CreateDefaultSubobject<UFlashlightComponent>(TEXT("Flashlight"));

	// Шкалы игрока
	VitalsComponent = CreateDefaultSubobject<UVitalsComponent>(TEXT("Vitals"));

	// Нагрудная камера для монитора оператора. Захват выключен,
	// включается локально только пока кто-то смотрит монитор (Tab в зоне ГАЗели)
	ChestCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ChestCamera"));
	ChestCamera->SetupAttachment(GetRootComponent());
	ChestCamera->SetRelativeLocation(FVector(30.f, 0.f, 30.f));
	ChestCamera->FOVAngle = 100.f;
	ChestCamera->CaptureSource = SCS_FinalColorLDR;
	ChestCamera->bCaptureEveryFrame = false;
	ChestCamera->bCaptureOnMovement = false;
	ChestCamera->SetComponentTickInterval(0.15f); // ~7 кадров/с — хватает и дёшево

	LightSlots.SetNum(NumLightSlots);
	ActiveSlot = 0;
	PickupRange = 350.f;
	ReviveRange = 250.f;

	BaseWalkSpeed = 500.f;
	SprintSpeed = 750.f;
	HeavyCarryMultiplier = 0.65f;
	CrawlSpeed = 120.f;
	IncidentSlowMultiplier = 0.7f;

	DragSpeedMultiplier = 0.55f;
	DragNoiseAccum = 0.f;
	bPanicShakeActive = false;
	bInteractionLocked = false;
	bAppliedInputLock = false;

	bMonitorOpen = false;
	bWasWounded = false;
	bSprayingHeld = false;
	SprayDrainAccum = 0.f;
	SprayNoiseAccum = 0.f;
	SprayFoamAccum = 0.f;
	FootstepNoiseAccum = 0.f;
	bSlipping = false;
	bSlipDefaultsSaved = false;
	SlipDefaultGroundFriction = 8.f;
	SlipDefaultBrakingDecel = 2048.f;

	ShoveRange = 220.f;
	ShoveForce = 750.f;
	ShoveUp = 280.f;
	ShovePanic = 10.f;
	ShoveCooldownTime = 1.2f;
	ShoveReadyTime = 0.f;
	ShoveFumbleChance = 0.4f;
	HeavyBonkChance = 0.3f;
	HeavyBonkDamage = 5.f;

	ThrowImpulseLight = 750.f;
	ThrowImpulseHeavy = 350.f;
	ThrowMaxSpreadDeg = 14.f;

	bStumbling = false;
	TripChancePerSecond = 0.03f;
	TripDarkMultiplier = 2.5f;
	TripPanicMultiplier = 1.5f;
	TripRecoverTime = 0.8f;
	TripSlowSpeed = 150.f;
	TripFumbleChance = 0.5f;
	TripTiredScale = 1.5f;
	StumbleUntil = 0.f;

	FoamSlipPanic = 4.f;
	FoamFallChancePerSecond = 0.25f;

	PanicSwayAmount = 9.f;
	bPanicCameraEffects = false; // паник-камера-эффекты (шейк+дрожь) выключены — мешали тесту; true чтобы вернуть
	AdrenalineHealthThreshold = 30.f;
	AdrenalineSpeedMult = 1.15f;
	AdrenalinePanicPerSecond = 3.f;
	UseCastRemaining = 0.f;
	UseCastDuration = 0.f;
	bOffering = false;

	// Приседание (Ctrl/C) — пригодится против монстра-слухача
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 250.f;

	// Сердцебиение паники: личный звук (2D), стартует выключенным, гонится в Tick
	HeartbeatAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("HeartbeatAudio"));
	HeartbeatAudio->SetupAttachment(RootComponent);
	HeartbeatAudio->bAutoActivate = false;
	HeartbeatAudio->bAllowSpatialization = false; // личное, без 3D-затухания
	HeartbeatAudio->SetVolumeMultiplier(0.f);
	static ConstructorHelpers::FObjectFinder<USoundBase> HeartSnd(TEXT("/Game/Audio/SFX/Heartbeat.Heartbeat"));
	if (HeartSnd.Succeeded())
	{
		HeartbeatSound = HeartSnd.Object;
		HeartbeatAudio->SetSound(HeartSnd.Object);
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> PickSnd(TEXT("/Game/Survival_SFX/User_Interface/Metal_item_pick_up.Metal_item_pick_up"));
	if (PickSnd.Succeeded()) { PickupSound = PickSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> UseSnd(TEXT("/Game/Survival_SFX/Craft/Crafting_cloth_item_1.Crafting_cloth_item_1"));
	if (UseSnd.Succeeded()) { UseSound = UseSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> HealSnd(TEXT("/Game/Survival_SFX/Survival/First_aid_1.First_aid_1"));
	if (HealSnd.Succeeded()) { HealSound = HealSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> SmokeSnd(TEXT("/Game/Survival_SFX/Survival/Lighter_1.Lighter_1"));
	if (SmokeSnd.Succeeded()) { SmokeSound = SmokeSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> DrinkSnd(TEXT("/Game/Audio/SFX/DrinkGlug.DrinkGlug"));
	if (DrinkSnd.Succeeded()) { DrinkSound = DrinkSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> ShoveSnd(TEXT("/Game/Survival_SFX/Survival/Punch_1.Punch_1"));
	if (ShoveSnd.Succeeded()) { ShoveSound = ShoveSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> RadioSnd(TEXT("/Game/Audio/SFX/RadioComm.RadioComm"));
	if (RadioSnd.Succeeded()) { RadioToggleSound = RadioSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> WalkSnd(TEXT("/Game/Survival_SFX/Movement/Walk_stone.Walk_stone"));
	if (WalkSnd.Succeeded()) { FootstepWalkSound = WalkSnd.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> RunSnd(TEXT("/Game/Survival_SFX/Movement/Jog_stone.Jog_stone"));
	if (RunSnd.Succeeded()) { FootstepRunSound = RunSnd.Object; }

	// Шаги — зацикленный цикл (клипы многошаговые), играем непрерывно во время движения
	FootstepAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudio"));
	FootstepAudio->SetupAttachment(RootComponent);
	FootstepAudio->bAutoActivate = false;

	// Звук применения предмета во время каста — гоняется в Tick по UseCastRemaining (реплиц.)
	UseCastAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("UseCastAudio"));
	UseCastAudio->SetupAttachment(RootComponent);
	UseCastAudio->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> JumpSnd(TEXT("/Game/Survival_SFX/Movement/Jump_stone.Jump_stone"));
	if (JumpSnd.Succeeded()) { JumpSound = JumpSnd.Object; }
	// Падение тела (споткнулся/поскользнулся) — глухой удар о бетон
	static ConstructorHelpers::FObjectFinder<USoundBase> FallSnd(TEXT("/Game/Audio/SFX/Foley/Foley_BodyFall_1.Foley_BodyFall_1"));
	if (FallSnd.Succeeded()) { FallSound = FallSnd.Object; }
	// Уронил предмет — металлический удар о пол
	static ConstructorHelpers::FObjectFinder<USoundBase> DropSnd(TEXT("/Game/Survival_SFX/User_Interface/Metal_item_drop.Metal_item_drop"));
	if (DropSnd.Succeeded()) { DropSound = DropSnd.Object; }
	// Переключение слота — тихий тик (личный 2D)
	static ConstructorHelpers::FObjectFinder<USoundBase> SlotSnd(TEXT("/Game/Survival_SFX/User_Interface/Button_hover.Button_hover"));
	if (SlotSnd.Succeeded()) { SlotSwitchSound = SlotSnd.Object; }

	// Струя огнетушителя — луп, гоняется в Tick по IsSpraying() (реплицируется → у всех)
	ExtinguisherAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ExtinguisherAudio"));
	ExtinguisherAudio->SetupAttachment(RootComponent);
	ExtinguisherAudio->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<USoundBase> SpraySnd(TEXT("/Game/Audio/SFX/Item/Item_ExtinguisherSpray_Loop.Item_ExtinguisherSpray_Loop"));
	if (SpraySnd.Succeeded())
	{
		ExtinguisherSprayLoopSound = SpraySnd.Object;
		ExtinguisherAudio->SetSound(SpraySnd.Object);
	}
}

void AAvaryoCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Камера из Blueprint — к ней крепится предмет в руках
	ViewCamera = FindComponentByClass<UCameraComponent>();

	// Фонарь по умолчанию выключен
	if (FlashlightComponent && FlashlightComponent->IsOn())
	{
		FlashlightComponent->TurnOff();
	}

	// Картинка нагрудной камеры. Низкое разрешение — её смотрят плиткой на мониторе
	ChestCamTarget = NewObject<UTextureRenderTarget2D>(this);
	ChestCamTarget->InitAutoFormat(320, 180);
	if (ChestCamera)
	{
		ChestCamera->TextureTarget = ChestCamTarget;
	}
}

void AAvaryoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Шаги — зацикленный цикл (звуки многошаговые): играем непрерывно во время движения,
	// переключая ходьба/бег; стоим — стоп. На всех машинах по скорости каждого персонажа.
	if (FootstepAudio)
	{
		const bool bMoving = GetCharacterMovement() && GetCharacterMovement()->IsMovingOnGround()
			&& GetVelocity().Size2D() > 10.f && (!VitalsComponent || !VitalsComponent->IsWounded());
		if (bMoving)
		{
			const bool bRun = VitalsComponent && VitalsComponent->IsSprinting();
			USoundBase* Want = (bRun && FootstepRunSound) ? FootstepRunSound : FootstepWalkSound;
			if (FootstepAudio->Sound != Want)
			{
				FootstepAudio->SetSound(Want);
				FootstepAudio->Play();
			}
			else if (!FootstepAudio->IsPlaying())
			{
				FootstepAudio->Play();
			}
			FootstepAudio->SetVolumeMultiplier(bIsCrouched ? 0.3f : (bRun ? 0.7f : 0.5f));
		}
		else if (FootstepAudio->Sound != nullptr)
		{
			FootstepAudio->Stop();
			FootstepAudio->SetSound(nullptr);
		}
	}

	// Звук применения предмета во время каста (UseCastRemaining реплицируется → у всех).
	// Играет всё время каста, гаснет при отмене (UseCastRemaining=0). Лупится, если у волны looping.
	if (UseCastAudio)
	{
		USoundBase* WantUse = (UseCastRemaining > 0.f) ? ItemUseSoundFor(GetHeldItem()) : nullptr;
		if (WantUse)
		{
			if (UseCastAudio->Sound != WantUse) { UseCastAudio->SetSound(WantUse); UseCastAudio->Play(); }
		}
		else if (UseCastAudio->Sound != nullptr)
		{
			UseCastAudio->Stop();
			UseCastAudio->SetSound(nullptr);
		}
	}

	// Струя огнетушителя: луп, пока удерживаемый баллон распыляет (bSpraying реплицируется → у всех).
	if (ExtinguisherAudio && ExtinguisherSprayLoopSound)
	{
		const APickupItem* Held = GetHeldItem();
		const bool bSpray = Held && Held->IsSpraying();
		if (bSpray && !ExtinguisherAudio->IsPlaying()) { ExtinguisherAudio->Play(); }
		else if (!bSpray && ExtinguisherAudio->IsPlaying()) { ExtinguisherAudio->Stop(); }
	}

	// Подсказки "[E] Подобрать" / "[E] Чинить" нужны только локальному игроку
	if (IsLocallyControlled())
	{
		FocusedItem = FindFocusedItem();
		FocusedRepairable = FindFocusedRepairable();
		FocusedWounded = FindFocusedWoundedTeammate();
		FocusedToilet = FindFocusedToilet();

		// Монитор оператора: закрывается, если вышел из зоны/ранен.
		// Захват камер у ВСЕХ персонажей включён локально только пока монитор открыт
		if (bMonitorOpen && !CanUseMonitor())
		{
			bMonitorOpen = false;
		}
		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			if (It->ChestCamera)
			{
				It->ChestCamera->bCaptureEveryFrame = bMonitorOpen;
			}
		}

		// Паника трясёт камеру: бесконечный шейк включается при входе в панику
		// и гасится при выходе (рестарты по таймеру давали рывки вбок)
		const bool bPanicNow = bPanicCameraEffects && VitalsComponent && VitalsComponent->IsPanicking();
		if (bPanicNow != bPanicShakeActive)
		{
			bPanicShakeActive = bPanicNow;
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				if (bPanicNow)
				{
					PC->ClientStartCameraShake(UPanicCameraShake::StaticClass(), 1.f);
				}
				else
				{
					PC->ClientStopCameraShake(UPanicCameraShake::StaticClass(), false);
				}
			}
		}

		// Сердцебиение при панике — только локальный игрок слышит своё (громче/быстрее с паникой)
		if (HeartbeatAudio)
		{
			const bool bPanic = VitalsComponent && VitalsComponent->IsPanicking();
			if (bPanic)
			{
				const float P = FMath::Clamp(VitalsComponent->GetPanic() / 100.f, 0.f, 1.f);
				if (!HeartbeatAudio->IsPlaying())
				{
					HeartbeatAudio->Play();
				}
				HeartbeatAudio->SetVolumeMultiplier(0.4f + 0.6f * P);
				HeartbeatAudio->SetPitchMultiplier(0.9f + 0.5f * P);
			}
			else if (HeartbeatAudio->IsPlaying())
			{
				HeartbeatAudio->Stop();
			}
		}

		// Блокировка ввода (туалет/щиток): применяем к контроллеру на переходах
		if (bInteractionLocked != bAppliedInputLock)
		{
			bAppliedInputLock = bInteractionLocked;
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				PC->SetIgnoreMoveInput(bInteractionLocked);
				PC->SetIgnoreLookInput(bInteractionLocked);
			}
		}
	}

	// Починка сорвалась на стороне объекта (ушёл, ранен) — чистим ссылку
	if (HasAuthority() && CurrentRepairable && CurrentRepairable->GetRepairer() != this)
	{
		CurrentRepairable = nullptr;
	}
	if (HasAuthority() && CurrentToilet && CurrentToilet->GetOccupant() != this)
	{
		CurrentToilet = nullptr; // «процесс» сорвался или завершился
	}

	// Скорость зависит от состояния — обновляем на сервере и у владельца
	if (HasAuthority() || IsLocallyControlled())
	{
		RefreshMoveSpeed();
	}

	// Ранение: тяжёлый предмет выпадает из рук (мелкий можно держать)
	if (HasAuthority() && VitalsComponent)
	{
		const bool bWoundedNow = VitalsComponent->IsWounded();
		if (bWoundedNow && !bWasWounded)
		{
			StopSpraying();
			CancelUseCast();
			ReleaseDraggedTeammate(); // раненый никого не тащит
			if (bOffering)
			{
				bOffering = false;
				RefreshHeldItem();
			}
			if (HeavySlot)
			{
				ActiveSlot = 0;
				DropItem();
			}
		}
		bWasWounded = bWoundedNow;
	}

	// Огнетушитель: расход, облако, шум
	if (HasAuthority() && bSprayingHeld)
	{
		TickSpray(DeltaSeconds);
	}

	// Скольжение по пене (сервер решает, клиент узнаёт через bSlipping)
	if (HasAuthority())
	{
		UpdateFoamSlip(DeltaSeconds);
		UpdateTrip(DeltaSeconds);

		// Адреналин на низком HP стоит нервов — паника подрастает
		if (VitalsComponent && !VitalsComponent->IsWounded()
			&& VitalsComponent->GetHealth() < AdrenalineHealthThreshold)
		{
			VitalsComponent->AddPanic(AdrenalinePanicPerSecond * DeltaSeconds);
		}
	}

	// Дрожащие руки: лёгкая тряска прицела при панике (у владельца, Panic реплицируется)
	if (bPanicCameraEffects && IsLocallyControlled() && VitalsComponent && VitalsComponent->IsPanicking())
	{
		const float Sway = PanicSwayAmount * (VitalsComponent->GetPanic() / 100.f);
		const float T = GetWorld()->GetTimeSeconds();
		AddControllerYawInput(FMath::Sin(T * 7.3f) * Sway * DeltaSeconds);
		AddControllerPitchInput(FMath::Sin(T * 9.1f) * Sway * DeltaSeconds);
	}

	// Волочение раненого
	if (HasAuthority() && DraggedTeammate)
	{
		TickDrag(DeltaSeconds);
	}

	// Применение предмета (аптечка 6 сек, сигарета 2 сек)
	if (HasAuthority() && UseCastRemaining > 0.f)
	{
		TickUseCast(DeltaSeconds);
	}

	// Бег шумит — монстр-слухач это услышит
	if (HasAuthority() && VitalsComponent && VitalsComponent->IsSprinting()
		&& GetVelocity().SizeSquared2D() > 100.f)
	{
		FootstepNoiseAccum += DeltaSeconds;
		if (FootstepNoiseAccum >= 0.5f)
		{
			FootstepNoiseAccum = 0.f;
			MakeNoise(1.f, this, GetActorLocation());
		}
	}
	else if (HasAuthority())
	{
		FootstepNoiseAccum = 0.f; // не бежим — следующий шаг отсчитываем с нуля (ровная каденция шума)
	}
}

void AAvaryoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Временные хардкод-бинды для теста; позже заменим на Enhanced Input
	PlayerInputComponent->BindKey(EKeys::F, IE_Pressed, this, &AAvaryoCharacter::ToggleFlashlight);
	PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AAvaryoCharacter::OnInteractPressed);
	PlayerInputComponent->BindKey(EKeys::E, IE_Released, this, &AAvaryoCharacter::OnInteractReleased);
	// G — единая кнопка: тап = слабый бросок, удержание = зарядка силы; в мини-игре = «встать/выйти»
	PlayerInputComponent->BindKey(EKeys::G, IE_Pressed, this, &AAvaryoCharacter::OnThrowKeyPressed);
	PlayerInputComponent->BindKey(EKeys::G, IE_Released, this, &AAvaryoCharacter::OnThrowKeyReleased);
	PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AAvaryoCharacter::Shove);
	// Использование: ЛКМ или R (нажал — эффект/распыление, отпустил — конец распыления)
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AAvaryoCharacter::BeginUseHeldItem);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AAvaryoCharacter::EndUseHeldItem);
	PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AAvaryoCharacter::BeginUseHeldItem);
	PlayerInputComponent->BindKey(EKeys::R, IE_Released, this, &AAvaryoCharacter::EndUseHeldItem);
	// Тот же R на экране «Акта» перезапускает смену (во время забега ничего не делает)
	PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AAvaryoCharacter::TryRestartRun);
	// Передача предмета: держишь ПКМ — предмет вытянут вперёд, тиммейт забирает по E
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AAvaryoCharacter::BeginOfferItem);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AAvaryoCharacter::EndOfferItem);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &AAvaryoCharacter::StartSprint);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Released, this, &AAvaryoCharacter::StopSprint);
	// Приседание: Ctrl или C (зажал — присел)
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Pressed, this, &AAvaryoCharacter::StartCrouchInput);
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Released, this, &AAvaryoCharacter::StopCrouchInput);
	PlayerInputComponent->BindKey(EKeys::C, IE_Pressed, this, &AAvaryoCharacter::StartCrouchInput);
	PlayerInputComponent->BindKey(EKeys::C, IE_Released, this, &AAvaryoCharacter::StopCrouchInput);

	// Монитор оператора (только в зоне ГАЗели)
	PlayerInputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &AAvaryoCharacter::ToggleMonitor);

	// Слоты инвентаря: 1 — тяжёлый, 2-5 — лёгкие
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AAvaryoCharacter::EquipSlot1);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AAvaryoCharacter::EquipSlot2);
	PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AAvaryoCharacter::EquipSlot3);
	PlayerInputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AAvaryoCharacter::EquipSlot4);
	PlayerInputComponent->BindKey(EKeys::Five, IE_Pressed, this, &AAvaryoCharacter::EquipSlot5);
}

void AAvaryoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAvaryoCharacter, HeavySlot);
	DOREPLIFETIME(AAvaryoCharacter, LightSlots);
	DOREPLIFETIME(AAvaryoCharacter, ActiveSlot);
	DOREPLIFETIME(AAvaryoCharacter, UseCastRemaining);
	DOREPLIFETIME(AAvaryoCharacter, UseCastDuration);
	DOREPLIFETIME(AAvaryoCharacter, bOffering);
	DOREPLIFETIME(AAvaryoCharacter, CurrentRepairable);
	DOREPLIFETIME(AAvaryoCharacter, CurrentToilet);
	DOREPLIFETIME(AAvaryoCharacter, DraggedTeammate);
	DOREPLIFETIME(AAvaryoCharacter, DraggedBy);
	DOREPLIFETIME(AAvaryoCharacter, bInteractionLocked);
	DOREPLIFETIME(AAvaryoCharacter, bSlipping);
	DOREPLIFETIME(AAvaryoCharacter, bStumbling);
}

void AAvaryoCharacter::SetInteractionLocked(bool bNewLocked)
{
	if (!HasAuthority() || bInteractionLocked == bNewLocked)
	{
		return;
	}
	bInteractionLocked = bNewLocked;
	// Движение глушим на сервере — позиция авторитетна
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->SetMovementMode(bNewLocked ? MOVE_None : MOVE_Walking);
	}
}

void AAvaryoCharacter::ClientSetControlYaw_Implementation(float NewYaw)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetControlRotation(FRotator(0.f, NewYaw, 0.f));
	}
}

float AAvaryoCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Actual = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (HasAuthority() && VitalsComponent)
	{
		VitalsComponent->ApplyDamage(DamageAmount);
	}
	return Actual;
}

bool AAvaryoCharacter::CanJumpInternal_Implementation() const
{
	// Раненый не прыгает; во время применения предмета — тоже
	return (!VitalsComponent || !VitalsComponent->IsWounded())
		&& !IsUsingItem()
		&& Super::CanJumpInternal_Implementation();
}

void AAvaryoCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	// Звук приземления — локально на каждой машине для приземлившегося персонажа (как шаги)
	if (JumpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation(), bIsCrouched ? 0.4f : 0.7f);
	}
}

// ---------- Движение ----------

void AAvaryoCharacter::RefreshMoveSpeed()
{
	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (!Move || !VitalsComponent)
	{
		return;
	}

	// Во время применения бег запрещён, скорость зависит от предмета
	const bool bCastingUse = IsUsingItem();
	float Speed = (VitalsComponent->IsSprinting() && !bCastingUse) ? SprintSpeed : BaseWalkSpeed;

	if (bCastingUse)
	{
		APickupItem* Held = GetHeldItem();
		Speed *= Held ? Held->CastSpeedMultiplier : 1.f; // аптечка 0.5, сигарета 1.0
	}

	if (IsCarryingHeavy())
	{
		Speed *= HeavyCarryMultiplier; // сварочник в руках — не побегаешь
	}
	if (DraggedTeammate)
	{
		Speed *= DragSpeedMultiplier; // тащишь тело — медленно
	}
	if (DraggedBy)
	{
		Speed = 0.f; // тебя волокут — не ползай против движения
	}
	if (VitalsComponent->IsIncidentSlowed())
	{
		Speed *= IncidentSlowMultiplier;
	}
	if (bStumbling)
	{
		Speed = FMath::Min(Speed, TripSlowSpeed); // споткнулся — резко сбросил ход
	}
	// Адреналин: на низком HP (но ещё на ногах) — рывок скорости (паника за это растёт в Vitals)
	if (!VitalsComponent->IsWounded() && VitalsComponent->GetHealth() < AdrenalineHealthThreshold)
	{
		Speed *= AdrenalineSpeedMult;
	}
	if (VitalsComponent->IsWounded())
	{
		Speed = CrawlSpeed; // ползём
	}

	Move->MaxWalkSpeed = Speed;
}

void AAvaryoCharacter::StartSprint()
{
	if (VitalsComponent)
	{
		VitalsComponent->SetSprinting(true); // локально для отзывчивости
	}
	if (!HasAuthority())
	{
		ServerSetSprinting(true);
	}
}

void AAvaryoCharacter::StopSprint()
{
	if (VitalsComponent)
	{
		VitalsComponent->SetSprinting(false);
	}
	if (!HasAuthority())
	{
		ServerSetSprinting(false);
	}
}

void AAvaryoCharacter::ServerSetSprinting_Implementation(bool bNewSprinting)
{
	if (VitalsComponent)
	{
		VitalsComponent->SetSprinting(bNewSprinting);
	}
}

// ---------- Фонарик ----------

void AAvaryoCharacter::ToggleFlashlight()
{
	if (!FlashlightComponent)
	{
		return;
	}

	if (HasAuthority())
	{
		FlashlightComponent->Toggle();
	}
	else
	{
		ServerToggleFlashlight();
	}
}

void AAvaryoCharacter::ServerToggleFlashlight_Implementation()
{
	if (FlashlightComponent)
	{
		FlashlightComponent->Toggle();
	}
}

// ---------- Инвентарь ----------

APickupItem* AAvaryoCharacter::GetItemInSlot(int32 SlotIndex) const
{
	if (SlotIndex == 0)
	{
		return HeavySlot;
	}
	return LightSlots.IsValidIndex(SlotIndex - 1) ? LightSlots[SlotIndex - 1].Get() : nullptr;
}

APickupItem* AAvaryoCharacter::GetHeldItem() const
{
	return GetItemInSlot(ActiveSlot);
}

bool AAvaryoCharacter::CanPickupItem(APickupItem* Item) const
{
	if (!Item)
	{
		return false;
	}

	// Предмет в чужих руках можно взять, только если его передают (вытянут вперёд по ПКМ)
	if (Item->GetOwner())
	{
		AAvaryoCharacter* Holder = Cast<AAvaryoCharacter>(Item->GetOwner());
		const bool bOffered = Holder && Holder != this && Holder->IsOffering() && Holder->GetHeldItem() == Item;
		if (!bOffered)
		{
			return false;
		}
	}

	if (Item->ItemSize == EItemSize::Heavy)
	{
		// Правило сварочника: только один тяжёлый; раненый тяжёлое не поднимает
		return !HeavySlot && !(VitalsComponent && VitalsComponent->IsWounded());
	}

	for (int32 i = 0; i < NumLightSlots; ++i)
	{
		if (!LightSlots[i])
		{
			return true;
		}
	}
	return false; // лёгкие слоты забиты
}

APickupItem* AAvaryoCharacter::FindFocusedItem() const
{
	// Сначала свип из камеры — предмет, на который смотрим.
	// Сфера вместо тонкого луча, чтобы не промахиваться по мелочи вроде предохранителя.
	FVector ViewLoc;
	FRotator ViewRot;
	GetActorEyesViewPoint(ViewLoc, ViewRot);

	FCollisionQueryParams Params(FName(TEXT("PickupTrace")), false, this);
	FHitResult Hit;
	const FCollisionShape Probe = FCollisionShape::MakeSphere(12.f);
	if (GetWorld()->SweepSingleByChannel(Hit, ViewLoc, ViewLoc + ViewRot.Vector() * PickupRange, FQuat::Identity, ECC_Visibility, Probe, Params))
	{
		if (APickupItem* Item = Cast<APickupItem>(Hit.GetActor()))
		{
			if (CanPickupItem(Item))
			{
				return Item;
			}
		}
	}

	// Фолбэк: ближайший ПОДБИРАЕМЫЙ предмет, в радиус которого мы вошли
	TArray<AActor*> Overlapping;
	GetOverlappingActors(Overlapping, APickupItem::StaticClass());

	APickupItem* Nearest = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	for (AActor* Actor : Overlapping)
	{
		APickupItem* Item = Cast<APickupItem>(Actor);
		if (!CanPickupItem(Item))
		{
			continue;
		}
		const float DistSq = FVector::DistSquared(GetActorLocation(), Item->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Nearest = Item;
		}
	}
	return Nearest;
}

void AAvaryoCharacter::TryPickupNearbyItem()
{
	if (!HasAuthority())
	{
		ServerTryPickupNearbyItem();
		return;
	}

	if (APickupItem* Item = FindFocusedItem())
	{
		PickupItem(Item);
	}
}

void AAvaryoCharacter::ServerTryPickupNearbyItem_Implementation()
{
	TryPickupNearbyItem();
}

// ---------- Взаимодействие (E) ----------

ARepairable* AAvaryoCharacter::FindFocusedRepairable() const
{
	// Раненый не работник — подсказку ему не показываем
	if (VitalsComponent && VitalsComponent->IsWounded())
	{
		return nullptr;
	}

	FVector ViewLoc;
	FRotator ViewRot;
	GetActorEyesViewPoint(ViewLoc, ViewRot);

	FCollisionQueryParams Params(FName(TEXT("RepairTrace")), false, this);
	FHitResult Hit;
	const FCollisionShape Probe = FCollisionShape::MakeSphere(12.f);
	if (GetWorld()->SweepSingleByChannel(Hit, ViewLoc, ViewLoc + ViewRot.Vector() * PickupRange, FQuat::Identity, ECC_Visibility, Probe, Params))
	{
		if (ARepairable* Repairable = Cast<ARepairable>(Hit.GetActor()))
		{
			if (Repairable->IsBroken())
			{
				return Repairable;
			}
		}
	}
	return nullptr;
}

void AAvaryoCharacter::OnInteractPressed()
{
	if (!HasAuthority())
	{
		ServerInteractPressed();
		return;
	}
	InteractPressedAuth();
}

void AAvaryoCharacter::OnInteractReleased()
{
	if (!HasAuthority())
	{
		ServerInteractReleased();
		return;
	}
	InteractReleasedAuth();
}

void AAvaryoCharacter::InteractPressedAuth()
{
	// Сидим в биотуалете — E бьёт по курсору мини-игры
	if (CurrentToilet)
	{
		CurrentToilet->TryHitBy(this);
		return;
	}

	// Чиним в мини-игре — E по типу: щиток — удар по курсору, вентиль — докрутка, стартер — потянуть шнур.
	// Колхоз (без инструмента) — это удержание E, а не мини-игра.
	if (CurrentRepairable && CurrentRepairable->IsMinigameRepair() && CurrentRepairable->ArePrereqsDone()
		&& !CurrentRepairable->IsBotching())
	{
		CurrentRepairable->TryHitBy(this);
		return;
	}

	// Активна prereq-мини-игра (заварка/починка руками) — E это удар по курсору
	if (CurrentRepairable && CurrentRepairable->IsDoingPrereqMinigame())
	{
		CurrentRepairable->TryHitBy(this);
		return;
	}

	// Этап «вставить расходник»: смотрим на объект, в руках нужный предмет (кабель/канистра/предохранитель) — E вставляет
	if (ARepairable* RInsert = FindFocusedRepairable())
	{
		if (RInsert->NeedsInsertNow() && RInsert->TryInsertBy(this))
		{
			return;
		}
	}

	// Уже тащишь раненого — повторное E отпускает
	if (DraggedTeammate)
	{
		ReleaseDraggedTeammate();
		return;
	}

	// Приоритет: предмет → ремонт → раненый тиммейт
	if (APickupItem* Item = FindFocusedItem())
	{
		PickupItem(Item);
		return;
	}

	if (ARepairable* Repairable = FindFocusedRepairable())
	{
		if (Repairable->BeginRepairBy(this))
		{
			CurrentRepairable = Repairable;
		}
		return;
	}

	if (AToilet* Toilet = FindFocusedToilet())
	{
		if (Toilet->BeginUseBy(this))
		{
			CurrentToilet = Toilet;
		}
		return;
	}

	if (AAvaryoCharacter* Wounded = FindFocusedWoundedTeammate())
	{
		if (CanDrag(Wounded))
		{
			StopSpraying();
			CancelUseCast();
			DraggedTeammate = Wounded;
			Wounded->DraggedBy = this;
			DragNoiseAccum = 0.f;
			if (ARunState* Run = ARunState::Get(GetWorld()))
			{
				Run->AddDrag(this); // эвакуация — в «Акт»
			}
		}
	}
}

void AAvaryoCharacter::InteractReleasedAuth()
{
	if (!CurrentRepairable)
	{
		return;
	}
	ARepairable* R = CurrentRepairable;

	// На подготовительных этапах: Hold завершается отпусканием E; AutoFill и prereq-мини-игра — НЕ завершаются
	if (!R->ArePrereqsDone())
	{
		if (R->IsDoingPrereqHold())
		{
			R->EndRepairBy(this);
			CurrentRepairable = nullptr;
		}
		return;
	}

	// Основная мини-игра НЕ завершается отпусканием E (выход по отходу), но стартеру важен момент отпускания
	if (R->IsMinigameRepair() && !R->IsBotching())
	{
		R->TryReleaseBy(this);
		return;
	}

	// Обычная починка (удержание E) — завершается
	R->EndRepairBy(this);
	CurrentRepairable = nullptr;
}

void AAvaryoCharacter::TryRestartRun()
{
	ARunState* Run = ARunState::Get(GetWorld());
	if (!Run || Run->GetPhase() == ERunPhase::InProgress)
	{
		return; // не финальный экран — R работает как применение предмета
	}
	if (HasAuthority())
	{
		Run->RequestRestart();
	}
	else
	{
		ServerRequestRestart();
	}
}

void AAvaryoCharacter::ServerRequestRestart_Implementation()
{
	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->RequestRestart();
	}
}

void AAvaryoCharacter::AvVital(const FString& Which, float Value)
{
	if (!HasAuthority()) { ServerAvVital(Which, Value); return; }
	if (VitalsComponent) { VitalsComponent->DebugSetVital(FName(*Which), Value); }
}
void AAvaryoCharacter::ServerAvVital_Implementation(const FString& Which, float Value) { AvVital(Which, Value); }

void AAvaryoCharacter::AvIncident()
{
	AvVital(TEXT("bladder"), 100.f); // шкала переполнена → инцидент сработает на ближайшем тике
}

void AAvaryoCharacter::AvGiveBio()
{
	if (!HasAuthority()) { ServerAvGiveBio(); return; }
	const FVector Loc = GetActorLocation() + GetActorForwardVector() * 120.f;
	FActorSpawnParameters P;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (ABioPickup* Bio = GetWorld()->SpawnActor<ABioPickup>(ABioPickup::StaticClass(), Loc, FRotator::ZeroRotator, P))
	{
		PickupItem(Bio); // сразу в руки (и руки пропахнут — как при обычном подборе)
	}
}
void AAvaryoCharacter::ServerAvGiveBio_Implementation() { AvGiveBio(); }

void AAvaryoCharacter::AvCheapGear()
{
	if (!HasAuthority()) { ServerAvCheapGear(); return; }
	if (ARunState* Run = ARunState::Get(GetWorld())) { Run->DebugForceCheapGear(); }
}
void AAvaryoCharacter::ServerAvCheapGear_Implementation() { AvCheapGear(); }

void AAvaryoCharacter::AvFinish(const FString& Outcome)
{
	if (!HasAuthority()) { ServerAvFinish(Outcome); return; }
	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->DebugFinishRun(Outcome.StartsWith(TEXT("w"))); // "win" → победа, иначе поражение
	}
}
void AAvaryoCharacter::ServerAvFinish_Implementation(const FString& Outcome) { AvFinish(Outcome); }

void AAvaryoCharacter::AvQuota(int32 Target)
{
	if (!HasAuthority()) { ServerAvQuota(Target); return; }
	if (ARunState* Run = ARunState::Get(GetWorld())) { Run->DebugSetQuota(Target); }
}
void AAvaryoCharacter::ServerAvQuota_Implementation(int32 Target) { AvQuota(Target); }

void AAvaryoCharacter::AvUpgrade(const FString& Tool)
{
	if (!HasAuthority()) { ServerAvUpgrade(Tool); return; }
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger = GI->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			Ledger->BuyUpgrade(FName(*Tool));
		}
	}
}
void AAvaryoCharacter::ServerAvUpgrade_Implementation(const FString& Tool) { AvUpgrade(Tool); }

void AAvaryoCharacter::AvShop()
{
	// Локальный экран — без сервера
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AAvaryoHUD* HUD = Cast<AAvaryoHUD>(PC->GetHUD()))
		{
			HUD->ToggleShop();
		}
	}
}

void AAvaryoCharacter::AvGod()
{
	if (!HasAuthority()) { ServerAvGod(); return; }
	if (VitalsComponent)
	{
		const bool bNew = !VitalsComponent->IsInvulnerable();
		VitalsComponent->SetInvulnerable(bNew);
		if (bNew) { VitalsComponent->DebugSetVital(TEXT("health"), 100.f); } // долить HP при включении
	}
}
void AAvaryoCharacter::ServerAvGod_Implementation() { AvGod(); }

void AAvaryoCharacter::AvGive(const FString& What)
{
	if (!HasAuthority()) { ServerAvGive(What); return; }

	const FString w = What.ToLower();
	if (w.Contains(TEXT("bio"))) { AvGiveBio(); return; }

	EItemEffect Eff = EItemEffect::None;
	if (w.Contains(TEXT("coffee")) || w.Contains(TEXT("кофе")) || w.Contains(TEXT("drink")) || w.Contains(TEXT("термос"))) Eff = EItemEffect::Drink;
	else if (w.Contains(TEXT("heal")) || w.Contains(TEXT("аптеч")))    Eff = EItemEffect::Heal;
	else if (w.Contains(TEXT("calm")) || w.Contains(TEXT("сигар")) || w.Contains(TEXT("cig"))) Eff = EItemEffect::Calm;
	else if (w.Contains(TEXT("ext"))  || w.Contains(TEXT("огнетуш")))  Eff = EItemEffect::Extinguish;
	else if (w.Contains(TEXT("radio")) || w.Contains(TEXT("рация")))   Eff = EItemEffect::Radio;
	else if (w.Contains(TEXT("recharge")) || w.Contains(TEXT("батар")) || w.Contains(TEXT("batt"))) Eff = EItemEffect::Recharge;
	else if (w.Contains(TEXT("trap")) || w.Contains(TEXT("растяж")))   Eff = EItemEffect::DeployTrap;
	else if (w.Contains(TEXT("light")) || w.Contains(TEXT("прожект"))) Eff = EItemEffect::DeployLight;

	// Образец на уровне для клонирования (по эффекту; иначе по имени актора)
	UClass* CloneClass = nullptr;
	for (TActorIterator<APickupItem> It(GetWorld()); It; ++It)
	{
		const bool bByEffect = (Eff != EItemEffect::None && It->ItemEffect == Eff);
		const bool bByName = (Eff == EItemEffect::None && It->GetName().ToLower().Contains(w));
		if (bByEffect || bByName) { CloneClass = It->GetClass(); break; }
	}

	const FVector Loc = GetActorLocation() + GetActorForwardVector() * 90.f + FVector(0.f, 0.f, 20.f);
	FActorSpawnParameters P;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APickupItem* NewItem = nullptr;
	if (CloneClass)
	{
		NewItem = GetWorld()->SpawnActor<APickupItem>(CloneClass, Loc, FRotator::ZeroRotator, P);
	}
	else if (Eff != EItemEffect::None)
	{
		// Нет образца на карте — базовый предмет с нужным эффектом (для теста эффекта)
		NewItem = GetWorld()->SpawnActor<APickupItem>(APickupItem::StaticClass(), Loc, FRotator::ZeroRotator, P);
		if (NewItem)
		{
			NewItem->ItemEffect = Eff;
			NewItem->ItemSize = EItemSize::Light;
			NewItem->Charges = 5;
			NewItem->EffectMagnitude = 60.f;
			NewItem->DisplayName = FText::FromString(What);
		}
	}

	if (NewItem)
	{
		PickupItem(NewItem); // сразу в свободный слот
	}
}
void AAvaryoCharacter::ServerAvGive_Implementation(const FString& What) { AvGive(What); }

void AAvaryoCharacter::AvToolQ(float Scale)
{
	if (!HasAuthority()) { ServerAvToolQ(Scale); return; }
	if (APickupItem* Held = GetHeldItem())
	{
		Held->ToolQualityScale = FMath::Clamp(Scale, 0.25f, 3.f);
	}
}
void AAvaryoCharacter::ServerAvToolQ_Implementation(float Scale) { AvToolQ(Scale); }

void AAvaryoCharacter::AvMoney(int32 Amount)
{
	if (!HasAuthority()) { ServerAvMoney(Amount); return; }
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger = GI->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			Ledger->AddBalance(Amount);
		}
	}
}
void AAvaryoCharacter::ServerAvMoney_Implementation(int32 Amount) { AvMoney(Amount); }

void AAvaryoCharacter::AvSetGear(const FString& Tool, int32 Level)
{
	if (!HasAuthority()) { ServerAvSetGear(Tool, Level); return; }
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCompanyLedgerSubsystem* Ledger = GI->GetSubsystem<UCompanyLedgerSubsystem>())
		{
			Ledger->SetEquipmentLevel(FName(*Tool), Level);
		}
	}
}
void AAvaryoCharacter::ServerAvSetGear_Implementation(const FString& Tool, int32 Level) { AvSetGear(Tool, Level); }

void AAvaryoCharacter::AvBattery(float Pct)
{
	if (!HasAuthority()) { ServerAvBattery(Pct); return; }
	if (FlashlightComponent) { FlashlightComponent->DebugSetBattery(Pct); }
}
void AAvaryoCharacter::ServerAvBattery_Implementation(float Pct) { AvBattery(Pct); }

static UAvariikaOnlineSubsystem* GetOnline(AActor* A)
{
	return (A && A->GetGameInstance()) ? A->GetGameInstance()->GetSubsystem<UAvariikaOnlineSubsystem>() : nullptr;
}
void AAvaryoCharacter::AvHost()  { if (UAvariikaOnlineSubsystem* O = GetOnline(this)) { O->HostGame(); } }
void AAvaryoCharacter::AvFind()  { if (UAvariikaOnlineSubsystem* O = GetOnline(this)) { O->FindGames(); } }
void AAvaryoCharacter::AvJoin(int32 Index) { if (UAvariikaOnlineSubsystem* O = GetOnline(this)) { O->JoinGameByIndex(Index); } }
void AAvaryoCharacter::AvLeave() { if (UAvariikaOnlineSubsystem* O = GetOnline(this)) { O->LeaveGame(); } }

// ---------- Оператор: нагрудные камеры ----------

bool AAvaryoCharacter::CanUseMonitor() const
{
	// Оператор сидит в машине: монитор доступен только в зоне ГАЗели и не раненому
	if (VitalsComponent && VitalsComponent->IsWounded())
	{
		return false;
	}
	for (TActorIterator<AExitZone> It(GetWorld()); It; ++It)
	{
		if (It->IsOverlappingActor(this))
		{
			return true;
		}
	}
	return false;
}

void AAvaryoCharacter::ToggleMonitor()
{
	if (!bMonitorOpen && !CanUseMonitor())
	{
		return;
	}
	bMonitorOpen = !bMonitorOpen;
}

AToilet* AAvaryoCharacter::FindFocusedToilet() const
{
	FVector ViewLoc;
	FRotator ViewRot;
	GetActorEyesViewPoint(ViewLoc, ViewRot);

	FCollisionQueryParams Params(FName(TEXT("ToiletTrace")), false, this);
	FHitResult Hit;
	const FCollisionShape Probe = FCollisionShape::MakeSphere(12.f);
	if (GetWorld()->SweepSingleByChannel(Hit, ViewLoc, ViewLoc + ViewRot.Vector() * PickupRange, FQuat::Identity, ECC_Visibility, Probe, Params))
	{
		return Cast<AToilet>(Hit.GetActor());
	}
	return nullptr;
}

// ---------- Перетаскивание раненого ----------

AAvaryoCharacter* AAvaryoCharacter::FindFocusedWoundedTeammate() const
{
	// Свип из камеры — раненый, на которого смотрим
	FVector ViewLoc;
	FRotator ViewRot;
	GetActorEyesViewPoint(ViewLoc, ViewRot);

	FCollisionQueryParams Params(FName(TEXT("DragTrace")), false, this);
	FHitResult Hit;
	const FCollisionShape Probe = FCollisionShape::MakeSphere(30.f);
	if (GetWorld()->SweepSingleByChannel(Hit, ViewLoc, ViewLoc + ViewRot.Vector() * PickupRange, FQuat::Identity, ECC_Visibility, Probe, Params))
	{
		AAvaryoCharacter* Other = Cast<AAvaryoCharacter>(Hit.GetActor());
		if (Other && Other->VitalsComponent && Other->VitalsComponent->IsWounded())
		{
			return Other;
		}
	}

	// Фолбэк: раненый вплотную (лежит под ногами — трейс легко промахивается)
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		AAvaryoCharacter* Other = *It;
		if (Other != this && Other->VitalsComponent && Other->VitalsComponent->IsWounded()
			&& FVector::DistSquared(Other->GetActorLocation(), GetActorLocation()) < FMath::Square(ReviveRange))
		{
			return Other;
		}
	}
	return nullptr;
}

bool AAvaryoCharacter::CanDrag(const AAvaryoCharacter* Wounded) const
{
	if (!Wounded || Wounded == this || !Wounded->VitalsComponent || !Wounded->VitalsComponent->IsWounded())
	{
		return false;
	}
	if (Wounded->DraggedBy)
	{
		return false; // его уже тащат
	}
	if (VitalsComponent && VitalsComponent->IsWounded())
	{
		return false; // раненый не носильщик
	}
	if (IsCarryingHeavy())
	{
		return false; // руки заняты тяжёлым — сначала поставь (G)
	}
	return true;
}

void AAvaryoCharacter::ReleaseDraggedTeammate()
{
	if (DraggedTeammate)
	{
		DraggedTeammate->DraggedBy = nullptr;
		DraggedTeammate = nullptr;
	}
}

void AAvaryoCharacter::TickDrag(float DeltaSeconds)
{
	// Драг срывается: раненый встал, тащащий ранен, взял тяжёлое, тело застряло далеко
	if (!CanDragContinue())
	{
		ReleaseDraggedTeammate();
		return;
	}

	// Тянем тело за спину с интерполяцией; свип не даёт протащить сквозь стены
	const FVector Target = GetActorLocation() - GetActorForwardVector() * 130.f;
	const FVector NewLoc = FMath::VInterpTo(DraggedTeammate->GetActorLocation(), Target, DeltaSeconds, 6.f);
	DraggedTeammate->SetActorLocation(NewLoc, true);

	// Волочение тела шуршит — монстр-слухач оценит
	DragNoiseAccum += DeltaSeconds;
	if (DragNoiseAccum >= 1.f)
	{
		DragNoiseAccum = 0.f;
		MakeNoise(0.5f, this, GetActorLocation());
	}
}

bool AAvaryoCharacter::CanDragContinue() const
{
	return DraggedTeammate
		&& DraggedTeammate->VitalsComponent && DraggedTeammate->VitalsComponent->IsWounded()
		&& !(VitalsComponent && VitalsComponent->IsWounded())
		&& !IsCarryingHeavy()
		&& FVector::DistSquared(DraggedTeammate->GetActorLocation(), GetActorLocation()) < FMath::Square(450.f);
}

void AAvaryoCharacter::ServerInteractPressed_Implementation()
{
	InteractPressedAuth();
}

void AAvaryoCharacter::ServerInteractReleased_Implementation()
{
	InteractReleasedAuth();
}

void AAvaryoCharacter::PickupItem(APickupItem* Item)
{
	// Инвентарь живёт на сервере; правила (один тяжёлый и т.д.) — в CanPickupItem
	if (!HasAuthority() || !CanPickupItem(Item))
	{
		return;
	}

	// Звук подбора — у всех
	if (PickupSound)
	{
		MulticastSound(PickupSound, GetActorLocation(), 1.f);
	}

	// Передача из рук в руки: забираем предмет у того, кто его протянул
	if (AAvaryoCharacter* Holder = Cast<AAvaryoCharacter>(Item->GetOwner()))
	{
		Holder->StopSpraying();
		Holder->CancelUseCast();
		Holder->bOffering = false;
		Holder->ClearSlotFor(Item);
		Holder->RefreshHeldItem();
	}

	int32 TargetSlot = 0;
	if (Item->ItemSize == EItemSize::Light)
	{
		for (int32 i = 0; i < NumLightSlots; ++i)
		{
			if (!LightSlots[i])
			{
				TargetSlot = i + 1;
				break;
			}
		}
	}

	if (TargetSlot == 0)
	{
		HeavySlot = Item;
	}
	else
	{
		LightSlots[TargetSlot - 1] = Item;
	}

	StopSpraying();   // подбор переключает активный слот —
	CancelUseCast();  // распыление, применение и передача срываются
	bOffering = false;

	ActiveSlot = TargetSlot; // взятый предмет сразу в руки
	Item->SetOwner(this);
	RefreshHeldItem();

	// Голыми руками поднял «комок» — пропах сразу (концепт §15)
	if (Item->ItemEffect == EItemEffect::ThrowBio && VitalsComponent)
	{
		VitalsComponent->AddSmell(40.f);
	}
}

void AAvaryoCharacter::DropItem()
{
	if (!HasAuthority())
	{
		ServerDropItem();
		return;
	}

	// G во время мини-игры — встать/отойти (движение заблокировано, иначе не выйти)
	if (CurrentToilet)
	{
		CurrentToilet->EndUseBy(this);
		CurrentToilet = nullptr;
		return;
	}
	if (CurrentRepairable && CurrentRepairable->IsMinigameRepair())
	{
		CurrentRepairable->EndRepairBy(this);
		CurrentRepairable = nullptr;
		return;
	}

	ReleaseHeldItem(/*bThrown=*/false);
}

void AAvaryoCharacter::ThrowItem()
{
	if (!HasAuthority())
	{
		ServerThrowItem();
		return;
	}
	// В мини-игре/туалете бросок не работает (там G — это «встать»)
	if (CurrentToilet || (CurrentRepairable && CurrentRepairable->IsMinigameRepair()))
	{
		return;
	}
	ReleaseHeldItem(/*bThrown=*/true);
}

void AAvaryoCharacter::ServerThrowItem_Implementation()
{
	ThrowItem();
}

void AAvaryoCharacter::OnThrowKeyPressed()
{
	// В мини-игре/туалете G = выйти/встать (как раньше)
	if (CurrentToilet || (CurrentRepairable && CurrentRepairable->IsMinigameRepair()))
	{
		DropItem();
		return;
	}
	if (!GetHeldItem())
	{
		return; // нечего метать
	}
	bChargingThrow = true;
	ThrowPressTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

void AAvaryoCharacter::OnThrowKeyReleased()
{
	if (!bChargingThrow)
	{
		return;
	}
	const float Alpha = GetThrowChargeAlpha();
	bChargingThrow = false;
	ServerThrowCharged(Alpha);
}

float AAvaryoCharacter::GetThrowChargeAlpha() const
{
	if (!bChargingThrow || !GetWorld())
	{
		return 0.f;
	}
	const float Held = GetWorld()->GetTimeSeconds() - ThrowPressTime;
	return FMath::Clamp((Held - ThrowChargeMinTime) / FMath::Max(0.01f, ThrowChargeMaxTime - ThrowChargeMinTime), 0.f, 1.f);
}

void AAvaryoCharacter::RegisterSelfNoise(float Loudness)
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	SelfNoiseLevel = FMath::Max(GetSelfNoise01(), FMath::Clamp(Loudness, 0.f, 1.f));
	SelfNoiseTime = Now;
}

float AAvaryoCharacter::GetSelfNoise01() const
{
	if (!GetWorld() || SelfNoiseLevel <= 0.f)
	{
		return 0.f;
	}
	const float Age = GetWorld()->GetTimeSeconds() - SelfNoiseTime;
	if (Age <= SelfNoiseHoldTime)
	{
		return SelfNoiseLevel; // короткий пик-холд — даже одиночный блип (икота, батарея) видно на шумомере
	}
	const float Decay = FMath::Clamp(1.f - (Age - SelfNoiseHoldTime) / SelfNoiseDecayTime, 0.f, 1.f);
	return SelfNoiseLevel * Decay;
}

void AAvaryoCharacter::ServerThrowCharged_Implementation(float ChargeAlpha)
{
	if (CurrentToilet || (CurrentRepairable && CurrentRepairable->IsMinigameRepair()))
	{
		return;
	}
	ReleaseHeldItem(/*bThrown=*/true, FMath::Clamp(ChargeAlpha, 0.f, 1.f));
}

void AAvaryoCharacter::ReleaseHeldItem(bool bThrown, float ChargeAlpha)
{
	APickupItem* Item = GetHeldItem();
	if (!Item)
	{
		return;
	}

	StopSpraying();
	CancelUseCast();
	bOffering = false;
	const bool bHeavy = Item->ItemSize == EItemSize::Heavy;

	// Освобождаем слот
	if (ActiveSlot == 0)
	{
		HeavySlot = nullptr;
	}
	else
	{
		LightSlots[ActiveSlot - 1] = nullptr;
	}

	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->SetActorHiddenInGame(false);
	Item->SetOwner(nullptr);

	FVector ViewLoc;
	FRotator ViewRot;
	GetActorEyesViewPoint(ViewLoc, ViewRot);
	FVector AimDir = ViewRot.Vector();

	// Паника = трясущиеся руки: бросок уходит с разбросом
	if (bThrown && VitalsComponent)
	{
		const float Spread01 = FMath::Clamp(VitalsComponent->GetPanic() / 100.f, 0.f, 1.f);
		if (Spread01 > 0.f)
		{
			AimDir = FMath::VRandCone(AimDir, FMath::DegreesToRadians(ThrowMaxSpreadDeg * Spread01));
		}
	}

	const float DropDistance = bHeavy ? 120.f : 150.f;
	const FVector DropLocation = bThrown
		? ViewLoc + AimDir * 60.f // из рук, чтобы летело по прицелу
		: GetActorLocation() + GetActorForwardVector() * DropDistance;
	Item->SetActorLocation(DropLocation, false, nullptr, ETeleportType::TeleportPhysics);
	Item->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));

	Item->SetActorEnableCollision(true);
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Prim->SetSimulatePhysics(true);
		if (bThrown)
		{
			// Бросок: сила базовая (тап) → ×ThrowChargeMaxMult при полной зарядке удержанием
			const float ChargeMult = 1.f + (ThrowChargeMaxMult - 1.f) * FMath::Clamp(ChargeAlpha, 0.f, 1.f);
			const float Power = (bHeavy ? ThrowImpulseHeavy : ThrowImpulseLight) * ChargeMult;
			Prim->AddImpulse(AimDir * Power + FVector(0.f, 0.f, 150.f + 150.f * ChargeAlpha), NAME_None, true);
		}
		else if (!bHeavy)
		{
			// Лёгкий — роняем с лёгким толчком; тяжёлый аккуратно ставим
			Prim->AddImpulse(GetActorForwardVector() * 200.f, NAME_None, true);
		}
	}

	// Бросок и падение слышно; тяжёлый — громче
	const float ReleaseNoise = bThrown ? 0.8f : (bHeavy ? 1.f : 0.6f);
	MakeNoise(ReleaseNoise, this, DropLocation);
	RegisterSelfNoise(ReleaseNoise);
	if (!bThrown && DropSound) // уронил — стук о пол у всех (бросок-импакт ждёт whoosh-пака)
	{
		MulticastSound(DropSound, DropLocation, bHeavy ? 1.f : 0.7f);
	}

	if (bThrown)
	{
		if (ARunState* Run = ARunState::Get(GetWorld()))
		{
			Run->NotifyThrow(this);
		}
	}

	RefreshHeldItem();
}

void AAvaryoCharacter::ServerDropItem_Implementation()
{
	DropItem();
}

void AAvaryoCharacter::EquipSlot(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= NumSlots)
	{
		return;
	}

	// Тик переключения — личный, только тому, кто нажал (2D, без мультикаста)
	if (IsLocallyControlled() && SlotSwitchSound && SlotIndex != ActiveSlot)
	{
		UGameplayStatics::PlaySound2D(this, SlotSwitchSound, 0.5f);
	}

	if (!HasAuthority())
	{
		ServerEquipSlot(SlotIndex);
		return;
	}

	StopSpraying();  // смена предмета прекращает распыление,
	CancelUseCast(); // срывает применение и передачу
	bOffering = false;
	ActiveSlot = SlotIndex;
	RefreshHeldItem();
}

void AAvaryoCharacter::ServerEquipSlot_Implementation(int32 SlotIndex)
{
	EquipSlot(SlotIndex);
}

void AAvaryoCharacter::UseHeldItem()
{
	if (!HasAuthority())
	{
		ServerUseHeldItem();
		return;
	}
	ApplyItemEffect(GetHeldItem());
}

AAvaryoCharacter* AAvaryoCharacter::FindHealTarget() const
{
	// Сначала раненый тиммейт рядом
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		AAvaryoCharacter* Other = *It;
		if (Other != this && Other->VitalsComponent && Other->VitalsComponent->IsWounded()
			&& FVector::DistSquared(Other->GetActorLocation(), GetActorLocation()) < FMath::Square(ReviveRange))
		{
			return Other;
		}
	}
	// Иначе лечим себя, если есть что лечить
	if (VitalsComponent && (VitalsComponent->IsWounded() || VitalsComponent->GetHealth() < 99.f))
	{
		return const_cast<AAvaryoCharacter*>(this);
	}
	return nullptr;
}

bool AAvaryoCharacter::CanApplyEffect(APickupItem* Item) const
{
	if (!Item || !VitalsComponent)
	{
		return false;
	}

	switch (Item->ItemEffect)
	{
	case EItemEffect::Heal:       return FindHealTarget() != nullptr;
	case EItemEffect::Calm:       return VitalsComponent->GetPanic() > 1.f;
	case EItemEffect::Extinguish: return Item->Charges != 0;
	case EItemEffect::Recharge:   return FlashlightComponent && FlashlightComponent->GetBatteryLevel() < 99.f;
	case EItemEffect::DeployTrap: return Item->Charges != 0;
	case EItemEffect::ThrowBio:   return Item->Charges != 0;
	case EItemEffect::DeployLight: return Item->Charges != 0;
	case EItemEffect::Drink:      return VitalsComponent->GetStamina() < 99.f;
	default:                      return false;
	}
}

void AAvaryoCharacter::ApplyItemEffect(APickupItem* Item)
{
	if (!HasAuthority() || !Item || !VitalsComponent)
	{
		return;
	}

	// Звук — только для МГНОВЕННЫХ предметов (у кастовых уже сыграл в начале каста, синхронно)
	if (Item->UseCastTime <= 0.f)
	{
		if (USoundBase* S = ItemUseSoundFor(Item))
		{
			MulticastSound(S, GetActorLocation(), 1.f);
		}
	}

	switch (Item->ItemEffect)
	{
	case EItemEffect::Heal:
	{
		AAvaryoCharacter* Target = FindHealTarget();
		if (!Target)
		{
			return; // все здоровы, заряд не тратим
		}
		const bool bRevive = Target != this && Target->VitalsComponent->IsWounded();
		Target->VitalsComponent->Heal(Item->EffectMagnitude);
		if (bRevive && !Target->VitalsComponent->IsWounded())
		{
			if (ARunState* Run = ARunState::Get(GetWorld()))
			{
				Run->AddRevive(this); // поднял тиммейта — в «Акт»
			}
		}
		ConsumeCharge(Item);
		break;
	}
	case EItemEffect::Calm:
		if (VitalsComponent->GetPanic() > 1.f)
		{
			VitalsComponent->StartSmoking(); // 15 сек по 2 очка паники в секунду
			ConsumeCharge(Item);
		}
		break;
	case EItemEffect::Recharge:
		if (FlashlightComponent && FlashlightComponent->GetBatteryLevel() < 99.f)
		{
			FlashlightComponent->Recharge(Item->EffectMagnitude);
			ConsumeCharge(Item);
		}
		break;
	case EItemEffect::Drink:
		// Кофе/термос: бодрит и успокаивает — но это диуретик, шкала туалета подскочит (комедийный риск)
		VitalsComponent->RestoreStamina(Item->EffectMagnitude > 0.f ? Item->EffectMagnitude : 60.f);
		VitalsComponent->ReducePanic(5.f);
		VitalsComponent->AddBladder(18.f);
		ConsumeCharge(Item);
		if (ARunState* Run = ARunState::Get(GetWorld()))
		{
			Run->NotifyCoffee(this);
		}
		break;
	case EItemEffect::DeployTrap:
	{
		// Ставим растяжку у ног, чуть впереди — взведётся через пару секунд
		const FVector Feet = GetActorLocation() - FVector(0.f, 0.f, GetSimpleCollisionHalfHeight());
		const FVector SpawnLoc = Feet + GetActorForwardVector() * 120.f + FVector(0.f, 0.f, 10.f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (GetWorld()->SpawnActor<ATrap>(ATrap::StaticClass(), SpawnLoc, GetActorRotation(), SpawnParams))
		{
			ConsumeCharge(Item);
		}
		break;
	}
	case EItemEffect::ThrowBio:
	{
		// Метаем комок из камеры по направлению взгляда
		FVector ViewLoc;
		FRotator ViewRot;
		GetActorEyesViewPoint(ViewLoc, ViewRot);
		const FVector Dir = ViewRot.Vector();
		const FVector SpawnLoc = ViewLoc + Dir * 80.f;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (ABioProjectile* Bio = GetWorld()->SpawnActor<ABioProjectile>(ABioProjectile::StaticClass(), SpawnLoc, ViewRot, SpawnParams))
		{
			Bio->Launch(Dir);
			ConsumeCharge(Item);
		}
		break;
	}
	case EItemEffect::DeployLight:
	{
		// Ставим прожектор у ног чуть впереди
		const FVector Feet = GetActorLocation() - FVector(0.f, 0.f, GetSimpleCollisionHalfHeight());
		const FVector SpawnLoc = Feet + GetActorForwardVector() * 120.f + FVector(0.f, 0.f, 10.f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (GetWorld()->SpawnActor<AFloodlight>(AFloodlight::StaticClass(), SpawnLoc, GetActorRotation(), SpawnParams))
		{
			ConsumeCharge(Item);
		}
		break;
	}
	default:
		break;
	}
}

void AAvaryoCharacter::ServerUseHeldItem_Implementation()
{
	UseHeldItem();
}

void AAvaryoCharacter::BeginUseHeldItem()
{
	if (!HasAuthority())
	{
		ServerBeginUseHeldItem();
		return;
	}

	APickupItem* Item = GetHeldItem();
	if (!Item)
	{
		return;
	}

	// Применение отменяет передачу
	if (bOffering)
	{
		bOffering = false;
		RefreshHeldItem();
	}

	if (Item->ItemEffect == EItemEffect::Radio)
	{
		Item->SetToggledOn(!Item->IsToggledOn()); // тумблер: щёлк
		if (RadioToggleSound)
		{
			MulticastSound(RadioToggleSound, GetActorLocation(), 0.7f);
		}
		return;
	}

	if (Item->ItemEffect == EItemEffect::Extinguish)
	{
		// Огнетушитель: распыляем, пока зажата кнопка и есть заряд
		if (Item->Charges != 0)
		{
			bSprayingHeld = true;
			Item->SetSpraying(true);
		}
	}
	else if (Item->ItemEffect != EItemEffect::None)
	{
		if (!CanApplyEffect(Item))
		{
			return; // нечего лечить / нет паники — каст не начинаем
		}

		if (Item->UseCastTime > 0.f)
		{
			// Применение со временем: держи кнопку. Паника = трясущиеся руки → дольше возишься.
			const float Panic01 = VitalsComponent ? FMath::Clamp(VitalsComponent->GetPanic() / 100.f, 0.f, 1.f) : 0.f;
			const float CastTime = Item->UseCastTime * (1.f + Panic01 * ItemUsePanicScale);
			UseCastRemaining = CastTime;
			UseCastDuration = CastTime;
			// Звук каста ведёт UseCastAudio в Tick (играет всё время каста, гаснет при отмене)
		}
		else
		{
			ApplyItemEffect(Item); // мгновенный эффект
		}
	}
}

void AAvaryoCharacter::ServerBeginUseHeldItem_Implementation()
{
	BeginUseHeldItem();
}

void AAvaryoCharacter::EndUseHeldItem()
{
	if (!HasAuthority())
	{
		ServerEndUseHeldItem();
		return;
	}
	StopSpraying();
	CancelUseCast(); // отпустил кнопку — применение сорвано
}

void AAvaryoCharacter::CancelUseCast()
{
	UseCastRemaining = 0.f;
	UseCastDuration = 0.f;
}

void AAvaryoCharacter::BeginOfferItem()
{
	if (!HasAuthority())
	{
		ServerBeginOfferItem();
		return;
	}

	if (!GetHeldItem())
	{
		return; // нечего передавать
	}

	StopSpraying();
	CancelUseCast();
	bOffering = true;
	RefreshHeldItem();
}

void AAvaryoCharacter::ServerBeginOfferItem_Implementation()
{
	BeginOfferItem();
}

void AAvaryoCharacter::EndOfferItem()
{
	if (!HasAuthority())
	{
		ServerEndOfferItem();
		return;
	}

	if (bOffering)
	{
		bOffering = false;
		RefreshHeldItem();
	}
}

void AAvaryoCharacter::ServerEndOfferItem_Implementation()
{
	EndOfferItem();
}

void AAvaryoCharacter::OnRep_Offering()
{
	RefreshHeldItem();
}

void AAvaryoCharacter::TickUseCast(float DeltaSeconds)
{
	APickupItem* Item = GetHeldItem();
	if (!Item || Item->ItemEffect == EItemEffect::None || Item->ItemEffect == EItemEffect::Extinguish)
	{
		CancelUseCast();
		return;
	}

	UseCastRemaining -= DeltaSeconds;
	if (UseCastRemaining <= 0.f)
	{
		CancelUseCast();
		ApplyItemEffect(Item); // применение завершено
	}
}

void AAvaryoCharacter::ServerEndUseHeldItem_Implementation()
{
	EndUseHeldItem();
}

void AAvaryoCharacter::StopSpraying()
{
	bSprayingHeld = false;
	SprayDrainAccum = 0.f;
	SprayNoiseAccum = 0.f; // ровная каденция шума/пены при повторном распылении
	SprayFoamAccum = 0.f;
	for (int32 SlotIndex = 0; SlotIndex < NumSlots; ++SlotIndex)
	{
		APickupItem* Item = GetItemInSlot(SlotIndex);
		if (Item && Item->IsSpraying())
		{
			Item->SetSpraying(false);
		}
	}
}

void AAvaryoCharacter::TickSpray(float DeltaSeconds)
{
	APickupItem* Item = GetHeldItem();
	if (!Item || Item->ItemEffect != EItemEffect::Extinguish || Item->Charges <= 0)
	{
		StopSpraying();
		return;
	}

	// Расход заряда: ~12 единиц в секунду (баллона на 100 хватает на ~8 сек)
	SprayDrainAccum += 12.f * DeltaSeconds;
	while (SprayDrainAccum >= 1.f && Item->Charges > 0)
	{
		Item->Charges--;
		SprayDrainAccum -= 1.f;
	}
	if (Item->Charges <= 0)
	{
		Item->Charges = 0; // пустой баллон остаётся в руках, не исчезает
		StopSpraying();
		return;
	}

	if (VitalsComponent)
	{
		VitalsComponent->AddSmell(6.f * DeltaSeconds); // облако порошка — сам пропах химией
	}

	FVector ViewLoc;
	FRotator ViewRot;
	GetActorEyesViewPoint(ViewLoc, ViewRot);
	const FVector Dir = ViewRot.Vector();

	// Облако отталкивает физические предметы.
	// TODO (монстр-слухач): здесь же слепить/замедлять монстра на 2-5 сек.
	TArray<FOverlapResult> Overlaps;
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	FCollisionQueryParams QueryParams(FName(TEXT("SprayPush")), false, this);
	GetWorld()->OverlapMultiByObjectType(Overlaps, ViewLoc + Dir * 180.f, FQuat::Identity, ObjParams, FCollisionShape::MakeSphere(160.f), QueryParams);
	for (const FOverlapResult& Overlap : Overlaps)
	{
		UPrimitiveComponent* Prim = Overlap.GetComponent();
		if (Prim && Prim->IsSimulatingPhysics() && Overlap.GetActor() != this)
		{
			Prim->AddImpulse(Dir * 500.f * DeltaSeconds, NAME_None, true);
		}
	}

	// Огнетушитель громкий — задел под монстра-слухача
	SprayNoiseAccum += DeltaSeconds;
	if (SprayNoiseAccum >= 0.4f)
	{
		SprayNoiseAccum = 0.f;
		MakeNoise(1.f, this, GetActorLocation());
	}

	// Порошок оседает скользкой пеной: раз в ~0.6 с роняем лужу на пол перед собой
	SprayFoamAccum += DeltaSeconds;
	if (SprayFoamAccum >= 0.6f)
	{
		SprayFoamAccum = 0.f;
		const FVector AheadFlat = GetActorLocation() + FVector(Dir.X, Dir.Y, 0.f).GetSafeNormal() * 160.f;
		FHitResult FloorHit;
		const FVector TraceStart = AheadFlat + FVector(0.f, 0.f, 60.f);
		const FVector TraceEnd = AheadFlat - FVector(0.f, 0.f, 260.f);
		if (GetWorld()->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AFoamPatch>(AFoamPatch::StaticClass(), FloorHit.Location + FVector(0.f, 0.f, 2.f), FRotator::ZeroRotator, SpawnParams);
		}
	}

	// Пеной можно сбить газовое облако: пока дуешь в зону утечки, поджечь нельзя
	const FVector SprayPoint = ViewLoc + Dir * 200.f;
	for (TActorIterator<ARepairable> RepIt(GetWorld()); RepIt; ++RepIt)
	{
		if (RepIt->IsLeakingGas()
			&& FVector::DistSquared(RepIt->GetActorLocation(), SprayPoint) <= FMath::Square(450.f))
		{
			RepIt->SuppressGas(2.0f); // держится пару секунд после струи
		}
	}
}

void AAvaryoCharacter::UpdateFoamSlip(float DeltaSeconds)
{
	bool bOverFoam = false;
	const FVector Loc = GetActorLocation();
	for (TActorIterator<AFoamPatch> It(GetWorld()); It; ++It)
	{
		const FVector P = It->GetActorLocation();
		const float Dz = Loc.Z - P.Z;
		if (Dz < -20.f || Dz > 160.f)
		{
			continue; // лужа на другом уровне (этаж/над головой) — не считаем
		}
		if (FVector::DistSquaredXY(Loc, P) <= FMath::Square(It->SlipRadius))
		{
			bOverFoam = true;
			break;
		}
	}

	if (bOverFoam != bSlipping)
	{
		bSlipping = bOverFoam;        // реплицируется -> OnRep_Slipping на клиентах
		ApplySlipFriction(bSlipping); // и сразу применяем на сервере
		if (bSlipping)
		{
			if (VitalsComponent)
			{
				VitalsComponent->AddPanic(FoamSlipPanic); // «ой!» — небольшой испуг
			}
			if (ARunState* Run = ARunState::Get(GetWorld()))
			{
				Run->NotifySlipped(this); // диспетчер прокомментирует (неважная — анти-спам глотает)
			}
		}
	}

	// Быстро катишься по пене — можно и навернуться (стан)
	if (bSlipping && !bStumbling)
	{
		UCharacterMovementComponent* Move = GetCharacterMovement();
		if (Move && Move->Velocity.SizeSquared2D() > FMath::Square(350.f)
			&& FMath::FRand() < FoamFallChancePerSecond * DeltaSeconds)
		{
			TriggerStumble();
		}
	}
}

void AAvaryoCharacter::ApplySlipFriction(bool bOn)
{
	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (!Move)
	{
		return;
	}
	if (!bSlipDefaultsSaved)
	{
		SlipDefaultGroundFriction = Move->GroundFriction;
		SlipDefaultBrakingDecel = Move->BrakingDecelerationWalking;
		bSlipDefaultsSaved = true;
	}
	if (bOn)
	{
		Move->GroundFriction = 0.4f;              // почти лёд
		Move->BrakingDecelerationWalking = 120.f; // тормозить нечем — катится
	}
	else
	{
		Move->GroundFriction = SlipDefaultGroundFriction;
		Move->BrakingDecelerationWalking = SlipDefaultBrakingDecel;
	}
}

void AAvaryoCharacter::OnRep_Slipping()
{
	ApplySlipFriction(bSlipping); // клиент тоже снижает/возвращает трение — чтобы скольжение совпало
}

void AAvaryoCharacter::UpdateTrip(float DeltaSeconds)
{
	const float Now = GetWorld()->GetTimeSeconds();
	if (bStumbling)
	{
		if (Now >= StumbleUntil)
		{
			bStumbling = false; // отпустило — RefreshMoveSpeed вернёт ход
		}
		return;
	}

	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (!Move || !VitalsComponent)
	{
		return;
	}

	// Спотыкаемся только на бегу по земле, не раненым
	if (VitalsComponent->IsWounded()
		|| !VitalsComponent->IsSprinting()
		|| !Move->IsMovingOnGround()
		|| Move->Velocity.SizeSquared2D() < FMath::Square(300.f))
	{
		return;
	}

	// Споткнуться о лежащего раненого товарища прямо по курсу — гарантированно
	const FVector Loc = GetActorLocation();
	const FVector Fwd = GetActorForwardVector();
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		if (*It == this || !It->VitalsComponent || !It->VitalsComponent->IsWounded())
		{
			continue;
		}
		const FVector To = It->GetActorLocation() - Loc;
		if (To.SizeSquared() <= FMath::Square(120.f) && FVector::DotProduct(To.GetSafeNormal(), Fwd) > 0.2f)
		{
			TriggerStumble(); // влетел в лежащего — оба в осадке
			return;
		}
	}

	// Споткнуться о брошенный на полу хлам (своя же выроненная канистра/ключ) прямо по курсу
	for (TActorIterator<APickupItem> It(GetWorld()); It; ++It)
	{
		UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(It->GetRootComponent());
		if (!Prim || !Prim->IsSimulatingPhysics()) // лежит на полу (не в руках/не закреплён)
		{
			continue;
		}
		const FVector To = It->GetActorLocation() - Loc;
		if (To.Z < 30.f && To.SizeSquared() <= FMath::Square(100.f)
			&& FVector::DotProduct(To.GetSafeNormal(), Fwd) > 0.25f)
		{
			TriggerStumble(); // насорил — сам и влетел
			return;
		}
	}

	float Mult = 1.f;
	if (FlashlightComponent && !FlashlightComponent->IsOn())
	{
		Mult *= TripDarkMultiplier; // в темноте не видно, обо что споткнуться
	}
	Mult *= 1.f + (VitalsComponent->GetPanic() / 100.f) * TripPanicMultiplier; // паника — суетятся ноги
	Mult *= 1.f + (1.f - VitalsComponent->GetStamina() / 100.f) * TripTiredScale; // устал — заплетаются ноги

	if (FMath::FRand() < TripChancePerSecond * DeltaSeconds * Mult)
	{
		TriggerStumble();
	}
}

void AAvaryoCharacter::TriggerStumble()
{
	bStumbling = true;
	StumbleUntil = GetWorld()->GetTimeSeconds() + TripRecoverTime;
	MakeNoise(0.7f, this, GetActorLocation()); // грохнулся — слышно
	RegisterSelfNoise(0.7f);
	if (FallSound) { MulticastSound(FallSound, GetActorLocation(), 0.9f); } // удар тела о пол — у всех
	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->NotifyTripped(this);
		Run->AddTrip(this); // счётчик «споткнулся» — в «Акт»
	}
	// При падении можно выронить активный ЛЁГКИЙ предмет (тяжёлый и так роняется при ранении)
	APickupItem* Held = GetHeldItem();
	if (Held && Held->ItemSize == EItemSize::Light
		&& !CurrentRepairable && !CurrentToilet
		&& FMath::FRand() < TripFumbleChance)
	{
		DropItem(); // выронил из рук — катится по полу
	}
}

void AAvaryoCharacter::Shove()
{
	if (!HasAuthority())
	{
		ServerShove();
		return;
	}
	ServerShove_Implementation();
}

void AAvaryoCharacter::MulticastSound_Implementation(USoundBase* Sound, FVector Loc, float Vol)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, Loc, Vol);
	}
}

USoundBase* AAvaryoCharacter::ItemUseSoundFor(const APickupItem* Item) const
{
	if (Item)
	{
		if (Item->ItemEffect == EItemEffect::Heal && HealSound) { return HealSound; }
		if (Item->ItemEffect == EItemEffect::Calm && SmokeSound) { return SmokeSound; }
		if (Item->ItemEffect == EItemEffect::Drink) { return nullptr; } // кофе: нет нормального глотка, звук пока выкл
	}
	return UseSound;
}

void AAvaryoCharacter::ServerShove_Implementation()
{
	// Нельзя толкать, пока сам ранен или залочен в мини-игре
	if ((VitalsComponent && VitalsComponent->IsWounded()) || bInteractionLocked)
	{
		return;
	}
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now < ShoveReadyTime)
	{
		return; // перезарядка
	}
	ShoveReadyTime = Now + ShoveCooldownTime;

	if (ShoveSound) // глухой удар толчка — у всех
	{
		MulticastSound(ShoveSound, GetActorLocation(), 1.f);
	}

	FVector ViewLoc;
	FRotator ViewRot;
	GetActorEyesViewPoint(ViewLoc, ViewRot);
	const FVector Dir = ViewRot.Vector().GetSafeNormal();
	const FVector FlatDir = FVector(Dir.X, Dir.Y, 0.f).GetSafeNormal();

	// Ищем ближайшего товарища в конусе перед собой
	AAvaryoCharacter* Target = nullptr;
	float BestDistSq = FMath::Square(ShoveRange);
	const FVector MyLoc = GetActorLocation();
	for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
	{
		AAvaryoCharacter* Other = *It;
		if (Other == this)
		{
			continue;
		}
		const FVector To = Other->GetActorLocation() - MyLoc;
		const float DistSq = To.SizeSquared();
		if (DistSq > BestDistSq)
		{
			continue;
		}
		if (FVector::DotProduct(To.GetSafeNormal(), Dir) < 0.35f) // ~70° конус
		{
			continue;
		}
		BestDistSq = DistSq;
		Target = Other;
	}

	MakeNoise(0.5f, this, MyLoc); // кряхтение/возня — слышно

	if (!Target)
	{
		return;
	}

	const FVector Push = FlatDir * ShoveForce + FVector(0.f, 0.f, ShoveUp);
	Target->LaunchCharacter(Push, true, true); // реплицируется владельцу
	if (Target->VitalsComponent)
	{
		Target->VitalsComponent->AddPanic(ShovePanic);
	}
	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		Run->NotifyShoved(Target);
		Run->AddShove(this); // счётчик «толкнул» — в «Акт»
	}
	if (FMath::FRand() < ShoveFumbleChance)
	{
		Target->FumbleHeavy(); // от толчка можно выронить сварочник
	}
}

void AAvaryoCharacter::FumbleHeavy()
{
	if (!HasAuthority() || CurrentRepairable || CurrentToilet)
	{
		return; // в мини-игре ничего не выбиваем (DropItem там — выход из мини-игры)
	}
	if (DraggedTeammate)
	{
		ReleaseDraggedTeammate(); // от удара выпустил несомого раненого
	}
	if (HeavySlot)
	{
		ActiveSlot = 0;  // сделать тяжёлый активным и уронить через общий путь
		DropItem();
		if (FMath::FRand() < HeavyBonkChance)
		{
			// Уронил себе на ногу — больно и громко
			TakeDamage(HeavyBonkDamage, FDamageEvent(), GetController(), this);
			MakeNoise(0.6f, this, GetActorLocation());
		}
	}
}



void AAvaryoCharacter::StartCrouchInput()
{
	Crouch();
}

void AAvaryoCharacter::StopCrouchInput()
{
	UnCrouch();
}

void AAvaryoCharacter::ConsumeHeldItemCharge()
{
	if (!HasAuthority())
	{
		return;
	}
	if (APickupItem* Held = GetHeldItem())
	{
		ConsumeCharge(Held);
	}
}

void AAvaryoCharacter::ConsumeCharge(APickupItem* Item)
{
	if (Item->Charges < 0)
	{
		return; // бесконечный предмет
	}

	Item->Charges--;
	if (Item->Charges <= 0)
	{
		ClearSlotFor(Item);
		Item->Destroy();
		RefreshHeldItem();
	}
}

void AAvaryoCharacter::ClearSlotFor(APickupItem* Item)
{
	if (HeavySlot == Item)
	{
		HeavySlot = nullptr;
	}
	for (int32 i = 0; i < NumLightSlots; ++i)
	{
		if (LightSlots[i] == Item)
		{
			LightSlots[i] = nullptr;
		}
	}
}

void AAvaryoCharacter::OnRep_Inventory()
{
	// Состояние пришло с сервера — обновляем визуал на клиенте
	RefreshHeldItem();
}

void AAvaryoCharacter::RefreshHeldItem()
{
	APickupItem* Held = GetHeldItem();
	for (int32 SlotIndex = 0; SlotIndex < NumSlots; ++SlotIndex)
	{
		APickupItem* Item = GetItemInSlot(SlotIndex);
		if (!Item)
		{
			continue;
		}

		if (Item == Held)
		{
			HoldItem(Item);
		}
		else if (SlotIndex == 0)
		{
			// Правило сварочника: тяжёлый не убирается в рюкзак — он опущен, но виден
			CarryHeavyLowered(Item);
		}
		else
		{
			StashItem(Item);
		}
	}
}

void AAvaryoCharacter::HoldItem(APickupItem* Item)
{
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		Prim->SetSimulatePhysics(false);
	}

	// Крепим к камере, чтобы предмет следовал за взглядом
	USceneComponent* HoldParent = ViewCamera ? static_cast<USceneComponent*>(ViewCamera) : GetRootComponent();
	Item->AttachToComponent(HoldParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	if (bOffering)
	{
		// Передача: предмет вытянут вперёд по центру, коллизия включена (QueryOnly),
		// чтобы тиммейт мог навестись на него и забрать по E
		Item->SetActorRelativeLocation(FVector(90.f, 0.f, -18.f));
		Item->SetActorEnableCollision(true);
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
		{
			Prim->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		Item->SetActorRelativeLocation(Item->HoldOffset);
		Item->SetActorEnableCollision(false);
	}

	Item->SetActorRelativeRotation(Item->HoldRotation);
	Item->SetActorHiddenInGame(false);
}

void AAvaryoCharacter::CarryHeavyLowered(APickupItem* Item)
{
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		Prim->SetSimulatePhysics(false);
	}
	Item->SetActorEnableCollision(false);

	// К капсуле (не к камере) — опущенный груз не дёргается от взгляда
	Item->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Item->SetActorRelativeLocation(Item->CarryOffset);
	Item->SetActorRelativeRotation(FRotator::ZeroRotator);
	Item->SetActorHiddenInGame(false);
}

void AAvaryoCharacter::StashItem(APickupItem* Item)
{
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		Prim->SetSimulatePhysics(false);
	}
	Item->SetActorEnableCollision(false);

	// Лёгкие неактивные предметы — "в карманах": прицеплены и скрыты
	Item->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Item->SetActorHiddenInGame(true);
}
