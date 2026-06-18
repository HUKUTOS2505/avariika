# -*- coding: utf-8 -*-
# Переносим НЕиспользуемые игрой паки в /Game/_Packs/ (по аудиту зависимостей).
# Пошаговый лог в файл (краш-устойчиво). Ссылки чинит сам редактор. Уровень не трогаем.
import unreal, json, traceback

PACKS = [
    "ActorCore_Sample_Motions", "AnimStarterPack", "Attaku", "CharacterEmotes",
    "Construction_Pit", "Construction_VOL2", "DynamicFalling", "Fire_EXP_Vol01_Free",
    "FootstepSystem", "FreeLadderAnimationSet", "Free_Crawl_Animation", "Free_Interaction_Animation",
    "Garage_Tools_Props", "GoreAndHorrorMegapack", "HospitalCombo", "Hyper",
    "Interaction", "InteractionKitVol1", "ItemConsumableAnims", "JKMotion_HitReaction",
    "KnockedDown", "MCO_Mocap_Basics", "MC_Sample", "ResourcePack",
    "ScifiWorkerAnimset", "Survival_Horror_Props_Pack", "WorkAnimations", "ZombieAnimationPack",
]
LOG = r"D:/unrealEngine/avariika/Scripts/manifests/_reorg_progress.txt"
RES = r"D:/unrealEngine/avariika/Scripts/manifests/_reorg_free.json"

def logline(s):
    with open(LOG, "a", encoding="utf-8") as f:
        f.write(s + "\n")

R = {"moved": [], "skipped": [], "failed": []}
eal = unreal.EditorAssetLibrary
logline("=== START move free packs ===")
for name in PACKS:
    src = "/Game/" + name
    dst = "/Game/_Packs/" + name
    try:
        if not eal.does_directory_exist(src):
            R["skipped"].append(name); logline("SKIP (нет) %s" % name); continue
        n = len(eal.list_assets(src, recursive=True, include_folder=False))
        ok = eal.rename_directory(src, dst)
        if ok and eal.does_directory_exist(dst):
            eal.save_directory(dst, only_if_is_dirty=False, recursive=True)
            R["moved"].append({"name": name, "assets": n})
            logline("MOVED %s (%d ассетов)" % (name, n))
        else:
            R["failed"].append({"name": name, "reason": "rename_ok=%s" % ok})
            logline("FAIL %s (rename_ok=%s)" % (name, ok))
    except Exception as e:
        R["failed"].append({"name": name, "reason": str(e)})
        logline("EXC %s: %s" % (name, e))
logline("=== DONE moved=%d skip=%d fail=%d ===" % (len(R["moved"]), len(R["skipped"]), len(R["failed"])))
with open(RES, "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("REORG_FREE done")
