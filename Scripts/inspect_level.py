# -*- coding: utf-8 -*-
"""Печатает акторов Lvl_FirstPerson с позициями и габаритами — чтобы выбрать точки размещения."""
import unreal

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.load_level('/Game/FirstPerson/Lvl_FirstPerson')

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
lines = []
for a in eas.get_all_level_actors():
    loc = a.get_actor_location()
    origin, extent = a.get_actor_bounds(False)
    lines.append('%s | %s | loc=(%.0f, %.0f, %.0f) | ext=(%.0f, %.0f, %.0f)' % (
        a.get_actor_label(), a.get_class().get_name(),
        loc.x, loc.y, loc.z, extent.x, extent.y, extent.z))

with open(r'D:\unrealEngine\avariika\Saved\level_actors.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines))
