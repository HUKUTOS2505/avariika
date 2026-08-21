import unreal
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
AL = unreal.AnimationLibrary
ar.scan_paths_synchronous(["/Game/Mobility_01"], True, False)

F = "/Game/Avariika/Anim/Rig"
SRC = "/Game/Mobility_01/Animation/Root_Motion"
src_mesh = unreal.load_asset("/Game/Mobility_01/Character/Mesh/SK_Mannequin.SK_Mannequin")
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
rt = unreal.load_asset(F + "/RTG_MobilityUE4_to_Mannequin.RTG_MobilityUE4_to_Mannequin")

names = ['MOB1_M1_Stand_Relaxed_R_45','MOB1_M1_Stand_Relaxed_R_90',
         'MOB1_M1_Stand_Relaxed_R_135','MOB1_M1_Stand_Relaxed_R_180']
wanted = set(names)
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=[SRC], recursive_paths=False)
clips = [a for a in ar.get_assets(f) if str(a.asset_name) in wanted]
print("SRC clips:", len(clips))

unreal.IKRetargetBatchOperation.duplicate_and_retarget(clips, src_mesh, tgt_mesh, rt, search="", replace="", prefix="RT_", suffix="")
ar.scan_paths_synchronous(["/Game"], True, False)

def root_yaw(a):
    n = AL.get_num_frames(a)
    return AL.get_bone_pose_for_frame(a,'root',max(0,n-1),False).rotation.rotator().yaw - AL.get_bone_pose_for_frame(a,'root',0,False).rotation.rotator().yaw

for n in names:
    rtname = "RT_" + n
    pkg = "/Game/" + rtname
    obj = unreal.load_asset(pkg + "." + rtname)
    if obj is None:
        print("  %s NOT created" % rtname); continue
    obj.set_editor_property('enable_root_motion', True)
    saved = eal.save_asset(pkg, only_if_is_dirty=False)
    exists = eal.does_asset_exist(pkg)
    try:
        d = root_yaw(obj)
    except Exception as e:
        d = -999
    print("  %s  saved=%s exists=%s root_d=%.1f rm=%s" % (rtname, saved, exists, d, obj.get_editor_property('enable_root_motion')))
print("DONE_V2")
