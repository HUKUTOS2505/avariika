import unreal, json, traceback
out = {}
try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    name2path = {}
    for a in ar.get_assets_by_path(unreal.Name("/Game/ResidentialHouses"), recursive=True):
        try:
            cls = str(a.asset_class_path.asset_name)
        except Exception:
            cls = ""
        if cls == "StaticMesh":
            full = str(a.package_name)
            name2path[full.split("/")[-1]] = full
    def bnd(nm):
        full = name2path.get(nm)
        if not full:
            return {"exists": False}
        m = unreal.load_asset(full)
        if not m:
            return {"exists": False, "path": full}
        b = m.get_bounds(); o = b.box_extent; org = b.origin
        return {"path": full,
                "size": [round(o.x*2,1), round(o.y*2,1), round(o.z*2,1)],
                "origin": [round(org.x,1), round(org.y,1), round(org.z,1)]}
    for nm in ["SM_Garage_Inside_1m","SM_Garage_Inside_2m","SM_Garage_Inside_3m",
               "SM_Garage_Inside_3m_Window","SM_Garage_Inside_Corner_1m",
               "SM_Garage_Inside_Corner_2m","SM_Garage_Inside_Corner_3m",
               "SM_Garage_Stairs","SM_Outside_GarageDoor_8m",
               "SM_Ceiling_1m_a","SM_Ceiling_2m_a","SM_Ceiling_3m",
               "SM_Floor_1x1m","SM_CeilingLamp","SM_PendantCeilingLamp",
               "SM_Outside_3m","SM_Outside_Corner_1m"]:
        out[nm] = bnd(nm)
except Exception:
    out["fatal"] = traceback.format_exc()
open("C:/unrealEngine/avariika/Saved/build_probe3.json", "w").write(json.dumps(out, indent=2))
