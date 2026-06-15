import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/manifests/_probe_work.json"
ar = unreal.AssetRegistryHelpers.get_asset_registry()
R = {}
def probe(root, want_substr):
    d = {"meshes": [], "skeletons_of_anims": {}, "anim_hits": [], "spine_count": {}}
    for a in ar.get_assets(unreal.ARFilter(class_names=["SkeletalMesh"], package_paths=[root], recursive_paths=True)):
        d["meshes"].append(str(a.package_name) + "." + str(a.asset_name))
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], package_paths=[root], recursive_paths=True)):
        nm = str(a.asset_name)
        try: sk = str(a.get_tag_value("Skeleton"))
        except Exception: sk = ""
        d["skeletons_of_anims"][sk] = d["skeletons_of_anims"].get(sk, 0) + 1
        low = nm.lower()
        if any(w in low for w in want_substr):
            d["anim_hits"].append(nm)
    d["anim_hits"] = sorted(set(d["anim_hits"]))
    # spine bone count per skeletal mesh
    for mp in d["meshes"]:
        m = unreal.load_asset(mp)
        if m:
            try:
                sk = m.skeleton
                bones = [str(b) for b in sk.get_reference_pose().get_bone_names()] if hasattr(sk, "get_reference_pose") else []
            except Exception:
                bones = []
            if not bones:
                try:
                    bones = [str(n) for n in unreal.SkeletonHelpers and []]
                except Exception:
                    bones = []
            d["spine_count"][mp] = sum(1 for b in bones if b.lower().startswith("spine_"))
    return d
R["WorkAnimations"] = probe("/Game/WorkAnimations", ["drill", "fix", "nail", "jackhammer", "carry", "hang", "screw", "weld", "wrench"])
with open(OUT, "w") as fp: json.dump(R, fp, indent=1)
unreal.log("PROBE_WORK done")
