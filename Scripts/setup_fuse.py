# -*- coding: utf-8 -*-
# Спавнит «Предохранитель» (PickupItem, tool_tag=Fuse) — расходник для щитка
# (этап InsertItem Fuse). Без него щиток не доделать. Идемпотентно + ГАРД на Lvl_FirstPerson.
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

    def tag_of(a):
        try: return str(a.get_editor_property("tool_tag"))
        except Exception: return ""

    item = next((a for a in eas.get_all_level_actors()
                 if a.get_class().get_name() == "PickupItem" and tag_of(a) == "Fuse"), None)
    if item is None:
        # рядом с кабелем/тестером (щиток ремонтный кластер)
        item = eas.spawn_actor_from_class(pk_cls, unreal.Vector(600.0, 280.0, 250.0), unreal.Rotator(0, 0, 0))
        item.set_actor_label("Fuse")
        mmc = item.get_editor_property("mesh_component")
        if mmc and cube:
            mmc.set_static_mesh(cube); item.set_actor_scale3d(unreal.Vector(0.14, 0.10, 0.10))
        item.set_editor_property("tool_tag", "Fuse")
        try: item.set_editor_property("display_name", "Предохранитель")
        except Exception as e: R["steps"].append("name err %s" % e)
        try: item.set_editor_property("item_size", unreal.ItemSize.LIGHT)
        except Exception as e: R["steps"].append("size err %s" % e)
        R["steps"].append("предохранитель заспавнен")
    else:
        R["steps"].append("предохранитель уже есть — пропуск")

    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
os.makedirs(r"C:/unrealEngine/avariika/Scripts/manifests", exist_ok=True)
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_fuse.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SETUP_FUSE %s" % json.dumps(R, default=str))
