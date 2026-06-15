import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/manifests/_anim_setup.json"
ar = unreal.AssetRegistryHelpers.get_asset_registry()
R = {"anim_bps": [], "char_bps": [], "op_skeleton": "", "blendspaces": [], "operator_assets": []}

# operator skeleton path
for a in ar.get_assets(unreal.ARFilter(class_names=["Skeleton"], package_paths=["/Game/Characters/Operator"], recursive_paths=True)):
    R["op_skeleton"] = str(a.package_name) + "." + str(a.asset_name)

# all AnimBlueprints + their target skeleton tag
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimBlueprint"], recursive_paths=True)):
    try: sk = str(a.get_tag_value("TargetSkeleton"))
    except Exception: sk = ""
    R["anim_bps"].append({"name": str(a.asset_name), "pkg": str(a.package_name), "skel": sk})

# blendspaces on operator skeleton
for cls in ["BlendSpace", "BlendSpace1D"]:
    for a in ar.get_assets(unreal.ARFilter(class_names=[cls], recursive_paths=True)):
        try: sk = str(a.get_tag_value("TargetSkeleton"))
        except Exception: sk = ""
        if "Operator" in sk or "SK_Operator" in str(a.package_name):
            R["blendspaces"].append(str(a.package_name) + "." + str(a.asset_name))

# blueprints that are Character/Pawn (the player) — scan generated class parent
for a in ar.get_assets(unreal.ARFilter(class_names=["Blueprint"], recursive_paths=True)):
    nm = str(a.asset_name); pkg = str(a.package_name)
    low = (nm + pkg).lower()
    if any(k in low for k in ["operator", "player", "character", "pawn", "bp_first", "avaryo", "repairer"]):
        try: parent = str(a.get_tag_value("ParentClass"))
        except Exception: parent = ""
        R["char_bps"].append({"name": nm, "pkg": pkg, "parent": parent})

# what's in the operator folder root
for a in ar.get_assets(unreal.ARFilter(package_paths=["/Game/Characters/Operator"], recursive_paths=False)):
    R["operator_assets"].append(str(a.asset_class_path.asset_name) + ":" + str(a.asset_name) if hasattr(a,"asset_class_path") else str(a.asset_name))

with open(OUT, "w") as f: json.dump(R, f, indent=1)
unreal.log("INSPECT_ANIM_SETUP done")
