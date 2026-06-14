import unreal
reg = unreal.AssetRegistryHelpers.get_asset_registry()
out=[]
for base in ["/Game/NiagaraExamples","/Game/Fire_EXP_Vol01_Free"]:
    reg.scan_paths_synchronous([base], True)
    for a in reg.get_assets_by_path(base, recursive=True):
        if str(a.asset_class_path.asset_name)=="NiagaraSystem":
            nm=str(a.asset_name); pn=str(a.package_name)
            low=nm.lower()
            tag=""
            for k in ("explos","exp_","fire","flame","smoke","spark","ember","debris","dust","fog","torch"):
                if k in low: tag=k; break
            if tag: out.append("%-10s %s" % (tag, pn))
out=sorted(set(out))
open("D:/unrealEngine/avariika/Saved/find_ns.txt","w",encoding="utf-8").write("\n".join(out))
