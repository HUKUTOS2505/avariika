import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/fix_ops.json"
rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MCO_to_Operator.RTG_MCO_to_Operator")
ctrl = unreal.IKRetargeterController.get_controller(rt)
R = {"changed": []}
# Keep only a clean FK retarget: Pelvis Motion(0) + FK Chains(1) + Root Motion(4) + Remap Curves(5).
# Disable IK ops (2 Retarget IK Goals, 3 Run IK Rig) and the whole duplicate set (6..10).
DISABLE = [2, 3, 6, 7, 8, 9, 10]
n = ctrl.get_num_retarget_ops()
for i in range(n):
    want = i not in DISABLE
    try:
        ctrl.set_retarget_op_enabled(i, want)
        R["changed"].append({"i": i, "name": str(ctrl.get_op_name(i)), "enabled": want})
    except Exception as e:
        R["changed"].append({"i": i, "err": str(e)})
unreal.EditorAssetLibrary.save_loaded_asset(rt, False)
with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("FIX_OPS done")
