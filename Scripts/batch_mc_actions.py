import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/batch_mc_actions.json"
R = {"steps": [], "picked": [], "retargeted": []}
eal = unreal.EditorAssetLibrary
DEST = "/Game/Characters/Operator/Anims_Retarget"

rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MC_to_Operator.RTG_MC_to_Operator")
src_mesh = unreal.load_asset("/Game/MC_Sample/Demo/Characters/MCUE5v2/Meshes/SKM_MCUE5v2.SKM_MCUE5v2")
op_mesh = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")

WANT = {
    # interact / pickup / push / panel  (vending = reach panel, grab item, hit machine)
    "am_Vend_Start", "am_Vend_Success_GrabItem", "am_Vend_Fail_HitMachine",
    # repair (tool work, standing) + transitions
    "am_StandDrillLow_01_Drill", "am_StandDrillLow_Trans_Stand", "am_Stand_Trans_StandDrillLow",
    # drink consumable + transitions
    "am_Moonshine_01_Drink_Loop", "am_Moonshine_Trans_Stand", "am_Stand_Trans_Moonshine",
    # woozy / gas / high-panic stagger
    "am_Drunk_Loco_Walk_01", "am_StandDrunk_Idle_01",
    # search / coop / flavor
    "am_Stand_Idle_03_LookAround", "am_Stand_LookAt_05_PointRHand",
    "am_Stand_Idle_06_ScratchArm", "am_Stand_Conv_Talk_05_Generic",
    "am_Stand_Emotion_Frustrated_01_StompFeet", "am_Stand_React_Excited_HeelClick",
}
ar = unreal.AssetRegistryHelpers.get_asset_registry()
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/MC_Sample"], recursive_paths=True)
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
with open(OUT, "w") as fp:
    json.dump(R, fp, indent=1)
unreal.log("BATCH_MC_ACTIONS done")
