# -*- coding: utf-8 -*-
# Создаёт RUNTIME UFont (AvCyrFont) поверх импортированного FontFace arial — он рендерит
# ЛЮБЫЕ глифы (кириллица) по запросу, в т.ч. для TextRender (3D-таблички).
import unreal, json, traceback
R = {"err": None, "steps": []}
try:
    ff = unreal.load_asset("/Game/Avariika/UI/Fonts/arial.arial")
    R["fontface"] = ff.get_class().get_name() if ff else None
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    font = tools.create_asset("AvCyrFont", "/Game/Avariika/UI/Fonts", unreal.Font, unreal.FontFactory())
    R["font"] = font.get_class().get_name() if font else None
    font.set_editor_property("font_cache_type", unreal.FontCacheType.RUNTIME)
    R["steps"].append("cache=RUNTIME")

    fd = unreal.FontData()
    fd.set_editor_property("font_face_asset", ff)
    entry = unreal.TypefaceEntry()
    entry.set_editor_property("name", unreal.Name("Default"))
    entry.set_editor_property("font", fd)

    cf = font.get_editor_property("composite_font")
    tf = cf.get_editor_property("default_typeface")
    tf.set_editor_property("fonts", [entry])
    cf.set_editor_property("default_typeface", tf)
    font.set_editor_property("composite_font", cf)
    R["steps"].append("typeface set")

    # верификация: прочитать обратно
    tf2 = font.get_editor_property("composite_font").get_editor_property("default_typeface")
    R["typeface_entries"] = len(tf2.get_editor_property("fonts"))

    unreal.EditorAssetLibrary.save_asset("/Game/Avariika/UI/Fonts/AvCyrFont")
    R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_create_font.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("CREATE_FONT %s" % json.dumps(R, default=str))
