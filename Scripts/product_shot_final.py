# -*- coding: utf-8 -*-
"""Финальная правка продуктового кадра: убрать fill (блик), один верхний свет над
предметами, экспозиция 9, предметы ниже/ближе по центру перед игроком (+X)."""
import unreal

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

# убрать fill-свет (он давал блик в центр)
f = by.get('PROD_Fill')
if f:
    eas.destroy_actor(f)
    out.append('fill removed')

# единственный свет — высоко над предметами
m = by.get('PROD_Light')
if m:
    m.set_actor_location(unreal.Vector(190.0, 10.0, 560.0), False, True)
    lc = m.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(220000.0)
    lc.set_attenuation_radius(2200.0)
    out.append('top light')

# предметы ниже и ближе, плотный ряд по центру
spots = {
    'PROD_SM_Radio':            (175.0, -95.0, 330.0),
    'PROD_SM_Battery':          (170.0, -35.0, 325.0),
    'PROD_SM_FireExtinguisher': (185.0, 35.0, 340.0),
    'PROD_SM_FirstAidKit':      (180.0, 110.0, 330.0),
}
for lbl, (x, y, z) in spots.items():
    a = by.get(lbl)
    if a:
        a.set_actor_location(unreal.Vector(x, y, z), False, True)
        out.append('%s moved' % lbl)

# экспозиция мягче
p = by.get('PROD_PPV')
if p:
    s = p.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 9.0)
    p.set_editor_property('settings', s)
    out.append('exposure 9')

open(r'C:\unrealEngine\avariika\Saved\product_shot_final.txt', 'w', encoding='utf-8').write('\n'.join(out))
