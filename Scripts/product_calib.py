# -*- coding: utf-8 -*-
"""Калибровка света/экспозиции под истинный цвет: свет 900cd, экспозиция bias 1.0,
bloom приглушён — чтобы красный огнетушитель был красным, а не белым."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
m = by.get('PROD_Light')
if m:
    m.set_actor_location(unreal.Vector(60.0, -134.0, 440.0), False, True)
    lc = m.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(900.0)
    lc.set_attenuation_radius(1200.0)
p = by.get('PROD_PPV')
if p:
    s = p.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 1.0)
    s.set_editor_property('override_bloom_intensity', True)
    s.set_editor_property('bloom_intensity', 0.1)
    p.set_editor_property('settings', s)
open(r'D:\unrealEngine\avariika\Saved\product_calib.txt', 'w', encoding='utf-8').write('ok')
