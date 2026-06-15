# -*- coding: utf-8 -*-
# Создаёт /Game/Avariika/Maps/L_Hub — функциональную БАЗУ бригады:
#   PlayerStart, доска заявок (ACallBoard), фургон в гараже, верстак-стойка инструмента,
#   зона выхода/выезда, MOVABLE-свет (иначе чёрный экран).
# Это РАБОЧИЙ каркас (не финальный арт) — красоту наводит пользователь арт-деталями
# (см. DOM_BUILD_GUIDE.md) или готовым пак-окружением. Геймплей-петля уже работает.
#
# Запуск (редактор ЗАКРЫТ):
#   UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script="D:\unrealEngine\avariika\Scripts\build_hub.py"
# Идемпотентно: каждый прогон пересоздаёт уровень свежим.
import unreal, json, traceback

R = {"steps": [], "actors": 0, "err": None}

def safe_load(path):
    try:
        return unreal.load_asset(path)
    except Exception:
        return None

try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

    MAP = "/Game/Avariika/Maps/L_Hub"
    # Чистый старт: удалить старый L_Hub, чтобы new_level создал свежий И переключил мир
    # (если ассет уже есть, new_level в живом редакторе НЕ переключается — спавны уходят
    #  в текущий уровень и портят его; именно так Lvl_FirstPerson был загрязнён).
    if unreal.EditorAssetLibrary.does_asset_exist(MAP):
        unreal.EditorAssetLibrary.delete_asset(MAP); R["steps"].append("deleted old L_Hub")
    les.new_level(MAP); R["steps"].append("new_level L_Hub")
    try:
        les.load_level(MAP)  # форс-переключение (L_Hub не WP — load безопасен)
    except Exception as e:
        R["steps"].append("load_level note: %s" % e)
    world = ues.get_editor_world()
    cur = world.get_path_name() if world else ""
    R["world"] = cur
    # ГАРД: если редактор НЕ на L_Hub — НЕ спавнить (иначе загрязним рабочий уровень)
    if "L_Hub" not in cur:
        R["err"] = "ABORT: editor world is not L_Hub (%s)" % cur
        raise RuntimeError(R["err"])
    R["steps"].append("confirmed on L_Hub")

    cube = unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
    SM = unreal.StaticMeshActor

    def box(label, cx, cy, cz, sx, sy, sz, tag="HUB"):
        a = eas.spawn_actor_from_class(SM, unreal.Vector(cx, cy, cz), unreal.Rotator(0, 0, 0))
        a.static_mesh_component.set_static_mesh(cube)
        a.set_actor_scale3d(unreal.Vector(sx/100.0, sy/100.0, sz/100.0))
        a.set_actor_label(label); a.tags = [tag]
        R["actors"] += 1
        return a

    T = 25.0; H = 320.0  # толщина стен, высота потолка
    # ── База ~16×11 м (общая комната + гаражный бокс сбоку). Угол в (0,0). ──
    box("Hub_Floor", 800, 550, -12, 1600, 1100, 24)
    box("Hub_Ceiling", 800, 550, H+12, 1600, 1100, 24)
    # периметр (фронт с проёмом-входом, правый бок — большой проём в гараж)
    def wall_x(lbl, y, x0, x1, gaps):
        cur = x0; segs = []
        for gc, gw in sorted(gaps):
            if gc-gw/2 > cur: segs.append((cur, gc-gw/2))
            cur = gc+gw/2
        if x1 > cur: segs.append((cur, x1))
        for i,(a,b) in enumerate(segs): box("%s_%d"%(lbl,i), (a+b)/2, y, H/2, (b-a)+T, T, H)
    def wall_y(lbl, x, y0, y1, gaps):
        cur = y0; segs = []
        for gc, gw in sorted(gaps):
            if gc-gw/2 > cur: segs.append((cur, gc-gw/2))
            cur = gc+gw/2
        if y1 > cur: segs.append((cur, y1))
        for i,(a,b) in enumerate(segs): box("%s_%d"%(lbl,i), x, (a+b)/2, H/2, T, (b-a)+T, H)
    wall_x("Hub_Front", 0, 0, 1600, [(400, 130)])     # входная дверь
    wall_x("Hub_Back", 1100, 0, 1600, [])
    wall_y("Hub_Left", 0, 0, 1100, [])
    wall_y("Hub_Right", 1600, 0, 1100, [(550, 360)])  # проём в гараж

    # ── Гаражный бокс справа (10×8 м), пол + ворота ──
    box("Hub_Garage_Floor", 2100, 550, -12, 1000, 800, 24)
    box("Hub_Garage_Ceiling", 2100, 550, H+12, 1000, 800, 24)
    box("Hub_Garage_BackWall", 2600, 550, H/2, T, 800, H)
    box("Hub_Garage_SideA", 2100, 150, H/2, 1000, T, H)
    box("Hub_Garage_SideB", 2100, 950, H/2, 1000, T, H)

    # ── Свет: MOVABLE (КРИТИЧНО — Stationary даёт чёрный экран без билда) ──
    dl = eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(800, 550, 700), unreal.Rotator(pitch=-48, yaw=35, roll=0))
    dl.set_actor_label("Hub_Sun"); R["actors"] += 1
    dc = dl.get_component_by_class(unreal.DirectionalLightComponent)
    dc.set_mobility(unreal.ComponentMobility.MOVABLE); dc.set_intensity(6.0)
    try: dc.set_editor_property("atmosphere_sun_light", True)
    except Exception: pass
    sky = eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(800, 550, 400)); sky.set_actor_label("Hub_Sky"); R["actors"] += 1
    sc = sky.get_component_by_class(unreal.SkyLightComponent); sc.set_mobility(unreal.ComponentMobility.MOVABLE)
    try: sc.set_editor_property("real_time_capture", True)
    except Exception: pass
    sc.set_intensity(1.0)
    eas.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(800, 550, 0)); R["actors"] += 1
    eas.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(800, 550, 100)); R["actors"] += 1
    try: sc.recapture_sky()
    except Exception: pass
    R["steps"].append("movable lights")

    # ── PlayerStart (у входа) ──
    ps = eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(400, 250, 100), unreal.Rotator(pitch=0, yaw=90, roll=0))
    ps.set_actor_label("PlayerStart_Hub"); R["actors"] += 1

    # ── Доска заявок (ACallBoard) — у левой стены, лицом в комнату ──
    board_cls = unreal.load_class(None, "/Script/Avaryo.CallBoard")
    if board_cls:
        board = eas.spawn_actor_from_class(board_cls, unreal.Vector(120, 550, 0), unreal.Rotator(pitch=0, yaw=0, roll=0))
        board.set_actor_label("CallBoard_Hub"); R["actors"] += 1
        # явный путь хаба для возврата (без PIE-префикса)
        try: board.set_editor_property("hub_map_override", MAP)
        except Exception as e: R["steps"].append("hub_override err: %s" % e)
        R["steps"].append("CallBoard spawned")
    else:
        R["steps"].append("WARN: /Script/Avaryo.CallBoard не найден — собери C++ (модуль не скомпилирован?)")

    # ── Фургон в гараже (SM_Gazelle — одиночный меш, надёжно) ──
    van_mesh = safe_load("/Game/Avariika/Meshes/SM_Gazelle.SM_Gazelle")
    if van_mesh:
        van = eas.spawn_actor_from_class(SM, unreal.Vector(2100, 550, 10), unreal.Rotator(pitch=0, yaw=0, roll=0))
        van.static_mesh_component.set_static_mesh(van_mesh)
        van.set_actor_label("Hub_Van"); van.tags = ["HUB", "VAN"]; R["actors"] += 1
        R["steps"].append("van placed")
    else:
        R["steps"].append("van mesh не найден — пропуск (декор)")

    # ── Стойка инструмента (верстак из Garage_Tools_Props, если есть) ──
    bench = (safe_load("/Game/Garage_Tools_Props/Meshes/SM_Workbench_01.SM_Workbench_01")
             or safe_load("/Game/Garage_Tools_Props/Meshes/SM_Workbench.SM_Workbench"))
    if bench:
        b = eas.spawn_actor_from_class(SM, unreal.Vector(700, 1000, 0), unreal.Rotator(pitch=0, yaw=0, roll=0))
        b.static_mesh_component.set_static_mesh(bench); b.set_actor_label("Hub_ToolRack"); R["actors"] += 1
        R["steps"].append("toolbench placed")
    else:
        box("Hub_ToolRack_PH", 700, 1000, 50, 120, 60, 100)  # заглушка-стойка
        R["steps"].append("toolbench PH (cube)")

    # ── Локальный эмбиент базы: гул лампы у доски заявок (3D AmbientSound) ──
    hum = safe_load("/Game/Audio/SFX/Hazard/Hazard_LampHum_Loop.Hazard_LampHum_Loop")
    if hum:
        amb = eas.spawn_actor_from_class(unreal.AmbientSound, unreal.Vector(120, 550, 200), unreal.Rotator(0, 0, 0))
        amb.set_actor_label("Hub_Ambient_LampHum"); R["actors"] += 1
        try:
            ac = amb.get_component_by_class(unreal.AudioComponent)
            ac.set_sound(hum); ac.set_editor_property("volume_multiplier", 0.4)
        except Exception as e: R["steps"].append("ambient err: %s" % e)
        R["steps"].append("hub ambient lamp hum")

    # ── GameMode Avaryo (тот же режим — спавн персонажа/HUD/RunState-хаб) ──
    gm = safe_load("/Game/Avariika/Blueprints/BP_AvaryoGameMode")
    if gm:
        try:
            world.get_world_settings().set_editor_property("default_game_mode", gm.generated_class())
            R["steps"].append("gamemode set")
        except Exception as e: R["steps"].append("gm err: %s" % e)

    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()

with open(r"D:/unrealEngine/avariika/Scripts/manifests/_hub.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("BUILD_HUB done actors=%d err=%s" % (R["actors"], R["err"]))
