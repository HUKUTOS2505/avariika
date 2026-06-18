# -*- coding: utf-8 -*-
# Ставит открывающуюся дверь (ADoor) на Lvl_FirstPerson с реальным дверным полотном
# (SM_InsideDoor, пивот на петле). Идемпотентно + ГАРД на Lvl_FirstPerson.
# Спавн у спавн-зоны на полу (z=-15) — ПОДВИНЬ актора в дверной проём в редакторе.
import unreal, json, os, traceback
R = {"err": None, "steps": []}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    w = ues.get_editor_world()
    R["map"] = w.get_path_name() if w else "?"
    if "Lvl_FirstPerson" not in (R["map"] or ""):
        R["err"] = "ABORT: не на Lvl_FirstPerson (%s)" % R["map"]; raise RuntimeError(R["err"])

    door_cls = unreal.load_class(None, "/Script/Avaryo.Door")
    if door_cls is None:
        R["err"] = "ADoor класс не найден"; raise RuntimeError(R["err"])

    existing = next((a for a in eas.get_all_level_actors()
                     if a.get_class().get_name() == "Door"), None)
    if existing:
        R["steps"].append("дверь уже есть (%s) — пропуск спавна" % existing.get_actor_label())
        door = existing
    else:
        door = eas.spawn_actor_from_class(door_cls, unreal.Vector(300.0, 0.0, -15.0), unreal.Rotator(0, 0, 0))
        door.set_actor_label("Door_Test")
        R["steps"].append("дверь заспавнена @ (300,0,-15)")

    # назначить полотно SM_InsideDoor на компонент DoorMesh
    leaf = unreal.load_asset("/Game/ResidentialHouses/Meshes/Architecture/Doors/SM_InsideDoor")
    R["leaf_loaded"] = bool(leaf)
    dm = None
    try: dm = door.get_editor_property("door_mesh")
    except Exception: pass
    if dm is None:
        for c in door.get_components_by_class(unreal.StaticMeshComponent):
            if "Door" in c.get_name():
                dm = c; break
    if dm and leaf:
        dm.set_static_mesh(leaf)
        try: dm.set_relative_scale3d(unreal.Vector(1.0, 1.0, 1.0))  # сброс масштаба куба-плейсхолдера (была «палка»)
        except Exception as e: R["steps"].append("scale reset err %s" % e)
        R["steps"].append("полотно SM_InsideDoor + масштаб 1.0 назначено на %s" % dm.get_name())
    else:
        R["steps"].append("DoorMesh не найден или меш не загрузился (dm=%s)" % (dm.get_name() if dm else None))

    # звук открытия на ИНСТАНС (CDO-дефолт не обновит уже стоящую дверь)
    snd = unreal.load_asset("/Game/Audio/Lib/door_impact/Ghosthack-SF_Household_Cabinet_Cupboard_Slide_Door_Close_01")
    if snd:
        try: door.set_editor_property("open_sound", snd); R["steps"].append("open_sound (household) назначен")
        except Exception as e: R["steps"].append("open_sound err %s" % e)

    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
os.makedirs(r"D:/unrealEngine/avariika/Scripts/manifests", exist_ok=True)
open(r"D:/unrealEngine/avariika/Scripts/manifests/_door.json", "w").write(json.dumps(R, indent=1, default=str))
unreal.log("SETUP_DOOR %s" % json.dumps(R, default=str))
