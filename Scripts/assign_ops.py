import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/assign_ops.json"
R = {"steps": []}
rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MC_to_Operator.RTG_MC_to_Operator")
tgt = unreal.load_asset("/Game/Characters/Operator/Rig/IK_Operator.IK_Operator")
src = unreal.load_asset("/Game/Characters/Operator/Rig/IK_MCUE5v2.IK_MCUE5v2")
ctrl = unreal.IKRetargeterController.get_controller(rt)

# assign target ik rig to all ops (signature: source_or_target, ik_rig)
try:
    ctrl.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.TARGET, tgt)
    R["steps"].append("assign TARGET ok")
except Exception as e:
    R["steps"].append("assign TARGET err " + str(e))
# also assign source to all ops (some ops need both)
try:
    ctrl.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.SOURCE, src)
    R["steps"].append("assign SOURCE ok")
except Exception as e:
    R["steps"].append("assign SOURCE err " + str(e))
# re-map chains into ops
try:
    ctrl.auto_map_chains(unreal.AutoMapChainType.EXACT, True)
    R["steps"].append("auto_map ok")
except Exception as e:
    R["steps"].append("auto_map err " + str(e))

unreal.EditorAssetLibrary.save_loaded_asset(rt, False)
R["num_ops"] = ctrl.get_num_retarget_ops()
with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("ASSIGN_OPS done")
