# -*- coding: utf-8 -*-
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
m = by.get('PROD_Light')
if m:
    m.set_actor_location(unreal.Vector(80.0, -134.0, 470.0), False, True)
    lc = m.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(40000.0)
    lc.set_attenuation_radius(1800.0)
p = by.get('PROD_PPV')
if p:
    s = p.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 6.0)
    p.set_editor_property('settings', s)
open(r'C:\unrealEngine\avariika\Saved\product_balance.txt', 'w', encoding='utf-8').write('ok')
