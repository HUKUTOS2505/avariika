#include "AvaryoCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/UFlashlightComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "World/AExitZone.h"
#include "Components/VitalsComponent.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputCoreTypes.h"
#include "Game/ARunState.h"
#include "Items/ABioPickup.h"
#include "Items/APickupItem.h"
#include "Net/UnrealNetwork.h"
#include "UI/AvaryoCameraShakes.h"
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

	bStumbling = false;
	TripChancePerSecond = 0.03f;
	TripDarkMultiplier = 2.5f;
	TripPanicMultiplier = 1.5f;
	TripRecoverTime = 0.8f;
	TripSlowSpeed = 150.f;
	StumbleUntil = 0.f;
	UseCastRemaining = 0.f;
	UseCastDuration = 0.f;
	bOffering = false;

	// Приседание (Ctrl/C) — пригодится против монстра-слухача
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 250.f;
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
		const bool bPanicNow = VitalsComponent && VitalsComponent->IsPanicking();
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
		UpdateFoamSlip();
		UpdateTrip(DeltaSeconds);
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
}

void AAvaryoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Временные хардкод-бинды для теста; позже заменим на Enhanced Input
	PlayerInputComponent->BindKey(EKeys::F, IE_Pressed, this, &AAvaryoCharacter::ToggleFlashlight);
	PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AAvaryoCharacter::OnInteractPressed);
	PlayerInputComponent->BindKey(EKeys::E, IE_Released, this, &AAvaryoCharacter::OnInteractReleased);
	PlayerInputComponent->BindKey(EKeys::G, IE_Pressed, this, &AAvaryoCharacter::DropItem);
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
	if (CurrentRepairable && CurrentRepairable->IsMinigameRepair() && !CurrentRepairable->IsBotching())
	{
		CurrentRepairable->TryHitBy(this);
		return;
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
	// Мини-игры НЕ завершаются отпусканием E (выход по G), но стартеру важен момент отпускания.
	// Колхоз — обычное удержание E, отпускание его завершает (ниже).
	if (CurrentRepairable && CurrentRepairable->IsMinigameRepair() && !CurrentRepairable->IsBotching())
	{
		CurrentRepairable->TryReleaseBy(this);
		return;
	}

	// Обычная починка (удержание E) — завершается
	if (CurrentRepairable)
	{
		CurrentRepairable->EndRepairBy(this);
		CurrentRepairable = nullptr;
	}
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

	const float DropDistance = bHeavy ? 120.f : 150.f;
	const FVector DropLocation = GetActorLocation() + GetActorForwardVector() * DropDistance;
	Item->SetActorLocation(DropLocation, false, nullptr, ETeleportType::TeleportPhysics);
	Item->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));

	Item->SetActorEnableCollision(true);
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Prim->SetSimulatePhysics(true);
		if (!bHeavy)
		{
			// Лёгкий — бросаем; тяжёлый аккуратно ставим (без импульса)
			Prim->AddImpulse(GetActorForwardVector() * 200.f, NAME_None, true);
		}
	}

	// Падение предмета слышно; тяжёлый — громче
	MakeNoise(bHeavy ? 1.f : 0.6f, this, DropLocation);

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
	default:                      return false;
	}
}

void AAvaryoCharacter::ApplyItemEffect(APickupItem* Item)
{
	if (!HasAuthority() || !Item || !VitalsComponent)
	{
		return;
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
			// Применение со временем: держи кнопку (аптечка 6 сек, сигарета 2 сек)
			UseCastRemaining = Item->UseCastTime;
			UseCastDuration = Item->UseCastTime;
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
}

void AAvaryoCharacter::UpdateFoamSlip()
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
			if (ARunState* Run = ARunState::Get(GetWorld()))
			{
				Run->NotifySlipped(this); // диспетчер прокомментирует (неважная — анти-спам глотает)
			}
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

	float Mult = 1.f;
	if (FlashlightComponent && !FlashlightComponent->IsOn())
	{
		Mult *= TripDarkMultiplier; // в темноте не видно, обо что споткнуться
	}
	Mult *= 1.f + (VitalsComponent->GetPanic() / 100.f) * TripPanicMultiplier; // паника — суетятся ноги

	if (FMath::FRand() < TripChancePerSecond * DeltaSeconds * Mult)
	{
		bStumbling = true;
		StumbleUntil = Now + TripRecoverTime;
		MakeNoise(0.7f, this, GetActorLocation()); // грохнулся — слышно
		if (ARunState* Run = ARunState::Get(GetWorld()))
		{
			Run->NotifyTripped(this);
		}
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

	Item->SetActorRelativeRotation(FRotator::ZeroRotator);
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
