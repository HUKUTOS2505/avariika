# -*- coding: utf-8 -*-
"""A/B: слева мой SM_FireExtinguisher (hand-built mat), справа meshy-нативный SM_FE_meshy
(_test, родной материал+текстуры meshy). Перед игроком, дневной свет. Проверка одноцветности."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label().startswith('PROD_'):
        eas.destroy_actor(a)
out = []
PAIRS = [('PROD_MINE', '/Game/Avariika/Meshes/SM_FireExtinguisher.SM_FireExtinguisher', -174.0),
         ('PROD_MESHY', '/Game/Avariika/Meshes/_test/SM_FE_meshy.SM_FE_meshy', -94.0)]
for lbl, path, dy in PAIRS:
    m = unreal.load_asset(path)
    if not m:
        out.append('no ' + path); continue
    a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(165.0, dy, 360.0))
    a.set_actor_label(lbl)
    a.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    a.static_mesh_component.set_static_mesh(m)
    b = m.get_bounds().box_extent
    mx = max(b.x, b.y, b.z) * 2.0
    s = 70.0 / mx if mx > 0 else 1.0
    a.set_actor_scale3d(unreal.Vector(s, s, s))
    a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=200.0, roll=0.0), False)
    out.append('%s placed (dy=%.0f)' % (lbl, dy))
open(r'C:\unrealEngine\avariika\Saved\ab_fe.txt', 'w', encoding='utf-8').write('\n'.join(out))
