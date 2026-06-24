# Курированный импорт звука из RawAssets -> /Game/SFX (Sonomar хоррор-эмбиент + Ghosthack взрывы/газ).
# Идемпотентен: пропускает уже существующие ассеты. Запуск: exec(open(r"...").read())
import unreal, os, glob

RA = r"C:/unrealEngine/avariika/RawAssets"
SON = RA + "/Sonomar Collection_ Abandoned Asylum/SA0365 Pro Sound Effects - Sonomar Collection Abandoned Asylum/SA0365 Pro Sound Effects - Sonomar Collection Abandoned Asylum/Sonomar Collection Abandoned Asylum"
GH = RA + "/Ghosthack x Boom - Bundle/Explosives"

# (исходная папка, целевой /Game-путь, лимит файлов или None)
MAP = [
    (SON + "/Ambience - Room Tone", "/Game/SFX/Sonomar_Asylum/Ambience", 12),
    (SON + "/Debris - Misc",        "/Game/SFX/Sonomar_Asylum/Debris",   None),
    (GH + "/Explosion Gas",         "/Game/SFX/Ghosthack/Explosion_Gas", None),
    (GH + "/Explosion Indoor",      "/Game/SFX/Ghosthack/Explosion_Indoor", None),
    (GH + "/Demolition",            "/Game/SFX/Ghosthack/Demolition",    None),
    (GH + "/Implosion",             "/Game/SFX/Ghosthack/Implosion",     None),
]

tools = unreal.AssetToolsHelpers.get_asset_tools()
areg = unreal.AssetRegistryHelpers.get_asset_registry()

def sanitize(n):
    out = ""
    for ch in n:
        out += ch if (ch.isalnum() or ch in "_-") else "_"
    return out

tasks = []
planned = 0
skipped = 0
for src, dest, lim in MAP:
    if not os.path.isdir(src):
        unreal.log_warning("MISSING SRC: " + src)
        continue
    wavs = sorted(glob.glob(src + "/*.wav") + glob.glob(src + "/*.WAV"))
    if lim is not None:
        wavs = wavs[:lim]
    for w in wavs:
        base = sanitize(os.path.splitext(os.path.basename(w))[0])
        objpath = dest + "/" + base + "." + base
        if unreal.EditorAssetLibrary.does_asset_exist(dest + "/" + base):
            skipped += 1
            continue
        t = unreal.AssetImportTask()
        t.filename = w
        t.destination_path = dest
        t.destination_name = base
        t.automated = True
        t.replace_existing = False
        t.save = True
        tasks.append(t)
        planned += 1

unreal.log("SFX import: planned=%d skipped(existing)=%d" % (planned, skipped))
if tasks:
    tools.import_asset_tasks(tasks)

# Подсчёт результата
total = 0
for _, dest, _lim in MAP:
    n = len(unreal.EditorAssetLibrary.list_assets(dest, recursive=False, include_folder=False)) if unreal.EditorAssetLibrary.does_directory_exist(dest) else 0
    unreal.log("  %s -> %d assets" % (dest, n))
    total += n
unreal.log("SFX import DONE. total in /Game/SFX targets = %d" % total)
print("SFX_IMPORT_RESULT planned=%d skipped=%d total=%d" % (planned, skipped, total))
