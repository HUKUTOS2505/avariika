import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/list_ops.json"
rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MCO_to_Operator.RTG_MCO_to_Operator")
ctrl = unreal.IKRetargeterController.get_controller(rt)
R = {"ops": []}
n = ctrl.get_num_retarget_ops()
for i in range(n):
    try:
        nm = str(ctrl.get_op_name(i))
    except Exception as e:
        nm = "nameerr:" + str(e)
    try:
        en = ctrl.get_retarget_op_enabled(i)
    except Exception as e:
        en = "enerr:" + str(e)
    R["ops"].append({"i": i, "name": nm, "enabled": en})
with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("LIST_OPS done")
