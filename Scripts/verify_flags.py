# verify_flags.py — read-only проверка флагов FLOOR1_DESIGN: просели ли двери коридора, под полом ли PlayerStart.
import unreal, json
OUT = r"C:\unrealEngine\avariika\Saved\verify_flags.json"
res = {"ok": False}
try:
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = eas.get_all_level_actors()
    w = actors[0].get_world().get_name() if actors else ""
    if "L_Dom1" not in w:
        res["error"] = "guard " + w
        raise Exception("guard")
    targets = ["COR_Door_L2", "COR_Door_R2", "COR_Door_L3", "COR_Door_R3", "PlayerStart"]
    floors = []
    bylabel = {}
    for a in actors:
        try: lb = a.get_actor_label()
        except: lb = a.get_name()
        if "SM_Floor" in lb:
            o, e = a.get_actor_bounds(False)
            floors.append((o.x, o.y, o.z + e.z))  # top z
        if lb in targets:
            o, e = a.get_actor_bounds(False)
            loc = a.get_actor_location()
            bylabel[lb] = {"loc": [round(loc.x,1), round(loc.y,1), round(loc.z,1)],
                           "bottom_z": round(o.z - e.z, 1), "top_z": round(o.z + e.z, 1)}
    out = {}
    for lb, info in bylabel.items():
        x, y = info["loc"][0], info["loc"][1]
        best = None; bd = 1e9
        for fx, fy, ftz in floors:
            d = ((fx-x)**2 + (fy-y)**2) ** 0.5
            if d < bd: bd = d; best = (ftz, d)
        info["nearest_floor_top_z"] = round(best[0],1) if best else None
        info["nearest_floor_dist_cm"] = round(best[1],1) if best else None
        # вердикт: низ актора заметно ниже пола → просел/под полом
        if best is not None:
            gap = info["bottom_z"] - best[0]
            info["gap_to_floor"] = round(gap, 1)
            info["verdict"] = ("OK (над полом)" if gap >= -10 else
                               "ПРОСЕЛ/ПОД ПОЛОМ на %d см" % round(-gap))
        out[lb] = info
    res["targets"] = out
    res["n_floors"] = len(floors)
    res["ok"] = True
except Exception as e:
    res["exc"] = str(e)
open(OUT, "w").write(json.dumps(res, indent=1, ensure_ascii=False))
