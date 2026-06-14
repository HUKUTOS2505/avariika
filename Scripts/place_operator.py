import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/place_operator_result.json"
R = {"ok": False, "err": None, "loc": None}
try:
    sk = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
    idle = unreal.load_asset("/Game/Characters/Operator/Anims/A_Idle_11.A_Idle_11")
    unreal.EditorLoadingAndSavingUtils.load_map("/Game/FirstPerson/Lvl_FirstPerson")

    # remove any previous TestOperator so reruns don't stack
    sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in sub.get_all_level_actors():
        if a.get_actor_label() == "TestOperator":
            sub.destroy_actor(a)

    loc = unreal.Vector(300.0, 0.0, -13.0)  # on the floor (z=-15) in front of the start
    rot = unreal.Rotator(0.0, 0.0, 180.0)
    actor = sub.spawn_actor_from_class(unreal.SkeletalMeshActor, loc, rot)
    actor.set_actor_label("TestOperator")
    comp = actor.skeletal_mesh_component
    try:
        comp.set_skeletal_mesh_asset(sk)
    except Exception:
        comp.set_editor_property("skeletal_mesh", sk)
    comp.set_editor_property("animation_mode", unreal.AnimationMode.ANIMATION_SINGLE_NODE)
    if idle:
        comp.set_animation(idle)
        comp.play(True)
    unreal.EditorLoadingAndSavingUtils.save_current_level()
    R["ok"] = True
    R["loc"] = [loc.x, loc.y, loc.z]
except Exception as e:
    R["err"] = str(e)

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("PLACE_OPERATOR: " + json.dumps(R))
