import unreal, json
JOUT = r"C:/unrealEngine/avariika/Scripts/manifests/_new2.json"
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game/Attaku", "/Game/ActorCore_Sample_Motions"], True, False)
R = {}
def dump(path):
    d = {"by_class": {}, "skeletons": {}, "anims": [], "poses": [], "meshes": []}
    for cls in ["AnimSequence", "AnimMontage", "PoseAsset", "BlendSpace", "BlendSpace1D", "SkeletalMesh", "Skeleton"]:
        f = unreal.ARFilter(class_names=[cls], package_paths=[path], recursive_paths=True)
        items = list(ar.get_assets(f))
        d["by_class"][cls] = len(items)
        for a in items:
            nm = str(a.asset_name)
            if cls in ("AnimSequence", "AnimMontage"):
                d["anims"].append(nm)
                try: sk = str(a.get_tag_value("Skeleton"))
                except Exception: sk = ""
                d["skeletons"][sk] = d["skeletons"].get(sk, 0) + 1
            elif cls == "PoseAsset":
                d["poses"].append(nm)
            elif cls in ("SkeletalMesh", "Skeleton"):
                d["meshes"].append(cls + ":" + str(a.package_name) + "." + nm)
    d["anims"] = sorted(set(d["anims"]))
    d["poses"] = sorted(set(d["poses"]))
    return d
R["Attaku"] = dump("/Game/Attaku")
R["ActorCore"] = dump("/Game/ActorCore_Sample_Motions")
with open(JOUT, "w") as fp: json.dump(R, fp, indent=1)
unreal.log("DUMP_NEW2 done")
