# -*- coding: utf-8 -*-
# Перед удалением пака уводим редактор с карты LV_Main_moy (она ВНУТРИ пака) на основную
# игровую карту, чтобы не держать удаляемые файлы. Останавливаем PIE если идёт.
import unreal, json
R = {"err": None}
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    # стоп PIE, если активна
    try:
        if unreal.EditorLevelLibrary.is_playing_in_editor():
            unreal.EditorLevelLibrary.editor_end_play()
            R["pie_stopped"] = True
    except Exception as pe:
        R["pie_note"] = str(pe)
    les.load_level("/Game/Avariika/Maps/Lvl_FirstPerson")
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world_now"] = w.get_name() if w else None
    R["ok"] = bool(w and "Lvl_FirstPerson" in w.get_name())
except Exception as e:
    R["err"] = str(e)
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_move_off.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("MOVE_OFF %s" % json.dumps(R, default=str))
