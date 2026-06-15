# -*- coding: utf-8 -*-
# Ставим игрока в открытое место (позади есть простор для пружины TP-камеры) — проверить вид от 3-го лица.
import unreal
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
gw = ues.get_game_world()
if gw:
    pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
    pc = unreal.GameplayStatics.get_player_controller(gw, 0)
    if pawn:
        pawn.set_actor_location(unreal.Vector(450.0, 0.0, 360.0), False, False)
    if pc:
        pc.set_control_rotation(unreal.Rotator(-3.0, 0.0, 0.0))  # лицом +X (к рабочей зоне), позади открыто
unreal.log("POSE_OPEN done")
