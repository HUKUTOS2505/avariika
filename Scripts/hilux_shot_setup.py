# -*- coding: utf-8 -*-
"""Временно поставить Hilux (Gazelle_Mesh) перед игроком PIE (cam -13.5,-134,377, +X)
для продуктового кадра: 3/4 ракурс, свет, temp PPV. Запомнить исходный трансформ в файл."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
out = []
truck = by.get('Gazelle_Mesh')
if truck:
    L = truck.get_actor_location(); R = truck.get_actor_rotation(); S = truck.get_actor_scale3d()
    out.append('ORIG loc=(%.2f,%.2f,%.2f) rot=(%.2f,%.2f,%.2f) scale=(%.4f,%.4f,%.4f)' % (
        L.x, L.y, L.z, R.pitch, R.yaw, R.roll, S.x, S.y, S.z))
    comp = truck.static_mesh_component
    comp.set_mobility(unreal.ComponentMobility.MOVABLE)
    truck.set_actor_location(unreal.Vector(900.0, -134.0, 330.0), False, True)
    truck.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=125.0, roll=0.0), False)
    out.append('moved to front (900,-134,330) yaw125')
else:
    out.append('NO Gazelle_Mesh')

# свет: высоко спереди-сбоку, чтобы осветить 5м грузовик без контрового блика в камеру
light = by.get('PROD_TruckLight')
if not light:
    light = eas.spawn_actor_from_class(unreal.PointLight, unreal.Vector(450.0, -450.0, 650.0))
    light.set_actor_label('PROD_TruckLight')
lc = light.get_component_by_class(unreal.PointLightComponent)
lc.set_intensity(60000.0)
lc.set_attenuation_radius(4000.0)
# temp PPV манульная экспозиция
ppv = by.get('PROD_TruckPPV')
if not ppv:
    ppv = eas.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(450.0, -134.0, 330.0))
    ppv.set_actor_label('PROD_TruckPPV')
ppv.set_editor_property('unbound', True)
s = ppv.get_editor_property('settings')
s.set_editor_property('override_auto_exposure_method', True)
s.set_editor_property('auto_exposure_method', unreal.AutoExposureMethod.AEM_MANUAL)
s.set_editor_property('override_auto_exposure_bias', True)
s.set_editor_property('auto_exposure_bias', 1.5)
s.set_editor_property('override_bloom_intensity', True)
s.set_editor_property('bloom_intensity', 0.1)
ppv.set_editor_property('settings', s)
out.append('light+ppv set')
open(r'D:\unrealEngine\avariika\Saved\hilux_shot_setup.txt', 'w', encoding='utf-8').write('\n'.join(out))
