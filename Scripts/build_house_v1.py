# -*- coding: utf-8 -*-
# Создаём карту дома /Game/Avariika/Maps/L_Dom, ставим готовый дом ResidentialHouses House01,
# землю-плоскость (чтобы не падать в пустоту), PlayerStart на полу, базовый свет, GameMode = BP_AvaryoGameMode.
# ГАРД: работаем только если мир == L_Dom (иначе abort, текущую карту не трогаем/не сохраняем).
import unreal, json, traceback
R = {"err": None, "steps": []}
def log(s):
    R["steps"].append(s); unreal.log("BUILDHOUSE %s" % s)
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    MAP = "/Game/Avariika/Maps/L_Dom"

    if unreal.EditorAssetLibrary.does_asset_exist(MAP):
        les.load_level(MAP); log("loaded existing L_Dom")
    else:
        ok = les.new_level(MAP); log("new_level=%s" % ok)
    w = ues.get_editor_world()
    if not (w and "L_Dom" in w.get_name()):
        les.load_level(MAP); w = ues.get_editor_world()  # фолбэк: явно загрузить
    R["world"] = w.get_name() if w else None
    if not (w and "L_Dom" in w.get_name()):
        R["err"] = "GUARD: не на L_Dom (world=%s)" % R["world"]; raise RuntimeError(R["err"])
    log("world OK = %s" % R["world"])

    # --- готовый дом House01 ---
    house_cls = unreal.load_object(None, "/Game/ResidentialHouses/Blueprints/Houses/House01.House01_C")
    if not house_cls:
        R["err"] = "House01 class not found"; raise RuntimeError(R["err"])
    house = eas.spawn_actor_from_class(house_cls, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    if house: house.set_actor_label("Dom_House01")
    log("house=%s" % (house.get_actor_label() if house else "FAIL"))
    bo, be = house.get_actor_bounds(False)
    R["house_origin"] = [round(bo.x), round(bo.y), round(bo.z)]
    R["house_extent"] = [round(be.x), round(be.y), round(be.z)]
    floor_z = bo.z - be.z  # низ дома

    # --- земля-плоскость под домом (Engine plane), большая ---
    plane = unreal.load_object(None, "/Engine/BasicShapes/Plane.Plane")
    ground = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(bo.x, bo.y, floor_z - 2.0), unreal.Rotator(0, 0, 0))
    if ground and plane:
        ground.set_actor_label("Ground")
        smc = ground.static_mesh_component
        smc.set_static_mesh(plane)
        ground.set_actor_scale3d(unreal.Vector(max(be.x, be.y) / 50.0 * 2.5, max(be.x, be.y) / 50.0 * 2.5, 1.0))
        log("ground placed z=%.0f" % (floor_z - 2.0))

    # --- свет ---
    eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 600), unreal.Rotator(-45, 30, 0))
    eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 500), unreal.Rotator(0, 0, 0))
    eas.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    eas.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    log("lights placed")

    # --- PlayerStart на полу у центра дома ---
    ps = eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(bo.x, bo.y, floor_z + 100.0), unreal.Rotator(0, 0, 0))
    if ps: ps.set_actor_label("PlayerStart")
    R["playerstart_z"] = round(floor_z + 100.0)
    log("PlayerStart placed")

    # --- GameMode override = BP_AvaryoGameMode ---
    gm = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoGameMode.BP_AvaryoGameMode_C")
    ws = None
    for a in eas.get_all_level_actors():
        if isinstance(a, unreal.WorldSettings):
            ws = a; break
    if ws and gm:
        ws.set_editor_property("default_game_mode", gm)
        log("GameMode set on WorldSettings")
    else:
        log("GameMode NOT set (ws=%s gm=%s)" % (bool(ws), bool(gm)))

    les.save_current_level(); R["saved"] = True
    R["actor_count"] = len(eas.get_all_level_actors())
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_build_house.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("BUILDHOUSE DONE %s" % json.dumps(R, default=str))
