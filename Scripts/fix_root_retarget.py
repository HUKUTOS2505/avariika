# Фикс "тонет в полу": добавляет Root-чейн в IK_Motifect, пересобирает ретаргетер, ре-экспорт теста.
# Итог -> Scripts/fix_root_result.txt
import unreal, re
eal = unreal.EditorAssetLibrary
lines = []
F = "/Game/Avariika/Anim/Rig"

src = unreal.load_asset(F + "/IK_Motifect.IK_Motifect")
sc = unreal.IKRigController.get_controller(src)
# узнать retarget root + кости
try:
    rroot = str(sc.get_retarget_root())
except Exception as e:
    rroot = "?"; lines.append("get_retarget_root err " + str(e))
bones = [str(b) for b in sc.get_skeleton().bone_names] if hasattr(sc.get_skeleton(),'bone_names') else []
# имя корня: retarget root, либо 'root', либо первая кость
root_bone = rroot if rroot and rroot != "None" else ("root" if "root" in [b.lower() for b in bones] else (bones[0] if bones else None))
# найти таз
pelvis = None
for b in bones:
    if b.lower() in ("pelvis","hips","spine_00","root"): pelvis = b; break
lines.append("retarget_root=%s root_bone=%s pelvis=%s bones=%d" % (rroot, root_bone, pelvis, len(bones)))

existing = [str(c.chain_name) for c in sc.get_retarget_chains()]
if "Root" not in existing and root_bone:
    try:
        sc.add_retarget_chain("Root", root_bone, root_bone, "None")
        lines.append("added Root chain (%s)" % root_bone)
    except Exception as e:
        lines.append("add Root err " + str(e))
eal.save_loaded_asset(src, False)
lines.append("IK_Motifect chains now: " + str([str(c.chain_name) for c in sc.get_retarget_chains()]))

# пересобрать ретаргетер
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
rt = unreal.load_asset(F + "/RTG_Motifect_to_Mannequin.RTG_Motifect_to_Mannequin")
rtc = unreal.IKRetargeterController.get_controller(rt)
rt.set_editor_property("source_ik_rig_asset", src)
rt.set_editor_property("target_ik_rig_asset", tgt_rig)
for i in range(rtc.get_num_retarget_ops() - 1, -1, -1):
    try: rtc.remove_retarget_op(i)
    except Exception: pass
try: rtc.add_default_ops()
except Exception: pass
try:
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.TARGET, tgt_rig)
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.SOURCE, src)
except Exception: pass
try: rtc.auto_map_chains(unreal.AutoMapChainType.FUZZY, True)
except Exception: pass
for side in (unreal.RetargetSourceOrTarget.SOURCE, unreal.RetargetSourceOrTarget.TARGET):
    try: rtc.auto_align_all_bones(side)
    except Exception: pass
KEEP = {"Pelvis Motion", "FK Chains", "Root Motion", "Remap Curves"}
seen = set()
for i in range(rtc.get_num_retarget_ops()):
    nm = str(rtc.get_op_name(i)); base = re.sub(r"_\d+$", "", nm)
    en = (base in KEEP) and (base not in seen)
    if en: seen.add(base)
    try: rtc.set_retarget_op_enabled(i, en)
    except Exception: pass
eal.save_loaded_asset(rt, False)

# удалить старый RT_ и ре-экспорт
old = "/Game/Avariika/Anim/Injured/RT_limp_walk_left_leg"
if eal.does_asset_exist(old):
    try: eal.delete_asset(old)
    except Exception: pass
ar = unreal.AssetRegistryHelpers.get_asset_registry()
src_mesh = sc.get_skeletal_mesh()
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Avariika/Anim/_MotifectNative"], recursive_paths=True)
pick = {str(a.asset_name): a for a in ar.get_assets(f)}.get("limp_walk_left_leg")
if pick:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget([pick], src_mesh, tgt_mesh, rt, search="", replace="", prefix="RT_", suffix="")
    except Exception as e:
        lines.append("retarget err: " + str(e)[:200])
    ar.scan_paths_synchronous(["/Game"], True, False)
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
        if str(a.asset_name) == "RT_limp_walk_left_leg":
            srcp = str(a.package_name); dstp = "/Game/Avariika/Anim/Injured/RT_limp_walk_left_leg"
            try:
                if srcp != dstp: eal.rename_asset(srcp, dstp)
            except Exception: pass
            obj = unreal.load_asset(dstp + ".RT_limp_walk_left_leg") or unreal.load_asset(srcp + ".RT_limp_walk_left_leg")
            if obj: eal.save_loaded_asset(obj, False); lines.append("re-exported OK frames=%s" % obj.get_editor_property("number_of_sampled_frames"))
with open(r"C:/unrealEngine/avariika/Scripts/fix_root_result.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(lines) + "\n")
print("FIX_ROOT done")
