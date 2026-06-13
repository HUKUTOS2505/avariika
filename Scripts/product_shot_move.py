# -*- coding: utf-8 -*-
"""Передвинуть PROD_-меши прямо перед игроком (смотрит +X от 0,0,302) на уровень глаз,
масштаб x2.5 для читаемости, свет рядом. PIE отрендерит камеру игрока."""
import unreal

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
# x вперёд, z уровень глаз, y — раскладка слева-направо
spots = {
    'PROD_SM_Radio':           (200.0, -120.0, 350.0),
    'PROD_SM_Battery':         (190.0, -40.0, 345.0),
    'PROD_SM_FireExtinguisher':(210.0, 55.0, 360.0),
    'PROD_SM_FirstAidKit':     (205.0, 155.0, 350.0),
}
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
for lbl, (x, y, z) in spots.items():
    a = by.get(lbl)
    if not a:
        out.append('%s НЕТ' % lbl)
        continue
    try:
        comp = a.static_mesh_component
        comp.set_mobility(unreal.ComponentMobility.MOVABLE)
        a.set_actor_scale3d(unreal.Vector(2.5, 2.5, 2.5))
        a.set_actor_location(unreal.Vector(x, y, z), False, True)
        a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=180.0, roll=0.0), False)
        out.append('%s -> (%.0f,%.0f,%.0f)' % (lbl, x, y, z))
    except Exception as e:
        out.append('%s FAIL %s' % (lbl, e))

light = by.get('PROD_Light')
if light:
    try:
        light.set_actor_location(unreal.Vector(60.0, 0.0, 470.0), False, True)
        lc = light.get_component_by_class(unreal.PointLightComponent)
        lc.set_intensity(120000.0)
        lc.set_attenuation_radius(1500.0)
        out.append('light moved')
    except Exception as e:
        out.append('light FAIL %s' % e)

open(r'D:\unrealEngine\avariika\Saved\product_shot_move.txt', 'w', encoding='utf-8').write('\n'.join(out))
