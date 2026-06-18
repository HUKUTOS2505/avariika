import unreal
import traceback

path = "D:/unrealEngine/avariika/Saved/casttime_dump.txt"
lines = ["probe start"]


def flush():
    with open(path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))


flush()
try:
    reg = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = reg.get_assets_by_path("/Game", recursive=True)
    lines.append("assets: %d" % len(assets))
    flush()
    rows = []
    for a in assets:
        try:
            acn = str(a.asset_class_path.asset_name)
        except Exception:
            acn = ""
        if acn != "Blueprint":
            continue
        obj = a.get_asset()
        if not isinstance(obj, unreal.Blueprint):
            continue
        gc = obj.generated_class()
        if not gc:
            continue
        try:
            cdo = unreal.get_default_object(gc)
        except Exception:
            continue
        if not isinstance(cdo, unreal.PickupItem):
            continue
        try:
            eff = cdo.get_editor_property("ItemEffect")
            cast = cdo.get_editor_property("UseCastTime")
            nm = cdo.get_editor_property("DisplayName")
            rows.append("%-30s effect=%-14s cast=%.2f  (%s)" % (str(a.asset_name), str(eff), cast, nm))
        except Exception as e:
            rows.append("%s ERR %s" % (a.asset_name, e))
    lines.append("=== APickupItem BPs: %d ===" % len(rows))
    lines.extend(sorted(rows))
    flush()
except Exception:
    lines.append("EXC:\n" + traceback.format_exc())
    flush()
