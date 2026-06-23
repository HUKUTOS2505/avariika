import unreal
import traceback

out = "C:/unrealEngine/avariika/Saved/cleanup_packs.txt"
log = []
def flush():
    with open(out, "w", encoding="utf-8") as f:
        f.write("\n".join(log))

reg = unreal.AssetRegistryHelpers.get_asset_registry()
EAL = unreal.EditorAssetLibrary

try:
    # 1) Полностью удаляемые паки
    for d in ["/Game/FoggyStreet", "/Game/Decal_Forge", "/Game/OGMainMenu"]:
        if EAL.does_directory_exist(d):
            EAL.delete_directory(d)
            log.append("deleted pack dir: %s" % d)
    flush()

    # 2) Hyper: оставить погодные Niagara + небо + их зависимости, удалить остальное
    assets = reg.get_assets_by_path("/Game/Hyper", recursive=True)
    allpkgs = set(str(a.package_name) for a in assets)
    roots = set()
    sky_names = {"SM_SkySphere", "M_SkySphere", "T_Moon", "T_Stars", "MI_SimpleVolumetricClouds"}
    for a in assets:
        pn = str(a.package_name); nm = str(a.asset_name); cls = str(a.asset_class_path.asset_name)
        if cls == "NiagaraSystem" and "/Effects/Weather" in pn:
            roots.add(pn)
        if nm in sky_names:
            roots.add(pn)
        if "/Icons/Weather_States" in pn or "/Post_Process_Effects" in pn or "/SpaceSkyboxes" in pn:
            roots.add(pn)
    log.append("Hyper total=%d, keep-roots=%d" % (len(allpkgs), len(roots)))
    flush()

    if len(roots) < 5:
        log.append("ABORT: too few roots (%d) - not deleting Hyper to avoid wiping VFX" % len(roots))
        flush()
    else:
        opts = unreal.AssetRegistryDependencyOptions()
        opts.include_hard_package_references = True
        opts.include_soft_package_references = True
        keep = set(); stack = list(roots)
        while stack:
            p = stack.pop()
            if p in keep:
                continue
            keep.add(p)
            for dep in (reg.get_dependencies(p, opts) or []):
                s = str(dep)
                if s.startswith("/Game/Hyper/") and s not in keep:
                    stack.append(s)
        log.append("keep-closure=%d, to-delete=%d" % (len(keep), len(allpkgs) - len(keep)))
        flush()

        # кандидаты-папки: дети /Game/Hyper, /Game/Hyper/ResourcePack, /Game/Hyper/DayNightCycle
        def childdirs(base):
            pref = base + "/"
            kids = set()
            for p in allpkgs:
                if p.startswith(pref):
                    kids.add(pref + p[len(pref):].split("/")[0])
            return kids
        cands = childdirs("/Game/Hyper") | childdirs("/Game/Hyper/ResourcePack") | childdirs("/Game/Hyper/DayNightCycle")
        deleted = []
        for d in sorted(cands, key=len, reverse=True):
            pref = d + "/"
            has_keep = any(k == d or k.startswith(pref) for k in keep)
            if not has_keep and EAL.does_directory_exist(d):
                try:
                    EAL.delete_directory(d)
                    deleted.append(d)
                except Exception as e:
                    log.append("  del fail %s: %s" % (d, e))
        log.append("deleted %d Hyper dirs:" % len(deleted))
        log.extend("  " + d for d in sorted(deleted))
        flush()
    log.append("DONE")
    flush()
except Exception:
    log.append("EXC:\n" + traceback.format_exc())
    flush()
