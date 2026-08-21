import unreal
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = ues.get_game_world()
pc = unreal.GameplayStatics.get_player_controller(world, 0) if world else None
if pc:
    pc.set_control_rotation(unreal.Rotator(0.0, 0.0, 90.0))  # roll,pitch,yaw -> yaw=90 (смотрим вправо)
    unreal.log("AIMSET| control set yaw=90")
else:
    unreal.log("AIMSET| no PC")
