# -*- coding: utf-8 -*-
"""Крупный ряд проблемных моделей перед игроком (дневной свет) для визуальной диагностики."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label().startswith('PROD_'):
        eas.destroy_actor(a)
CY = 0.0
items = [('SM_WeldingMachine', -120.0), ('SM_Radio', -40.0), ('SM_Thermos', 40.0), ('SM_Fuse', 120.0)]
out = []
for name, dy in items:
    m = unreal.load_asset('/Game/Avariika/Meshes/%s.%s' % (name, name))
    if not m:
        out.append('no ' + name); continue
    a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(200.0, CY + dy, 360.0))
    a.set_actor_label('PROD_' + name)
    a.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    a.static_mesh_component.set_static_mesh(m)
    b = m.get_bounds().box_extent
    mx = max(b.x, b.y, b.z) * 2.0
    s = 65.0 / mx if mx > 0 else 1.0
    a.set_actor_scale3d(unreal.Vector(s, s, s))
    a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=180.0, roll=0.0), False)
    out.append('%s placed' % name)
open(r'D:\unrealEngine\avariika\Saved\diag_lineup.txt', 'w', encoding='utf-8').write('\n'.join(out))
