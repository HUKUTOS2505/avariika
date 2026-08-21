import unreal, json, traceback
out = {}
try:
    W = "/Game/ResidentialHouses/Meshes/Architecture/Walls/"
    def bnd(full):
        m = unreal.load_asset(full)
        if not m:
            return {"exists": False}
        b = m.get_bounds(); o = b.box_extent; org = b.origin
        return {"size": [round(o.x*2,1), round(o.y*2,1), round(o.z*2,1)],
                "origin": [round(org.x,1), round(org.y,1), round(org.z,1)]}
    for nm in ["SM_Garage_Inside_1m","SM_Garage_Inside_2m","SM_Garage_Inside_3m",
               "SM_Garage_Inside_3m_Window","SM_Garage_Inside_4m_Window",
               "SM_Garage_Inside_Corner_1m","SM_Garage_Inside_Corner_2m","SM_Garage_Inside_Corner_3m",
               "SM_Garage_Int_Corner_1m","SM_Garage_Stairs",
               "SM_Outside_GarageDoor_8m","SM_Outside_Door_2m","SM_Outside_Corner_1m"]:
        out[nm] = bnd(W + nm)
    # find ceilings + garage props anywhere in pack
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    ceil, gprop = [], []
    for a in ar.get_assets_by_path(unreal.Name("/Game/ResidentialHouses"), recursive=True):
        n = str(a.package_name).split("/")[-1]
        if "Ceiling" in n: ceil.append(n)
    out["ceilings_found"] = sorted(set(ceil))[:40]
except Exception:
    out["fatal"] = traceback.format_exc()
open("C:/unrealEngine/avariika/Saved/build_probe2.json", "w").write(json.dumps(out, indent=2))
