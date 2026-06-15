import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/batch_uefn.json"
R = {"steps": [], "retargeted": []}
eal = unreal.EditorAssetLibrary
DEST = "/Game/Characters/Operator/Anims_Retarget"

rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_UEFN_to_Operator.RTG_UEFN_to_Operator")
rtc = unreal.IKRetargeterController.get_controller(rt)
# keep only clean single FK set: Pelvis Motion(0), FK Chains(1), Root Motion(4), Remap(5)
KEEP = {0, 1, 4, 5}
for i in range(rtc.get_num_retarget_ops()):
    try: rtc.set_retarget_op_enabled(i, i in KEEP)
    except Exception: pass
eal.save_loaded_asset(rt, False)
R["steps"].append("ops trimmed")

mesh = unreal.load_asset("/Game/Characters/UEFN_Mannequin/Meshes/SKM_UEFN_Mannequin.SKM_UEFN_Mannequin")
opmesh = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
ar = unreal.AssetRegistryHelpers.get_asset_registry()
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Characters/UEFN_Mannequin"], recursive_paths=True)
picks = list(ar.get_assets(f))
R["pick_count"] = len(picks)
try:
    unreal.IKRetargetBatchOperation.duplicate_and_retarget(picks, mesh, opmesh, rt, search="", replace="", prefix="Op_", suffix="")
    R["steps"].append("retarget ok")
except Exception as e:
    R["steps"].append("retarget err " + str(e))

ar.scan_paths_synchronous(["/Game"], True, False)
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
    nm = str(a.asset_name)
    if nm.startswith("Op_M_Neutral_"):
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
unreal.log("BATCH_UEFN done")
