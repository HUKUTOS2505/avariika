# -*- coding: utf-8 -*-
# Грузим LV_Main_moy, ищем статичные двери (SM_Door*), снимаем их трансформы + ПИВОТ меша
# (по bounding box: смещён ли центр от origin → петля на краю или в центре). Пока без изменений.
import unreal, json, traceback
R = {"err": None, "doors": []}
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.load_level("/Game/PostApocalypticHouse/Maps/LV_Main_moy")
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world"] = w.get_name() if w else None
    if not w or "LV_Main_moy" not in w.get_name():
        R["err"] = "ГАРД: не на LV_Main_moy"; raise RuntimeError(R["err"])
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        smc = a.get_component_by_class(unreal.StaticMeshComponent) if a else None
        if not smc: continue
        sm = smc.get_editor_property("static_mesh")
        if not sm: continue
        nm = sm.get_name()
        if "Door" not in nm or "Frame" in nm or "Cut" in nm: continue
        loc = a.get_actor_location(); rot = a.get_actor_rotation()
        info = {"label": a.get_actor_label(), "mesh": nm,
                "loc": [round(loc.x),round(loc.y),round(loc.z)],
                "rot_yaw": round(rot.yaw,1)}
        try:
            b = sm.get_bounds()  # FBoxSphereBounds
            o = b.origin; e = b.box_extent
            info["bounds_origin"] = [round(o.x,1),round(o.y,1),round(o.z,1)]
            info["bounds_extent"] = [round(e.x,1),round(e.y,1),round(e.z,1)]
        except Exception as e2:
            info["bounds_err"] = str(e2)
        R["doors"].append(info)
    R["door_count"] = len(R["doors"])
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_doors_in_map.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("INSPECT_DOORS %s" % json.dumps(R, default=str))
