import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/inspect_uefn.json"
R = {}
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game/Characters/UEFN_Mannequin"], True, False)

# find skeletal mesh
f = unreal.ARFilter(class_names=["SkeletalMesh"], package_paths=["/Game/Characters/UEFN_Mannequin"], recursive_paths=True)
meshes = [str(a.package_name) for a in ar.get_assets(f)]
R["meshes"] = meshes
# anims copied
fa = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Characters/UEFN_Mannequin"], recursive_paths=True)
anims = [str(a.asset_name) for a in ar.get_assets(fa)]
R["anim_count"] = len(anims)
R["anims"] = sorted(anims)

# bones + parents of the mesh
if meshes:
    mp = meshes[0] + "." + meshes[0].split("/")[-1]
    sm = unreal.load_asset(mp)
    R["mesh_loaded"] = sm is not None
    if sm:
        comp = unreal.new_object(unreal.SkeletalMeshComponent)
        try: comp.set_skeletal_mesh_asset(sm)
        except Exception: comp.set_editor_property("skeletal_mesh", sm)
        R["bones"] = [str(comp.get_bone_name(i)) for i in range(comp.get_num_bones())][:80]
        par = {}
        for b in ["pelvis", "spine_01", "spine_05", "neck_01", "head", "clavicle_l", "upperarm_l", "hand_l", "thigh_l", "foot_l", "ball_l"]:
            try: par[b] = str(comp.get_parent_bone(b))
            except Exception as e: par[b] = "err"
        R["parents"] = par

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("INSPECT_UEFN done")
