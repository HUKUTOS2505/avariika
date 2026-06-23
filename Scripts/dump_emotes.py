import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/manifests/mod_charemotes.txt"
JOUT = r"C:/unrealEngine/avariika/Scripts/manifests/_charemotes.json"
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game/CharacterEmotes"], True, False)
R = {"anims": [], "skeletons": {}, "meshes": [], "other": {}}
for cls in ["AnimSequence", "AnimMontage"]:
    f = unreal.ARFilter(class_names=[cls], package_paths=["/Game/CharacterEmotes"], recursive_paths=True)
    for a in ar.get_assets(f):
        nm = str(a.asset_name)
        try: sk = str(a.get_tag_value("Skeleton"))
        except Exception: sk = ""
        R["anims"].append(nm)
        R["skeletons"][sk] = R["skeletons"].get(sk, 0) + 1
for a in ar.get_assets(unreal.ARFilter(class_names=["SkeletalMesh"], package_paths=["/Game/CharacterEmotes"], recursive_paths=True)):
    R["meshes"].append(str(a.package_name) + "." + str(a.asset_name))
R["anims"] = sorted(set(R["anims"]))
with open(OUT, "w") as fp: fp.write("\n".join(R["anims"]))
with open(JOUT, "w") as fp: json.dump(R, fp, indent=1)
unreal.log("DUMP_EMOTES done %d anims" % len(R["anims"]))
