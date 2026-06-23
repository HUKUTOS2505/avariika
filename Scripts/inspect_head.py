import unreal, json, math
OUT = r"C:/unrealEngine/avariika/Scripts/inspect_head.json"
R = {}

# parents via transient component
sm = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
comp = unreal.new_object(unreal.SkeletalMeshComponent)
try: comp.set_skeletal_mesh_asset(sm)
except Exception: comp.set_editor_property("skeletal_mesh", sm)
allb = [str(comp.get_bone_name(i)) for i in range(comp.get_num_bones())]
R["all_bones"] = allb
parents = {}
for b in ["neck", "Head", "head_end", "headfront", "Spine02", "Spine01", "Spine"]:
    try:
        parents[b] = str(comp.get_parent_bone(b))
    except Exception as e:
        parents[b] = "err:" + str(e)
R["parents"] = parents

# ref-pose rotation (euler) from the operator IK rig
rig = unreal.load_asset("/Game/Characters/Operator/Rig/IK_Operator.IK_Operator")
ctrl = unreal.IKRigController.get_controller(rig)
rots = {}
for b in ["Hips", "Spine", "Spine02", "neck", "Head", "head_end", "headfront"]:
    try:
        t = ctrl.get_ref_pose_transform_of_bone(b)
        r = t.rotation.rotator()
        rots[b] = {"pitch": round(r.pitch, 1), "yaw": round(r.yaw, 1), "roll": round(r.roll, 1)}
    except Exception as e:
        rots[b] = "err:" + str(e)
R["ref_rot"] = rots

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("INSPECT_HEAD done")
