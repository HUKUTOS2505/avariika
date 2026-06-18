# -*- coding: utf-8 -*-
# Диагностика остаточных папок в корне после переезда: что внутри — пусто / редиректоры / реальные ассеты.
# НИЧЕГО не удаляет. Только считает, чтобы решить, что безопасно убрать.
import unreal, json, traceback
R = {"err": None, "folders": {}}
LEFT = ["ActorCore_Sample_Motions","AnimStarterPack","Attaku","CharacterEmotes","Construction_Pit",
        "DynamicFalling","FreeLadderAnimationSet","Free_Crawl_Animation","Free_Interaction_Animation",
        "GoreAndHorrorMegapack","Hyper","Interaction","InteractionKitVol1","ItemConsumableAnims",
        "JKMotion_HitReaction","KnockedDown","MCO_Mocap_Basics","MC_Sample","ResourcePack",
        "ScifiWorkerAnimset","WorkAnimations","ZombieAnimationPack"]
try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    eal = unreal.EditorAssetLibrary
    for name in LEFT:
        folder = "/Game/" + name
        if not eal.does_directory_exist(folder):
            R["folders"][name] = {"state": "нет папки"}; continue
        datas = ar.get_assets_by_path(folder, recursive=True, include_only_on_disk_assets=False)
        redir = 0; real = 0; real_examples = []
        for d in datas:
            try:
                cls = str(d.asset_class_path.asset_name)
            except Exception:
                cls = str(getattr(d, "asset_class", "?"))
            if cls == "ObjectRedirector":
                redir += 1
            else:
                real += 1
                if len(real_examples) < 3:
                    real_examples.append("%s (%s)" % (str(d.asset_name), cls))
        # есть ли ссылки на редиректоры (реферим ли кто-то старый путь)
        R["folders"][name] = {
            "total": len(datas), "redirectors": redir, "real_assets": real,
            "real_examples": real_examples,
            "verdict": ("ПУСТО → удалить" if len(datas) == 0 else
                        ("только редиректоры → fixup+удалить" if real == 0 else
                         "ЕСТЬ РЕАЛЬНЫЕ → не трогать (частичный переезд)"))
        }
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_leftovers.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("DIAGNOSE_LEFTOVERS done")
