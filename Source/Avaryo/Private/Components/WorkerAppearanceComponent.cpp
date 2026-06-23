#include "Components/WorkerAppearanceComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

// ---- Курированные пути паком Modular Workers (Quantum, UE5-mann). См. MODULAR_WORKER_PLAN.md ----
namespace AvWorkerAssets
{
	// База («бомж»)
	static const TCHAR* Body  = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Body_Full.SKM_Quantum_Body_Full");
	static const TCHAR* Head  = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Head.SKM_Quantum_Head");
	static const TCHAR* Feet  = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Feet.SKM_Quantum_Feet");
	static const TCHAR* Hair  = TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Short.SKM_Hair_Short");
	static const TCHAR* Tshirt= TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Bege.SKM_TShirt_Bege");
	static const TCHAR* Jeans = TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Blue.SKM_Jeans_Blue");
	// Снаряжение (косметика=функция). Каска оранжевая — акцентный цвет проекта.
	static const TCHAR* Helmet     = TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Orange.SKM_Helmet_Worker_Orange");
	static const TCHAR* Respirator = TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Respirator/SKM_Respirator.SKM_Respirator");
	static const TCHAR* Gloves     = TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker/SKM_Gloves_Worker.SKM_Gloves_Worker");

	static TSoftObjectPtr<USkeletalMesh> Soft(const TCHAR* Path)
	{
		return TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Path));
	}
}

// =================== FWorkerAppearance ===================

void FWorkerAppearance::Set(EWorkerSlot Slot, const TSoftObjectPtr<USkeletalMesh>& Mesh)
{
	if (Mesh.IsNull())
	{
		Clear(Slot);
		return;
	}
	for (FWorkerSlotMesh& S : Slots)
	{
		if (S.Slot == Slot) { S.Mesh = Mesh; return; }
	}
	Slots.Emplace(Slot, Mesh);
}

void FWorkerAppearance::Clear(EWorkerSlot Slot)
{
	Slots.RemoveAll([Slot](const FWorkerSlotMesh& S){ return S.Slot == Slot; });
}

TSoftObjectPtr<USkeletalMesh> FWorkerAppearance::Get(EWorkerSlot Slot) const
{
	for (const FWorkerSlotMesh& S : Slots)
	{
		if (S.Slot == Slot) return S.Mesh;
	}
	return TSoftObjectPtr<USkeletalMesh>();
}

// =================== UWorkerAppearanceComponent ===================

UWorkerAppearanceComponent::UWorkerAppearanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UWorkerAppearanceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWorkerAppearanceComponent, Appearance);
}

void UWorkerAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();
	// Если внешность пришла заранее (сейв/реплика до BeginPlay) — собрать.
	if (Appearance.Slots.Num() > 0)
	{
		RebuildVisuals();
	}
}

bool IsAuth(const UActorComponent* C)
{
	const AActor* O = C ? C->GetOwner() : nullptr;
	return O && O->HasAuthority();
}

void UWorkerAppearanceComponent::SetSlotMesh(EWorkerSlot Slot, USkeletalMesh* Mesh)
{
	if (!IsAuth(this)) return;
	Appearance.Set(Slot, TSoftObjectPtr<USkeletalMesh>(Mesh));
	RebuildVisuals();
}

void UWorkerAppearanceComponent::ClearSlot(EWorkerSlot Slot)
{
	if (!IsAuth(this)) return;
	Appearance.Clear(Slot);
	RebuildVisuals();
}

void UWorkerAppearanceComponent::ApplyAppearance(const FWorkerAppearance& NewAppearance)
{
	if (!IsAuth(this)) return;
	Appearance = NewAppearance;
	RebuildVisuals();
}

void UWorkerAppearanceComponent::ClearAll()
{
	if (!IsAuth(this)) return;
	Appearance.Slots.Reset();
	RebuildVisuals();
}

void UWorkerAppearanceComponent::ApplyDefaultPreset()
{
	if (!IsAuth(this)) return;
	FWorkerAppearance A;
	A.Set(EWorkerSlot::Body,  AvWorkerAssets::Soft(AvWorkerAssets::Body));
	A.Set(EWorkerSlot::Head,  AvWorkerAssets::Soft(AvWorkerAssets::Head));
	A.Set(EWorkerSlot::Feet,  AvWorkerAssets::Soft(AvWorkerAssets::Feet));
	A.Set(EWorkerSlot::Hair,  AvWorkerAssets::Soft(AvWorkerAssets::Hair));
	A.Set(EWorkerSlot::Torso, AvWorkerAssets::Soft(AvWorkerAssets::Tshirt));
	A.Set(EWorkerSlot::Legs,  AvWorkerAssets::Soft(AvWorkerAssets::Jeans));
	ApplyAppearance(A);
}

void UWorkerAppearanceComponent::ApplyEquipmentFlags(bool bHelmet, bool bGasMask, bool bGloves)
{
	if (!IsAuth(this)) return;
	if (bHelmet)  Appearance.Set(EWorkerSlot::Headgear, AvWorkerAssets::Soft(AvWorkerAssets::Helmet));
	else          Appearance.Clear(EWorkerSlot::Headgear);
	if (bGasMask) Appearance.Set(EWorkerSlot::FaceMask, AvWorkerAssets::Soft(AvWorkerAssets::Respirator));
	else          Appearance.Clear(EWorkerSlot::FaceMask);
	if (bGloves)  Appearance.Set(EWorkerSlot::Gloves,   AvWorkerAssets::Soft(AvWorkerAssets::Gloves));
	else          Appearance.Clear(EWorkerSlot::Gloves);
	RebuildVisuals();
}

void UWorkerAppearanceComponent::OnRep_Appearance()
{
	RebuildVisuals();
}

USkeletalMeshComponent* UWorkerAppearanceComponent::GetBodyComponent() const
{
	const TObjectPtr<USkeletalMeshComponent>* Found = SlotComps.Find(EWorkerSlot::Body);
	return Found ? *Found : nullptr;
}

USkeletalMeshComponent* UWorkerAppearanceComponent::GetOrCreateSlotComp(EWorkerSlot Slot)
{
	if (TObjectPtr<USkeletalMeshComponent>* Found = SlotComps.Find(Slot))
	{
		if (*Found) return *Found;
		SlotComps.Remove(Slot);
	}
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;
	USkeletalMeshComponent* Comp = NewObject<USkeletalMeshComponent>(Owner);
	if (!Comp) return nullptr;
	Comp->SetIsReplicated(false); // визуал локальный; реплицируется только FWorkerAppearance
	Comp->RegisterComponent();
	SlotComps.Add(Slot, Comp);
	return Comp;
}

void UWorkerAppearanceComponent::AttachAsLeaderFollower(USkeletalMeshComponent* Part, USkeletalMeshComponent* Body)
{
	if (!Part || !Body) return;
	Part->AttachToComponent(Body, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Part->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	Part->SetLeaderPoseComponent(Body); // общий скелет → кости 1:1
	Part->SetVisibility(true, true);
}

void UWorkerAppearanceComponent::RebuildVisuals()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// На выделенном сервере визуал не нужен — внешность всё равно реплицирована.
	if (GetNetMode() == NM_DedicatedServer) return;

	USceneComponent* Root = Owner->GetRootComponent();
	if (!Root) return;

	// 1) Тело — лидер позы. Создаём/обновляем первым.
	USkeletalMeshComponent* Body = nullptr;
	const TSoftObjectPtr<USkeletalMesh> BodySoft = Appearance.Get(EWorkerSlot::Body);
	if (!BodySoft.IsNull())
	{
		Body = GetOrCreateSlotComp(EWorkerSlot::Body);
		if (Body)
		{
			if (USkeletalMesh* M = BodySoft.LoadSynchronous())
			{
				Body->SetSkeletalMeshAsset(M);
			}
			Body->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			float HalfH = 88.f;
			if (const ACharacter* Ch = Cast<ACharacter>(Owner))
			{
				if (Ch->GetCapsuleComponent())
				{
					HalfH = Ch->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				}
			}
			Body->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -HalfH), FRotator(0.f, -90.f, 0.f));
			Body->SetVisibility(true, true);
		}
	}

	// 2) Остальные слоты — следуют позе тела.
	TSet<EWorkerSlot> Present;
	Present.Add(EWorkerSlot::Body);
	for (const FWorkerSlotMesh& S : Appearance.Slots)
	{
		if (S.Slot == EWorkerSlot::Body) continue;
		Present.Add(S.Slot);
		USkeletalMeshComponent* Part = GetOrCreateSlotComp(S.Slot);
		if (!Part) continue;
		if (USkeletalMesh* M = S.Mesh.LoadSynchronous())
		{
			Part->SetSkeletalMeshAsset(M);
		}
		if (Body)
		{
			AttachAsLeaderFollower(Part, Body);
		}
		else
		{
			// Без тела-лидера часть просто крепится к корню (ref-поза) — деградация, не краш.
			Part->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Part->SetVisibility(true, true);
		}
	}

	// 3) Снести компоненты слотов, которых больше нет во внешности.
	TArray<EWorkerSlot> ToRemove;
	for (const TPair<EWorkerSlot, TObjectPtr<USkeletalMeshComponent>>& Pair : SlotComps)
	{
		if (!Present.Contains(Pair.Key))
		{
			ToRemove.Add(Pair.Key);
		}
	}
	for (EWorkerSlot Slot : ToRemove)
	{
		if (TObjectPtr<USkeletalMeshComponent>* Found = SlotComps.Find(Slot))
		{
			if (*Found)
			{
				(*Found)->DestroyComponent();
			}
			SlotComps.Remove(Slot);
		}
	}
}
