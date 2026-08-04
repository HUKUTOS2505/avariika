# Скан Mobility_01 + тест-ретаргет 3 клипов через готовый RTG_MobilityUE4_to_Mannequin.
import unreal
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
lines = []
ar.scan_paths_synchronous(["/Game/Mobility_01"], True, False)

F = "/Game/Avariika/Anim/Rig"
src_mesh = unreal.load_asset("/Game/Mobility_01/Character/Mesh/SK_Mannequin.SK_Mannequin")
rig = unreal.load_asset(F + "/IK_MobilityUE4.IK_MobilityUE4")
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
rt = unreal.load_asset(F + "/RTG_MobilityUE4_to_Mannequin.RTG_MobilityUE4_to_Mannequin")

f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Mobility_01"], recursive_paths=True)
byname = {str(a.asset_name): a for a in ar.get_assets(f)}
lines.append("найдено анимов в Mobility_01: %d" % len(byname))
WANT = ["MOB1_M1_Walk_F_IP", "MOB1_M1_Jog_F_IP", "MOB1_M1_Stand_Relaxed_Idle_IP"]
picks = [byname[n] for n in WANT if n in byname]
lines.append("picks: " + ", ".join(str(p.asset_name) for p in picks))

if picks and src_mesh and tgt_mesh and rt:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget(picks, src_mesh, tgt_mesh, rt, search="", replace="", prefix="RT_", suffix="")
        lines.append("retarget call ok")
    except Exception as e:
        lines.append("retarget err: " + str(e)[:200])
    ar.scan_paths_synchronous(["/Game"], True, False)
    DEST = "/Game/Avariika/Anim/Locomotion"
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
        nm = str(a.asset_name)
        if nm.startswith("RT_MOB1"):
            srcp = str(a.package_name); dstp = DEST + "/" + nm
            try:
                if srcp != dstp: eal.rename_asset(srcp, dstp)
            except Exception: pass
            obj = unreal.load_asset(dstp + "." + nm) or unreal.load_asset(srcp + "." + nm)
            if obj:
                eal.save_loaded_asset(obj, False)
                sk = obj.get_editor_property("skeleton")
                lines.append("  OK %s skeleton=%s frames=%s" % (nm, sk.get_name() if sk else "-", obj.get_editor_property("number_of_sampled_frames")))
with open(r"C:/unrealEngine/avariika/Scripts/mobility_clips_result.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(lines) + "\n")
print("TEST_MOBILITY done")
