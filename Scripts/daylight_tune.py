# -*- coding: utf-8 -*-
"""Сделать дневной белый свет: солнце белое, выше (pitch -58), без тёплой температуры,
интенсивнее; перекапчуть SkyLight. Убрать закатный оранжевый."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

dl = by.get('DirectionalLight')
if dl:
    lc = dl.get_component_by_class(unreal.DirectionalLightComponent)
    try:
        lc.set_editor_property('use_temperature', False)
    except Exception as e:
        out.append('temp? %s' % e)
    lc.set_light_color(unreal.LinearColor(1.0, 1.0, 1.0, 1.0))
    lc.set_intensity(9.0)
    dl.set_actor_rotation(unreal.Rotator(pitch=-58.0, yaw=-35.0, roll=0.0), False)
    dl.modify()
    out.append('sun white, intensity 9, pitch -58')

sk = by.get('SkyLight')
if sk:
    lc = sk.get_component_by_class(unreal.SkyLightComponent)
    lc.set_intensity(2.0)
    lc.recapture_sky()
    sk.modify()
    out.append('skylight 2.0 recaptured')

# чуть приглушим экспоз-bias, раз свет ярче
ppv = by.get('PPV_Night')
if ppv:
    s = ppv.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 0.5)
    ppv.set_editor_property('settings', s)
    out.append('exposure bias 0.5')

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\daylight_tune.txt', 'w', encoding='utf-8').write('\n'.join(out))
