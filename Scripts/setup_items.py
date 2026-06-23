# -*- coding: utf-8 -*-
"""Донастройка после ночных фич: рация в мир, газ у трубы, сварочник как инструмент генератора.

Запуск: UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="Scripts/setup_items.py"
Идемпотентен.
"""
import unreal

out = []

# --- ToolTag сварочника (CDO Blueprint'а) ---
welder_set = False
for path in unreal.EditorAssetLibrary.list_assets('/Game/Avariika', recursive=True):
    if 'WeldingMachine' in path:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        cdo = unreal.get_default_object(bp.generated_class())
        cdo.set_editor_property('ToolTag', 'Welder')
        unreal.EditorAssetLibrary.save_loaded_asset(bp)
        out.append('ToolTag=Welder: ' + path)
        welder_set = True
        break
if not welder_set:
    out.append('WARN: BP_WeldingMachine не найден — ToolTag не задан')

# --- Уровень ---
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not les.load_level('/Game/FirstPerson/Lvl_FirstPerson'):
    raise RuntimeError('Не удалось загрузить уровень')

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

# Газовая труба травит газ, пока сломана
pipe = by_label.get('Repairable_GasPipe')
if pipe:
    pipe.set_editor_property('bLeaksGasWhenBroken', True)
    out.append('GasPipe: bLeaksGasWhenBroken=True')
else:
    out.append('WARN: Repairable_GasPipe не найдена')

# Генератор чинится только со сварочником в руках
gen = by_label.get('Repairable_Generator')
if gen:
    gen.set_editor_property('RequiredTool', 'Welder')
    out.append('Generator: RequiredTool=Welder')
else:
    out.append('WARN: Repairable_Generator не найден')

# Биотуалет в дальнем углу карты (пол z=-15, куб 100 см: z = -15 + 50*scale.z)
if 'Toilet' not in by_label:
    toilet = eas.spawn_actor_from_class(unreal.Toilet, unreal.Vector(-1650.0, -1550.0, 95.0), unreal.Rotator(0.0, 0.0, 0.0))
    toilet.set_actor_label('Toilet')
    toilet.set_actor_scale3d(unreal.Vector(0.9, 0.9, 2.2))
    toilet.get_editor_property('MeshComponent').set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    out.append('Toilet заспавнен')
else:
    out.append('Toilet уже стоит')

# Рация на платформе спавна
if 'Radio' not in by_label:
    radio = eas.spawn_actor_from_class(unreal.PickupItem, unreal.Vector(150.0, -80.0, 342.0), unreal.Rotator(0.0, 0.0, 0.0))
    radio.set_actor_label('Radio')
    radio.set_actor_scale3d(unreal.Vector(0.18, 0.12, 0.3))
    radio.get_editor_property('MeshComponent').set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    radio.set_editor_property('DisplayName', 'Рация')
    radio.set_editor_property('ItemEffect', unreal.ItemEffect.RADIO)
    radio.set_editor_property('ItemSize', unreal.ItemSize.LIGHT)
    radio.set_editor_property('Charges', -1)
    out.append('Radio заспавнена')
else:
    out.append('Radio уже стоит')

if not les.save_current_level():
    raise RuntimeError('Уровень не сохранился')
out.append('saved ok')

with open(r'C:\unrealEngine\avariika\Saved\setup_items_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
