# -*- coding: utf-8 -*-
# Добавляет AExitZone (точку эвакуации/возврата на базу) на Lvl_FirstPerson, если её нет.
# Без неё забег после последней починки сразу заканчивается «Актом» — нет точки выхода.
# Идемпотентно + ГАРД: спавнит только если редактор реально на Lvl_FirstPerson.
import unreal, json, traceback
R = {"steps": [], "err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    w = ues.get_editor_world()
    R["map"] = w.get_path_name() if w else "?"
    if "Lvl_FirstPerson" not in R["map"]:
        R["err"] = "ABORT: редактор не на Lvl_FirstPerson (%s)" % R["map"]
        raise RuntimeError(R["err"])

    actors = eas.get_all_level_actors()
    existing = [a for a in actors if a.get_class().get_name() == "ExitZone"]
    if existing:
        R["steps"].append("ExitZone уже есть (%d) — пропуск" % len(existing))
    else:
        # Найти фургон/ГАЗель как место эвакуации, иначе — у PlayerStart
        anchor = None
        for a in actors:
            lbl = a.get_actor_label()
            if any(k in lbl for k in ("azelle", "Gazelle", "Van", "Truck", "Hilux")):
                anchor = a; break
        if not anchor:
            for a in actors:
                if a.get_class().get_name() == "PlayerStart":
                    anchor = a; break
        if anchor:
            loc = anchor.get_actor_location()
            loc = unreal.Vector(loc.x, loc.y, loc.z + 90.0)
            R["steps"].append("якорь: %s @ %s" % (anchor.get_actor_label(), loc))
        else:
            loc = unreal.Vector(-300.0, 0.0, 150.0)
            R["steps"].append("якорь не найден — фолбэк (-300,0,150)")

        cls = unreal.load_class(None, "/Script/Avaryo.ExitZone")
        if not cls:
            R["err"] = "класс ExitZone не найден (C++ не собран?)"
            raise RuntimeError(R["err"])
        ez = eas.spawn_actor_from_class(cls, loc, unreal.Rotator(0, 0, 0))
        ez.set_actor_label("ExitZone_FirstPerson")
        R["steps"].append("ExitZone заспавнен")
        les.save_current_level()
        R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_exitzone.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("ADD_EXITZONE done err=%s" % R["err"])
