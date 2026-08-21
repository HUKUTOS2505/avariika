# Сравнивает скелеты: BS_Locomotion, старый клип, Mobility-клип, ABP_Worker, тело воркера.
import unreal
lines = []
def skel_of(path, prop="skeleton"):
    a = unreal.load_asset(path)
    if not a: return "НЕТ(" + path + ")"
    try:
        s = a.get_editor_property(prop)
        return s.get_path_name() if s else "None"
    except Exception as e:
        return "err " + str(e)[:40]

lines.append("BS_Locomotion skel:      " + skel_of("/Game/Avariika/Anim/Locomotion/BS_Locomotion.BS_Locomotion"))
# старый клип — найдём anim_Idle
ar = unreal.AssetRegistryHelpers.get_asset_registry()
old = None
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
    if str(a.asset_name) == "anim_Idle":
        old = str(a.package_name) + ".anim_Idle"; break
lines.append("anim_Idle (старый) skel: " + (skel_of(old) if old else "не найден"))
lines.append("RT_MOB1 (Mobility) skel: " + skel_of("/Game/Avariika/Anim/Locomotion/Mobility/RT_MOB1_M1_Walk_F_IP.RT_MOB1_M1_Walk_F_IP"))
lines.append("ABP_Worker skel:         " + skel_of("/Game/Avariika/Anim/Locomotion/ABP_Worker.ABP_Worker", "target_skeleton"))

# тело воркера: какой скелет у SKM_Worker_Male_1 + его compatible skeletons
worker = unreal.load_asset("/Game/Modular_Workers/Meshes/SKM_Worker_Male_1.SKM_Worker_Male_1")
if not worker:
    # поискать
    for a in ar.get_assets(unreal.ARFilter(class_names=["SkeletalMesh"], recursive_paths=True)):
        if str(a.asset_name) == "SKM_Worker_Male_1":
            worker = unreal.load_asset(str(a.package_name) + ".SKM_Worker_Male_1"); break
if worker:
    sk = worker.get_editor_property("skeleton")
    lines.append("SKM_Worker_Male_1 skel:  " + (sk.get_path_name() if sk else "None"))
    if sk:
        try:
            comp = sk.get_editor_property("compatible_skeletons")
            lines.append("  Quantum compatible_skeletons: " + str([str(c.get_path_name()) for c in comp]))
        except Exception as e:
            lines.append("  compat err " + str(e)[:50])
else:
    lines.append("SKM_Worker_Male_1 не найден")

with open(r"C:/unrealEngine/avariika/Scripts/skeletons.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("DIAG_SKEL done")
