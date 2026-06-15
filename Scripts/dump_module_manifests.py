import unreal, json, os
OUTDIR = r"D:/unrealEngine/avariika/Scripts/manifests"
ar = unreal.AssetRegistryHelpers.get_asset_registry()
MODS = {
    "zombie": "/Game/ZombieAnimationPack",
    "crawl": "/Game/Free_Crawl_Animation",
    "ladder": "/Game/FreeLadderAnimationSet",
    "free_interaction": "/Game/Free_Interaction_Animation",
    "interaction": "/Game/Interaction",
    "dynfalling": "/Game/DynamicFalling",
}
summary = {}
for key, path in MODS.items():
    names = []
    for cls in ["AnimSequence", "AnimMontage", "BlendSpace", "BlendSpace1D"]:
        f = unreal.ARFilter(class_names=[cls], package_paths=[path], recursive_paths=True)
        for a in ar.get_assets(f):
            names.append(cls[:4] + ":" + str(a.asset_name))
    names = sorted(set(names))
    with open(os.path.join(OUTDIR, "mod_%s.txt" % key), "w") as fp:
        fp.write("\n".join(names))
    summary[key] = len(names)
with open(os.path.join(OUTDIR, "_modules_summary.json"), "w") as fp:
    json.dump(summary, fp, indent=1)
unreal.log("DUMP_MODULE_MANIFESTS done " + json.dumps(summary))
