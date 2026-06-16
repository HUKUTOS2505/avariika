# -*- coding: utf-8 -*-
# Кладёт предмет «Газоанализатор» (ToolTag=GasDetector, лёгкий) на Lvl_FirstPerson.
# Держишь в руках → HUD показывает уровень газа рядом. Меш-заглушка (куб) до meshy-модели.
# Идемпотентно + ГАРД на Lvl_FirstPerson.
import unreal, json, traceback
R = {"err": None, "steps": []}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    w = ues.get_editor_world()
    R["map"] = w.get_path_name() if w else "?"
    if "Lvl_FirstPerson" not in R["map"]:
        R["err"] = "ABORT: не на Lvl_FirstPerson (%s)" % R["map"]; raise RuntimeError(R["err"])

    cls = unreal.load_class(None, "/Script/Avaryo.PickupItem")
    def tag_of(a):
        try: return str(a.get_editor_property("tool_tag"))
        except Exception: return ""
    existing = [a for a in eas.get_all_level_actors()
                if a.get_class().get_name() == "PickupItem" and tag_of(a) == "GasDetector"]
    if existing:
        R["steps"].append("GasDetector уже есть — пропуск")
    else:
        det = eas.spawn_actor_from_class(cls, unreal.Vector(150.0, -120.0, 342.0), unreal.Rotator(0, 0, 0))
        det.set_actor_label("GasDetector")
        cube = unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
        mc = det.get_editor_property("mesh_component")
        if mc and cube:
            mc.set_static_mesh(cube)
            det.set_actor_scale3d(unreal.Vector(0.18, 0.12, 0.08))
        det.set_editor_property("tool_tag", "GasDetector")
        try: det.set_editor_property("display_name", unreal.Text.as_culture_invariant("Газоанализатор"))
        except Exception:
            try: det.set_editor_property("display_name", "Газоанализатор")
            except Exception as e: R["steps"].append("dn err %s" % e)
        try: det.set_editor_property("item_size", unreal.ItemSize.LIGHT)
        except Exception as e: R["steps"].append("size err %s" % e)
        R["steps"].append("GasDetector заспавнен")
        les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_gas_detector.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SETUP_GAS_DETECTOR %s" % json.dumps(R, default=str))
