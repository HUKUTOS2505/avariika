# Ретаргет Root_Motion стоячих поворотов Mobility UE4 -> WorkAnim (SK_Mannequin UE5), с сохранением root-motion.
# Источник имеет реальный поворот в кости root (R_90 -> 90 град). In-Place версии его НЕ имеют (оттого мёртвые).
import unreal
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
AL = unreal.AnimationLibrary
ar.scan_paths_synchronous(["/Game/Mobility_01"], True, False)

F = "/Game/Avariika/Anim/Rig"
DEST = "/Game/Avariika/Anim/Locomotion/Mobility/RootMotion"
SRC = "/Game/Mobility_01/Animation/Root_Motion"
src_mesh = unreal.load_asset("/Game/Mobility_01/Character/Mesh/SK_Mannequin.SK_Mannequin")
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
rt = unreal.load_asset(F + "/RTG_MobilityUE4_to_Mannequin.RTG_MobilityUE4_to_Mannequin")

names = ['MOB1_M1_Stand_Relaxed_R_45','MOB1_M1_Stand_Relaxed_R_90',
         'MOB1_M1_Stand_Relaxed_R_135','MOB1_M1_Stand_Relaxed_R_180']
# duplicate_and_retarget ждёт AssetData (не загруженные объекты) — берём фильтром из реестра
wanted = set(names)
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=[SRC], recursive_paths=False)
clips = [a for a in ar.get_assets(f) if str(a.asset_name) in wanted]
print("SRC clips loaded:", len(clips), "mesh=", bool(src_mesh), "tgt=", bool(tgt_mesh), "rt=", bool(rt))

try:
    unreal.IKRetargetBatchOperation.duplicate_and_retarget(clips, src_mesh, tgt_mesh, rt, search="", replace="", prefix="RT_", suffix="")
    print("RETARGET ok")
except Exception as e:
    print("RETARGET ERR:", str(e)[:200])

ar.scan_paths_synchronous(["/Game"], True, False)

def root_yaw(a):
    n = AL.get_num_frames(a)
    y0 = AL.get_bone_pose_for_frame(a, 'root', 0, False).rotation.rotator().yaw
    y1 = AL.get_bone_pose_for_frame(a, 'root', max(0,n-1), False).rotation.rotator().yaw
    return y1 - y0

for n in names:
    rtname = "RT_" + n
    srcpkg = "%s/%s" % (SRC, rtname)
    dstpkg = "%s/%s" % (DEST, rtname)
    moved = False
    if eal.does_asset_exist(srcpkg) and srcpkg != dstpkg:
        try:
            eal.rename_asset(srcpkg, dstpkg); moved = True
        except Exception as e:
            print("move err %s: %s" % (rtname, str(e)[:80]))
    obj = unreal.load_asset(dstpkg + "." + rtname) or unreal.load_asset(srcpkg + "." + rtname)
    if obj:
        obj.set_editor_property('enable_root_motion', True)
        eal.save_loaded_asset(obj, False)
        try:
            print("  %s -> root d=%.1f  rootMotion=%s  moved=%s" % (rtname, root_yaw(obj), obj.get_editor_property('enable_root_motion'), moved))
        except Exception as e:
            print("  %s saved (measure err %s)" % (rtname, str(e)[:60]))
    else:
        print("  %s NOT FOUND after retarget" % rtname)
print("DONE_RM_TURNS")
