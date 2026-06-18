# -*- coding: utf-8 -*-
# Проверяем рубильник в живом PIE: переключается ли bPowerOn и СНИМАЕТ ли ток с трубы-потопа.
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен — нажми Play"
    else:
        sw = None; pipe = None
        for a in unreal.GameplayStatics.get_all_actors_of_class(gw, unreal.PowerSwitch):
            sw = a; break
        for a in unreal.GameplayStatics.get_all_actors_of_class(gw, unreal.Repairable):
            try:
                if a.is_flooding(): pipe = a; break
            except Exception: pass
        if sw:
            R["switch_found"] = sw.get_actor_label()
            try: R["power_on"] = bool(sw.is_power_on())
            except Exception as e: R["power_err"] = str(e)
            # цвет лампы
            for c in sw.get_components_by_class(unreal.PointLightComponent):
                try:
                    col = c.get_light_color()
                    R["light_color"] = [round(col.r,2), round(col.g,2), round(col.b,2)]
                except Exception as e: R["color_err"] = str(e)
        else:
            R["switch_found"] = None
        if pipe:
            R["pipe"] = pipe.get_actor_label()
            try: R["flood_electrified"] = bool(pipe.is_flood_electrified())
            except Exception as e: R["fe_err"] = str(e)
        # ВЫВОД: power_on должен совпадать с flood_electrified (рубильник управляет током)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_powerswitch.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("INSPECT_POWERSWITCH %s" % json.dumps(R, default=str))
