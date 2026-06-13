# -*- coding: utf-8 -*-
"""Поставить 4 новых меша в ряд высоко над уровнем + яркий свет для чистого кадра.
Temp-акторы по префиксу метки 'PROD_' — потом снести. Сначала чистим старые PROD_."""
import unreal

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []

# снести предыдущие PROD_
for a in list(eas.get_all_level_actors()):
    try:
        if a.get_actor_label().startswith('PROD_'):
            eas.destroy_actor(a)
    except Exception:
        pass

meshes = [
    ('SM_Radio', -150.0),
    ('SM_Battery', -50.0),
    ('SM_FireExtinguisher', 60.0),
    ('SM_FirstAidKit', 170.0),
]
Z = 1000.0
for name, y in meshes:
    try:
        m = unreal.load_asset('/Game/Avariika/Meshes/%s.%s' % (name, name))
        if not m:
            out.append('NO MESH ' + name)
            continue
        a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0.0, y, Z))
        a.set_actor_label('PROD_' + name)
        a.static_mesh_component.set_static_mesh(m)
        b = m.get_bounds()
        a.set_actor_location(unreal.Vector(0.0, y, Z + b.box_extent.z))
        out.append('%s ok ext=(%.0f,%.0f,%.0f)' % (name, b.box_extent.x, b.box_extent.y, b.box_extent.z))
    except Exception as e:
        out.append('%s FAIL %s' % (name, e))

try:
    light = eas.spawn_actor_from_class(unreal.PointLight, unreal.Vector(-200.0, 0.0, Z + 200.0))
    light.set_actor_label('PROD_Light')
    lc = light.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(150000.0)
    lc.set_attenuation_radius(3000.0)
    out.append('light ok')
except Exception as e:
    out.append('light FAIL %s' % e)

open(r'D:\unrealEngine\avariika\Saved\product_shot.txt', 'w', encoding='utf-8').write('\n'.join(out))
