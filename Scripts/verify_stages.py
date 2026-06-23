# -*- coding: utf-8 -*-
"""Прочитать обратно PrereqStages у ремонтируемых + конфиг новых предметов."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

for lbl in ['Repairable_GasPipe', 'Repairable_Generator', 'Repairable_Breaker']:
    a = by.get(lbl)
    if not a:
        out.append('%s НЕТ' % lbl); continue
    stages = a.get_editor_property('PrereqStages')
    rt = a.get_editor_property('RequiredTool')
    parts = []
    for s in stages:
        parts.append('%s/%s/%.0fs' % (str(s.get_editor_property('kind')).split('.')[-1],
                                      s.get_editor_property('item_tag'), s.get_editor_property('duration')))
    out.append('%s: tool=%s stages=[%s]' % (lbl, rt, ' | '.join(parts)))

for lbl in ['Cable', 'FuelCanister', 'WeldingMachine', 'Fuse']:
    a = by.get(lbl)
    if not a:
        out.append('%s НЕТ' % lbl); continue
    out.append('%s: size=%s tooltag=%s charges=%s' % (
        lbl, str(a.get_editor_property('ItemSize')).split('.')[-1],
        a.get_editor_property('ToolTag'), a.get_editor_property('Charges')))

open(r'C:\unrealEngine\avariika\Saved\verify_stages.txt', 'w', encoding='utf-8').write('\n'.join(out))
