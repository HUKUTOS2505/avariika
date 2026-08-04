import unreal, json, traceback
rep = {"floor": 0, "walls": 0, "placed": 0, "fails": [], "notes": []}
try:
    ues = unreal.UnrealEditorSubsystem()
    world = ues.get_editor_world()
    wn = world.get_name() if world else None
    rep["world"] = wn
    if wn != "L_BakerHouse":
        rep["ABORT"] = "world is %r, expected L_BakerHouse" % wn
    else:
        eas = unreal.EditorActorSubsystem()
        # idempotent: clear previous BG_ actors
        cleared = 0
        for a in eas.get_all_level_actors():
            try:
                if a.get_actor_label().startswith("BG_"):
                    eas.destroy_actor(a); cleared += 1
            except Exception:
                pass
        rep["cleared"] = cleared

        G = "/Game/ResidentialHouses/Meshes/Architecture/Walls/Garage/"
        FLOOR = "/Game/ResidentialHouses/Meshes/Architecture/Floors/SM_Floor_1x1m"
        WALL = {1: G + "SM_Garage_Inside_1m", 2: G + "SM_Garage_Inside_2m", 3: G + "SM_Garage_Inside_3m"}
        placed = []

        def spawn(path, x, y, yaw, label):
            m = unreal.load_asset(path)
            if not m:
                rep["fails"].append(path); return None
            a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(x, y, 0.0), unreal.Rotator(0.0, 0.0, yaw))
            a.static_mesh_component.set_static_mesh(m)
            a.set_actor_label(label); a.set_folder_path(unreal.Name("Garage"))
            placed.append(a); return a

        # floor 7 (X) x 8 (Y) tiles
        for cx in range(7):
            for cy in range(8):
                if spawn(FLOOR, 100 * cx + 100, 100 * cy, 0.0, "BG_Floor_%d_%d" % (cx, cy)):
                    rep["floor"] += 1

        # walls: (length_m, px, py, yaw)  interior X[0,700] Y[0,800], door gap on east at Y[300,400]
        walls = [
            (3, 300, -23, 0), (3, 600, -23, 0), (1, 700, -23, 0),     # south  (Y~0)
            (3, 0, 823, 180), (3, 300, 823, 180), (1, 600, 823, 180), # north  (Y~800)
            (3, 20, 300, 90), (3, 20, 600, 90), (2, 20, 800, 90),     # west   (X~0)
            (3, 680, 0, 270), (2, 680, 400, 270), (2, 680, 600, 270), # east   (X~700, gap Y300-400 = door)
        ]
        for idx, (L, x, y, yaw) in enumerate(walls, 1):
            if spawn(WALL[L], x, y, yaw, "BG_Wall_%02d" % idx):
                rep["walls"] += 1

        # lights + player start
        def try_spawn(cls, x, y, z, label, rot=(0.0, 0.0, 0.0)):
            try:
                a = eas.spawn_actor_from_class(cls, unreal.Vector(x, y, z), unreal.Rotator(*rot))
                a.set_actor_label(label); a.set_folder_path(unreal.Name("Garage")); placed.append(a); return a
            except Exception as e:
                rep["notes"].append("%s:%s" % (label, e)); return None
        try_spawn(unreal.DirectionalLight, 350, 400, 600, "BG_Sun", (0.0, -50.0, -45.0))
        sl = try_spawn(unreal.SkyLight, 350, 400, 300, "BG_SkyLight")
        if sl:
            try:
                sl.get_component_by_class(unreal.SkyLightComponent).set_editor_property("real_time_capture", True)
            except Exception as e:
                rep["notes"].append("skycap:%s" % e)
        try_spawn(unreal.SkyAtmosphere, 350, 400, 0, "BG_Atmo")
        try_spawn(unreal.PlayerStart, 350, 400, 120, "BG_PlayerStart")

        rep["placed"] = len(placed)

        # readback combined bounds (walls + floor only)
        minv = [1e9, 1e9, 1e9]; maxv = [-1e9, -1e9, -1e9]
        for a in placed:
            lbl = a.get_actor_label()
            if not (lbl.startswith("BG_Floor") or lbl.startswith("BG_Wall")):
                continue
            try:
                o, e = a.get_actor_bounds(False)
                for k, ax in enumerate(("x", "y", "z")):
                    lo = getattr(o, ax) - getattr(e, ax); hi = getattr(o, ax) + getattr(e, ax)
                    if lo < minv[k]: minv[k] = lo
                    if hi > maxv[k]: maxv[k] = hi
            except Exception:
                pass
        rep["bbox_min"] = [round(v, 1) for v in minv]
        rep["bbox_max"] = [round(v, 1) for v in maxv]
        rep["interior_target"] = "X[0,700] Y[0,800]"

        try:
            unreal.LevelEditorSubsystem().save_current_level()
            rep["saved"] = True
        except Exception as e:
            rep["notes"].append("save:%s" % e)
except Exception:
    rep["fatal"] = traceback.format_exc()
open("C:/unrealEngine/avariika/Saved/garage_build.json", "w").write(json.dumps(rep, indent=2))
