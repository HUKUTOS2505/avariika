# -*- coding: utf-8 -*-
"""Вернуть Hilux (Gazelle_Mesh) на исходное место у выхода и снести временные PROD_Truck*.
Исходно: loc=(-300,0,210) rot=(0,90,0) scale=0.9952."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
truck = by.get('Gazelle_Mesh')
if truck:
    truck.set_actor_location(unreal.Vector(-300.0, 0.0, 210.0), False, True)
    truck.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=90.0, roll=0.0), False)
    truck.set_actor_scale3d(unreal.Vector(0.9952, 0.9952, 0.9952))
    truck.static_mesh_component.set_mobility(unreal.ComponentMobility.STATIC)
    truck.modify()
    out.append('truck restored to (-300,0,210) yaw90 scale0.9952 Static')
killed = 0
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label().startswith('PROD_'):
        eas.destroy_actor(a)
        killed += 1
out.append('killed temp PROD_ = %d' % killed)
les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'C:\unrealEngine\avariika\Saved\hilux_shot_restore.txt', 'w', encoding='utf-8').write('\n'.join(out))
