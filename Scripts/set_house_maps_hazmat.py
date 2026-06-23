# -*- coding: utf-8 -*-
# Ставим GameMode=BP_AvaryoGameMode (hazmat) на карты дома, чтобы играть оператором, а не кубом.
# Пошаговый лог. Каждая карта: load -> set World Settings GM -> save. В конце вернуться на Lvl_FirstPerson.
import unreal, json, traceback
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
avaryo = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoGameMode.BP_AvaryoGameMode_C")
LOG = r"C:/unrealEngine/avariika/Scripts/manifests/_house_gm_progress.txt"
def L(s):
    with open(LOG, "a", encoding="utf-8") as f: f.write(s+"\n")
MAPS = ["LV_Main", "LV_House", "LV_Horror_Light", "LV_Cinematic", "LV_Sunset_Light"]
R = {"done": [], "skip": [], "err": None}
L("=== SET HOUSE GM (hazmat) ===")
try:
    for nm in MAPS:
        path = "/Game/PostApocalypticHouse/Maps/" + nm
        if not unreal.EditorAssetLibrary.does_asset_exist(path):
            R["skip"].append(nm); L("нет карты %s" % nm); continue
        les.load_level(path)
        w = ues.get_editor_world()
        if not w or nm not in w.get_name():
            R["skip"].append(nm); L("ГАРД не прошёл %s (мир=%s)" % (nm, w.get_name() if w else None)); continue
        ws = w.get_world_settings()
        ws.set_editor_property("default_game_mode", avaryo)
        ok = les.save_current_level()
        R["done"].append({"map": nm, "saved": bool(ok)})
        L("OK %s saved=%s" % (nm, ok))
    les.load_level("/Game/Avariika/Maps/Lvl_FirstPerson")
    L("вернулся на Lvl_FirstPerson")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc(); L("EXC %s" % e)
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_house_gm.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("SET_HOUSE_GM done")
