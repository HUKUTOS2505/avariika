import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/manifests/_srcmesh.json"
ar = unreal.AssetRegistryHelpers.get_asset_registry()
R = {}
def find_src(root):
    d = {"meshes": [], "anim_skel": {}}
    for a in ar.get_assets(unreal.ARFilter(class_names=["SkeletalMesh"], package_paths=[root], recursive_paths=True)):
        d["meshes"].append(str(a.package_name) + "." + str(a.asset_name))
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], package_paths=[root], recursive_paths=True)):
        try: sk = str(a.get_tag_value("Skeleton"))
        except Exception: sk = ""
        d["anim_skel"][sk] = d["anim_skel"].get(sk, 0) + 1
    return d
for r in ["/Game/ItemConsumableAnims", "/Game/KnockedDown", "/Game/JKMotion_HitReaction", "/Game/Loot_Anim_Set", "/Game/ScifiWorkerAnimset"]:
    R[r] = find_src(r)
with open(OUT, "w") as fp: json.dump(R, fp, indent=1)
unreal.log("PROBE_SRCMESH done")
