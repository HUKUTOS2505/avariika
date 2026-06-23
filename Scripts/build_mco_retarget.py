import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/build_mco_retarget.json"
R = {"steps": []}
tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary
FOLDER = "/Game/Characters/Operator/Rig"

# 1) IK rig for UE4 mannequin (MCO mesh)
mco_mesh = unreal.load_asset("/Game/MCO_Mocap_Basics/Character/Mesh/SK_Mannequin.SK_Mannequin")
rig = unreal.load_asset(FOLDER + "/IK_UE4Mann.IK_UE4Mann")
if rig is None:
    rig = tools.create_asset("IK_UE4Mann", FOLDER, unreal.IKRigDefinition, unreal.IKRigDefinitionFactory())
ctrl = unreal.IKRigController.get_controller(rig)
ctrl.set_skeletal_mesh(mco_mesh)
try:
    ctrl.set_retarget_root("pelvis")
except Exception as e:
    R["steps"].append("root err " + str(e))
for c in list(ctrl.get_retarget_chains()):
    ctrl.remove_retarget_chain(c.chain_name)
chains = [
    ("Spine", "spine_01", "spine_03"), ("Neck", "neck_01", "neck_01"), ("Head", "head", "head"),
    ("LeftClavicle", "clavicle_l", "clavicle_l"), ("LeftArm", "upperarm_l", "hand_l"),
    ("RightClavicle", "clavicle_r", "clavicle_r"), ("RightArm", "upperarm_r", "hand_r"),
    ("LeftLeg", "thigh_l", "ball_l"), ("RightLeg", "thigh_r", "ball_r"),
]
for n, s, e in chains:
    try:
        ctrl.add_retarget_chain(n, s, e, "None")
    except Exception as ex:
        R["steps"].append("chain " + n + " err " + str(ex))
eal.save_loaded_asset(rig, False)
R["ue4_chains"] = [str(c.chain_name) for c in ctrl.get_retarget_chains()]

# 2) retargeter MCO -> Operator
tgt = unreal.load_asset(FOLDER + "/IK_Operator.IK_Operator")
rt = unreal.load_asset(FOLDER + "/RTG_MCO_to_Operator.RTG_MCO_to_Operator")
if rt is None:
    rt = tools.create_asset("RTG_MCO_to_Operator", FOLDER, unreal.IKRetargeter, unreal.IKRetargetFactory())
rtc = unreal.IKRetargeterController.get_controller(rt)
rt.set_editor_property("source_ik_rig_asset", rig)
rt.set_editor_property("target_ik_rig_asset", tgt)
try:
    rtc.add_default_ops(); R["steps"].append("add_default_ops ok")
except Exception as e:
    R["steps"].append("add_default_ops err " + str(e))
try:
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.TARGET, tgt)
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.SOURCE, rig)
    R["steps"].append("assign ok")
except Exception as e:
    R["steps"].append("assign err " + str(e))
try:
    rtc.auto_map_chains(unreal.AutoMapChainType.EXACT, True); R["steps"].append("auto_map ok")
except Exception as e:
    R["steps"].append("auto_map err " + str(e))
eal.save_loaded_asset(rt, False)
R["has_source"] = rt.has_source_ik_rig()
R["has_target"] = rt.has_target_ik_rig()
R["num_ops"] = rtc.get_num_retarget_ops()

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("BUILD_MCO_RETARGET done")
