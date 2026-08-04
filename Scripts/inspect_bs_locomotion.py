# Читает оси + сэмплы BS_Locomotion (что ABP_Worker уже использует).
import unreal
bs = unreal.load_asset("/Game/Avariika/Anim/Locomotion/BS_Locomotion.BS_Locomotion")
lines = []
if not bs:
    lines.append("BS_Locomotion НЕ найден")
else:
    try:
        bp = bs.get_editor_property("blend_parameters")
        for i, p in enumerate(bp):
            try:
                lines.append("axis[%d]: name='%s' min=%s max=%s grid=%s" % (
                    i, p.get_editor_property("display_name"), p.get_editor_property("min"),
                    p.get_editor_property("max"), p.get_editor_property("grid_num")))
            except Exception as e:
                lines.append("axis[%d] err %s" % (i, str(e)[:60]))
    except Exception as e:
        lines.append("blend_parameters err: %s" % str(e)[:80])
    try:
        sd = bs.get_editor_property("sample_data")
        lines.append("сэмплов: %d" % len(sd))
        for s in sd:
            try:
                a = s.get_editor_property("animation")
                v = s.get_editor_property("sample_value")
                lines.append("  (%.0f, %.0f) <- %s" % (v.x, v.y, a.get_name() if a else "None"))
            except Exception as e:
                lines.append("  sample err %s" % str(e)[:60])
    except Exception as e:
        lines.append("sample_data err: %s" % str(e)[:80])
with open(r"C:/unrealEngine/avariika/Scripts/bs_loco_inspect.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("done")
