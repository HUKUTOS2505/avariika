import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/batch_mc_wounded.json"
R = {"steps": [], "picked": [], "retargeted": []}
eal = unreal.EditorAssetLibrary
DEST = "/Game/Characters/Operator/Anims_Retarget"

# --- fix up RTG_MC_to_Operator (IK_Operator spine is now fixed) ---
rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MC_to_Operator.RTG_MC_to_Operator")
rtc = unreal.IKRetargeterController.get_controller(rt)
try: rtc.auto_map_chains(unreal.AutoMapChainType.EXACT, True); R["steps"].append("map ok")
except Exception as e: R["steps"].append("map err " + str(e))
try: rtc.auto_align_all_bones(unreal.RetargetSourceOrTarget.TARGET); R["steps"].append("align ok")
except Exception as e: R["steps"].append("align err " + str(e))
# disable IK ops + any duplicate "_0" op set (keep Pelvis/FK/Root/Remap first set)
opinfo = []
for i in range(rtc.get_num_retarget_ops()):
    nm = str(rtc.get_op_name(i))
    dis = ("IK Goals" in nm) or ("Run IK Rig" in nm) or nm.endswith("_0")
    try: rtc.set_retarget_op_enabled(i, not dis)
    except Exception: pass
    opinfo.append({"name": nm, "enabled": not dis})
R["ops"] = opinfo
eal.save_loaded_asset(rt, False)

# --- pick MC clips: wounded + crawl + knockdown + key emotes ---
src_mesh = unreal.load_asset("/Game/MC_Sample/Demo/Characters/MCUE5v2/Meshes/SKM_MCUE5v2.SKM_MCUE5v2")
op_mesh = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
ar = unreal.AssetRegistryHelpers.get_asset_registry()
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/MC_Sample"], recursive_paths=True)
KW = ["injuredbelly", "pronecrawl", "knockdown", "kickdown",
      "emotion_frustrated_01_all", "react_excited_01", "emotion_sad_01", "wave_02"]
picks = []
for a in ar.get_assets(f):
    nm = str(a.asset_name)
    low = nm.lower()
    if any(k in low for k in KW) and "_norm" not in low and "_ipc" not in low:
        picks.append(a)
R["picked"] = sorted([str(p.asset_name) for p in picks])

if picks and src_mesh and op_mesh:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget(picks, src_mesh, op_mesh, rt,
            search="", replace="", prefix="Op_", suffix="")
        R["steps"].append("retarget ok")
    except Exception as e:
        R["steps"].append("retarget err " + str(e))

# move new Op_ (from these picks) into DEST + save
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
unreal.log("BATCH_MC_WOUNDED done")
