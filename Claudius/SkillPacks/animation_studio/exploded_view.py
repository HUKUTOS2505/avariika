# @claudius category: animation_studio
# @claudius description: Animate a list of actors outward along their pivot-to-center vector to form an exploded view. Stamps the start + end transforms; a follow-up Sequencer skill can keyframe them.
# @claudius workflow: animation, shot, level

import unreal


def execute(params):
    actor_names = params.get("actors") or []
    explosion_factor = float(params.get("factor", 1.5))
    center = params.get("center")
    apply_immediately = bool(params.get("apply", True))

    if not actor_names:
        return {"success": False, "message": "Missing 'actors' parameter (list of actor labels)"}

    world = unreal.EditorLevelLibrary.get_editor_world()
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    by_label = {a.get_actor_label(): a for a in all_actors}

    selected = [by_label[n] for n in actor_names if n in by_label]
    if not selected:
        return {"success": False, "message": "None of the named actors were found in the level"}

    # Determine center: explicit, or centroid of selection
    if center:
        cx, cy, cz = float(center.get("x", 0)), float(center.get("y", 0)), float(center.get("z", 0))
    else:
        cx = sum(a.get_actor_location().x for a in selected) / len(selected)
        cy = sum(a.get_actor_location().y for a in selected) / len(selected)
        cz = sum(a.get_actor_location().z for a in selected) / len(selected)
    center_v = unreal.Vector(cx, cy, cz)

    moves = []
    for a in selected:
        loc = a.get_actor_location()
        delta = unreal.Vector(loc.x - center_v.x, loc.y - center_v.y, loc.z - center_v.z)
        target = unreal.Vector(
            center_v.x + delta.x * explosion_factor,
            center_v.y + delta.y * explosion_factor,
            center_v.z + delta.z * explosion_factor,
        )
        moves.append({
            "actor": a.get_actor_label(),
            "from": {"x": loc.x, "y": loc.y, "z": loc.z},
            "to":   {"x": target.x, "y": target.y, "z": target.z},
        })
        if apply_immediately:
            a.set_actor_location(target, False, True)

    return {
        "success": True,
        "message": f"{'Applied' if apply_immediately else 'Computed'} exploded transforms for {len(moves)} actors",
        "output": {
            "center": {"x": cx, "y": cy, "z": cz},
            "factor": explosion_factor,
            "moves": moves,
        },
    }
