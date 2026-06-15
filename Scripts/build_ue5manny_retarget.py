import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/manifests/_build_ue5manny.json"
R = {"steps": []}
tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary
F = "/Game/Characters/Operator/Rig"

# canonical standard UE5 Manny (CharacterEmotes copy == WorkAnimations copy == Epic UE5 Manny)
mesh = unreal.load_asset("/Game/CharacterEmotes/Demo/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny")
R["mesh"] = bool(mesh)

rig = unreal.load_asset(F + "/IK_UE5Manny.IK_UE5Manny")
if rig is None:
    rig = tools.create_asset("IK_UE5Manny", F, unreal.IKRigDefinition, unreal.IKRigDefinitionFactory())
ctrl = unreal.IKRigController.get_controller(rig)
ctrl.set_skeletal_mesh(mesh)

ctrl.set_retarget_root("pelvis")
for c in list(ctrl.get_retarget_chains()):
    ctrl.remove_retarget_chain(c.chain_name)

def chain_names():
    return [str(c.chain_name) for c in ctrl.get_retarget_chains()]

def add_chain(name, start, end_candidates):
    ends = end_candidates if isinstance(end_candidates, list) else [end_candidates]
    for e in ends:
        try:
            ctrl.add_retarget_chain(name, start, e, "None")
        except Exception:
            pass
        if name in chain_names():
            return e
        # remove a possibly-created-but-invalid chain before next try
        try: ctrl.remove_retarget_chain(name)
        except Exception: pass
    R["steps"].append("chain " + name + " FAILED (ends=" + ",".join(ends) + ")")
    return None

used = {}
used["Spine"] = add_chain("Spine", "spine_01", ["spine_06", "spine_05", "spine_04", "spine_03"])
used["Neck"] = add_chain("Neck", "neck_01", ["neck_02", "neck_01"])
add_chain("Head", "head", ["head"])
add_chain("LeftClavicle", "clavicle_l", ["clavicle_l"])
add_chain("LeftArm", "upperarm_l", ["hand_l"])
add_chain("RightClavicle", "clavicle_r", ["clavicle_r"])
add_chain("RightArm", "upperarm_r", ["hand_r"])
add_chain("LeftLeg", "thigh_l", ["ball_l", "foot_l"])
add_chain("RightLeg", "thigh_r", ["ball_r", "foot_r"])
eal.save_loaded_asset(rig, False)
R["chains"] = chain_names()
R["used"] = used

tgt = unreal.load_asset(F + "/IK_Operator.IK_Operator")
rt = unreal.load_asset(F + "/RTG_UE5Manny_to_Operator.RTG_UE5Manny_to_Operator")
if rt is None:
    rt = tools.create_asset("RTG_UE5Manny_to_Operator", F, unreal.IKRetargeter, unreal.IKRetargetFactory())
rtc = unreal.IKRetargeterController.get_controller(rt)
rt.set_editor_property("source_ik_rig_asset", rig)
rt.set_editor_property("target_ik_rig_asset", tgt)
# remove any accumulated ops from prior runs (reverse order), then add ONE default set
try:
    for i in range(rtc.get_num_retarget_ops() - 1, -1, -1):
        try: rtc.remove_retarget_op(i)
        except Exception: pass
    R["steps"].append("ops cleared -> " + str(rtc.get_num_retarget_ops()))
except Exception as e: R["steps"].append("clear err " + str(e))
if rtc.get_num_retarget_ops() == 0:
    try: rtc.add_default_ops(); R["steps"].append("ops ok")
    except Exception as e: R["steps"].append("ops err " + str(e))
try:
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.TARGET, tgt)
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.SOURCE, rig)
    R["steps"].append("assign ok")
except Exception as e: R["steps"].append("assign err " + str(e))
try: rtc.auto_map_chains(unreal.AutoMapChainType.EXACT, True); R["steps"].append("map ok")
except Exception as e: R["steps"].append("map err " + str(e))
# align BOTH sides (the missing piece in RTG_UEFN that broke arms)
for side, lbl in [(unreal.RetargetSourceOrTarget.SOURCE, "src"), (unreal.RetargetSourceOrTarget.TARGET, "tgt")]:
    try: rtc.auto_align_all_bones(side); R["steps"].append("align " + lbl + " ok")
    except Exception as e: R["steps"].append("align " + lbl + " err " + str(e))
# FK-only + dedup: enable only the FIRST occurrence of each base op type
import re
KEEP = {"Pelvis Motion", "FK Chains", "Root Motion", "Remap Curves"}
seen = set(); ops = []
for i in range(rtc.get_num_retarget_ops()):
    nm = str(rtc.get_op_name(i))
    base = re.sub(r"_\d+$", "", nm)
    enable = (base in KEEP) and (base not in seen)
    if enable: seen.add(base)
    try: rtc.set_retarget_op_enabled(i, enable)
    except Exception: pass
    ops.append({"name": nm, "enabled": enable})
R["ops"] = ops
eal.save_loaded_asset(rt, False)
R["has_source"] = rt.has_source_ik_rig(); R["has_target"] = rt.has_target_ik_rig()
with open(OUT, "w") as f: json.dump(R, f, indent=1)
unreal.log("BUILD_UE5MANNY done")
