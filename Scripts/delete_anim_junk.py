# Удаляет безопасные (0 внешних ссылок) дубли/устаревшие аним-паки + битый Loot-файл.
# Итог -> Scripts/delete_junk_result.txt
import unreal

FOLDERS = [
    "/Game/_Packs/oldBenchPack",
    "/Game/_Packs/AnimStarterPack",
    "/Game/_Packs/MCO_Mocap_Basics",
    "/Game/_Packs/Attaku",
    "/Game/_Packs/DynamicFalling",
]
BROKEN = "/Game/Loot_Anim_Set/Animations/Paired_Loot/Paired_Loot_FlipOverCorpse_GrabItem_Vic2"

lines = []
for f in FOLDERS:
    if unreal.EditorAssetLibrary.does_directory_exist(f):
        n = len(unreal.EditorAssetLibrary.list_assets(f, recursive=True, include_folder=False))
        ok = unreal.EditorAssetLibrary.delete_directory(f)
        still = unreal.EditorAssetLibrary.does_directory_exist(f)
        lines.append("%-40s assets=%-4d deleted=%s still=%s" % (f, n, ok, still))
    else:
        lines.append("%-40s (НЕТ)" % f)

# битый файл — пробуем delete_asset
if unreal.EditorAssetLibrary.does_asset_exist(BROKEN):
    ok = unreal.EditorAssetLibrary.delete_asset(BROKEN)
    lines.append("BROKEN delete_asset ok=%s" % ok)
else:
    lines.append("BROKEN: does_asset_exist=False (возможно битый — удалить файл вручную)")

text = "\n".join(lines)
for ln in lines:
    unreal.log(ln)
with open(r"C:/unrealEngine/avariika/Scripts/delete_junk_result.txt", "w", encoding="utf-8") as f:
    f.write(text + "\n")
print("DELETE_JUNK_DONE")
