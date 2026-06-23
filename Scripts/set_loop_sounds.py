import unreal

out = "C:/unrealEngine/avariika/Saved/set_loop_sounds.txt"
lines = []
for name in ["Ambient_Boiler", "Heartbeat"]:
    path = "/Game/Audio/SFX/%s" % name
    sw = unreal.load_asset(path)
    if not sw:
        lines.append("%s: NOT FOUND" % name); continue
    try:
        sw.set_editor_property("looping", True)
        unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
        lines.append("%s: looping=%s, class=%s" % (name, sw.get_editor_property("looping"), sw.get_class().get_name()))
    except Exception as e:
        lines.append("%s ERR %s" % (name, e))

# also list what's in /Game/Audio/SFX
reg = unreal.AssetRegistryHelpers.get_asset_registry()
reg.scan_paths_synchronous(["/Game/Audio"], force_rescan=True)
for a in reg.get_assets_by_path("/Game/Audio", recursive=True):
    lines.append("  asset: %s" % a.get_full_name())

open(out, "w", encoding="utf-8").write("\n".join(lines))
print("\n".join(lines))
