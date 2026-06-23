# -*- coding: utf-8 -*-
"""Раскладывает лут по тёмным углам карты — стимул исследовать ночь.

Запуск: UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="Scripts/scatter_loot.py"
Идемпотентен (по меткам Loot_*).
"""
import unreal

out = []

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not les.load_level('/Game/FirstPerson/Lvl_FirstPerson'):
    raise RuntimeError('Не удалось загрузить уровень')

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
labels = {a.get_actor_label() for a in eas.get_all_level_actors()}

ITEMS = '/Game/Avariika/Items/'
# (метка, blueprint, x, y) — пол z=-15, предметы чуть над ним
LOOT = [
    ('Loot_Battery_SE',    'BP_Battery',        1600.0, -1650.0),
    ('Loot_Battery_NW',    'BP_Battery',       -1650.0,  1600.0),
    ('Loot_FirstAid_NE',   'BP_FirstAidKit',    1650.0,  1650.0),
    ('Loot_Cigarettes_SW', 'BP_Cigarettes',    -1450.0, -1700.0),
    ('Loot_Fuse_N',        'BP_Fuse',           -250.0,  1750.0),
]

for label, bp_name, x, y in LOOT:
    if label in labels:
        out.append(label + ' уже лежит')
        continue
    bp = unreal.EditorAssetLibrary.load_asset(ITEMS + bp_name)
    if not bp:
        out.append('WARN: нет ' + bp_name)
        continue
    actor = eas.spawn_actor_from_class(bp.generated_class(), unreal.Vector(x, y, 25.0), unreal.Rotator(0.0, 0.0, 0.0))
    actor.set_actor_label(label)
    out.append('%s в (%.0f, %.0f)' % (label, x, y))

if not les.save_current_level():
    raise RuntimeError('Уровень не сохранился')
out.append('saved ok')

with open(r'C:\unrealEngine\avariika\Saved\loot_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
