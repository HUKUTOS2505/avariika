#include "AvaryoCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/UFlashlightComponent.h"
#include "Components/VitalsComponent.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputCoreTypes.h"
#include "Items/APickupItem.h"
#include "Net/UnrealNetwork.h"

AAvaryoCharacter::AAvaryoCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // фокус предмета + пересчёт скорости
	bReplicates = true;

	// Налобный фонарик. Сам свет (SpotLight) добавляется в Blueprint
	// и назначается в FlashlightComponent->AttachedLight (или найдётся автоматически).
	FlashlightComponent = CreateDefaultSubobject<UFlashlightComponent>(TEXT("Flashlight"));

	// Шкалы игрока
	VitalsComponent = CreateDefaultSubobject<UVitalsComponent>(TEXT("Vitals"));

	LightSlots.SetNum(NumLightSlots);
	ActiveSlot = 0;
	PickupRange = 350.f;
	ReviveRange = 250.f;

	BaseWalkSpeed = 500.f;
	SprintSpeed = 750.f;
	HeavyCarryMultiplier = 0.65f;
	CrawlSpeed = 120.f;
	IncidentSlowMultiplier = 0.7f;

	bWasWounded = false;
	bSprayingHeld = false;
	SprayDrainAccum = 0.f;
	SprayNoiseAccum = 0.f;
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
}

void AAvaryoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Подсказка "[E] Подобрать" нужна только локальному игроку
	if (IsLocallyControlled())
	{
		FocusedItem = FindFocusedItem();
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

	// Применение предмета (аптечка 6 сек, сигарета 2 сек)
	if (HasAuthority() && UseCastRemaining > 0.f)
	{
		TickUseCast(DeltaSeconds);
	}
}

void AAvaryoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Временные хардкод-бинды для теста; позже заменим на Enhanced Input
	PlayerInputComponent->BindKey(EKeys::F, IE_Pressed, this, &AAvaryoCharacter::ToggleFlashlight);
	PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AAvaryoCharacter::TryPickupNearbyItem);
	PlayerInputComponent->BindKey(EKeys::G, IE_Pressed, this, &AAvaryoCharacter::DropItem);
	// Использование: ЛКМ или R (нажал — эффект/распыление, отпустил — конец распыления)
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AAvaryoCharacter::BeginUseHeldItem);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AAvaryoCharacter::EndUseHeldItem);
	PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AAvaryoCharacter::BeginUseHeldItem);
	PlayerInputComponent->BindKey(EKeys::R, IE_Released, this, &AAvaryoCharacter::EndUseHeldItem);
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
	if (VitalsComponent->IsIncidentSlowed())
	{
		Speed *= IncidentSlowMultiplier;
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
}

void AAvaryoCharacter::DropItem()
{
	if (!HasAuthority())
	{
		ServerDropItem();
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
		Target->VitalsComponent->Heal(Item->EffectMagnitude);
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
