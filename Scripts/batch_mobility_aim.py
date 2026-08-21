# Ретаргет 18 Aim_Offset Look-поз Mobility UE4 -> SK_Mannequin(WorkAnim). Выход: Locomotion/Mobility/Aim
import unreal
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
lines = []
def log(s): lines.append(str(s)); unreal.log("AIMRT| "+str(s))

ar.scan_paths_synchronous(["/Game/Mobility_01"], True, False)
F = "/Game/Avariika/Anim/Rig"
DEST = "/Game/Avariika/Anim/Locomotion/Mobility/Aim"
src_mesh = unreal.load_asset("/Game/Mobility_01/Character/Mesh/SK_Mannequin.SK_Mannequin")
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
rt = unreal.load_asset(F + "/RTG_MobilityUE4_to_Mannequin.RTG_MobilityUE4_to_Mannequin")

f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Mobility_01/Animation/Aim_Offset"], recursive_paths=True)
clips = list(ar.get_assets(f))
log("Aim_Offset clips found: %d" % len(clips))

try:
    unreal.IKRetargetBatchOperation.duplicate_and_retarget(list(clips), src_mesh, tgt_mesh, rt, search="", replace="", prefix="RT_", suffix="")
except Exception as e:
    log("retarget err: %s" % str(e)[:160])

ar.scan_paths_synchronous(["/Game"], True, False)
moved = 0
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
    nm = str(a.asset_name); pkg = str(a.package_name)
    if nm.startswith("RT_MOB1") and "Look" in nm and not pkg.startswith(DEST):
        dstp = DEST + "/" + nm
        try:
            if eal.does_asset_exist(pkg) and pkg != dstp:
                eal.rename_asset(pkg, dstp)
        except Exception: pass
        obj = unreal.load_asset(dstp + "." + nm) or unreal.load_asset(pkg + "." + nm)
        if obj:
            eal.save_loaded_asset(obj, False); moved += 1
log("moved Look poses: %d" % moved)
ar.scan_paths_synchronous([DEST], True, False)
final = unreal.EditorAssetLibrary.list_assets(DEST, recursive=True, include_folder=False) if eal.does_directory_exist(DEST) else []
log("ИТОГО в Aim: %d" % len(final))
for p in sorted(final): log("  "+p.split('/')[-1].split('.')[0])

with open(r"C:/unrealEngine/avariika/Scripts/batch_mobility_aim_out.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(lines)+"\n")
