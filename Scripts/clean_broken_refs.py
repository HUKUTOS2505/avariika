# Удаляет ассеты с битыми /Game-зависимостями в указанных корнях (демо-BP, чужие материалы).
# Сохраняет полезный контент. Итог -> Scripts/clean_result.txt
import unreal

ROOTS = ["/Game/NextGenDestruction", "/Game/GothicTexture_Meshingun"]
areg = unreal.AssetRegistryHelpers.get_asset_registry()
opts = unreal.AssetRegistryDependencyOptions(include_hard_package_references=True)

def broken_assets(root):
    out = []
    for a in unreal.EditorAssetLibrary.list_assets(root, recursive=True, include_folder=False):
        pkg = a.split(".")[0]
        try:
            deps = areg.get_dependencies(unreal.Name(pkg), opts) or []
        except Exception:
            deps = []
        for d in deps:
            ds = str(d)
            if ds.startswith("/Game/") and not unreal.EditorAssetLibrary.does_asset_exist(ds) and not unreal.EditorAssetLibrary.does_directory_exist(ds):
                out.append(pkg)
                break
    return out

lines = []
deleted = 0
for root in ROOTS:
    bad = broken_assets(root)
    lines.append("%s: битых ассетов=%d" % (root, len(bad)))
    for pkg in bad:
        ok = unreal.EditorAssetLibrary.delete_asset(pkg)
        lines.append(("  DEL %s" % pkg) + ("" if ok else "  (FAIL)"))
        if ok:
            deleted += 1

# Ре-скан и пересчёт битых
total_broken = 0
for root in ROOTS:
    rem = broken_assets(root)
    n = len(unreal.EditorAssetLibrary.list_assets(root, recursive=True, include_folder=False))
    total_broken += len(rem)
    lines.append("ПОСЛЕ %s: assets=%d, осталось битых=%d" % (root, n, len(rem)))

summary = "CLEAN_RESULT deleted=%d remaining_broken=%d" % (deleted, total_broken)
lines.append(summary)
for ln in lines:
    unreal.log(ln)
with open(r"C:/unrealEngine/avariika/Scripts/clean_result.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print(summary)
