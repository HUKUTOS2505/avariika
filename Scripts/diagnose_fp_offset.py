# -*- coding: utf-8 -*-
# Спавним BP_AvaryoCharacter в редакторский мир (на высоте, в пустоте), читаем относительные
# и мировые координаты всех scene-компонентов (капсула, тело, FP-меш, FP-камера, фонарь),
# чтобы найти, что смещено вбок от капсулы. Потом удаляем актора. Ничего не сохраняем в карту.
import unreal, json, traceback
R = {"err": None, "components": []}
try:
    gen = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
    if not gen:
        R["err"] = "BP class not found"; raise RuntimeError(R["err"])
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    origin = unreal.Vector(0.0, 0.0, 5000.0)
    a = eas.spawn_actor_from_class(gen, origin, unreal.Rotator(0.0, 0.0, 0.0))
    if not a:
        R["err"] = "spawn failed"; raise RuntimeError(R["err"])
    caps = a.get_component_by_class(unreal.CapsuleComponent)
    if caps:
        R["capsule"] = {"radius": caps.get_unscaled_capsule_radius(),
                        "halfheight": caps.get_unscaled_capsule_half_height()}
    for c in a.get_components_by_class(unreal.SceneComponent):
        try:
            rl = c.get_editor_property("relative_location")
            rr = c.get_editor_property("relative_rotation")
            wl = c.get_world_location()
        except Exception as ce:
            R["components"].append({"name": c.get_name(), "read_err": str(ce)}); continue
        parent = c.get_attach_parent()
        socket = ""
        try: socket = str(c.get_attach_socket_name())
        except Exception: pass
        R["components"].append({
            "name": c.get_name(),
            "class": c.get_class().get_name(),
            "parent": parent.get_name() if parent else None,
            "socket": socket,
            # мировое смещение от центра капсулы (актор стоит в origin): X вперёд, Y вбок(право+), Z вверх
            "world_off": [round(wl.x - origin.x, 1), round(wl.y - origin.y, 1), round(wl.z - origin.z, 1)],
            "rel_loc": [round(rl.x, 1), round(rl.y, 1), round(rl.z, 1)],
            "rel_rot_pyr": [round(rr.pitch, 1), round(rr.yaw, 1), round(rr.roll, 1)],
        })
    eas.destroy_actor(a)
    R["ok"] = True
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_fp_offset.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("FP_OFFSET %s" % json.dumps(R, default=str))
