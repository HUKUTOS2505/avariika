import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/batch_mco.json"
R = {"steps": [], "retargeted": []}
eal = unreal.EditorAssetLibrary
DEST = "/Game/Characters/Operator/Anims_Retarget"

mco_mesh = unreal.load_asset("/Game/MCO_Mocap_Basics/Character/Mesh/SK_Mannequin.SK_Mannequin")
op_mesh = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
rtg = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MCO_to_Operator.RTG_MCO_to_Operator")

WANT = [
    "MOB1_Walk_F_IPC", "MOB1_Jog_F_IPC", "MOB1_Jog_F_to_Stand_Relaxed_RU_IPC",
    "MOB1_Run_F_IPC", "MOB1_Run_F_to_Stand_Relaxed_RU_IPC",
    "MOB1_CrouchWalk_F_IPC", "MOB1_Crouch_Idle_V2_IPC",
    "MOB1_Stand_Relaxed_Idle_v2_IPC", "MOB1_Stand_Relaxed_Fgt_v1_IPC", "MOB1_Stand_Relaxed_Fgt_v4_IPC",
    "MOB1_Stand_Relaxed_Death_B_IPC",
    "SCR_Walk_Scared_Fwd_Look_Left_Loop_IP", "SCR_Walk_Scared_Fwd_Look_Right_Loop_IP",
    "Walk_02_Cheerful_Loop_IP", "Walk_06_Look_Around_Loop_IP", "Walk_13_Power_Walk_Loop_IP",
]

ar = unreal.AssetRegistryHelpers.get_asset_registry()
# delete previous Op_ results so re-batch (with aligned pose) doesn't collide on rename
ar.scan_paths_synchronous(["/Game"], True, False)
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
    if str(a.asset_name).startswith("Op_"):
        try:
            eal.delete_asset(str(a.package_name))
        except Exception:
            pass

f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/MCO_Mocap_Basics"], recursive_paths=True)
byname = {str(a.asset_name): a for a in ar.get_assets(f)}
picks = [byname[n] for n in WANT if n in byname]
R["picked"] = [str(p.asset_name) for p in picks]
R["missing"] = [n for n in WANT if n not in byname]

if picks and mco_mesh and op_mesh and rtg:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget(picks, mco_mesh, op_mesh, rtg,
            search="", replace="", prefix="Op_", suffix="")
        R["steps"].append("retarget ok")
    except Exception as e:
        R["steps"].append("retarget err " + str(e))

# move all Op_ results into DEST + save
ar.scan_paths_synchronous(["/Game"], True, False)
f2 = unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)
for a in ar.get_assets(f2):
    nm = str(a.asset_name)
    if nm.startswith("Op_"):
        srcp = str(a.package_name)
        dstp = DEST + "/" + nm
        try:
            if eal.does_asset_exist(srcp) and srcp != dstp:
                eal.rename_asset(srcp, dstp)
        except Exception:
            pass
        obj = unreal.load_asset(dstp + "." + nm)
        if obj is None:
            obj = unreal.load_asset(srcp + "." + nm)
        if obj:
            eal.save_loaded_asset(obj, False)
            R["retargeted"].append(nm)

# cleanup old test anims OP_ / OP2_
for nm in ["OP_am_Staff_Cine_01_WalkFwd", "OP_am_StandDrunk_Idle_01", "OP_am_Stand_Idle_06_ScratchArm",
           "OP2_am_Staff_Cine_01_WalkFwd"]:
    p = DEST + "/" + nm
    try:
        if eal.does_asset_exist(p):
            eal.delete_asset(p)
    except Exception:
        pass

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("BATCH_MCO done")
