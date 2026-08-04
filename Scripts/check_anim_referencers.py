# Анализ референсеров для кандидатов на удаление (чтобы не сломать ABP/BP).
# Для каждой папки: сколько ассетов, сколько референсимы ИЗВНЕ папки, примеры внешних референсеров.
# Итог -> Scripts/anim_referencers.txt
import unreal

areg = unreal.AssetRegistryHelpers.get_asset_registry()
opts = unreal.AssetRegistryDependencyOptions(include_hard_package_references=True, include_soft_package_references=True)

CANDIDATES = [
    "/Game/AnimX",                         # собака
    "/Game/Testing",                       # n00dEmotes тест
    "/Game/Loot_Anim_Set",                 # UE4-mann лут/грэб (+ битый файл)
    "/Game/hazmat",                        # UE4-mann
    "/Game/_Packs/ZombieAnimationPack",    # монстр/хоррор
    "/Game/_Packs/GoreAndHorrorMegapack",
    "/Game/_Packs/oldBenchPack",           # "old"
    "/Game/_Packs/AnimStarterPack",        # дубль локомоции?
    "/Game/_Packs/ScifiWorkerAnimset",
    "/Game/_Packs/MCO_Mocap_Basics",
    "/Game/_Packs/Attaku",
    "/Game/_Packs/DynamicFalling",
]

lines = []
for folder in CANDIDATES:
    if not unreal.EditorAssetLibrary.does_directory_exist(folder):
        lines.append("%-42s (НЕТ ПАПКИ)" % folder)
        continue
    assets = unreal.EditorAssetLibrary.list_assets(folder, recursive=True, include_folder=False)
    ext_refs = set()
    for a in assets:
        pkg = a.split(".")[0]
        try:
            refs = areg.get_referencers(unreal.Name(pkg), opts) or []
        except Exception:
            refs = []
        for r in refs:
            rs = str(r)
            if not rs.startswith(folder) and rs.startswith("/Game/"):
                ext_refs.add(rs)
    sample = list(ext_refs)[:6]
    verdict = "SAFE-DELETE" if not ext_refs else "REFERENCED"
    lines.append("%-42s assets=%-4d ext_refs=%-3d  %s" % (folder, len(assets), len(ext_refs), verdict))
    for s in sample:
        lines.append("      <- " + s)

text = "\n".join(lines)
for ln in lines:
    unreal.log(ln)
with open(r"C:/unrealEngine/avariika/Scripts/anim_referencers.txt", "w", encoding="utf-8") as f:
    f.write(text + "\n")
print("REFERENCER_CHECK_DONE")
