# -*- coding: utf-8 -*-
# Дёргает смену вида 1-е↔3-е лицо (как клавиша V) — для проверки скриншотом.
import unreal
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
gw = ues.get_game_world()
if gw:
    pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
    if pawn:
        try:
            pawn.toggle_camera_mode()
            unreal.log("CAMERA toggled, third_person=%s" % pawn.is_third_person())
        except Exception as e:
            unreal.log("toggle err %s" % e)
