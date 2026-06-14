import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/build_anim_test_map.json"
MAP = "/Game/Avariika/Maps/L_AnimTest"
R = {"steps": [], "err": None}


def step(s):
    R["steps"].append(s)


try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.new_level(MAP)
    step("new_level")
    sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

    # --- Floor (engine cube, thin slab) ---
    cube = unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
    floor = sub.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, -5), unreal.Rotator(0, 0, 0))
    floor.set_actor_label("Floor")
    fc = floor.static_mesh_component
    fc.set_editor_property("static_mesh", cube)
    floor.set_actor_scale3d(unreal.Vector(40.0, 40.0, 0.1))  # 4000x4000, ~10 thick -> top at z=0
    step("floor")

    # --- Lighting: directional + skylight + sky atmosphere (even daylight) ---
    sun = sub.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 600), unreal.Rotator(-50, -35, 0))
    sun.set_actor_label("Sun")
    sc = sun.get_component_by_class(unreal.DirectionalLightComponent)
    if sc:
        sc.set_mobility(unreal.ComponentMobility.MOVABLE)
        sc.set_intensity(6.0)
    atmo = sub.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    atmo.set_actor_label("SkyAtmosphere")
    sky = sub.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 300), unreal.Rotator(0, 0, 0))
    sky.set_actor_label("SkyLight")
    skc = sky.get_component_by_class(unreal.SkyLightComponent)
    if skc:
        skc.set_mobility(unreal.ComponentMobility.MOVABLE)
        try:
            skc.set_editor_property("real_time_capture", True)
        except Exception:
            pass
        skc.set_editor_property("intensity", 1.0)
    step("lighting")

    # --- PlayerStart facing the operator ---
    try:
        ps = sub.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, -350, 90), unreal.Rotator(0, 90, 0))
        ps.set_actor_label("PlayerStart")
        step("playerstart")
    except Exception as e:
        step("playerstart_skip:" + str(e))

    # --- Operator (idle) ---
    sk = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
    idle = unreal.load_asset("/Game/Characters/Operator/Anims/A_Idle_11.A_Idle_11")
    op = sub.spawn_actor_from_class(unreal.SkeletalMeshActor, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    op.set_actor_label("Operator")
    comp = op.skeletal_mesh_component
    try:
        comp.set_skeletal_mesh_asset(sk)
    except Exception:
        comp.set_editor_property("skeletal_mesh", sk)
    comp.set_editor_property("animation_mode", unreal.AnimationMode.ANIMATION_SINGLE_NODE)
    if idle:
        comp.set_animation(idle)
        comp.play(True)
    step("operator")

    unreal.EditorLoadingAndSavingUtils.save_current_level()
    step("saved")
except Exception as e:
    R["err"] = str(e)

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("BUILD_ANIM_TEST: " + json.dumps(R))
