# Перестраивает RTG_Motifect_to_Mannequin с ПРАВИЛЬНЫМ включением ops (по паттерну build_ue5manny),
# затем тест-ретаргет 1 анима. Итог -> Scripts/fix_test_result.txt
import unreal, re
eal = unreal.EditorAssetLibrary
lines = []
F = "/Game/Avariika/Anim/Rig"

src = unreal.load_asset(F + "/IK_Motifect.IK_Motifect")
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
rt = unreal.load_asset(F + "/RTG_Motifect_to_Mannequin.RTG_Motifect_to_Mannequin")
rtc = unreal.IKRetargeterController.get_controller(rt)
rt.set_editor_property("source_ik_rig_asset", src)
rt.set_editor_property("target_ik_rig_asset", tgt_rig)
# пересоздать ops
for i in range(rtc.get_num_retarget_ops() - 1, -1, -1):
    try: rtc.remove_retarget_op(i)
    except Exception: pass
try: rtc.add_default_ops()
except Exception as e: lines.append("add_default_ops err " + str(e))
try:
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.TARGET, tgt_rig)
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.SOURCE, src)
except Exception as e: lines.append("assign err " + str(e))
try: rtc.auto_map_chains(unreal.AutoMapChainType.FUZZY, True)
except Exception as e: lines.append("map err " + str(e))
for side, lbl in [(unreal.RetargetSourceOrTarget.SOURCE,"src"),(unreal.RetargetSourceOrTarget.TARGET,"tgt")]:
    try: rtc.auto_align_all_bones(side)
    except Exception as e: lines.append("align "+lbl+" err "+str(e))
# ВКЛЮЧИТЬ нужные ops (это и был пропущенный шаг)
KEEP = {"Pelvis Motion", "FK Chains", "Root Motion", "Remap Curves"}
seen = set(); ops = []
for i in range(rtc.get_num_retarget_ops()):
    nm = str(rtc.get_op_name(i)); base = re.sub(r"_\d+$", "", nm)
    en = (base in KEEP) and (base not in seen)
    if en: seen.add(base)
    try: rtc.set_retarget_op_enabled(i, en)
    except Exception: pass
    ops.append("%s=%s" % (nm, en))
lines.append("ops: " + " | ".join(ops))
eal.save_loaded_asset(rt, False)

# тест 1 аним
ar = unreal.AssetRegistryHelpers.get_asset_registry()
src_mesh = unreal.IKRigController.get_controller(src).get_skeletal_mesh()
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Avariika/Anim/_MotifectNative"], recursive_paths=True)
byname = {str(a.asset_name): a for a in ar.get_assets(f)}
pick = byname.get("limp_walk_left_leg")
if pick and src_mesh and tgt_mesh:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget([pick], src_mesh, tgt_mesh, rt,
            search="", replace="", prefix="RT_", suffix="")
        lines.append("retarget call ok")
    except Exception as e:
        lines.append("retarget err: " + str(e)[:300])
    ar.scan_paths_synchronous(["/Game"], True, False)
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
        nm = str(a.asset_name)
        if nm == "RT_limp_walk_left_leg":
            srcp = str(a.package_name); dstp = "/Game/Avariika/Anim/Injured/" + nm
            try:
                if srcp != dstp: eal.rename_asset(srcp, dstp)
            except Exception: pass
            obj = unreal.load_asset(dstp + "." + nm) or unreal.load_asset(srcp + "." + nm)
            if obj:
                eal.save_loaded_asset(obj, False)
                sk = obj.get_editor_property("skeleton")
                lines.append("RESULT %s skeleton=%s frames=%s" % (nm, sk.get_name() if sk else "-", obj.get_editor_property("number_of_sampled_frames")))
with open(r"C:/unrealEngine/avariika/Scripts/fix_test_result.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(lines) + "\n")
print("FIX_TEST done")
