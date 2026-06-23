import unreal

paths = ["/Game/Hyper"]
reg = unreal.AssetRegistryHelpers.get_asset_registry()
reg.scan_paths_synchronous(paths, force_rescan=True)

out = []
for p in paths:
    assets = reg.get_assets_by_path(p, recursive=True)
    out.append("%s : %d assets" % (p, len(assets)))

# locate the weather manager / sky BPs
wm = reg.get_assets_by_path("/Game/Hyper/Core/WeatherManager", recursive=True)
out.append("WeatherManager assets: %d" % len(wm))
names = sorted(str(a.asset_name) for a in wm if "anager" in str(a.asset_name) or "BP_" in str(a.asset_name))
out.append("  key: " + ", ".join(names[:12]))

text = "\n".join(out)
with open("C:/unrealEngine/avariika/Saved/scan_new_packs.txt", "w", encoding="utf-8") as f:
    f.write(text)
print(text)
