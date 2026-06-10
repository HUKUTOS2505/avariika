# @claudius category: animation_studio
# @claudius description: Swap a material on one or more actors as a reveal step. Reads material_index per actor; fails open if the actor isn't a StaticMeshActor.
# @claudius workflow: animation, shot, asset

import unreal


def execute(params):
    actor_names = params.get("actors") or []
    material_path = params.get("material_path")
    material_index = int(params.get("material_index", 0))

    if not actor_names:
        return {"success": False, "message": "Missing 'actors' parameter (list of actor labels)"}
    if not material_path:
        return {"success": False, "message": "Missing 'material_path' parameter"}

    material = unreal.load_asset(material_path)
    if not material:
        return {"success": False, "message": f"Material not found: {material_path}"}

    world = unreal.EditorLevelLibrary.get_editor_world()
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    by_label = {a.get_actor_label(): a for a in all_actors}

    swapped = []
    skipped = []
    for name in actor_names:
        a = by_label.get(name)
        if not a:
            skipped.append({"actor": name, "reason": "not found"})
            continue

        # Try StaticMeshActor first
        comp = None
        if isinstance(a, unreal.StaticMeshActor):
            comp = a.static_mesh_component
        elif isinstance(a, unreal.SkeletalMeshActor):
            comp = a.skeletal_mesh_component

        if not comp:
            skipped.append({"actor": name, "reason": "no compatible mesh component"})
            continue

        try:
            comp.set_material(material_index, material)
            a.modify()
            swapped.append(name)
        except Exception as e:
            skipped.append({"actor": name, "reason": str(e)})

    return {
        "success": True,
        "message": f"Swapped material on {len(swapped)} actors; skipped {len(skipped)}",
        "output": {
            "material": material_path,
            "material_index": material_index,
            "swapped": swapped,
            "skipped": skipped,
        },
    }
