# Верификация вытащенного контента: счётчики ассетов + битые зависимости в новых корнях.
# Пишет итог в Scripts/verify_result.txt (Claudius не захватывает stdout).
# Запуск: editor.run_python_script script_path=...
import unreal

ROOTS = [
    "/Game/NextGenDestruction",
    "/Game/GothicTexture_Meshingun",
    "/Game/Foley_Props_Sounds",
    "/Game/SFX",
]

areg = unreal.AssetRegistryHelpers.get_asset_registry()
try:
    areg.scan_paths_synchronous(ROOTS, force_rescan=True)
except Exception as e:
    unreal.log_warning("scan_paths: %s" % e)

lines = []
total_assets = 0
total_broken = 0
for root in ROOTS:
    assets = unreal.EditorAssetLibrary.list_assets(root, recursive=True, include_folder=False)
    n = len(assets)
    total_assets += n
    broken = []
    for a in assets:
        pkg = a.split(".")[0]
        try:
            deps = areg.get_dependencies(unreal.Name(pkg), unreal.AssetRegistryDependencyOptions(include_hard_package_references=True))
        except Exception:
            deps = None
        if deps:
            for d in deps:
                ds = str(d)
                if ds.startswith("/Game/"):
                    if not unreal.EditorAssetLibrary.does_directory_exist(ds) and not unreal.EditorAssetLibrary.does_asset_exist(ds):
                        broken.append(pkg + " -> " + ds)
    total_broken += len(broken)
    lines.append("%-35s assets=%-5d broken_deps=%d" % (root, n, len(broken)))
    for b in broken[:8]:
        lines.append("     BROKEN: " + b)

# Подсчёт по подпапкам SFX
for sub in ["/Game/SFX/Sonomar_Asylum/Ambience","/Game/SFX/Sonomar_Asylum/Debris","/Game/SFX/Ghosthack/Explosion_Gas","/Game/SFX/Ghosthack/Explosion_Indoor","/Game/SFX/Ghosthack/Demolition","/Game/SFX/Ghosthack/Implosion"]:
    if unreal.EditorAssetLibrary.does_directory_exist(sub):
        c = len(unreal.EditorAssetLibrary.list_assets(sub, recursive=False, include_folder=False))
        lines.append("  SFX %-45s = %d" % (sub, c))

summary = "TOTAL assets=%d broken_deps=%d" % (total_assets, total_broken)
lines.append(summary)
text = "\n".join(lines)
for ln in lines:
    unreal.log(ln)
with open(r"C:/unrealEngine/avariika/Scripts/verify_result.txt", "w", encoding="utf-8") as f:
    f.write(text + "\n")
print(summary)
