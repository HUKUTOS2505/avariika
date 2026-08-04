import unreal, json, traceback
# Idempotent: clears every SM_Floor*/BG_Floor actor in L_BakerHouse and re-lays a
# clean complete 7x8 garage floor (56x SM_Floor_1x1m), then verifies zero holes.
# Fixes the half-finished strip-floor left over from the 2026-06-20 freeze.
rep = {"cleared": 0, "cleared_lbls": [], "floor": 0, "fails": [], "missing_cells": []}
try:
    ues = unreal.UnrealEditorSubsystem(); world = ues.get_editor_world()
    wn = world.get_name() if world else None
    rep["world"] = wn
    if wn != "L_BakerHouse":
        rep["ABORT"] = "world is %r" % wn
    else:
        eas = unreal.EditorActorSubsystem()
        for a in list(eas.get_all_level_actors()):
            try: lbl = a.get_actor_label()
            except Exception: continue
            if lbl.startswith("SM_Floor") or lbl.startswith("BG_Floor"):
                rep["cleared_lbls"].append(lbl); eas.destroy_actor(a); rep["cleared"] += 1
        FLOOR = "/Game/ResidentialHouses/Meshes/Architecture/Floors/SM_Floor_1x1m"
        m = unreal.load_asset(FLOOR)
        if not m:
            rep["fails"].append(FLOOR)
        else:
            for cx in range(7):
                for cy in range(8):
                    a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(100*cx+100, 100*cy, 0.0), unreal.Rotator(0,0,0))
                    a.static_mesh_component.set_static_mesh(m)
                    a.set_actor_label("BG_Floor_%d_%d" % (cx, cy)); a.set_folder_path(unreal.Name("Garage"))
                    rep["floor"] += 1
        # verify: every 1m cell center in X[0,700] Y[0,800] must be covered
        covered = set()
        for a in eas.get_all_level_actors():
            try: lbl = a.get_actor_label()
            except Exception: continue
            if lbl.startswith("BG_Floor"):
                o, e = a.get_actor_bounds(False)
                x0, x1, y0, y1 = o.x-e.x, o.x+e.x, o.y-e.y, o.y+e.y
                for cx in range(7):
                    for cy in range(8):
                        px, py = 100*cx+50, 100*cy+50
                        if x0-1 <= px <= x1+1 and y0-1 <= py <= y1+1:
                            covered.add((cx, cy))
        for cx in range(7):
            for cy in range(8):
                if (cx, cy) not in covered:
                    rep["missing_cells"].append([cx, cy])
        unreal.LevelEditorSubsystem().save_current_level(); rep["saved"] = True
except Exception:
    rep["fatal"] = traceback.format_exc()
open("C:/unrealEngine/avariika/Saved/fix_garage_floor.json", "w").write(json.dumps(rep, indent=2))
