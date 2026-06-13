# -*- coding: utf-8 -*-
"""Ряд 4 «проблемных» (subfolder native) по центру y=-134 для проверки многоцветности."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label().startswith('PROD_'):
        eas.destroy_actor(a)
CY = -134.0
items = [('SM_WeldingMachine', -160.0), ('SM_FireExtinguisher', -55.0), ('SM_FirstAidKit', 45.0), ('SM_Battery', 135.0)]
for name, dy in items:
    m = unreal.load_asset('/Game/Avariika/Meshes/%s/%s.%s' % (name, name, name))
    if not m:
        continue
    a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(175.0, CY + dy, 360.0))
    a.set_actor_label('PROD_' + name)
    a.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    a.static_mesh_component.set_static_mesh(m)
    b = m.get_bounds().box_extent
    mx = max(b.x, b.y, b.z) * 2.0
    s = 60.0 / mx if mx > 0 else 1.0
    a.set_actor_scale3d(unreal.Vector(s, s, s))
    a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=200.0, roll=0.0), False)
open(r'D:\unrealEngine\avariika\Saved\lineup4.txt', 'w', encoding='utf-8').write('done')
