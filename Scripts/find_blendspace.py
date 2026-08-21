import unreal
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game/Avariika/Anim"], True, False)
lines = []
for cls in ("BlendSpace", "BlendSpace1D"):
    for a in ar.get_assets(unreal.ARFilter(class_names=[cls], recursive_classes=True)):
        nm = str(a.asset_name)
        if "Loco_Mobility" in nm or "Mobility" in nm:
            pkg = str(a.package_name)
            lines.append("FOUND %s : %s" % (nm, pkg))
            bs = unreal.load_asset(pkg + "." + nm)
            if bs:
                for ax in ("horizontal_axis", "vertical_axis"):
                    try:
                        v = bs.get_editor_property(ax)
                        lines.append("   %s: name='%s' min=%s max=%s" % (ax, v.display_name, v.min, v.max))
                    except Exception as e:
                        lines.append("   %s err %s" % (ax, str(e)[:60]))
                try:
                    sd = bs.get_editor_property("sample_data")
                    lines.append("   sample_data: %d сэмплов" % len(sd))
                except Exception as e:
                    lines.append("   sample_data err %s" % str(e)[:60])
if not lines:
    lines.append("BlendSpace 'Mobility' не найден в реестре (возможно НЕ сохранён — нужен Ctrl+S)")
with open(r"C:/unrealEngine/avariika/Scripts/find_bs.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("FIND done")
