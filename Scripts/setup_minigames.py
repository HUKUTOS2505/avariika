# -*- coding: utf-8 -*-
"""Мини-игры: щиток (курсор), труба (вентиль), генератор (стартер), туалет-сиденье, тестер в мир.

Запуск: UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="Scripts/setup_minigames.py"
Идемпотентен.
"""
import unreal

out = []

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not les.load_level('/Game/FirstPerson/Lvl_FirstPerson'):
    raise RuntimeError('Не удалось загрузить уровень')

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

# Щиток: мини-игра «курсор» + требуется тестер
breaker = by_label.get('Repairable_Breaker')
if breaker:
    breaker.set_editor_property('MinigameType', unreal.RepairMinigameType.CURSOR)
    breaker.set_editor_property('RequiredTool', 'Tester')
    out.append('Breaker: MinigameType=Cursor, RequiredTool=Tester')
else:
    out.append('WARN: Repairable_Breaker не найден')

# Труба: мини-игра «вентиль» — без инструмента, жать E размеренно
pipe = by_label.get('Repairable_GasPipe')
if pipe:
    pipe.set_editor_property('MinigameType', unreal.RepairMinigameType.VALVE)
    pipe.set_editor_property('RequiredTool', 'None')
    out.append('GasPipe: MinigameType=Valve, без инструмента')
else:
    out.append('WARN: Repairable_GasPipe не найдена')

# Генератор: мини-игра «стартер» — сварочник в руках остаётся обязательным
gen = by_label.get('Repairable_Generator')
if gen:
    gen.set_editor_property('MinigameType', unreal.RepairMinigameType.STARTER)
    gen.set_editor_property('RequiredTool', 'Welder')
    out.append('Generator: MinigameType=Starter, RequiredTool=Welder')
else:
    out.append('WARN: Repairable_Generator не найден')

# Туалет: низкий куб-сиденье (на него телепортируемся при посадке)
toilet = by_label.get('Toilet')
if toilet:
    toilet.set_actor_scale3d(unreal.Vector(0.9, 0.9, 0.8))
    loc = toilet.get_actor_location()
    toilet.set_actor_location(unreal.Vector(loc.x, loc.y, 25.0), False, False)  # пол -15 + полувысота 40
    out.append('Toilet: куб-сиденье 90x90x80, z=25')
else:
    out.append('WARN: Toilet не найден')

# Тестер — инструмент для щитка, лежит у ГАЗели
if 'Tester' not in by_label:
    tester = eas.spawn_actor_from_class(unreal.PickupItem, unreal.Vector(240.0, 60.0, 342.0), unreal.Rotator(0.0, 0.0, 0.0))
    tester.set_actor_label('Tester')
    tester.set_actor_scale3d(unreal.Vector(0.2, 0.12, 0.05))
    tester.get_editor_property('MeshComponent').set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    tester.set_editor_property('DisplayName', 'Тестер')
    tester.set_editor_property('ItemSize', unreal.ItemSize.LIGHT)
    tester.set_editor_property('Charges', -1)
    tester.set_editor_property('ToolTag', 'Tester')
    out.append('Tester заспавнен у ГАЗели')
else:
    out.append('Tester уже лежит')

if not les.save_current_level():
    raise RuntimeError('Уровень не сохранился')
out.append('saved ok')

with open(r'C:\unrealEngine\avariika\Saved\minigames_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
