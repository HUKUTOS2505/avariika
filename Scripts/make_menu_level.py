import unreal
import traceback

out = "D:/unrealEngine/avariika/Saved/make_menu_level.txt"
log = []


def flush():
    with open(out, "w", encoding="utf-8") as f:
        f.write("\n".join(log))


try:
    level_path = "/Game/Avariika/Maps/L_MainMenu"
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    ok = les.new_level(level_path)
    log.append("new_level(%s): %s" % (level_path, ok))
    flush()

    # world + WorldSettings
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = ues.get_editor_world()
    ws = world.get_world_settings()
    log.append("world: %s | worldsettings: %s" % (world.get_name(), ws.get_name()))

    # set GameMode override to our C++ menu gamemode
    gm_cls = unreal.MenuGameMode
    ws.set_editor_property("default_game_mode", gm_cls)
    check = ws.get_editor_property("default_game_mode")
    log.append("default_game_mode set -> %s" % (check.get_name() if check else "None"))
    flush()

    # add a PlayerStart (avoids 'no playerstart' warning; pawn won't spawn anyway)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    ps = eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, 0, 100), unreal.Rotator(0, 0, 0))
    log.append("PlayerStart spawned: %s" % (ps.get_name() if ps else "None"))

    # save
    saved = les.save_current_level()
    log.append("save_current_level: %s" % saved)
    flush()
    log.append("DONE")
    flush()
except Exception:
    log.append("EXC:\n" + traceback.format_exc())
    flush()
