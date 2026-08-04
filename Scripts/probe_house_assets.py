import unreal, json, traceback
out = {}
try:
    ues = unreal.UnrealEditorSubsystem()
    w = ues.get_editor_world()
    out["world"] = w.get_name() if w else None
    ar = unreal.AssetRegistryHelpers.get_asset_registry()

    def list_meshes(path):
        res = []
        for a in ar.get_assets_by_path(unreal.Name(path), recursive=True):
            try:
                cls = str(a.asset_class_path.asset_name)
            except Exception:
                cls = str(getattr(a, "asset_class", ""))
            if cls == "StaticMesh":
                res.append(str(a.package_name).split("/")[-1])
        return sorted(set(res))

    out["arch_walls"] = list_meshes("/Game/ResidentialHouses/Meshes/Architecture/Walls")
    out["arch_floors"] = list_meshes("/Game/ResidentialHouses/Meshes/Architecture/Floors")
    out["arch_ceilings"] = list_meshes("/Game/ResidentialHouses/Meshes/Architecture/Ceilings")
    out["arch_foundation"] = list_meshes("/Game/ResidentialHouses/Meshes/Architecture/Foundation")

    def bnd(pth):
        m = unreal.load_asset(pth)
        if not m:
            return {"exists": False}
        b = m.get_bounds()
        o = b.box_extent
        org = b.origin
        return {"exists": True,
                "origin": [round(org.x, 1), round(org.y, 1), round(org.z, 1)],
                "extent": [round(o.x, 1), round(o.y, 1), round(o.z, 1)],
                "size": [round(o.x * 2, 1), round(o.y * 2, 1), round(o.z * 2, 1)]}

    probe = {}
    base = "/Game/ResidentialHouses/Meshes/Architecture/"
    for nm in ["Walls/SM_Outside_1m", "Walls/SM_Outside_2m", "Walls/SM_Outside_3m",
               "Walls/SM_Inside_1m", "Walls/SM_Inside_2m", "Walls/SM_Inside_3m",
               "Floors/SM_Floor_1x1m", "Ceilings/SM_Ceiling_2m_a"]:
        try:
            probe[nm] = bnd(base + nm)
        except Exception as e:
            probe[nm] = {"err": str(e)}
    out["probe"] = probe
except Exception:
    out["fatal"] = traceback.format_exc()
open("C:/unrealEngine/avariika/Saved/build_probe.json", "w").write(json.dumps(out, indent=2))
