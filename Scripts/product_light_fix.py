# -*- coding: utf-8 -*-
"""Освещение продуктового ряда: top-down над предметами + заполняющий со стороны камеры
(высоко, вне кадра, чтобы не было контрового силуэта). Плюс temp PPV с фикс-экспозицией."""
import unreal

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

# главный свет — прямо над предметами, top-down
key = by.get('PROD_Light')
if key:
    key.set_actor_location(unreal.Vector(205.0, 20.0, 620.0), False, True)
    lc = key.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(300000.0)
    lc.set_attenuation_radius(2500.0)
    out.append('main light top-down')

# заполняющий со стороны камеры, высоко (вне кадра), мягко в лоб предметам
fill = eas.spawn_actor_from_class(unreal.PointLight, unreal.Vector(40.0, 0.0, 520.0))
fill.set_actor_label('PROD_Fill')
fc = fill.get_component_by_class(unreal.PointLightComponent)
fc.set_intensity(80000.0)
fc.set_attenuation_radius(2000.0)
out.append('fill light')

# temp PPV с фикс-экспозицией, чтобы ночная авто-экспозиция не давила кадр
ppv = eas.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(150.0, 20.0, 380.0))
ppv.set_actor_label('PROD_PPV')
ppv.set_editor_property('unbound', True)
s = ppv.get_editor_property('settings')
s.set_editor_property('override_auto_exposure_method', True)
s.set_editor_property('auto_exposure_method', unreal.AutoExposureMethod.AEM_MANUAL)
s.set_editor_property('override_auto_exposure_bias', True)
s.set_editor_property('auto_exposure_bias', 11.0)
ppv.set_editor_property('settings', s)
out.append('temp PPV manual exposure')

open(r'C:\unrealEngine\avariika\Saved\product_light_fix.txt', 'w', encoding='utf-8').write('\n'.join(out))
