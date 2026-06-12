# -*- coding: utf-8 -*-
"""Кладёт ставимые предметы (растяжка, прожектор) на платформу спавна. §18.

Запуск: UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="D:\\unrealEngine\\avariika\\Scripts\\setup_traps.py"
Путь к скрипту — АБСОЛЮТНЫЙ (относительный резолвится от папки движка). Идемпотентен.
"""
import unreal

out = []

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not les.load_level('/Game/FirstPerson/Lvl_FirstPerson'):
    raise RuntimeError('Не удалось загрузить уровень')

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

if 'TrapKit' not in by_label:
    trap = eas.spawn_actor_from_class(unreal.PickupItem, unreal.Vector(120.0, 120.0, 342.0), unreal.Rotator(0.0, 0.0, 0.0))
    trap.set_actor_label('TrapKit')
    trap.set_actor_scale3d(unreal.Vector(0.14, 0.14, 0.14))
    trap.get_editor_property('MeshComponent').set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    trap.set_editor_property('DisplayName', 'Растяжка')
    trap.set_editor_property('ItemEffect', unreal.ItemEffect.DEPLOY_TRAP)
    trap.set_editor_property('ItemSize', unreal.ItemSize.LIGHT)
    trap.set_editor_property('Charges', 2)
    out.append('TrapKit заспавнен')
else:
    out.append('TrapKit уже стоит')

if 'LightKit' not in by_label:
    lamp = eas.spawn_actor_from_class(unreal.PickupItem, unreal.Vector(120.0, 180.0, 342.0), unreal.Rotator(0.0, 0.0, 0.0))
    lamp.set_actor_label('LightKit')
    lamp.set_actor_scale3d(unreal.Vector(0.14, 0.14, 0.18))
    lamp.get_editor_property('MeshComponent').set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    lamp.set_editor_property('DisplayName', 'Прожектор')
    lamp.set_editor_property('ItemEffect', unreal.ItemEffect.DEPLOY_LIGHT)
    lamp.set_editor_property('ItemSize', unreal.ItemSize.LIGHT)
    lamp.set_editor_property('Charges', 1)
    out.append('LightKit заспавнен')
else:
    out.append('LightKit уже стоит')

if not les.save_current_level():
    raise RuntimeError('Уровень не сохранился')
out.append('saved ok')

with open(r'D:\unrealEngine\avariika\Saved\setup_traps_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
