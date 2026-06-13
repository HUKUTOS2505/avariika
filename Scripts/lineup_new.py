# -*- coding: utf-8 -*-
"""Временные копии 4 новых мешей в ряд перед игроком PIE (cam -13.5,-134,377,+X), дневной
свет уже есть. Масштаб ~60см для читаемости. Метка PROD_ — потом снести."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
# снести старые PROD_
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label().startswith('PROD_'):
        eas.destroy_actor(a)
items = [('SM_Thermos', -180.0), ('SM_LightKit', -75.0), ('SM_TrapKit', 25.0), ('SM_MotionSensor', 120.0)]
CY = -134.0
out = []
for name, dy in items:
    m = unreal.load_asset('/Game/Avariika/Meshes/%s.%s' % (name, name))
    if not m:
        out.append('no ' + name); continue
    a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(180.0, CY + dy, 360.0))
    a.set_actor_label('PROD_' + name)
    a.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    a.static_mesh_component.set_static_mesh(m)
    b = m.get_bounds().box_extent
    mx = max(b.x, b.y, b.z) * 2.0
    s = 60.0 / mx if mx > 0 else 1.0
    a.set_actor_scale3d(unreal.Vector(s, s, s))
    a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=180.0, roll=0.0), False)
    out.append('%s placed' % name)
open(r'D:\unrealEngine\avariika\Saved\lineup_new.txt', 'w', encoding='utf-8').write('\n'.join(out))
