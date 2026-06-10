# @claudius category: fps_starter
# @claudius description: Spawn a grid of target dummies for shooting practice.
# @claudius workflow: fps, combat, debug

import unreal


def execute(params):
    rows = int(params.get("rows", 3))
    cols = int(params.get("cols", 3))
    spacing = float(params.get("spacing", 200.0))
    origin = params.get("origin", {"x": 0.0, "y": 0.0, "z": 100.0})

    sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    spawned = []
    mesh = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cylinder")

    for r in range(rows):
        for c in range(cols):
            loc = unreal.Vector(
                origin["x"] + c * spacing,
                origin["y"] + r * spacing,
                origin["z"],
            )
            actor = sub.spawn_actor_from_class(unreal.StaticMeshActor, loc)
            if not actor:
                continue
            comp = actor.static_mesh_component
            if comp and mesh:
                comp.set_static_mesh(mesh)
            actor.set_actor_label(f"TargetDummy_{r}_{c}")
            actor.set_actor_scale3d(unreal.Vector(0.5, 0.5, 1.5))
            spawned.append(actor.get_actor_label())

    return {
        "success": True,
        "message": f"Spawned {len(spawned)} target dummies",
        "output": {"count": len(spawned), "actors": spawned},
    }
