# -*- coding: utf-8 -*-
"""Финальный кадр: убрать fill, один мягкий верхне-фронтальный свет 1100cd, bias 1.7."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
for lbl in ['PROD_Fill2', 'PROD_Fill']:
    a = by.get(lbl)
    if a:
        eas.destroy_actor(a)
m = by.get('PROD_Light')
if m:
    m.set_actor_location(unreal.Vector(80.0, -134.0, 430.0), False, True)
    lc = m.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(1100.0)
    lc.set_attenuation_radius(1300.0)
p = by.get('PROD_PPV')
if p:
    s = p.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 1.7)
    p.set_editor_property('settings', s)
open(r'D:\unrealEngine\avariika\Saved\product_finalshot.txt', 'w', encoding='utf-8').write('ok')
