import unreal, json
SRC = "/Game/PostApocalypticHouse/Maps/LV_Horror_Light"
DST = "/Game/Avariika/Maps/L_House01"
OUT = r"D:/unrealEngine/avariika/Scripts/build_house_probe.json"
R = {"steps": [], "err": None}
eal = unreal.EditorAssetLibrary
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

try:
    if not eal.does_asset_exist(DST):
        dup = eal.duplicate_asset(SRC, DST)
        R["steps"].append("duplicated=" + str(dup is not None))
    else:
        R["steps"].append("exists")

    les.load_level(DST)
    R["steps"].append("loaded")
    actors = eas.get_all_level_actors()
    R["actor_count"] = len(actors)
    hist = {}
    for a in actors:
        cn = a.get_class().get_name()
        hist[cn] = hist.get(cn, 0) + 1
    R["class_hist"] = dict(sorted(hist.items(), key=lambda kv: -kv[1])[:15])

    sm = [a for a in actors if a.get_class().get_name() == "StaticMeshActor"]
    R["sm_count"] = len(sm)
    if sm:
        minv = [1e9, 1e9, 1e9]; maxv = [-1e9, -1e9, -1e9]
        for a in sm:
            try:
                origin, ext = a.get_actor_bounds(True)
                vals = [(origin.x, ext.x), (origin.y, ext.y), (origin.z, ext.z)]
                for i, (o, e) in enumerate(vals):
                    minv[i] = min(minv[i], o - e); maxv[i] = max(maxv[i], o + e)
            except Exception:
                pass
        R["bounds_min"] = minv
        R["bounds_max"] = maxv
except Exception as e:
    R["err"] = str(e)

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("HOUSE_PROBE: " + json.dumps(R))
