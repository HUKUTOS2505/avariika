# -*- coding: utf-8 -*-
# Спавним BP_AvaryoCharacter ДАЛЕКО в редакторском мире, читаем иерархию компонентов
# (родитель/сокет/отн.позиция/owner_no_see), затем УДАЛЯЕМ. Уровень НЕ сохраняем.
import unreal, json, traceback
R = {"err": None, "comps": []}
spawned = None
try:
    bp = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    loc = unreal.Vector(0.0, 0.0, 200000.0)  # далеко вверх, чтобы ничего не задеть
    spawned = eas.spawn_actor_from_class(bp, loc, unreal.Rotator(0,0,0))
    R["spawned"] = spawned.get_name() if spawned else None
    if spawned:
        cap = spawned.get_component_by_class(unreal.CapsuleComponent)
        if cap:
            try: R["capsule_half_height"] = round(float(cap.get_unscaled_capsule_half_height()),1)
            except Exception: pass
        try:
            move = spawned.get_component_by_class(unreal.CharacterMovementComponent)
            if move:
                R["crouched_half_height"] = round(float(move.get_editor_property("crouched_half_height")),1)
        except Exception as e: R["move_err"] = str(e)
        for c in spawned.get_components_by_class(unreal.SceneComponent):
            info = {"name": c.get_name(), "class": c.get_class().get_name()}
            try:
                par = c.get_attach_parent(); info["parent"] = par.get_name() if par else None
            except Exception: pass
            try:
                s = c.get_attach_socket_name(); s = str(s)
                info["socket"] = s if s and s != "None" else None
            except Exception: pass
            try:
                rl = c.get_relative_location(); info["rel"] = [round(rl.x,1), round(rl.y,1), round(rl.z,1)]
            except Exception: pass
            try:
                info["use_pawn_ctrl_rot"] = bool(c.get_editor_property("use_pawn_control_rotation"))
            except Exception: pass
            try:
                info["owner_no_see"] = bool(c.get_editor_property("owner_no_see"))
                info["only_owner_see"] = bool(c.get_editor_property("only_owner_see"))
            except Exception: pass
            R["comps"].append(info)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
finally:
    try:
        if spawned:
            unreal.get_editor_subsystem(unreal.EditorActorSubsystem).destroy_actor(spawned)
            R["destroyed"] = True
    except Exception as e:
        R["destroy_err"] = str(e)
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_bp_hierarchy.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("BP_HIERARCHY %s" % json.dumps(R, default=str))
