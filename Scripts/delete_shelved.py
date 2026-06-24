# Удаляет из Content частичные копии паков, уходящих в _KeptPacks (NextGenDestruction, GothicTexture).
# Итог -> Scripts/shelve_result.txt
import unreal

DIRS = ["/Game/NextGenDestruction", "/Game/GothicTexture_Meshingun"]
lines = []
for d in DIRS:
    existed = unreal.EditorAssetLibrary.does_directory_exist(d)
    n = len(unreal.EditorAssetLibrary.list_assets(d, recursive=True, include_folder=False)) if existed else 0
    ok = unreal.EditorAssetLibrary.delete_directory(d) if existed else True
    still = unreal.EditorAssetLibrary.does_directory_exist(d)
    lines.append("%s: existed=%s assets=%d deleted_ok=%s still_exists=%s" % (d, existed, n, ok, still))

text = "\n".join(lines)
for ln in lines:
    unreal.log(ln)
with open(r"C:/unrealEngine/avariika/Scripts/shelve_result.txt", "w", encoding="utf-8") as f:
    f.write(text + "\n")
print("SHELVE_DONE")
