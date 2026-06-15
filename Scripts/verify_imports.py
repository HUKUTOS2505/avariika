import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/manifests/_verify_imports.json"
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ROOTS = ["/Game/AnimX", "/Game/JKMotion_HitReaction", "/Game/KnockedDown",
         "/Game/InteractionKitVol1", "/Game/ScifiWorkerAnimset", "/Game/WorkAnimations",
         "/Game/ItemConsumableAnims", "/Game/Loot_Anim_Set", "/Game/StorageUnitsSet"]
ar.scan_paths_synchronous(ROOTS, True, False)
R = {"roots": {}, "dog_check": {}}
for root in ROOTS:
    d = {}
    for cls in ["AnimSequence", "AnimMontage", "SkeletalMesh", "Skeleton", "AnimBlueprint", "BlendSpace", "Blueprint"]:
        f = unreal.ARFilter(class_names=[cls], package_paths=[root], recursive_paths=True)
        d[cls] = len(list(ar.get_assets(f)))
    R["roots"][root] = d
# dog deep check: resolve skeleton + mesh + animbp + one anim
checks = {
 "skel": "/Game/AnimX/Dogs/Meshes/Dog_GH_Skeleton.Dog_GH_Skeleton",
 "mesh": "/Game/AnimX/Dogs/Meshes/SK_Dog_R.SK_Dog_R",
 "bite": None, "animbp": None,
}
# find an attack anim + the animbp by name
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/AnimX"], recursive_paths=True)):
    if str(a.asset_name) == "A_Dog_Combat_Attack_Bite_R":
        checks["bite"] = str(a.package_name) + "." + str(a.asset_name)
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimBlueprint"], package_paths=["/Game/AnimX"], recursive_paths=True)):
    checks["animbp"] = str(a.package_name) + "." + str(a.asset_name); break
for k, p in checks.items():
    if p:
        try: R["dog_check"][k] = bool(unreal.load_asset(p))
        except Exception as e: R["dog_check"][k] = "err " + str(e)
    else: R["dog_check"][k] = "not found"
with open(OUT, "w") as fp: json.dump(R, fp, indent=1)
unreal.log("VERIFY_IMPORTS done")
