import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/build_uefn_retarget.json"
R = {"steps": []}
tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary
F = "/Game/Characters/Operator/Rig"

mesh = unreal.load_asset("/Game/Characters/UEFN_Mannequin/Meshes/SKM_UEFN_Mannequin.SKM_UEFN_Mannequin")
rig = unreal.load_asset(F + "/IK_UEFN.IK_UEFN")
if rig is None:
    rig = tools.create_asset("IK_UEFN", F, unreal.IKRigDefinition, unreal.IKRigDefinitionFactory())
ctrl = unreal.IKRigController.get_controller(rig)
ctrl.set_skeletal_mesh(mesh)
ctrl.set_retarget_root("pelvis")
for c in list(ctrl.get_retarget_chains()):
    ctrl.remove_retarget_chain(c.chain_name)
for n, s, e in [("Spine","spine_01","spine_05"),("Neck","neck_01","neck_02"),("Head","head","head"),
                ("LeftClavicle","clavicle_l","clavicle_l"),("LeftArm","upperarm_l","hand_l"),
                ("RightClavicle","clavicle_r","clavicle_r"),("RightArm","upperarm_r","hand_r"),
                ("LeftLeg","thigh_l","ball_l"),("RightLeg","thigh_r","ball_r")]:
    try: ctrl.add_retarget_chain(n, s, e, "None")
    except Exception as ex: R["steps"].append("chain "+n+" err "+str(ex))
eal.save_loaded_asset(rig, False)
R["uefn_chains"] = [str(c.chain_name) for c in ctrl.get_retarget_chains()]

tgt = unreal.load_asset(F + "/IK_Operator.IK_Operator")
rt = unreal.load_asset(F + "/RTG_UEFN_to_Operator.RTG_UEFN_to_Operator")
if rt is None:
    rt = tools.create_asset("RTG_UEFN_to_Operator", F, unreal.IKRetargeter, unreal.IKRetargetFactory())
rtc = unreal.IKRetargeterController.get_controller(rt)
rt.set_editor_property("source_ik_rig_asset", rig)
rt.set_editor_property("target_ik_rig_asset", tgt)
try: rtc.add_default_ops(); R["steps"].append("ops ok")
except Exception as e: R["steps"].append("ops err "+str(e))
try:
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.TARGET, tgt)
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.SOURCE, rig)
    R["steps"].append("assign ok")
except Exception as e: R["steps"].append("assign err "+str(e))
try: rtc.auto_map_chains(unreal.AutoMapChainType.EXACT, True); R["steps"].append("map ok")
except Exception as e: R["steps"].append("map err "+str(e))
try: rtc.auto_align_all_bones(unreal.RetargetSourceOrTarget.TARGET); R["steps"].append("align ok")
except Exception as e: R["steps"].append("align err "+str(e))
# disable IK ops (operator rig has no IK goals -> they stretch feet)
ops = []
for i in range(rtc.get_num_retarget_ops()):
    nm = str(rtc.get_op_name(i))
    dis = ("IK Goals" in nm) or ("Run IK Rig" in nm)
    try: rtc.set_retarget_op_enabled(i, not dis)
    except Exception: pass
    ops.append({"i": i, "name": nm, "enabled": not dis})
R["ops"] = ops
eal.save_loaded_asset(rt, False)
R["has_source"] = rt.has_source_ik_rig(); R["has_target"] = rt.has_target_ik_rig()
with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("BUILD_UEFN_RETARGET done")
