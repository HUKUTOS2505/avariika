import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/build_ikrig_chains.json"
R = {"op": {}, "src": {}, "err": None}

def setup(rig_path, root, chains):
    rig = unreal.load_asset(rig_path)
    if not rig:
        return {"loaded": False}
    ctrl = unreal.IKRigController.get_controller(rig)
    res = {"loaded": True, "added": [], "fail": []}
    try:
        ctrl.set_retarget_root(root)
        res["root"] = root
    except Exception as e:
        res["root_err"] = str(e)
    # clear existing chains (idempotent)
    try:
        for c in list(ctrl.get_retarget_chains()):
            ctrl.remove_retarget_chain(c.chain_name)
    except Exception:
        pass
    for name, start, end in chains:
        try:
            ctrl.add_retarget_chain(name, start, end, "None")
            res["added"].append(name)
        except Exception as e:
            res["fail"].append(name + ":" + str(e))
    unreal.EditorAssetLibrary.save_loaded_asset(rig, False)
    try:
        res["chains_now"] = [str(c.chain_name) for c in ctrl.get_retarget_chains()]
    except Exception as e:
        res["chains_err"] = str(e)
    return res

# Operator (Mixamo-style)
R["op"] = setup("/Game/Characters/Operator/Rig/IK_Operator.IK_Operator", "Hips", [
    ("Spine", "Spine", "Spine02"),
    ("Neck", "neck", "neck"),
    ("Head", "Head", "Head"),
    ("LeftClavicle", "LeftShoulder", "LeftShoulder"),
    ("LeftArm", "LeftArm", "LeftHand"),
    ("RightClavicle", "RightShoulder", "RightShoulder"),
    ("RightArm", "RightArm", "RightHand"),
    ("LeftLeg", "LeftUpLeg", "LeftToeBase"),
    ("RightLeg", "RightUpLeg", "RightToeBase"),
])

# Source MCUE5v2 (UE5 mannequin)
R["src"] = setup("/Game/Characters/Operator/Rig/IK_MCUE5v2.IK_MCUE5v2", "pelvis", [
    ("Spine", "spine_01", "spine_05"),
    ("Neck", "neck_01", "neck_02"),
    ("Head", "head", "head"),
    ("LeftClavicle", "clavicle_l", "clavicle_l"),
    ("LeftArm", "upperarm_l", "hand_l"),
    ("RightClavicle", "clavicle_r", "clavicle_r"),
    ("RightArm", "upperarm_r", "hand_r"),
    ("LeftLeg", "thigh_l", "ball_l"),
    ("RightLeg", "thigh_r", "ball_r"),
])

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("BUILD_IKRIG_CHAINS done")
