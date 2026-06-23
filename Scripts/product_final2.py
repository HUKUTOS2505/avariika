# -*- coding: utf-8 -*-
"""Финал: два мягких света (фронт+верх) ~1600/1200cd, экспозиция bias 2.3 — цвета
читаются, без пересвета."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
# верхний
m = by.get('PROD_Light')
if m:
    m.set_actor_location(unreal.Vector(70.0, -134.0, 470.0), False, True)
    lc = m.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(1300.0)
    lc.set_attenuation_radius(1400.0)
# фронтальный заполняющий низко у камеры (вне кадра снизу), мягко в лоб
f = by.get('PROD_Fill2')
if not f:
    f = eas.spawn_actor_from_class(unreal.PointLight, unreal.Vector(30.0, -134.0, 300.0))
    f.set_actor_label('PROD_Fill2')
f.set_actor_location(unreal.Vector(30.0, -134.0, 300.0), False, True)
fc = f.get_component_by_class(unreal.PointLightComponent)
fc.set_intensity(1500.0)
fc.set_attenuation_radius(1400.0)
p = by.get('PROD_PPV')
if p:
    s = p.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 2.3)
    p.set_editor_property('settings', s)
open(r'C:\unrealEngine\avariika\Saved\product_final2.txt', 'w', encoding='utf-8').write('ok')
