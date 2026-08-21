import unreal
_OUT=[]
def line(s=""):
    unreal.log("BONES| "+str(s)); _OUT.append(str(s))

FBL_MESH  = "/Game/FreeAnimationLibrary/Demo/Characters/Mannequins/Meshes/SK_Mannequin"
WORK_MESH = "/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin"
QSK_MESH  = "/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_1"

def bone_names(mesh_path):
    m = unreal.load_asset(mesh_path)
    if not m:
        return None, "load fail"
    try:
        comp = unreal.new_object(unreal.SkeletalMeshComponent, outer=unreal.get_editor_subsystem(unreal.EditorActorSubsystem) or unreal.get_engine_subsystem(unreal.EditorAssetSubsystem))
    except Exception:
        comp = unreal.SkeletalMeshComponent()
    try:
        comp.set_skeletal_mesh_asset(m)
    except Exception:
        try:
            comp.set_editor_property("skeletal_mesh_asset", m)
        except Exception as e:
            return None, "set mesh err %s" % e
    try:
        n = comp.get_num_bones()
        names = [str(comp.get_bone_name(i)) for i in range(n)]
        return names, None
    except Exception as e:
        return None, "bone read err %s" % e

KEY = ["root","pelvis","spine_01","spine_03","spine_05","clavicle_l","upperarm_l",
       "lowerarm_l","hand_l","thigh_l","calf_l","foot_l","ball_l","neck_01","head"]

sets = {}
for label, p in (("FBL", FBL_MESH), ("WORK", WORK_MESH), ("QSK", QSK_MESH)):
    names, err = bone_names(p)
    if names is None:
        line("%s : ERR %s" % (label, err)); sets[label]=set(); continue
    s = set(names)
    sets[label]=s
    line("%s : %d bones; first8=%s" % (label, len(names), names[:8]))
    missing = [k for k in KEY if k not in s]
    line("    KEY missing: %s" % (missing if missing else "none (all core bones present)"))

q = sets.get("QSK", set())
for label in ("FBL","WORK"):
    s = sets.get(label, set())
    if s and q:
        inter = len(s & q)
        line("OVERLAP %s∩QSK = %d (of %s in %s, %s in QSK)" % (label, inter, len(s), label, len(q)))

# re-confirm compatible lists persisted in memory
line("--- compatible (in-memory now) ---")
for label, p in (("FBL","/Game/FreeAnimationLibrary/Demo/Characters/Mannequins/Meshes/SK_Mannequin"),
                 ("WORK","/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin"),
                 ("QSK","/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton")):
    sk = unreal.load_asset(p)
    try:
        cs = sk.get_editor_property("compatible_skeletons")
        line("%s compatible: %s" % (label, [c.get_name() for c in (cs or [])]))
    except Exception as e:
        line("%s compat err %s" % (label, e))

with open("C:/unrealEngine/avariika/Scripts/diag_bones_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
