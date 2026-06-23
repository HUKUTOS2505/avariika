# -*- coding: utf-8 -*-
"""Кладёт термос (кофе) в Lvl_FirstPerson. Headless (from_class, редактор закрыт).
Идемпотентно: ищет актора по метке 'Thermos'."""
import unreal

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les.load_level('/Game/FirstPerson/Lvl_FirstPerson')
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
out = []

if 'Thermos' not in by_label:
    t = eas.spawn_actor_from_class(unreal.PickupItem, unreal.Vector(150.0, 40.0, 342.0), unreal.Rotator(0, 0, 0))
    t.set_actor_label('Thermos')
    t.set_actor_scale3d(unreal.Vector(0.12, 0.12, 0.22))
    t.get_editor_property('MeshComponent').set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cylinder'))
    t.set_editor_property('DisplayName', 'Термос')
    t.set_editor_property('ItemEffect', unreal.ItemEffect.DRINK)
    t.set_editor_property('ItemSize', unreal.ItemSize.LIGHT)
    t.set_editor_property('Charges', 3)            # три глотка на смену
    try:
        t.set_editor_property('EffectMagnitude', 60.0)  # столько выносливости за глоток
    except Exception as e:
        out.append('EffectMagnitude? ' + str(e))
    try:
        t.set_editor_property('UseCastTime', 2.5)       # пьёшь ~2.5 с (held)
    except Exception as e:
        out.append('UseCastTime? ' + str(e))
    out.append('Thermos заспавнен')
else:
    out.append('Thermos уже стоит')

les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('сохранено')
open(r'C:\unrealEngine\avariika\Saved\place_thermos.txt', 'w', encoding='utf-8').write('\n'.join(out))
