# Тест-ретаргет 3 анимов Motifect -> SK_Mannequin через RTG_Motifect_to_Mannequin.
# Проверка качества до батча. Итог -> Scripts/test_retarget_result.txt
import unreal

eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
lines = []

# меши
src_rig = unreal.load_asset("/Game/Avariika/Anim/Rig/IK_Motifect.IK_Motifect")
src_mesh = None
try:
    src_mesh = unreal.IKRigController.get_controller(src_rig).get_skeletal_mesh()
except Exception as e:
    lines.append("src mesh from rig err: " + str(e))
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
tgt_mesh = None
try:
    tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
except Exception as e:
    lines.append("tgt mesh from rig err: " + str(e))
rtg = unreal.load_asset("/Game/Avariika/Anim/Rig/RTG_Motifect_to_Mannequin.RTG_Motifect_to_Mannequin")
lines.append("src_mesh_cls=%s tgt_mesh_cls=%s" % (src_mesh.get_class().get_name() if src_mesh else "-", tgt_mesh.get_class().get_name() if tgt_mesh else "-"))
lines.append("src_mesh=%s tgt_mesh=%s rtg=%s" % (bool(src_mesh), bool(tgt_mesh), bool(rtg)))

TEST = ["crawl_exhausted", "limp_walk_left_leg", "collapse_to_knees"]
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Avariika/Anim/_MotifectNative"], recursive_paths=True)
byname = {str(a.asset_name): a for a in ar.get_assets(f)}
picks = [byname[n] for n in TEST if n in byname]
lines.append("picks: " + ", ".join(str(p.asset_name) for p in picks))
lines.append("missing: " + ", ".join(n for n in TEST if n not in byname))

if picks and src_mesh and tgt_mesh and rtg:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget(picks, src_mesh, tgt_mesh, rtg,
            search="", replace="", prefix="RT_", suffix="")
        lines.append("retarget call ok")
    except Exception as e:
        lines.append("retarget err: " + str(e))
    # переместить RT_ результаты в Injured + санити (кадры/скелет)
    ar.scan_paths_synchronous(["/Game/Avariika/Anim"], True, False)
    DEST = "/Game/Avariika/Anim/Injured"
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
        nm = str(a.asset_name)
        if nm.startswith("RT_"):
            srcp = str(a.package_name); dstp = DEST + "/" + nm
            try:
                if eal.does_asset_exist(srcp) and srcp != dstp:
                    eal.rename_asset(srcp, dstp)
            except Exception: pass
            obj = unreal.load_asset(dstp + "." + nm) or unreal.load_asset(srcp + "." + nm)
            if obj:
                eal.save_loaded_asset(obj, False)
                sk = obj.get_editor_property("skeleton")
                frames = obj.get_editor_property("number_of_sampled_frames") if hasattr(obj, 'get_editor_property') else "?"
                lines.append("  OK %s skeleton=%s frames=%s" % (nm, str(sk.get_name()) if sk else "-", frames))
else:
    lines.append("ABORT: чего-то нет")

with open(r"C:/unrealEngine/avariika/Scripts/test_retarget_result.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(lines) + "\n")
print("TEST_RETARGET done")
