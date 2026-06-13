# -*- coding: utf-8 -*-
"""Поставить рации стартовый HoldRotation (~105 вправо) на размещённом акторе Radio.
Проверить, что у класса Floodlight источник света — SpotLight."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []

# Radio HoldRotation
radios = [a for a in eas.get_all_level_actors() if a.get_actor_label() == 'Radio']
for a in radios:
    try:
        a.set_editor_property('HoldRotation', unreal.Rotator(pitch=0.0, yaw=-105.0, roll=0.0))
        a.modify()
        hr = a.get_editor_property('HoldRotation')
        out.append('Radio HoldRotation = (p=%.0f,y=%.0f,r=%.0f)' % (hr.pitch, hr.yaw, hr.roll))
    except Exception as e:
        out.append('Radio err %s' % e)

# Проверка Floodlight = SpotLight (заспавнить и глянуть компонент света)
try:
    cls = unreal.load_class(None, '/Script/Avaryo.Floodlight')
    fl = eas.spawn_actor_from_class(cls, unreal.Vector(0, 0, -6000))
    spot = fl.get_component_by_class(unreal.SpotLightComponent)
    pt = fl.get_component_by_class(unreal.PointLightComponent)
    out.append('Floodlight: SpotLight=%s outer=%s' % (
        bool(spot), spot.get_editor_property('outer_cone_angle') if spot else '-'))
    eas.destroy_actor(fl)
except Exception as e:
    out.append('floodlight check err %s' % e)

les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'D:\unrealEngine\avariika\Saved\fix_radio_rot_verify_spot.txt', 'w', encoding='utf-8').write('\n'.join(out))
