# -*- coding: utf-8 -*-
# Остаточные корневые папки = НЕreferenced дубликаты (оригиналы уже в _Packs). Удаляем ТОЛЬКО те,
# у которых ВСЕ ассеты без референсеров. Что-то держится — оставляем и репортим (не рискуем).
import unreal, json, traceback
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
OPT = unreal.AssetRegistryDependencyOptions(include_hard_package_references=True,
                                            include_soft_package_references=True)
LEFT = ["ActorCore_Sample_Motions","AnimStarterPack","Attaku","CharacterEmotes","DynamicFalling",
        "FreeLadderAnimationSet","Free_Crawl_Animation","Free_Interaction_Animation","Interaction",
        "InteractionKitVol1","ItemConsumableAnims","JKMotion_HitReaction","KnockedDown",
        "MCO_Mocap_Basics","MC_Sample","ResourcePack","ScifiWorkerAnimset","WorkAnimations","ZombieAnimationPack"]
R = {"deleted": [], "kept_referenced": [], "err": None}
try:
    for name in LEFT:
        folder = "/Game/" + name
        if not eal.does_directory_exist(folder):
            continue
        assets = list(eal.list_assets(folder, recursive=True, include_folder=False))
        held = []
        for a in assets:
            seg = a.rsplit("/", 1); src = seg[0] + "/" + seg[1].split(".")[0]
            refs = ar.get_referencers(unreal.Name(src), OPT) or []
            # игнорируем «самоссылки» внутри той же корневой папки
            ext = [str(x) for x in refs if not str(x).startswith(folder + "/")]
            if ext:
                held.append({"asset": src, "by": ext[:4]})
        if not held:
            eal.delete_directory(folder)
            R["deleted"].append(name)
        else:
            R["kept_referenced"].append({"name": name, "held": held})
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_del_leftovers.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("DEL_LEFTOVERS done deleted=%d kept=%d" % (len(R["deleted"]), len(R["kept_referenced"])))
