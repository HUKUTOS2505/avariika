# -*- coding: utf-8 -*-
# Включает «повторный выбив» на щитке (Repairable_Breaker, Cursor): починка при поданном
# питании → КЗ вместо успеха (надо сперва обесточить рубильником). Ставит флаг
# b_circuit_breaker_shorts_if_panel_live=true. Заодно репортит рубильники и их питание.
# Идемпотентно + ГАРД на Lvl_FirstPerson.
import unreal, json, os, traceback
R = {"err": None, "steps": [], "switches": [], "breakers": []}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    w = ues.get_editor_world()
    R["map"] = w.get_path_name() if w else "?"
    if "Lvl_FirstPerson" not in (R["map"] or ""):
        R["err"] = "ABORT: не на Lvl_FirstPerson (%s)" % R["map"]; raise RuntimeError(R["err"])

    for a in eas.get_all_level_actors():
        cn = a.get_class().get_name()
        if cn == "Repairable":
            lbl = a.get_actor_label()
            mt = None
            try: mt = str(a.get_editor_property("minigame_type"))
            except Exception: pass
            is_cursor = (mt is not None and "CURSOR" in mt.upper()) or ("Breaker" in lbl)
            if is_cursor:
                readback = "?"
                try:
                    a.set_editor_property("bCircuitBreakerShortsIfPanelLive", True)
                    readback = bool(a.get_editor_property("bCircuitBreakerShortsIfPanelLive"))
                    R["steps"].append("щиток %s: повторный выбив ВКЛ (read-back=%s)" % (lbl, readback))
                except Exception as e:
                    R["steps"].append("set flag err on %s: %s" % (lbl, e))
                ep = "?"
                try: ep = bool(a.get_editor_property("bElectricallyPowered"))
                except Exception: pass
                R["breakers"].append({"label": lbl, "minigame": mt, "electrically_powered": ep, "flag_readback": readback})
        elif cn == "PowerSwitch":
            pw = "?"
            try: pw = bool(a.get_editor_property("bPowerOn"))
            except Exception: pass
            R["switches"].append({"label": a.get_actor_label(), "power_on": pw})

    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
os.makedirs(r"C:/unrealEngine/avariika/Scripts/manifests", exist_ok=True)
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_breaker_short.json", "w") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("SETUP_BREAKER_SHORT %s" % json.dumps(R, default=str, ensure_ascii=False))
