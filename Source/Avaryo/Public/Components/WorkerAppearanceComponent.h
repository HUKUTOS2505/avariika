#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorkerAppearanceComponent.generated.h"

class USkeletalMeshComponent;
class USkeletalMesh;

/**
 * Слоты модульного рабочего (пак Modular Workers / Quantum, скелет UE5-mann).
 * Body — ведущий меш: остальные части копируют его позу через SetLeaderPoseComponent.
 * См. MODULAR_WORKER_PLAN.md (Ф3) и память [[modular-worker-integration]].
 */
UENUM(BlueprintType)
enum class EWorkerSlot : uint8
{
	Body      UMETA(DisplayName="Тело (ведущий)"),
	Head      UMETA(DisplayName="Голова"),
	Hair      UMETA(DisplayName="Волосы"),
	Beard     UMETA(DisplayName="Борода"),
	Torso     UMETA(DisplayName="Верх (одежда)"),
	Legs      UMETA(DisplayName="Низ (штаны)"),
	Feet      UMETA(DisplayName="Обувь/ступни"),
	Gloves    UMETA(DisplayName="Перчатки"),
	Headgear  UMETA(DisplayName="Каска/головной убор"),
	FaceMask  UMETA(DisplayName="Маска/респиратор"),
	Glasses   UMETA(DisplayName="Очки"),
	Vest      UMETA(DisplayName="Жилет"),
	FullOutfit UMETA(DisplayName="Цельный комплект"),
	Headphones UMETA(DisplayName="Наушники"),
	Watch UMETA(DisplayName="Часы"),
	/** Append-only independent skeletal wearables authored under Hips_Modules. */
	HipAccessory UMETA(DisplayName="Набедренное"),
	/** Append-only bib-and-straps layer worn over a separate Torso selection. */
	Overalls UMETA(DisplayName="Рабочий полукомбинезон")
};

/** Один слот: какой меш в какой слот. Софт-ссылка — грузится по требованию. */
/** Derived visual coverage. This is runtime catalog metadata and is never serialized in SaveGame. */
UENUM(BlueprintType, meta=(Bitflags))
enum class EWorkerBodyZone : uint8
{
	None      = 0 UMETA(Hidden),
	Torso     = 1 << 0,
	Pelvis    = 1 << 1,
	UpperArms = 1 << 2,
	Forearms  = 1 << 3,
	Hands     = 1 << 4,
	UpperLegs = 1 << 5,
	LowerLegs = 1 << 6,
	Feet      = 1 << 7
};
ENUM_CLASS_FLAGS(EWorkerBodyZone);

USTRUCT(BlueprintType)
struct FWorkerSlotMesh
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Worker")
	EWorkerSlot Slot = EWorkerSlot::Body;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Worker")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	FWorkerSlotMesh() {}
	FWorkerSlotMesh(EWorkerSlot InSlot, const TSoftObjectPtr<USkeletalMesh>& InMesh)
		: Slot(InSlot), Mesh(InMesh) {}
};

/**
 * Полная внешность рабочего. Реплицируется (косметику видит вся бригада) и
 * сериализуема (TSoftObjectPtr → путь) → готова к сейву (Ф4).
 */
USTRUCT(BlueprintType)
struct FWorkerAppearance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Worker")
	TArray<FWorkerSlotMesh> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Worker")
	FLinearColor SkinColor = FLinearColor(0.86f, 0.62f, 0.45f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Worker")
	bool bUseSkinColor = false;

	/** Назначить/заменить слот (пустой меш = удалить слот). */
	void Set(EWorkerSlot Slot, const TSoftObjectPtr<USkeletalMesh>& Mesh);
	void Clear(EWorkerSlot Slot);
	TSoftObjectPtr<USkeletalMesh> Get(EWorkerSlot Slot) const;
	void SetSkinColor(const FLinearColor& Color);
	bool IsEquivalentTo(const FWorkerAppearance& Other) const;
	FString ToStableDebugString() const;
};

/** Product sections exposed by the Head customization page. */
UENUM(BlueprintType)
enum class EAvHeadCustomizationSection : uint8
{
	HeadType,
	Headgear,
	Hair,
	Beard
};

/**
 * UI-facing projection of the existing exact-path appearance eligibility metadata.
 * Optional head items are derived from the same audited records used by Random; the two
 * mandatory Head records are the separately audited FaceRig product choices.
 */
USTRUCT(BlueprintType)
struct FAvHeadCustomizationCatalogItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	EAvHeadCustomizationSection UISection = EAvHeadCustomizationSection::HeadType;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	EWorkerSlot Slot = EWorkerSlot::Head;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString DisplayName;

	/** Stable AvariikaUI String Table key; DisplayName is only a current-culture compatibility cache. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString DisplayNameKey;

	/** Stable identity used by pending modal actions; never derived from localized text. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString StableId;

	/** Exact Head asset selected by this HeadType record. Empty for optional item records. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString ExactHeadAssetPath;

	/** Stable product identifier; never inferred from a display label or filename substring. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FName HeadTypeId;

	/** Legacy catalog field. HeadType records leave this empty; the profile is HeadType-owned. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FName SkinToneId;

	/** HeadType-owned Light/Dark endpoint applied to every verified skin-bearing material slot. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FLinearColor SkinColor = FLinearColor::White;

	/** Legacy catalog field retained for Blueprint compatibility; mirrors the HeadType profile. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FLinearColor HeadColor = FLinearColor::White;

	/** Legacy catalog field retained for Blueprint compatibility; mirrors the HeadType profile. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FLinearColor BodyColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	float BodyColorCorrectionStrength = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	float BodyBrightness = 1.f;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	float BodySaturation = 1.f;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	float BodyContrast = 1.f;

	/** Exact prepared Texture2D object path for this HeadType card. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString Thumbnail;

	/** Exact prepared Texture2D object path for this item on the HeadType01 profile. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString ThumbnailType01;

	/** Exact prepared Texture2D object path for this item on the HeadType02 profile. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString ThumbnailType02;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString ExactObjectPath;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString Family;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString AuditStatus;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString VisualCondition;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	bool bAllowInUI = false;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	bool bHidesHair = false;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	bool bConflictsWithIntegratedHood = false;

	/** Exact per-item audit result for a raised hood (Hood_2), not a disabled-card rule. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	bool bConflictsWithRaisedHood = false;

	/** Exact-path product policy: only the six ordinary Cap-family records allow Headphones. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	bool bCompatibleWithHeadphones = false;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString HeadphonesCompatibilityReason;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	TArray<FString> Compatibility;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString TooltipReason;

	UPROPERTY(BlueprintReadOnly, Category="Worker|HeadCustomization")
	FString AuditEvidence;
};

/** Development/runtime evidence for the single canonical Random HeadType choice. */
struct FAvRandomHeadSelectionDiagnostics
{
	int32 CandidateCount = 0;
	TArray<FString> CandidateStableIds;
	TArray<FString> CandidatePaths;
	int32 SelectedIndex = INDEX_NONE;
	FString SelectedStableId = TEXT("None");
	FString SelectedPath = TEXT("None");
	FString ResolvedProfile = TEXT("None");
	FString ValidationResult = TEXT("NotRun");
	bool bFallbackUsed = false;
	FString SeedSource = TEXT("FMath::RandRange_GlobalProcessPRNG_NoPerClickReseed");
};

/** Product sections authored in the static Face Protection WidgetSwitcher. */
UENUM(BlueprintType)
enum class EAvFaceProtectionSection : uint8
{
	Glasses,
	Respirator,
	Headphones
};

/** Exact-path product record consumed by the Face Protection page. */
USTRUCT(BlueprintType)
struct FAvFaceProtectionCatalogItem
{
	GENERATED_BODY()

	/** Stable identity used by pending modal actions; never derived from localized text. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString StableId;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	EAvFaceProtectionSection Section = EAvFaceProtectionSection::Glasses;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	EWorkerSlot WorkerSlot = EWorkerSlot::Glasses;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString ExactMeshPath;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString DisplayName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString DisplayNameKey;

	/** Compact value for the constrained Current Selection row; card title stays unchanged. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString CurrentSelectionName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString CurrentSelectionNameKey;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString ThumbnailType01;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString ThumbnailType02;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	bool bIsNone = false;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	bool bEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString DisabledReason;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	FString DisabledReasonKey;

	/** Exact per-item audit result for Jacket_Worker_Hood_2; does not disable the card. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	bool bConflictsWithRaisedHood = false;

	UPROPERTY(BlueprintReadOnly, Category="Worker|FaceProtection")
	TArray<FString> BlockedCompatibilityTags;
};

UENUM(BlueprintType)
enum class EAvHandsAccessoriesSection : uint8
{
	Gloves,
	Watches
};

USTRUCT(BlueprintType)
struct FAvHandsAccessoriesCatalogItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	EAvHandsAccessoriesSection Section = EAvHandsAccessoriesSection::Gloves;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	EWorkerSlot WorkerSlot = EWorkerSlot::Gloves;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString StableId;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString ExactMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString DisplayName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString DisplayNameKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString CurrentSelectionDisplayName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString CurrentSelectionDisplayNameKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString ThumbnailType01;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString ThumbnailType02;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	bool bIsNone = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	bool bEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	bool bCompatibleWithWatch = true;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString DisabledReason;
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories")
	FString DisabledReasonKey;
	/** Derived item-level coverage; never copied into FWorkerAppearance or SaveGame. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|HandsAccessories", meta=(Bitmask, BitmaskEnum="/Script/Avaryo.EWorkerBodyZone"))
	int32 BodyCoverageMask = 0;
};

USTRUCT(BlueprintType)
struct FAvTorsoCatalogItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString StableId;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString ExactMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") TArray<FString> ExactMaterialOverrides;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString DisplayName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString DisplayNameKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString CurrentSelectionName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString CurrentSelectionNameKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString ThumbnailType01;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString ThumbnailType02;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString Family;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString Color;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") int32 SortOrder = 0;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") bool bIsNone = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") bool bEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString DisabledReason;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") FString DisabledReasonKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") bool bCompatibleWithGloves = true;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") bool bCompatibleWithWatch = true;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") bool bCompatibleWithVest = true;
	/** Exact catalog metadata; true only for the raised Jacket_Worker_Hood_2 geometry. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") bool bHasRaisedHood = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso") TArray<FString> CompatibilityTags;
	/** Derived item-level coverage; never copied into FWorkerAppearance or SaveGame. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|Torso", meta=(Bitmask, BitmaskEnum="/Script/Avaryo.EWorkerBodyZone"))
	int32 BodyCoverageMask = 0;
};

UENUM(BlueprintType)
enum class EAvLowerHipSection : uint8
{
	Legs,
	Hip
};

/** Curated exact-path record shared by the Legs and independent hip-wearable pages. */
USTRUCT(BlueprintType)
struct FAvLowerHipCatalogItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") EAvLowerHipSection Section = EAvLowerHipSection::Legs;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") EWorkerSlot WorkerSlot = EWorkerSlot::Legs;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString StableId;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString ExactMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") TArray<FString> ExactMaterialOverrides;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString DisplayName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString DisplayNameKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString CurrentSelectionName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString CurrentSelectionNameKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString ThumbnailType01;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString ThumbnailType02;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString Family;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString Color;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") int32 SortOrder = 0;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") bool bIsNone = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") bool bEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString DisabledReason;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") FString DisabledReasonKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip") TArray<FString> CompatibilityTags;
	/** Derived item-level coverage; Hip records intentionally use None. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|LowerHip", meta=(Bitmask, BitmaskEnum="/Script/Avaryo.EWorkerBodyZone"))
	int32 BodyCoverageMask = 0;
};

/** Technical classification for the single user-facing Full Outfits page. */
UENUM(BlueprintType)
enum class EAvEnsembleTechnicalKind : uint8
{
	None,
	FullOutfit,
	Overalls
};

/** Curated exact-path record shared by true complete outfits and independent overalls. */
USTRUCT(BlueprintType)
struct FAvFullOutfitCatalogItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString StableId;
	/** Zero-based index in WB_Male_Modular_Character_Builder's package-native Overalls array; -1 for None/legacy. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") int32 DemoIndex = -1;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") EAvEnsembleTechnicalKind TechnicalKind = EAvEnsembleTechnicalKind::None;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") EWorkerSlot TargetSlot = EWorkerSlot::FullOutfit;
	/** Primary mesh used by the original demo recipe. ExactMeshPath is retained for existing callers/save lookup. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString PrimaryMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") TArray<FString> PrimaryMaterialOverrides;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString ExactMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") TArray<FString> ExactMaterialOverrides;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString CompanionTorsoMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") TArray<FString> CompanionTorsoMaterials;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString CompanionChestMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") TArray<FString> CompanionChestMaterials;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString CompanionLegsMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") TArray<FString> CompanionLegsMaterials;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString CompanionFeetMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") TArray<FString> CompanionFeetMaterials;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") TArray<FString> AdditionalPackageModules;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString DisplayName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString DisplayNameKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString CurrentSelectionName;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString CurrentSelectionNameKey;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString ThumbnailType01;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString ThumbnailType02;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString Family;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString Color;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") int32 SortOrder = 0;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits", meta=(Bitmask, BitmaskEnum="/Script/Avaryo.EWorkerBodyZone"))
	int32 BodyCoverageMask = 0;
	/** Exact derived shirt used only when bib overalls are active and the saved Torso slot is None. */
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString FallbackTorsoMeshPath;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits", meta=(Bitmask, BitmaskEnum="/Script/Avaryo.EWorkerBodyZone"))
	int32 FallbackTorsoCoverageMask = 0;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") TArray<EWorkerSlot> SuppressedRenderSlots;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") bool bIsNone = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") bool bEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString DisabledReason;
	UPROPERTY(BlueprintReadOnly, Category="Worker|FullOutfits") FString DisabledReasonKey;
};

/**
 * Собирает модульного рабочего из частей-мешей по слотам (тело-лидер + дочерние
 * SkeletalMeshComponent на общей позе). Внешность реплицируется; визуал клиенты
 * строят локально по OnRep. Тело ИГРОКА не подменяет — это Ф2 (свап под глаза юзера);
 * здесь только инфраструктура (build-верифицируемо).
 */
UCLASS(ClassGroup=(Avaryo), meta=(BlueprintSpawnableComponent))
class AVARYO_API UWorkerAppearanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorkerAppearanceComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Назначить меш слота. Только сервер (реплицируется бригаде). */
	UFUNCTION(BlueprintCallable, Category="Worker")
	void SetSlotMesh(EWorkerSlot Slot, USkeletalMesh* Mesh);

	/** Снять меш со слота. Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Worker")
	void ClearSlot(EWorkerSlot Slot);

	/** Применить внешность целиком. Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Worker")
	void ApplyAppearance(const FWorkerAppearance& NewAppearance);

	/** Apply appearance only to a local, non-replicated owner such as the customization preview actor. */
	bool ApplyLocalPreviewAppearance(const FWorkerAppearance& NewAppearance);

	/** Базовый «бомж»: тело/голова/волосы + тишка + джинсы + ступни. Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Worker")
	void ApplyDefaultPreset();

	/** Canonical project base: complete male body modules with authored underwear. */
	static FWorkerAppearance MakeBaseMaleUnderwearAppearance();

	/** Exact legacy factory signature used by the incomplete modular base character. */
	static bool IsObsoleteBaseMaleUnderwearAppearance(const FWorkerAppearance& InAppearance);

	/** Build a male modular look from curated, skeleton-compatible slot assets (never from Presets). */
	static FWorkerAppearance MakeRandomCompatibleMaleAppearance(
		const FWorkerAppearance& PreviousAppearance,
		TMap<EWorkerSlot, int32>* OutCandidateCounts = nullptr,
		FAvRandomHeadSelectionDiagnostics* OutHeadDiagnostics = nullptr);

	/** Validate the structural guarantees required only for RandomGenerated appearances. */
	static bool ValidateRandomGeneratedMaleAppearance(
		const FWorkerAppearance& InAppearance,
		FString& OutValidationDetails);

	/** Exact-path visual eligibility for every non-factory slot selected by Random. */
	static FString DescribeRandomVisualEligibility(const FWorkerAppearance& InAppearance);

	/** Build the unified Head page catalog (HeadType + audited Headgear/Hair/Beard). */
	static TArray<FAvHeadCustomizationCatalogItem> GetHeadCustomizationCatalog();

	/** Curated, default-deny Face Protection catalog using exact verified asset paths. */
	UFUNCTION(BlueprintCallable, Category="Worker|FaceProtection")
	static TArray<FAvFaceProtectionCatalogItem> GetFaceProtectionCatalog();

	UFUNCTION(BlueprintCallable, Category="Worker|HandsAccessories")
	static TArray<FAvHandsAccessoriesCatalogItem> GetHandsAccessoriesCatalog();

	/** Curated exact-path, default-deny catalog for the single Upper Equipment Torso page. */
	static TArray<FAvTorsoCatalogItem> GetTorsoCatalog();

	/** Curated exact-path catalogs for existing Legs and appended independent HipAccessory. */
	static TArray<FAvLowerHipCatalogItem> GetLowerHipCatalog();

	/** Single structured catalog: None, then the original demo's 30 package-native Overalls recipes. */
	static TArray<FAvFullOutfitCatalogItem> GetFullOutfitCatalog();
	/** Resolve a stored ensemble path, including the six hidden legacy Pants_Worker_Full aliases. */
	static bool ResolveFullOutfitCatalogRecord(
		EWorkerSlot StoredSlot,
		const FSoftObjectPath& StoredPath,
		FAvFullOutfitCatalogItem& OutRecord);

	/** Resolve structured HeadType metadata by exact Head asset path. */
	static bool TryGetHeadTypeMetadata(
		const TSoftObjectPtr<USkeletalMesh>& HeadMesh,
		FAvHeadCustomizationCatalogItem& OutMetadata);

	/** Normalize mapped FaceRig heads to the product profile: HeadType01=Light, HeadType02=Dark. */
	static bool NormalizeHeadTypeSkinPresentation(
		FWorkerAppearance& InOutAppearance,
		FString* OutDetails = nullptr);

	/** Validate that an exact HeadType carries its required Light/Dark skin profile. */
	static bool ValidateHeadTypeSkinPresentation(
		const FWorkerAppearance& InAppearance,
		FString& OutValidationDetails);

	/** Validate a one-slot manual Head page edit without reclassifying unrelated active slots. */
	static bool ValidateHeadCustomizationSelection(
		const FWorkerAppearance& InAppearance,
		EWorkerSlot ChangedSlot,
		FString& OutValidationDetails);

	/** Structured exact-path metadata used by runtime Hair suppression. */
	static bool ShouldHeadgearHideHair(const TSoftObjectPtr<USkeletalMesh>& HeadgearMesh);
	bool IsHairSuppressedByHeadgear() const;

	/** Structured exact-path metadata for the raised Jacket_Worker_Hood_2 family. */
	static bool HasRaisedHood(const FWorkerAppearance& InAppearance);
	static bool IsRaisedHoodTorsoPath(const FSoftObjectPath& TorsoPath);
	static bool HeadgearConflictsWithRaisedHood(const TSoftObjectPtr<USkeletalMesh>& HeadgearMesh);
	static bool HeadphonesConflictWithRaisedHood(const TSoftObjectPtr<USkeletalMesh>& HeadphonesMesh);
	/** Exact-path allowlist: None and the six ordinary /Cap/ records are compatible. */
	static bool IsHeadgearCompatibleWithHeadphones(
		const TSoftObjectPtr<USkeletalMesh>& HeadgearMesh);
	static bool HeadgearConflictsWithHeadphones(
		const TSoftObjectPtr<USkeletalMesh>& HeadgearMesh,
		const TSoftObjectPtr<USkeletalMesh>& HeadphonesMesh);

	/** Legacy name retained for source compatibility; now has raised-hood semantics. */
	static bool HasIntegratedHood(const FWorkerAppearance& InAppearance);

	/** Persist the exact catalog requested for Codex/product diagnostics. */
	static bool WriteHeadCustomizationCatalogDiagnostics(FString& OutPathOrError);

	/** Deterministic structural audit used by runtime smoke automation. */
	UFUNCTION(BlueprintCallable, Category="Worker|HeadCustomization|Diagnostics")
	static bool RunHeadCustomizationCatalogSelfTest(FString& OutReport);

	/** Deterministic HeadType skin-profile, Factory/Random and material-boundary validation. */
	UFUNCTION(BlueprintCallable, Category="Worker|HeadCustomization|Diagnostics")
	static bool RunHeadSkinPresentationSelfTest(FString& OutReport);

	/** 200 pure canonical HeadType selections; never mutates SaveGame or a live component. */
	UFUNCTION(BlueprintCallable, Category="Worker|HeadCustomization|Diagnostics")
	static bool RunRandomHeadDistributionSelfTest(FString& OutReport);

	/** Persist exact LOD section -> material-index evidence for the canonical body and heads. */
	UFUNCTION(BlueprintCallable, Category="Worker|HeadCustomization|Diagnostics")
	static bool WriteSkinMaterialSectionAudit(FString& OutPathOrError);

	/** Deterministic catalog/coverage validation; does not spawn actors or mutate saves. */
	UFUNCTION(BlueprintCallable, Category="Worker|Diagnostics")
	static bool RunBodyCoverageSelfTest(FString& OutReport);

	/** Exact 22-record Headgear audit and symmetric Headgear/Headphones policy test. */
	UFUNCTION(BlueprintCallable, Category="Worker|Diagnostics")
	static bool RunHeadgearHeadphonesCompatibilitySelfTest(FString& OutReport);

	/** Persist the exact Headgear compatibility manifest under Saved/CodexDiagnostics. */
	UFUNCTION(BlueprintCallable, Category="Worker|Diagnostics")
	static bool WriteHeadgearHeadphonesCompatibilityDiagnostics(FString& OutPathOrError);

	/**
	 * Exhaustively verifies the single runtime eligibility layer: every allowed exact asset is
	 * AssetVerifiedIntact and accepted by final validation, while every damaged/quarantined exact
	 * asset is absent from scan candidates and rejected by final validation/fallback construction.
	 */
	UFUNCTION(BlueprintCallable, Category="Worker|Diagnostics")
	static bool RunRandomEligibilityExhaustiveSelfTest(FString& OutReport);

	/** Драйв слотов снаряжения от флагов (косметика=функция): каска/респиратор/перчатки. Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Worker")
	void ApplyEquipmentFlags(bool bHelmet, bool bGasMask, bool bGloves);

	/** Снять весь собранный рабочий (вернуть к пустому). Только сервер. */
	UFUNCTION(BlueprintCallable, Category="Worker")
	void ClearAll();

	const FWorkerAppearance& GetAppearance() const { return Appearance; }
	bool HasActiveBodyMesh() const;
	bool IsManagedVisualComponent(const USkeletalMeshComponent* Component) const;

	/** Ведущий компонент тела (leader pose). null пока не собран. */
	UFUNCTION(BlueprintPure, Category="Worker")
	USkeletalMeshComponent* GetBodyComponent() const;

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	/** Development-only exact slot lookup for transient preview diagnostics. Never mutates Appearance. */
	USkeletalMeshComponent* GetAppearanceInspectorSlotComponent(EWorkerSlot Slot) const;
	bool IsAppearanceInspectorSlotSuppressed(EWorkerSlot Slot) const;
	/** Development-only JSON fragment for F9 Appearance Inspector. */
	FString GetBodyCoverageDiagnosticsJson() const;
	const FString& GetResolvedSkinProfileForDiagnostics() const { return ResolvedSkinProfile; }
#endif

	/** Все доступные варианты (имена мешей SKM_*) для слота — авто-скан папок пака. */
	UFUNCTION(BlueprintCallable, Category="Worker")
	TArray<FString> GetOptionsForSlot(EWorkerSlot Slot) const;

	/** Надеть на слот вариант по ключу (часть имени, регистр неважен). Ключ пустой/"none"/"off" — снять. Только сервер. Возвращает успех. */
	UFUNCTION(BlueprintCallable, Category="Worker")
	bool SetSlotByKey(EWorkerSlot Slot, const FString& Key);

	/** Папка пака + фильтры имени для слота (для каталога). */
	static void GetSlotSearch(EWorkerSlot Slot, FString& OutFolder, FString& OutInclude, FString& OutExclude);

	/** Перекрасить слот (тинт через параметр материала "Color Correction" на динамик-инстансе).
	 *  Цвет хранится локально и переживает пересборку визуала. Локально (репликация/сейв — позже). */
	UFUNCTION(BlueprintCallable, Category="Worker")
	void SetSlotColor(EWorkerSlot Slot, FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category="Worker")
	void SetSkinColor(FLinearColor Color);

	/** Сбросить перекрас слота (вернуть исходный материал — через пересборку). */
	UFUNCTION(BlueprintCallable, Category="Worker")
	void ClearSlotColor(EWorkerSlot Slot);

protected:
	virtual void BeginPlay() override;

	/** Реплицируемая внешность; на клиентах OnRep пересобирает визуал. */
	UPROPERTY(ReplicatedUsing=OnRep_Appearance)
	FWorkerAppearance Appearance;

	UFUNCTION()
	void OnRep_Appearance();

	/** Пересобрать дочерние меши под Appearance (Body — лидер). */
	void RebuildVisuals();

	/** Найти/создать дочерний SkeletalMeshComponent под слот. */
	USkeletalMeshComponent* GetOrCreateSlotComp(EWorkerSlot Slot);
	USkeletalMeshComponent* RecreateSlotComp(EWorkerSlot Slot);

	/** Прицепить часть к телу и копировать его позу (общий скелет → кости 1:1). */
	void AttachAsLeaderFollower(USkeletalMeshComponent* Part, USkeletalMeshComponent* Body);

	/** Применить тинт к компоненту (создаёт dynamic material instance на каждом слоте материала, ставит "Color Correction"). */
	void ApplyColorToComp(USkeletalMeshComponent* Comp, const FLinearColor& Color) const;
	/** Apply the material family and calibrated correction owned by the current HeadType. */
	void ApplyHeadTypeSkinProfile();
	void ApplyResolvedBodyCoverage(USkeletalMeshComponent* Body);
	USkeletalMeshComponent* GetOrCreateDerivedBodyComp(FName Role);
	void RemoveUnusedDerivedBodyComps(const TSet<FName>& PresentRoles);
	void RefreshVisualComponent(USkeletalMeshComponent* Comp) const;

	/** Перекрасы слотов (локально, transient). Переприменяются в RebuildVisuals. */
	UPROPERTY(Transient)
	TMap<EWorkerSlot, FLinearColor> SlotColors;

	/** Рантайм-карта слот→компонент. Строится локально, не реплицируется. */
	UPROPERTY(Transient)
	TMap<EWorkerSlot, TObjectPtr<USkeletalMeshComponent>> SlotComps;

	/** Package-native body/underlayer modules derived from coverage, never serialized appearance slots. */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<USkeletalMeshComponent>> DerivedBodyComps;

	uint16 ResolvedBodyCoverageMask = 0;
	uint16 ResolvedTorsoCoverageMask = 0;
	uint16 ResolvedLegsCoverageMask = 0;
	uint16 ResolvedFeetCoverageMask = 0;
	uint16 ResolvedGlovesCoverageMask = 0;
	uint16 ResolvedEnsembleCoverageMask = 0;
	uint16 ResolvedFallbackTorsoCoverageMask = 0;
	FString ResolvedTorsoStableId;
	FString ResolvedLegsStableId;
	FString ResolvedFeetStableId;
	FString ResolvedGlovesStableId;
	FString ResolvedFullOutfitStableId;
	FString ResolvedOverallsStableId;
	int32 ResolvedEnsembleDemoIndex = -1;
	FString ResolvedEnsemblePrimaryMeshPath;
	FString ResolvedEnsembleCompanionTorsoPath;
	FString ResolvedEnsembleCompanionChestPath;
	FString ResolvedEnsembleCompanionLegsPath;
	FString ResolvedEnsembleCompanionFeetPath;
	bool bResolvedOriginalDemoRecipeMatched = false;
	FString ResolvedEnsembleTechnicalKind = TEXT("None");
	FString ResolvedFallbackTorsoPath;
	bool bResolvedEnsembleRecordExists = true;
	TSet<EWorkerSlot> ResolvedSuppressedRenderSlots;
	FString ResolvedBodyCoverageImplementation = TEXT("None");
	FString ResolvedBodyVariantPath;
	FString ResolvedSkinProfile;
	TArray<FString> ResolvedHiddenSections;
	TArray<FString> UnknownCoverageRecords;
};
