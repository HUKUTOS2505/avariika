# Батч-ретаргет ВСЕХ 219 In-Place клипов Mobility UE4 -> SK_Mannequin (UE5), чанками.
# Выход: /Game/Avariika/Anim/Locomotion/Mobility. Итог -> Scripts/batch_mobility_result.txt
import unreal
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
lines = []
ar.scan_paths_synchronous(["/Game/Mobility_01"], True, False)

F = "/Game/Avariika/Anim/Rig"
DEST = "/Game/Avariika/Anim/Locomotion/Mobility"
src_mesh = unreal.load_asset("/Game/Mobility_01/Character/Mesh/SK_Mannequin.SK_Mannequin")
rig = unreal.load_asset(F + "/IK_MobilityUE4.IK_MobilityUE4")
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
rt = unreal.load_asset(F + "/RTG_MobilityUE4_to_Mannequin.RTG_MobilityUE4_to_Mannequin")

f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Mobility_01/Animation/In-Place"], recursive_paths=True)
allclips = list(ar.get_assets(f))
lines.append("In-Place найдено: %d" % len(allclips))

def chunks(lst, n):
    for i in range(0, len(lst), n):
        yield lst[i:i+n]

done = 0
for ci, chunk in enumerate(chunks(allclips, 40)):
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget(list(chunk), src_mesh, tgt_mesh, rt, search="", replace="", prefix="RT_", suffix="")
    except Exception as e:
        lines.append("chunk %d err: %s" % (ci, str(e)[:120]))
    # переместить+сохранить RT_ этого чанка
    ar.scan_paths_synchronous(["/Game"], True, False)
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
        nm = str(a.asset_name); pkg = str(a.package_name)
        if nm.startswith("RT_MOB1") and not pkg.startswith(DEST):
            dstp = DEST + "/" + nm
            try:
                if eal.does_asset_exist(pkg) and pkg != dstp:
                    eal.rename_asset(pkg, dstp)
            except Exception: pass
            obj = unreal.load_asset(dstp + "." + nm) or unreal.load_asset(pkg + "." + nm)
            if obj:
                eal.save_loaded_asset(obj, False); done += 1
    lines.append("chunk %d готово, всего перемещено: %d" % (ci, done))

# финальный подсчёт
ar.scan_paths_synchronous([DEST], True, False)
final = len(unreal.EditorAssetLibrary.list_assets(DEST, recursive=True, include_folder=False)) if unreal.EditorAssetLibrary.does_directory_exist(DEST) else 0
lines.append("ИТОГО в Locomotion/Mobility: %d" % final)
with open(r"C:/unrealEngine/avariika/Scripts/batch_mobility_result.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(lines) + "\n")
print("BATCH_MOBILITY done count=%d" % final)
