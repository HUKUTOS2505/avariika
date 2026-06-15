import unreal, json
OUT=r"D:/unrealEngine/avariika/Scripts/manifests/_tp_set.json"
ar=unreal.AssetRegistryHelpers.get_asset_registry()
R={}
for base in ["/Game/Hospital/Free_Content_Epic_Games/Mannequin","/Game/AnimX/_Common/Demo/ThirdPersonBP/Mannequin"]:
    d={"anim_bp":[],"blendspace":[],"anims":[],"meshes":[]}
    for cls in ["AnimBlueprint","BlendSpace","BlendSpace1D","AnimSequence","SkeletalMesh"]:
        for a in ar.get_assets(unreal.ARFilter(class_names=[cls],package_paths=[base],recursive_paths=True)):
            p=str(a.package_name)+"."+str(a.asset_name)
            if cls=="AnimBlueprint": d["anim_bp"].append(p)
            elif "BlendSpace" in cls: d["blendspace"].append(p)
            elif cls=="SkeletalMesh": d["meshes"].append(p)
            else: d["anims"].append(str(a.asset_name))
    R[base]=d
with open(OUT,"w") as f: json.dump(R,f,indent=1)
unreal.log("FIND_TP done")
