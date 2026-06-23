import unreal, json
DST = "/Game/Avariika/Maps/L_House01"
OUT = r"C:/unrealEngine/avariika/Scripts/build_house_level.json"
R = {"steps": [], "placed": [], "err": None}
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
cube = unreal.load_asset("/Engine/BasicShapes/Cube")

try:
    les.load_level(DST)
    world = unreal.EditorLevelLibrary.get_editor_world()
    actors = eas.get_all_level_actors()
    by = {a.get_actor_label(): a for a in actors}

    # bounds from static meshes
    sm = [a for a in actors if a.get_class().get_name() == "StaticMeshActor"]
    minv = [1e9, 1e9, 1e9]; maxv = [-1e9, -1e9, -1e9]
    for a in sm:
        try:
            o, e = a.get_actor_bounds(True)
            for i, (oo, ee) in enumerate([(o.x, e.x), (o.y, e.y), (o.z, e.z)]):
                minv[i] = min(minv[i], oo - ee); maxv[i] = max(maxv[i], oo + ee)
        except Exception:
            pass
    cx = (minv[0] + maxv[0]) / 2.0; cy = (minv[1] + maxv[1]) / 2.0
    floor = minv[2] if minv[2] < 1e8 else 0.0
    hx = max((maxv[0] - minv[0]) / 2.0, 400.0); hy = max((maxv[1] - minv[1]) / 2.0, 400.0)
    R["center"] = [cx, cy, floor]; R["half"] = [hx, hy]

    # GameMode (playable like the real game)
    gm_bp = unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoGameMode")
    ws = unreal.GameplayStatics.get_actor_of_class(world, unreal.WorldSettings)
    if gm_bp and ws:
        ws.set_editor_property("default_game_mode", gm_bp.generated_class())
        R["steps"].append("gamemode_set")

    def upsert(cls, label, loc, scale):
        a = by.get(label)
        if a is None:
            a = eas.spawn_actor_from_class(cls, loc, unreal.Rotator(0, 0, 0))
            a.set_actor_label(label)
        else:
            a.set_actor_location(loc, False, False)
        a.set_actor_scale3d(scale)
        return a

    def repairable(label, name, loc, scale, dur, mg, tool, gas=False):
        a = upsert(unreal.Repairable, label, loc, scale)
        a.get_editor_property("MeshComponent").set_static_mesh(cube)
        a.set_editor_property("DisplayName", name)
        a.set_editor_property("RepairDuration", dur)
        a.set_editor_property("MinigameType", mg)
        a.set_editor_property("RequiredTool", tool)
        if gas:
            a.set_editor_property("bLeaksGasWhenBroken", True)
        R["placed"].append(label)

    def tool_item(label, name, loc, scale, tag, size):
        a = upsert(unreal.PickupItem, label, loc, scale)
        a.get_editor_property("MeshComponent").set_static_mesh(cube)
        a.set_editor_property("DisplayName", name)
        a.set_editor_property("ItemSize", size)
        a.set_editor_property("Charges", -1)
        a.set_editor_property("ToolTag", tag)
        R["placed"].append(label)

    V = unreal.Vector
    repairable("Repairable_Breaker", "Электрощиток", V(cx - hx * 0.5, cy - hy * 0.5, floor + 75), V(0.35, 1.0, 1.5), 6.0, unreal.RepairMinigameType.CURSOR, "Tester")
    repairable("Repairable_GasPipe", "Газовая труба", V(cx + hx * 0.45, cy + hy * 0.4, floor + 110), V(0.35, 0.35, 2.2), 8.0, unreal.RepairMinigameType.VALVE, "None", gas=True)
    repairable("Repairable_Generator", "Генератор", V(cx, cy + hy * 0.5, floor + 50), V(1.5, 1.0, 1.0), 10.0, unreal.RepairMinigameType.STARTER, "Welder")

    ez = upsert(unreal.ExitZone, "ExitZone_Gazelle", V(cx - hx * 0.6, cy + hy * 0.2, floor + 100), V(2.5, 2.5, 1.5))
    R["placed"].append("ExitZone_Gazelle")
    upsert(unreal.Toilet, "Toilet", V(cx + hx * 0.4, cy - hy * 0.5, floor + 40), V(0.9, 0.9, 0.8))
    R["placed"].append("Toilet")

    tool_item("Tester", "Тестер", V(cx - hx * 0.45, cy - hy * 0.4, floor + 30), V(0.2, 0.12, 0.05), "Tester", unreal.ItemSize.LIGHT)
    tool_item("Welder", "Сварочник", V(cx + 100.0, cy + hy * 0.45, floor + 20), V(0.3, 0.16, 0.16), "Welder", unreal.ItemSize.HEAVY)

    ps = upsert(unreal.PlayerStart, "PlayerStart", V(cx, cy, floor + 97), V(1, 1, 1))
    R["placed"].append("PlayerStart")

    les.save_current_level()
    R["steps"].append("saved")
    R["final_actor_count"] = len(eas.get_all_level_actors())
except Exception as e:
    R["err"] = str(e)

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("BUILD_HOUSE: " + json.dumps(R))
