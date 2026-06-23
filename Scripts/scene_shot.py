import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/manifests/_scene_shot.json"
R = {"steps": [], "err": None}
try:
    PAIRS = [
        ("/Game/Characters/Operator/Anims_Retarget/Op_Bandage_loop.Op_Bandage_loop", 0.5, -90, "GOOD_UE4"),
        ("/Game/Characters/Operator/Anims_Retarget/Op_AS_Fixing.Op_AS_Fixing", 0.6, 90, "BAD_UE5"),
    ]
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = ues.get_editor_world()
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        try:
            if a.get_actor_label().startswith(("PREVIEW_RIG", "PREVIEW_CAP")): eas.destroy_actor(a)
        except Exception: pass
    op_mesh = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
    for path, t, x, label in PAIRS:
        anim = unreal.load_asset(path)
        actor = eas.spawn_actor_from_class(unreal.SkeletalMeshActor, unreal.Vector(x, 0, 0), unreal.Rotator(0, 0, 0))
        actor.set_actor_label("PREVIEW_RIG_" + label)
        comp = actor.skeletal_mesh_component
        comp.set_skeletal_mesh(op_mesh)
        comp.set_animation_mode(unreal.AnimationMode.ANIMATION_SINGLE_NODE)
        comp.set_animation(anim)
        comp.set_position(t, False); comp.play(False); comp.set_position(t, False)
        R["steps"].append("posed " + label)
    rt = unreal.RenderingLibrary.create_render_target2_d(world, 1200, 800)
    R["steps"].append("rt " + str(bool(rt)))
    cap = eas.spawn_actor_from_class(unreal.SceneCapture2D, unreal.Vector(0, 360, 110), unreal.Rotator(0, -90, 0))
    cap.set_actor_label("PREVIEW_CAP")
    cc = cap.scene_capture_component2d
    cc.set_editor_property("texture_target", rt)
    cc.set_editor_property("fov_angle", 55.0)
    try: cc.set_editor_property("capture_source", unreal.SceneCaptureSource.SCS_FINAL_COLOR_LDR)
    except Exception as e: R["steps"].append("capsrc " + str(e))
    cc.capture_scene()
    R["steps"].append("captured")
    ok = unreal.RenderingLibrary.export_render_target(world, rt, r"C:/unrealEngine/avariika/Saved/Screenshots", "scene_cmp.png")
    R["export"] = str(ok)
    for a in eas.get_all_level_actors():
        try:
            if a.get_actor_label().startswith(("PREVIEW_RIG", "PREVIEW_CAP")): eas.destroy_actor(a)
        except Exception: pass
except Exception as e:
    import traceback
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(OUT, "w") as f: json.dump(R, f, indent=1)
unreal.log("SCENE_SHOT done")
