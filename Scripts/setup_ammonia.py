# -*- coding: utf-8 -*-
# Спавнит «Нашатырь» (PickupItem, ItemEffect=Ammonia) — будит тиммейта «без сознания»
# (оживляет) + сбивает панику. Этап 2 ранения. Идемпотентно + ГАРД на Lvl_FirstPerson.
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

    cube = unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
    pk_cls = unreal.load_class(None, "/Script/Avaryo.PickupItem")

    def name_of(a):
        try: return str(a.get_editor_property("display_name"))
        except Exception: return ""

    item = next((a for a in eas.get_all_level_actors()
                 if a.get_class().get_name() == "PickupItem" and "ашатыр" in name_of(a)), None)
    if item is None:
        # у полки снаряжения рядом с аптечкой
        item = eas.spawn_actor_from_class(pk_cls, unreal.Vector(-150.0, 200.0, 342.0), unreal.Rotator(0, 0, 0))
        item.set_actor_label("Ammonia")
        mmc = item.get_editor_property("mesh_component")
        if mmc and cube:
            mmc.set_static_mesh(cube); item.set_actor_scale3d(unreal.Vector(0.10, 0.10, 0.16))
        try: item.set_editor_property("item_effect", unreal.ItemEffect.AMMONIA)
        except Exception as e: R["steps"].append("effect err %s" % e)
        try: item.set_editor_property("display_name", "Нашатырь")
        except Exception as e: R["steps"].append("name err %s" % e)
        try: item.set_editor_property("item_size", unreal.ItemSize.LIGHT)
        except Exception as e: R["steps"].append("size err %s" % e)
        try: item.set_editor_property("charges", 3)
        except Exception as e: R["steps"].append("charges err %s" % e)
        R["steps"].append("нашатырь заспавнен")
    else:
        R["steps"].append("нашатырь уже есть — пропуск")

    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
os.makedirs(r"C:/unrealEngine/avariika/Scripts/manifests", exist_ok=True)
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_ammonia.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SETUP_AMMONIA %s" % json.dumps(R, default=str))
