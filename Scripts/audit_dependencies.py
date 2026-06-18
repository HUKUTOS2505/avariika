# -*- coding: utf-8 -*-
# Аудит (только чтение): какие top-level папки /Game реально используются НАШЕЙ игрой
# (контент Avariika + карта Lvl_FirstPerson + её внешние акторы OFPA). BFS по зависимостям.
# Итог: used (трогать с кодом/осторожно) vs free (двигать свободно).
import unreal, json, traceback
R = {"err": None}
try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()

    def top_folder(pkg):
        # /Game/Foo/Bar -> Foo
        p = str(pkg)
        if p.startswith("/Game/"):
            rest = p[len("/Game/"):]
            return rest.split("/")[0] if "/" in rest else rest
        return None

    # стартовые пакеты «нашей игры»
    seeds = set()
    for path in ["/Game/Avariika", "/Game/__ExternalActors__/Avariika/Maps/Lvl_FirstPerson",
                 "/Game/__ExternalObjects__/Avariika/Maps/Lvl_FirstPerson"]:
        for a in ar.get_assets_by_path(path, recursive=True, include_only_on_disk_assets=True):
            seeds.add(str(a.package_name))
    seeds.add("/Game/Avariika/Maps/Lvl_FirstPerson")
    R["seed_count"] = len(seeds)

    # BFS по зависимостям
    visited = set()
    queue = list(seeds)
    opts = unreal.AssetRegistryDependencyOptions(include_soft_package_references=True,
                                                 include_hard_package_references=True)
    steps = 0
    while queue and steps < 200000:
        steps += 1
        pkg = queue.pop()
        if pkg in visited:
            continue
        visited.add(pkg)
        try:
            deps = ar.get_dependencies(unreal.Name(pkg), opts) or []
        except Exception:
            deps = []
        for d in deps:
            ds = str(d)
            if ds.startswith("/Game/") and ds not in visited:
                queue.append(ds)
    R["visited_count"] = len(visited)

    used = {}
    for pkg in visited:
        tf = top_folder(pkg)
        if tf:
            used[tf] = used.get(tf, 0) + 1
    R["used_folders"] = dict(sorted(used.items(), key=lambda kv: -kv[1]))

    # все top-folders на диске
    all_tops = set()
    for a in ar.get_assets_by_path("/Game", recursive=False):
        pass
    # перечислим папки через подпути
    for sub in ar.get_sub_paths("/Game", recurse=False):
        tf = str(sub)[len("/Game/"):]
        if tf and "/" not in tf:
            all_tops.add(tf)
    R["all_top_folders"] = sorted(all_tops)
    R["free_to_move"] = sorted([t for t in all_tops if t not in used and not t.startswith("__")])
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_dep_audit.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("DEP_AUDIT %s" % json.dumps(R, default=str))
