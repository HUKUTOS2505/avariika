# -*- coding: utf-8 -*-
"""Сбавить пересвет: экспозиция 5.5, свет мягче и чуть дальше, чтобы проявились
реальные PBR-цвета предметов (красный огнетушитель и т.д.)."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
m = by.get('PROD_Light')
if m:
    m.set_actor_location(unreal.Vector(90.0, -134.0, 480.0), False, True)
    lc = m.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(45000.0)
    lc.set_attenuation_radius(1800.0)
    out.append('light softer')
p = by.get('PROD_PPV')
if p:
    s = p.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 5.5)
    p.set_editor_property('settings', s)
    out.append('exposure 5.5')
open(r'D:\unrealEngine\avariika\Saved\product_exposure.txt', 'w', encoding='utf-8').write('\n'.join(out))
