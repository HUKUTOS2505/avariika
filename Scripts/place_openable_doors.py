# -*- coding: utf-8 -*-
# Заменяем статичные распашные двери (SM_Door_01a/02a) в LV_Main_moy на ADoor (открываются по E).
# Пивот меша — на петле (X=0), поэтому ADoor в том же трансформе + меш с offset 0 = дверь на месте,
# распахивается вокруг петли. Гаражные/фасадные не трогаем. Оригиналы удаляем. Сохраняем.
import unreal, json, traceback
R = {"err": None, "placed": [], "skipped": []}
SWING = ("SM_Door_01a", "SM_Door_02a")
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.load_level("/Game/PostApocalypticHouse/Maps/LV_Main_moy")
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    w = ues.get_editor_world()
    if not w or "LV_Main_moy" not in w.get_name():
        R["err"] = "ГАРД: не на LV_Main_moy"; raise RuntimeError(R["err"])
    door_cls = unreal.load_class(None, "/Script/Avaryo.Door")
    if not door_cls:
        R["err"] = "ADoor не собран"; raise RuntimeError(R["err"])

    targets = []
    for a in eas.get_all_level_actors():
        smc = a.get_component_by_class(unreal.StaticMeshComponent) if a else None
        if not smc: continue
        sm = smc.get_editor_property("static_mesh")
        if sm and sm.get_name() in SWING:
            targets.append((a, sm, a.get_actor_location(), a.get_actor_rotation(), a.get_actor_scale3d(), a.get_actor_label()))

    for (a, sm, loc, rot, scl, label) in targets:
        d = eas.spawn_actor_from_class(door_cls, loc, rot)
        if not d:
            R["skipped"].append(label); continue
        d.set_actor_label("Door_" + label)
        d.set_actor_scale3d(scl)
        try: d.set_editor_property("leaf_offset_y", 0.0)
        except Exception: pass
        dm = d.get_editor_property("door_mesh")
        if dm:
            dm.set_static_mesh(sm)
            dm.set_editor_property("relative_location", unreal.Vector(0.0, 0.0, 0.0))
            dm.set_editor_property("relative_scale3d", unreal.Vector(1.0, 1.0, 1.0))
        zone = d.get_editor_property("zone")
        if zone:
            zone.set_editor_property("relative_location", unreal.Vector(56.0, 0.0, 108.0))
            zone.set_box_extent(unreal.Vector(70.0, 70.0, 110.0))
        eas.destroy_actor(a)  # убрать статичный оригинал
        R["placed"].append({"from": label, "mesh": sm.get_name()})
    R["count"] = len(R["placed"])
    les.save_current_level(); R["saved"] = True
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_place_doors.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("PLACE_DOORS %s" % json.dumps(R, default=str))
