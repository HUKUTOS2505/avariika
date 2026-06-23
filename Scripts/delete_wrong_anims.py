import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/delete_wrong_anims.json"
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game/Characters/Operator"], True, False)
deleted = []
for ad in ar.get_assets_by_path("/Game/Characters/Operator/Anims", recursive=True):
    op = str(ad.package_name) + "." + str(ad.asset_name)
    obj = unreal.load_asset(op)
    if obj and obj.get_class().get_name() == "SkeletalMesh":
        if eal.delete_asset(str(ad.package_name)):
            deleted.append(str(ad.asset_name))
with open(OUT, "w") as f:
    json.dump({"deleted": len(deleted), "names": deleted}, f, indent=1)
unreal.log("DELETE_WRONG_ANIMS: %d" % len(deleted))
