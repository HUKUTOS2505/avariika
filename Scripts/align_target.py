import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/align_target.json"
R = {"steps": []}
rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MCO_to_Operator.RTG_MCO_to_Operator")
ctrl = unreal.IKRetargeterController.get_controller(rt)
# write a marker BEFORE the risky call so we know we reached it
with open(OUT, "w") as f:
    json.dump({"steps": ["about to auto_align TARGET"]}, f, indent=1)
try:
    ctrl.auto_align_all_bones(unreal.RetargetSourceOrTarget.TARGET)
    R["steps"].append("auto_align TARGET ok")
except Exception as e:
    R["steps"].append("auto_align TARGET err " + str(e))
unreal.EditorAssetLibrary.save_loaded_asset(rt, False)
R["steps"].append("saved")
with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("ALIGN_TARGET done")
