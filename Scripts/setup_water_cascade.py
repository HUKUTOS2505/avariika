# -*- coding: utf-8 -*-
# Ставит сцену каскада «Прорыв воды 2.1» на Lvl_FirstPerson (новый путь /Game/Avariika/Maps/):
#  • труба-источник (Repairable, bFloodsWhenBroken) — заливает зону, зона под током;
#  • рубильник (APowerSwitch) на СУХОМ месте у спавна — обесточивает зону (порядок-наказание);
#  • резиновые сапоги (PickupItem ToolTag=RubberBoots) — диэлектрик, можно лезть в живую воду.
# Запускать ПОСЛЕ сборки (APowerSwitch — новый класс). Идемпотентно + ГАРД на Lvl_FirstPerson.
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
    by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

    def set_prop(actor, names, value):
        for n in names:
            try:
                actor.set_editor_property(n, value); return True
            except Exception:
                continue
        R["steps"].append("prop FAIL %s" % (names,)); return False

    # 1) Труба-источник потопа -------------------------------------------------
    pipe = by_label.get("Repairable_WaterPipe")
    if pipe is None:
        pipe = eas.spawn_actor_from_class(unreal.Repairable, unreal.Vector(600.0, 1400.0, 35.0), unreal.Rotator(0, 0, 0))
        pipe.set_actor_label("Repairable_WaterPipe")
        R["steps"].append("труба заспавнена")
    pipe.set_actor_location(unreal.Vector(600.0, 1400.0, 35.0), False, False)
    pipe.set_actor_scale3d(unreal.Vector(0.35, 0.35, 1.6))
    mc = pipe.get_editor_property("mesh_component")
    if mc and cube: mc.set_static_mesh(cube)
    set_prop(pipe, ["display_name"], "Прорыв трубы")
    set_prop(pipe, ["repair_duration"], 8.0)
    set_prop(pipe, ["b_floods_when_broken", "bFloodsWhenBroken"], True)
    set_prop(pipe, ["flood_radius", "FloodRadius"], 500.0)
    set_prop(pipe, ["flood_shock_damage", "FloodShockDamage"], 18.0)
    try: pipe.set_broken(True)
    except Exception as e: R["steps"].append("set_broken err %s" % e)
    R["pipe_flooding"] = bool(getattr(pipe, "is_flooding", lambda: None)())

    # 2) Рубильник на сухом месте у спавна ------------------------------------
    sw_cls = unreal.load_class(None, "/Script/Avaryo.PowerSwitch")
    if sw_cls is None:
        R["steps"].append("APowerSwitch ещё не собран — пропуск рубильника (запусти после сборки)")
    else:
        sw = by_label.get("WaterPowerSwitch")
        if sw is None:
            sw = eas.spawn_actor_from_class(sw_cls, unreal.Vector(250.0, 0.0, 235.0), unreal.Rotator(0, 0, 0))
            sw.set_actor_label("WaterPowerSwitch")
            R["steps"].append("рубильник заспавнен")
        sw.set_actor_location(unreal.Vector(250.0, 0.0, 235.0), False, False)

    # 3) Резиновые сапоги (пикап) ---------------------------------------------
    pk_cls = unreal.load_class(None, "/Script/Avaryo.PickupItem")
    def tag_of(a):
        try: return str(a.get_editor_property("tool_tag"))
        except Exception: return ""
    boots = next((a for a in eas.get_all_level_actors()
                  if a.get_class().get_name() == "PickupItem" and tag_of(a) == "RubberBoots"), None)
    if boots is None:
        boots = eas.spawn_actor_from_class(pk_cls, unreal.Vector(-150.0, 150.0, 342.0), unreal.Rotator(0, 0, 0))
        boots.set_actor_label("RubberBoots")
        bmc = boots.get_editor_property("mesh_component")
        if bmc and cube:
            bmc.set_static_mesh(cube); boots.set_actor_scale3d(unreal.Vector(0.22, 0.16, 0.12))
        boots.set_editor_property("tool_tag", "RubberBoots")
        set_prop(boots, ["display_name"], "Резиновые сапоги")
        try: boots.set_editor_property("item_size", unreal.ItemSize.LIGHT)
        except Exception as e: R["steps"].append("size err %s" % e)
        R["steps"].append("сапоги заспавнены")

    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_water_cascade.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SETUP_WATER_CASCADE %s" % json.dumps(R, default=str))
