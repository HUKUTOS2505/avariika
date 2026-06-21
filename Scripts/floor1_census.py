# floor1_census.py — read-only перепись L_Dom1 для FLOOR1_DESIGN.
# Запуск: через Claudius editor.run_python_script -> exec(open(path).read())
# World-guard на L_Dom1. Пишет сводку в Saved/floor1_census.json (stdout не читается).
import unreal, json
from collections import defaultdict

OUT = r"D:\unrealEngine\avariika\Saved\floor1_census.json"
res = {"ok": False}
try:
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = eas.get_all_level_actors()
    wname = actors[0].get_world().get_name() if actors else ""
    res["world"] = wname
    res["n_actors"] = len(actors)
    if "L_Dom1" not in wname:
        res["error"] = "world guard failed: " + wname
        raise Exception("world guard")

    cls_count = defaultdict(int)
    mesh_count = defaultdict(int)
    special = {"PlayerStart": [], "Repairable": [], "Door": [], "PowerSwitch": [],
               "ToolCase": [], "CallBoard": [], "Light": [], "Generator": [], "Trap": []}
    minx=miny=minz=1e9; maxx=maxy=maxz=-1e9
    zhist = defaultdict(int)

    for a in actors:
        cn = a.get_class().get_name()
        cls_count[cn] += 1
        loc = a.get_actor_location()
        try: label = a.get_actor_label()
        except: label = a.get_name()
        try:
            origin, ext = a.get_actor_bounds(False)
            minx=min(minx,origin.x-ext.x); maxx=max(maxx,origin.x+ext.x)
            miny=min(miny,origin.y-ext.y); maxy=max(maxy,origin.y+ext.y)
            minz=min(minz,origin.z-ext.z); maxz=max(maxz,origin.z+ext.z)
        except: pass
        zhist[int(round(loc.z/50.0))*50] += 1
        # mesh name for StaticMeshActor
        if cn == "StaticMeshActor":
            try:
                smc = a.static_mesh_component
                m = smc.get_editor_property("static_mesh")
                if m: mesh_count[m.get_name()] += 1
            except: pass
        rec = {"label": label, "class": cn,
               "loc":[round(loc.x,1),round(loc.y,1),round(loc.z,1)]}
        low = (cn + " " + label).lower()
        cl = cn.lower()
        if "playerstart" in cl: special["PlayerStart"].append(rec)
        if "repairable" in low: special["Repairable"].append(rec)
        if cn == "Door": special["Door"].append(rec)
        if "powerswitch" in low: special["PowerSwitch"].append(rec)
        if "toolcase" in low: special["ToolCase"].append(rec)
        if "callboard" in low: special["CallBoard"].append(rec)
        if "generator" in low: special["Generator"].append(rec)
        if cn == "Trap" or "trap" in low: special["Trap"].append(rec)

    res["class_histogram"] = dict(sorted(cls_count.items(), key=lambda kv:-kv[1]))
    res["mesh_top50"] = dict(sorted(mesh_count.items(), key=lambda kv:-kv[1])[:50])
    res["special"] = special
    res["bbox"] = {"min":[round(minx,1),round(miny,1),round(minz,1)],
                   "max":[round(maxx,1),round(maxy,1),round(maxz,1)]}
    res["z_histogram"] = dict(sorted(zhist.items()))
    res["ok"] = True
except Exception as e:
    res["exc"] = str(e)

with open(OUT, "w") as f:
    json.dump(res, f, indent=1)
