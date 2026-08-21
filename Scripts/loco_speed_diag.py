import unreal
_OUT=[]
def line(s=""):
    unreal.log("LOCO| "+str(s)); _OUT.append(str(s))

# ---- 1) speeds: walk +50% (150->225), sprint +20% (450->540) ----
gc = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
cdo = unreal.get_default_object(gc)
cdo.set_editor_property("base_walk_speed", 225.0)
cdo.set_editor_property("sprint_speed", 540.0)
line("speeds: walk=%s sprint=%s" % (cdo.get_editor_property("base_walk_speed"), cdo.get_editor_property("sprint_speed")))

# ---- 2) backup BS_Loco_Mobility (4-way) ----
SRC = "/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility"
BAK = "/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility_4way_bak"
if not unreal.EditorAssetLibrary.does_asset_exist(BAK):
    unreal.EditorAssetLibrary.duplicate_asset(SRC, BAK)
    line("backup created: %s" % BAK)
else:
    line("backup already exists")

# ---- 3) add 8 diagonal samples (full 8-way) ----
M = "/Game/Avariika/Anim/Locomotion/Mobility/"
def clip(n): return unreal.load_asset(M+n)
# (animation, direction_deg(X), speed(Y))
diag = [
    ("RT_MOB1_M1_Walk_FR_Loop_IP",        45, 150),
    ("RT_MOB1_M1_Walk_FL_Loop_IP",       -45, 150),
    ("RT_MOB1_M1_Walk_BR_BkPd_Loop_IP",  135, 150),
    ("RT_MOB1_M1_Walk_BL_BkPd_Loop_IP", -135, 150),
    ("RT_MOB1_M1_Jog_FR_Loop_IP",         45, 450),
    ("RT_MOB1_M1_Jog_FL_Loop_IP",        -45, 450),
    ("RT_MOB1_M1_Jog_BR_BkPd_Loop_IP",   135, 450),
    ("RT_MOB1_M1_Jog_BL_BkPd_Loop_IP",  -135, 450),
]
bs = unreal.load_asset(SRC)
sd = list(bs.get_editor_property("sample_data"))
line("samples before: %d" % len(sd))
existing = set()
for s in sd:
    a = s.get_editor_property("animation")
    if a: existing.add(a.get_name())
added=0
for name, x, y in diag:
    c = clip(name)
    if not c:
        line("  MISSING clip %s" % name); continue
    if c.get_name() in existing:
        line("  skip (already) %s" % name); continue
    smp = unreal.BlendSample()
    smp.set_editor_property("animation", c)
    smp.set_editor_property("sample_value", unreal.Vector(float(x), float(y), 0.0))
    try: smp.set_editor_property("rate_scale", 1.0)
    except Exception: pass
    sd.append(smp); added+=1
bs.set_editor_property("sample_data", sd)
line("added %d diagonals -> total %d" % (added, len(sd)))

# save BP + blendspace
pkgs = [cdo.get_outermost(), bs.get_outermost()]
ok = unreal.EditorLoadingAndSavingUtils.save_packages(pkgs, False)
line("save_packages -> %s" % ok)

with open("C:/unrealEngine/avariika/Scripts/loco_speed_diag_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
