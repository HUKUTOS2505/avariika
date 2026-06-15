import unreal, json
ar=unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game/hazmat"],True,False)
R={}
for cls in ["SkeletalMesh","Skeleton","AnimBlueprint","AnimSequence","Blueprint"]:
    items=[str(a.package_name)+"."+str(a.asset_name) for a in ar.get_assets(unreal.ARFilter(class_names=[cls],package_paths=["/Game/hazmat"],recursive_paths=True))]
    R[cls]=items
# skeleton of the skeletal mesh + the player's current skeleton for comparison
sm=ar.get_assets(unreal.ARFilter(class_names=["SkeletalMesh"],package_paths=["/Game/hazmat"],recursive_paths=True))
R["hazmat_mesh_skel"]={}
for a in sm:
    try: R["hazmat_mesh_skel"][str(a.asset_name)]=str(a.get_tag_value("Skeleton"))
    except Exception: pass
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_hazmat.json","w") as f: json.dump(R,f,indent=1)
unreal.log("INSPECT_HAZMAT done")
