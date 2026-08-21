# Собирает 2D-блендспейс BS_Loco_Mobility (Direction x Speed) из ретаргетнутых Mobility-клипов.
# Итог -> Scripts/blendspace_result.txt
import unreal
eal = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
lines = []
M = "/Game/Avariika/Anim/Locomotion/Mobility/"

def A(n):
    p = M + n
    a = unreal.load_asset(p + "." + n)
    return a

idle = A("RT_MOB1_M1_Stand_Relaxed_Idle_IP")
if not idle:
    lines.append("НЕТ idle-клипа");
skel = idle.get_editor_property("skeleton") if idle else None
lines.append("skeleton=%s" % (skel.get_name() if skel else "-"))

# создать blendspace
BSPATH = "/Game/Avariika/Anim/Locomotion"
bs = unreal.load_asset(BSPATH + "/BS_Loco_Mobility.BS_Loco_Mobility")
if bs is None:
    fac = unreal.BlendSpaceFactory()
    try: fac.set_editor_property("target_skeleton", skel)
    except Exception as e: lines.append("fac skel err " + str(e))
    bs = tools.create_asset("BS_Loco_Mobility", BSPATH, unreal.BlendSpace, fac)
lines.append("blendspace created=%s" % bool(bs))

# доступные методы (пробинг)
methods = [m for m in dir(unreal.BlendSpace) if 'ample' in m or 'xis' in m]
lines.append("BS methods: " + str(methods))
lib = [m for m in dir(unreal) if 'BlendSpace' in m]
lines.append("unreal BlendSpace libs: " + str(lib))

# оси
try:
    bs.set_editor_property("axis_to_scale_animation", unreal.BlendSpaceAxis.BSA_X)
except Exception as e: lines.append("axis scale err " + str(e))

# сэмплы: (clip, Direction X, Speed Y)
SAMPLES = [
    ("RT_MOB1_M1_Stand_Relaxed_Idle_IP", 0, 0),
    ("RT_MOB1_M1_Walk_F_IP", 0, 150), ("RT_MOB1_M1_Walk_R_IP", 90, 150),
    ("RT_MOB1_M1_Walk_L_IP", -90, 150), ("RT_MOB1_M1_Walk_B_IP", 180, 150), ("RT_MOB1_M1_Walk_B_IP", -180, 150),
    ("RT_MOB1_M1_Jog_F_IP", 0, 450), ("RT_MOB1_M1_Jog_R_IP", 90, 450),
    ("RT_MOB1_M1_Jog_L_IP", -90, 450), ("RT_MOB1_M1_Jog_B_IP", 180, 450), ("RT_MOB1_M1_Jog_B_IP", -180, 450),
]
added = 0
helper = None
for cand in ("AnimationBlendSpaceSampleLibrary", "BlendSpaceLibrary"):
    if hasattr(unreal, cand): helper = getattr(unreal, cand); break
lines.append("sample helper: " + str(helper))
for nm, x, y in SAMPLES:
    a = A(nm)
    if not a: lines.append("  нет клипа " + nm); continue
    ok = False
    try:
        if helper and hasattr(helper, "add_blend_space_sample"):
            helper.add_blend_space_sample(bs, a, unreal.Vector(x, y, 0)); ok = True
    except Exception as e:
        lines.append("  add err %s: %s" % (nm, str(e)[:80]))
    if not ok:
        try:
            bs.add_sample(a, unreal.Vector(x, y, 0)); ok = True
        except Exception as e:
            lines.append("  add_sample err %s: %s" % (nm, str(e)[:80]))
    if ok: added += 1
lines.append("сэмплов добавлено: %d/%d" % (added, len(SAMPLES)))
eal.save_loaded_asset(bs, False)
with open(r"C:/unrealEngine/avariika/Scripts/blendspace_result.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(lines) + "\n")
print("BLENDSPACE done")
