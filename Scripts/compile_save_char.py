# -*- coding: utf-8 -*-
# Компилируем + сохраняем BP_AvaryoCharacter, чтобы изменение интенсивности фонаря (900) точно прижилось.
import unreal, json, traceback
R = {"err": None}
try:
    path = "/Game/Avariika/Blueprints/BP_AvaryoCharacter"
    bp = unreal.load_asset(path)
    R["loaded"] = bool(bp)
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp); R["compiled"] = True
    except Exception as e:
        R["compile_err"] = str(e)
    R["saved"] = bool(unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False))
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_compile_char.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("COMPILE_CHAR %s" % json.dumps(R, default=str))
