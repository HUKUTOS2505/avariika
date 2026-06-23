# -*- coding: utf-8 -*-
# Проверяем перенесённую карту: грузим новый путь, считаем акторов (ждём 94).
import unreal, json, traceback
R = {"err": None, "expected": 94}
DST = "/Game/Avariika/Maps/Lvl_FirstPerson"
try:
    R["dst_asset_exists"] = unreal.EditorAssetLibrary.does_asset_exist(DST)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    loaded = les.load_level(DST)
    R["load_ok"] = bool(loaded)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world"] = w.get_path_name() if w else None
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = eas.get_all_level_actors()
    R["actor_count"] = len(actors)
    R["match"] = (len(actors) == R["expected"])
    # сверим характерные имена
    names = set(a.get_actor_label() for a in actors if a)
    R["has_key_actors"] = {
        "Repairable_Breaker": "Repairable_Breaker" in names,
        "Repairable_Generator": "Repairable_Generator" in names,
        "ExitZone_Gazelle": "ExitZone_Gazelle" in names,
        "PlayerStart": "PlayerStart" in names,
        "WeldingMachine": "WeldingMachine" in names,
    }
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_verify_moved.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("VERIFY_MOVED %s" % json.dumps(R, default=str))
