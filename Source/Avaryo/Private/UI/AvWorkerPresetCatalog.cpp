#include "UI/AvWorkerPresetCatalog.h"

#include "AvariikaLoc.h"

namespace
{
	struct FWorkerPresetLoadoutEntry
	{
		const TCHAR* MeshName;
		const TCHAR* HeadKey;
		const TCHAR* FaceKey;
		const TCHAR* TopKey;
		const TCHAR* HandsKey;
		const TCHAR* BottomKey;
	};

	// Project-owned descriptions for the merged Modular Workers preset meshes.
	// The composition was verified from each mesh's material slots and assigned materials.
	const FWorkerPresetLoadoutEntry WorkerPresetLoadouts[] =
	{
		{ TEXT("SKM_Worker_Male_1"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_1.Head"),               TEXT("Customization.Preset.Loadout.SKM_Worker_Male_1.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_1.Upper"),             TEXT("Customization.Preset.Loadout.SKM_Worker_Male_1.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_1.Lower") },
		{ TEXT("SKM_Worker_Male_2"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_2.Head"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_2.Face"),                TEXT("Customization.Preset.Loadout.SKM_Worker_Male_2.Upper"),               TEXT("Customization.Preset.Loadout.SKM_Worker_Male_2.Hands"),      TEXT("Customization.Preset.Loadout.SKM_Worker_Male_2.Lower") },
		{ TEXT("SKM_Worker_Male_3"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_3.Head"),                TEXT("Customization.Preset.Loadout.SKM_Worker_Male_3.Face"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_3.Upper"),                 TEXT("Customization.Preset.Loadout.SKM_Worker_Male_3.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_3.Lower") },
		{ TEXT("SKM_Worker_Male_4"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_4.Head"),               TEXT("Customization.Preset.Loadout.SKM_Worker_Male_4.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_4.Upper"),             TEXT("Customization.Preset.Loadout.SKM_Worker_Male_4.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_4.Lower") },
		{ TEXT("SKM_Worker_Male_5"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_5.Head"),                TEXT("Customization.Preset.Loadout.SKM_Worker_Male_5.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_5.Upper"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_5.Hands"),      TEXT("Customization.Preset.Loadout.SKM_Worker_Male_5.Lower") },
		{ TEXT("SKM_Worker_Male_6"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_6.Head"),               TEXT("Customization.Preset.Loadout.SKM_Worker_Male_6.Face"),           TEXT("Customization.Preset.Loadout.SKM_Worker_Male_6.Upper"),        TEXT("Customization.Preset.Loadout.SKM_Worker_Male_6.Hands"),     TEXT("Customization.Preset.Loadout.SKM_Worker_Male_6.Lower") },
		{ TEXT("SKM_Worker_Male_7"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_7.Head"),        TEXT("Customization.Preset.Loadout.SKM_Worker_Male_7.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_7.Upper"),   TEXT("Customization.Preset.Loadout.SKM_Worker_Male_7.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_7.Lower") },
		{ TEXT("SKM_Worker_Male_8"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_8.Head"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_8.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_8.Upper"),    TEXT("Customization.Preset.Loadout.SKM_Worker_Male_8.Hands"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_8.Lower") },
		{ TEXT("SKM_Worker_Male_9"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_9.Head"),               TEXT("Customization.Preset.Loadout.SKM_Worker_Male_9.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_9.Upper"),                     TEXT("Customization.Preset.Loadout.SKM_Worker_Male_9.Hands"),     TEXT("Customization.Preset.Loadout.SKM_Worker_Male_9.Lower") },
		{ TEXT("SKM_Worker_Male_10"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_10.Head"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_10.Face"),           TEXT("Customization.Preset.Loadout.SKM_Worker_Male_10.Upper"),                       TEXT("Customization.Preset.Loadout.SKM_Worker_Male_10.Hands"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_10.Lower") },
		{ TEXT("SKM_Worker_Male_11"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_11.Head"),              TEXT("Customization.Preset.Loadout.SKM_Worker_Male_11.Face"),                TEXT("Customization.Preset.Loadout.SKM_Worker_Male_11.Upper"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_11.Hands"),        TEXT("Customization.Preset.Loadout.SKM_Worker_Male_11.Lower") },
		{ TEXT("SKM_Worker_Male_12"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_12.Head"),              TEXT("Customization.Preset.Loadout.SKM_Worker_Male_12.Face"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_12.Upper"),                      TEXT("Customization.Preset.Loadout.SKM_Worker_Male_12.Hands"),        TEXT("Customization.Preset.Loadout.SKM_Worker_Male_12.Lower") },
		{ TEXT("SKM_Worker_Male_13"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_13.Head"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_13.Face"),                  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_13.Upper"),                      TEXT("Customization.Preset.Loadout.SKM_Worker_Male_13.Hands"),  TEXT("Customization.Preset.Loadout.SKM_Worker_Male_13.Lower") },
		{ TEXT("SKM_Worker_Male_14"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_14.Head"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_14.Face"),             TEXT("Customization.Preset.Loadout.SKM_Worker_Male_14.Upper"),     TEXT("Customization.Preset.Loadout.SKM_Worker_Male_14.Hands"),     TEXT("Customization.Preset.Loadout.SKM_Worker_Male_14.Lower") },
		{ TEXT("SKM_Worker_Male_15"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_15.Head"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_15.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_15.Upper"),             TEXT("Customization.Preset.Loadout.SKM_Worker_Male_15.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_15.Lower") },
		{ TEXT("SKM_Worker_Male_16"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_16.Head"),           TEXT("Customization.Preset.Loadout.SKM_Worker_Male_16.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_16.Upper"),              TEXT("Customization.Preset.Loadout.SKM_Worker_Male_16.Hands"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_16.Lower") },
		{ TEXT("SKM_Worker_Male_17"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_17.Head"),           TEXT("Customization.Preset.Loadout.SKM_Worker_Male_17.Face"),             TEXT("Customization.Preset.Loadout.SKM_Worker_Male_17.Upper"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_17.Hands"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_17.Lower") },
		{ TEXT("SKM_Worker_Male_18"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_18.Head"),                        TEXT("Customization.Preset.Loadout.SKM_Worker_Male_18.Face"),             TEXT("Customization.Preset.Loadout.SKM_Worker_Male_18.Upper"),                     TEXT("Customization.Preset.Loadout.SKM_Worker_Male_18.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_18.Lower") },
		{ TEXT("SKM_Worker_Male_19"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_19.Head"),        TEXT("Customization.Preset.Loadout.SKM_Worker_Male_19.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_19.Upper"),                   TEXT("Customization.Preset.Loadout.SKM_Worker_Male_19.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_19.Lower") },
		{ TEXT("SKM_Worker_Male_20"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_20.Head"),        TEXT("Customization.Preset.Loadout.SKM_Worker_Male_20.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_20.Upper"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_20.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_20.Lower") },
		{ TEXT("SKM_Worker_Male_21"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_21.Head"),               TEXT("Customization.Preset.Loadout.SKM_Worker_Male_21.Face"),    TEXT("Customization.Preset.Loadout.SKM_Worker_Male_21.Upper"),                       TEXT("Customization.Preset.Loadout.SKM_Worker_Male_21.Hands"),      TEXT("Customization.Preset.Loadout.SKM_Worker_Male_21.Lower") },
		{ TEXT("SKM_Worker_Male_22"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_22.Head"),                        TEXT("Customization.Preset.Loadout.SKM_Worker_Male_22.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_22.Upper"),                     TEXT("Customization.Preset.Loadout.SKM_Worker_Male_22.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_22.Lower") },
		{ TEXT("SKM_Worker_Male_23"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_23.Head"),              TEXT("Customization.Preset.Loadout.SKM_Worker_Male_23.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_23.Upper"),                       TEXT("Customization.Preset.Loadout.SKM_Worker_Male_23.Hands"),            TEXT("Customization.Preset.Loadout.SKM_Worker_Male_23.Lower") },
		{ TEXT("SKM_Worker_Male_24"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_24.Head"),              TEXT("Customization.Preset.Loadout.SKM_Worker_Male_24.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_24.Upper"),                     TEXT("Customization.Preset.Loadout.SKM_Worker_Male_24.Hands"),         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_24.Lower") },
		{ TEXT("SKM_Worker_Male_25"), TEXT("Customization.Preset.Loadout.SKM_Worker_Male_25.Head"),              TEXT("Customization.Preset.Loadout.SKM_Worker_Male_25.Face"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_25.Upper"),              TEXT("Customization.Preset.Loadout.SKM_Worker_Male_25.Hands"),                         TEXT("Customization.Preset.Loadout.SKM_Worker_Male_25.Lower") }
	};

	static_assert(UE_ARRAY_COUNT(WorkerPresetLoadouts) == AvWorkerPresetCatalog::PresetCount);
}

bool AvWorkerPresetCatalog::TryGetLoadoutSummary(
	const FSoftObjectPath& PresetMeshPath,
	FAvWorkerPresetLoadoutSummary& OutSummary)
{
	const FString MeshName = PresetMeshPath.GetAssetName();
	for (const FWorkerPresetLoadoutEntry& Entry : WorkerPresetLoadouts)
	{
		if (MeshName.Equals(Entry.MeshName, ESearchCase::CaseSensitive))
		{
			OutSummary.Head = FAvLoc::Text(Entry.HeadKey);
			OutSummary.Face = FAvLoc::Text(Entry.FaceKey);
			OutSummary.Top = FAvLoc::Text(Entry.TopKey);
			OutSummary.Hands = FAvLoc::Text(Entry.HandsKey);
			OutSummary.Bottom = FAvLoc::Text(Entry.BottomKey);
			return true;
		}
	}

	return false;
}
