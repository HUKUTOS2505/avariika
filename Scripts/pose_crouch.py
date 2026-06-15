# -*- coding: utf-8 -*-
import unreal
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
gw = ues.get_game_world()
if gw:
    pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
    if pawn:
        try: pawn.crouch(False)
        except Exception as e: unreal.log("crouch err %s" % e)
unreal.log("POSE_CROUCH done")
