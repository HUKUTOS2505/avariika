# -*- coding: utf-8 -*-
# Доводим переезд до чистого состояния:
#  • пустые папки в корне -> удалить;
#  • GoreAndHorror (не переехал) -> почистить частичный dest и перенести целиком;
#  • прочие остатки (общие меши манекена) -> перенести поассетно в /Game/_Packs/<name>,
#    затем удалить опустевшую корневую папку (там остаются лишь редиректоры — ссылки уже
#    переправлены rename_asset, поэтому delete безопасен).
import unreal, json, traceback
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
LOG = r"C:/unrealEngine/avariika/Scripts/manifests/_consolidate_progress.txt"
RES = r"C:/unrealEngine/avariika/Scripts/manifests/_consolidate.json"

def logline(s):
    with open(LOG, "a", encoding="utf-8") as f: f.write(s + "\n")

def real_count(folder):
    n = 0
    for d in ar.get_assets_by_path(folder, recursive=True, include_only_on_disk_assets=False):
        try: cls = str(d.asset_class_path.asset_name)
        except Exception: cls = "?"
        if cls != "ObjectRedirector": n += 1
    return n

R = {"deleted_empty": [], "gore": None, "consolidated": [], "left": [], "err": None}
EMPTIES = ["Construction_Pit", "Hyper"]
LEFT = ["ActorCore_Sample_Motions","AnimStarterPack","Attaku","CharacterEmotes","DynamicFalling",
        "FreeLadderAnimationSet","Free_Crawl_Animation","Free_Interaction_Animation","Interaction",
        "InteractionKitVol1","ItemConsumableAnims","JKMotion_HitReaction","KnockedDown",
        "MCO_Mocap_Basics","MC_Sample","ResourcePack","ScifiWorkerAnimset","WorkAnimations","ZombieAnimationPack"]
logline("=== CONSOLIDATE START ===")
try:
    # 1) пустые
    for name in EMPTIES:
        f = "/Game/" + name
        if eal.does_directory_exist(f) and real_count(f) == 0:
            eal.delete_directory(f); R["deleted_empty"].append(name); logline("EMPTY del %s" % name)

    # 2) GoreAndHorror — перенести целиком заново
    gp, gd = "/Game/GoreAndHorrorMegapack", "/Game/_Packs/GoreAndHorrorMegapack"
    if eal.does_directory_exist(gp):
        if eal.does_directory_exist(gd):
            eal.delete_directory(gd); logline("Gore: cleared partial dest")
        ok = eal.rename_directory(gp, gd)
        eal.save_directory(gd, only_if_is_dirty=False, recursive=True)
        R["gore"] = "rename=%s" % ok; logline("Gore rename=%s" % ok)

    # 3) остальные остатки — поассетно
    for name in LEFT:
        f = "/Game/" + name
        if not eal.does_directory_exist(f): continue
        assets = eal.list_assets(f, recursive=True, include_folder=False)
        fails = 0
        for a in assets:
            seg = a.rsplit("/", 1)
            nm = seg[1].split(".")[0]
            src = seg[0] + "/" + nm
            dst = src.replace("/Game/" + name + "/", "/Game/_Packs/" + name + "/", 1)
            try:
                eal.rename_asset(src, dst)
            except Exception as e:
                fails += 1
        # если реальных ассетов не осталось — удалить папку (вычистит редиректоры)
        if real_count(f) == 0:
            eal.delete_directory(f); R["consolidated"].append({"name": name, "fails": fails})
            logline("CONSOLIDATED %s (fails=%d)" % (name, fails))
        else:
            R["left"].append({"name": name, "real": real_count(f), "fails": fails})
            logline("LEFT %s real=%d fails=%d" % (name, real_count(f), fails))
    logline("=== CONSOLIDATE DONE ===")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc(); logline("EXC %s" % e)
with open(RES, "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("REORG_CONSOLIDATE done")
