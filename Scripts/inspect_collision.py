# -*- coding: utf-8 -*-
# Почему в 1-м лице не зайти в дверь, а в 3-м — да. Капсула одна на оба вида, значит ищем:
# коллизию на мешах (FP-меш блокирует?) или габариты капсулы/позицию FP-камеры.
import unreal, json, traceback
R = {"err": None}
spawned = None
try:
    bp = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    spawned = eas.spawn_actor_from_class(bp, unreal.Vector(0,0,400000.0), unreal.Rotator(0,0,0))
    if spawned:
        cap = spawned.get_component_by_class(unreal.CapsuleComponent)
        if cap:
            R["capsule_half_height"] = round(float(cap.get_unscaled_capsule_half_height()),1)
            R["capsule_radius"] = round(float(cap.get_unscaled_capsule_radius()),1)
            try: R["capsule_collision"] = str(cap.get_collision_enabled())
            except Exception: pass
        for c in spawned.get_components_by_class(unreal.SkeletalMeshComponent):
            info = {"name": c.get_name()}
            try: info["collision_enabled"] = str(c.get_collision_enabled())
            except Exception as e: info["ce_err"] = str(e)
            try: info["collision_profile"] = str(c.get_collision_profile_name())
            except Exception: pass
            R.setdefault("skeletals", []).append(info)
        for c in spawned.get_components_by_class(unreal.CameraComponent):
            rl = c.get_editor_property("relative_location")
            R.setdefault("cameras", []).append({"name": c.get_name(), "rel": [round(rl.x,1),round(rl.y,1),round(rl.z,1)]})
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
finally:
    try:
        if spawned: unreal.get_editor_subsystem(unreal.EditorActorSubsystem).destroy_actor(spawned)
    except Exception: pass
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_collision.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("INSPECT_COLLISION %s" % json.dumps(R, default=str))
