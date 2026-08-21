import unreal
bs = unreal.load_asset("/Game/Avariika/Anim/Locomotion/BS_Loco_Mobility.BS_Loco_Mobility")
lines = []
if not bs:
    lines.append("BS_Loco_Mobility НЕ найден")
else:
    # оси
    for prop in ("blend_parameters",):
        try:
            bp = bs.get_editor_property(prop)
            lines.append("%s = %s" % (prop, bp))
        except Exception as e:
            lines.append("%s err: %s" % (prop, str(e)[:80]))
    # пробуем отдельные оси
    for ax in ("horizontal_axis","vertical_axis"):
        try:
            v = bs.get_editor_property(ax)
            lines.append("%s: name=%s min=%s max=%s" % (ax, v.display_name, v.min, v.max))
        except Exception as e:
            lines.append("%s err: %s" % (ax, str(e)[:80]))
    # кол-во сэмплов
    for sp in ("sample_data",):
        try:
            sd = bs.get_editor_property(sp)
            lines.append("%s: %d сэмплов" % (sp, len(sd)))
        except Exception as e:
            lines.append("%s err: %s" % (sp, str(e)[:80]))
    # все свойства с 'axis' или 'sample'
    lines.append("props: " + str([p for p in dir(bs) if 'axis' in p.lower() or 'sample' in p.lower() or 'parameter' in p.lower()][:20]))
with open(r"C:/unrealEngine/avariika/Scripts/bs_inspect.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("INSPECT done")
