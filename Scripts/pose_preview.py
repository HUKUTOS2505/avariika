import unreal, json
JOB = r"C:/unrealEngine/avariika/Scripts/preview_job.json"
with open(JOB) as fp: J = json.load(fp)
anim_path = J["anim"]; t = float(J.get("time", 0.5))
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
# clean previous preview rigs
for a in eas.get_all_level_actors():
    if a.get_actor_label().startswith("PREVIEW_RIG"):
        eas.destroy_actor(a)
op_mesh = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
anim = unreal.load_asset(anim_path)
loc = unreal.Vector(0, 0, 0)
actor = eas.spawn_actor_from_class(unreal.SkeletalMeshActor, loc, unreal.Rotator(0, 0, 0))
actor.set_actor_label("PREVIEW_RIG")
comp = actor.skeletal_mesh_component
comp.set_skeletal_mesh(op_mesh)
comp.set_animation_mode(unreal.AnimationMode.ANIMATION_SINGLE_NODE)
comp.set_animation(anim)
try:
    comp.set_position(t, False)
    comp.set_play_rate(0.0)
    comp.play(False)
    comp.set_position(t, False)
except Exception as e:
    pass
# camera: in front of character (faces +X), full body
cam_loc = unreal.Vector(300, 0, 100)
cam_rot = unreal.Rotator(0, 180, 0)  # roll,pitch,yaw? UE Rotator(pitch,yaw,roll)
cam_rot = unreal.Rotator(-3, 180, 0)
unreal.EditorLevelLibrary.set_level_viewport_camera_info(cam_loc, cam_rot)
unreal.log("POSE_PREVIEW posed %s @ %.2f" % (anim_path, t))
