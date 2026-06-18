# -*- coding: utf-8 -*-
# Доводим остатки. БЕЗОПАСНОЕ правило на каждый реальный ассет:
#  • есть внешние референсы → rename в _Packs (ссылки чинятся сами); коллизия → не трогаем, репорт;
#  • нет референсов + дубль уже в _Packs → delete (безопасно);
#  • нет референсов + уникален → rename в _Packs.
# Редиректоры пропускаем (их вычистит delete_directory). Папку удаляем, если реальных ассетов не осталось.
import unreal, json, traceback
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
OPT = unreal.AssetRegistryDependencyOptions(include_hard_package_references=True,
                                            include_soft_package_references=True)
STR = ["ActorCore_Sample_Motions","Construction_Pit","Hyper","GoreAndHorrorMegapack",
       "JKMotion_HitReaction","ResourcePack"]
LOG = r"D:/unrealEngine/avariika/Scripts/manifests/_stragglers_progress.txt"
def L(s):
    with open(LOG, "a", encoding="utf-8") as f: f.write(s+"\n")
R = {"deleted_folders": [], "left_folders": [], "err": None}
L("=== FINISH STRAGGLERS ===")
try:
    for name in STR:
        F = "/Game/"+name
        if not eal.does_directory_exist(F):
            L("нет в реестре: %s"%name); continue
        datas = ar.get_assets_by_path(F, recursive=True, include_only_on_disk_assets=False)
        moved=deleted=left=fail=0
        for d in datas:
            try: cls = str(d.asset_class_path.asset_name)
            except Exception: cls = "?"
            if cls == "ObjectRedirector": continue
            src = str(d.package_name)
            dst = src.replace("/Game/"+name+"/", "/Game/_Packs/"+name+"/", 1)
            ext = [str(x) for x in (ar.get_referencers(unreal.Name(src), OPT) or []) if not str(x).startswith(F)]
            try:
                if not ext:
                    if eal.does_asset_exist(dst): eal.delete_asset(src); deleted+=1
                    else: eal.rename_asset(src, dst); moved+=1
                else:
                    if eal.does_asset_exist(dst): left+=1  # коллизия + referenced → не рискуем
                    else: eal.rename_asset(src, dst); moved+=1
            except Exception as e:
                fail+=1
        # пересчёт реальных
        rem = 0
        for d in ar.get_assets_by_path(F, recursive=True, include_only_on_disk_assets=False):
            try: c = str(d.asset_class_path.asset_name)
            except Exception: c = "?"
            if c != "ObjectRedirector": rem += 1
        if rem == 0:
            eal.delete_directory(F); R["deleted_folders"].append(name)
            L("FOLDER DELETED %s (moved=%d del=%d fail=%d)"%(name,moved,deleted,fail))
        else:
            R["left_folders"].append({"name":name,"real_left":rem,"moved":moved,"deleted":deleted,"left_collision":left,"fail":fail})
            L("LEFT %s real=%d (moved=%d del=%d coll=%d fail=%d)"%(name,rem,moved,deleted,left,fail))
    L("=== DONE ===")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc(); L("EXC %s"%e)
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_stragglers.json","w",encoding="utf-8") as f:
    json.dump(R,f,indent=1,ensure_ascii=False)
unreal.log("FINISH_STRAGGLERS done")
