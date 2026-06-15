import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/inspect_rigs.json"
R = {"rigs": {}, "retargeters": {}, "asp_meshes": [], "asp_skel": ""}
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()

def rig_skel(rig):
    try:
        c = unreal.IKRigController.get_controller(rig)
        sk = c.get_skeletal_mesh()
        return str(sk.get_path_name()) if sk else "None"
    except Exception as e:
        return "err " + str(e)

for nm in ["IK_UE4Mann", "IK_MCUE5v2", "IK_Operator", "IK_UEFN"]:
    p = "/Game/Characters/Operator/Rig/%s.%s" % (nm, nm)
    rig = unreal.load_asset(p)
    R["rigs"][nm] = rig_skel(rig) if rig else "missing"

for nm in ["RTG_MCO_to_Operator", "RTG_MC_to_Operator", "RTG_UEFN_to_Operator"]:
    p = "/Game/Characters/Operator/Rig/%s.%s" % (nm, nm)
    rt = unreal.load_asset(p)
    info = {}
    if rt:
        try:
            c = unreal.IKRetargeterController.get_controller(rt)
            srig = c.get_ik_rig(unreal.RetargetSourceOrTarget.SOURCE)
            trig = c.get_ik_rig(unreal.RetargetSourceOrTarget.TARGET)
            info["source_rig"] = str(srig.get_path_name()) if srig else "None"
            info["target_rig"] = str(trig.get_path_name()) if trig else "None"
            info["source_skel"] = rig_skel(srig) if srig else "None"
        except Exception as e:
            info["err"] = str(e)
    R["retargeters"][nm] = info

# find ASP skeletal meshes
for a in ar.get_assets(unreal.ARFilter(class_names=["SkeletalMesh"],
        package_paths=["/Game/AnimStarterPack"], recursive_paths=True)):
    R["asp_meshes"].append(str(a.package_name) + "." + str(a.asset_name))
R["asp_skel"] = "/Game/AnimStarterPack/UE4_Mannequin/Mesh/UE4_Mannequin_Skeleton"

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("INSPECT_RIGS done")
