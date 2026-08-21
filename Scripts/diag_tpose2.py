import unreal

_OUT = []
def line(s=""):
    unreal.log("DIAG2| " + str(s))
    _OUT.append(str(s))

line("===== BODY CHAIN DIAGNOSTIC =====")

# ABP_Worker preview mesh + graph anim refs
abp = unreal.load_asset("/Game/Avariika/Anim/Locomotion/ABP_Worker")
if abp:
    try:
        pm = abp.get_editor_property("preview_skeletal_mesh")
        line("ABP preview mesh: %s" % (pm.get_path_name() if pm else None))
    except Exception as e:
        line("ABP preview mesh err: %s" % e)

# Which blendspaces / anims does ABP reference? Use AssetRegistry dependencies.
ar = unreal.AssetRegistryHelpers.get_asset_registry()
deps = ar.get_dependencies("/Game/Avariika/Anim/Locomotion/ABP_Worker",
                           unreal.AssetRegistryDependencyOptions(include_hard_package_references=True))
line("ABP_Worker hard deps:")
for d in (deps or []):
    s = str(d)
    if any(k in s for k in ("BS_", "Blend", "RT_MOB", "Mobility", "Loco", "SK_Mann", "Quantum")):
        line("  dep: %s" % s)

# Player BP mesh component: skeletal mesh + anim class
bp = unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
gc = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
cdo = None
try:
    cdo = unreal.get_default_object(gc)
except Exception as e:
    line("CDO err: %s" % e)
if cdo:
    try:
        mc = cdo.get_editor_property("mesh")  # ACharacter Mesh component
        if mc:
            sm = mc.get_editor_property("skeletal_mesh_asset") if hasattr(mc, "get_editor_property") else None
            try:
                sm = mc.get_editor_property("skeletal_mesh_asset")
            except Exception:
                sm = mc.get_skeletal_mesh_asset() if hasattr(mc, "get_skeletal_mesh_asset") else None
            line("Player Mesh comp SkeletalMesh: %s" % (sm.get_path_name() if sm else None))
            if sm:
                sk = sm.get_editor_property("skeleton")
                line("  -> skeleton: %s" % (sk.get_path_name() if sk else None))
            ac = mc.get_editor_property("anim_class")
            line("Player Mesh AnimClass: %s" % (ac.get_path_name() if ac else None))
    except Exception as e:
        line("mesh comp err: %s" % e)

line("===== END =====")
try:
    with open("C:/unrealEngine/avariika/Scripts/diag_tpose2_out.txt", "w", encoding="utf-8") as f:
        f.write("\n".join(_OUT))
except Exception as e:
    unreal.log_error("DIAG2 write fail: %s" % e)
