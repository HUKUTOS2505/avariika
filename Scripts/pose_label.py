# -*- coding: utf-8 -*-
# Встаём ПЕРЕД табличкой газовой трубы (она смотрит +X), лицом к ней (-X), чтобы прочитать текст.
import unreal
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
gw = ues.get_game_world()
if gw:
    pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
    pc = unreal.GameplayStatics.get_player_controller(gw, 0)
    if pawn:
        pawn.set_actor_location(unreal.Vector(430.0, 40.0, 360.0), False, False)  # вплотную перед трубой [600,40]
    if pc:
        pc.set_control_rotation(unreal.Rotator(-12.0, 0.0, 0.0))  # лицом +X на трубу
unreal.log("POSE_LABEL done")
