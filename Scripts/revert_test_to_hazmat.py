# -*- coding: utf-8 -*-
# Возвращаем hazmat-персонажа для всего проекта: на карте Test ставим GameMode = BP_AvaryoGameMode
# (вместо учебного BP_Pawn без движения). Глобальный дефолт уже BP_AvaryoGameMode. BP_Pawn-ассеты НЕ удаляю.
import unreal, json, traceback
R = {"err": None, "steps": []}
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    avaryo = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoGameMode.BP_AvaryoGameMode_C")
    R["avaryo_gm_loaded"] = bool(avaryo)

    # 1) Test -> hazmat
    les.load_level("/Game/Avariika/Maps/Test")
    w = ues.get_editor_world()
    R["world"] = w.get_name() if w else None
    if w and "Test" in w.get_name():
        ws = w.get_world_settings()
        ws.set_editor_property("default_game_mode", avaryo)
        chk = ws.get_editor_property("default_game_mode")
        R["test_gm_after"] = chk.get_name() if chk else None
        les.save_current_level(); R["steps"].append("Test GM -> BP_AvaryoGameMode, saved")
    else:
        R["err"] = "ГАРД: не на Test"

    # 2) Вернуть редактор на боевую карту (с hazmat + сценой воды)
    les.load_level("/Game/Avariika/Maps/Lvl_FirstPerson")
    R["steps"].append("вернулся на Lvl_FirstPerson")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_revert_test.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("REVERT_TEST %s" % json.dumps(R, default=str))
