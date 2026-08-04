# Ретаргет ВСЕХ Root_Motion -> WorkAnim. НАДЁЖНО: малые чанки, сохраняем save_asset В КОРНЕ /Game/ сразу
# после ретаргета (до GC). Организацию в папку делаем отдельным проходом (move СОХРАНЁННЫХ ассетов надёжен).
import unreal
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game/Mobility_01"], True, False)

F = "/Game/Avariika/Anim/Rig"
SRC = "/Game/Mobility_01/Animation/Root_Motion"
src_mesh = unreal.load_asset("/Game/Mobility_01/Character/Mesh/SK_Mannequin.SK_Mannequin")
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
rt = unreal.load_asset(F + "/RTG_MobilityUE4_to_Mannequin.RTG_MobilityUE4_to_Mannequin")

f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=[SRC], recursive_paths=False)
allclips = list(ar.get_assets(f))
total = len(allclips)
log = ["Root_Motion найдено: %d" % total]

def chunks(lst, n):
    for i in range(0, len(lst), n):
        yield lst[i:i+n]

saved = 0
for ci, chunk in enumerate(chunks(allclips, 20)):
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget(list(chunk), src_mesh, tgt_mesh, rt, search="", replace="", prefix="RT_", suffix="")
    except Exception as e:
        log.append("chunk %d err: %s" % (ci, str(e)[:100]))
    ar.scan_paths_synchronous(["/Game"], True, False)
    for src in list(chunk):
        nm = "RT_" + str(src.asset_name)
        rootpkg = "/Game/" + nm
        obj = unreal.load_asset(rootpkg + "." + nm)
        if obj is None:
            continue
        try:
            obj.set_editor_property('enable_root_motion', True)
        except Exception:
            pass
        if eal.save_asset(rootpkg, only_if_is_dirty=False):
            saved += 1
    log.append("chunk %d: saved_total=%d" % (ci, saved))

with open(r"C:/unrealEngine/avariika/Scripts/rootmotion_all_result.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(log) + "\n")
print("RM_ROOT saved=%d of %d" % (saved, total))
