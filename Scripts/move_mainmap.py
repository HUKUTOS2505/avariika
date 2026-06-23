# -*- coding: utf-8 -*-
# Переносим главную карту в /Game/Avariika/Maps/ (имя сохраняем). Проверяем число акторов (ждём 94).
import unreal, json, traceback
R = {"err": None, "steps": [], "expected_actors": 94}
SRC = "/Game/FirstPerson/Lvl_FirstPerson"
DST = "/Game/Avariika/Maps/Lvl_FirstPerson"
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    # 1) уйти с главной карты на Test, чтобы её можно было переименовать
    les.load_level("/Game/Avariika/Maps/Test")
    R["steps"].append("loaded Test")
    # 2) перенос ассета уровня (рассчитываем, что внешние акторы OFPA переедут вместе с ним)
    R["already_exists_dst"] = unreal.EditorAssetLibrary.does_asset_exist(DST)
    ok = unreal.EditorAssetLibrary.rename_asset(SRC, DST)
    R["rename_ok"] = bool(ok)
    R["dst_exists"] = unreal.EditorAssetLibrary.does_asset_exist(DST)
    R["src_still_exists"] = unreal.EditorAssetLibrary.does_asset_exist(SRC)
    # 3) загрузить новую карту и посчитать акторов
    les.load_level(DST)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world_after"] = w.get_path_name() if w else None
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = eas.get_all_level_actors()
    R["actor_count_after"] = len(actors)
    R["match"] = (len(actors) == R["expected_actors"])
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_move_mainmap.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("MOVE_MAINMAP %s" % json.dumps(R, default=str))
