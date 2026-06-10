# @claudius category: fps_starter
# @claudius description: Spawn a weapon pickup placeholder (cube with a label).
# @claudius workflow: fps, combat, level

import unreal


def execute(params):
    name = params.get("name", "Pickup")
    weapon_class = params.get("weapon_class", "AR")
    location = params.get("location", {"x": 0.0, "y": 0.0, "z": 60.0})

    sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    loc = unreal.Vector(location["x"], location["y"], location["z"])
    actor = sub.spawn_actor_from_class(unreal.StaticMeshActor, loc)
    if not actor:
        return {"success": False, "message": "Failed to spawn pickup"}

    mesh = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube")
    comp = actor.static_mesh_component
    if comp and mesh:
        comp.set_static_mesh(mesh)
    actor.set_actor_scale3d(unreal.Vector(0.4, 0.8, 0.25))
    actor.set_actor_label(f"Pickup_{weapon_class}_{name}")

    return {
        "success": True,
        "message": f"Spawned pickup '{actor.get_actor_label()}'",
        "output": {"actor": actor.get_actor_label(), "weapon_class": weapon_class},
    }
