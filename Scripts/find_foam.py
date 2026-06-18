import unreal
reg=unreal.AssetRegistryHelpers.get_asset_registry()
out=[]
for a in reg.get_assets_by_path("/Game/NiagaraExamples", recursive=True):
    if str(a.asset_class_path.asset_name)=="NiagaraSystem":
        nm=str(a.asset_name).lower()
        if any(k in nm for k in ("water","spray","fountain","foam","mist","splash","steam","bubble")):
            out.append(str(a.package_name))
open("D:/unrealEngine/avariika/Saved/find_foam.txt","w").write("\n".join(sorted(set(out))))
