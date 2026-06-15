# -*- coding: utf-8 -*-
import unreal
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
gw = ues.get_game_world()
if gw:
    pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
    pc = unreal.GameplayStatics.get_player_controller(gw, 0)
    if pawn:
        pawn.set_actor_location(unreal.Vector(150.0, 0.0, 330.0), False, False)
        try: pawn.un_crouch(False)
        except Exception: pass
    if pc:
        pc.set_control_rotation(unreal.Rotator(0.0, 0.0, 0.0))
unreal.log("POSE_STAND done")
