#include "Components/WorkerAppearanceComponent.h"

#include "AvariikaLoc.h"
#include "AvaryoCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture2D.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"

// ---- Курированные пути паком Modular Workers (Quantum, UE5-mann). См. MODULAR_WORKER_PLAN.md ----
namespace AvWorkerAssets
{
	// Canonical factory base: one complete body (including underwear) plus the FaceRig head.
	static const TCHAR* FactoryBody = TEXT("/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Qunatum_FaceRig_Body_Hight.SKM_Qunatum_FaceRig_Body_Hight");
	static const TCHAR* FactoryHead = TEXT("/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Quantum_FaceRig.SKM_Quantum_FaceRig");
	static const TCHAR* FactoryHeadAfro = TEXT("/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Quantum_FaceRig_Afro.SKM_Quantum_FaceRig_Afro");

	// База («бомж»)
	static const TCHAR* Body  = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Body_Bottom.SKM_Quantum_Body_Bottom");
	static const TCHAR* BodyBottom = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Body_Bottom.SKM_Quantum_Body_Bottom");
	static const TCHAR* Head  = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Head.SKM_Quantum_Head");
	static const TCHAR* Feet  = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Feet.SKM_Quantum_Feet");
	static const TCHAR* Hair  = TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Short.SKM_Hair_Short");
	static const TCHAR* BareTorso = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Torso.SKM_Quantum_Torso");
	static const TCHAR* BareHands = TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Arms/SKM_Quantum_Hands.SKM_Quantum_Hands");
	static const TCHAR* BareWrist = TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Wrist/SKM_Quantum_Wrist.SKM_Quantum_Wrist");
	static const TCHAR* UnderPants = TEXT("/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_UnderPants.SKM_Quantum_UnderPants");
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

namespace
{
	FString AvResolveLocalizedString(const FString& Key)
	{
		return Key.IsEmpty() ? FString() : FAvLoc::S(Key);
	}

	constexpr uint16 AvBodyZone(EWorkerBodyZone Zone)
	{
		return static_cast<uint16>(Zone);
	}

	constexpr uint16 AvBodyZones(
		EWorkerBodyZone A,
		EWorkerBodyZone B = EWorkerBodyZone::None,
		EWorkerBodyZone C = EWorkerBodyZone::None,
		EWorkerBodyZone D = EWorkerBodyZone::None,
		EWorkerBodyZone E = EWorkerBodyZone::None)
	{
		return AvBodyZone(A) | AvBodyZone(B) | AvBodyZone(C) | AvBodyZone(D) | AvBodyZone(E);
	}

	bool AvHasBodyZone(uint16 Mask, EWorkerBodyZone Zone)
	{
		return (Mask & AvBodyZone(Zone)) != 0;
	}

	FString AvBodyCoverageToString(uint16 Mask)
	{
		if (Mask == 0)
		{
			return TEXT("None");
		}

		TArray<FString> Zones;
		auto Add = [&Zones, Mask](EWorkerBodyZone Zone, const TCHAR* Name)
		{
			if (AvHasBodyZone(Mask, Zone))
			{
				Zones.Add(Name);
			}
		};
		Add(EWorkerBodyZone::Torso, TEXT("Torso"));
		Add(EWorkerBodyZone::Pelvis, TEXT("Pelvis"));
		Add(EWorkerBodyZone::UpperArms, TEXT("UpperArms"));
		Add(EWorkerBodyZone::Forearms, TEXT("Forearms"));
		Add(EWorkerBodyZone::Hands, TEXT("Hands"));
		Add(EWorkerBodyZone::UpperLegs, TEXT("UpperLegs"));
		Add(EWorkerBodyZone::LowerLegs, TEXT("LowerLegs"));
		Add(EWorkerBodyZone::Feet, TEXT("Feet"));
		return FString::Join(Zones, TEXT("|"));
	}

	FString AvJsonEscape(FString Value)
	{
		Value.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
		Value.ReplaceInline(TEXT("\""), TEXT("\\\""));
		Value.ReplaceInline(TEXT("\r"), TEXT("\\r"));
		Value.ReplaceInline(TEXT("\n"), TEXT("\\n"));
		return Value;
	}

	struct FAvHeadTypePresentation
	{
		FName HeadTypeId;
		FSoftObjectPath ExactHeadAssetPath;
		FString DisplayNameKey;
		FString ThumbnailObjectPath;
		FString AuditEvidence;
		FName SkinProfileId;
		FLinearColor SkinColor;
	};

	struct FAvHeadTypeSkinMaterialProfile
	{
		FName SkinProfileId;
		const TCHAR* HeadMaterialPath;
		const TCHAR* HeadAlphaMaterialPath;
		const TCHAR* ArmsMaterialPath;
		const TCHAR* MaskedBodyMaterialPath;
		const TCHAR* BodyBaseColorPath;
		const TCHAR* BodyNormalPath;
		const TCHAR* BodyOrmPath;
	};

	const TArray<FAvHeadTypeSkinMaterialProfile>& AvGetHeadTypeSkinMaterialProfiles()
	{
		static const TArray<FAvHeadTypeSkinMaterialProfile> Profiles =
		{
			{
				TEXT("Light"),
				TEXT("/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Head.MI_Quantum_Head"),
				TEXT("/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Head_Alpha.MI_Quantum_Head_Alpha"),
				TEXT("/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Arms.MI_Quantum_Arms"),
				TEXT("/Game/Avariika/Materials/CharacterCustomization/MI_AvQuantum_Body_SkinMasked.MI_AvQuantum_Body_SkinMasked"),
				TEXT("/Game/Modular_Workers/Textures/Male_Body/European/T_Quantum_Basemesh_Body_BaseColor.T_Quantum_Basemesh_Body_BaseColor"),
				TEXT("/Game/Modular_Workers/Textures/Male_Body/European/T_Quantum_Basemesh_Body_Normal.T_Quantum_Basemesh_Body_Normal"),
				TEXT("/Game/Modular_Workers/Textures/Male_Body/European/T_Quantum_Basemesh_Body_OcclusionRoughnessMetallic.T_Quantum_Basemesh_Body_OcclusionRoughnessMetallic")
			},
			{
				TEXT("Dark"),
				TEXT("/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Head_Afro.MI_Quantum_Head_Afro"),
				TEXT("/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Head_Afro_Alpha.MI_Quantum_Head_Afro_Alpha"),
				TEXT("/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Arms_Afro.MI_Quantum_Arms_Afro"),
				TEXT("/Game/Avariika/Materials/CharacterCustomization/MI_AvQuantum_Body_SkinMasked.MI_AvQuantum_Body_SkinMasked"),
				TEXT("/Game/Modular_Workers/Textures/Male_Body/Afro/T_Quantum_Basemesh_Body_Afro_BaseColor.T_Quantum_Basemesh_Body_Afro_BaseColor"),
				TEXT("/Game/Modular_Workers/Textures/Male_Body/Afro/T_Quantum_Basemesh_Body_Afro_Normal.T_Quantum_Basemesh_Body_Afro_Normal"),
				TEXT("/Game/Modular_Workers/Textures/Male_Body/Afro/T_Quantum_Basemesh_Body_Afro_OcclusionRoughnessMetallic.T_Quantum_Basemesh_Body_Afro_OcclusionRoughnessMetallic")
			}
		};
		return Profiles;
	}

	const FAvHeadTypeSkinMaterialProfile* AvFindHeadTypeSkinMaterialProfile(FName SkinProfileId)
	{
		return AvGetHeadTypeSkinMaterialProfiles().FindByPredicate(
			[SkinProfileId](const FAvHeadTypeSkinMaterialProfile& Profile)
			{
				return Profile.SkinProfileId == SkinProfileId;
			});
	}

	float AvGetReferenceSkinColorCorrectionStrength(const FLinearColor& Color)
	{
		return FMath::GetMappedRangeValueClamped(
			FVector2D(0.12f, 0.82f),
			FVector2D(0.27f, 0.15f),
			Color.GetLuminance());
	}

	FLinearColor AvMakeReferenceSkinCorrectionTarget(const FLinearColor& Color)
	{
		const float Luminance = Color.GetLuminance();
		const float Warmth = FMath::GetMappedRangeValueClamped(
			FVector2D(0.12f, 0.82f), FVector2D(1.08f, 1.16f), Luminance);
		const float BlueKeep = FMath::GetMappedRangeValueClamped(
			FVector2D(0.12f, 0.82f), FVector2D(0.88f, 0.78f), Luminance);
		const float Lift = FMath::GetMappedRangeValueClamped(
			FVector2D(0.12f, 0.82f), FVector2D(0.025f, 0.055f), Luminance);

		return FLinearColor(
			FMath::Clamp(Color.R * Warmth + Lift, 0.f, 1.15f),
			FMath::Clamp(Color.G * 1.03f + Lift * 0.42f, 0.f, 1.05f),
			FMath::Clamp(Color.B * BlueKeep, 0.f, 0.92f),
			1.f);
	}

	float AvGetReferenceSkinBrightness(const FLinearColor& Color)
	{
		return FMath::GetMappedRangeValueClamped(
			FVector2D(0.12f, 0.82f), FVector2D(1.05f, 1.015f), Color.GetLuminance());
	}

	float AvGetReferenceSkinSaturation(const FLinearColor& Color)
	{
		return FMath::GetMappedRangeValueClamped(
			FVector2D(0.12f, 0.82f), FVector2D(1.11f, 1.06f), Color.GetLuminance());
	}

	float AvGetReferenceSkinContrast(const FLinearColor& Color)
	{
		return FMath::GetMappedRangeValueClamped(
			FVector2D(0.12f, 0.82f), FVector2D(1.035f, 1.015f), Color.GetLuminance());
	}

	const TArray<FAvHeadTypePresentation>& AvGetHeadTypePresentations()
	{
		static const TArray<FAvHeadTypePresentation> Presentations =
		{
			{
				FName(TEXT("HeadType01")),
				FSoftObjectPath(AvWorkerAssets::FactoryHead),
				TEXT("Customization.Item.Head.Type01"),
				TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/HeadType/T_UI_HeadType_01.T_UI_HeadType_01"),
				TEXT("Saved/CodexScreenshots/HeadCustomization_20260713/Audit/HeadType/HeadType_01"),
				FName(TEXT("Light")),
				FLinearColor(1.00f, 0.80f, 0.64f, 1.f)
			},
			{
				FName(TEXT("HeadType02")),
				FSoftObjectPath(AvWorkerAssets::FactoryHeadAfro),
				TEXT("Customization.Item.Head.Type02"),
				TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/HeadType/T_UI_HeadType_02.T_UI_HeadType_02"),
				TEXT("Saved/CodexScreenshots/HeadCustomization_20260713/Audit/HeadType/HeadType_02"),
				FName(TEXT("Dark")),
				FLinearColor(0.27f, 0.16f, 0.10f, 1.f)
			}
		};
		return Presentations;
	}

	const FAvHeadTypePresentation* AvFindHeadTypePresentation(const FSoftObjectPath& HeadPath)
	{
		return AvGetHeadTypePresentations().FindByPredicate(
			[&HeadPath](const FAvHeadTypePresentation& Presentation)
			{
				return Presentation.ExactHeadAssetPath == HeadPath;
			});
	}

	/** Exact-path product copy for optional Head-page items. Asset/package names never reach the UI. */
	const TMap<FSoftObjectPath, FString>& AvGetHeadItemDisplayNames()
	{
		static const TMap<FSoftObjectPath, FString> DisplayNames =
		{
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Bege.SKM_Cap_Bege")), TEXT("Customization.Item.Headgear.Cap_SKM_Cap_Bege") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Black.SKM_Cap_Black")), TEXT("Customization.Item.Headgear.Cap_SKM_Cap_Black") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Blue.SKM_Cap_Blue")), TEXT("Customization.Item.Headgear.Cap_SKM_Cap_Blue") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Color.SKM_Cap_Color")), TEXT("Customization.Item.Headgear.Cap_SKM_Cap_Color") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Red.SKM_Cap_Red")), TEXT("Customization.Item.Headgear.Cap_SKM_Cap_Red") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Street.SKM_Cap_Street")), TEXT("Customization.Item.Headgear.Cap_SKM_Cap_Street") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Cowboy/SKM_Hat_Cowboy.SKM_Hat_Cowboy")), TEXT("Customization.Item.Headgear.Hat_Cowboy_SKM_Hat_Cowboy") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Farmer/SKM_Hat_Cowboy.SKM_Hat_Cowboy")), TEXT("Customization.Item.Headgear.Hat_Farmer_SKM_Hat_Cowboy") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Blue.SKM_Hat_Flat_Blue")), TEXT("Customization.Item.Headgear.Hat_Flat_SKM_Hat_Flat_Blue") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Brown.SKM_Hat_Flat_Brown")), TEXT("Customization.Item.Headgear.Hat_Flat_SKM_Hat_Flat_Brown") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Grey.SKM_Hat_Flat_Grey")), TEXT("Customization.Item.Headgear.Hat_Flat_SKM_Hat_Flat_Grey") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Up/SKM_Hat_Up.SKM_Hat_Up")), TEXT("Customization.Item.Headgear.Hat_Up_SKM_Hat_Up") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Blue.SKM_Helmet_Worker_Blue")), TEXT("Customization.Item.Headgear.Helmet_Worker_SKM_Helmet_Worker_Blue") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Green.SKM_Helmet_Worker_Green")), TEXT("Customization.Item.Headgear.Helmet_Worker_SKM_Helmet_Worker_Green") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Orange.SKM_Helmet_Worker_Orange")), TEXT("Customization.Item.Headgear.Helmet_Worker_SKM_Helmet_Worker_Orange") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Red.SKM_Helmet_Worker_Red")), TEXT("Customization.Item.Headgear.Helmet_Worker_SKM_Helmet_Worker_Red") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_White.SKM_Helmet_Worker_White")), TEXT("Customization.Item.Headgear.Helmet_Worker_SKM_Helmet_Worker_White") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Yellow.SKM_Helmet_Worker_Yellow")), TEXT("Customization.Item.Headgear.Helmet_Worker_SKM_Helmet_Worker_Yellow") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Bege.SKM_Warm_Hat_Bege")), TEXT("Customization.Item.Headgear.Warm_Hat_SKM_Warm_Hat_Bege") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Black.SKM_Warm_Hat_Black")), TEXT("Customization.Item.Headgear.Warm_Hat_SKM_Warm_Hat_Black") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Gray.SKM_Warm_Hat_Gray")), TEXT("Customization.Item.Headgear.Warm_Hat_SKM_Warm_Hat_Gray") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Green.SKM_Warm_Hat_Green")), TEXT("Customization.Item.Headgear.Warm_Hat_SKM_Warm_Hat_Green") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Back.SKM_Hair_Back")), TEXT("Customization.Item.Hair.Hair_SKM_Hair_Back") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Long.SKM_Hair_Long")), TEXT("Customization.Item.Hair.Hair_SKM_Hair_Long") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Mohawk.SKM_Hair_Mohawk")), TEXT("Customization.Item.Hair.Hair_SKM_Hair_Mohawk") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Short.SKM_Hair_Short")), TEXT("Customization.Item.Hair.Hair_SKM_Hair_Short") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Skin.SKM_Hair_Skin")), TEXT("Customization.Item.Hair.Hair_SKM_Hair_Skin") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard.SKM_Beard")), TEXT("Customization.Item.Beard.Beard_SKM_Beard") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_02.SKM_Beard_02")), TEXT("Customization.Item.Beard.Beard_SKM_Beard_02") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_03.SKM_Beard_03")), TEXT("Customization.Item.Beard.Beard_SKM_Beard_03") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_4.SKM_Beard_4")), TEXT("Customization.Item.Beard.Beard_SKM_Beard_4") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_Long.SKM_Beard_Long")), TEXT("Customization.Item.Beard.Beard_SKM_Beard_Long") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_Long_2.SKM_Beard_Long_2")), TEXT("Customization.Item.Beard.Beard_SKM_Beard_Long_2") },
			{ FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Moustache.SKM_Moustache")), TEXT("Customization.Item.Beard.Beard_SKM_Moustache") }
		};
		return DisplayNames;
	}

	const FString* AvFindHeadItemDisplayName(const FSoftObjectPath& ExactAssetPath)
	{
		return AvGetHeadItemDisplayNames().Find(ExactAssetPath);
	}

	enum class EAvSkinMaterialRegion : uint8
	{
		None,
		Head,
		Arms,
		Body
	};

	EAvSkinMaterialRegion AvGetSupportedSkinMaterialRegion(
		const FName SlotName,
		const FName MaterialName)
	{
		static const TSet<FName> HeadMaterials = {
			TEXT("MI_Quantum_Head"),
			TEXT("MI_Quantum_Head_Afro"),
			TEXT("MI_Quantum_Head_Alpha"),
			TEXT("MI_Quantum_Head_Afro_Alpha")
		};
		static const TSet<FName> ArmsMaterials = {
			TEXT("MI_Quantum_Arms"),
			TEXT("MI_Quantum_Arms_Afro")
		};
		static const TSet<FName> BodyMaterials = {
			TEXT("MI_Quantum_Body"),
			TEXT("MI_Quantum_Body_Afro")
		};
		static const TSet<FName> ArmsSlots = {
			TEXT("M_Quantum_Arms"),
			TEXT("M_Quantum_Arms1"),
			TEXT("M_Quantum_Arms2"),
			TEXT("M_Quantum_Arms3")
		};
		static const TSet<FName> BodySlots = {
			TEXT("M_Quantum_Body"),
			TEXT("M_Quantum_Body1")
		};

		// FaceRig heads use authored slot names such as standardSurface2/lambert1, while the
		// canonical body's neck uses M_Quantum_Head1. The exact material allowlist is the
		// stable boundary shared by both meshes.
		if (HeadMaterials.Contains(MaterialName))
		{
			return EAvSkinMaterialRegion::Head;
		}
		if (ArmsSlots.Contains(SlotName) && ArmsMaterials.Contains(MaterialName))
		{
			return EAvSkinMaterialRegion::Arms;
		}
		if (BodySlots.Contains(SlotName) && BodyMaterials.Contains(MaterialName))
		{
			return EAvSkinMaterialRegion::Body;
		}
		return EAvSkinMaterialRegion::None;
	}

	bool AvIsSupportedSkinMaterial(const FName SlotName, const FName MaterialName)
	{
		return AvGetSupportedSkinMaterialRegion(SlotName, MaterialName) !=
			EAvSkinMaterialRegion::None;
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

void FWorkerAppearance::SetSkinColor(const FLinearColor& Color)
{
	SkinColor = Color;
	SkinColor.A = 1.f;
	bUseSkinColor = true;
}

bool FWorkerAppearance::IsEquivalentTo(const FWorkerAppearance& Other) const
{
	if (bUseSkinColor != Other.bUseSkinColor ||
		(bUseSkinColor && !SkinColor.Equals(Other.SkinColor, KINDA_SMALL_NUMBER)) ||
		Slots.Num() != Other.Slots.Num())
	{
		return false;
	}
	for (const FWorkerSlotMesh& SlotMesh : Slots)
	{
		if (Get(SlotMesh.Slot).ToSoftObjectPath() != Other.Get(SlotMesh.Slot).ToSoftObjectPath())
		{
			return false;
		}
	}
	return true;
}

FString FWorkerAppearance::ToStableDebugString() const
{
	TArray<FWorkerSlotMesh> SortedSlots = Slots;
	SortedSlots.Sort([](const FWorkerSlotMesh& A, const FWorkerSlotMesh& B)
	{
		return static_cast<uint8>(A.Slot) < static_cast<uint8>(B.Slot);
	});
	TArray<FString> Parts;
	for (const FWorkerSlotMesh& SlotMesh : SortedSlots)
	{
		Parts.Add(FString::Printf(
			TEXT("%d=%s"),
			static_cast<int32>(SlotMesh.Slot),
			*SlotMesh.Mesh.ToSoftObjectPath().ToString()));
	}
	if (bUseSkinColor)
	{
		Parts.Add(FString::Printf(TEXT("Skin=%.3f,%.3f,%.3f"), SkinColor.R, SkinColor.G, SkinColor.B));
	}
	return FString::Join(Parts, TEXT("|"));
}

bool UWorkerAppearanceComponent::TryGetHeadTypeMetadata(
	const TSoftObjectPtr<USkeletalMesh>& HeadMesh,
	FAvHeadCustomizationCatalogItem& OutMetadata)
{
	const FAvHeadTypePresentation* Presentation =
		AvFindHeadTypePresentation(HeadMesh.ToSoftObjectPath());
	if (!Presentation)
	{
		return false;
	}

	OutMetadata = FAvHeadCustomizationCatalogItem();
	OutMetadata.UISection = EAvHeadCustomizationSection::HeadType;
	OutMetadata.Slot = EWorkerSlot::Head;
	OutMetadata.DisplayNameKey = Presentation->DisplayNameKey;
	OutMetadata.DisplayName = AvResolveLocalizedString(OutMetadata.DisplayNameKey);
	OutMetadata.StableId = Presentation->HeadTypeId.ToString();
	OutMetadata.ExactHeadAssetPath = Presentation->ExactHeadAssetPath.ToString();
	OutMetadata.ExactObjectPath = OutMetadata.ExactHeadAssetPath;
	OutMetadata.HeadTypeId = Presentation->HeadTypeId;
	// SkinToneId remains empty for Blueprint compatibility. The product profile is owned by
	// HeadType, and SkinColor carries its normalized Light/Dark endpoint through existing saves.
	OutMetadata.SkinToneId = NAME_None;
	OutMetadata.SkinColor = Presentation->SkinColor;
	OutMetadata.HeadColor = Presentation->SkinColor;
	OutMetadata.BodyColor = Presentation->SkinColor;
	OutMetadata.BodyColorCorrectionStrength =
		AvGetReferenceSkinColorCorrectionStrength(Presentation->SkinColor);
	OutMetadata.BodyBrightness = AvGetReferenceSkinBrightness(Presentation->SkinColor);
	OutMetadata.BodySaturation = AvGetReferenceSkinSaturation(Presentation->SkinColor);
	OutMetadata.BodyContrast = AvGetReferenceSkinContrast(Presentation->SkinColor);
	OutMetadata.Thumbnail = Presentation->ThumbnailObjectPath;
	OutMetadata.ThumbnailType01 = Presentation->ThumbnailObjectPath;
	OutMetadata.ThumbnailType02 = Presentation->ThumbnailObjectPath;
	OutMetadata.Family = TEXT("Quantum_FaceRig");
	OutMetadata.AuditStatus = TEXT("AssetVerifiedIntact");
	OutMetadata.VisualCondition = TEXT("Intact");
	OutMetadata.bAllowInUI = true;
	OutMetadata.Compatibility = {
		TEXT("MaleQuantumSkeletonCompatible"),
		TEXT("FaceRigCompatible"),
		TEXT("CanonicalFactoryBodyCompatible"),
		TEXT("HairCompatible"),
		TEXT("BeardCompatible"),
		TEXT("HeadgearCompatible"),
		TEXT("FullBodySkinPresentation")
	};
	OutMetadata.AuditEvidence = Presentation->AuditEvidence;
	return true;
}

bool UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(
	FWorkerAppearance& InOutAppearance,
	FString* OutDetails)
{
	const FAvHeadTypePresentation* Presentation =
		AvFindHeadTypePresentation(InOutAppearance.Get(EWorkerSlot::Head).ToSoftObjectPath());
	if (!Presentation)
	{
		if (OutDetails)
		{
			*OutDetails = TEXT("HeadType=Unmapped SkinPresentation=Unchanged");
		}
		return false;
	}

	const bool bChanged = !InOutAppearance.bUseSkinColor ||
		!InOutAppearance.SkinColor.Equals(Presentation->SkinColor, KINDA_SMALL_NUMBER);
	if (bChanged)
	{
		InOutAppearance.SetSkinColor(Presentation->SkinColor);
	}
	if (OutDetails)
	{
		*OutDetails = FString::Printf(
			TEXT("HeadTypeId=%s SkinProfile=%s Skin=(%.3f,%.3f,%.3f) Normalized=%s"),
			*Presentation->HeadTypeId.ToString(),
			*Presentation->SkinProfileId.ToString(),
			InOutAppearance.SkinColor.R,
			InOutAppearance.SkinColor.G,
			InOutAppearance.SkinColor.B,
			bChanged ? TEXT("true") : TEXT("false"));
	}
	return bChanged;
}

bool UWorkerAppearanceComponent::ValidateHeadTypeSkinPresentation(
	const FWorkerAppearance& InAppearance,
	FString& OutValidationDetails)
{
	const FAvHeadTypePresentation* Presentation =
		AvFindHeadTypePresentation(InAppearance.Get(EWorkerSlot::Head).ToSoftObjectPath());
	if (!Presentation)
	{
		OutValidationDetails = TEXT("Result=FAIL Reason=HeadTypeNotInStructuredMetadata");
		return false;
	}
	if (!InAppearance.bUseSkinColor ||
		!InAppearance.SkinColor.Equals(Presentation->SkinColor, KINDA_SMALL_NUMBER))
	{
		OutValidationDetails = FString::Printf(
			TEXT("Result=FAIL Reason=HeadTypeSkinProfileMismatch HeadTypeId=%s ExpectedProfile=%s ExpectedSkin=(%.3f,%.3f,%.3f) ActualSkin=(%.3f,%.3f,%.3f)"),
			*Presentation->HeadTypeId.ToString(),
			*Presentation->SkinProfileId.ToString(),
			Presentation->SkinColor.R,
			Presentation->SkinColor.G,
			Presentation->SkinColor.B,
			InAppearance.SkinColor.R,
			InAppearance.SkinColor.G,
			InAppearance.SkinColor.B);
		return false;
	}

	OutValidationDetails = FString::Printf(
		TEXT("Result=PASS HeadTypeId=%s SkinProfile=%s Skin=(%.3f,%.3f,%.3f)"),
		*Presentation->HeadTypeId.ToString(),
		*Presentation->SkinProfileId.ToString(),
		InAppearance.SkinColor.R,
		InAppearance.SkinColor.G,
		InAppearance.SkinColor.B);
	return true;
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
		NormalizeHeadTypeSkinPresentation(Appearance);
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
	if (Slot == EWorkerSlot::Head)
	{
		NormalizeHeadTypeSkinPresentation(Appearance);
	}
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
	SlotColors.Reset();
	Appearance = NewAppearance;
	NormalizeHeadTypeSkinPresentation(Appearance);
	RebuildVisuals();
}

bool UWorkerAppearanceComponent::ApplyLocalPreviewAppearance(const FWorkerAppearance& NewAppearance)
{
	AActor* Owner = GetOwner();
	if (!Owner || Owner->GetIsReplicated())
	{
		return false;
	}

	Appearance = NewAppearance;
	NormalizeHeadTypeSkinPresentation(Appearance);
	SlotColors.Reset();
	RebuildVisuals();
	return true;
}

void UWorkerAppearanceComponent::ClearAll()
{
	if (!IsAuth(this)) return;
	Appearance.Slots.Reset();
	SlotColors.Reset();
	RebuildVisuals();
}

void UWorkerAppearanceComponent::ApplyDefaultPreset()
{
	if (!IsAuth(this)) return;
	ApplyAppearance(MakeBaseMaleUnderwearAppearance());
}

FWorkerAppearance UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance()
{
	FWorkerAppearance Appearance;
	Appearance.Set(EWorkerSlot::Body, AvWorkerAssets::Soft(AvWorkerAssets::FactoryBody));
	Appearance.Set(EWorkerSlot::Head, AvWorkerAssets::Soft(AvWorkerAssets::FactoryHead));
	NormalizeHeadTypeSkinPresentation(Appearance);
	return Appearance;
}

bool UWorkerAppearanceComponent::IsObsoleteBaseMaleUnderwearAppearance(const FWorkerAppearance& InAppearance)
{
	if (InAppearance.bUseSkinColor || InAppearance.Slots.Num() != 5)
	{
		return false;
	}

	return InAppearance.Get(EWorkerSlot::Body).ToSoftObjectPath() == FSoftObjectPath(AvWorkerAssets::UnderPants)
		&& InAppearance.Get(EWorkerSlot::Head).ToSoftObjectPath() == FSoftObjectPath(AvWorkerAssets::Head)
		&& InAppearance.Get(EWorkerSlot::Hair).ToSoftObjectPath() == FSoftObjectPath(AvWorkerAssets::Hair)
		&& InAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath() == FSoftObjectPath(AvWorkerAssets::BareTorso)
		&& InAppearance.Get(EWorkerSlot::Gloves).ToSoftObjectPath() == FSoftObjectPath(AvWorkerAssets::BareHands);
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
	NormalizeHeadTypeSkinPresentation(Appearance);
	SlotColors.Reset();
	RebuildVisuals();
}

bool UWorkerAppearanceComponent::HasActiveBodyMesh() const
{
	return !Appearance.Get(EWorkerSlot::Body).IsNull();
}

bool UWorkerAppearanceComponent::IsManagedVisualComponent(const USkeletalMeshComponent* Component) const
{
	if (!Component)
	{
		return false;
	}

	for (const TPair<EWorkerSlot, TObjectPtr<USkeletalMeshComponent>>& Pair : SlotComps)
	{
		if (Pair.Value == Component)
		{
			return true;
		}
	}
	for (const TPair<FName, TObjectPtr<USkeletalMeshComponent>>& Pair : DerivedBodyComps)
	{
		if (Pair.Value == Component)
		{
			return true;
		}
	}

	return false;
}

USkeletalMeshComponent* UWorkerAppearanceComponent::GetBodyComponent() const
{
	const TObjectPtr<USkeletalMeshComponent>* Found = SlotComps.Find(EWorkerSlot::Body);
	return Found ? *Found : nullptr;
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
USkeletalMeshComponent* UWorkerAppearanceComponent::GetAppearanceInspectorSlotComponent(EWorkerSlot Slot) const
{
	const TObjectPtr<USkeletalMeshComponent>* Found = SlotComps.Find(Slot);
	return Found ? *Found : nullptr;
}

bool UWorkerAppearanceComponent::IsAppearanceInspectorSlotSuppressed(EWorkerSlot Slot) const
{
	return ResolvedSuppressedRenderSlots.Contains(Slot) ||
		(Slot == EWorkerSlot::Hair && IsHairSuppressedByHeadgear());
}

FString UWorkerAppearanceComponent::GetBodyCoverageDiagnosticsJson() const
{
	const uint16 AllZones = AvBodyZones(
		EWorkerBodyZone::Torso,
		EWorkerBodyZone::Pelvis,
		EWorkerBodyZone::UpperArms,
		EWorkerBodyZone::Forearms,
		EWorkerBodyZone::Hands) |
		AvBodyZones(EWorkerBodyZone::UpperLegs, EWorkerBodyZone::LowerLegs, EWorkerBodyZone::Feet);
	const FString Unknown = UnknownCoverageRecords.IsEmpty()
		? TEXT("None")
		: FString::Join(UnknownCoverageRecords, TEXT("; "));
	const FString Sections = ResolvedHiddenSections.IsEmpty()
		? TEXT("None")
		: FString::Join(ResolvedHiddenSections, TEXT(", "));
	const FString BodyPath = GetBodyComponent() && GetBodyComponent()->GetSkeletalMeshAsset()
		? GetBodyComponent()->GetSkeletalMeshAsset()->GetPathName()
		: TEXT("None");
	TArray<FString> SuppressedSlotNames;
	for (const EWorkerSlot Slot : ResolvedSuppressedRenderSlots)
	{
		switch (Slot)
		{
		case EWorkerSlot::Torso: SuppressedSlotNames.Add(TEXT("Torso")); break;
		case EWorkerSlot::Legs: SuppressedSlotNames.Add(TEXT("Legs")); break;
		case EWorkerSlot::Feet: SuppressedSlotNames.Add(TEXT("Feet")); break;
		case EWorkerSlot::Vest: SuppressedSlotNames.Add(TEXT("Vest")); break;
		case EWorkerSlot::Gloves: SuppressedSlotNames.Add(TEXT("Gloves")); break;
		case EWorkerSlot::FullOutfit: SuppressedSlotNames.Add(TEXT("FullOutfit")); break;
		case EWorkerSlot::Overalls: SuppressedSlotNames.Add(TEXT("Overalls")); break;
		default: SuppressedSlotNames.Add(FString::Printf(TEXT("Slot%d"), static_cast<int32>(Slot))); break;
		}
	}
	SuppressedSlotNames.Sort();
	const FString Suppressed = SuppressedSlotNames.IsEmpty()
		? TEXT("None") : FString::Join(SuppressedSlotNames, TEXT(","));
	return FString::Printf(
		TEXT("{\"active_body_mesh\":\"%s\",\"implementation\":\"%s\",")
		TEXT("\"resolved_mask\":\"%s\",\"visible_zones\":\"%s\",\"hidden_zones\":\"%s\",")
		TEXT("\"torso_stable_id\":\"%s\",\"torso_coverage\":\"%s\",")
		TEXT("\"legs_stable_id\":\"%s\",\"legs_coverage\":\"%s\",")
		TEXT("\"feet_stable_id\":\"%s\",\"feet_coverage\":\"%s\",")
		TEXT("\"gloves_stable_id\":\"%s\",\"gloves_coverage\":\"%s\",")
		TEXT("\"ensemble_kind\":\"%s\",\"full_outfit_stable_id\":\"%s\",")
		TEXT("\"overalls_stable_id\":\"%s\",\"ensemble_record_exists\":%s,")
		TEXT("\"ensemble_demo_index\":%d,\"ensemble_primary_mesh\":\"%s\",")
		TEXT("\"companion_torso\":\"%s\",\"companion_chest\":\"%s\",")
		TEXT("\"companion_legs\":\"%s\",\"companion_feet\":\"%s\",")
		TEXT("\"original_demo_recipe_matched\":%s,\"thumbnail_runtime_recipe_parity\":true,")
		TEXT("\"ensemble_coverage\":\"%s\",\"fallback_torso_path\":\"%s\",")
		TEXT("\"fallback_torso_coverage\":\"%s\",\"suppressed_render_slots\":\"%s\",")
		TEXT("\"unknown_coverage_records\":\"%s\",\"active_body_variant\":\"%s\",")
		TEXT("\"hidden_sections\":\"%s\",\"skin_profile\":\"%s\",")
		TEXT("\"preview_gameplay_parity\":\"WorkerAppearanceComponent::RebuildVisuals\"}"),
		*AvJsonEscape(BodyPath),
		*AvJsonEscape(ResolvedBodyCoverageImplementation),
		*AvJsonEscape(AvBodyCoverageToString(ResolvedBodyCoverageMask)),
		*AvJsonEscape(AvBodyCoverageToString(AllZones & ~ResolvedBodyCoverageMask)),
		*AvJsonEscape(AvBodyCoverageToString(ResolvedBodyCoverageMask)),
		*AvJsonEscape(ResolvedTorsoStableId),
		*AvJsonEscape(AvBodyCoverageToString(ResolvedTorsoCoverageMask)),
		*AvJsonEscape(ResolvedLegsStableId),
		*AvJsonEscape(AvBodyCoverageToString(ResolvedLegsCoverageMask)),
		*AvJsonEscape(ResolvedFeetStableId),
		*AvJsonEscape(AvBodyCoverageToString(ResolvedFeetCoverageMask)),
		*AvJsonEscape(ResolvedGlovesStableId),
		*AvJsonEscape(AvBodyCoverageToString(ResolvedGlovesCoverageMask)),
		*AvJsonEscape(ResolvedEnsembleTechnicalKind),
		*AvJsonEscape(ResolvedFullOutfitStableId),
		*AvJsonEscape(ResolvedOverallsStableId),
		bResolvedEnsembleRecordExists ? TEXT("true") : TEXT("false"),
		ResolvedEnsembleDemoIndex,
		*AvJsonEscape(ResolvedEnsemblePrimaryMeshPath.IsEmpty() ? TEXT("None") : ResolvedEnsemblePrimaryMeshPath),
		*AvJsonEscape(ResolvedEnsembleCompanionTorsoPath.IsEmpty() ? TEXT("None") : ResolvedEnsembleCompanionTorsoPath),
		*AvJsonEscape(ResolvedEnsembleCompanionChestPath.IsEmpty() ? TEXT("None") : ResolvedEnsembleCompanionChestPath),
		*AvJsonEscape(ResolvedEnsembleCompanionLegsPath.IsEmpty() ? TEXT("None") : ResolvedEnsembleCompanionLegsPath),
		*AvJsonEscape(ResolvedEnsembleCompanionFeetPath.IsEmpty() ? TEXT("None") : ResolvedEnsembleCompanionFeetPath),
		bResolvedOriginalDemoRecipeMatched ? TEXT("true") : TEXT("false"),
		*AvJsonEscape(AvBodyCoverageToString(ResolvedEnsembleCoverageMask)),
		*AvJsonEscape(ResolvedFallbackTorsoPath.IsEmpty() ? TEXT("None") : ResolvedFallbackTorsoPath),
		*AvJsonEscape(AvBodyCoverageToString(ResolvedFallbackTorsoCoverageMask)),
		*AvJsonEscape(Suppressed),
		*AvJsonEscape(Unknown),
		*AvJsonEscape(ResolvedBodyVariantPath),
		*AvJsonEscape(Sections),
		*AvJsonEscape(ResolvedSkinProfile));
}
#endif

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

USkeletalMeshComponent* UWorkerAppearanceComponent::RecreateSlotComp(EWorkerSlot Slot)
{
	if (TObjectPtr<USkeletalMeshComponent>* Found = SlotComps.Find(Slot))
	{
		if (*Found)
		{
			(*Found)->SetLeaderPoseComponent(nullptr);
			(*Found)->EmptyOverrideMaterials();
			(*Found)->SetSkeletalMeshAsset(nullptr);
			(*Found)->DestroyComponent();
		}
		SlotComps.Remove(Slot);
	}

	return GetOrCreateSlotComp(Slot);
}

USkeletalMeshComponent* UWorkerAppearanceComponent::GetOrCreateDerivedBodyComp(FName Role)
{
	if (TObjectPtr<USkeletalMeshComponent>* Found = DerivedBodyComps.Find(Role))
	{
		if (*Found)
		{
			return *Found;
		}
		DerivedBodyComps.Remove(Role);
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}
	USkeletalMeshComponent* Comp = NewObject<USkeletalMeshComponent>(Owner);
	if (!Comp)
	{
		return nullptr;
	}
	Comp->SetIsReplicated(false);
	Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Comp->SetGenerateOverlapEvents(false);
	Comp->RegisterComponent();
	DerivedBodyComps.Add(Role, Comp);
	return Comp;
}

void UWorkerAppearanceComponent::RemoveUnusedDerivedBodyComps(const TSet<FName>& PresentRoles)
{
	TArray<FName> ToRemove;
	for (const TPair<FName, TObjectPtr<USkeletalMeshComponent>>& Pair : DerivedBodyComps)
	{
		if (!PresentRoles.Contains(Pair.Key))
		{
			ToRemove.Add(Pair.Key);
		}
	}
	for (FName Role : ToRemove)
	{
		if (TObjectPtr<USkeletalMeshComponent>* Found = DerivedBodyComps.Find(Role))
		{
			if (*Found)
			{
				(*Found)->SetLeaderPoseComponent(nullptr);
				(*Found)->EmptyOverrideMaterials();
				(*Found)->SetSkeletalMeshAsset(nullptr);
				(*Found)->DestroyComponent();
			}
		}
		DerivedBodyComps.Remove(Role);
	}
}

void UWorkerAppearanceComponent::AttachAsLeaderFollower(USkeletalMeshComponent* Part, USkeletalMeshComponent* Body)
{
	if (!Part || !Body) return;
	Part->AttachToComponent(Body, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Part->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	Part->SetLeaderPoseComponent(Body); // общий скелет → кости 1:1
	Part->SetOwnerNoSee(Body->bOwnerNoSee);
	Part->SetOnlyOwnerSee(Body->bOnlyOwnerSee);
	Part->SetHiddenInGame(false, true);
	Part->Activate(true);
	Part->SetVisibility(true, true);
}

// Папка пака + фильтры имени для каждого слота (авто-каталог).
void UWorkerAppearanceComponent::GetSlotSearch(EWorkerSlot Slot, FString& Folder, FString& Inc, FString& Exc)
{
	const TCHAR* Base = TEXT("/Game/Modular_Workers/Mesh/Male/");
	Folder.Reset(); Inc.Reset(); Exc.Reset();
	switch (Slot)
	{
	case EWorkerSlot::Hair:     Folder = FString(Base) + TEXT("Head_Modules/Hair"); break;
	case EWorkerSlot::Beard:    Folder = FString(Base) + TEXT("Face_Modules/Beard"); break;
	case EWorkerSlot::Torso:    Folder = FString(Base) + TEXT("Clothes_Modules"); break;
	case EWorkerSlot::Legs:     Folder = FString(Base) + TEXT("Pants_Modules"); break;
	case EWorkerSlot::Headgear: Folder = FString(Base) + TEXT("Head_Modules"); Exc = TEXT("Hair"); break; // всё кроме волос
	case EWorkerSlot::FaceMask: Folder = FString(Base) + TEXT("Face_Modules/Respirator"); break;
	case EWorkerSlot::Glasses:  Folder = FString(Base) + TEXT("Face_Modules"); Inc = TEXT("Glasses"); break;
	case EWorkerSlot::Headphones: Folder = FString(Base) + TEXT("Head_Modules"); Inc = TEXT("Headph"); break;
	case EWorkerSlot::Gloves:   Folder = FString(Base) + TEXT("Arms_Modules"); Inc = TEXT("Gloves"); break;
	case EWorkerSlot::Watch:    Folder = FString(Base) + TEXT("Arms_Modules/Watches"); Inc = TEXT("Watches"); break;
	case EWorkerSlot::HipAccessory: Folder = FString(Base) + TEXT("Hips_Modules/Bag_Hip"); Inc = TEXT("Bag_Hip"); break;
	case EWorkerSlot::FullOutfit: Folder = FString(Base) + TEXT("Overalls_Modules"); break;
	case EWorkerSlot::Overalls: Folder = FString(Base) + TEXT("Pants_Modules/Pants_Worker_Full"); Inc = TEXT("Pants_Worker_Full"); break;
	case EWorkerSlot::Vest:     Folder = FString(Base) + TEXT("Chest_Modules/Vest_Worker"); break;
	case EWorkerSlot::Feet:     Folder = FString(Base) + TEXT("Body/European"); Inc = TEXT("Feet"); break;
	case EWorkerSlot::Body:     Folder = FString(Base) + TEXT("Body/European"); Inc = TEXT("Body"); break;
	case EWorkerSlot::Head:     Folder = FString(Base) + TEXT("Body/European"); Inc = TEXT("Head"); break;
	default: break;
	}
}

static void AvScanSlot(EWorkerSlot Slot, TArray<FAssetData>& OutAssets)
{
	FString Folder, Inc, Exc;
	UWorkerAppearanceComponent::GetSlotSearch(Slot, Folder, Inc, Exc);
	if (Folder.IsEmpty()) return;
	IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	FARFilter F;
	F.PackagePaths.Add(FName(*Folder));
	F.bRecursivePaths = true;
	F.ClassPaths.Add(USkeletalMesh::StaticClass()->GetClassPathName());
	TArray<FAssetData> Raw;
	AR.GetAssets(F, Raw);
	for (const FAssetData& A : Raw)
	{
		const FString N = A.AssetName.ToString();
		if (!N.StartsWith(TEXT("SKM_"))) continue;
		if (!Inc.IsEmpty() && !N.Contains(Inc)) continue;
		if (!Exc.IsEmpty() && N.Contains(Exc)) continue;
		OutAssets.Add(A);
	}
}

namespace
{
	const FString CompatibleMaleSkeletonPath =
		TEXT("/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton");
	constexpr int32 RandomGenerationMaxAttempts = 16;

	struct FAvRandomFamilyDefinition
	{
		EWorkerSlot Slot;
		const TCHAR* Folder;
		bool bCoversUpperBody = false;
		bool bCoversLowerBody = false;
		bool bIncludesFootwear = false;
		bool bHasIntegratedHood = false;
		bool bIsFullOutfit = false;
	};

	// The source pack has no compatibility metadata. Its folder-authored families are described
	// once here instead of scattering asset-name checks through the UI and generator.
	const FAvRandomFamilyDefinition RandomFamilies[] =
	{
		{ EWorkerSlot::Hair, TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair") },
		{ EWorkerSlot::Beard, TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard") },
		{ EWorkerSlot::Torso, TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_M65"), true },
		{ EWorkerSlot::Torso, TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker"), true },
		{ EWorkerSlot::Torso, TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1"), true },
		{ EWorkerSlot::Torso, TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2"), true, false, false, true },
		{ EWorkerSlot::Torso, TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp"), true },
		{ EWorkerSlot::Torso, TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt"), true },
		{ EWorkerSlot::Torso, TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked"), true },
		// Jeans have lower-body coverage but no boot material. They are intentionally retained
		// so the final validator rejects them until compatible standalone footwear exists.
		{ EWorkerSlot::Legs, TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans"), false, true },
		{ EWorkerSlot::Legs, TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jumpsuit"), false, true, true },
		{ EWorkerSlot::Legs, TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker"), false, true, true },
		{ EWorkerSlot::Legs, TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full"), false, true, true },
		{ EWorkerSlot::Gloves, TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker") },
		{ EWorkerSlot::Headgear, TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker") },
		{ EWorkerSlot::Headgear, TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap") },
		{ EWorkerSlot::Headgear, TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Cowboy") },
		{ EWorkerSlot::Headgear, TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Farmer") },
		{ EWorkerSlot::Headgear, TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat") },
		{ EWorkerSlot::Headgear, TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Up") },
		{ EWorkerSlot::Headgear, TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat") },
		{ EWorkerSlot::FaceMask, TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Respirator") },
		{ EWorkerSlot::Glasses, TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Aviator") },
		{ EWorkerSlot::Glasses, TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Classic") },
		{ EWorkerSlot::Glasses, TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Worker") },
		{ EWorkerSlot::Vest, TEXT("/Game/Modular_Workers/Mesh/Male/Chest_Modules/Vest_Worker") },
		{ EWorkerSlot::FullOutfit, TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots"), true, true, true, false, true },
		{ EWorkerSlot::FullOutfit, TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Pants_Worker"), true, true, true, false, true },
		{ EWorkerSlot::FullOutfit, TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots"), true, true, true, false, true },
		{ EWorkerSlot::FullOutfit, TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt"), true, true, true, false, true },
		{ EWorkerSlot::FullOutfit, TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt"), true, true, true, false, true },
		{ EWorkerSlot::FullOutfit, TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants"), true, true, true, false, true }
	};

	const TSet<FSoftObjectPath>& AvGetRaisedHoodConflictingHeadgear()
	{
		// Exact-path audit: Hood_2 surrounds the crown, temples and ears. Every current
		// Headgear mesh occupies or protrudes through at least one of those volumes.
		static const TSet<FSoftObjectPath> Paths =
		{
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Bege.SKM_Cap_Bege")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Black.SKM_Cap_Black")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Blue.SKM_Cap_Blue")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Color.SKM_Cap_Color")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Red.SKM_Cap_Red")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Street.SKM_Cap_Street")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Cowboy/SKM_Hat_Cowboy.SKM_Hat_Cowboy")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Farmer/SKM_Hat_Cowboy.SKM_Hat_Cowboy")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Blue.SKM_Hat_Flat_Blue")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Brown.SKM_Hat_Flat_Brown")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Grey.SKM_Hat_Flat_Grey")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Up/SKM_Hat_Up.SKM_Hat_Up")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Blue.SKM_Helmet_Worker_Blue")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Green.SKM_Helmet_Worker_Green")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Orange.SKM_Helmet_Worker_Orange")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Red.SKM_Helmet_Worker_Red")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_White.SKM_Helmet_Worker_White")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Yellow.SKM_Helmet_Worker_Yellow")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Bege.SKM_Warm_Hat_Bege")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Black.SKM_Warm_Hat_Black")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Gray.SKM_Warm_Hat_Gray")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Green.SKM_Warm_Hat_Green"))
		};
		return Paths;
	}

	const TSet<FSoftObjectPath>& AvGetHeadphonesCompatibleHeadgear()
	{
		// Product rule, exact-path and deliberately narrower than visual similarity:
		// only the ordinary Cap family may be combined with working headphones.
		static const TSet<FSoftObjectPath> Paths =
		{
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Bege.SKM_Cap_Bege")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Black.SKM_Cap_Black")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Blue.SKM_Cap_Blue")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Color.SKM_Cap_Color")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Red.SKM_Cap_Red")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Street.SKM_Cap_Street"))
		};
		return Paths;
	}

	const TSet<FSoftObjectPath>& AvGetRaisedHoodConflictingHeadphones()
	{
		// Exact-path audit: both product families use large circumaural cups and a
		// crown band inside Hood_2. The Micro family also drives a boom through the
		// hood opening. Color variants share their family's imported geometry.
		static const TSet<FSoftObjectPath> Paths =
		{
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Green.SKM_Headphones_Worker_Green")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Orange.SKM_Headphones_Worker_Orange")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_White.SKM_Headphones_Worker_White")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Yellow.SKM_Headphones_Worker_Yellow")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Gr.SKM_Headph_Worker_Mic_Gr")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Orn.SKM_Headph_Worker_Mic_Orn")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Wht.SKM_Headph_Worker_Mic_Wht")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Yel.SKM_Headph_Worker_Mic_Yel"))
		};
		return Paths;
	}

	enum class EAvRandomVisualCondition : uint8
	{
		Unverified,
		Intact,
		AuthoredDamage,
		OpacityMaskDamage,
		InvalidLODMaterialSection,
		BodyClipping,
		LayerConflict,
		MissingRequiredUnderlayer,
		Quarantined
	};

	enum class EAvRandomAuditStatus : uint8
	{
		Unverified,
		VerifiedIntact,
		VerifiedDamaged,
		Quarantined
	};

	struct FAvRandomAssetEligibility
	{
		FSoftObjectPath AssetPath;
		EWorkerSlot Slot = EWorkerSlot::Body;
		bool bAllowInRandom = false;
		EAvRandomAuditStatus AuditStatus = EAvRandomAuditStatus::Unverified;
		EAvRandomVisualCondition VisualCondition = EAvRandomVisualCondition::Unverified;
		FString SharedGeometryGroup;
		FString SharedMaterialGroup;
		FString AuditEvidence;
		FString LastAuditDate;
		FString AuditVersion;
		FString QuarantineReason;
		FSoftObjectPath RequiredUnderlayer;
		TArray<FSoftObjectPath> IncompatibleAssets;
		TArray<FString> RequiredFamilies;
		TArray<FString> IncompatibleFamilies;
		FString BodyCoverageRequirement;
		bool bHidesHair = false;
	};

	const TCHAR* AvAuditStatusName(EAvRandomAuditStatus Status)
	{
		switch (Status)
		{
		case EAvRandomAuditStatus::VerifiedIntact: return TEXT("AssetVerifiedIntact");
		case EAvRandomAuditStatus::VerifiedDamaged: return TEXT("VerifiedDamaged");
		case EAvRandomAuditStatus::Quarantined: return TEXT("Quarantined");
		default: return TEXT("Unverified");
		}
	}

	const TCHAR* AvVisualConditionName(EAvRandomVisualCondition Condition)
	{
		switch (Condition)
		{
		case EAvRandomVisualCondition::Intact: return TEXT("Intact");
		case EAvRandomVisualCondition::AuthoredDamage: return TEXT("AuthoredDamage");
		case EAvRandomVisualCondition::OpacityMaskDamage: return TEXT("OpacityMaskDamage");
		case EAvRandomVisualCondition::InvalidLODMaterialSection: return TEXT("InvalidLODMaterialSection");
		case EAvRandomVisualCondition::BodyClipping: return TEXT("BodyClipping");
		case EAvRandomVisualCondition::LayerConflict: return TEXT("LayerConflict");
		case EAvRandomVisualCondition::MissingRequiredUnderlayer: return TEXT("MissingRequiredUnderlayer");
		case EAvRandomVisualCondition::Quarantined: return TEXT("Quarantined");
		default: return TEXT("Unverified");
		}
	}

	FSoftObjectPath AvExactMeshPath(const TCHAR* PackagePath)
	{
		const FString Package(PackagePath);
		return FSoftObjectPath(Package + TEXT(".") + FPackageName::GetLongPackageAssetName(Package));
	}

	FString AvSharedGeometryGroupForPackage(const TCHAR* PackagePath)
	{
		const FString Package(PackagePath);
		const FString Folder = FPackageName::GetLongPackagePath(Package);
		return FPackageName::GetLongPackageAssetName(Folder);
	}

	const TSet<FString>& AvGetQuarantinedSharedGeometryGroups()
	{
		static const TSet<FString> Groups =
		{
			TEXT("Pants_Worker_Full"),
			TEXT("Jacket_Worker"),
			TEXT("Gloves_Worker")
		};
		return Groups;
	}

	FString AvDefaultAuditEvidence(EWorkerSlot Slot, const TCHAR* PackagePath)
	{
		const FString AssetName = FPackageName::GetLongPackageAssetName(PackagePath);
		if (Slot == EWorkerSlot::Torso ||
			Slot == EWorkerSlot::Legs ||
			Slot == EWorkerSlot::FullOutfit)
		{
			const TCHAR* ClothingSlot =
				Slot == EWorkerSlot::Torso ? TEXT("Torso") :
					Slot == EWorkerSlot::Legs ? TEXT("Lower") : TEXT("FullOutfit");
			return FString::Printf(
				TEXT("Saved/CodexScreenshots/RandomVisualIntegrityFix_20260712/ExhaustiveAssetAudit/%s/%s"),
				ClothingSlot,
				*AssetName);
		}

		const TCHAR* AccessorySlot =
			Slot == EWorkerSlot::Hair ? TEXT("Hair") :
				Slot == EWorkerSlot::Beard ? TEXT("Beard") :
					Slot == EWorkerSlot::Gloves ? TEXT("Gloves") :
						Slot == EWorkerSlot::Headgear ? TEXT("Headgear") :
							Slot == EWorkerSlot::FaceMask ? TEXT("FaceMask") :
								Slot == EWorkerSlot::Glasses ? TEXT("Glasses") :
									Slot == EWorkerSlot::Vest ? TEXT("Vest") : TEXT("Other");
		return FString::Printf(
			TEXT("Saved/CodexScreenshots/RandomVisualIntegrityFix_20260712/AssetAudit/Accessories/accessory_%s_%s.png"),
			AccessorySlot,
			*AssetName);
	}

	bool AvAuditEvidenceExists(const FString& AuditEvidence)
	{
		if (AuditEvidence.IsEmpty())
		{
			return false;
		}
		const FString AbsolutePath = FPaths::ConvertRelativePathToFull(
			FPaths::Combine(FPaths::ProjectDir(), AuditEvidence));
		return IFileManager::Get().FileExists(*AbsolutePath) ||
			IFileManager::Get().DirectoryExists(*AbsolutePath);
	}

	const TMap<FSoftObjectPath, FAvRandomAssetEligibility>& AvGetRandomAssetEligibility()
	{
		static const TMap<FSoftObjectPath, FAvRandomAssetEligibility> Eligibility = []
		{
			TMap<FSoftObjectPath, FAvRandomAssetEligibility> Result;
			auto Add = [&Result](
				EWorkerSlot Slot,
				const TCHAR* PackagePath,
				EAvRandomAuditStatus AuditStatus,
				EAvRandomVisualCondition Condition,
				const TCHAR* Reason = TEXT(""),
				const TCHAR* RequiredUnderlayer = TEXT(""),
				const TCHAR* BodyCoverage = TEXT("CanonicalFactoryBodyCompatible"),
				const TCHAR* SharedGeometryGroup = TEXT(""),
				const TCHAR* SharedMaterialGroup = TEXT(""),
				const TCHAR* AuditEvidence = TEXT(""))
			{
				FAvRandomAssetEligibility Entry;
				Entry.AssetPath = AvExactMeshPath(PackagePath);
				Entry.Slot = Slot;
				Entry.AuditStatus = AuditStatus;
				Entry.VisualCondition = Condition;
				Entry.bAllowInRandom =
					AuditStatus == EAvRandomAuditStatus::VerifiedIntact &&
					Condition == EAvRandomVisualCondition::Intact;
				Entry.SharedGeometryGroup =
					SharedGeometryGroup && SharedGeometryGroup[0] != 0
						? SharedGeometryGroup
						: AvSharedGeometryGroupForPackage(PackagePath);
				Entry.SharedMaterialGroup =
					SharedMaterialGroup && SharedMaterialGroup[0] != 0
						? SharedMaterialGroup
						: FString::Printf(TEXT("%s_MaterialVariants"), *Entry.SharedGeometryGroup);
				Entry.AuditEvidence =
					AuditEvidence && AuditEvidence[0] != 0
						? AuditEvidence
						: AvDefaultAuditEvidence(Slot, PackagePath);
				Entry.LastAuditDate = TEXT("2026-07-13");
				Entry.AuditVersion = TEXT("RVI-20260713-v2");
				Entry.QuarantineReason = Reason;
				if (RequiredUnderlayer && RequiredUnderlayer[0] != 0)
				{
					Entry.RequiredUnderlayer = AvExactMeshPath(RequiredUnderlayer);
				}
				Entry.BodyCoverageRequirement = BodyCoverage;
				Result.Add(Entry.AssetPath, MoveTemp(Entry));
			};
			auto VerifyIntact = [&Add](EWorkerSlot Slot, const TCHAR* PackagePath)
			{
				Add(Slot, PackagePath,
					EAvRandomAuditStatus::VerifiedIntact,
					EAvRandomVisualCondition::Intact);
			};
			auto Quarantine = [&Add](
				EWorkerSlot Slot,
				const TCHAR* PackagePath,
				EAvRandomVisualCondition Condition,
				const TCHAR* Reason)
			{
				const bool bProvenVisualDamage =
					Condition == EAvRandomVisualCondition::AuthoredDamage ||
					Condition == EAvRandomVisualCondition::OpacityMaskDamage ||
					Condition == EAvRandomVisualCondition::BodyClipping ||
					Condition == EAvRandomVisualCondition::LayerConflict;
				Add(Slot, PackagePath,
					bProvenVisualDamage
						? EAvRandomAuditStatus::VerifiedDamaged
						: EAvRandomAuditStatus::Quarantined,
					Condition, Reason);
			};

			// Torso meshes visually checked with the canonical factory Body, front and side, LOD0.
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Gray"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Gray"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_RedCage"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_White_Cage"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Black"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Blue") })
			{
				VerifyIntact(EWorkerSlot::Torso, Path);
			}

			// Lower-body meshes include authored footwear and were checked at waist, crotch and boots.
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Gray"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Green") })
			{
				VerifyIntact(EWorkerSlot::Legs, Path);
			}

			// Five visually intact complete outfits; 6 and 7 are authored torn variants.
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_1"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_2"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_3"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_4"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_5") })
			{
				VerifyIntact(EWorkerSlot::FullOutfit, Path);
			}

			// Optional accessories were inspected on the canonical Head/Body. Exact paths only.
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Back"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Long"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Mohawk"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Short"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hair/SKM_Hair_Skin") })
			{
				VerifyIntact(EWorkerSlot::Hair, Path);
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_02"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_03"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_4"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_Long"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Beard_Long_2"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Moustache") })
			{
				VerifyIntact(EWorkerSlot::Beard, Path);
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Orange"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Red"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_White"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Yellow"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Black"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Color"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Red"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Street"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Cowboy/SKM_Hat_Cowboy"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Farmer/SKM_Hat_Cowboy"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Brown"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Grey"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Up/SKM_Hat_Up"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Black"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Gray"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Green") })
			{
				VerifyIntact(EWorkerSlot::Headgear, Path);
				if (FAvRandomAssetEligibility* Entry = Result.Find(AvExactMeshPath(Path)))
				{
					// Exact structured policy: every currently audited product headgear fully
					// replaces the visible hair layer while equipped. The Hair slot itself is retained.
					Entry->bHidesHair = true;
					Entry->IncompatibleFamilies.Add(
						TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1"));
					Entry->IncompatibleFamilies.Add(
						TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2"));
				}
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Respirator/SKM_Respirator"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Respirator/SKM_Respirator_Black") })
			{
				VerifyIntact(EWorkerSlot::FaceMask, Path);
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator_Brown"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator_Gold"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_Classic"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_SunGlass"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Worker/SKM_Glasses_Worker_Orange"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Worker/SKM_Glasses_Worker_White") })
			{
				VerifyIntact(EWorkerSlot::Glasses, Path);
			}

			// Exact-path quarantines for defects proven during the 2026-07-12 visual audit.
			// The 2026-07-13 Inspector isolation proved a large authored opening immediately
			// above both cuffs on the blue Jacket_Worker. Beige and gray use the same sleeve
			// geometry/material-mask layout, so the complete shared group remains denied until
			// every exact variant has independent clean evidence.
			Add(EWorkerSlot::Torso,
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Blue"),
				EAvRandomAuditStatus::VerifiedDamaged,
				EAvRandomVisualCondition::OpacityMaskDamage,
				TEXT("GloveIntegrity20260713; SleeveCuffDamage; Authored opening above both reflective cuffs"),
				TEXT(""), TEXT("CanonicalFactoryBodyCompatible"),
				TEXT("Jacket_Worker"), TEXT("Jacket_Worker_SharedSleeveMaskLayout"),
				TEXT("Saved/CodexScreenshots/GloveIntegrityFix_20260713/Isolation"));
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Gray") })
			{
				Add(EWorkerSlot::Torso, Path,
					EAvRandomAuditStatus::Quarantined,
					EAvRandomVisualCondition::Quarantined,
					TEXT("GloveIntegrity20260713; SharedSleeveGeometryOrMask; NeedsIndependentCleanCuffAudit"),
					TEXT(""), TEXT("CanonicalFactoryBodyCompatible"),
					TEXT("Jacket_Worker"), TEXT("Jacket_Worker_SharedSleeveMaskLayout"),
					TEXT("Saved/CodexScreenshots/GloveIntegrityFix_20260713/ExhaustiveAudit"));
			}

			// Both worker-glove colors are intentionally open at the fingertips. Inspector Solo
			// without Body shows the shortened glove geometry; enabling the canonical Body exposes
			// skin at every opening. This fails Random's full-hand visual-integrity requirement.
			Add(EWorkerSlot::Gloves,
				TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker/SKM_Gloves_Worker_Brown"),
				EAvRandomAuditStatus::VerifiedDamaged,
				EAvRandomVisualCondition::BodyClipping,
				TEXT("GloveIntegrity20260713; AuthoredOpenFingertips; CanonicalBodyFingersVisible; SharedOpenFingerGeometry"),
				TEXT(""), TEXT("InsufficientFullHandCoverage"),
				TEXT("Gloves_Worker"), TEXT("Gloves_Worker_SharedNormalORMAlphaLayout"),
				TEXT("Saved/CodexScreenshots/GloveIntegrityFix_20260713/Isolation"));
			Add(EWorkerSlot::Gloves,
				TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker/SKM_Gloves_Worker"),
				EAvRandomAuditStatus::Quarantined,
				EAvRandomVisualCondition::Quarantined,
				TEXT("GloveIntegrity20260713; SharedOpenFingerGeometry; NeedsIndependentColorAudit"),
				TEXT(""), TEXT("InsufficientFullHandCoverage"),
				TEXT("Gloves_Worker"), TEXT("Gloves_Worker_SharedNormalORMAlphaLayout"),
				TEXT("Saved/CodexScreenshots/GloveIntegrityFix_20260713/ExhaustiveAudit/Body/SKM_Gloves_Worker"));

			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Chest_Modules/Vest_Worker/SKM_Vest_Worker_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Chest_Modules/Vest_Worker/SKM_Vest_Worker_LimeGreen"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Chest_Modules/Vest_Worker/SKM_Vest_Worker_Orange"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Chest_Modules/Vest_Worker/SKM_Vest_Worker_Yellow") })
			{
				Quarantine(EWorkerSlot::Vest, Path, EAvRandomVisualCondition::OpacityMaskDamage,
					TEXT("Authored repeated opacity-mask holes expose the torso underlayer"));
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jumpsuit/SKM_Jumpsuit_DarkBlue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jumpsuit/SKM_Jumpsuit_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jumpsuit/SKM_Jumpsuit_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jumpsuit/SKM_Jumpsuit_Black") })
			{
				Quarantine(EWorkerSlot::Legs, Path, EAvRandomVisualCondition::InvalidLODMaterialSection,
					TEXT("LOD0 section 1 referenced material index 2 with fallback to index 1; family also failed crotch/bib visual audit"));
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_LightGreen"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Orange"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_LightGr"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Orange"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_LightGr"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Orange"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Gray"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Red") })
			{
				Quarantine(EWorkerSlot::Torso, Path, EAvRandomVisualCondition::OpacityMaskDamage,
					TEXT("Authored torn alpha-mask variant exposes body or underlayer"));
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_LightGreen"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Orange") })
			{
				Quarantine(EWorkerSlot::Legs, Path, EAvRandomVisualCondition::OpacityMaskDamage,
					TEXT("Authored torn alpha-mask variant exposes body or underlayer"));
			}
			// Post-fix regression: Pants_Worker_Full is a lower-slot mesh whose authored geometry
			// extends through the chest bib. Blue/Gray/Green reproduced the same large torn bib
			// openings with different hood Torso assets, so the complete shared-geometry group is
			// denied. No color variant is inherited as safe from another exact asset.
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Gray"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_LightGrn"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Orange") })
			{
				Quarantine(EWorkerSlot::Legs, Path, EAvRandomVisualCondition::OpacityMaskDamage,
					TEXT("PostFixVisualRegression; AuthoredBibDamage; SharedDamagedGeometryOrMask; NeedsDeterministicAssetAudit"));
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_M65/SKM_Jacket_M65_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_M65/SKM_Jacket_M65_Black"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_M65/SKM_Jacket_M65_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Black"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Red"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_White"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Black"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Green"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Grey"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Red"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_White") })
			{
				Quarantine(EWorkerSlot::Torso, Path, EAvRandomVisualCondition::OpacityMaskDamage,
					TEXT("Authored ripped material-mask variant exposes canonical Body"));
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Bege"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Black"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Blue"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Turn") })
			{
				Quarantine(EWorkerSlot::Legs, Path, EAvRandomVisualCondition::MissingRequiredUnderlayer,
					TEXT("No visually verified standalone footwear candidate exists for this lower-only family"));
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_6"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_7"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_2"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_1"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_2"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_3"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_4"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_5") })
			{
				Quarantine(EWorkerSlot::FullOutfit, Path, EAvRandomVisualCondition::AuthoredDamage,
					TEXT("Authored torn side/crotch/bib region failed canonical Body visual audit"));
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_1"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_2"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_3") })
			{
				Quarantine(EWorkerSlot::FullOutfit, Path, EAvRandomVisualCondition::LayerConflict,
					TEXT("Combined shirt layer inherits the damaged rolled-shirt material family"));
			}
			for (const TCHAR* Path : {
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_1"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_2"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_3"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_4"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_5"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_1"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_2"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_3"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_4"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_5"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_1"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_3"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_4"),
				TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_5") })
			{
				Quarantine(EWorkerSlot::FullOutfit, Path, EAvRandomVisualCondition::Unverified,
					TEXT("Not proven visually intact at crotch, side and all authored material sections"));
			}

			return Result;
		}();
		return Eligibility;
	}

	const FAvRandomAssetEligibility* AvFindRandomAssetEligibility(
		EWorkerSlot Slot,
		const FSoftObjectPath& AssetPath)
	{
		const FAvRandomAssetEligibility* Entry = AvGetRandomAssetEligibility().Find(AssetPath);
		return Entry && Entry->Slot == Slot ? Entry : nullptr;
	}

	struct FAvRandomCandidate
	{
		FAssetData Asset;
		const FAvRandomFamilyDefinition* Family = nullptr;
		const FAvRandomAssetEligibility* Eligibility = nullptr;
	};

	const FAvRandomFamilyDefinition* AvFindRandomFamily(
		EWorkerSlot Slot,
		const FSoftObjectPath& AssetPath)
	{
		const FString PackagePath = AssetPath.GetLongPackageName();
		for (const FAvRandomFamilyDefinition& Family : RandomFamilies)
		{
			const FString Folder(Family.Folder);
			if (Family.Slot == Slot &&
				(PackagePath == Folder || PackagePath.StartsWith(Folder + TEXT("/"))))
			{
				return &Family;
			}
		}
		return nullptr;
	}

	void AvScanRandomFolder(
		const FAvRandomFamilyDefinition& Family,
		TArray<FAvRandomCandidate>& OutAssets,
		int32& OutStructurallyEligibleCount)
	{
		IAssetRegistry& AssetRegistry =
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
		FARFilter Filter;
		Filter.PackagePaths.Add(FName(Family.Folder));
		Filter.bRecursivePaths = true;
		Filter.ClassPaths.Add(USkeletalMesh::StaticClass()->GetClassPathName());
		TArray<FAssetData> RawAssets;
		AssetRegistry.GetAssets(Filter, RawAssets);
		for (const FAssetData& Asset : RawAssets)
		{
			const FString AssetName = Asset.AssetName.ToString();
			const FString PackageName = Asset.PackageName.ToString();
			FString SkeletonTag;
			Asset.GetTagValue(FName(TEXT("Skeleton")), SkeletonTag);
			if (!AssetName.StartsWith(TEXT("SKM_")) ||
				PackageName.Contains(TEXT("/Presets/")) ||
				PackageName.Contains(TEXT("/Demo/")) ||
				PackageName.Contains(TEXT("/Editor/")) ||
				PackageName.Contains(TEXT("/Female/")) ||
				!SkeletonTag.Contains(CompatibleMaleSkeletonPath))
			{
				continue;
			}
			++OutStructurallyEligibleCount;
			const FSoftObjectPath AssetPath = Asset.GetSoftObjectPath();
			const FAvRandomAssetEligibility* Eligibility =
				AvFindRandomAssetEligibility(Family.Slot, AssetPath);
			const bool bSharedGeometryQuarantined = Eligibility &&
				AvGetQuarantinedSharedGeometryGroups().Contains(Eligibility->SharedGeometryGroup);
			if (!Eligibility || !Eligibility->bAllowInRandom ||
				Eligibility->AuditStatus != EAvRandomAuditStatus::VerifiedIntact ||
				Eligibility->VisualCondition != EAvRandomVisualCondition::Intact ||
				bSharedGeometryQuarantined)
			{
				static TSet<FSoftObjectPath> LoggedRejections;
				if (!LoggedRejections.Contains(AssetPath))
				{
					LoggedRejections.Add(AssetPath);
					UE_LOG(LogTemp, Warning,
						TEXT("[AvCustomizeRandomEligibility] Result=REJECT Slot=%d SelectedExactPath=%s EligibilityExactPath=%s AuditStatus=%s VisualCondition=%s Allow=%s SharedGeometryGroup=%s GroupQuarantined=%s Reason=%s"),
						static_cast<int32>(Family.Slot),
						*AssetPath.ToString(),
						Eligibility ? *Eligibility->AssetPath.ToString() : TEXT("missing"),
						Eligibility ? AvAuditStatusName(Eligibility->AuditStatus) : TEXT("Unverified"),
						Eligibility ? AvVisualConditionName(Eligibility->VisualCondition) : TEXT("Unverified"),
						Eligibility && Eligibility->bAllowInRandom ? TEXT("true") : TEXT("false"),
						Eligibility ? *Eligibility->SharedGeometryGroup : TEXT("none"),
						bSharedGeometryQuarantined ? TEXT("true") : TEXT("false"),
						Eligibility && !Eligibility->QuarantineReason.IsEmpty()
							? *Eligibility->QuarantineReason
							: TEXT("No exact-path AssetVerifiedIntact eligibility metadata"));
				}
				continue;
			}
			FAvRandomCandidate Candidate;
			Candidate.Asset = Asset;
			Candidate.Family = &Family;
			Candidate.Eligibility = Eligibility;
			OutAssets.Add(MoveTemp(Candidate));
		}
	}

	void AvSortAndUnique(TArray<FAvRandomCandidate>& Assets)
	{
		Assets.Sort([](const FAvRandomCandidate& A, const FAvRandomCandidate& B)
		{
			return A.Asset.PackageName.LexicalLess(B.Asset.PackageName);
		});
		for (int32 Index = Assets.Num() - 1; Index > 0; --Index)
		{
			if (Assets[Index].Asset.PackageName == Assets[Index - 1].Asset.PackageName)
			{
				Assets.RemoveAt(Index);
			}
		}
	}

	const FAvRandomCandidate* AvChooseOptionalSlot(
		FWorkerAppearance& Appearance,
		EWorkerSlot Slot,
		const TArray<FAvRandomCandidate>& Candidates)
	{
		const int32 Choice = FMath::RandRange(0, Candidates.Num());
		if (Choice == 0)
		{
			Appearance.Clear(Slot);
			return nullptr;
		}
		const FAvRandomCandidate& Candidate = Candidates[Choice - 1];
		Appearance.Set(
			Slot,
			TSoftObjectPtr<USkeletalMesh>(Candidate.Asset.GetSoftObjectPath()));
		return &Candidate;
	}

	const FAvRandomCandidate* AvChooseRequiredSlot(
		FWorkerAppearance& Appearance,
		EWorkerSlot Slot,
		const TArray<FAvRandomCandidate>& Candidates)
	{
		if (Candidates.IsEmpty())
		{
			Appearance.Clear(Slot);
			return nullptr;
		}
		const FAvRandomCandidate& Candidate = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
		Appearance.Set(Slot, TSoftObjectPtr<USkeletalMesh>(Candidate.Asset.GetSoftObjectPath()));
		return &Candidate;
	}

	TArray<FAvHeadCustomizationCatalogItem> AvGetCanonicalRandomHeadCandidates()
	{
		TArray<FAvHeadCustomizationCatalogItem> Candidates;
		TSet<FString> SeenStableIds;
		TSet<FString> SeenPaths;
		for (const FAvHeadCustomizationCatalogItem& Item :
			UWorkerAppearanceComponent::GetHeadCustomizationCatalog())
		{
			const FSoftObjectPath HeadPath(Item.ExactHeadAssetPath);
			const bool bValidRecord =
				Item.UISection == EAvHeadCustomizationSection::HeadType &&
				Item.Slot == EWorkerSlot::Head &&
				Item.bAllowInUI &&
				!Item.StableId.IsEmpty() &&
				!Item.HeadTypeId.IsNone() &&
				HeadPath.IsValid() &&
				!HeadPath.IsNull() &&
				Item.ExactHeadAssetPath == Item.ExactObjectPath &&
				!SeenStableIds.Contains(Item.StableId) &&
				!SeenPaths.Contains(Item.ExactHeadAssetPath);
			if (!bValidRecord)
			{
				continue;
			}
			SeenStableIds.Add(Item.StableId);
			SeenPaths.Add(Item.ExactHeadAssetPath);
			Candidates.Add(Item);
		}
		Candidates.Sort([](
			const FAvHeadCustomizationCatalogItem& A,
			const FAvHeadCustomizationCatalogItem& B)
		{
			return A.StableId < B.StableId;
		});
		return Candidates;
	}

	bool AvSelectCanonicalRandomHeadType(
		FWorkerAppearance& InOutAppearance,
		FAvRandomHeadSelectionDiagnostics* OutDiagnostics)
	{
		FAvRandomHeadSelectionDiagnostics LocalDiagnostics;
		FAvRandomHeadSelectionDiagnostics& Diagnostics = OutDiagnostics
			? *OutDiagnostics
			: LocalDiagnostics;
		Diagnostics = FAvRandomHeadSelectionDiagnostics();

		const TArray<FAvHeadCustomizationCatalogItem> Candidates =
			AvGetCanonicalRandomHeadCandidates();
		Diagnostics.CandidateCount = Candidates.Num();
		for (const FAvHeadCustomizationCatalogItem& Candidate : Candidates)
		{
			Diagnostics.CandidateStableIds.Add(Candidate.StableId);
			Diagnostics.CandidatePaths.Add(Candidate.ExactHeadAssetPath);
		}
		if (Candidates.IsEmpty())
		{
			InOutAppearance.Clear(EWorkerSlot::Head);
			Diagnostics.ValidationResult = TEXT("Result=FAIL Reason=NoCanonicalRandomHeadCandidates");
			return false;
		}

		Diagnostics.SelectedIndex = FMath::RandRange(0, Candidates.Num() - 1);
		const FAvHeadCustomizationCatalogItem& Selected =
			Candidates[Diagnostics.SelectedIndex];
		Diagnostics.SelectedStableId = Selected.StableId;
		Diagnostics.SelectedPath = Selected.ExactHeadAssetPath;
		InOutAppearance.Set(
			EWorkerSlot::Head,
			TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Selected.ExactHeadAssetPath)));

		FString NormalizationDetails;
		UWorkerAppearanceComponent::NormalizeHeadTypeSkinPresentation(
			InOutAppearance,
			&NormalizationDetails);
		if (const FAvHeadTypePresentation* Presentation =
			AvFindHeadTypePresentation(FSoftObjectPath(Selected.ExactHeadAssetPath)))
		{
			Diagnostics.ResolvedProfile = Presentation->SkinProfileId.ToString();
		}
		FString ValidationDetails;
		const bool bValid = UWorkerAppearanceComponent::ValidateHeadTypeSkinPresentation(
			InOutAppearance,
			ValidationDetails);
		Diagnostics.ValidationResult = ValidationDetails;
		UE_LOG(LogTemp, Log,
			TEXT("[AvCustomizeRandomHead] CandidateCount=%d StableIds=%s Paths=%s SelectedIndex=%d SelectedStableId=%s SelectedPath=%s ResolvedProfile=%s Validation={%s} Fallback=false SeedSource=%s"),
			Diagnostics.CandidateCount,
			*FString::Join(Diagnostics.CandidateStableIds, TEXT(",")),
			*FString::Join(Diagnostics.CandidatePaths, TEXT(",")),
			Diagnostics.SelectedIndex,
			*Diagnostics.SelectedStableId,
			*Diagnostics.SelectedPath,
			*Diagnostics.ResolvedProfile,
			*Diagnostics.ValidationResult,
			*Diagnostics.SeedSource);
		return bValid;
	}

	int32 AvApplyRandomCompatibilityCleanup(FWorkerAppearance& Appearance)
	{
		int32 CleanupCount = 0;
		if (!Appearance.Get(EWorkerSlot::FullOutfit).IsNull())
		{
			for (const EWorkerSlot ConflictingSlot :
				{ EWorkerSlot::Torso, EWorkerSlot::Legs, EWorkerSlot::Feet, EWorkerSlot::Vest })
			{
				if (!Appearance.Get(ConflictingSlot).IsNull())
				{
					Appearance.Clear(ConflictingSlot);
					++CleanupCount;
				}
			}
		}

		const FAvRandomFamilyDefinition* UpperFamily = AvFindRandomFamily(
			EWorkerSlot::Torso,
			Appearance.Get(EWorkerSlot::Torso).ToSoftObjectPath());
		if (UpperFamily && UpperFamily->bHasIntegratedHood)
		{
			for (const EWorkerSlot ConflictingSlot :
				{ EWorkerSlot::Headgear, EWorkerSlot::Hair, EWorkerSlot::Headphones })
			{
				if (!Appearance.Get(ConflictingSlot).IsNull())
				{
					Appearance.Clear(ConflictingSlot);
					++CleanupCount;
				}
			}
		}
		else if (!Appearance.Get(EWorkerSlot::Headgear).IsNull() &&
			!Appearance.Get(EWorkerSlot::Hair).IsNull())
		{
			Appearance.Clear(EWorkerSlot::Hair);
			++CleanupCount;
		}
		if (UWorkerAppearanceComponent::HeadgearConflictsWithHeadphones(
			Appearance.Get(EWorkerSlot::Headgear),
			Appearance.Get(EWorkerSlot::Headphones)))
		{
			// Deterministic Random policy: an incompatible Headgear wins and Headphones
			// are cleared. Current Random does not select Headphones, so this is also
			// future-proofing rather than a catalog expansion.
			Appearance.Clear(EWorkerSlot::Headphones);
			++CleanupCount;
		}
		return CleanupCount;
	}

	bool AvHasSameSlotMeshes(const FWorkerAppearance& A, const FWorkerAppearance& B)
	{
		for (int32 SlotValue = static_cast<int32>(EWorkerSlot::Body);
			SlotValue <= static_cast<int32>(EWorkerSlot::Overalls);
			++SlotValue)
		{
			const EWorkerSlot Slot = static_cast<EWorkerSlot>(SlotValue);
			if (A.Get(Slot).ToSoftObjectPath() != B.Get(Slot).ToSoftObjectPath())
			{
				return false;
			}
		}
		return true;
	}

	bool AvTryBuildSafeRandomFallback(
		const TMap<EWorkerSlot, TArray<FAvRandomCandidate>>& Candidates,
		FWorkerAppearance& OutAppearance)
	{
		OutAppearance = UWorkerAppearanceComponent::MakeBaseMaleUnderwearAppearance();
		const TArray<FAvRandomCandidate>* UpperCandidates = Candidates.Find(EWorkerSlot::Torso);
		const TArray<FAvRandomCandidate>* LowerCandidates = Candidates.Find(EWorkerSlot::Legs);
		if (!UpperCandidates || !LowerCandidates)
		{
			return false;
		}

		const FAvRandomCandidate* SafeUpper = UpperCandidates->FindByPredicate(
			[](const FAvRandomCandidate& Candidate)
			{
				return Candidate.Family && Candidate.Family->bCoversUpperBody &&
					!Candidate.Family->bHasIntegratedHood;
			});
		const FAvRandomCandidate* SafeLower = LowerCandidates->FindByPredicate(
			[](const FAvRandomCandidate& Candidate)
			{
				return Candidate.Family && Candidate.Family->bCoversLowerBody &&
					Candidate.Family->bIncludesFootwear;
			});
		if (!SafeUpper || !SafeLower)
		{
			return false;
		}

		OutAppearance.Set(EWorkerSlot::Torso,
			TSoftObjectPtr<USkeletalMesh>(SafeUpper->Asset.GetSoftObjectPath()));
		OutAppearance.Set(EWorkerSlot::Legs,
			TSoftObjectPtr<USkeletalMesh>(SafeLower->Asset.GetSoftObjectPath()));
		return true;
	}
}

bool UWorkerAppearanceComponent::ValidateRandomGeneratedMaleAppearance(
	const FWorkerAppearance& InAppearance,
	FString& OutValidationDetails)
{
	auto Fail = [&OutValidationDetails](const TCHAR* Reason)
	{
		OutValidationDetails = FString::Printf(TEXT("Reason=%s"), Reason);
		return false;
	};
	auto FailAsset = [&OutValidationDetails](
		const TCHAR* Reason,
		EWorkerSlot Slot,
		const FSoftObjectPath& AssetPath,
		const TCHAR* Condition,
		const FString& Details)
	{
		OutValidationDetails = FString::Printf(
			TEXT("Reason=%s Slot=%d Asset=%s VisualCondition=%s Details=%s"),
			Reason,
			static_cast<int32>(Slot),
			*AssetPath.ToString(),
			Condition,
			Details.IsEmpty() ? TEXT("none") : *Details);
		return false;
	};

	if (InAppearance.Get(EWorkerSlot::Body).ToSoftObjectPath() != FSoftObjectPath(AvWorkerAssets::FactoryBody))
	{
		return Fail(TEXT("MissingFactoryBody"));
	}
	FAvHeadCustomizationCatalogItem HeadMetadata;
	if (!TryGetHeadTypeMetadata(InAppearance.Get(EWorkerSlot::Head), HeadMetadata) ||
		!HeadMetadata.bAllowInUI ||
		HeadMetadata.UISection != EAvHeadCustomizationSection::HeadType ||
		HeadMetadata.Slot != EWorkerSlot::Head ||
		HeadMetadata.ExactHeadAssetPath.IsEmpty())
	{
		return Fail(TEXT("MissingCanonicalRandomHeadType"));
	}
	FString SkinPresentationValidation;
	if (!ValidateHeadTypeSkinPresentation(InAppearance, SkinPresentationValidation))
	{
		return Fail(TEXT("HeadTypeSkinToneMismatch"));
	}

	for (const FWorkerSlotMesh& SlotMesh : InAppearance.Slots)
	{
		const FSoftObjectPath SoftPath = SlotMesh.Mesh.ToSoftObjectPath();
		const FString AssetPath = SoftPath.ToString();
		if (AssetPath.Contains(TEXT("/Presets/")))
		{
			return Fail(TEXT("PresetAsset"));
		}
		if (AssetPath.Contains(TEXT("/Female/")) ||
			AssetPath.Contains(TEXT("/Demo/")) ||
			AssetPath.Contains(TEXT("/Editor/")))
		{
			return Fail(TEXT("DisallowedAssetScope"));
		}
		if (SlotMesh.Slot == EWorkerSlot::Body || SlotMesh.Slot == EWorkerSlot::Head)
		{
			continue;
		}

		const FAvRandomAssetEligibility* Eligibility =
			AvFindRandomAssetEligibility(SlotMesh.Slot, SoftPath);
		if (!Eligibility)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[AvCustomizeRandomEligibilityValidation] SelectedExactPath=%s EligibilityExactPath=missing AuditStatus=Unverified VisualCondition=Unverified Allow=false Result=REJECT Reason=VisualEligibilityMissing"),
				*SoftPath.ToString());
			return FailAsset(
				TEXT("VisualEligibilityMissing"), SlotMesh.Slot, SoftPath,
				TEXT("Unverified"),
				TEXT("No exact-path visually verified eligibility metadata"));
		}
		const bool bSharedGeometryQuarantined =
			AvGetQuarantinedSharedGeometryGroups().Contains(Eligibility->SharedGeometryGroup);
		if (!Eligibility->bAllowInRandom ||
			Eligibility->AuditStatus != EAvRandomAuditStatus::VerifiedIntact ||
			Eligibility->VisualCondition != EAvRandomVisualCondition::Intact ||
			bSharedGeometryQuarantined)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[AvCustomizeRandomEligibilityValidation] SelectedExactPath=%s EligibilityExactPath=%s AuditStatus=%s VisualCondition=%s Allow=%s SharedGeometryGroup=%s GroupQuarantined=%s Result=REJECT Reason=%s"),
				*SoftPath.ToString(),
				*Eligibility->AssetPath.ToString(),
				AvAuditStatusName(Eligibility->AuditStatus),
				AvVisualConditionName(Eligibility->VisualCondition),
				Eligibility->bAllowInRandom ? TEXT("true") : TEXT("false"),
				*Eligibility->SharedGeometryGroup,
				bSharedGeometryQuarantined ? TEXT("true") : TEXT("false"),
				Eligibility->QuarantineReason.IsEmpty()
					? TEXT("NotAssetVerifiedIntact") : *Eligibility->QuarantineReason);
			return FailAsset(
				TEXT("VisualEligibilityDenied"), SlotMesh.Slot, SoftPath,
				AvVisualConditionName(Eligibility->VisualCondition),
				Eligibility->QuarantineReason.IsEmpty()
					? FString::Printf(TEXT("AuditStatus=%s GroupQuarantined=%s"),
						AvAuditStatusName(Eligibility->AuditStatus),
						bSharedGeometryQuarantined ? TEXT("true") : TEXT("false"))
					: Eligibility->QuarantineReason);
		}
		if (Eligibility->AssetPath != SoftPath)
		{
			return FailAsset(
				TEXT("VisualEligibilityExactPathMismatch"), SlotMesh.Slot, SoftPath,
				AvVisualConditionName(Eligibility->VisualCondition),
				Eligibility->AssetPath.ToString());
		}
		if (Eligibility->AuditEvidence.IsEmpty())
		{
			return FailAsset(
				TEXT("VisualEligibilityMissingAuditEvidence"), SlotMesh.Slot, SoftPath,
				AvVisualConditionName(Eligibility->VisualCondition),
				TEXT("AssetVerifiedIntact requires non-empty per-asset evidence"));
		}
		if (!Eligibility->RequiredUnderlayer.IsNull())
		{
			bool bFoundRequiredUnderlayer = false;
			for (const FWorkerSlotMesh& OtherSlot : InAppearance.Slots)
			{
				if (OtherSlot.Mesh.ToSoftObjectPath() == Eligibility->RequiredUnderlayer)
				{
					bFoundRequiredUnderlayer = true;
					break;
				}
			}
			if (!bFoundRequiredUnderlayer)
			{
				return FailAsset(
					TEXT("MissingRequiredUnderlayer"), SlotMesh.Slot, SoftPath,
					AvVisualConditionName(Eligibility->VisualCondition),
					Eligibility->RequiredUnderlayer.ToString());
			}
		}
		for (const FString& RequiredFamily : Eligibility->RequiredFamilies)
		{
			bool bFoundRequiredFamily = false;
			for (const FWorkerSlotMesh& OtherSlot : InAppearance.Slots)
			{
				const FString OtherPackage = OtherSlot.Mesh.ToSoftObjectPath().GetLongPackageName();
				if (OtherPackage == RequiredFamily ||
					OtherPackage.StartsWith(RequiredFamily + TEXT("/")))
				{
					bFoundRequiredFamily = true;
					break;
				}
			}
			if (!bFoundRequiredFamily)
			{
				return FailAsset(
					TEXT("MissingRequiredFamily"), SlotMesh.Slot, SoftPath,
					AvVisualConditionName(Eligibility->VisualCondition),
					RequiredFamily);
			}
		}
		for (const FSoftObjectPath& IncompatibleAsset : Eligibility->IncompatibleAssets)
		{
			for (const FWorkerSlotMesh& OtherSlot : InAppearance.Slots)
			{
				if (OtherSlot.Mesh.ToSoftObjectPath() == IncompatibleAsset)
				{
					return FailAsset(
						TEXT("IncompatibleExactAsset"), SlotMesh.Slot, SoftPath,
						AvVisualConditionName(Eligibility->VisualCondition),
						IncompatibleAsset.ToString());
				}
			}
		}
		for (const FString& IncompatibleFamily : Eligibility->IncompatibleFamilies)
		{
			for (const FWorkerSlotMesh& OtherSlot : InAppearance.Slots)
			{
				const FString OtherPackage = OtherSlot.Mesh.ToSoftObjectPath().GetLongPackageName();
				if (OtherPackage == IncompatibleFamily ||
					OtherPackage.StartsWith(IncompatibleFamily + TEXT("/")))
				{
					return FailAsset(
						TEXT("IncompatibleFamily"), SlotMesh.Slot, SoftPath,
						AvVisualConditionName(Eligibility->VisualCondition),
						IncompatibleFamily);
				}
			}
		}
		UE_LOG(LogTemp, Log,
			TEXT("[AvCustomizeRandomEligibilityValidation] SelectedExactPath=%s EligibilityExactPath=%s AuditStatus=%s VisualCondition=%s Allow=true SharedGeometryGroup=%s SharedMaterialGroup=%s AuditEvidence=%s Result=PASS"),
			*SoftPath.ToString(),
			*Eligibility->AssetPath.ToString(),
			AvAuditStatusName(Eligibility->AuditStatus),
			AvVisualConditionName(Eligibility->VisualCondition),
			*Eligibility->SharedGeometryGroup,
			*Eligibility->SharedMaterialGroup,
			*Eligibility->AuditEvidence);
	}

	const TSoftObjectPtr<USkeletalMesh> FullOutfit = InAppearance.Get(EWorkerSlot::FullOutfit);
	const TSoftObjectPtr<USkeletalMesh> Upper = InAppearance.Get(EWorkerSlot::Torso);
	const TSoftObjectPtr<USkeletalMesh> Lower = InAppearance.Get(EWorkerSlot::Legs);
	if (!FullOutfit.IsNull())
	{
		const FAvRandomFamilyDefinition* OutfitFamily = AvFindRandomFamily(
			EWorkerSlot::FullOutfit,
			FullOutfit.ToSoftObjectPath());
		if (!OutfitFamily || !OutfitFamily->bIsFullOutfit ||
			!OutfitFamily->bCoversUpperBody || !OutfitFamily->bCoversLowerBody)
		{
			return Fail(TEXT("FullOutfitMissingCoverage"));
		}
		if (!OutfitFamily->bIncludesFootwear)
		{
			return Fail(TEXT("FullOutfitMissingFootwear"));
		}
		if (!Upper.IsNull() || !Lower.IsNull() ||
			!InAppearance.Get(EWorkerSlot::Feet).IsNull() ||
			!InAppearance.Get(EWorkerSlot::Vest).IsNull())
		{
			return Fail(TEXT("FullOutfitSlotConflict"));
		}
		OutValidationDetails = TEXT("Scheme=FullOutfit VisualQuality=PASS Eligibility=true Unverified=false Damaged=false Quarantined=false LowerCoverage=true Footwear=Integrated FullOutfitConflicts=0 HeadConflict=false HoodHeadgearConflicts=0 InvalidComponents=0 Preset=false");
		return true;
	}

	const FAvRandomFamilyDefinition* UpperFamily = AvFindRandomFamily(
		EWorkerSlot::Torso,
		Upper.ToSoftObjectPath());
	if (!UpperFamily || !UpperFamily->bCoversUpperBody)
	{
		return Fail(TEXT("MissingUpperCoverage"));
	}
	const FAvRandomFamilyDefinition* LowerFamily = AvFindRandomFamily(
		EWorkerSlot::Legs,
		Lower.ToSoftObjectPath());
	if (!LowerFamily || !LowerFamily->bCoversLowerBody)
	{
		return Fail(TEXT("MissingLowerCoverage"));
	}
	if (!LowerFamily->bIncludesFootwear)
	{
		return Fail(TEXT("MissingFootwear"));
	}
	if (UpperFamily->bHasIntegratedHood &&
		!InAppearance.Get(EWorkerSlot::Headgear).IsNull())
	{
		return Fail(TEXT("IntegratedHoodHeadwearConflict"));
	}
	if (UpperFamily->bHasIntegratedHood &&
		HeadphonesConflictWithRaisedHood(InAppearance.Get(EWorkerSlot::Headphones)))
	{
		return Fail(TEXT("IntegratedHoodHeadphonesConflict"));
	}
	if (HeadgearConflictsWithHeadphones(
		InAppearance.Get(EWorkerSlot::Headgear),
		InAppearance.Get(EWorkerSlot::Headphones)))
	{
		return Fail(TEXT("HeadgearHeadphonesConflict"));
	}

	OutValidationDetails = TEXT("Scheme=Separate VisualQuality=PASS Eligibility=true Unverified=false Damaged=false Quarantined=false LowerCoverage=true Footwear=Integrated FullOutfitConflicts=0 HeadConflict=false HoodHeadgearConflicts=0 InvalidComponents=0 Preset=false");
	return true;
}

FString UWorkerAppearanceComponent::DescribeRandomVisualEligibility(
	const FWorkerAppearance& InAppearance)
{
	TArray<FWorkerSlotMesh> SortedSlots = InAppearance.Slots;
	SortedSlots.Sort([](const FWorkerSlotMesh& A, const FWorkerSlotMesh& B)
	{
		return static_cast<uint8>(A.Slot) < static_cast<uint8>(B.Slot);
	});

	TArray<FString> Parts;
	for (const FWorkerSlotMesh& SlotMesh : SortedSlots)
	{
		if (SlotMesh.Slot == EWorkerSlot::Body || SlotMesh.Slot == EWorkerSlot::Head)
		{
			continue;
		}
		const FSoftObjectPath AssetPath = SlotMesh.Mesh.ToSoftObjectPath();
		const FAvRandomAssetEligibility* Eligibility =
			AvFindRandomAssetEligibility(SlotMesh.Slot, AssetPath);
		const FAvRandomFamilyDefinition* Family =
			AvFindRandomFamily(SlotMesh.Slot, AssetPath);
		const bool bSharedGeometryQuarantined = Eligibility &&
			AvGetQuarantinedSharedGeometryGroups().Contains(Eligibility->SharedGeometryGroup);
		Parts.Add(FString::Printf(
			TEXT("Slot=%d SelectedExactPath=%s SelectedPackagePath=%s EligibilityExactPath=%s Allow=%s AuditStatus=%s VisualCondition=%s Family=%s SharedGeometryGroup=%s SharedMaterialGroup=%s GroupQuarantined=%s AuditEvidence=%s LastAuditDate=%s AuditVersion=%s QuarantineReason=%s RequiredUnderlayer=%s BodyCoverage=%s"),
			static_cast<int32>(SlotMesh.Slot),
			*AssetPath.ToString(),
			*AssetPath.GetLongPackageName(),
			Eligibility ? *Eligibility->AssetPath.ToString() : TEXT("missing"),
			Eligibility && Eligibility->bAllowInRandom ? TEXT("true") : TEXT("false"),
			Eligibility ? AvAuditStatusName(Eligibility->AuditStatus) : TEXT("Unverified"),
			Eligibility ? AvVisualConditionName(Eligibility->VisualCondition) : TEXT("Unverified"),
			Family ? Family->Folder : TEXT("none"),
			Eligibility ? *Eligibility->SharedGeometryGroup : TEXT("none"),
			Eligibility ? *Eligibility->SharedMaterialGroup : TEXT("none"),
			bSharedGeometryQuarantined ? TEXT("true") : TEXT("false"),
			Eligibility && !Eligibility->AuditEvidence.IsEmpty()
				? *Eligibility->AuditEvidence : TEXT("none"),
			Eligibility ? *Eligibility->LastAuditDate : TEXT("none"),
			Eligibility ? *Eligibility->AuditVersion : TEXT("none"),
			Eligibility && !Eligibility->QuarantineReason.IsEmpty()
				? *Eligibility->QuarantineReason : TEXT("none"),
			Eligibility && !Eligibility->RequiredUnderlayer.IsNull()
				? *Eligibility->RequiredUnderlayer.ToString()
				: TEXT("none"),
			Eligibility ? *Eligibility->BodyCoverageRequirement : TEXT("Unverified")));
	}
	return Parts.IsEmpty() ? TEXT("NoRandomClothingSlots") : FString::Join(Parts, TEXT("; "));
}

TArray<FAvHeadCustomizationCatalogItem> UWorkerAppearanceComponent::GetHeadCustomizationCatalog()
{
	TArray<FAvHeadCustomizationCatalogItem> Catalog;
	for (const FAvHeadTypePresentation& Presentation : AvGetHeadTypePresentations())
	{
		FAvHeadCustomizationCatalogItem Item;
		TryGetHeadTypeMetadata(
			TSoftObjectPtr<USkeletalMesh>(Presentation.ExactHeadAssetPath),
			Item);
		Catalog.Add(MoveTemp(Item));
	}

	for (const TPair<FSoftObjectPath, FAvRandomAssetEligibility>& Pair : AvGetRandomAssetEligibility())
	{
		const FAvRandomAssetEligibility& Eligibility = Pair.Value;
		EAvHeadCustomizationSection Section;
		switch (Eligibility.Slot)
		{
		case EWorkerSlot::Headgear: Section = EAvHeadCustomizationSection::Headgear; break;
		case EWorkerSlot::Hair: Section = EAvHeadCustomizationSection::Hair; break;
		case EWorkerSlot::Beard: Section = EAvHeadCustomizationSection::Beard; break;
		default: continue;
		}

		const FString ExactPath = Eligibility.AssetPath.ToString();
		const bool bForbiddenSource =
			ExactPath.Contains(TEXT("/Female/")) ||
			ExactPath.Contains(TEXT("/Demo/")) ||
			ExactPath.Contains(TEXT("/Editor/")) ||
			ExactPath.Contains(TEXT("/Presets/"));
		const bool bGroupQuarantined =
			AvGetQuarantinedSharedGeometryGroups().Contains(Eligibility.SharedGeometryGroup);
		const bool bAllowInUI =
			Eligibility.bAllowInRandom &&
			Eligibility.AuditStatus == EAvRandomAuditStatus::VerifiedIntact &&
			Eligibility.VisualCondition == EAvRandomVisualCondition::Intact &&
			!bGroupQuarantined &&
			!bForbiddenSource &&
			AvAuditEvidenceExists(Eligibility.AuditEvidence);
		if (!bAllowInUI)
		{
			continue;
		}

		const FString* DisplayNameKey = AvFindHeadItemDisplayName(Eligibility.AssetPath);
		if (!DisplayNameKey)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvCustomizeHeadCatalog] Exact item has no Russian display-name metadata and will not be exposed in UI: %s"),
				*ExactPath);
			continue;
		}

		FAvHeadCustomizationCatalogItem Item;
		Item.UISection = Section;
		Item.Slot = Eligibility.Slot;
		Item.DisplayNameKey = *DisplayNameKey;
		Item.DisplayName = AvResolveLocalizedString(Item.DisplayNameKey);
		Item.ExactObjectPath = ExactPath;
		FString ThumbnailSection;
		FString ThumbnailPrefix;
		switch (Section)
		{
		case EAvHeadCustomizationSection::Headgear:
			ThumbnailSection = TEXT("Headgear");
			ThumbnailPrefix = TEXT("Headgear");
			break;
		case EAvHeadCustomizationSection::Hair:
			ThumbnailSection = TEXT("Hair");
			ThumbnailPrefix = TEXT("Hair");
			break;
		case EAvHeadCustomizationSection::Beard:
			ThumbnailSection = TEXT("Beard");
			ThumbnailPrefix = TEXT("Beard");
			break;
		default:
			break;
		}
		const FString AppearanceAssetName = Eligibility.AssetPath.GetAssetName();
		const FString AppearanceFolderName = FPaths::GetCleanFilename(
			FPaths::GetPath(Eligibility.AssetPath.GetLongPackageName()));
		const FString StableAssetName = FString::Printf(
			TEXT("%s_%s"), *AppearanceFolderName, *AppearanceAssetName);
		Item.StableId = StableAssetName;
		auto BuildThumbnailPath = [
			&ThumbnailSection,
			&ThumbnailPrefix,
			&StableAssetName](
				const TCHAR* HeadTypeDirectory,
				const TCHAR* ThumbnailSuffix)
		{
			const FString AssetName = FString::Printf(
				TEXT("T_UI_%s_%s_%s"),
				*ThumbnailPrefix,
				*StableAssetName,
				ThumbnailSuffix);
			return FString::Printf(
				TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/%s/%s/%s.%s"),
				*ThumbnailSection,
				HeadTypeDirectory,
				*AssetName,
				*AssetName);
		};
		Item.ThumbnailType01 = BuildThumbnailPath(TEXT("HeadType01"), TEXT("Type01"));
		Item.ThumbnailType02 = BuildThumbnailPath(TEXT("HeadType02"), TEXT("Type02"));
		Item.Family = Eligibility.SharedGeometryGroup;
		Item.AuditStatus = AvAuditStatusName(Eligibility.AuditStatus);
		Item.VisualCondition = AvVisualConditionName(Eligibility.VisualCondition);
		Item.bAllowInUI = true;
		Item.bHidesHair = Eligibility.bHidesHair;
		Item.bConflictsWithRaisedHood = Eligibility.Slot == EWorkerSlot::Headgear &&
			AvGetRaisedHoodConflictingHeadgear().Contains(Eligibility.AssetPath);
		Item.bConflictsWithIntegratedHood = Item.bConflictsWithRaisedHood;
		Item.bCompatibleWithHeadphones = Eligibility.Slot == EWorkerSlot::Headgear &&
			AvGetHeadphonesCompatibleHeadgear().Contains(Eligibility.AssetPath);
		Item.HeadphonesCompatibilityReason = Item.bCompatibleWithHeadphones
			? TEXT("ExactOrdinaryCapFamilyAllowlist")
			: TEXT("OnlyExactOrdinaryCapFamilyAllowsHeadphones");
		Item.Compatibility = {
			TEXT("MaleQuantumSkeletonCompatible"),
			TEXT("CanonicalFactoryBodyCompatible"),
			TEXT("ExactAssetVerified")
		};
		if (Eligibility.Slot == EWorkerSlot::Headgear)
		{
			Item.Compatibility.Add(Item.bConflictsWithRaisedHood
				? TEXT("ConfirmWithRaisedHood")
				: TEXT("CompatibleWithRaisedHood"));
			Item.Compatibility.Add(Eligibility.bHidesHair
				? TEXT("HidesHairWithoutClearingSlot")
				: TEXT("HairVisible"));
			Item.Compatibility.Add(Item.bCompatibleWithHeadphones
				? TEXT("CompatibleWithHeadphones")
				: TEXT("ConfirmAndClearHeadphones"));
		}
		Item.TooltipReason = Eligibility.QuarantineReason;
		Item.AuditEvidence = Eligibility.AuditEvidence;
		Catalog.Add(MoveTemp(Item));
	}

	Catalog.Sort([](const FAvHeadCustomizationCatalogItem& A, const FAvHeadCustomizationCatalogItem& B)
	{
		if (A.UISection != B.UISection)
		{
			return static_cast<uint8>(A.UISection) < static_cast<uint8>(B.UISection);
		}
		return A.ExactObjectPath < B.ExactObjectPath;
	});
	return Catalog;
}

TArray<FAvHandsAccessoriesCatalogItem> UWorkerAppearanceComponent::GetHandsAccessoriesCatalog()
{
	TArray<FAvHandsAccessoriesCatalogItem> Catalog;
	auto Add = [&Catalog](EAvHandsAccessoriesSection Section, EWorkerSlot Slot,
		const TCHAR* MeshPath, const TCHAR* DisplayNameKey, const TCHAR* CurrentSelectionDisplayNameKey,
		const TCHAR* StableName, bool bNone, bool bCompatibleWithWatch = true)
	{
		FAvHandsAccessoriesCatalogItem Item;
		Item.Section = Section;
		Item.WorkerSlot = Slot;
		Item.StableId = StableName;
		Item.ExactMeshPath = bNone ? TEXT("") : MeshPath;
		Item.DisplayNameKey = DisplayNameKey;
		Item.CurrentSelectionDisplayNameKey = CurrentSelectionDisplayNameKey;
		Item.DisplayName = AvResolveLocalizedString(Item.DisplayNameKey);
		Item.CurrentSelectionDisplayName = AvResolveLocalizedString(
			Item.CurrentSelectionDisplayNameKey);
		Item.bIsNone = bNone;
		Item.bEnabled = true;
		Item.bCompatibleWithWatch = bCompatibleWithWatch;
		Item.BodyCoverageMask =
			!bNone && Section == EAvHandsAccessoriesSection::Gloves
				? AvBodyZone(EWorkerBodyZone::Hands)
				: 0;
		const FString Root = TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/HandsAccessories");
		const FString SectionName = Section == EAvHandsAccessoriesSection::Gloves ? TEXT("Gloves") : TEXT("Watches");
		const FString Name01 = FString::Printf(TEXT("T_UI_HA_%s_Type01"), StableName);
		const FString Name02 = FString::Printf(TEXT("T_UI_HA_%s_Type02"), StableName);
		const FString Dir01 = Root / SectionName / TEXT("HeadType01");
		const FString Dir02 = Root / SectionName / TEXT("HeadType02");
		Item.ThumbnailType01 = FString::Printf(TEXT("%s/%s.%s"), *Dir01, *Name01, *Name01);
		Item.ThumbnailType02 = FString::Printf(TEXT("%s/%s.%s"), *Dir02, *Name02, *Name02);
		Catalog.Add(MoveTemp(Item));
	};
	Add(EAvHandsAccessoriesSection::Gloves, EWorkerSlot::Gloves, TEXT(""), TEXT("Customization.Common.None"), TEXT("Customization.Common.None"), TEXT("Gloves_None"), true);
	Add(EAvHandsAccessoriesSection::Gloves, EWorkerSlot::Gloves,
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Gray.SKM_Arms_Gloves_Print_Gray"),
		TEXT("Customization.Item.Gloves.Gloves_Print_Gray"), TEXT("Customization.Item.Gloves.Gloves_Print_Gray"), TEXT("Gloves_Print_Gray"), false);
	Add(EAvHandsAccessoriesSection::Gloves, EWorkerSlot::Gloves,
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Green.SKM_Arms_Gloves_Print_Green"),
		TEXT("Customization.Item.Gloves.Gloves_Print_Green"), TEXT("Customization.Item.Gloves.Gloves_Print_Green"), TEXT("Gloves_Print_Green"), false);
	Add(EAvHandsAccessoriesSection::Gloves, EWorkerSlot::Gloves,
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Orange.SKM_Arms_Gloves_Print_Orange"),
		TEXT("Customization.Item.Gloves.Gloves_Print_Orange"), TEXT("Customization.Item.Gloves.Gloves_Print_Orange"), TEXT("Gloves_Print_Orange"), false);
	Add(EAvHandsAccessoriesSection::Gloves, EWorkerSlot::Gloves,
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Yellow.SKM_Arms_Gloves_Print_Yellow"),
		TEXT("Customization.Item.Gloves.Gloves_Print_Yellow"), TEXT("Customization.Item.Gloves.Gloves_Print_Yellow"), TEXT("Gloves_Print_Yellow"), false);
	Add(EAvHandsAccessoriesSection::Watches, EWorkerSlot::Watch, TEXT(""), TEXT("Customization.Common.None"), TEXT("Customization.Common.None"), TEXT("Watches_None"), true);
	Add(EAvHandsAccessoriesSection::Watches, EWorkerSlot::Watch,
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_L_Bege.SKM_Watches_Military_L_Bege"),
		TEXT("Customization.Item.Watch.Watch_L_Bege"), TEXT("Customization.Item.Watch.Watch_L_Bege.CurrentSelection"), TEXT("Watch_L_Bege"), false);
	Add(EAvHandsAccessoriesSection::Watches, EWorkerSlot::Watch,
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_L_Black.SKM_Watches_Military_L_Black"),
		TEXT("Customization.Item.Watch.Watch_L_Black"), TEXT("Customization.Item.Watch.Watch_L_Black.CurrentSelection"), TEXT("Watch_L_Black"), false);
	Add(EAvHandsAccessoriesSection::Watches, EWorkerSlot::Watch,
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_R_Bege.SKM_Watches_Military_R_Bege"),
		TEXT("Customization.Item.Watch.Watch_R_Bege"), TEXT("Customization.Item.Watch.Watch_R_Bege.CurrentSelection"), TEXT("Watch_R_Bege"), false);
	Add(EAvHandsAccessoriesSection::Watches, EWorkerSlot::Watch,
		TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_R_Black.SKM_Watches_Military_R_Black"),
		TEXT("Customization.Item.Watch.Watch_R_Black"), TEXT("Customization.Item.Watch.Watch_R_Black.CurrentSelection"), TEXT("Watch_R_Black"), false);
	return Catalog;
}

TArray<FAvTorsoCatalogItem> UWorkerAppearanceComponent::GetTorsoCatalog()
{
	TArray<FAvTorsoCatalogItem> Catalog;
	const FString MeshRoot = TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules");
	const FString MaterialRoot = TEXT("/Game/Modular_Workers/Materials/Clothes_Modules");
	const FString ThumbnailRoot = TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/UpperEquipment/Torso");
	auto Add = [&Catalog, &ThumbnailRoot](const FString& MeshPath, const FString& MaterialPath,
		const FString& StableName, const FString& Family,
		const FString& Color, int32 SortOrder, bool bNone = false, int32 MaterialSlotCount = 1,
		bool bHasRaisedHood = false)
	{
		FAvTorsoCatalogItem Item;
		Item.StableId = StableName;
		Item.ExactMeshPath = bNone ? FString() : MeshPath;
		if (!bNone && !MaterialPath.IsEmpty())
		{
			for (int32 SlotIndex = 0; SlotIndex < MaterialSlotCount; ++SlotIndex)
			{
				Item.ExactMaterialOverrides.Add(MaterialPath);
			}
		}
		Item.DisplayNameKey = bNone
			? TEXT("Customization.Common.None")
			: FString::Printf(TEXT("Customization.Item.Torso.%s"), *StableName);
		Item.CurrentSelectionNameKey = Item.DisplayNameKey;
		Item.DisplayName = AvResolveLocalizedString(Item.DisplayNameKey);
		Item.CurrentSelectionName = Item.DisplayName;
		Item.Family = Family;
		Item.Color = Color;
		Item.SortOrder = SortOrder;
		Item.bIsNone = bNone;
		Item.bEnabled = true;
		Item.bHasRaisedHood = bHasRaisedHood;
		if (!bNone)
		{
			const bool bLongSleeve =
				Family == TEXT("WorkJacket") ||
				Family == TEXT("ProtectiveJacket") ||
				Family == TEXT("HoodJacket") ||
				Family == TEXT("M65");
			Item.BodyCoverageMask = bLongSleeve
				? AvBodyZones(EWorkerBodyZone::Torso, EWorkerBodyZone::UpperArms, EWorkerBodyZone::Forearms)
				: AvBodyZones(EWorkerBodyZone::Torso, EWorkerBodyZone::UpperArms);
		}
		if (bHasRaisedHood)
		{
			Item.CompatibilityTags.Add(TEXT("Torso.Hood.Raised"));
			Item.CompatibilityTags.Add(TEXT("ConflictsWithAuditedHeadgear"));
		}
		const FString Name01 = FString::Printf(TEXT("T_UI_Torso_%s_Type01"), *StableName);
		const FString Name02 = FString::Printf(TEXT("T_UI_Torso_%s_Type02"), *StableName);
		Item.ThumbnailType01 = FString::Printf(TEXT("%s/HeadType01/%s.%s"), *ThumbnailRoot, *Name01, *Name01);
		Item.ThumbnailType02 = FString::Printf(TEXT("%s/HeadType02/%s.%s"), *ThumbnailRoot, *Name02, *Name02);
		Catalog.Add(MoveTemp(Item));
	};
	Add({}, {}, TEXT("None"), TEXT("None"), {}, 0, true, 0);
	struct FVariant { const TCHAR* Suffix; };
	const FVariant ShirtColors[] = {
		{TEXT("Bege")}, {TEXT("Black")}, {TEXT("Blue")}, {TEXT("Gray")},
		{TEXT("Green")}, {TEXT("Red")}
	};
	for (int32 ColorIndex = 0; ColorIndex < UE_ARRAY_COUNT(ShirtColors); ++ColorIndex)
	{
		const FVariant& V = ShirtColors[ColorIndex];
		const FString MeshName = FString::Printf(TEXT("SKM_TShirt_%s"), V.Suffix);
		const FString MatName = FString::Printf(TEXT("MI_TShirt_%s"), V.Suffix);
		Add(FString::Printf(TEXT("%s/Tshirt/%s.%s"),*MeshRoot,*MeshName,*MeshName),
			FString::Printf(TEXT("%s/Tshirt/%s.%s"),*MaterialRoot,*MatName,*MatName),
			MeshName, TEXT("TShirt"), V.Suffix, 100 + ColorIndex);
	}
	// Tshirt_Tucked is deliberately absent from the user-facing Torso catalog. All seven
	// exact color meshes share the authored ripped geometry seen in their technical audit;
	// the source pack assets stay intact and the Random eligibility audit keeps quarantining
	// the same exact paths as OpacityMaskDamage.
	const FVariant RolledColors[] = {
		{TEXT("Bege")}, {TEXT("Black")}, {TEXT("Blue")}, {TEXT("Green")},
		{TEXT("Red")}, {TEXT("RedCage")}, {TEXT("White")}, {TEXT("White_Cage")}
	};
	for (int32 ColorIndex = 0; ColorIndex < UE_ARRAY_COUNT(RolledColors); ++ColorIndex)
	{
		const FVariant& V = RolledColors[ColorIndex];
		const FString MeshName=FString::Printf(TEXT("SKM_Shirt_RolledUp_%s"),V.Suffix);
		const FString MatName=FString::Printf(TEXT("MI_Shirt_RolledUp_%s"),V.Suffix);
		Add(FString::Printf(TEXT("%s/Shirt_RolledUp/%s.%s"),*MeshRoot,*MeshName,*MeshName),
			FString::Printf(TEXT("%s/Shirt_RolledUp/%s.%s"),*MaterialRoot,*MatName,*MatName),
			MeshName, TEXT("WorkShirt"), V.Suffix, 300 + ColorIndex);
	}
	const FVariant JacketColors[]={{TEXT("Bege")},{TEXT("Blue")},{TEXT("Gray")},{TEXT("Green")},{TEXT("LightGreen")},{TEXT("Orange")}};
	for (int32 ColorIndex = 0; ColorIndex < UE_ARRAY_COUNT(JacketColors); ++ColorIndex)
	{
		const FVariant& V = JacketColors[ColorIndex];
		for(int32 Style=0;Style<3;++Style)
		{
			const FString Folder=Style==0?TEXT("Jacket_Worker"):(Style==1?TEXT("Jacket_Worker_Hood_1"):TEXT("Jacket_Worker_Hood_2"));
			const FString MeshSuffix=FCString::Strcmp(V.Suffix,TEXT("LightGreen"))==0?(Style==0?TEXT("LightGreen"):TEXT("LightGr")):V.Suffix;
			const FString MeshName=Style==0?FString::Printf(TEXT("SKM_Jacket_Worker_%s"),*MeshSuffix):FString::Printf(TEXT("SKM_Jacket_Worker_Hood_%d_%s"),Style,*MeshSuffix);
			const FString MatName=FString::Printf(TEXT("MI_Jacket_Worker_%s"),V.Suffix);
			Add(FString::Printf(TEXT("%s/%s/%s.%s"),*MeshRoot,*Folder,*MeshName,*MeshName),
				FString::Printf(TEXT("%s/Jacket_Worker/%s.%s"),*MaterialRoot,*MatName,*MatName),
				MeshName,
				Style == 0 ? TEXT("WorkJacket") : (Style == 1 ? TEXT("ProtectiveJacket") : TEXT("HoodJacket")),
				V.Suffix, 400 + Style * 100 + ColorIndex, false, Style == 0 ? 1 : 2,
				Style == 2);
		}
	}
	const FVariant M65Colors[]={{TEXT("Bege")},{TEXT("Black")},{TEXT("Green")}};
	for (int32 ColorIndex = 0; ColorIndex < UE_ARRAY_COUNT(M65Colors); ++ColorIndex)
	{
		const FVariant& V = M65Colors[ColorIndex];
		const FString MeshName=FString::Printf(TEXT("SKM_Jacket_M65_%s"),V.Suffix),MatName=FString::Printf(TEXT("MI_Jacket_M65_%s"),V.Suffix);
		Add(FString::Printf(TEXT("%s/Jacket_M65/%s.%s"),*MeshRoot,*MeshName,*MeshName),
			FString::Printf(TEXT("%s/Jacket_M65/%s.%s"),*MaterialRoot,*MatName,*MatName),
			MeshName, TEXT("M65"), V.Suffix, 450 + ColorIndex);
	}
	Catalog.Sort([](const FAvTorsoCatalogItem& Left, const FAvTorsoCatalogItem& Right)
	{
		return Left.SortOrder < Right.SortOrder;
	});
	return Catalog;
}

TArray<FAvLowerHipCatalogItem> UWorkerAppearanceComponent::GetLowerHipCatalog()
{
	TArray<FAvLowerHipCatalogItem> Catalog;
	const FString ThumbnailRoot = TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/LowerHip");
	auto BuildThumbnailPair = [&ThumbnailRoot](
		EAvLowerHipSection Section, const FString& StableId, bool bIsNone,
		FString& OutType01, FString& OutType02)
	{
		const FString SectionName = Section == EAvLowerHipSection::Legs ? TEXT("Legs") : TEXT("Hip");
		const FString Leaf = bIsNone ? FString::Printf(TEXT("None_%s"), *SectionName) : StableId;
		const FString Directory = bIsNone
			? ThumbnailRoot / TEXT("None") / SectionName
			: ThumbnailRoot / SectionName;
		const FString Name01 = FString::Printf(TEXT("T_UI_%s_Type01"), *Leaf);
		const FString Name02 = FString::Printf(TEXT("T_UI_%s_Type02"), *Leaf);
		OutType01 = FString::Printf(TEXT("%s/HeadType01/%s.%s"), *Directory, *Name01, *Name01);
		OutType02 = FString::Printf(TEXT("%s/HeadType02/%s.%s"), *Directory, *Name02, *Name02);
	};
	auto AddNone = [&Catalog, &BuildThumbnailPair](EAvLowerHipSection Section, EWorkerSlot Slot, int32 SortOrder)
	{
		FAvLowerHipCatalogItem Item;
		Item.Section = Section;
		Item.WorkerSlot = Slot;
		Item.StableId = Section == EAvLowerHipSection::Legs ? TEXT("Legs_None") : TEXT("Hip_None");
		Item.DisplayNameKey = TEXT("Customization.Common.None");
		Item.CurrentSelectionNameKey = Item.DisplayNameKey;
		Item.DisplayName = AvResolveLocalizedString(Item.DisplayNameKey);
		Item.CurrentSelectionName = Item.DisplayName;
		Item.SortOrder = SortOrder;
		Item.bIsNone = true;
		Item.bEnabled = true;
		BuildThumbnailPair(Section, Item.StableId, true, Item.ThumbnailType01, Item.ThumbnailType02);
		Catalog.Add(MoveTemp(Item));
	};
	auto Add = [&Catalog, &BuildThumbnailPair](
		EAvLowerHipSection Section, EWorkerSlot Slot, const TCHAR* StableId,
		const TCHAR* ExactMeshPath, std::initializer_list<const TCHAR*> Materials,
		const TCHAR* DisplayNameKey, const TCHAR* Family, const TCHAR* Color, int32 SortOrder)
	{
		FAvLowerHipCatalogItem Item;
		Item.Section = Section;
		Item.WorkerSlot = Slot;
		Item.StableId = StableId;
		Item.ExactMeshPath = ExactMeshPath;
		for (const TCHAR* Material : Materials) Item.ExactMaterialOverrides.Add(Material);
		Item.DisplayNameKey = DisplayNameKey;
		Item.CurrentSelectionNameKey = DisplayNameKey;
		Item.DisplayName = AvResolveLocalizedString(Item.DisplayNameKey);
		Item.CurrentSelectionName = Item.DisplayName;
		Item.Family = Family;
		Item.Color = Color;
		Item.SortOrder = SortOrder;
		Item.bEnabled = true;
		Item.BodyCoverageMask = Section == EAvLowerHipSection::Legs
			? AvBodyZones(
				EWorkerBodyZone::Pelvis,
				EWorkerBodyZone::UpperLegs,
				EWorkerBodyZone::LowerLegs,
				EWorkerBodyZone::Feet)
			: 0;
		Item.CompatibilityTags.Add(Section == EAvLowerHipSection::Legs
			? TEXT("LowerBody.Legs") : TEXT("LowerBody.HipAccessory"));
		BuildThumbnailPair(Section, Item.StableId, false, Item.ThumbnailType01, Item.ThumbnailType02);
		Catalog.Add(MoveTemp(Item));
	};

	AddNone(EAvLowerHipSection::Legs, EWorkerSlot::Legs, 0);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_Jeans_Classic"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans.SKM_Jeans"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jeans/MI_Jeans.MI_Jeans") },
		TEXT("Customization.Item.Legs.Legs_Jeans_Classic"), TEXT("Jeans"), TEXT("Classic"), 100);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_Jeans_Bege"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Bege.SKM_Jeans_Bege"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jeans/MI_Jeans_Bege.MI_Jeans_Bege") },
		TEXT("Customization.Item.Legs.Legs_Jeans_Bege"), TEXT("Jeans"), TEXT("Beige"), 110);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_Jeans_Black"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Black.SKM_Jeans_Black"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jeans/MI_Jeans_Black.MI_Jeans_Black") },
		TEXT("Customization.Item.Legs.Legs_Jeans_Black"), TEXT("Jeans"), TEXT("Black"), 120);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_Jeans_Blue"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Blue.SKM_Jeans_Blue"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jeans/MI_Jeans_Blue.MI_Jeans_Blue") },
		TEXT("Customization.Item.Legs.Legs_Jeans_Blue"), TEXT("Jeans"), TEXT("Blue"), 130);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_Jeans_Turn"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Turn.SKM_Jeans_Turn"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jeans/MI_Jeans_Turn.MI_Jeans_Turn") },
		TEXT("Customization.Item.Legs.Legs_Jeans_Turn"), TEXT("Jeans"), TEXT("Blue"), 140);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_PantsWorker_Bege"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Bege.SKM_Pants_Worker_Bege"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Bege.MI_Pants_Worker_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.Legs.Legs_PantsWorker_Bege"), TEXT("Pants_Worker"), TEXT("Beige"), 200);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_PantsWorker_Blue"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Blue.SKM_Pants_Worker_Blue"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Blue.MI_Pants_Worker_Blue"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.Legs.Legs_PantsWorker_Blue"), TEXT("Pants_Worker"), TEXT("Blue"), 210);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_PantsWorker_Gray"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Gray.SKM_Pants_Worker_Gray"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Gray.MI_Pants_Worker_Gray"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.Legs.Legs_PantsWorker_Gray"), TEXT("Pants_Worker"), TEXT("Gray"), 220);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_PantsWorker_Green"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Green.SKM_Pants_Worker_Green"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Green.MI_Pants_Worker_Green"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.Legs.Legs_PantsWorker_Green"), TEXT("Pants_Worker"), TEXT("Green"), 230);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_PantsWorker_LightGreen"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_LightGreen.SKM_Pants_Worker_LightGreen"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_LightGreen.MI_Pants_Worker_LightGreen"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.Legs.Legs_PantsWorker_LightGreen"), TEXT("Pants_Worker"), TEXT("LightGreen"), 240);
	Add(EAvLowerHipSection::Legs, EWorkerSlot::Legs, TEXT("Legs_PantsWorker_Orange"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Orange.SKM_Pants_Worker_Orange"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Orange.MI_Pants_Worker_Orange"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege") },
		TEXT("Customization.Item.Legs.Legs_PantsWorker_Orange"), TEXT("Pants_Worker"), TEXT("Orange"), 250);

	AddNone(EAvLowerHipSection::Hip, EWorkerSlot::HipAccessory, 1000);
	Add(EAvLowerHipSection::Hip, EWorkerSlot::HipAccessory, TEXT("Hip_Bag_Bege"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Bege.SKM_Bag_Hip_Bege"),
		{ TEXT("/Game/Modular_Workers/Materials/Hips_Modules/Bag_Hip/MI_Bag_Hip_Bege.MI_Bag_Hip_Bege") },
		TEXT("Customization.Item.HipAccessory.Hip_Bag_Bege"), TEXT("Bag_Hip"), TEXT("Beige"), 1100);
	Add(EAvLowerHipSection::Hip, EWorkerSlot::HipAccessory, TEXT("Hip_Bag_Black"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Black.SKM_Bag_Hip_Black"),
		{ TEXT("/Game/Modular_Workers/Materials/Hips_Modules/Bag_Hip/MI_Bag_Hip_Black.MI_Bag_Hip_Black") },
		TEXT("Customization.Item.HipAccessory.Hip_Bag_Black"), TEXT("Bag_Hip"), TEXT("Black"), 1110);
	Add(EAvLowerHipSection::Hip, EWorkerSlot::HipAccessory, TEXT("Hip_Bag_Green"),
		TEXT("/Game/Modular_Workers/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Green.SKM_Bag_Hip_Green"),
		{ TEXT("/Game/Modular_Workers/Materials/Hips_Modules/Bag_Hip/MI_Bag_Hip_Green.MI_Bag_Hip_Green") },
		TEXT("Customization.Item.HipAccessory.Hip_Bag_Green"), TEXT("Bag_Hip"), TEXT("Green"), 1120);

	Catalog.Sort([](const FAvLowerHipCatalogItem& Left, const FAvLowerHipCatalogItem& Right)
	{
		return Left.SortOrder < Right.SortOrder;
	});
	return Catalog;
}

namespace
{
	/**
	 * These source-pack meshes live under Pants_Modules, but their authored geometry extends
	 * through the chest bib/straps. Product classification is therefore future FullOutfit /
	 * Overalls, never the user-facing Legs or Hip catalogs.
	 */
	const TSet<FSoftObjectPath>& AvGetLowerHipFutureFullOutfitAssets()
	{
		static const TSet<FSoftObjectPath> Paths =
		{
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Bege.SKM_Pants_Worker_Full_Bege")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Blue.SKM_Pants_Worker_Full_Blue")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Gray.SKM_Pants_Worker_Full_Gray")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Green.SKM_Pants_Worker_Full_Green")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_LightGrn.SKM_Pants_Worker_Full_LightGrn")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Orange.SKM_Pants_Worker_Full_Orange"))
		};
		return Paths;
	}
}

TArray<FAvFullOutfitCatalogItem> UWorkerAppearanceComponent::GetFullOutfitCatalog()
{
	TArray<FAvFullOutfitCatalogItem> Catalog;
	const FString ThumbnailRoot =
		TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/FullOutfits");
	auto Add = [&Catalog, &ThumbnailRoot](
		const TCHAR* StableId,
		int32 DemoIndex,
		EAvEnsembleTechnicalKind TechnicalKind,
		EWorkerSlot TargetSlot,
		const TCHAR* ExactMeshPath,
		std::initializer_list<const TCHAR*> Materials,
		const TCHAR* DisplayNameKey,
		const TCHAR* Family,
		const TCHAR* Color,
		int32 SortOrder,
		int32 CoverageMask,
		std::initializer_list<EWorkerSlot> SuppressedSlots,
		bool bIsNone = false)
	{
		FAvFullOutfitCatalogItem Item;
		Item.StableId = StableId;
		Item.DemoIndex = DemoIndex;
		Item.TechnicalKind = TechnicalKind;
		Item.TargetSlot = TargetSlot;
		Item.PrimaryMeshPath = bIsNone ? FString() : ExactMeshPath;
		Item.ExactMeshPath = bIsNone ? FString() : ExactMeshPath;
		for (const TCHAR* Material : Materials)
		{
			Item.PrimaryMaterialOverrides.Add(Material);
			Item.ExactMaterialOverrides.Add(Material);
		}
		Item.DisplayNameKey = DisplayNameKey;
		Item.CurrentSelectionNameKey = DisplayNameKey;
		Item.DisplayName = AvResolveLocalizedString(Item.DisplayNameKey);
		Item.CurrentSelectionName = Item.DisplayName;
		Item.Family = Family;
		Item.Color = Color;
		Item.SortOrder = SortOrder;
		Item.BodyCoverageMask = CoverageMask;
		for (EWorkerSlot Slot : SuppressedSlots)
		{
			Item.SuppressedRenderSlots.Add(Slot);
		}
		Item.bIsNone = bIsNone;
		Item.bEnabled = true;
		const FString Name01 = FString::Printf(TEXT("T_UI_FullOutfit_%s_Type01"), StableId);
		const FString Name02 = FString::Printf(TEXT("T_UI_FullOutfit_%s_Type02"), StableId);
		Item.ThumbnailType01 = FString::Printf(
			TEXT("%s/HeadType01/%s.%s"), *ThumbnailRoot, *Name01, *Name01);
		Item.ThumbnailType02 = FString::Printf(
			TEXT("%s/HeadType02/%s.%s"), *ThumbnailRoot, *Name02, *Name02);
		Catalog.Add(MoveTemp(Item));
	};

	Add(TEXT("None"), -1, EAvEnsembleTechnicalKind::None, EWorkerSlot::FullOutfit,
		TEXT(""), {}, TEXT("Customization.Common.None"), TEXT("None"), TEXT("None"), 0, 0, {}, true);

	// WB_Male_Modular_Character_Builder exposes this package-native array through its
	// historically named Scarf/Scarf NR variables and Scarf component. Its click handler
	// resets Clothes NR and Pants NR because every entry already contains its own top,
	// lower garment and footwear. Keep the CDO order exactly: DemoIndex 0..29.
	const int32 FullOutfitCoverage = AvBodyZones(
		EWorkerBodyZone::Torso,
		EWorkerBodyZone::Pelvis,
		EWorkerBodyZone::UpperArms,
		EWorkerBodyZone::UpperLegs,
		EWorkerBodyZone::LowerLegs) | AvBodyZone(EWorkerBodyZone::Feet);
	const std::initializer_list<EWorkerSlot> FullOutfitSuppression =
		{ EWorkerSlot::Torso, EWorkerSlot::Legs, EWorkerSlot::Feet };
	auto AddDemo = [&Add, FullOutfitCoverage, FullOutfitSuppression](
		const TCHAR* StableId, int32 DemoIndex, const TCHAR* MeshPath,
		std::initializer_list<const TCHAR*> Materials, const TCHAR* DisplayName,
		const TCHAR* Family, const TCHAR* Color)
	{
		Add(StableId, DemoIndex, EAvEnsembleTechnicalKind::FullOutfit,
			EWorkerSlot::FullOutfit, MeshPath, Materials, DisplayName, Family, Color,
			DemoIndex + 1, FullOutfitCoverage, FullOutfitSuppression);
	};

	AddDemo(TEXT("FullOutfit_ShirtJumpsuitBoots_1"), 0,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_1.SKM_Shirt_Jumpsuit_Boots_1"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Brown.MI_Protective_Boots_Brown"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Blue.MI_Jumpsuit_Blue"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_White_Cage.MI_Shirt_RolledUp_White_Cage") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_ShirtJumpsuitBoots_1"), TEXT("ShirtProtectiveBoots"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_ShirtJumpsuitBoots_2"), 1,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_2.SKM_Shirt_Jumpsuit_Boots_2"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Bege.MI_Protective_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Blue.MI_Jumpsuit_Blue"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_White_Cage.MI_Shirt_RolledUp_White_Cage") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_ShirtJumpsuitBoots_2"), TEXT("ShirtProtectiveBoots"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_ShirtJumpsuitBoots_3"), 2,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_3.SKM_Shirt_Jumpsuit_Boots_3"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Yellow.MI_Protective_Boots_Yellow"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Bege.MI_Jumpsuit_Bege"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_RedCage.MI_Shirt_RolledUp_RedCage") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_ShirtJumpsuitBoots_3"), TEXT("ShirtProtectiveBoots"), TEXT("Beige"));
	AddDemo(TEXT("FullOutfit_ShirtJumpsuitBoots_4"), 3,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_4.SKM_Shirt_Jumpsuit_Boots_4"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Black.MI_Protective_Boots_Black"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Black.MI_Jumpsuit_Black"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_White_Cage.MI_Shirt_RolledUp_White_Cage") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_ShirtJumpsuitBoots_4"), TEXT("ShirtProtectiveBoots"), TEXT("Black"));
	AddDemo(TEXT("FullOutfit_ShirtJumpsuitBoots_5"), 4,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_5.SKM_Shirt_Jumpsuit_Boots_5"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Bege.MI_Protective_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_DarkBlue.MI_Jumpsuit_DarkBlue"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_RedCage.MI_Shirt_RolledUp_RedCage") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_ShirtJumpsuitBoots_5"), TEXT("ShirtProtectiveBoots"), TEXT("Blue"));

	AddDemo(TEXT("FullOutfit_TshirtJumpsuitBoots_1"), 5,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_1.SKM_Overalls_Tshirt_Boots_1"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Bege.MI_Protective_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Blue.MI_Jumpsuit_Blue"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_White.MI_Tshirt_Tucked_White") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_TshirtJumpsuitBoots_1"), TEXT("TshirtProtectiveBoots"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_TshirtJumpsuitBoots_2"), 6,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_2.SKM_Overalls_Tshirt_Boots_2"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Bege.MI_Protective_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Blue.MI_Jumpsuit_Blue"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Bege.MI_Tshirt_Tucked_Bege") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_TshirtJumpsuitBoots_2"), TEXT("TshirtProtectiveBoots"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_TshirtJumpsuitBoots_3"), 7,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_3.SKM_Overalls_Tshirt_Boots_3"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Yellow.MI_Protective_Boots_Yellow"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Bege.MI_Jumpsuit_Bege"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Blue.MI_Tshirt_Tucked_Blue") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_TshirtJumpsuitBoots_3"), TEXT("TshirtProtectiveBoots"), TEXT("Beige"));
	AddDemo(TEXT("FullOutfit_TshirtJumpsuitBoots_4"), 8,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_4.SKM_Overalls_Tshirt_Boots_4"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Black.MI_Protective_Boots_Black"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Black.MI_Jumpsuit_Black"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_White.MI_Tshirt_Tucked_White") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_TshirtJumpsuitBoots_4"), TEXT("TshirtProtectiveBoots"), TEXT("Black"));
	AddDemo(TEXT("FullOutfit_TshirtJumpsuitBoots_5"), 9,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_5.SKM_Overalls_Tshirt_Boots_5"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Protective_Suit_Boots/MI_Protective_Boots_Yellow.MI_Protective_Boots_Yellow"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_DarkBlue.MI_Jumpsuit_DarkBlue"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Red.MI_Tshirt_Tucked_Red") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_TshirtJumpsuitBoots_5"), TEXT("TshirtProtectiveBoots"), TEXT("Blue"));

	AddDemo(TEXT("FullOutfit_WorkerShirtJumpsuit_1"), 10,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_1.SKM_Worker_Shirt_Jumpsuit_1"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_White_Cage.MI_Shirt_RolledUp_White_Cage"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Blue.MI_Jumpsuit_Blue") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerShirtJumpsuit_1"), TEXT("WorkerShirtJumpsuit"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_WorkerShirtJumpsuit_2"), 11,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_2.SKM_Worker_Shirt_Jumpsuit_2"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_Bege.MI_Shirt_RolledUp_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Blue.MI_Jumpsuit_Blue") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerShirtJumpsuit_2"), TEXT("WorkerShirtJumpsuit"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_WorkerShirtJumpsuit_3"), 12,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_3.SKM_Worker_Shirt_Jumpsuit_3"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_White_Cage.MI_Shirt_RolledUp_White_Cage"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Black.MI_Jumpsuit_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerShirtJumpsuit_3"), TEXT("WorkerShirtJumpsuit"), TEXT("Black"));
	AddDemo(TEXT("FullOutfit_WorkerShirtJumpsuit_4"), 13,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_4.SKM_Worker_Shirt_Jumpsuit_4"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_White.MI_Shirt_RolledUp_White"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Bege.MI_Jumpsuit_Bege") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerShirtJumpsuit_4"), TEXT("WorkerShirtJumpsuit"), TEXT("Beige"));
	AddDemo(TEXT("FullOutfit_WorkerShirtJumpsuit_5"), 14,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_5.SKM_Worker_Shirt_Jumpsuit_5"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_RedCage.MI_Shirt_RolledUp_RedCage"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_DarkBlue.MI_Jumpsuit_DarkBlue") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerShirtJumpsuit_5"), TEXT("WorkerShirtJumpsuit"), TEXT("Blue"));

	AddDemo(TEXT("FullOutfit_WorkerTshirtJumpsuit_1"), 15,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_1.SKM_Worker_Tshirt_Jumpsuit_1"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_White.MI_Tshirt_Tucked_White"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_DarkBlue.MI_Jumpsuit_DarkBlue") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtJumpsuit_1"), TEXT("WorkerTshirtJumpsuit"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtJumpsuit_2"), 16,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_2.SKM_Worker_Tshirt_Jumpsuit_2"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Bege.MI_Tshirt_Tucked_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Blue.MI_Jumpsuit_Blue") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtJumpsuit_2"), TEXT("WorkerTshirtJumpsuit"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtJumpsuit_3"), 17,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_3.SKM_Worker_Tshirt_Jumpsuit_3"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_White.MI_Tshirt_Tucked_White"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Black.MI_Jumpsuit_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtJumpsuit_3"), TEXT("WorkerTshirtJumpsuit"), TEXT("Black"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtJumpsuit_4"), 18,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_4.SKM_Worker_Tshirt_Jumpsuit_4"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Blue.MI_Tshirt_Tucked_Blue"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Bege.MI_Jumpsuit_Bege") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtJumpsuit_4"), TEXT("WorkerTshirtJumpsuit"), TEXT("Beige"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtJumpsuit_5"), 19,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_5.SKM_Worker_Tshirt_Jumpsuit_5"),
		{ TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black"), TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Red.MI_Tshirt_Tucked_Red"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_Blue.MI_Jumpsuit_Blue") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtJumpsuit_5"), TEXT("WorkerTshirtJumpsuit"), TEXT("Blue"));

	AddDemo(TEXT("FullOutfit_WorkerTshirtPants_1"), 20,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_1.SKM_Worker_Tshirt_Pants_1"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Bege.MI_Tshirt_Tucked_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Green.MI_Pants_Worker_Green"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtPants_1"), TEXT("WorkerTshirtPants"), TEXT("Green"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtPants_2"), 21,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_2.SKM_Worker_Tshirt_Pants_2"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_White.MI_Tshirt_Tucked_White"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Gray.MI_Pants_Worker_Gray"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtPants_2"), TEXT("WorkerTshirtPants"), TEXT("Gray"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtPants_3"), 22,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_3.SKM_Worker_Tshirt_Pants_3"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Blue.MI_Tshirt_Tucked_Blue"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Bege.MI_Pants_Worker_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtPants_3"), TEXT("WorkerTshirtPants"), TEXT("Beige"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtPants_4"), 23,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_4.SKM_Worker_Tshirt_Pants_4"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Red.MI_Tshirt_Tucked_Red"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Blue.MI_Pants_Worker_Blue"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtPants_4"), TEXT("WorkerTshirtPants"), TEXT("Blue"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtPants_5"), 24,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_5.SKM_Worker_Tshirt_Pants_5"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Bege.MI_Tshirt_Tucked_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Green.MI_Pants_Worker_Green"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtPants_5"), TEXT("WorkerTshirtPants"), TEXT("Green"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtPants_6"), 25,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_6.SKM_Worker_Tshirt_Pants_6"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Bege.MI_Tshirt_Tucked_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_LightGreen.MI_Pants_Worker_LightGreen"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtPants_6"), TEXT("WorkerTshirtPants"), TEXT("LightGreen"));
	AddDemo(TEXT("FullOutfit_WorkerTshirtPants_7"), 26,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_7.SKM_Worker_Tshirt_Pants_7"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/MI_Tshirt_Tucked_Black.MI_Tshirt_Tucked_Black"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Orange.MI_Pants_Worker_Orange"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerTshirtPants_7"), TEXT("WorkerTshirtPants"), TEXT("Orange"));

	AddDemo(TEXT("FullOutfit_WorkerShirtPants_1"), 27,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_1.SKM_Overalls_Shirt_Pants_Worker_1"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_White_Cage.MI_Shirt_RolledUp_White_Cage"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Bege.MI_Pants_Worker_Bege"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Bege.MI_Boots_Bege") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerShirtPants_1"), TEXT("WorkerShirtPants"), TEXT("Beige"));
	AddDemo(TEXT("FullOutfit_WorkerShirtPants_2"), 28,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_2.SKM_Overalls_Shirt_Pants_Worker_2"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_Red.MI_Shirt_RolledUp_Red"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Gray.MI_Pants_Worker_Gray"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerShirtPants_2"), TEXT("WorkerShirtPants"), TEXT("Gray"));
	AddDemo(TEXT("FullOutfit_WorkerShirtPants_3"), 29,
		TEXT("/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_3.SKM_Overalls_Shirt_Pants_Worker_3"),
		{ TEXT("/Game/Modular_Workers/Materials/Clothes_Modules/Shirt_RolledUp/MI_Shirt_RolledUp_White_Cage.MI_Shirt_RolledUp_White_Cage"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/MI_Pants_Worker_Blue.MI_Pants_Worker_Blue"), TEXT("/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black.MI_Boots_Black") },
		TEXT("Customization.Item.FullOutfit.FullOutfit_WorkerShirtPants_3"), TEXT("WorkerShirtPants"), TEXT("Blue"));

	Catalog.Sort([](const FAvFullOutfitCatalogItem& Left, const FAvFullOutfitCatalogItem& Right)
	{
		return Left.SortOrder < Right.SortOrder;
	});
	return Catalog;
}

bool UWorkerAppearanceComponent::ResolveFullOutfitCatalogRecord(
	EWorkerSlot StoredSlot,
	const FSoftObjectPath& StoredPath,
	FAvFullOutfitCatalogItem& OutRecord)
{
	if (!StoredPath.IsValid())
	{
		return false;
	}
	const TArray<FAvFullOutfitCatalogItem> Catalog = GetFullOutfitCatalog();
	const FString StoredPathString = StoredPath.ToString();
	if (const FAvFullOutfitCatalogItem* Direct = Catalog.FindByPredicate(
		[StoredSlot, &StoredPathString](const FAvFullOutfitCatalogItem& Item)
		{
			return !Item.bIsNone && Item.bEnabled && Item.TargetSlot == StoredSlot &&
				Item.ExactMeshPath == StoredPathString;
		}))
	{
		OutRecord = *Direct;
		return true;
	}

	// The incomplete 12-card corrective pass exposed these lower-only bib meshes through
	// EWorkerSlot::Overalls. Keep old saves readable without putting them back in the visible
	// catalog: resolve each color to the original-demo full recipe with the matching pants color.
	if (StoredSlot != EWorkerSlot::Overalls)
	{
		return false;
	}
	struct FLegacyAlias
	{
		const TCHAR* StoredPath;
		int32 DemoIndex;
	};
	static const FLegacyAlias LegacyAliases[] =
	{
		{ TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Bege.SKM_Pants_Worker_Full_Bege"), 22 },
		{ TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Blue.SKM_Pants_Worker_Full_Blue"), 23 },
		{ TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Gray.SKM_Pants_Worker_Full_Gray"), 21 },
		{ TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Green.SKM_Pants_Worker_Full_Green"), 20 },
		{ TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_LightGrn.SKM_Pants_Worker_Full_LightGrn"), 25 },
		{ TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Orange.SKM_Pants_Worker_Full_Orange"), 26 }
	};
	for (const FLegacyAlias& Alias : LegacyAliases)
	{
		if (StoredPathString == Alias.StoredPath)
		{
			if (const FAvFullOutfitCatalogItem* Match = Catalog.FindByPredicate(
				[Alias](const FAvFullOutfitCatalogItem& Item)
				{
					return !Item.bIsNone && Item.bEnabled && Item.DemoIndex == Alias.DemoIndex;
				}))
			{
				OutRecord = *Match;
				return true;
			}
			break;
		}
	}
	return false;
}

TArray<FAvFaceProtectionCatalogItem> UWorkerAppearanceComponent::GetFaceProtectionCatalog()
{
	TArray<FAvFaceProtectionCatalogItem> Catalog;
	auto SectionName = [](EAvFaceProtectionSection Section)
	{
		switch (Section)
		{
		case EAvFaceProtectionSection::Glasses: return TEXT("Glasses");
		case EAvFaceProtectionSection::Respirator: return TEXT("Respirator");
		case EAvFaceProtectionSection::Headphones: return TEXT("Headphones");
		default: return TEXT("Unknown");
		}
	};
	auto MakeTexturePath = [](const FString& Directory, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *Directory, *AssetName, *AssetName);
	};
	auto BuildThumbnailPair = [SectionName, MakeTexturePath](
		EAvFaceProtectionSection Section,
		const FSoftObjectPath& ExactMeshPath,
		bool bIsNone,
		FString& OutType01,
		FString& OutType02)
	{
		const FString SectionFolder = SectionName(Section);
		const FString Root = TEXT("/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/FaceProtection");
		if (bIsNone)
		{
			const FString Directory = Root / TEXT("None") / SectionFolder;
			const FString Type01Name = FString::Printf(TEXT("T_UI_None_%s_Type01"), *SectionFolder);
			const FString Type02Name = FString::Printf(TEXT("T_UI_None_%s_Type02"), *SectionFolder);
			OutType01 = MakeTexturePath(Directory, Type01Name);
			OutType02 = MakeTexturePath(Directory, Type02Name);
			return;
		}

		const FString PackagePath = ExactMeshPath.GetLongPackageName();
		const FString AssetName = ExactMeshPath.GetAssetName();
		const FString FolderName = FPaths::GetCleanFilename(FPaths::GetPath(PackagePath));
		const FString StableAssetName = Section == EAvFaceProtectionSection::Headphones
			? AssetName
			: FString::Printf(TEXT("%s_%s"), *FolderName, *AssetName);
		const FString AssetPrefix = Section == EAvFaceProtectionSection::Headphones
			? TEXT("T_UI_HP")
			: FString::Printf(TEXT("T_UI_%s"), *SectionFolder);
		const FString Type01Name = FString::Printf(
			TEXT("%s_%s_Type01"), *AssetPrefix, *StableAssetName);
		const FString Type02Name = FString::Printf(
			TEXT("%s_%s_Type02"), *AssetPrefix, *StableAssetName);
		OutType01 = MakeTexturePath(Root / SectionFolder / TEXT("HeadType01"), Type01Name);
		OutType02 = MakeTexturePath(Root / SectionFolder / TEXT("HeadType02"), Type02Name);
	};
	auto AddNone = [&Catalog, &BuildThumbnailPair](
		EAvFaceProtectionSection Section,
		EWorkerSlot WorkerSlot,
		bool bEnabled,
		const TCHAR* DisabledReasonKey = TEXT(""))
	{
		FAvFaceProtectionCatalogItem Item;
		Item.StableId = FString::Printf(TEXT("None_%d"), static_cast<int32>(Section));
		Item.Section = Section;
		Item.WorkerSlot = WorkerSlot;
		Item.DisplayNameKey = TEXT("Customization.Common.None");
		Item.CurrentSelectionNameKey = Item.DisplayNameKey;
		Item.DisplayName = AvResolveLocalizedString(Item.DisplayNameKey);
		Item.CurrentSelectionName = Item.DisplayName;
		Item.bIsNone = true;
		Item.bEnabled = bEnabled;
		Item.DisabledReasonKey = DisabledReasonKey;
		Item.DisabledReason = AvResolveLocalizedString(Item.DisabledReasonKey);
		BuildThumbnailPair(
			Section,
			FSoftObjectPath(),
			true,
			Item.ThumbnailType01,
			Item.ThumbnailType02);
		Catalog.Add(MoveTemp(Item));
	};

	AddNone(EAvFaceProtectionSection::Glasses, EWorkerSlot::Glasses, true);
	AddNone(EAvFaceProtectionSection::Respirator, EWorkerSlot::FaceMask, true);
	AddNone(EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones, true);

	struct FCuratedDefinition
	{
		EAvFaceProtectionSection Section;
		EWorkerSlot WorkerSlot;
		const TCHAR* ExactMeshPath;
		const TCHAR* DisplayNameKey;
		const TCHAR* CurrentSelectionNameKey;
	};
	const FCuratedDefinition CuratedDefinitions[] =
	{
		{ EAvFaceProtectionSection::Glasses, EWorkerSlot::Glasses,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator.SKM_Glasses_Aviator"),
			TEXT("Customization.Item.Glasses.SKM_Glasses_Aviator"), TEXT("Customization.Item.Glasses.SKM_Glasses_Aviator") },
		{ EAvFaceProtectionSection::Glasses, EWorkerSlot::Glasses,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator_Brown.SKM_Glasses_Aviator_Brown"),
			TEXT("Customization.Item.Glasses.SKM_Glasses_Aviator_Brown"), TEXT("Customization.Item.Glasses.SKM_Glasses_Aviator_Brown") },
		{ EAvFaceProtectionSection::Glasses, EWorkerSlot::Glasses,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator_Gold.SKM_Glasses_Aviator_Gold"),
			TEXT("Customization.Item.Glasses.SKM_Glasses_Aviator_Gold"), TEXT("Customization.Item.Glasses.SKM_Glasses_Aviator_Gold") },
		{ EAvFaceProtectionSection::Glasses, EWorkerSlot::Glasses,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_Classic.SKM_Glasses_Classic"),
			TEXT("Customization.Item.Glasses.SKM_Glasses_Classic"), TEXT("Customization.Item.Glasses.SKM_Glasses_Classic") },
		{ EAvFaceProtectionSection::Glasses, EWorkerSlot::Glasses,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_SunGlass.SKM_Glasses_SunGlass"),
			TEXT("Customization.Item.Glasses.SKM_Glasses_SunGlass"), TEXT("Customization.Item.Glasses.SKM_Glasses_SunGlass") },
		{ EAvFaceProtectionSection::Glasses, EWorkerSlot::Glasses,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Worker/SKM_Glasses_Worker_Orange.SKM_Glasses_Worker_Orange"),
			TEXT("Customization.Item.Glasses.SKM_Glasses_Worker_Orange"), TEXT("Customization.Item.Glasses.SKM_Glasses_Worker_Orange") },
		{ EAvFaceProtectionSection::Glasses, EWorkerSlot::Glasses,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Worker/SKM_Glasses_Worker_White.SKM_Glasses_Worker_White"),
			TEXT("Customization.Item.Glasses.SKM_Glasses_Worker_White"), TEXT("Customization.Item.Glasses.SKM_Glasses_Worker_White") },
		{ EAvFaceProtectionSection::Respirator, EWorkerSlot::FaceMask,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Respirator/SKM_Respirator.SKM_Respirator"),
			TEXT("Customization.Item.FaceMask.SKM_Respirator"), TEXT("Customization.Item.FaceMask.SKM_Respirator") },
		{ EAvFaceProtectionSection::Respirator, EWorkerSlot::FaceMask,
			TEXT("/Game/Modular_Workers/Mesh/Male/Face_Modules/Respirator/SKM_Respirator_Black.SKM_Respirator_Black"),
			TEXT("Customization.Item.FaceMask.SKM_Respirator_Black"), TEXT("Customization.Item.FaceMask.SKM_Respirator_Black") },
		{ EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones,
			TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Green.SKM_Headphones_Worker_Green"),
			TEXT("Customization.Item.Headphones.SKM_Headphones_Worker_Green"), TEXT("Customization.Item.Headphones.SKM_Headphones_Worker_Green.CurrentSelection") },
		{ EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones,
			TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Orange.SKM_Headphones_Worker_Orange"),
			TEXT("Customization.Item.Headphones.SKM_Headphones_Worker_Orange"), TEXT("Customization.Item.Headphones.SKM_Headphones_Worker_Orange.CurrentSelection") },
		{ EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones,
			TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_White.SKM_Headphones_Worker_White"),
			TEXT("Customization.Item.Headphones.SKM_Headphones_Worker_White"), TEXT("Customization.Item.Headphones.SKM_Headphones_Worker_White.CurrentSelection") },
		{ EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones,
			TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Yellow.SKM_Headphones_Worker_Yellow"),
			TEXT("Customization.Item.Headphones.SKM_Headphones_Worker_Yellow"), TEXT("Customization.Item.Headphones.SKM_Headphones_Worker_Yellow.CurrentSelection") },
		{ EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones,
			TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Gr.SKM_Headph_Worker_Mic_Gr"),
			TEXT("Customization.Item.Headphones.SKM_Headph_Worker_Mic_Gr"), TEXT("Customization.Item.Headphones.SKM_Headph_Worker_Mic_Gr") },
		{ EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones,
			TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Orn.SKM_Headph_Worker_Mic_Orn"),
			TEXT("Customization.Item.Headphones.SKM_Headph_Worker_Mic_Orn"), TEXT("Customization.Item.Headphones.SKM_Headph_Worker_Mic_Orn") },
		{ EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones,
			TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Wht.SKM_Headph_Worker_Mic_Wht"),
			TEXT("Customization.Item.Headphones.SKM_Headph_Worker_Mic_Wht"), TEXT("Customization.Item.Headphones.SKM_Headph_Worker_Mic_Wht") },
		{ EAvFaceProtectionSection::Headphones, EWorkerSlot::Headphones,
			TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Yel.SKM_Headph_Worker_Mic_Yel"),
			TEXT("Customization.Item.Headphones.SKM_Headph_Worker_Mic_Yel"), TEXT("Customization.Item.Headphones.SKM_Headph_Worker_Mic_Yel") }
	};

	for (const FCuratedDefinition& Definition : CuratedDefinitions)
	{
		FAvFaceProtectionCatalogItem Item;
		const FSoftObjectPath ExactPath(Definition.ExactMeshPath);
		Item.StableId = ExactPath.GetAssetName();
		Item.Section = Definition.Section;
		Item.WorkerSlot = Definition.WorkerSlot;
		Item.ExactMeshPath = Definition.ExactMeshPath;
		Item.DisplayNameKey = Definition.DisplayNameKey;
		Item.CurrentSelectionNameKey = Definition.CurrentSelectionNameKey;
		Item.DisplayName = AvResolveLocalizedString(Item.DisplayNameKey);
		Item.CurrentSelectionName = AvResolveLocalizedString(Item.CurrentSelectionNameKey);
		const FAvRandomAssetEligibility* Eligibility =
			AvFindRandomAssetEligibility(Definition.WorkerSlot, ExactPath);
		const bool bCuratedHeadphones = Definition.WorkerSlot == EWorkerSlot::Headphones;
		const bool bGroupQuarantined = Eligibility &&
			AvGetQuarantinedSharedGeometryGroups().Contains(Eligibility->SharedGeometryGroup);
		Item.bEnabled = bCuratedHeadphones || (Eligibility &&
			Eligibility->bAllowInRandom &&
			Eligibility->AuditStatus == EAvRandomAuditStatus::VerifiedIntact &&
			Eligibility->VisualCondition == EAvRandomVisualCondition::Intact &&
			!bGroupQuarantined &&
			AvAuditEvidenceExists(Eligibility->AuditEvidence));
		if (!Item.bEnabled)
		{
			Item.DisabledReasonKey = TEXT("Customization.Disabled.AssetNotVerified");
			Item.DisabledReason = AvResolveLocalizedString(Item.DisabledReasonKey);
		}
		Item.bConflictsWithRaisedHood = bCuratedHeadphones &&
			AvGetRaisedHoodConflictingHeadphones().Contains(ExactPath);
		if (Item.bConflictsWithRaisedHood)
		{
			Item.BlockedCompatibilityTags.Add(TEXT("Torso.Hood.Raised"));
		}
		BuildThumbnailPair(
			Definition.Section,
			ExactPath,
			false,
			Item.ThumbnailType01,
			Item.ThumbnailType02);
		Catalog.Add(MoveTemp(Item));
	}
	return Catalog;
}

bool UWorkerAppearanceComponent::IsRaisedHoodTorsoPath(const FSoftObjectPath& TorsoPath)
{
	const FAvRandomFamilyDefinition* TorsoFamily = AvFindRandomFamily(
		EWorkerSlot::Torso,
		TorsoPath);
	return TorsoFamily && TorsoFamily->bHasIntegratedHood;
}

bool UWorkerAppearanceComponent::HasRaisedHood(const FWorkerAppearance& InAppearance)
{
	if (IsRaisedHoodTorsoPath(
		InAppearance.Get(EWorkerSlot::Torso).ToSoftObjectPath()))
	{
		return true;
	}

	const FAvRandomFamilyDefinition* FullOutfitFamily = AvFindRandomFamily(
		EWorkerSlot::FullOutfit,
		InAppearance.Get(EWorkerSlot::FullOutfit).ToSoftObjectPath());
	return FullOutfitFamily && FullOutfitFamily->bHasIntegratedHood;
}

bool UWorkerAppearanceComponent::HeadgearConflictsWithRaisedHood(
	const TSoftObjectPtr<USkeletalMesh>& HeadgearMesh)
{
	return !HeadgearMesh.IsNull() &&
		AvGetRaisedHoodConflictingHeadgear().Contains(HeadgearMesh.ToSoftObjectPath());
}

bool UWorkerAppearanceComponent::HeadphonesConflictWithRaisedHood(
	const TSoftObjectPtr<USkeletalMesh>& HeadphonesMesh)
{
	return !HeadphonesMesh.IsNull() &&
		AvGetRaisedHoodConflictingHeadphones().Contains(HeadphonesMesh.ToSoftObjectPath());
}

bool UWorkerAppearanceComponent::IsHeadgearCompatibleWithHeadphones(
	const TSoftObjectPtr<USkeletalMesh>& HeadgearMesh)
{
	return HeadgearMesh.IsNull() ||
		AvGetHeadphonesCompatibleHeadgear().Contains(HeadgearMesh.ToSoftObjectPath());
}

bool UWorkerAppearanceComponent::HeadgearConflictsWithHeadphones(
	const TSoftObjectPtr<USkeletalMesh>& HeadgearMesh,
	const TSoftObjectPtr<USkeletalMesh>& HeadphonesMesh)
{
	return !HeadphonesMesh.IsNull() &&
		!IsHeadgearCompatibleWithHeadphones(HeadgearMesh);
}

bool UWorkerAppearanceComponent::HasIntegratedHood(const FWorkerAppearance& InAppearance)
{
	return HasRaisedHood(InAppearance);
}

bool UWorkerAppearanceComponent::ShouldHeadgearHideHair(
	const TSoftObjectPtr<USkeletalMesh>& HeadgearMesh)
{
	if (HeadgearMesh.IsNull())
	{
		return false;
	}
	const FAvRandomAssetEligibility* Eligibility = AvFindRandomAssetEligibility(
		EWorkerSlot::Headgear,
		HeadgearMesh.ToSoftObjectPath());
	return Eligibility &&
		Eligibility->AuditStatus == EAvRandomAuditStatus::VerifiedIntact &&
		Eligibility->VisualCondition == EAvRandomVisualCondition::Intact &&
		Eligibility->bHidesHair;
}

bool UWorkerAppearanceComponent::IsHairSuppressedByHeadgear() const
{
	return !Appearance.Get(EWorkerSlot::Hair).IsNull() &&
		ShouldHeadgearHideHair(Appearance.Get(EWorkerSlot::Headgear));
}

bool UWorkerAppearanceComponent::ValidateHeadCustomizationSelection(
	const FWorkerAppearance& InAppearance,
	EWorkerSlot ChangedSlot,
	FString& OutValidationDetails)
{
	auto Fail = [&OutValidationDetails](const TCHAR* Reason)
	{
		OutValidationDetails = FString::Printf(TEXT("Result=FAIL Reason=%s"), Reason);
		return false;
	};

	if (InAppearance.Get(EWorkerSlot::Body).IsNull())
	{
		return Fail(TEXT("MissingFactoryBody"));
	}
	const FSoftObjectPath HeadPath = InAppearance.Get(EWorkerSlot::Head).ToSoftObjectPath();
	if (HeadPath != FSoftObjectPath(AvWorkerAssets::FactoryHead) &&
		HeadPath != FSoftObjectPath(AvWorkerAssets::FactoryHeadAfro))
	{
		return Fail(TEXT("HeadTypeNotInVerifiedCatalog"));
	}
	FWorkerAppearance NormalizedAppearance = InAppearance;
	NormalizeHeadTypeSkinPresentation(NormalizedAppearance);
	FString SkinPresentationValidation;
	if (!ValidateHeadTypeSkinPresentation(NormalizedAppearance, SkinPresentationValidation))
	{
		OutValidationDetails = SkinPresentationValidation;
		return false;
	}

	if (ChangedSlot == EWorkerSlot::Headgear &&
		!InAppearance.Get(EWorkerSlot::Headgear).IsNull() &&
		HasRaisedHood(InAppearance) &&
		HeadgearConflictsWithRaisedHood(InAppearance.Get(EWorkerSlot::Headgear)))
	{
		return Fail(TEXT("IntegratedHoodHeadgearConflict"));
	}

	if ((ChangedSlot == EWorkerSlot::Headgear ||
		 ChangedSlot == EWorkerSlot::Headphones) &&
		HeadgearConflictsWithHeadphones(
			InAppearance.Get(EWorkerSlot::Headgear),
			InAppearance.Get(EWorkerSlot::Headphones)))
	{
		return Fail(TEXT("HeadgearHeadphonesConflict"));
	}

	if (ChangedSlot == EWorkerSlot::Headphones)
	{
		const FSoftObjectPath SelectedPath = InAppearance.Get(ChangedSlot).ToSoftObjectPath();
		static const TSet<FSoftObjectPath> CuratedHeadphones =
		{
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Green.SKM_Headphones_Worker_Green")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Orange.SKM_Headphones_Worker_Orange")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_White.SKM_Headphones_Worker_White")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Yellow.SKM_Headphones_Worker_Yellow")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Gr.SKM_Headph_Worker_Mic_Gr")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Orn.SKM_Headph_Worker_Mic_Orn")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Wht.SKM_Headph_Worker_Mic_Wht")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Yel.SKM_Headph_Worker_Mic_Yel"))
		};
		if (!SelectedPath.IsNull() && !CuratedHeadphones.Contains(SelectedPath))
		{
			return Fail(TEXT("SelectedHeadphonesAssetNotInCuratedAllowlist"));
		}
		if (!SelectedPath.IsNull() && HasRaisedHood(InAppearance) &&
			HeadphonesConflictWithRaisedHood(InAppearance.Get(EWorkerSlot::Headphones)))
		{
			return Fail(TEXT("IntegratedHoodHeadphonesConflict"));
		}
	}

	if (ChangedSlot == EWorkerSlot::Gloves || ChangedSlot == EWorkerSlot::Watch)
	{
		const FSoftObjectPath SelectedPath = InAppearance.Get(ChangedSlot).ToSoftObjectPath();
		static const TSet<FSoftObjectPath> CuratedGloves =
		{
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Gray.SKM_Arms_Gloves_Print_Gray")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Green.SKM_Arms_Gloves_Print_Green")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Orange.SKM_Arms_Gloves_Print_Orange")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Yellow.SKM_Arms_Gloves_Print_Yellow"))
		};
		static const TSet<FSoftObjectPath> CuratedWatches =
		{
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_L_Bege.SKM_Watches_Military_L_Bege")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_L_Black.SKM_Watches_Military_L_Black")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_R_Bege.SKM_Watches_Military_R_Bege")),
			FSoftObjectPath(TEXT("/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_R_Black.SKM_Watches_Military_R_Black"))
		};
		const TSet<FSoftObjectPath>& Allowlist = ChangedSlot == EWorkerSlot::Gloves ? CuratedGloves : CuratedWatches;
		if (!SelectedPath.IsNull() && !Allowlist.Contains(SelectedPath))
		{
			return Fail(ChangedSlot == EWorkerSlot::Gloves
				? TEXT("SelectedGlovesAssetNotInCuratedAllowlist")
				: TEXT("SelectedWatchAssetNotInCuratedAllowlist"));
		}
	}

	if (ChangedSlot == EWorkerSlot::Legs || ChangedSlot == EWorkerSlot::HipAccessory)
	{
		const FSoftObjectPath SelectedPath = InAppearance.Get(ChangedSlot).ToSoftObjectPath();
		if (!SelectedPath.IsNull())
		{
			if (ChangedSlot == EWorkerSlot::Legs &&
				AvGetLowerHipFutureFullOutfitAssets().Contains(SelectedPath))
			{
				return Fail(TEXT("SelectedLegsAssetClassifiedAsFutureFullOutfit"));
			}
			const TArray<FAvLowerHipCatalogItem> Catalog = GetLowerHipCatalog();
			const bool bAllowed = Catalog.ContainsByPredicate(
				[ChangedSlot, &SelectedPath](const FAvLowerHipCatalogItem& Item)
				{
					return Item.WorkerSlot == ChangedSlot && !Item.bIsNone && Item.bEnabled &&
						FSoftObjectPath(Item.ExactMeshPath) == SelectedPath;
				});
			if (!bAllowed)
			{
				return Fail(ChangedSlot == EWorkerSlot::Legs
					? TEXT("SelectedLegsAssetNotInCuratedAllowlist")
					: TEXT("SelectedHipAssetNotInCuratedAllowlist"));
			}
		}
	}

	if (ChangedSlot == EWorkerSlot::FullOutfit || ChangedSlot == EWorkerSlot::Overalls)
	{
		const FSoftObjectPath SelectedPath = InAppearance.Get(ChangedSlot).ToSoftObjectPath();
		if (!SelectedPath.IsNull())
		{
			const TArray<FAvFullOutfitCatalogItem> Catalog = GetFullOutfitCatalog();
			const bool bAllowed = Catalog.ContainsByPredicate(
				[ChangedSlot, &SelectedPath](const FAvFullOutfitCatalogItem& Item)
				{
					return Item.TargetSlot == ChangedSlot && !Item.bIsNone && Item.bEnabled &&
						FSoftObjectPath(Item.ExactMeshPath) == SelectedPath;
				});
			if (!bAllowed)
			{
				return Fail(ChangedSlot == EWorkerSlot::FullOutfit
					? TEXT("SelectedFullOutfitAssetNotInCuratedAllowlist")
					: TEXT("SelectedOverallsAssetNotInCuratedAllowlist"));
			}
		}
	}

	if (ChangedSlot == EWorkerSlot::Headgear ||
		ChangedSlot == EWorkerSlot::Hair ||
		ChangedSlot == EWorkerSlot::Beard ||
		ChangedSlot == EWorkerSlot::FaceMask ||
		ChangedSlot == EWorkerSlot::Glasses)
	{
		const FSoftObjectPath SelectedPath = InAppearance.Get(ChangedSlot).ToSoftObjectPath();
		if (!SelectedPath.IsNull())
		{
			const FAvRandomAssetEligibility* Eligibility =
				AvFindRandomAssetEligibility(ChangedSlot, SelectedPath);
			const bool bGroupQuarantined = Eligibility &&
				AvGetQuarantinedSharedGeometryGroups().Contains(Eligibility->SharedGeometryGroup);
			if (!Eligibility ||
				!Eligibility->bAllowInRandom ||
				Eligibility->AuditStatus != EAvRandomAuditStatus::VerifiedIntact ||
				Eligibility->VisualCondition != EAvRandomVisualCondition::Intact ||
				bGroupQuarantined)
			{
				return Fail(TEXT("SelectedOptionalHeadAssetNotVerifiedIntact"));
			}
		}
	}

	OutValidationDetails = FString::Printf(
		TEXT("Result=PASS ChangedSlot=%d Head=%s HairSuppressed=%s IntegratedHood=%s %s"),
		static_cast<int32>(ChangedSlot),
		*HeadPath.ToString(),
		ShouldHeadgearHideHair(InAppearance.Get(EWorkerSlot::Headgear)) &&
			!InAppearance.Get(EWorkerSlot::Hair).IsNull() ? TEXT("true") : TEXT("false"),
		HasIntegratedHood(InAppearance) ? TEXT("true") : TEXT("false"),
		*SkinPresentationValidation);
	return true;
}

bool UWorkerAppearanceComponent::WriteHeadCustomizationCatalogDiagnostics(FString& OutPathOrError)
{
	auto EscapeJson = [](FString Value)
	{
		Value.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
		Value.ReplaceInline(TEXT("\""), TEXT("\\\""));
		Value.ReplaceInline(TEXT("\r"), TEXT("\\r"));
		Value.ReplaceInline(TEXT("\n"), TEXT("\\n"));
		return Value;
	};
	auto SectionName = [](EAvHeadCustomizationSection Section)
	{
		switch (Section)
		{
		case EAvHeadCustomizationSection::HeadType: return TEXT("HeadType");
		case EAvHeadCustomizationSection::Headgear: return TEXT("Headgear");
		case EAvHeadCustomizationSection::Hair: return TEXT("Hair");
		case EAvHeadCustomizationSection::Beard: return TEXT("Beard");
		default: return TEXT("Unknown");
		}
	};

	const TArray<FAvHeadCustomizationCatalogItem> Catalog = GetHeadCustomizationCatalog();
	TMap<EAvHeadCustomizationSection, int32> Counts;
	for (const FAvHeadCustomizationCatalogItem& Item : Catalog)
	{
		++Counts.FindOrAdd(Item.UISection);
	}

	FString Json = TEXT("{\n  \"generated_at\": \"") + FDateTime::Now().ToIso8601() + TEXT("\",\n");
	Json += TEXT("  \"catalog_source\": \"ExistingStructuredEligibilityPlusAuditedFaceRigHeadTypes\",\n");
	Json += FString::Printf(
		TEXT("  \"counts\": {\"HeadType\": %d, \"Headgear\": %d, \"Hair\": %d, \"Beard\": %d},\n  \"items\": [\n"),
		Counts.FindRef(EAvHeadCustomizationSection::HeadType),
		Counts.FindRef(EAvHeadCustomizationSection::Headgear),
		Counts.FindRef(EAvHeadCustomizationSection::Hair),
		Counts.FindRef(EAvHeadCustomizationSection::Beard));
	for (int32 Index = 0; Index < Catalog.Num(); ++Index)
	{
		const FAvHeadCustomizationCatalogItem& Item = Catalog[Index];
		Json += TEXT("    {\n");
		Json += FString::Printf(TEXT("      \"UISection\": \"%s\",\n"), SectionName(Item.UISection));
		Json += FString::Printf(TEXT("      \"EWorkerSlot\": %d,\n"), static_cast<int32>(Item.Slot));
		Json += FString::Printf(TEXT("      \"DisplayName\": \"%s\",\n"), *EscapeJson(Item.DisplayName));
		Json += FString::Printf(TEXT("      \"ExactObjectPath\": \"%s\",\n"), *EscapeJson(Item.ExactObjectPath));
		Json += FString::Printf(TEXT("      \"ExactHeadAssetPath\": \"%s\",\n"), *EscapeJson(Item.ExactHeadAssetPath));
		Json += FString::Printf(TEXT("      \"HeadTypeId\": \"%s\",\n"), *EscapeJson(Item.HeadTypeId.ToString()));
		Json += FString::Printf(TEXT("      \"SkinToneId\": \"%s\",\n"), *EscapeJson(Item.SkinToneId.ToString()));
		Json += FString::Printf(
			TEXT("      \"SkinColor\": [%.6f, %.6f, %.6f, %.6f],\n"),
			Item.SkinColor.R, Item.SkinColor.G, Item.SkinColor.B, Item.SkinColor.A);
		Json += FString::Printf(
			TEXT("      \"HeadColor\": [%.6f, %.6f, %.6f, %.6f],\n"),
			Item.HeadColor.R, Item.HeadColor.G, Item.HeadColor.B, Item.HeadColor.A);
		Json += FString::Printf(
			TEXT("      \"BodyColor\": [%.6f, %.6f, %.6f, %.6f],\n"),
			Item.BodyColor.R, Item.BodyColor.G, Item.BodyColor.B, Item.BodyColor.A);
		Json += FString::Printf(
			TEXT("      \"BodyCalibration\": {\"ColorCorrectionStrength\": %.6f, \"Brightness\": %.6f, \"Saturation\": %.6f, \"Contrast\": %.6f},\n"),
			Item.BodyColorCorrectionStrength,
			Item.BodyBrightness,
			Item.BodySaturation,
			Item.BodyContrast);
		Json += FString::Printf(TEXT("      \"Thumbnail\": \"%s\",\n"), *EscapeJson(Item.Thumbnail));
		Json += FString::Printf(TEXT("      \"ThumbnailType01\": \"%s\",\n"), *EscapeJson(Item.ThumbnailType01));
		Json += FString::Printf(TEXT("      \"ThumbnailType02\": \"%s\",\n"), *EscapeJson(Item.ThumbnailType02));
		Json += FString::Printf(TEXT("      \"Family\": \"%s\",\n"), *EscapeJson(Item.Family));
		Json += FString::Printf(TEXT("      \"AuditStatus\": \"%s\",\n"), *EscapeJson(Item.AuditStatus));
		Json += FString::Printf(TEXT("      \"VisualCondition\": \"%s\",\n"), *EscapeJson(Item.VisualCondition));
		Json += FString::Printf(TEXT("      \"AllowInUI\": %s,\n"), Item.bAllowInUI ? TEXT("true") : TEXT("false"));
		Json += FString::Printf(TEXT("      \"bHidesHair\": %s,\n"), Item.bHidesHair ? TEXT("true") : TEXT("false"));
		Json += FString::Printf(TEXT("      \"bConflictsWithIntegratedHood\": %s,\n"), Item.bConflictsWithIntegratedHood ? TEXT("true") : TEXT("false"));
		Json += TEXT("      \"compatibility\": [");
		for (int32 CompatibilityIndex = 0; CompatibilityIndex < Item.Compatibility.Num(); ++CompatibilityIndex)
		{
			Json += FString::Printf(
				TEXT("%s\"%s\""),
				CompatibilityIndex > 0 ? TEXT(", ") : TEXT(""),
				*EscapeJson(Item.Compatibility[CompatibilityIndex]));
		}
		Json += TEXT("],\n");
		Json += FString::Printf(TEXT("      \"tooltip_reason\": \"%s\",\n"), *EscapeJson(Item.TooltipReason));
		Json += FString::Printf(TEXT("      \"audit_evidence\": \"%s\"\n"), *EscapeJson(Item.AuditEvidence));
		Json += Index + 1 < Catalog.Num() ? TEXT("    },\n") : TEXT("    }\n");
	}
	Json += TEXT("  ]\n}\n");

	const FString Directory = FPaths::Combine(
		FPaths::ProjectSavedDir(),
		TEXT("CodexDiagnostics/HeadCustomization_20260713"));
	IFileManager::Get().MakeDirectory(*Directory, true);
	const FString FullPath = FPaths::Combine(Directory, TEXT("head_catalog.json"));
	if (!FFileHelper::SaveStringToFile(
		Json,
		*FullPath,
		FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		OutPathOrError = FString::Printf(TEXT("FailedToWrite=%s"), *FullPath);
		return false;
	}
	OutPathOrError = FullPath;
	return true;
}

bool UWorkerAppearanceComponent::RunHeadCustomizationCatalogSelfTest(FString& OutReport)
{
	TArray<FString> Errors;
	const TArray<FAvHeadCustomizationCatalogItem> Catalog = GetHeadCustomizationCatalog();
	TMap<EAvHeadCustomizationSection, int32> Counts;
	TSet<FString> ExactPaths;
	for (const FAvHeadCustomizationCatalogItem& Item : Catalog)
	{
		++Counts.FindOrAdd(Item.UISection);
		if (!Item.bAllowInUI)
		{
			Errors.Add(FString::Printf(TEXT("CatalogContainsDenied=%s"), *Item.ExactObjectPath));
		}
		if (ExactPaths.Contains(Item.ExactObjectPath))
		{
			Errors.Add(FString::Printf(TEXT("DuplicateExactPath=%s"), *Item.ExactObjectPath));
		}
		ExactPaths.Add(Item.ExactObjectPath);
		if (Item.ExactObjectPath.Contains(TEXT("/Female/")) ||
			Item.ExactObjectPath.Contains(TEXT("/Demo/")) ||
			Item.ExactObjectPath.Contains(TEXT("/Editor/")) ||
			Item.ExactObjectPath.Contains(TEXT("/Presets/")))
		{
			Errors.Add(FString::Printf(TEXT("ForbiddenSource=%s"), *Item.ExactObjectPath));
		}
		if (Item.ThumbnailType01.IsEmpty() || Item.ThumbnailType02.IsEmpty())
		{
			Errors.Add(FString::Printf(TEXT("MissingHeadTypeThumbnailPair=%s"), *Item.ExactObjectPath));
		}
		if (Item.UISection != EAvHeadCustomizationSection::HeadType &&
			Item.ThumbnailType01 == Item.ThumbnailType02)
		{
			Errors.Add(FString::Printf(TEXT("SharedHeadTypeThumbnail=%s"), *Item.ExactObjectPath));
		}

		USkeletalMesh* Mesh = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Item.ExactObjectPath)).LoadSynchronous();
		if (!Mesh || Mesh->GetMaterials().IsEmpty())
		{
			Errors.Add(FString::Printf(TEXT("MissingMeshOrMaterials=%s"), *Item.ExactObjectPath));
			continue;
		}
		if (Item.UISection != EAvHeadCustomizationSection::HeadType)
		{
			const FString SkeletonPath = Mesh->GetSkeleton() ? Mesh->GetSkeleton()->GetPathName() : TEXT("None");
			if (!SkeletonPath.Contains(CompatibleMaleSkeletonPath))
			{
				Errors.Add(FString::Printf(TEXT("MaleSkeletonMismatch=%s Skeleton=%s"), *Item.ExactObjectPath, *SkeletonPath));
			}
			if (Item.UISection == EAvHeadCustomizationSection::Headgear && !Item.bHidesHair)
			{
				Errors.Add(FString::Printf(TEXT("MissingStructuredHairPolicy=%s"), *Item.ExactObjectPath));
			}

			FWorkerAppearance Probe = MakeBaseMaleUnderwearAppearance();
			Probe.Set(Item.Slot, TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Item.ExactObjectPath)));
			FString Validation;
			if (!ValidateHeadCustomizationSelection(Probe, Item.Slot, Validation))
			{
				Errors.Add(FString::Printf(TEXT("SelectionRejected=%s %s"), *Item.ExactObjectPath, *Validation));
			}
		}
	}

	auto CheckCount = [&Errors, &Counts](EAvHeadCustomizationSection Section, int32 Expected, const TCHAR* Name)
	{
		if (Counts.FindRef(Section) != Expected)
		{
			Errors.Add(FString::Printf(TEXT("Count%s=%d Expected=%d"), Name, Counts.FindRef(Section), Expected));
		}
	};
	CheckCount(EAvHeadCustomizationSection::HeadType, 2, TEXT("HeadType"));
	CheckCount(EAvHeadCustomizationSection::Headgear, 22, TEXT("Headgear"));
	CheckCount(EAvHeadCustomizationSection::Hair, 5, TEXT("Hair"));
	CheckCount(EAvHeadCustomizationSection::Beard, 7, TEXT("Beard"));

	USkeletalMesh* Head01 = LoadObject<USkeletalMesh>(nullptr, AvWorkerAssets::FactoryHead);
	USkeletalMesh* Head02 = LoadObject<USkeletalMesh>(nullptr, AvWorkerAssets::FactoryHeadAfro);
	if (!Head01 || !Head02 || Head01 == Head02)
	{
		Errors.Add(TEXT("HeadTypesMustBeDistinctLoadedAssets"));
	}
	else
	{
		if (Head01->GetSkeleton() != Head02->GetSkeleton())
		{
			Errors.Add(TEXT("HeadTypeFaceRigSkeletonMismatch"));
		}
		if (Head01->GetMorphTargets().IsEmpty() || Head02->GetMorphTargets().IsEmpty())
		{
			Errors.Add(TEXT("HeadTypeMissingFaceRigMorphTargets"));
		}
	}

	FWorkerAppearance HoodProbe = MakeBaseMaleUnderwearAppearance();
	HoodProbe.Set(EWorkerSlot::Torso, TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(
		TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Bege.SKM_Jacket_Worker_Hood_2_Bege"))));
	HoodProbe.Set(EWorkerSlot::Headgear, AvWorkerAssets::Soft(AvWorkerAssets::Helmet));
	FString HoodValidation;
	if (!HasIntegratedHood(HoodProbe) ||
		ValidateHeadCustomizationSelection(HoodProbe, EWorkerSlot::Headgear, HoodValidation))
	{
		Errors.Add(TEXT("IntegratedHoodHeadgearMustReject"));
	}

	OutReport = FString::Printf(
		TEXT("Result=%s HeadType=%d Headgear=%d Hair=%d Beard=%d Records=%d Errors={%s}"),
		Errors.IsEmpty() ? TEXT("PASS") : TEXT("FAIL"),
		Counts.FindRef(EAvHeadCustomizationSection::HeadType),
		Counts.FindRef(EAvHeadCustomizationSection::Headgear),
		Counts.FindRef(EAvHeadCustomizationSection::Hair),
		Counts.FindRef(EAvHeadCustomizationSection::Beard),
		Catalog.Num(),
		*FString::Join(Errors, TEXT("; ")));
	return Errors.IsEmpty();
}

bool UWorkerAppearanceComponent::WriteSkinMaterialSectionAudit(FString& OutPathOrError)
{
	struct FMeshAuditInput
	{
		const TCHAR* Label;
		const TCHAR* ObjectPath;
	};
	const FMeshAuditInput Inputs[] =
	{
		{ TEXT("Body"), AvWorkerAssets::FactoryBody },
		{ TEXT("HeadType01"), AvWorkerAssets::FactoryHead },
		{ TEXT("HeadType02"), AvWorkerAssets::FactoryHeadAfro }
	};

	TArray<FString> MeshJson;
	bool bAllRenderDataAvailable = true;
	for (const FMeshAuditInput& Input : Inputs)
	{
		USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, Input.ObjectPath);
		const FSkeletalMeshRenderData* RenderData = Mesh ? Mesh->GetResourceForRendering() : nullptr;
		if (!Mesh || !RenderData)
		{
			bAllRenderDataAvailable = false;
			MeshJson.Add(FString::Printf(
				TEXT("    {\"component\":\"%s\",\"mesh\":\"%s\",\"error\":\"RenderDataUnavailable\"}"),
				Input.Label,
				Input.ObjectPath));
			continue;
		}

		TArray<FString> LodJson;
		for (int32 LodIndex = 0; LodIndex < RenderData->LODRenderData.Num(); ++LodIndex)
		{
			const FSkeletalMeshLODRenderData& LodData = RenderData->LODRenderData[LodIndex];
			TArray<FString> SectionJson;
			for (int32 SectionIndex = 0; SectionIndex < LodData.RenderSections.Num(); ++SectionIndex)
			{
				const int32 MaterialIndex = LodData.RenderSections[SectionIndex].MaterialIndex;
				FString SlotName;
				FString MaterialPath;
				if (Mesh->GetMaterials().IsValidIndex(MaterialIndex))
				{
					const FSkeletalMaterial& Material = Mesh->GetMaterials()[MaterialIndex];
					SlotName = Material.MaterialSlotName.ToString();
					MaterialPath = Material.MaterialInterface
						? Material.MaterialInterface->GetPathName()
						: FString();
				}
				const bool bBody = FCString::Strcmp(Input.Label, TEXT("Body")) == 0;
				const FString Classification = bBody
					? (MaterialIndex == 2
						? TEXT("skin+underwear_shared")
						: (MaterialIndex == 0
							? TEXT("skin_head_neck")
							: (MaterialIndex == 1 ? TEXT("skin_arms_hands") : TEXT("non_skin"))))
					: (MaterialIndex == 0 ? TEXT("skin_face_scalp_ears") : TEXT("non_skin"));
				SectionJson.Add(FString::Printf(
					TEXT("          {\"section_index\":%d,\"material_index\":%d,\"slot_name\":\"%s\",\"source_material\":\"%s\",\"classification\":\"%s\"}"),
					SectionIndex,
					MaterialIndex,
					*SlotName,
					*MaterialPath,
					*Classification));
			}
			LodJson.Add(FString::Printf(
				TEXT("      {\"lod_index\":%d,\"section_count\":%d,\"sections\":[\n%s\n        ]}"),
				LodIndex,
				LodData.RenderSections.Num(),
				*FString::Join(SectionJson, TEXT(",\n"))));
		}

		MeshJson.Add(FString::Printf(
			TEXT("    {\"component\":\"%s\",\"mesh\":\"%s\",\"lod_count\":%d,\"lods\":[\n%s\n    ]}"),
			Input.Label,
			Input.ObjectPath,
			RenderData->LODRenderData.Num(),
			*FString::Join(LodJson, TEXT(",\n"))));
	}

	const FString Output = FString::Printf(
		TEXT("{\n  \"phase\":\"persistent_fix\",\n  \"mappings\":[\n%s\n  ],\n  \"underwear_has_separate_section\":false,\n  \"underwear_shared_material_index\":2,\n  \"all_render_data_available\":%s\n}\n"),
		*FString::Join(MeshJson, TEXT(",\n")),
		bAllRenderDataAvailable ? TEXT("true") : TEXT("false"));
	const FString Directory = FPaths::Combine(
		FPaths::ProjectSavedDir(),
		TEXT("CodexDiagnostics/SkinToneMaterialConsistency_20260714"));
	IFileManager::Get().MakeDirectory(*Directory, true);
	const FString OutputPath = FPaths::Combine(Directory, TEXT("section_material_mapping.json"));
	if (!FFileHelper::SaveStringToFile(Output, *OutputPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		OutPathOrError = FString::Printf(TEXT("FailedToWrite=%s"), *OutputPath);
		return false;
	}
	OutPathOrError = OutputPath;
	return bAllRenderDataAvailable;
}

bool UWorkerAppearanceComponent::RunBodyCoverageSelfTest(FString& OutReport)
{
	TArray<FString> Errors;
	const uint16 ShortSleeveMask = AvBodyZones(
		EWorkerBodyZone::Torso, EWorkerBodyZone::UpperArms);
	const uint16 LongSleeveMask = AvBodyZones(
		EWorkerBodyZone::Torso, EWorkerBodyZone::UpperArms, EWorkerBodyZone::Forearms);
	const uint16 PantsMask = AvBodyZones(
		EWorkerBodyZone::Pelvis,
		EWorkerBodyZone::UpperLegs,
		EWorkerBodyZone::LowerLegs,
		EWorkerBodyZone::Feet);

	const TArray<FAvTorsoCatalogItem> TorsoCatalog = GetTorsoCatalog();
	int32 RealTorso = 0;
	for (const FAvTorsoCatalogItem& Item : TorsoCatalog)
	{
		if (Item.bIsNone)
		{
			if (Item.BodyCoverageMask != 0)
			{
				Errors.Add(TEXT("TorsoNoneCoverageMustBeNone"));
			}
			continue;
		}
		++RealTorso;
		const bool bLongSleeve =
			Item.Family == TEXT("WorkJacket") ||
			Item.Family == TEXT("ProtectiveJacket") ||
			Item.Family == TEXT("HoodJacket") ||
			Item.Family == TEXT("M65");
		const uint16 Expected = bLongSleeve ? LongSleeveMask : ShortSleeveMask;
		if (static_cast<uint16>(Item.BodyCoverageMask) != Expected)
		{
			Errors.Add(FString::Printf(
				TEXT("TorsoCoverageMismatch=%s Actual=%s Expected=%s"),
				*Item.StableId,
				*AvBodyCoverageToString(static_cast<uint16>(Item.BodyCoverageMask)),
				*AvBodyCoverageToString(Expected)));
		}
	}

	const TArray<FAvLowerHipCatalogItem> LowerCatalog = GetLowerHipCatalog();
	int32 RealLegs = 0;
	int32 RealHip = 0;
	for (const FAvLowerHipCatalogItem& Item : LowerCatalog)
	{
		if (Item.Section == EAvLowerHipSection::Legs && !Item.bIsNone)
		{
			++RealLegs;
			if (static_cast<uint16>(Item.BodyCoverageMask) != PantsMask)
			{
				Errors.Add(FString::Printf(TEXT("LegsCoverageMismatch=%s"), *Item.StableId));
			}
			if (Item.ExactMeshPath.Contains(TEXT("Pants_Worker_Full")))
			{
				Errors.Add(FString::Printf(TEXT("FullOutfitLeakedIntoLegs=%s"), *Item.ExactMeshPath));
			}
		}
		if (Item.Section == EAvLowerHipSection::Hip && !Item.bIsNone)
		{
			++RealHip;
			if (Item.BodyCoverageMask != 0)
			{
				Errors.Add(FString::Printf(TEXT("HipCoverageMustBeNone=%s"), *Item.StableId));
			}
		}
	}

	const TArray<FAvHandsAccessoriesCatalogItem> HandsCatalog = GetHandsAccessoriesCatalog();
	for (const FAvHandsAccessoriesCatalogItem& Item : HandsCatalog)
	{
		const int32 Expected =
			!Item.bIsNone && Item.Section == EAvHandsAccessoriesSection::Gloves
				? AvBodyZone(EWorkerBodyZone::Hands)
				: 0;
		if (Item.BodyCoverageMask != Expected)
		{
			Errors.Add(FString::Printf(TEXT("HandsCoverageMismatch=%s"), *Item.ExactMeshPath));
		}
	}

	const uint16 FullOutfitMask = AvBodyZones(
		EWorkerBodyZone::Torso,
		EWorkerBodyZone::Pelvis,
		EWorkerBodyZone::UpperArms,
		EWorkerBodyZone::UpperLegs,
		EWorkerBodyZone::LowerLegs) | AvBodyZone(EWorkerBodyZone::Feet);
	const TArray<FAvFullOutfitCatalogItem> EnsembleCatalog = GetFullOutfitCatalog();
	int32 NoneEnsembles = 0;
	int32 RealOveralls = 0;
	int32 RealFullOutfits = 0;
	int32 ExpectedDemoIndex = 0;
	for (const FAvFullOutfitCatalogItem& Item : EnsembleCatalog)
	{
		if (Item.bIsNone)
		{
			++NoneEnsembles;
			if (Item.BodyCoverageMask != 0 || !Item.ExactMeshPath.IsEmpty())
			{
				Errors.Add(TEXT("FullOutfitNoneMustHaveNoMeshOrCoverage"));
			}
			continue;
		}
		if (Item.TechnicalKind == EAvEnsembleTechnicalKind::Overalls)
		{
			++RealOveralls;
			if (Item.TargetSlot != EWorkerSlot::Overalls ||
				static_cast<uint16>(Item.BodyCoverageMask) != PantsMask ||
				Item.FallbackTorsoMeshPath != AvWorkerAssets::Tshirt ||
				static_cast<uint16>(Item.FallbackTorsoCoverageMask) != ShortSleeveMask ||
				!Item.SuppressedRenderSlots.Contains(EWorkerSlot::Legs) ||
				!Item.SuppressedRenderSlots.Contains(EWorkerSlot::Feet) ||
				Item.SuppressedRenderSlots.Contains(EWorkerSlot::Torso))
			{
				Errors.Add(FString::Printf(TEXT("OverallsMetadataMismatch=%s"), *Item.StableId));
			}
		}
		else if (Item.TechnicalKind == EAvEnsembleTechnicalKind::FullOutfit)
		{
			++RealFullOutfits;
			if (Item.TargetSlot != EWorkerSlot::FullOutfit ||
				Item.DemoIndex != ExpectedDemoIndex ||
				Item.PrimaryMeshPath != Item.ExactMeshPath ||
				Item.PrimaryMaterialOverrides != Item.ExactMaterialOverrides ||
				!Item.CompanionTorsoMeshPath.IsEmpty() ||
				!Item.CompanionChestMeshPath.IsEmpty() ||
				!Item.CompanionLegsMeshPath.IsEmpty() ||
				!Item.CompanionFeetMeshPath.IsEmpty() ||
				!Item.AdditionalPackageModules.IsEmpty() ||
				static_cast<uint16>(Item.BodyCoverageMask) != FullOutfitMask ||
				!Item.SuppressedRenderSlots.Contains(EWorkerSlot::Torso) ||
				!Item.SuppressedRenderSlots.Contains(EWorkerSlot::Legs) ||
				!Item.SuppressedRenderSlots.Contains(EWorkerSlot::Feet))
			{
				Errors.Add(FString::Printf(TEXT("FullOutfitMetadataMismatch=%s"), *Item.StableId));
			}
			++ExpectedDemoIndex;
		}
		else
		{
			Errors.Add(FString::Printf(TEXT("UnexpectedEnsembleKind=%s"), *Item.StableId));
		}
	}
	if (EnsembleCatalog.Num() != 31 || NoneEnsembles != 1 ||
		RealOveralls != 0 || RealFullOutfits != 30 || ExpectedDemoIndex != 30)
	{
		Errors.Add(FString::Printf(
			TEXT("EnsembleCatalogCountsMismatch None=%d Overalls=%d FullOutfit=%d"),
			NoneEnsembles, RealOveralls, RealFullOutfits));
	}
	if (static_cast<int32>(EWorkerSlot::FullOutfit) != 12 ||
		static_cast<int32>(EWorkerSlot::Headphones) != 13 ||
		static_cast<int32>(EWorkerSlot::Watch) != 14 ||
		static_cast<int32>(EWorkerSlot::HipAccessory) != 15 ||
		static_cast<int32>(EWorkerSlot::Overalls) != 16)
	{
		Errors.Add(TEXT("AppendOnlyEnumValuesChanged"));
	}

	USkeletalMesh* FactoryBody = LoadObject<USkeletalMesh>(nullptr, AvWorkerAssets::FactoryBody);
	USkeletalMesh* BareTorso = LoadObject<USkeletalMesh>(nullptr, AvWorkerAssets::BareTorso);
	USkeletalMesh* BodyBottom = LoadObject<USkeletalMesh>(nullptr, AvWorkerAssets::BodyBottom);
	USkeletalMesh* BareArms = LoadObject<USkeletalMesh>(nullptr, AvWorkerAssets::BareHands);
	USkeletalMesh* BareWrist = LoadObject<USkeletalMesh>(nullptr, AvWorkerAssets::BareWrist);
	const USkeleton* ExpectedSkeleton = FactoryBody ? FactoryBody->GetSkeleton() : nullptr;
	for (const TPair<const TCHAR*, USkeletalMesh*>& Module :
		{
			TPair<const TCHAR*, USkeletalMesh*>(TEXT("FactoryBody"), FactoryBody),
			TPair<const TCHAR*, USkeletalMesh*>(TEXT("BareTorso"), BareTorso),
			TPair<const TCHAR*, USkeletalMesh*>(TEXT("BodyBottom"), BodyBottom),
			TPair<const TCHAR*, USkeletalMesh*>(TEXT("BareArms"), BareArms),
			TPair<const TCHAR*, USkeletalMesh*>(TEXT("BareWrist"), BareWrist)
		})
	{
		if (!Module.Value)
		{
			Errors.Add(FString::Printf(TEXT("MissingPackageModule=%s"), Module.Key));
		}
		else if (ExpectedSkeleton && Module.Value->GetSkeleton() != ExpectedSkeleton)
		{
			Errors.Add(FString::Printf(TEXT("PackageModuleSkeletonMismatch=%s"), Module.Key));
		}
	}
	if (!FactoryBody || FactoryBody->GetMaterials().Num() < 3)
	{
		Errors.Add(TEXT("FactoryBodyRequiresHeadArmsBodyMaterialRegions"));
	}
	if (RealTorso != 35 || RealLegs != 11 || RealHip != 3)
	{
		Errors.Add(FString::Printf(
			TEXT("CatalogCountsMismatch Torso=%d Legs=%d Hip=%d"),
			RealTorso, RealLegs, RealHip));
	}

	OutReport = FString::Printf(
		TEXT("Result=%s TorsoReal=%d LegsReal=%d HipReal=%d OverallsReal=%d FullOutfitReal=%d ")
		TEXT("ShortSleeve=%s LongSleeve=%s Pants=%s SaveSchema=AppendOnlyOveralls16 Errors={%s}"),
		Errors.IsEmpty() ? TEXT("PASS") : TEXT("FAIL"),
		RealTorso,
		RealLegs,
		RealHip,
		RealOveralls,
		RealFullOutfits,
		*AvBodyCoverageToString(ShortSleeveMask),
		*AvBodyCoverageToString(LongSleeveMask),
		*AvBodyCoverageToString(PantsMask),
		*FString::Join(Errors, TEXT("; ")));
	return Errors.IsEmpty();
}

bool UWorkerAppearanceComponent::RunHeadSkinPresentationSelfTest(FString& OutReport)
{
	TArray<FString> Errors;
	const TArray<FAvHeadCustomizationCatalogItem> Catalog = GetHeadCustomizationCatalog();
	TArray<FAvHeadCustomizationCatalogItem> HeadTypes;
	for (const FAvHeadCustomizationCatalogItem& Item : Catalog)
	{
		if (Item.UISection == EAvHeadCustomizationSection::HeadType)
		{
			HeadTypes.Add(Item);
			if (Item.ExactHeadAssetPath.IsEmpty() || Item.HeadTypeId.IsNone() ||
				Item.Thumbnail.IsEmpty() || Item.ThumbnailType01.IsEmpty() ||
				Item.ThumbnailType02.IsEmpty())
			{
				Errors.Add(FString::Printf(TEXT("IncompleteHeadTypeMetadata=%s"), *Item.DisplayName));
			}
			if (!Item.SkinToneId.IsNone())
			{
				Errors.Add(FString::Printf(TEXT("HeadTypeMustNotOwnSkinTone=%s"), *Item.DisplayName));
			}
			if (Item.ExactHeadAssetPath != Item.ExactObjectPath)
			{
				Errors.Add(FString::Printf(TEXT("ExactHeadPathMismatch=%s"), *Item.DisplayName));
			}
		}
	}
	if (HeadTypes.Num() != 2)
	{
		Errors.Add(FString::Printf(TEXT("HeadTypeCount=%d Expected=2"), HeadTypes.Num()));
	}
	else if (HeadTypes[0].HeadTypeId == HeadTypes[1].HeadTypeId ||
		HeadTypes[0].ExactHeadAssetPath == HeadTypes[1].ExactHeadAssetPath ||
		HeadTypes[0].Thumbnail == HeadTypes[1].Thumbnail)
	{
		Errors.Add(TEXT("HeadTypeMetadataMustBeDistinct"));
	}

	FWorkerAppearance Factory = MakeBaseMaleUnderwearAppearance();
	FString FactoryValidation;
	if (!ValidateHeadTypeSkinPresentation(Factory, FactoryValidation) ||
		Factory.Get(EWorkerSlot::Head).ToSoftObjectPath() != FSoftObjectPath(AvWorkerAssets::FactoryHead) ||
		!Factory.bUseSkinColor || !Factory.SkinColor.Equals(
			FLinearColor(1.00f, 0.80f, 0.64f, 1.f), KINDA_SMALL_NUMBER) ||
		!Factory.Get(EWorkerSlot::Gloves).IsNull())
	{
		Errors.Add(TEXT("FactoryMustBeHeadType01SkinTone01GlovesNone"));
	}

	int32 ProfilePassCount = 0;
	for (const FAvHeadTypePresentation& Presentation : AvGetHeadTypePresentations())
	{
		FWorkerAppearance Probe = Factory;
		Probe.Set(EWorkerSlot::Head, TSoftObjectPtr<USkeletalMesh>(Presentation.ExactHeadAssetPath));
		Probe.SetSkinColor(FLinearColor(0.50f, 0.30f, 0.19f, 1.f));
		const bool bNormalized = NormalizeHeadTypeSkinPresentation(Probe);
		FString Validation;
		if (!bNormalized ||
			!ValidateHeadTypeSkinPresentation(Probe, Validation) ||
			!Probe.SkinColor.Equals(Presentation.SkinColor, KINDA_SMALL_NUMBER))
		{
			Errors.Add(FString::Printf(
				TEXT("HeadTypeSkinProfileFailed HeadType=%s Profile=%s %s"),
				*Presentation.HeadTypeId.ToString(),
				*Presentation.SkinProfileId.ToString(),
				*Validation));
		}
		else
		{
			++ProfilePassCount;
		}
	}

	const FWorkerAppearance RandomAppearance = MakeRandomCompatibleMaleAppearance(FWorkerAppearance());
	FString RandomValidation;
	FAvHeadCustomizationCatalogItem RandomHeadMetadata;
	if (!ValidateRandomGeneratedMaleAppearance(RandomAppearance, RandomValidation) ||
		!ValidateHeadTypeSkinPresentation(RandomAppearance, RandomValidation) ||
		!TryGetHeadTypeMetadata(RandomAppearance.Get(EWorkerSlot::Head), RandomHeadMetadata) ||
		!RandomAppearance.Get(EWorkerSlot::Gloves).IsNull())
	{
		Errors.Add(TEXT("RandomMustUseCanonicalHeadTypeProfileAndGlovesNone"));
	}

	if (!AvIsSupportedSkinMaterial(TEXT("M_Quantum_Head1"), TEXT("MI_Quantum_Head_Afro")) ||
		!AvIsSupportedSkinMaterial(TEXT("M_Quantum_Body"), TEXT("MI_Quantum_Body")) ||
		!AvIsSupportedSkinMaterial(TEXT("M_Quantum_Body1"), TEXT("MI_Quantum_Body")) ||
		!AvIsSupportedSkinMaterial(TEXT("M_Quantum_Arms1"), TEXT("MI_Quantum_Arms")) ||
		!AvIsSupportedSkinMaterial(TEXT("M_Quantum_Arms2"), TEXT("MI_Quantum_Arms")))
	{
		Errors.Add(TEXT("VerifiedSkinMaterialAllowlistMissingExpectedSlot"));
	}
	if (AvIsSupportedSkinMaterial(TEXT("M_TShirt"), TEXT("MI_TShirt_Black")) ||
		AvIsSupportedSkinMaterial(TEXT("M_Gloves_Print"), TEXT("MI_Gloves_Worker_Print_Yellow")) ||
		AvIsSupportedSkinMaterial(TEXT("M_Hair"), TEXT("MI_Hair")) ||
		AvIsSupportedSkinMaterial(TEXT("M_Beard"), TEXT("MI_Beard")))
	{
		Errors.Add(TEXT("ClothingOrAccessoryMustNeverEnterSkinMaterialAllowlist"));
	}
	if (GetDefault<UWorkerAppearanceComponent>()->PrimaryComponentTick.bCanEverTick)
	{
		Errors.Add(TEXT("SkinAppearanceComponentMustNotCreateMIDsFromTick"));
	}

	OutReport = FString::Printf(
		TEXT("Result=%s HeadTypes=%d SkinProfiles=%d/2 Factory={%s} Random={%s} SkinMaterialBoundary=%s TickDisabled=%s Errors={%s}"),
		Errors.IsEmpty() ? TEXT("PASS") : TEXT("FAIL"),
		HeadTypes.Num(),
		ProfilePassCount,
		*FactoryValidation,
		*RandomValidation,
		Errors.Contains(TEXT("ClothingOrAccessoryMustNeverEnterSkinMaterialAllowlist")) ? TEXT("FAIL") : TEXT("PASS"),
		GetDefault<UWorkerAppearanceComponent>()->PrimaryComponentTick.bCanEverTick ? TEXT("false") : TEXT("true"),
		*FString::Join(Errors, TEXT("; ")));
	UE_LOG(LogTemp, Log, TEXT("[AvHeadSkinPresentationSelfTest] %s"), *OutReport);
	return Errors.IsEmpty();
}

bool UWorkerAppearanceComponent::RunRandomHeadDistributionSelfTest(FString& OutReport)
{
	constexpr int32 Iterations = 200;
	int32 HeadType01Count = 0;
	int32 HeadType02Count = 0;
	int32 UnknownCount = 0;
	int32 InvalidCount = 0;
	int32 FallbackCount = 0;
	int32 CandidateCount = 0;
	TArray<FString> CandidateStableIds;
	TArray<FString> CandidatePaths;
	for (int32 Index = 0; Index < Iterations; ++Index)
	{
		FWorkerAppearance Probe = MakeBaseMaleUnderwearAppearance();
		FAvRandomHeadSelectionDiagnostics Diagnostics;
		const bool bSelected = AvSelectCanonicalRandomHeadType(Probe, &Diagnostics);
		CandidateCount = Diagnostics.CandidateCount;
		CandidateStableIds = Diagnostics.CandidateStableIds;
		CandidatePaths = Diagnostics.CandidatePaths;
		FallbackCount += Diagnostics.bFallbackUsed ? 1 : 0;
		if (!bSelected || Diagnostics.SelectedIndex == INDEX_NONE ||
			!Diagnostics.ValidationResult.StartsWith(TEXT("Result=PASS")))
		{
			++InvalidCount;
			continue;
		}
		if (Diagnostics.SelectedStableId == TEXT("HeadType01"))
		{
			++HeadType01Count;
		}
		else if (Diagnostics.SelectedStableId == TEXT("HeadType02"))
		{
			++HeadType02Count;
		}
		else
		{
			++UnknownCount;
		}
	}

	const float Type01Percent = 100.f * HeadType01Count / Iterations;
	const float Type02Percent = 100.f * HeadType02Count / Iterations;
	const bool bPass =
		CandidateCount == 2 &&
		CandidateStableIds.Contains(TEXT("HeadType01")) &&
		CandidateStableIds.Contains(TEXT("HeadType02")) &&
		HeadType01Count > 0 &&
		HeadType02Count > 0 &&
		Type01Percent >= 35.f && Type01Percent <= 65.f &&
		Type02Percent >= 35.f && Type02Percent <= 65.f &&
		UnknownCount == 0 && InvalidCount == 0 && FallbackCount == 0;
	OutReport = FString::Printf(
		TEXT("Result=%s Iterations=%d CandidateCount=%d CandidateStableIds=%s CandidatePaths=%s HeadType01=%d HeadType02=%d HeadType01Pct=%.1f HeadType02Pct=%.1f Unknown=%d Invalid=%d Fallbacks=%d SeedSource=FMath::RandRange_GlobalProcessPRNG_NoPerClickReseed SaveGameMutations=0"),
		bPass ? TEXT("PASS") : TEXT("FAIL"),
		Iterations,
		CandidateCount,
		*FString::Join(CandidateStableIds, TEXT(",")),
		*FString::Join(CandidatePaths, TEXT(",")),
		HeadType01Count,
		HeadType02Count,
		Type01Percent,
		Type02Percent,
		UnknownCount,
		InvalidCount,
		FallbackCount);
	UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeRandomHeadDistributionTest] %s"), *OutReport);
	return bPass;
}

bool UWorkerAppearanceComponent::RunHeadgearHeadphonesCompatibilitySelfTest(
	FString& OutReport)
{
	const TArray<FAvHeadCustomizationCatalogItem> Catalog = GetHeadCustomizationCatalog();
	const TArray<FAvFaceProtectionCatalogItem> FaceCatalog = GetFaceProtectionCatalog();
	const FAvFaceProtectionCatalogItem* Headphones = FaceCatalog.FindByPredicate(
		[](const FAvFaceProtectionCatalogItem& Item)
		{
			return Item.Section == EAvFaceProtectionSection::Headphones &&
				!Item.bIsNone && Item.bEnabled;
		});
	TArray<FString> Errors;
	int32 HeadgearCount = 0;
	int32 CompatibleCount = 0;
	int32 IncompatibleCount = 0;
	TSet<FString> StableIds;
	TSet<FSoftObjectPath> Paths;
	for (const FAvHeadCustomizationCatalogItem& Item : Catalog)
	{
		if (Item.UISection != EAvHeadCustomizationSection::Headgear || !Item.bAllowInUI)
		{
			continue;
		}
		++HeadgearCount;
		const FSoftObjectPath Path(Item.ExactObjectPath);
		const bool bExpectedCompatible = AvGetHeadphonesCompatibleHeadgear().Contains(Path);
		if (Item.bCompatibleWithHeadphones != bExpectedCompatible ||
			IsHeadgearCompatibleWithHeadphones(TSoftObjectPtr<USkeletalMesh>(Path)) !=
				bExpectedCompatible)
		{
			Errors.Add(TEXT("CompatibilityMetadataMismatch:") + Item.StableId);
		}
		if (StableIds.Contains(Item.StableId))
		{
			Errors.Add(TEXT("DuplicateStableId:") + Item.StableId);
		}
		StableIds.Add(Item.StableId);
		if (Paths.Contains(Path))
		{
			Errors.Add(TEXT("DuplicatePath:") + Path.ToString());
		}
		Paths.Add(Path);
		bExpectedCompatible ? ++CompatibleCount : ++IncompatibleCount;
		if (Headphones)
		{
			const bool bConflict = HeadgearConflictsWithHeadphones(
				TSoftObjectPtr<USkeletalMesh>(Path),
				TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(Headphones->ExactMeshPath)));
			if (bConflict == bExpectedCompatible)
			{
				Errors.Add(TEXT("SymmetryPolicyMismatch:") + Item.StableId);
			}
		}
	}
	if (HeadgearCount != 22 || CompatibleCount != 6 || IncompatibleCount != 16)
	{
		Errors.Add(FString::Printf(
			TEXT("CatalogCounts Headgear=%d Compatible=%d Incompatible=%d"),
			HeadgearCount, CompatibleCount, IncompatibleCount));
	}
	if (!IsHeadgearCompatibleWithHeadphones(TSoftObjectPtr<USkeletalMesh>()))
	{
		Errors.Add(TEXT("NoneHeadgearMustBeCompatible"));
	}
	if (!Headphones)
	{
		Errors.Add(TEXT("MissingEnabledHeadphonesControl"));
	}
	for (int32 Iteration = 0; Iteration < 32; ++Iteration)
	{
		const FWorkerAppearance RandomAppearance =
			MakeRandomCompatibleMaleAppearance(FWorkerAppearance());
		if (HeadgearConflictsWithHeadphones(
			RandomAppearance.Get(EWorkerSlot::Headgear),
			RandomAppearance.Get(EWorkerSlot::Headphones)) ||
			!RandomAppearance.Get(EWorkerSlot::FullOutfit).IsNull() ||
			!RandomAppearance.Get(EWorkerSlot::Overalls).IsNull() ||
			!RandomAppearance.Get(EWorkerSlot::Gloves).IsNull())
		{
			Errors.Add(FString::Printf(TEXT("RandomPolicyViolation:%d"), Iteration));
			break;
		}
	}

	OutReport = FString::Printf(
		TEXT("Result=%s Headgear=%d CompatibleCap=%d Incompatible=%d HeadphonesControl=%s RandomSamples=32 FullOutfit=None Overalls=None Gloves=None Errors={%s}"),
		Errors.IsEmpty() ? TEXT("PASS") : TEXT("FAIL"),
		HeadgearCount,
		CompatibleCount,
		IncompatibleCount,
		Headphones ? TEXT("true") : TEXT("false"),
		*FString::Join(Errors, TEXT(";")));
	UE_LOG(LogTemp, Warning,
		TEXT("[AvHeadgearHeadphonesCompatibilitySelfTest] %s"), *OutReport);
	return Errors.IsEmpty();
}

bool UWorkerAppearanceComponent::WriteHeadgearHeadphonesCompatibilityDiagnostics(
	FString& OutPathOrError)
{
	const TArray<FAvHeadCustomizationCatalogItem> Catalog = GetHeadCustomizationCatalog();
	FString Json = TEXT("{\n  \"policy\": \"HeadphonesAllowOnlyExactOrdinaryCapFamily\",\n  \"records\": [\n");
	bool bFirst = true;
	int32 HeadgearCount = 0;
	int32 CompatibleCount = 0;
	for (const FAvHeadCustomizationCatalogItem& Item : Catalog)
	{
		if (Item.UISection != EAvHeadCustomizationSection::Headgear || !Item.bAllowInUI)
		{
			continue;
		}
		if (!bFirst)
		{
			Json += TEXT(",\n");
		}
		bFirst = false;
		++HeadgearCount;
		CompatibleCount += Item.bCompatibleWithHeadphones ? 1 : 0;
		Json += FString::Printf(
			TEXT("    {\"stable_id\":\"%s\",\"object_path\":\"%s\",\"family\":\"%s\",\"display_name\":\"%s\",\"compatible_with_headphones\":%s,\"reason\":\"%s\"}"),
			*AvJsonEscape(Item.StableId),
			*AvJsonEscape(Item.ExactObjectPath),
			*AvJsonEscape(Item.Family),
			*AvJsonEscape(Item.DisplayName),
			Item.bCompatibleWithHeadphones ? TEXT("true") : TEXT("false"),
			*AvJsonEscape(Item.HeadphonesCompatibilityReason));
	}
	Json += FString::Printf(
		TEXT("\n  ],\n  \"headgear_count\": %d,\n  \"compatible_count\": %d,\n  \"incompatible_count\": %d\n}\n"),
		HeadgearCount, CompatibleCount, HeadgearCount - CompatibleCount);

	const FString Directory = FPaths::Combine(
		FPaths::ProjectSavedDir(),
		TEXT("CodexDiagnostics/ModularSelectionAndHeadphonesCompatibility_20260717"));
	IFileManager::Get().MakeDirectory(*Directory, true);
	const FString Path = FPaths::Combine(Directory, TEXT("headgear_compatibility_audit.json"));
	if (!FFileHelper::SaveStringToFile(Json, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		OutPathOrError = TEXT("FailedToWrite:") + Path;
		return false;
	}
	OutPathOrError = Path;
	return HeadgearCount == 22 && CompatibleCount == 6;
}

bool UWorkerAppearanceComponent::RunRandomEligibilityExhaustiveSelfTest(FString& OutReport)
{
	TArray<FString> Errors;
	auto AddError = [&Errors](const FString& Error)
	{
		Errors.Add(Error);
	};
	auto ContainsExactAsset = [](const FWorkerAppearance& Appearance, const FSoftObjectPath& AssetPath)
	{
		return Appearance.Slots.ContainsByPredicate(
			[&AssetPath](const FWorkerSlotMesh& SlotMesh)
			{
				return SlotMesh.Mesh.ToSoftObjectPath() == AssetPath;
			});
	};

	TMap<EWorkerSlot, TArray<FAvRandomCandidate>> Candidates;
	TMap<EWorkerSlot, int32> StructurallyEligibleCounts;
	for (const FAvRandomFamilyDefinition& Family : RandomFamilies)
	{
		AvScanRandomFolder(
			Family,
			Candidates.FindOrAdd(Family.Slot),
			StructurallyEligibleCounts.FindOrAdd(Family.Slot));
	}
	TSet<FSoftObjectPath> CandidatePaths;
	for (TPair<EWorkerSlot, TArray<FAvRandomCandidate>>& Pair : Candidates)
	{
		AvSortAndUnique(Pair.Value);
		for (const FAvRandomCandidate& Candidate : Pair.Value)
		{
			CandidatePaths.Add(Candidate.Asset.GetSoftObjectPath());
		}
	}

	const FSoftObjectPath SafeTorso = AvExactMeshPath(
		TEXT("/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Black"));
	const FSoftObjectPath SafeLower = AvExactMeshPath(
		TEXT("/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Bege"));
	auto MakeManualAppearance = [&SafeTorso, &SafeLower](const FAvRandomAssetEligibility& Entry)
	{
		FWorkerAppearance Appearance = MakeBaseMaleUnderwearAppearance();
		if (Entry.Slot == EWorkerSlot::FullOutfit)
		{
			Appearance.Set(
				EWorkerSlot::FullOutfit,
				TSoftObjectPtr<USkeletalMesh>(Entry.AssetPath));
			return Appearance;
		}
		Appearance.Set(EWorkerSlot::Torso, TSoftObjectPtr<USkeletalMesh>(SafeTorso));
		Appearance.Set(EWorkerSlot::Legs, TSoftObjectPtr<USkeletalMesh>(SafeLower));
		Appearance.Set(Entry.Slot, TSoftObjectPtr<USkeletalMesh>(Entry.AssetPath));
		return Appearance;
	};

	int32 AllowedCount = 0;
	int32 DeniedCount = 0;
	int32 AllowedFinalValidationPasses = 0;
	int32 DeniedFinalValidationRejects = 0;
	int32 LegacyCarryoverRejects = 0;
	int32 GloveBodyBaselineRejects = 0;
	int32 GloveTorsoPairRejects = 0;
	int32 GloveFullOutfitPairRejects = 0;
	int32 GlovesNoneValidationPasses = 0;
	int32 RandomGlovesNonePasses = 0;
	TSet<FSoftObjectPath> DeniedPaths;
	for (const TPair<FSoftObjectPath, FAvRandomAssetEligibility>& Pair :
		AvGetRandomAssetEligibility())
	{
		const FSoftObjectPath& Key = Pair.Key;
		const FAvRandomAssetEligibility& Entry = Pair.Value;
		const bool bGroupQuarantined =
			AvGetQuarantinedSharedGeometryGroups().Contains(Entry.SharedGeometryGroup);
		const bool bAllowed = Entry.bAllowInRandom &&
			Entry.AuditStatus == EAvRandomAuditStatus::VerifiedIntact &&
			Entry.VisualCondition == EAvRandomVisualCondition::Intact &&
			!bGroupQuarantined;

		if (Key != Entry.AssetPath)
		{
			AddError(FString::Printf(TEXT("EligibilityKeyMismatch:%s"), *Key.ToString()));
		}
		if (Entry.AssetPath.GetLongPackageName().IsEmpty() ||
			Entry.AssetPath.GetAssetName().IsEmpty() ||
			Entry.AssetPath.GetAssetName() !=
				FPackageName::GetLongPackageAssetName(Entry.AssetPath.GetLongPackageName()))
		{
			AddError(FString::Printf(TEXT("NonCanonicalExactObjectPath:%s"), *Key.ToString()));
		}

		FString ValidationDetails;
		const FWorkerAppearance ManualAppearance = MakeManualAppearance(Entry);
		const bool bFinalValidation = ValidateRandomGeneratedMaleAppearance(
			ManualAppearance, ValidationDetails);
		if (bAllowed)
		{
			++AllowedCount;
			if (Entry.AuditEvidence.IsEmpty() || Entry.SharedGeometryGroup.IsEmpty() ||
				Entry.SharedMaterialGroup.IsEmpty() || Entry.LastAuditDate.IsEmpty() ||
				Entry.AuditVersion.IsEmpty())
			{
				AddError(FString::Printf(TEXT("AllowedMetadataIncomplete:%s"), *Key.ToString()));
			}
			else if (!AvAuditEvidenceExists(Entry.AuditEvidence))
			{
				AddError(FString::Printf(
					TEXT("AllowedAuditEvidenceMissing:%s:%s"),
					*Key.ToString(),
					*Entry.AuditEvidence));
			}
			if (!Entry.QuarantineReason.IsEmpty())
			{
				AddError(FString::Printf(TEXT("AllowedHasQuarantineReason:%s"), *Key.ToString()));
			}
			if (!CandidatePaths.Contains(Key))
			{
				AddError(FString::Printf(TEXT("AllowedMissingFromScan:%s"), *Key.ToString()));
			}
			if (!bFinalValidation)
			{
				AddError(FString::Printf(
					TEXT("AllowedRejectedByFinalValidation:%s:%s"),
					*Key.ToString(), *ValidationDetails));
			}
			else
			{
				++AllowedFinalValidationPasses;
			}
		}
		else
		{
			++DeniedCount;
			DeniedPaths.Add(Key);
			if (CandidatePaths.Contains(Key))
			{
				AddError(FString::Printf(TEXT("DeniedPresentInScan:%s"), *Key.ToString()));
			}
			if (bFinalValidation)
			{
				AddError(FString::Printf(TEXT("DeniedAcceptedByFinalValidation:%s"), *Key.ToString()));
			}
			else
			{
				++DeniedFinalValidationRejects;
			}
		}
	}

	// Explicit glove-integrity matrix. The two known worker gloves are intentionally
	// open-fingered, so every Body/Torso/FullOutfit composition containing either exact
	// path must be rejected. Every currently allowed Torso/FullOutfit remains valid with
	// Gloves=None.
	TArray<const FAvRandomAssetEligibility*> DeniedGloves;
	TArray<const FAvRandomAssetEligibility*> AllowedTorsos;
	TArray<const FAvRandomAssetEligibility*> AllowedFullOutfits;
	for (const TPair<FSoftObjectPath, FAvRandomAssetEligibility>& Pair :
		AvGetRandomAssetEligibility())
	{
		const FAvRandomAssetEligibility& Entry = Pair.Value;
		const bool bAllowed = Entry.bAllowInRandom &&
			Entry.AuditStatus == EAvRandomAuditStatus::VerifiedIntact &&
			Entry.VisualCondition == EAvRandomVisualCondition::Intact &&
			!AvGetQuarantinedSharedGeometryGroups().Contains(Entry.SharedGeometryGroup);
		if (Entry.Slot == EWorkerSlot::Gloves && !bAllowed)
		{
			DeniedGloves.Add(&Entry);
		}
		else if (Entry.Slot == EWorkerSlot::Torso && bAllowed)
		{
			AllowedTorsos.Add(&Entry);
		}
		else if (Entry.Slot == EWorkerSlot::FullOutfit && bAllowed)
		{
			AllowedFullOutfits.Add(&Entry);
		}
	}

	for (const FAvRandomAssetEligibility* Glove : DeniedGloves)
	{
		FWorkerAppearance BodyBaseline = MakeBaseMaleUnderwearAppearance();
		BodyBaseline.Set(EWorkerSlot::Gloves, TSoftObjectPtr<USkeletalMesh>(Glove->AssetPath));
		FString BodyValidation;
		if (ValidateRandomGeneratedMaleAppearance(BodyBaseline, BodyValidation) ||
			!BodyValidation.Contains(TEXT("VisualEligibilityDenied")))
		{
			AddError(FString::Printf(TEXT("DeniedGloveBodyAccepted:%s:%s"),
				*Glove->AssetPath.ToString(), *BodyValidation));
		}
		else
		{
			++GloveBodyBaselineRejects;
		}

		for (const FAvRandomAssetEligibility* Torso : AllowedTorsos)
		{
			FWorkerAppearance PairAppearance = MakeBaseMaleUnderwearAppearance();
			PairAppearance.Set(EWorkerSlot::Torso, TSoftObjectPtr<USkeletalMesh>(Torso->AssetPath));
			PairAppearance.Set(EWorkerSlot::Legs, TSoftObjectPtr<USkeletalMesh>(SafeLower));
			PairAppearance.Set(EWorkerSlot::Gloves, TSoftObjectPtr<USkeletalMesh>(Glove->AssetPath));
			FString PairValidation;
			if (ValidateRandomGeneratedMaleAppearance(PairAppearance, PairValidation) ||
				!PairValidation.Contains(TEXT("VisualEligibilityDenied")))
			{
				AddError(FString::Printf(TEXT("DeniedGloveTorsoPairAccepted:%s:%s:%s"),
					*Glove->AssetPath.ToString(), *Torso->AssetPath.ToString(), *PairValidation));
			}
			else
			{
				++GloveTorsoPairRejects;
			}
		}

		for (const FAvRandomAssetEligibility* FullOutfit : AllowedFullOutfits)
		{
			FWorkerAppearance PairAppearance = MakeBaseMaleUnderwearAppearance();
			PairAppearance.Set(EWorkerSlot::FullOutfit,
				TSoftObjectPtr<USkeletalMesh>(FullOutfit->AssetPath));
			PairAppearance.Set(EWorkerSlot::Gloves, TSoftObjectPtr<USkeletalMesh>(Glove->AssetPath));
			FString PairValidation;
			if (ValidateRandomGeneratedMaleAppearance(PairAppearance, PairValidation) ||
				!PairValidation.Contains(TEXT("VisualEligibilityDenied")))
			{
				AddError(FString::Printf(TEXT("DeniedGloveFullOutfitPairAccepted:%s:%s:%s"),
					*Glove->AssetPath.ToString(), *FullOutfit->AssetPath.ToString(), *PairValidation));
			}
			else
			{
				++GloveFullOutfitPairRejects;
			}
		}
	}

	for (const FAvRandomAssetEligibility* Torso : AllowedTorsos)
	{
		FWorkerAppearance NoGloves = MakeBaseMaleUnderwearAppearance();
		NoGloves.Set(EWorkerSlot::Torso, TSoftObjectPtr<USkeletalMesh>(Torso->AssetPath));
		NoGloves.Set(EWorkerSlot::Legs, TSoftObjectPtr<USkeletalMesh>(SafeLower));
		FString Validation;
		if (!ValidateRandomGeneratedMaleAppearance(NoGloves, Validation))
		{
			AddError(FString::Printf(TEXT("GlovesNoneTorsoRejected:%s:%s"),
				*Torso->AssetPath.ToString(), *Validation));
		}
		else
		{
			++GlovesNoneValidationPasses;
		}
	}
	for (const FAvRandomAssetEligibility* FullOutfit : AllowedFullOutfits)
	{
		FWorkerAppearance NoGloves = MakeBaseMaleUnderwearAppearance();
		NoGloves.Set(EWorkerSlot::FullOutfit,
			TSoftObjectPtr<USkeletalMesh>(FullOutfit->AssetPath));
		FString Validation;
		if (!ValidateRandomGeneratedMaleAppearance(NoGloves, Validation))
		{
			AddError(FString::Printf(TEXT("GlovesNoneFullOutfitRejected:%s:%s"),
				*FullOutfit->AssetPath.ToString(), *Validation));
		}
		else
		{
			++GlovesNoneValidationPasses;
		}
	}

	FWorkerAppearance FallbackAppearance;
	const bool bFallbackBuilt = AvTryBuildSafeRandomFallback(Candidates, FallbackAppearance);
	FString FallbackValidation;
	const bool bFallbackValid = bFallbackBuilt &&
		ValidateRandomGeneratedMaleAppearance(FallbackAppearance, FallbackValidation);
	if (!bFallbackValid)
	{
		AddError(FString::Printf(
			TEXT("FallbackInvalid:%s"),
			FallbackValidation.IsEmpty() ? TEXT("NotBuilt") : *FallbackValidation));
	}
	for (const FSoftObjectPath& DeniedPath : DeniedPaths)
	{
		if (ContainsExactAsset(FallbackAppearance, DeniedPath))
		{
			AddError(FString::Printf(TEXT("DeniedPresentInFallback:%s"), *DeniedPath.ToString()));
		}
	}

	// A saved/legacy appearance is input only to duplicate avoidance. Prove every denied exact
	// asset cannot be copied into a newly generated result.
	for (const TPair<FSoftObjectPath, FAvRandomAssetEligibility>& Pair :
		AvGetRandomAssetEligibility())
	{
		const FAvRandomAssetEligibility& Entry = Pair.Value;
		if (!DeniedPaths.Contains(Entry.AssetPath))
		{
			continue;
		}
		const FWorkerAppearance LegacyAppearance = MakeManualAppearance(Entry);
		const FWorkerAppearance Generated = MakeRandomCompatibleMaleAppearance(LegacyAppearance);
		FString GeneratedValidation;
		if (ContainsExactAsset(Generated, Entry.AssetPath) ||
			!ValidateRandomGeneratedMaleAppearance(Generated, GeneratedValidation))
		{
			AddError(FString::Printf(
				TEXT("LegacyDeniedCarryover:%s:%s"),
				*Entry.AssetPath.ToString(), *GeneratedValidation));
		}
		else
		{
			++LegacyCarryoverRejects;
		}
	}

	for (int32 RandomIndex = 0; RandomIndex < 32; ++RandomIndex)
	{
		const FWorkerAppearance Generated = MakeRandomCompatibleMaleAppearance(FWorkerAppearance());
		FString Validation;
		if (!Generated.Get(EWorkerSlot::Gloves).IsNull() ||
			!ValidateRandomGeneratedMaleAppearance(Generated, Validation))
		{
			AddError(FString::Printf(TEXT("RandomGloveSafetyFailure:%d:%s:%s"),
				RandomIndex,
				*Generated.Get(EWorkerSlot::Gloves).ToSoftObjectPath().ToString(),
				*Validation));
		}
		else
		{
			++RandomGlovesNonePasses;
		}
	}

	OutReport = FString::Printf(
		TEXT("Result=%s Records=%d Allowed=%d Denied=%d ScanCandidates=%d AllowedFinalValidationPasses=%d DeniedFinalValidationRejects=%d LegacyCarryoverRejects=%d GloveBodyBaselineRejects=%d GloveTorsoPairRejects=%d GloveFullOutfitPairRejects=%d GlovesNoneValidationPasses=%d RandomGlovesNonePasses=%d Fallback=%s Errors={%s}"),
		Errors.IsEmpty() ? TEXT("PASS") : TEXT("FAIL"),
		AvGetRandomAssetEligibility().Num(),
		AllowedCount,
		DeniedCount,
		CandidatePaths.Num(),
		AllowedFinalValidationPasses,
		DeniedFinalValidationRejects,
		LegacyCarryoverRejects,
		GloveBodyBaselineRejects,
		GloveTorsoPairRejects,
		GloveFullOutfitPairRejects,
		GlovesNoneValidationPasses,
		RandomGlovesNonePasses,
		bFallbackValid ? TEXT("PASS") : TEXT("FAIL"),
		*FString::Join(Errors, TEXT(";")));
	UE_LOG(LogTemp, Warning,
		TEXT("[AvCustomizeRandomEligibilityExhaustiveTest] %s"), *OutReport);
	return Errors.IsEmpty();
}

FWorkerAppearance UWorkerAppearanceComponent::MakeRandomCompatibleMaleAppearance(
	const FWorkerAppearance& PreviousAppearance,
	TMap<EWorkerSlot, int32>* OutCandidateCounts,
	FAvRandomHeadSelectionDiagnostics* OutHeadDiagnostics)
{
	static uint64 CatalogBuildSerial = 0;
	const uint64 CurrentCatalogBuild = ++CatalogBuildSerial;
	TMap<EWorkerSlot, TArray<FAvRandomCandidate>> Candidates;
	TMap<EWorkerSlot, int32> StructurallyEligibleCounts;
	UE_LOG(LogTemp, Log,
		TEXT("[AvCustomizeRandomEligibilityCatalogBuild] Serial=%llu Source=AssetRegistry LocalRebuild=true CachedCandidates=false PreviousAppearanceUse=DuplicateCheckOnly"),
		CurrentCatalogBuild);
	for (const FAvRandomFamilyDefinition& Family : RandomFamilies)
	{
		AvScanRandomFolder(
			Family,
			Candidates.FindOrAdd(Family.Slot),
			StructurallyEligibleCounts.FindOrAdd(Family.Slot));
	}

	for (TPair<EWorkerSlot, TArray<FAvRandomCandidate>>& Pair : Candidates)
	{
		AvSortAndUnique(Pair.Value);
		if (OutCandidateCounts)
		{
			OutCandidateCounts->Add(Pair.Key, Pair.Value.Num());
		}
		const int32 Before = StructurallyEligibleCounts.FindRef(Pair.Key);
		UE_LOG(LogTemp, Log,
			TEXT("[AvCustomizeRandomEligibilityCatalog] Serial=%llu Slot=%d Before=%d Eligible=%d Rejected=%d DefaultPolicy=UnverifiedDenied RequiredAuditStatus=AssetVerifiedIntact"),
			CurrentCatalogBuild,
			static_cast<int32>(Pair.Key), Before, Pair.Value.Num(), Before - Pair.Value.Num());
	}

	FWorkerAppearance Result;
	int32 RejectedMissingFootwear = 0;
	int32 RejectedOther = 0;
	int32 CompatibilityCleanups = 0;
	bool bAccepted = false;
	for (int32 Attempt = 0; Attempt < RandomGenerationMaxAttempts; ++Attempt)
	{
		Result = MakeBaseMaleUnderwearAppearance();
		FAvRandomHeadSelectionDiagnostics AttemptHeadDiagnostics;
		const bool bHeadSelected = AvSelectCanonicalRandomHeadType(
			Result,
			&AttemptHeadDiagnostics);
		if (OutHeadDiagnostics)
		{
			*OutHeadDiagnostics = AttemptHeadDiagnostics;
		}
		// The curated Random policy has not adopted the Full Outfits page. Keep both
		// ensemble layers empty and generate the established modular composition only.
		Result.Clear(EWorkerSlot::FullOutfit);
		Result.Clear(EWorkerSlot::Overalls);
		AvChooseRequiredSlot(Result, EWorkerSlot::Torso,
			Candidates.FindChecked(EWorkerSlot::Torso));
		AvChooseRequiredSlot(Result, EWorkerSlot::Legs,
			Candidates.FindChecked(EWorkerSlot::Legs));
		AvChooseOptionalSlot(Result, EWorkerSlot::Vest,
			Candidates.FindChecked(EWorkerSlot::Vest));
		AvChooseOptionalSlot(Result, EWorkerSlot::Headgear, Candidates.FindChecked(EWorkerSlot::Headgear));
		const FAvRandomFamilyDefinition* UpperFamily = AvFindRandomFamily(
			EWorkerSlot::Torso,
			Result.Get(EWorkerSlot::Torso).ToSoftObjectPath());
		if (Result.Get(EWorkerSlot::Headgear).IsNull() &&
			(!UpperFamily || !UpperFamily->bHasIntegratedHood))
		{
			AvChooseOptionalSlot(Result, EWorkerSlot::Hair, Candidates.FindChecked(EWorkerSlot::Hair));
		}
		AvChooseOptionalSlot(Result, EWorkerSlot::Gloves, Candidates.FindChecked(EWorkerSlot::Gloves));
		AvChooseOptionalSlot(Result, EWorkerSlot::FaceMask, Candidates.FindChecked(EWorkerSlot::FaceMask));
		if (Result.Get(EWorkerSlot::FaceMask).IsNull())
		{
			AvChooseOptionalSlot(Result, EWorkerSlot::Beard, Candidates.FindChecked(EWorkerSlot::Beard));
			AvChooseOptionalSlot(Result, EWorkerSlot::Glasses, Candidates.FindChecked(EWorkerSlot::Glasses));
		}
		CompatibilityCleanups += AvApplyRandomCompatibilityCleanup(Result);

		FString ValidationDetails;
		const bool bValid = bHeadSelected &&
			ValidateRandomGeneratedMaleAppearance(Result, ValidationDetails);
		if (!bHeadSelected)
		{
			ValidationDetails = AttemptHeadDiagnostics.ValidationResult;
		}
		if (bValid &&
			!AvHasSameSlotMeshes(Result, PreviousAppearance))
		{
			UE_LOG(LogTemp, Log,
				TEXT("[AvCustomizeRandomValidation] Result=PASS Attempts=%d Rejected=%d MissingFootwear=%d Other=%d Cleanup=%d Fallback=false %s"),
				Attempt + 1,
				RejectedMissingFootwear + RejectedOther,
				RejectedMissingFootwear,
				RejectedOther,
				CompatibilityCleanups,
				*ValidationDetails);
			bAccepted = true;
			break;
		}

		if (!bValid && ValidationDetails.Contains(TEXT("MissingFootwear")))
		{
			++RejectedMissingFootwear;
		}
		else
		{
			++RejectedOther;
		}
		UE_LOG(LogTemp, Log,
			TEXT("[AvCustomizeRandomValidation] Result=REJECT Attempt=%d %s"),
			Attempt + 1,
			bValid ? TEXT("Reason=DuplicatePrevious") : *ValidationDetails);
	}

	FString FinalValidation;
	if (!bAccepted)
	{
		const bool bBuiltFallback = AvTryBuildSafeRandomFallback(Candidates, Result);
		FAvRandomHeadSelectionDiagnostics FallbackHeadDiagnostics;
		const bool bFallbackHeadSelected = bBuiltFallback &&
			AvSelectCanonicalRandomHeadType(Result, &FallbackHeadDiagnostics);
		if (OutHeadDiagnostics)
		{
			*OutHeadDiagnostics = FallbackHeadDiagnostics;
		}
		const bool bFallbackValid = bFallbackHeadSelected &&
			ValidateRandomGeneratedMaleAppearance(Result, FinalValidation);
		if (bFallbackValid)
		{
			UE_LOG(LogTemp, Log,
				TEXT("[AvCustomizeRandomValidation] Result=PASS Attempts=%d Rejected=%d MissingFootwear=%d Other=%d Cleanup=%d Fallback=true %s"),
				RandomGenerationMaxAttempts,
				RejectedMissingFootwear + RejectedOther,
				RejectedMissingFootwear,
				RejectedOther,
				CompatibilityCleanups,
				*FinalValidation);
		}
		else
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AvCustomizeRandomValidation] Result=FAIL Attempts=%d Rejected=%d MissingFootwear=%d Other=%d Cleanup=%d Fallback=true %s"),
				RandomGenerationMaxAttempts,
				RejectedMissingFootwear + RejectedOther,
				RejectedMissingFootwear,
				RejectedOther,
				CompatibilityCleanups,
				*FinalValidation);
		}
	}
	Result.Clear(EWorkerSlot::FullOutfit);
	Result.Clear(EWorkerSlot::Overalls);
	return Result;
}

TArray<FString> UWorkerAppearanceComponent::GetOptionsForSlot(EWorkerSlot Slot) const
{
	TArray<FAssetData> Assets;
	AvScanSlot(Slot, Assets);
	TArray<FString> Out;
	for (const FAssetData& A : Assets) { Out.Add(A.AssetName.ToString()); }
	Out.Sort();
	return Out;
}

bool UWorkerAppearanceComponent::SetSlotByKey(EWorkerSlot Slot, const FString& Key)
{
	if (!IsAuth(this)) return false;
	if (Key.IsEmpty() || Key.Equals(TEXT("none"), ESearchCase::IgnoreCase) || Key.Equals(TEXT("off"), ESearchCase::IgnoreCase))
	{
		ClearSlot(Slot);
		return true;
	}
	TArray<FAssetData> Assets;
	AvScanSlot(Slot, Assets);
	// Сначала точное совпадение имени, потом «содержит».
	const FAssetData* Best = nullptr;
	for (const FAssetData& A : Assets)
	{
		if (A.AssetName.ToString().Equals(Key, ESearchCase::IgnoreCase)) { Best = &A; break; }
	}
	if (!Best)
	{
		for (const FAssetData& A : Assets)
		{
			if (A.AssetName.ToString().Contains(Key, ESearchCase::IgnoreCase)) { Best = &A; break; }
		}
	}
	if (!Best) return false;
	if (USkeletalMesh* M = Cast<USkeletalMesh>(Best->GetAsset()))
	{
		SetSlotMesh(Slot, M);
		return true;
	}
	return false;
}

void UWorkerAppearanceComponent::ApplyColorToComp(USkeletalMeshComponent* Comp, const FLinearColor& Color) const
{
	if (!Comp) return;
	const int32 Num = Comp->GetNumMaterials();
	for (int32 i = 0; i < Num; ++i)
	{
		if (UMaterialInstanceDynamic* MID = Comp->CreateAndSetMaterialInstanceDynamic(i))
		{
			// Материалы пакета Modular_Workers имеют вектор-параметр "Color Correction" (тинт).
			MID->SetVectorParameterValue(TEXT("Color Correction"), Color);
		}
	}
}

void UWorkerAppearanceComponent::ApplyResolvedBodyCoverage(USkeletalMeshComponent* Body)
{
	ResolvedBodyCoverageMask = 0;
	ResolvedTorsoCoverageMask = 0;
	ResolvedLegsCoverageMask = 0;
	ResolvedFeetCoverageMask = 0;
	ResolvedGlovesCoverageMask = 0;
	ResolvedEnsembleCoverageMask = 0;
	ResolvedFallbackTorsoCoverageMask = 0;
	ResolvedTorsoStableId = TEXT("None");
	ResolvedLegsStableId = TEXT("None");
	ResolvedFeetStableId = TEXT("None");
	ResolvedGlovesStableId = TEXT("None");
	ResolvedFullOutfitStableId = TEXT("None");
	ResolvedOverallsStableId = TEXT("None");
	ResolvedEnsembleDemoIndex = -1;
	ResolvedEnsemblePrimaryMeshPath.Reset();
	ResolvedEnsembleCompanionTorsoPath.Reset();
	ResolvedEnsembleCompanionChestPath.Reset();
	ResolvedEnsembleCompanionLegsPath.Reset();
	ResolvedEnsembleCompanionFeetPath.Reset();
	bResolvedOriginalDemoRecipeMatched = false;
	ResolvedEnsembleTechnicalKind = TEXT("None");
	ResolvedFallbackTorsoPath.Reset();
	bResolvedEnsembleRecordExists = true;
	ResolvedSuppressedRenderSlots.Reset();
	ResolvedBodyCoverageImplementation = TEXT("None");
	ResolvedBodyVariantPath.Reset();
	ResolvedHiddenSections.Reset();
	UnknownCoverageRecords.Reset();
	const TArray<FAvTorsoCatalogItem> TorsoCoverageCatalog = GetTorsoCatalog();
	const TArray<FAvLowerHipCatalogItem> LowerCoverageCatalog = GetLowerHipCatalog();
	const TArray<FAvHandsAccessoriesCatalogItem> HandsCoverageCatalog =
		GetHandsAccessoriesCatalog();

	auto ResolveTorso = [this, &TorsoCoverageCatalog]()
	{
		const FSoftObjectPath ActivePath = Appearance.Get(EWorkerSlot::Torso).ToSoftObjectPath();
		if (!ActivePath.IsValid())
		{
			return;
		}
		const FString ExactPath = ActivePath.ToString();
		const FAvTorsoCatalogItem* Match = TorsoCoverageCatalog.FindByPredicate(
			[&ExactPath](const FAvTorsoCatalogItem& Item)
			{
				return !Item.bIsNone && Item.ExactMeshPath == ExactPath;
			});
		if (!Match)
		{
			ResolvedTorsoStableId = TEXT("Unknown");
			UnknownCoverageRecords.Add(FString::Printf(TEXT("Torso=%s"), *ExactPath));
			return;
		}
		ResolvedTorsoStableId = Match->StableId;
		ResolvedTorsoCoverageMask = static_cast<uint16>(Match->BodyCoverageMask);
	};
	auto ResolveLegs = [this, &LowerCoverageCatalog]()
	{
		const FSoftObjectPath ActivePath = Appearance.Get(EWorkerSlot::Legs).ToSoftObjectPath();
		if (!ActivePath.IsValid())
		{
			return;
		}
		const FString ExactPath = ActivePath.ToString();
		const FAvLowerHipCatalogItem* Match = LowerCoverageCatalog.FindByPredicate(
			[&ExactPath](const FAvLowerHipCatalogItem& Item)
			{
				return Item.Section == EAvLowerHipSection::Legs &&
					!Item.bIsNone &&
					Item.ExactMeshPath == ExactPath;
			});
		if (!Match)
		{
			ResolvedLegsStableId = TEXT("Unknown");
			UnknownCoverageRecords.Add(FString::Printf(TEXT("Legs=%s"), *ExactPath));
			return;
		}
		ResolvedLegsStableId = Match->StableId;
		ResolvedLegsCoverageMask = static_cast<uint16>(Match->BodyCoverageMask);
	};
	auto ResolveGloves = [this, &HandsCoverageCatalog]()
	{
		const FSoftObjectPath ActivePath = Appearance.Get(EWorkerSlot::Gloves).ToSoftObjectPath();
		if (!ActivePath.IsValid())
		{
			return;
		}
		const FString ExactPath = ActivePath.ToString();
		const FAvHandsAccessoriesCatalogItem* Match = HandsCoverageCatalog.FindByPredicate(
			[&ExactPath](const FAvHandsAccessoriesCatalogItem& Item)
			{
				return Item.Section == EAvHandsAccessoriesSection::Gloves &&
					!Item.bIsNone &&
					Item.ExactMeshPath == ExactPath;
			});
		if (!Match)
		{
			ResolvedGlovesStableId = TEXT("Unknown");
			UnknownCoverageRecords.Add(FString::Printf(TEXT("Gloves=%s"), *ExactPath));
			return;
		}
		ResolvedGlovesStableId = Match->StableId;
		ResolvedGlovesCoverageMask = static_cast<uint16>(Match->BodyCoverageMask);
	};

	ResolveTorso();
	ResolveLegs();
	ResolveGloves();

	const FSoftObjectPath FullOutfitPath = Appearance.Get(EWorkerSlot::FullOutfit).ToSoftObjectPath();
	const FSoftObjectPath OverallsPath = Appearance.Get(EWorkerSlot::Overalls).ToSoftObjectPath();
	const bool bHasFullOutfit = FullOutfitPath.IsValid();
	const bool bHasOveralls = OverallsPath.IsValid();
	const EWorkerSlot ActiveEnsembleSlot = bHasFullOutfit
		? EWorkerSlot::FullOutfit
		: (bHasOveralls ? EWorkerSlot::Overalls : EWorkerSlot::FullOutfit);
	const FSoftObjectPath ActiveEnsemblePath = bHasFullOutfit ? FullOutfitPath : OverallsPath;
	if (bHasFullOutfit && bHasOveralls)
	{
		// Old/test saves are not mutated on load. FullOutfit has deterministic visual precedence.
		ResolvedSuppressedRenderSlots.Add(EWorkerSlot::Overalls);
		UnknownCoverageRecords.Add(FString::Printf(
			TEXT("EnsembleConflict=FullOutfitPrecedence;Overalls=%s"), *OverallsPath.ToString()));
	}
	if (ActiveEnsemblePath.IsValid())
	{
		const FString ExactPath = ActiveEnsemblePath.ToString();
		FAvFullOutfitCatalogItem ResolvedRecord;
		const FAvFullOutfitCatalogItem* Match = ResolveFullOutfitCatalogRecord(
			ActiveEnsembleSlot, ActiveEnsemblePath, ResolvedRecord)
			? &ResolvedRecord
			: nullptr;
		if (Match)
		{
			ResolvedEnsembleDemoIndex = Match->DemoIndex;
			ResolvedEnsemblePrimaryMeshPath = Match->PrimaryMeshPath;
			ResolvedEnsembleCompanionTorsoPath = Match->CompanionTorsoMeshPath;
			ResolvedEnsembleCompanionChestPath = Match->CompanionChestMeshPath;
			ResolvedEnsembleCompanionLegsPath = Match->CompanionLegsMeshPath;
			ResolvedEnsembleCompanionFeetPath = Match->CompanionFeetMeshPath;
			bResolvedOriginalDemoRecipeMatched = Match->DemoIndex >= 0;
			ResolvedEnsembleCoverageMask = static_cast<uint16>(Match->BodyCoverageMask);
			ResolvedEnsembleTechnicalKind = Match->TechnicalKind == EAvEnsembleTechnicalKind::FullOutfit
				? TEXT("FullOutfit") : TEXT("Overalls");
			if (Match->TechnicalKind == EAvEnsembleTechnicalKind::FullOutfit)
			{
				ResolvedFullOutfitStableId = Match->StableId;
			}
			else
			{
				ResolvedOverallsStableId = Match->StableId;
			}
			for (const EWorkerSlot SuppressedSlot : Match->SuppressedRenderSlots)
			{
				ResolvedSuppressedRenderSlots.Add(SuppressedSlot);
			}
			if (Match->TechnicalKind == EAvEnsembleTechnicalKind::Overalls &&
				Appearance.Get(EWorkerSlot::Torso).IsNull() &&
				!Match->FallbackTorsoMeshPath.IsEmpty())
			{
				ResolvedFallbackTorsoPath = Match->FallbackTorsoMeshPath;
				ResolvedFallbackTorsoCoverageMask =
					static_cast<uint16>(Match->FallbackTorsoCoverageMask);
			}
		}
		else
		{
			bResolvedEnsembleRecordExists = false;
			ResolvedEnsembleTechnicalKind = ActiveEnsembleSlot == EWorkerSlot::FullOutfit
				? TEXT("UnknownFullOutfit") : TEXT("UnknownOveralls");
			if (ActiveEnsembleSlot == EWorkerSlot::FullOutfit)
			{
				ResolvedFullOutfitStableId = TEXT("Unknown");
			}
			else
			{
				ResolvedOverallsStableId = TEXT("Unknown");
			}
			UnknownCoverageRecords.Add(FString::Printf(
				TEXT("%s=%s"),
				ActiveEnsembleSlot == EWorkerSlot::FullOutfit ? TEXT("FullOutfit") : TEXT("Overalls"),
				*ExactPath));
		}
	}

	const FSoftObjectPath FeetPath = Appearance.Get(EWorkerSlot::Feet).ToSoftObjectPath();
	if (FeetPath.IsValid())
	{
		// No footwear is currently curated. Unknown records deliberately receive coverage=None.
		ResolvedFeetStableId = TEXT("Unknown");
		UnknownCoverageRecords.Add(FString::Printf(TEXT("Feet=%s"), *FeetPath.ToString()));
	}
	ResolvedBodyCoverageMask =
		(ResolvedSuppressedRenderSlots.Contains(EWorkerSlot::Torso) ? 0 : ResolvedTorsoCoverageMask) |
		(ResolvedSuppressedRenderSlots.Contains(EWorkerSlot::Legs) ? 0 : ResolvedLegsCoverageMask) |
		(ResolvedSuppressedRenderSlots.Contains(EWorkerSlot::Feet) ? 0 : ResolvedFeetCoverageMask) |
		(ResolvedSuppressedRenderSlots.Contains(EWorkerSlot::Gloves) ? 0 : ResolvedGlovesCoverageMask) |
		ResolvedFallbackTorsoCoverageMask |
		ResolvedEnsembleCoverageMask;

	const uint16 VisibleTorsoCoverageMask =
		ResolvedSuppressedRenderSlots.Contains(EWorkerSlot::Torso) ? 0 : ResolvedTorsoCoverageMask;
	const uint16 VisibleLegsCoverageMask =
		ResolvedSuppressedRenderSlots.Contains(EWorkerSlot::Legs) ? 0 : ResolvedLegsCoverageMask;
	const uint16 VisibleGlovesCoverageMask =
		ResolvedSuppressedRenderSlots.Contains(EWorkerSlot::Gloves) ? 0 : ResolvedGlovesCoverageMask;
	const uint16 EffectiveUpperCoverageMask =
		VisibleTorsoCoverageMask |
		ResolvedFallbackTorsoCoverageMask |
		ResolvedEnsembleCoverageMask;
	const uint16 EffectiveLowerCoverageMask = VisibleLegsCoverageMask | ResolvedEnsembleCoverageMask;
	const bool bTorsoCovered = AvHasBodyZone(EffectiveUpperCoverageMask, EWorkerBodyZone::Torso);
	const bool bLowerCovered =
		AvHasBodyZone(EffectiveLowerCoverageMask, EWorkerBodyZone::Pelvis) ||
		AvHasBodyZone(EffectiveLowerCoverageMask, EWorkerBodyZone::UpperLegs) ||
		AvHasBodyZone(EffectiveLowerCoverageMask, EWorkerBodyZone::LowerLegs);
	const bool bHideBodyRegion = bTorsoCovered || bLowerCovered;
	// Hands-only accessories cannot drive the coarse FaceRig Arms section off. The package
	// glove meshes are authored over the arm/body layer; hiding that entire section removed
	// shoulder caps that SKM_Quantum_Hands does not contain.
	const bool bHideArmsRegion =
		AvHasBodyZone(EffectiveUpperCoverageMask, EWorkerBodyZone::UpperArms) ||
		AvHasBodyZone(EffectiveUpperCoverageMask, EWorkerBodyZone::Forearms);

	if (Body)
	{
		const USkeletalMesh* BodyMesh = Body->GetSkeletalMeshAsset();
		const FSkeletalMeshRenderData* RenderData = BodyMesh ? BodyMesh->GetResourceForRendering() : nullptr;
		if (BodyMesh && BodyMesh->GetPathName() == AvWorkerAssets::FactoryBody && RenderData)
		{
			for (int32 LodIndex = 0; LodIndex < RenderData->LODRenderData.Num(); ++LodIndex)
			{
				Body->ShowAllMaterialSections(LodIndex);
				const FSkeletalMeshLODRenderData& LodData = RenderData->LODRenderData[LodIndex];
				for (int32 SectionIndex = 0; SectionIndex < LodData.RenderSections.Num(); ++SectionIndex)
				{
					const int32 MaterialIndex = LodData.RenderSections[SectionIndex].MaterialIndex;
					if (!BodyMesh->GetMaterials().IsValidIndex(MaterialIndex))
					{
						continue;
					}
					const FSkeletalMaterial& Material = BodyMesh->GetMaterials()[MaterialIndex];
					const FName MaterialName = Material.MaterialInterface
						? Material.MaterialInterface->GetFName()
						: NAME_None;
					const EAvSkinMaterialRegion Region = AvGetSupportedSkinMaterialRegion(
						Material.MaterialSlotName, MaterialName);
					const bool bHide =
						(Region == EAvSkinMaterialRegion::Body && bHideBodyRegion) ||
						(Region == EAvSkinMaterialRegion::Arms && bHideArmsRegion);
					if (bHide)
					{
						Body->ShowMaterialSection(MaterialIndex, SectionIndex, false, LodIndex);
						ResolvedHiddenSections.Add(FString::Printf(
							TEXT("LOD%d:Section%d:Material%d"),
							LodIndex, SectionIndex, MaterialIndex));
					}
				}
			}
		}
		else if (ResolvedBodyCoverageMask != 0)
		{
			UnknownCoverageRecords.Add(TEXT("Body=UnsupportedSectionLayout"));
		}
	}

	TSet<FName> PresentRoles;
	TArray<FString> ActiveModules;
	auto ApplyModule = [this, Body, &PresentRoles, &ActiveModules](
		FName Role, const TCHAR* ObjectPath)
	{
		if (!Body)
		{
			return;
		}
		USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(
			nullptr, ObjectPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
		if (!Mesh)
		{
			UnknownCoverageRecords.Add(FString::Printf(
				TEXT("MissingPackageModule=%s"), ObjectPath));
			return;
		}
		USkeletalMeshComponent* Module = GetOrCreateDerivedBodyComp(Role);
		if (!Module)
		{
			UnknownCoverageRecords.Add(FString::Printf(
				TEXT("FailedPackageModuleComponent=%s"), *Role.ToString()));
			return;
		}
		Module->SetLeaderPoseComponent(nullptr);
		Module->EmptyOverrideMaterials();
		Module->SetSkeletalMeshAsset(Mesh);
		Module->EmptyOverrideMaterials();
		AttachAsLeaderFollower(Module, Body);
		Module->SetVisibility(false, false);
		RefreshVisualComponent(Module);
		PresentRoles.Add(Role);
		ActiveModules.Add(Mesh->GetPathName());
	};
	if (!ResolvedFallbackTorsoPath.IsEmpty())
	{
		ApplyModule(TEXT("OverallsFallbackTorso"), *ResolvedFallbackTorsoPath);
	}

	if (bHideBodyRegion)
	{
		if (bTorsoCovered && !bLowerCovered)
		{
			ApplyModule(TEXT("BodyBottom"), AvWorkerAssets::BodyBottom);
		}
		else if (!bTorsoCovered && bLowerCovered)
		{
			ApplyModule(TEXT("BareTorso"), AvWorkerAssets::BareTorso);
		}
	}
	if (bHideArmsRegion)
	{
		if (!AvHasBodyZone(EffectiveUpperCoverageMask, EWorkerBodyZone::Forearms))
		{
			ApplyModule(TEXT("BareArms"), AvWorkerAssets::BareHands);
		}
		if (!AvHasBodyZone(VisibleGlovesCoverageMask, EWorkerBodyZone::Hands))
		{
			ApplyModule(TEXT("BareWrist"), AvWorkerAssets::BareWrist);
		}
	}
	RemoveUnusedDerivedBodyComps(PresentRoles);

	if (!ResolvedHiddenSections.IsEmpty() && !ActiveModules.IsEmpty())
	{
		ResolvedBodyCoverageImplementation = TEXT("HiddenSections+PackageBodyModules");
	}
	else if (!ResolvedHiddenSections.IsEmpty())
	{
		ResolvedBodyCoverageImplementation = TEXT("HiddenSections");
	}
	else if (!ActiveModules.IsEmpty())
	{
		ResolvedBodyCoverageImplementation = TEXT("PackageBodyModules");
	}
	ResolvedBodyVariantPath = ActiveModules.IsEmpty()
		? TEXT("None")
		: FString::Join(ActiveModules, TEXT(";"));
}

void UWorkerAppearanceComponent::ApplyHeadTypeSkinProfile()
{
	ResolvedSkinProfile = TEXT("None");
	static const FName ColorCorrectionName(TEXT("Color Correction"));
	static const FName ColorCorrectionValueName(TEXT("Color Correction Value"));
	static const FName BrightnessName(TEXT("Brightness"));
	static const FName SaturationName(TEXT("Saturation"));
	static const FName ContrastName(TEXT("Contrast"));
	static const FName BaseColorName(TEXT("BaseColor"));
	static const FName NormalName(TEXT("Normal"));
	static const FName OrmName(TEXT("ORM"));

	const FAvHeadTypePresentation* Presentation =
		AvFindHeadTypePresentation(Appearance.Get(EWorkerSlot::Head).ToSoftObjectPath());
	const FAvHeadTypeSkinMaterialProfile* Profile = Presentation
		? AvFindHeadTypeSkinMaterialProfile(Presentation->SkinProfileId)
		: nullptr;
	if (!Presentation || !Profile)
	{
		return;
	}
	ResolvedSkinProfile = Profile->SkinProfileId.ToString();

	UMaterialInterface* HeadMaterial = LoadObject<UMaterialInterface>(
		nullptr, Profile->HeadMaterialPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
	UMaterialInterface* HeadAlphaMaterial = LoadObject<UMaterialInterface>(
		nullptr, Profile->HeadAlphaMaterialPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
	UMaterialInterface* ArmsMaterial = LoadObject<UMaterialInterface>(
		nullptr, Profile->ArmsMaterialPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
	UMaterialInterface* MaskedBodyMaterial = LoadObject<UMaterialInterface>(
		nullptr, Profile->MaskedBodyMaterialPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
	UTexture2D* BodyBaseColor = LoadObject<UTexture2D>(
		nullptr, Profile->BodyBaseColorPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
	UTexture2D* BodyNormal = LoadObject<UTexture2D>(
		nullptr, Profile->BodyNormalPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
	UTexture2D* BodyOrm = LoadObject<UTexture2D>(
		nullptr, Profile->BodyOrmPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
	if (!HeadMaterial || !HeadAlphaMaterial || !ArmsMaterial || !MaskedBodyMaterial ||
		!BodyBaseColor || !BodyNormal || !BodyOrm)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvHeadTypeSkinProfile] Missing profile asset(s). HeadType=%s Profile=%s"),
			*Presentation->HeadTypeId.ToString(),
			*Profile->SkinProfileId.ToString());
		return;
	}

	const FLinearColor CorrectionTarget =
		AvMakeReferenceSkinCorrectionTarget(Presentation->SkinColor);
	const float CorrectionStrength =
		AvGetReferenceSkinColorCorrectionStrength(Presentation->SkinColor);
	const float Brightness = AvGetReferenceSkinBrightness(Presentation->SkinColor);
	const float Saturation = AvGetReferenceSkinSaturation(Presentation->SkinColor);
	const float Contrast = AvGetReferenceSkinContrast(Presentation->SkinColor);

	TArray<USkeletalMeshComponent*> VisualComponents;
	for (const TPair<EWorkerSlot, TObjectPtr<USkeletalMeshComponent>>& Pair : SlotComps)
	{
		if (Pair.Value)
		{
			VisualComponents.AddUnique(Pair.Value);
		}
	}
	for (const TPair<FName, TObjectPtr<USkeletalMeshComponent>>& Pair : DerivedBodyComps)
	{
		if (Pair.Value)
		{
			VisualComponents.AddUnique(Pair.Value);
		}
	}

	for (USkeletalMeshComponent* Comp : VisualComponents)
	{
		const USkeletalMesh* Mesh = Comp ? Comp->GetSkeletalMeshAsset() : nullptr;
		if (!Comp || !Mesh)
		{
			continue;
		}

		const FString MeshPath = Mesh->GetPathName();
		const bool bSupportedBodyMesh =
			MeshPath == AvWorkerAssets::FactoryBody ||
			MeshPath == AvWorkerAssets::BodyBottom ||
			MeshPath == AvWorkerAssets::BareTorso ||
			MeshPath == AvWorkerAssets::UnderPants ||
			MeshPath == AvWorkerAssets::Feet;
		bool bChangedMaterial = false;
		const TArray<FSkeletalMaterial>& MeshMaterials = Mesh->GetMaterials();
		for (int32 Index = 0; Index < MeshMaterials.Num(); ++Index)
		{
			const FSkeletalMaterial& MeshMaterial = MeshMaterials[Index];
			const FName MaterialName = MeshMaterial.MaterialInterface
				? MeshMaterial.MaterialInterface->GetFName()
				: NAME_None;
			const EAvSkinMaterialRegion MaterialRegion = AvGetSupportedSkinMaterialRegion(
				MeshMaterial.MaterialSlotName,
				MaterialName);
			if (MaterialRegion == EAvSkinMaterialRegion::None ||
				(MaterialRegion == EAvSkinMaterialRegion::Body && !bSupportedBodyMesh))
			{
				continue;
			}

			UMaterialInterface* ProfileMaterial = nullptr;
			switch (MaterialRegion)
			{
			case EAvSkinMaterialRegion::Head:
				ProfileMaterial = MaterialName.ToString().Contains(TEXT("Alpha"))
					? HeadAlphaMaterial
					: HeadMaterial;
				break;
			case EAvSkinMaterialRegion::Arms:
				ProfileMaterial = ArmsMaterial;
				break;
			case EAvSkinMaterialRegion::Body:
				ProfileMaterial = MaskedBodyMaterial;
				break;
			default:
				break;
			}
			if (!ProfileMaterial)
			{
				continue;
			}

			// Always rebuild the MID from the profile source. This prevents correction values from
			// stacking when an appearance is loaded, previewed, or switched repeatedly.
			Comp->SetMaterial(Index, ProfileMaterial);
			UMaterialInstanceDynamic* MID = Comp->CreateAndSetMaterialInstanceDynamic(Index);
			if (!MID)
			{
				continue;
			}
			if (MaterialRegion == EAvSkinMaterialRegion::Body)
			{
				MID->SetTextureParameterValue(BaseColorName, BodyBaseColor);
				MID->SetTextureParameterValue(NormalName, BodyNormal);
				MID->SetTextureParameterValue(OrmName, BodyOrm);
			}
			MID->SetVectorParameterValue(ColorCorrectionName, CorrectionTarget);
			MID->SetScalarParameterValue(ColorCorrectionValueName, CorrectionStrength);
			MID->SetScalarParameterValue(BrightnessName, Brightness);
			MID->SetScalarParameterValue(SaturationName, Saturation);
			MID->SetScalarParameterValue(ContrastName, Contrast);
			bChangedMaterial = true;
		}

		if (bChangedMaterial)
		{
			RefreshVisualComponent(Comp);
		}
	}
}

void UWorkerAppearanceComponent::SetSlotColor(EWorkerSlot Slot, FLinearColor Color)
{
	SlotColors.Add(Slot, Color);
	USkeletalMeshComponent* Comp = nullptr;
	if (TObjectPtr<USkeletalMeshComponent>* Found = SlotComps.Find(Slot))
	{
		Comp = *Found;
	}
	ApplyColorToComp(Comp, Color);
}

void UWorkerAppearanceComponent::SetSkinColor(FLinearColor Color)
{
	Appearance.SetSkinColor(Color);
	// Persisted SkinColor remains schema-compatible, but mapped heads always own the profile.
	NormalizeHeadTypeSkinPresentation(Appearance);
	ApplyHeadTypeSkinProfile();
}

void UWorkerAppearanceComponent::ClearSlotColor(EWorkerSlot Slot)
{
	SlotColors.Remove(Slot);
	RebuildVisuals(); // пересборка вернёт исходные материалы меша
}

void UWorkerAppearanceComponent::RefreshVisualComponent(USkeletalMeshComponent* Comp) const
{
	if (!Comp)
	{
		return;
	}

	Comp->UpdateComponentToWorld();
	Comp->RefreshBoneTransforms();
	Comp->UpdateBounds();
	Comp->MarkRenderDynamicDataDirty();
	Comp->MarkRenderTransformDirty();
	Comp->MarkRenderStateDirty();
}

void UWorkerAppearanceComponent::RebuildVisuals()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// На выделенном сервере визуал не нужен — внешность всё равно реплицирована.
	if (GetNetMode() == NM_DedicatedServer) return;

	USceneComponent* Root = Owner->GetRootComponent();
	if (!Root) return;

	// Keep the synchronous rebuild atomic from the renderer's perspective. No Tick/timer is used.
	for (const TPair<EWorkerSlot, TObjectPtr<USkeletalMeshComponent>>& Pair : SlotComps)
	{
		if (Pair.Value)
		{
			Pair.Value->SetVisibility(false, false);
		}
	}
	for (const TPair<FName, TObjectPtr<USkeletalMeshComponent>>& Pair : DerivedBodyComps)
	{
		if (Pair.Value)
		{
			Pair.Value->SetVisibility(false, false);
		}
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(Owner);
	const AAvaryoCharacter* AvaryoOwner = Cast<AAvaryoCharacter>(Owner);
	USkeletalMeshComponent* SourceBody = OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr;
	const bool bHasModularBody = !Appearance.Get(EWorkerSlot::Body).IsNull();
	const bool bLocalOwnerShouldHideAppearance = AvaryoOwner && AvaryoOwner->IsLocallyControlled() && !AvaryoOwner->IsThirdPerson();
	if (SourceBody)
	{
		SourceBody->SetVisibility(!bHasModularBody, true);
	}

	// 1) Тело — лидер позы. Создаём/обновляем первым.
	USkeletalMeshComponent* Body = nullptr;
	TSoftObjectPtr<USkeletalMesh> BodySoft = Appearance.Get(EWorkerSlot::Body);
	if (!BodySoft.IsNull())
	{
		if (USkeletalMesh* M = BodySoft.LoadSynchronous())
		{
			Body = GetOrCreateSlotComp(EWorkerSlot::Body);
			if (Body && Body->GetSkeletalMeshAsset() && Body->GetSkeletalMeshAsset() != M)
			{
				Body = RecreateSlotComp(EWorkerSlot::Body);
			}
			if (Body)
			{
				Body->SetLeaderPoseComponent(nullptr);
				Body->EmptyOverrideMaterials();
				Body->SetSkeletalMeshAsset(M);
				Body->EmptyOverrideMaterials();
				if (const FLinearColor* C = SlotColors.Find(EWorkerSlot::Body)) { ApplyColorToComp(Body, *C); }
			}
		}
		if (Body)
		{
			if (SourceBody)
			{
				Body->AttachToComponent(SourceBody->GetAttachParent() ? SourceBody->GetAttachParent() : Root, FAttachmentTransformRules::KeepRelativeTransform);
				Body->SetRelativeTransform(SourceBody->GetRelativeTransform());
				Body->SetLeaderPoseComponent(SourceBody);
				Body->SetOwnerNoSee(bLocalOwnerShouldHideAppearance);
				Body->SetOnlyOwnerSee(false);
			}
			else
			{
				Body->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				Body->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator(0.f, -90.f, 0.f));
				Body->SetOwnerNoSee(false);
				Body->SetOnlyOwnerSee(false);
			}
			Body->SetVisibility(false, false);
			Body->SetHiddenInGame(false, true);
			Body->Activate(true);
			RefreshVisualComponent(Body);
		}
	}

	// 2) Остальные слоты — следуют позе тела.
	TSet<EWorkerSlot> Present;
	if (Body)
	{
		Present.Add(EWorkerSlot::Body);
	}
	for (const FWorkerSlotMesh& S : Appearance.Slots)
	{
		if (S.Slot == EWorkerSlot::Body) continue;
		Present.Add(S.Slot);
		USkeletalMeshComponent* Part = GetOrCreateSlotComp(S.Slot);
		if (!Part) continue;
		TSoftObjectPtr<USkeletalMesh> MeshToLoad = S.Mesh;
		if (S.Slot == EWorkerSlot::FullOutfit || S.Slot == EWorkerSlot::Overalls)
		{
			FAvFullOutfitCatalogItem ResolvedRecord;
			if (ResolveFullOutfitCatalogRecord(S.Slot, S.Mesh.ToSoftObjectPath(), ResolvedRecord) &&
				!ResolvedRecord.PrimaryMeshPath.IsEmpty())
			{
				MeshToLoad = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(ResolvedRecord.PrimaryMeshPath));
			}
		}
		if (USkeletalMesh* M = MeshToLoad.LoadSynchronous())
		{
			if (Part->GetSkeletalMeshAsset() && Part->GetSkeletalMeshAsset() != M)
			{
				Part = RecreateSlotComp(S.Slot);
				if (!Part) continue;
			}
			Part->SetLeaderPoseComponent(nullptr);
			Part->EmptyOverrideMaterials();
			Part->SetSkeletalMeshAsset(M);
			Part->EmptyOverrideMaterials();
			if (const FLinearColor* C = SlotColors.Find(S.Slot)) { ApplyColorToComp(Part, *C); }
		}
		if (Body)
		{
			AttachAsLeaderFollower(Part, Body);
			Part->SetVisibility(false, false);
		}
		else
		{
			// Без тела-лидера часть просто крепится к корню (ref-поза) — деградация, не краш.
			Part->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Part->SetHiddenInGame(false, true);
			Part->Activate(true);
			Part->SetVisibility(false, false);
		}
		RefreshVisualComponent(Part);
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
				(*Found)->SetLeaderPoseComponent(nullptr);
				(*Found)->EmptyOverrideMaterials();
				(*Found)->SetSkeletalMeshAsset(nullptr);
				(*Found)->DestroyComponent();
			}
			SlotComps.Remove(Slot);
		}
	}

	// Coverage is a derived visual state. Preview, gameplay, presets, old-save load and thumbnail
	// capture all converge here and never serialize the mask.
	ApplyResolvedBodyCoverage(Body);

	// One canonical material-profile pass for live pawns, local preview, Factory, Random,
	// Reset, SaveGame load, Head selection, and thumbnail capture.
	ApplyHeadTypeSkinProfile();

	if (Body)
	{
		Body->SetVisibility(true, false);
		Body->SetHiddenInGame(false, false);
		RefreshVisualComponent(Body);
	}
	const bool bSuppressHair = IsHairSuppressedByHeadgear();
	for (const TPair<EWorkerSlot, TObjectPtr<USkeletalMeshComponent>>& Pair : SlotComps)
	{
		if (!Pair.Value || Pair.Key == EWorkerSlot::Body)
		{
			continue;
		}
		const bool bVisible =
			!ResolvedSuppressedRenderSlots.Contains(Pair.Key) &&
			(Pair.Key != EWorkerSlot::Hair || !bSuppressHair);
		Pair.Value->SetVisibility(bVisible, false);
		Pair.Value->SetHiddenInGame(!bVisible, false);
		RefreshVisualComponent(Pair.Value);
	}
	for (const TPair<FName, TObjectPtr<USkeletalMeshComponent>>& Pair : DerivedBodyComps)
	{
		if (Pair.Value)
		{
			Pair.Value->SetVisibility(true, false);
			Pair.Value->SetHiddenInGame(false, false);
			RefreshVisualComponent(Pair.Value);
		}
	}
}
