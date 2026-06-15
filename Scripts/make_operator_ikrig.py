import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/make_operator_ikrig.json"
R = {"steps": [], "op_bones": [], "src_bones": [], "err": None}
tools = unreal.AssetToolsHelpers.get_asset_tools()


def make_ikrig(name, folder, mesh_path):
    sm = unreal.load_asset(mesh_path)
    if not sm:
        return None, None, "mesh not found: " + mesh_path
    pkg = folder + "/" + name
    rig = unreal.load_asset(pkg + "." + name)
    if rig is None:
        rig = tools.create_asset(name, folder, unreal.IKRigDefinition, unreal.IKRigDefinitionFactory())
    if rig is None:
        return None, None, "rig create failed"
    try:
        ctrl = unreal.IKRigController.get_controller(rig)
    except Exception as e:
        return rig, None, "controller err: " + str(e)
    try:
        ctrl.set_skeletal_mesh(sm)
    except Exception as e:
        return rig, ctrl, "set_mesh err: " + str(e)
    return rig, ctrl, None


def dump_bones(ctrl, limit=120):
    names = []
    try:
        skel = ctrl.get_skeleton()
        for n in skel.bone_names:
            names.append(str(n))
    except Exception as e:
        names = ["ERR:" + str(e)]
    return names[:limit]


# Operator
rig, ctrl, err = make_ikrig("IK_Operator", "/Game/Characters/Operator/Rig",
                            "/Game/Characters/Operator/SK_Operator.SK_Operator")
R["steps"].append("op_rig=" + str(rig is not None) + " err=" + str(err))
if ctrl:
    unreal.EditorAssetLibrary.save_loaded_asset(rig, False)
    R["op_bones"] = dump_bones(ctrl)

# Source (MC_Sample UE5 v2)
srig, sctrl, serr = make_ikrig("IK_MCUE5v2", "/Game/Characters/Operator/Rig",
                               "/Game/MC_Sample/Demo/Characters/MCUE5v2/Meshes/SKM_MCUE5v2.SKM_MCUE5v2")
R["steps"].append("src_rig=" + str(srig is not None) + " err=" + str(serr))
if sctrl:
    unreal.EditorAssetLibrary.save_loaded_asset(srig, False)
    R["src_bones"] = dump_bones(sctrl)

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("MAKE_IKRIG done")
