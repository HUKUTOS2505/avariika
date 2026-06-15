import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/inventory_asp.json"
R = {"asp_anims": [], "asp_skeletons": {}, "retarget_count": 0, "retarget_list": [],
     "rigs": [], "retargeters": [], "asp_paths": []}
ar = unreal.AssetRegistryHelpers.get_asset_registry()
eal = unreal.EditorAssetLibrary

# find AnimStarterPack folder(s)
for top in ["/Game/AnimStarterPack", "/Game/AnimationStarterPack", "/Game/ASP",
            "/Game/Mannequin", "/Game/CharacterAnimationStarter"]:
    if eal.does_directory_exist(top):
        R["asp_paths"].append(top)

# broad scan: any AnimSequence whose path hints starter pack / mannequin
allseq = ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True))
for a in allseq:
    pkg = str(a.package_name)
    low = pkg.lower()
    if "starter" in low or "/asp" in low or ("mannequin" in low and "uefn" not in low and "mcue5" not in low):
        nm = str(a.asset_name)
        try:
            tags = a.get_tag_value("Skeleton")
        except Exception:
            tags = ""
        R["asp_anims"].append({"name": nm, "pkg": pkg, "skel": str(tags)})

# tally skeletons used
for x in R["asp_anims"]:
    s = x["skel"]
    R["asp_skeletons"][s] = R["asp_skeletons"].get(s, 0) + 1

# count current Op_ retargets
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"],
        package_paths=["/Game/Characters/Operator/Anims_Retarget"], recursive_paths=True)):
    R["retarget_list"].append(str(a.asset_name))
R["retarget_count"] = len(R["retarget_list"])
R["retarget_list"].sort()

# list existing IK rigs + retargeters
for a in ar.get_assets(unreal.ARFilter(package_paths=["/Game/Characters/Operator/Rig"], recursive_paths=True)):
    cn = str(a.asset_class_path.asset_name) if hasattr(a, "asset_class_path") else ""
    nm = str(a.asset_name)
    if "Rig" in nm or "IK_" in nm:
        R["rigs"].append(nm)
    if "RTG" in nm:
        R["retargeters"].append(nm)

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("INVENTORY_ASP done")
