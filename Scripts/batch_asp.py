import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/batch_asp.json"
R = {"steps": [], "picked": [], "retargeted": []}
eal = unreal.EditorAssetLibrary
DEST = "/Game/Characters/Operator/Anims_Retarget"

# reuse the already-tuned UE4-mannequin retargeter (clean FK ops); ASP shares bone names
rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MCO_to_Operator.RTG_MCO_to_Operator")
src_mesh = unreal.load_asset("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin")
op_mesh = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
R["have"] = {"rt": bool(rt), "src": bool(src_mesh), "op": bool(op_mesh)}

# clean unarmed clips only (skip every rifle/pistol/ironsights/prone pose)
WANT = {"Death_1", "Death_2", "Death_3",
        "Hit_React_1", "Hit_React_2", "Hit_React_3", "Hit_React_4",
        "Jump_From_Stand", "Jump_From_Jog"}
ar = unreal.AssetRegistryHelpers.get_asset_registry()
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/AnimStarterPack"], recursive_paths=True)
picks = [a for a in ar.get_assets(f) if str(a.asset_name) in WANT]
R["picked"] = sorted(str(p.asset_name) for p in picks)

if picks and rt and src_mesh and op_mesh:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget(
            picks, src_mesh, op_mesh, rt, search="", replace="", prefix="Op_", suffix="")
        R["steps"].append("retarget ok")
    except Exception as e:
        R["steps"].append("retarget err " + str(e))

picknames = set("Op_" + str(p.asset_name) for p in picks)
ar.scan_paths_synchronous(["/Game"], True, False)
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
    nm = str(a.asset_name)
    if nm in picknames:
        srcp = str(a.package_name); dstp = DEST + "/" + nm
        try:
            if eal.does_asset_exist(srcp) and srcp != dstp:
                eal.rename_asset(srcp, dstp)
        except Exception: pass
        obj = unreal.load_asset(dstp + "." + nm) or unreal.load_asset(srcp + "." + nm)
        if obj:
            eal.save_loaded_asset(obj, False); R["retargeted"].append(nm)
with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("BATCH_ASP done")
