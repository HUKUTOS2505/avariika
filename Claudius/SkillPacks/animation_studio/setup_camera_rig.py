# @claudius category: animation_studio
# @claudius description: Spawn a cinematic camera + simple orbit rig around a target actor or location. Sensible defaults for product / explainer shots.
# @claudius workflow: animation, shot, level

import unreal


def execute(params):
    target_actor = params.get("target_actor")
    target_location = params.get("target_location")
    radius = float(params.get("radius", 400.0))
    height = float(params.get("height", 150.0))
    focal_length_mm = float(params.get("focal_length_mm", 35.0))
    name = params.get("name", "ShotCamera_01")

    actor_sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    world = unreal.EditorLevelLibrary.get_editor_world()

    # Resolve focus point
    if target_actor:
        actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
        focus = None
        for a in actors:
            if a.get_actor_label() == target_actor or a.get_name() == target_actor:
                focus = a
                break
        if not focus:
            return {"success": False, "message": f"target_actor not found: {target_actor}"}
        focus_loc = focus.get_actor_location()
    elif target_location:
        focus_loc = unreal.Vector(
            float(target_location.get("x", 0)),
            float(target_location.get("y", 0)),
            float(target_location.get("z", 0)),
        )
    else:
        focus_loc = unreal.Vector(0, 0, 0)

    cam_loc = unreal.Vector(focus_loc.x + radius, focus_loc.y, focus_loc.z + height)
    cam = actor_sub.spawn_actor_from_class(unreal.CineCameraActor, cam_loc)
    if not cam:
        return {"success": False, "message": "Failed to spawn CineCameraActor"}

    cam.set_actor_label(name)

    # Aim at focus
    delta = focus_loc - cam_loc
    rot = unreal.Rotator(0, 0, 0)
    try:
        rot = unreal.MathLibrary.find_look_at_rotation(cam_loc, focus_loc)
    except Exception:
        # Fallback: leave default rotation
        pass
    cam.set_actor_rotation(rot, False)

    cam_comp = cam.get_cine_camera_component()
    if cam_comp:
        cam_comp.current_focal_length = focal_length_mm
        cam_comp.focus_settings.focus_method = unreal.CameraFocusMethod.MANUAL
        cam_comp.focus_settings.manual_focus_distance = max(50.0, delta.size())

    return {
        "success": True,
        "message": f"Spawned shot camera '{name}' aimed at focus point",
        "output": {
            "camera": name,
            "focus_location": {"x": focus_loc.x, "y": focus_loc.y, "z": focus_loc.z},
            "focal_length_mm": focal_length_mm,
            "radius": radius,
            "height": height,
        },
    }
