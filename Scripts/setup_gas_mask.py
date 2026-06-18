# -*- coding: utf-8 -*-
# Спавнит предмет «Противогаз» (PickupItem, tool_tag=GasMask) — защита от газовой
# дыхательной опасности (см. AAvaryoCharacter::HasGasMask + газовая петля в ARepairable).
# Зеркало блока «резиновые сапоги» из setup_water_cascade.py.
# Идемпотентно (не дублирует, если GasMask уже есть) + ГАРД на Lvl_FirstPerson.
# Запуск (редактор открыт, активная карта = Lvl_FirstPerson):
#   UnrealEditor-Cmd по плагину editor.run_python_script, либо вручную в редакторе.
# Место спавна — у «полки снаряжения» рядом со спавном; переставь актора в редакторе как удобно.
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

    cube = unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
    pk_cls = unreal.load_class(None, "/Script/Avaryo.PickupItem")
    if pk_cls is None:
        R["err"] = "PickupItem класс не найден"; raise RuntimeError(R["err"])

    def tag_of(a):
        try: return str(a.get_editor_property("tool_tag"))
        except Exception: return ""

    mask = next((a for a in eas.get_all_level_actors()
                 if a.get_class().get_name() == "PickupItem" and tag_of(a) == "GasMask"), None)
    if mask is None:
        # рядом с сапогами (-150,150) — «полка снаряжения»; переставь как удобно
        mask = eas.spawn_actor_from_class(pk_cls, unreal.Vector(-150.0, 50.0, 342.0), unreal.Rotator(0, 0, 0))
        mask.set_actor_label("GasMask")
        mmc = mask.get_editor_property("mesh_component")
        if mmc and cube:
            mmc.set_static_mesh(cube); mask.set_actor_scale3d(unreal.Vector(0.20, 0.18, 0.16))
        mask.set_editor_property("tool_tag", "GasMask")
        try: mask.set_editor_property("display_name", "Противогаз")
        except Exception as e: R["steps"].append("name err %s" % e)
        try: mask.set_editor_property("item_size", unreal.ItemSize.LIGHT)
        except Exception as e: R["steps"].append("size err %s" % e)
        R["steps"].append("противогаз заспавнен")
    else:
        R["steps"].append("противогаз уже есть — пропуск")

    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
import os
os.makedirs(r"D:/unrealEngine/avariika/Scripts/manifests", exist_ok=True)
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_gas_mask.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SETUP_GAS_MASK %s" % json.dumps(R, default=str))
