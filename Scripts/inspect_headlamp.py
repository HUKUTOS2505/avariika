# -*- coding: utf-8 -*-
# Читаем настройки фонаря (Headlamp SpotLight) на BP_AvaryoCharacter — почему пересвет в белое.
import unreal, json, traceback
R = {"err": None, "spotlights": []}
spawned = None
try:
    bp = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    spawned = eas.spawn_actor_from_class(bp, unreal.Vector(0,0,300000.0), unreal.Rotator(0,0,0))
    if spawned:
        for c in spawned.get_components_by_class(unreal.SpotLightComponent):
            info = {"name": c.get_name()}
            for p in ("intensity", "attenuation_radius", "outer_cone_angle", "inner_cone_angle",
                      "use_inverse_squared_falloff", "source_radius", "volumetric_scattering_intensity"):
                try: info[p] = c.get_editor_property(p)
                except Exception as e: info[p] = "ERR:%s" % str(e)[:40]
            try: info["intensity_units"] = str(c.get_editor_property("intensity_units"))
            except Exception: pass
            try:
                col = c.get_editor_property("light_color"); info["color"] = [col.r, col.g, col.b]
            except Exception: pass
            R["spotlights"].append(info)
        # заодно точечные (если фонарь PointLight)
        for c in spawned.get_components_by_class(unreal.PointLightComponent):
            if c.get_class().get_name() == "SpotLightComponent": continue
            R["spotlights"].append({"name": c.get_name(), "class": "PointLight",
                                    "intensity": c.get_editor_property("intensity")})
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
finally:
    try:
        if spawned: unreal.get_editor_subsystem(unreal.EditorActorSubsystem).destroy_actor(spawned)
    except Exception: pass
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_headlamp.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("INSPECT_HEADLAMP %s" % json.dumps(R, default=str))
