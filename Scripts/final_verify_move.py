# -*- coding: utf-8 -*-
# Финальная проверка после переноса+пересборки: какой мир открыт, сколько акторов, GameMode override.
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world_path"] = w.get_path_name() if w else None
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    R["actor_count"] = len(eas.get_all_level_actors())
    ws = w.get_world_settings() if w else None
    if ws:
        gm = ws.get_editor_property("default_game_mode")
        R["gamemode_override"] = gm.get_name() if gm else None
    # старый путь больше не должен существовать
    R["old_exists"] = unreal.EditorAssetLibrary.does_asset_exist("/Game/FirstPerson/Lvl_FirstPerson")
    R["new_exists"] = unreal.EditorAssetLibrary.does_asset_exist("/Game/Avariika/Maps/Lvl_FirstPerson")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_final_verify.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("FINAL_VERIFY %s" % json.dumps(R, default=str))
