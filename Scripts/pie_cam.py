# -*- coding: utf-8 -*-
"""Считать трансформ камеры игрока в PIE-мире."""
import unreal
out = []
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = ues.get_game_world()
    out.append('world=%s' % (world.get_name() if world else 'None'))
    cam = unreal.GameplayStatics.get_player_camera_manager(world, 0)
    if cam:
        l = cam.get_camera_location()
        r = cam.get_camera_rotation()
        fov = cam.get_fov_angle()
        out.append('cam loc=(%.1f,%.1f,%.1f) rot(pitch=%.1f,yaw=%.1f,roll=%.1f) fov=%.1f' % (
            l.x, l.y, l.z, r.pitch, r.yaw, r.roll, fov))
    pawn = unreal.GameplayStatics.get_player_pawn(world, 0)
    if pawn:
        pl = pawn.get_actor_location()
        out.append('pawn loc=(%.1f,%.1f,%.1f)' % (pl.x, pl.y, pl.z))
except Exception as e:
    out.append('ERR %s' % e)
open(r'C:\unrealEngine\avariika\Saved\pie_cam.txt', 'w', encoding='utf-8').write('\n'.join(out))
