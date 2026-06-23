# -*- coding: utf-8 -*-
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
labels = ['Radio', 'Battery', 'FireExtinguisher', 'FirstAidKit', 'WeldingMachine',
          'Gazelle_Mesh', 'ExitZone_Gazelle']
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
for a in eas.get_all_level_actors():
    if a.get_class().get_name() == 'PlayerStart':
        l = a.get_actor_location()
        out.append('PlayerStart: (%.0f,%.0f,%.0f)' % (l.x, l.y, l.z))
for lbl in labels:
    a = by.get(lbl)
    if a:
        l = a.get_actor_location()
        out.append('%s: (%.0f,%.0f,%.0f)' % (lbl, l.x, l.y, l.z))
open(r'C:\unrealEngine\avariika\Saved\loc_report.txt', 'w', encoding='utf-8').write('\n'.join(out))
