# -*- coding: utf-8 -*-
"""Ночная смена: притушить солнце и небо, поднять туман, ограничить автоэкспозицию.

Запуск: UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="Scripts/night_atmosphere.py"
Идемпотентен (просто выставляет значения).
"""
import unreal

out = []

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not les.load_level('/Game/FirstPerson/Lvl_FirstPerson'):
    raise RuntimeError('Не удалось загрузить уровень')

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
actors = eas.get_all_level_actors()


def first_of(cls):
    for a in actors:
        if isinstance(a, cls):
            return a
    return None


# Солнце → луна: слабый холодный свет
sun = first_of(unreal.DirectionalLight)
if sun:
    light = sun.get_editor_property('light_component')
    light.set_editor_property('intensity', 0.6)  # lux, лунная ночь
    light.set_editor_property('light_color', unreal.Color(120, 150, 255, 255))
    out.append('DirectionalLight: 0.6 lux, холодный')
else:
    out.append('WARN: DirectionalLight не найден')

# Небо почти не подсвечивает
sky = first_of(unreal.SkyLight)
if sky:
    comp = sky.get_editor_property('light_component')
    comp.set_editor_property('intensity', 0.15)
    out.append('SkyLight: 0.15')
else:
    out.append('WARN: SkyLight не найден')

# Туман: плотнее и ниже к полу
fog = first_of(unreal.ExponentialHeightFog)
if fog:
    fog.set_actor_location(unreal.Vector(0.0, 0.0, -40.0), False, False)
    comp = fog.get_editor_property('component')
    comp.set_editor_property('fog_density', 0.045)
    comp.set_editor_property('fog_height_falloff', 0.4)
    comp.set_editor_property('start_distance', 600.0)
    out.append('Fog: density 0.045')
else:
    out.append('WARN: ExponentialHeightFog не найден')

# Автоэкспозиция не должна вытягивать ночь в день: ограничиваем усиление
ppv = None
for a in actors:
    if isinstance(a, unreal.PostProcessVolume) and a.get_actor_label() == 'PPV_Night':
        ppv = a
        break
if ppv is None:
    ppv = eas.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0))
    ppv.set_actor_label('PPV_Night')
ppv.set_editor_property('unbound', True)
settings = ppv.get_editor_property('settings')
settings.set_editor_property('override_auto_exposure_min_brightness', True)
settings.set_editor_property('auto_exposure_min_brightness', 0.25)
settings.set_editor_property('override_auto_exposure_max_brightness', True)
settings.set_editor_property('auto_exposure_max_brightness', 0.8)
ppv.set_editor_property('settings', settings)
out.append('PPV_Night: экспозиция 0.25..0.8')

if not les.save_current_level():
    raise RuntimeError('Уровень не сохранился')
out.append('saved ok')

with open(r'D:\unrealEngine\avariika\Saved\night_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
