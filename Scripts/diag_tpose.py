import unreal

_OUT = []
def line(s=""):
    unreal.log("DIAG| " + str(s))
    _OUT.append(str(s))

def load(p):
    try:
        return unreal.load_asset(p)
    except Exception as e:
        line("LOAD FAIL %s : %s" % (p, e))
        return None

line("===== T-POSE DIAGNOSTIC =====")

# ---- 1. The base blendspace ----
BS = "/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility"
bs = load(BS)
line("--- BS_Loco_Mobility ---")
if bs:
    line("class: %s" % bs.get_class().get_name())
    try:
        sk = bs.get_editor_property("skeleton")
        line("skeleton: %s" % (sk.get_path_name() if sk else None))
    except Exception as e:
        line("skeleton read err: %s" % e)
    # try to enumerate samples
    for prop in ("sample_data", "blend_samples"):
        try:
            sd = bs.get_editor_property(prop)
            line("prop %s len=%s" % (prop, len(sd) if sd is not None else None))
            cnt = 0
            for s in (sd or []):
                cnt += 1
                if cnt <= 6:
                    try:
                        anim = s.get_editor_property("animation")
                        pos = s.get_editor_property("sample_value")
                        line("  sample[%d] anim=%s pos=%s" % (cnt, anim.get_name() if anim else None, pos))
                    except Exception as e2:
                        line("  sample read err: %s" % e2)
        except Exception:
            pass

# ---- 2. A retargeted clip ----
CLIP = "/Game/Avariika/Anim/Locomotion/Mobility/RT_MOB1_M1_Jog_F_IP"
clip = load(CLIP)
line("--- RT_MOB1_M1_Jog_F_IP ---")
if clip:
    line("class: %s" % clip.get_class().get_name())
    try:
        sk = clip.get_editor_property("skeleton")
        line("skeleton: %s" % (sk.get_path_name() if sk else None))
    except Exception as e:
        line("skeleton err: %s" % e)
    try:
        line("play_length: %s" % clip.get_play_length())
    except Exception as e:
        line("play_length err: %s" % e)
    try:
        nf = unreal.AnimationLibrary.get_num_frames(clip)
        line("num_frames: %s" % nf)
    except Exception as e:
        line("num_frames err: %s" % e)
    try:
        tn = unreal.AnimationLibrary.get_animation_track_names(clip)
        line("track_count: %s  first5=%s" % (len(tn), [str(x) for x in tn[:5]]))
    except Exception as e:
        line("track_names err: %s" % e)

# ---- 3. ABP_Worker copy in Locomotion ----
for ABP in ("/Game/Avariika/Anim/Locomotion/ABP_Worker", "/Game/FreeAnimationLibrary/ABP_Worker"):
    abp = None
    try:
        abp = unreal.load_asset(ABP)
    except Exception:
        pass
    if abp:
        line("--- ABP found: %s (class %s) ---" % (ABP, abp.get_class().get_name()))
        try:
            tsk = abp.get_editor_property("target_skeleton")
            line("  target_skeleton: %s" % (tsk.get_path_name() if tsk else None))
        except Exception as e:
            line("  target_skeleton err: %s" % e)

# ---- 4. Skeletons + compatible lists ----
SKM = "/Game/FreeAnimationLibrary/Demo/Characters/Mannequins/Meshes/SK_Mannequin"
QSK = "/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton"
for label, p in (("SK_Mannequin(FBL)", SKM), ("Quantum_Skeleton", QSK)):
    line("--- skeleton %s : %s ---" % (label, p))
    a = load(p)
    if not a:
        continue
    line("  loaded class: %s" % a.get_class().get_name())
    # if it's a SkeletalMesh, get its skeleton
    sk = a
    if a.get_class().get_name() == "SkeletalMesh":
        try:
            sk = a.get_editor_property("skeleton")
            line("  (skeletalmesh) skeleton: %s" % (sk.get_path_name() if sk else None))
        except Exception as e:
            line("  skel from mesh err: %s" % e)
    try:
        cs = sk.get_editor_property("compatible_skeletons")
        line("  compatible_skeletons count: %s" % (len(cs) if cs is not None else None))
        for c in (cs or []):
            try:
                line("    compat -> %s" % c.get_path_name())
            except Exception:
                line("    compat -> %s" % c)
    except Exception as e:
        line("  compatible_skeletons read err: %s" % e)

# ---- 5. What skeleton does the player BP mesh use ----
line("--- BP_AvaryoCharacter mesh ---")
bp = load("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
line("BP loaded: %s" % (bp is not None))

line("===== END DIAGNOSTIC =====")

try:
    with open("C:/unrealEngine/avariika/Scripts/diag_tpose_out.txt", "w", encoding="utf-8") as f:
        f.write("\n".join(_OUT))
except Exception as e:
    unreal.log_error("DIAG write fail: %s" % e)
