import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/fix_spine_chain.json"
R = {"steps": []}
eal = unreal.EditorAssetLibrary

# 1) fix operator Spine chain direction: bottom(Spine02) -> top(Spine)
rig = unreal.load_asset("/Game/Characters/Operator/Rig/IK_Operator.IK_Operator")
ctrl = unreal.IKRigController.get_controller(rig)
try:
    ctrl.set_retarget_chain_start_bone("Spine", "Spine02")
    ctrl.set_retarget_chain_end_bone("Spine", "Spine")
    R["steps"].append("spine chain Spine02->Spine ok")
except Exception as e:
    R["steps"].append("spine err " + str(e))
# verify
try:
    R["spine_start"] = str(ctrl.get_retarget_chain_start_bone("Spine"))
    R["spine_end"] = str(ctrl.get_retarget_chain_end_bone("Spine"))
except Exception as e:
    R["verify_err"] = str(e)
eal.save_loaded_asset(rig, False)

# 2) remap + align retargeter
rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MCO_to_Operator.RTG_MCO_to_Operator")
rtc = unreal.IKRetargeterController.get_controller(rt)
try:
    rtc.auto_map_chains(unreal.AutoMapChainType.EXACT, True); R["steps"].append("remap ok")
except Exception as e:
    R["steps"].append("remap err " + str(e))
try:
    rtc.auto_align_all_bones(unreal.RetargetSourceOrTarget.TARGET); R["steps"].append("align ok")
except Exception as e:
    R["steps"].append("align err " + str(e))
eal.save_loaded_asset(rt, False)

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("FIX_SPINE done")
