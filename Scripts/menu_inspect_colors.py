import unreal
import traceback

path = "/Game/OGMainMenu/UI/CoreUI/Text/CTS_Large_Color"
out = "D:/unrealEngine/avariika/Saved/menu_colors.txt"
lines = []

try:
    bp = unreal.load_asset(path)
    lines.append("load_asset class: %s" % (bp.get_class().get_name() if bp else "None"))

    gc = None
    # method 1: generated_class()
    try:
        gc = bp.generated_class()
        lines.append("generated_class(): %s" % (gc.get_name() if gc else "None"))
    except Exception as e:
        lines.append("generated_class() ERR: %s" % e)
    # method 2: get_editor_property
    if not gc:
        try:
            gc = bp.get_editor_property("generated_class")
            lines.append("prop generated_class: %s" % (gc.get_name() if gc else "None"))
        except Exception as e:
            lines.append("prop generated_class ERR: %s" % e)

    if gc:
        cdo = unreal.get_default_object(gc)
        lines.append("super: %s" % gc.get_super_class().get_name())
        # try reading Color (FSlateColor)
        try:
            sc = cdo.get_editor_property("Color")
            lc = sc.get_editor_property("specified_color")
            lines.append("Color: R%.3f G%.3f B%.3f A%.3f" % (lc.r, lc.g, lc.b, lc.a))
        except Exception as e:
            lines.append("Color read ERR: %s" % e)
except Exception:
    lines.append("EXC: " + traceback.format_exc())

with open(out, "w", encoding="utf-8") as f:
    f.write("\n".join(lines))
print("\n".join(lines))
