# -*- coding: utf-8 -*-
# Импортирует кириллический шрифт (Arial из Windows) для 3D-табличек (TextRender).
# Дефолтный шрифт TextRender (RobotoDistanceField) — без кириллицы → прямоугольники.
import unreal, json, traceback
R = {"created": [], "err": None}
try:
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    t = unreal.AssetImportTask()
    t.set_editor_property("filename", "C:/Windows/Fonts/arial.ttf")
    t.set_editor_property("destination_path", "/Game/Avariika/UI/Fonts")
    t.set_editor_property("automated", True)
    t.set_editor_property("save", True)
    t.set_editor_property("replace_existing", True)
    tools.import_asset_tasks([t])
    paths = list(t.get_editor_property("imported_object_paths"))
    for p in paths:
        obj = unreal.load_asset(p)
        R["created"].append({"path": p, "class": obj.get_class().get_name() if obj else "?"})
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_import_font.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("IMPORT_FONT %s" % json.dumps(R, default=str))
