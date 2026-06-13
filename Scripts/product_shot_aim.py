# -*- coding: utf-8 -*-
"""Поставить ряд строго по лучу камеры PIE: cam=(-13.5,-134,377.8), +X, FOV90.
Центр взгляда y=-134, уровень глаз z~375, перед камерой x~170, лицом к камере."""
import unreal

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

CY = -134.0
Z = 375.0
X = 170.0
spots = {
    'PROD_SM_Radio':            (X, CY - 115.0),
    'PROD_SM_Battery':          (X, CY - 45.0),
    'PROD_SM_FireExtinguisher': (X, CY + 35.0),
    'PROD_SM_FirstAidKit':      (X, CY + 120.0),
}
for lbl, (x, y) in spots.items():
    a = by.get(lbl)
    if a:
        a.set_actor_scale3d(unreal.Vector(2.0, 2.0, 2.0))
        a.set_actor_location(unreal.Vector(x, y, Z), False, True)
        a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=180.0, roll=0.0), False)
        out.append('%s -> (%.0f,%.0f,%.0f)' % (lbl, x, y, Z))

m = by.get('PROD_Light')
if m:
    m.set_actor_location(unreal.Vector(120.0, CY, 540.0), False, True)
    lc = m.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(180000.0)
    lc.set_attenuation_radius(2000.0)
    out.append('light over center')

p = by.get('PROD_PPV')
if p:
    s = p.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 8.5)
    p.set_editor_property('settings', s)
    out.append('exposure 8.5')

open(r'D:\unrealEngine\avariika\Saved\product_shot_aim.txt', 'w', encoding='utf-8').write('\n'.join(out))
