# -*- coding: utf-8 -*-
# Настраиваем УЧЕБНУЮ карту Test: BP_GameMode (с BP_Pawn + BP_PlayerController) как override.
# ГАРД: меняем World Settings только если загруженный мир — действительно Test.
import unreal, json, traceback
R = {"err": None, "steps": []}
base = "/Game/Avariika/blueprinsTest/"
try:
    gm_gen = unreal.load_object(None, base + "BP_GameMode.BP_GameMode_C")
    pawn_gen = unreal.load_object(None, base + "BP_Pawn.BP_Pawn_C")
    pc_gen = unreal.load_object(None, base + "BP_PlayerController.BP_PlayerController_C")
    R["loaded"] = {"gm": bool(gm_gen), "pawn": bool(pawn_gen), "pc": bool(pc_gen)}

    # 1) BP_GameMode: назначаем pawn + controller на CDO, сохраняем ассет.
    gm_cdo = unreal.get_default_object(gm_gen)
    gm_cdo.set_editor_property("default_pawn_class", pawn_gen)
    gm_cdo.set_editor_property("player_controller_class", pc_gen)
    R["gm_pawn_after"] = gm_cdo.get_editor_property("default_pawn_class").get_name()
    R["gm_pc_after"] = gm_cdo.get_editor_property("player_controller_class").get_name()
    saved_gm = unreal.EditorAssetLibrary.save_asset(base + "BP_GameMode", only_if_is_dirty=False)
    R["steps"].append("saved BP_GameMode=%s" % saved_gm)

    # 2) Загружаем карту Test и ставим override (с гардом).
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.load_level("/Game/Avariika/Maps/Test")
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world_after_load"] = w.get_path_name() if w else None
    if not w or "Test" not in w.get_name():
        R["err"] = "ГАРД: загруженный мир не Test (%s) — override НЕ ставлю" % (w.get_name() if w else None)
    else:
        ws = w.get_world_settings()
        ws.set_editor_property("default_game_mode", gm_gen)
        chk = ws.get_editor_property("default_game_mode")
        R["test_override_after"] = chk.get_name() if chk else None
        saved_lvl = les.save_current_level()
        R["steps"].append("saved Test level=%s" % saved_lvl)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_set_test_gamemode.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SET_TEST_GAMEMODE %s" % json.dumps(R, default=str))
