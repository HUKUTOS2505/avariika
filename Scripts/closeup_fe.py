# -*- coding: utf-8 -*-
"""Текущий огнетушитель вплотную к камере PIE (cam -13.5,0,377.6,+X) для чистого осмотра."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label().startswith('PROD_'):
        eas.destroy_actor(a)
m = unreal.load_asset('/Game/Avariika/Meshes/SM_FireExtinguisher.SM_FireExtinguisher')
a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(85.0, 0.0, 372.0))
a.set_actor_label('PROD_FE')
a.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
a.static_mesh_component.set_static_mesh(m)
b = m.get_bounds().box_extent
mx = max(b.x, b.y, b.z) * 2.0
s = 55.0 / mx if mx > 0 else 1.0
a.set_actor_scale3d(unreal.Vector(s, s, s))
a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=200.0, roll=0.0), False)
open(r'D:\unrealEngine\avariika\Saved\closeup_fe.txt', 'w', encoding='utf-8').write('ok scale %.3f' % s)
