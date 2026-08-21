# Пытается программно задать оси (blend_parameters) + сэмплы (sample_data) у BS_Loco_Mobility.
# Всё в try, всегда пишет результат -> Scripts/bs_samples.txt
import unreal
eal = unreal.EditorAssetLibrary
BS = "/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility"
M = "/Game/Avariika/Anim/Locomotion/Mobility/"
lines = []
bs = unreal.load_asset(BS + ".BS_Loco_Mobility")
lines.append("bs loaded: %s" % bool(bs))

# --- пробинг типов ---
lines.append("BlendSample exists: %s" % hasattr(unreal, "BlendSample"))
lines.append("BlendParameter exists: %s" % hasattr(unreal, "BlendParameter"))
lines.append("InterpolationParameter exists: %s" % hasattr(unreal, "InterpolationParameter"))

# --- оси через blend_parameters ---
def mk_param(name, mn, mx, grid):
    p = unreal.BlendParameter()
    try: p.set_editor_property("display_name", name)
    except Exception: pass
    try: p.set_editor_property("min", float(mn))
    except Exception: pass
    try: p.set_editor_property("max", float(mx))
    except Exception: pass
    try: p.set_editor_property("grid_num", int(grid))
    except Exception: pass
    return p
try:
    h = mk_param("Direction", -180, 180, 4)
    v = mk_param("Speed", 0, 450, 3)
    # blend_parameters — обычно фикс. массив из 3 (X,Y,Z); ставим X и Y
    try:
        bs.set_editor_property("blend_parameters", [h, v, mk_param("None", 0, 100, 4)])
        lines.append("blend_parameters set (3)")
    except Exception as e1:
        try:
            bs.set_editor_property("blend_parameters", [h, v])
            lines.append("blend_parameters set (2)")
        except Exception as e2:
            lines.append("blend_parameters err: %s | %s" % (str(e1)[:60], str(e2)[:60]))
except Exception as e:
    lines.append("axis build err: %s" % str(e)[:100])

# --- сэмплы ---
SAMPLES = [
    ("RT_MOB1_M1_Stand_Relaxed_Idle_IP", 0, 0),
    ("RT_MOB1_M1_Walk_F_IP", 0, 150), ("RT_MOB1_M1_Walk_R_IP", 90, 150),
    ("RT_MOB1_M1_Walk_L_IP", -90, 150), ("RT_MOB1_M1_Walk_B_IP", 180, 150), ("RT_MOB1_M1_Walk_B_IP", -180, 150),
    ("RT_MOB1_M1_Jog_F_IP", 0, 450), ("RT_MOB1_M1_Jog_R_IP", 90, 450),
    ("RT_MOB1_M1_Jog_L_IP", -90, 450), ("RT_MOB1_M1_Jog_B_IP", 180, 450), ("RT_MOB1_M1_Jog_B_IP", -180, 450),
]
samples = []
for nm, x, y in SAMPLES:
    a = unreal.load_asset(M + nm + "." + nm)
    if not a:
        lines.append("  нет клипа " + nm); continue
    try:
        s = unreal.BlendSample()
        s.set_editor_property("animation", a)
        s.set_editor_property("sample_value", unreal.Vector(x, y, 0))
        samples.append(s)
    except Exception as e:
        lines.append("  sample err %s: %s" % (nm, str(e)[:60]))
lines.append("сконструировано сэмплов: %d" % len(samples))
try:
    bs.set_editor_property("sample_data", samples)
    lines.append("sample_data set OK")
except Exception as e:
    lines.append("sample_data set err: %s" % str(e)[:100])
# проверка
try:
    lines.append("итог sample_data: %d" % len(bs.get_editor_property("sample_data")))
except Exception: pass
eal.save_loaded_asset(bs, False)
with open(r"C:/unrealEngine/avariika/Scripts/bs_samples.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("BS_SAMPLES done")
