# -*- coding: utf-8 -*-
"""Отчёт по размещённым ремонтируемым + инструментам: позиция, поломка, инструмент, мини-игра."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []


def loc(a):
    l = a.get_actor_location(); r = a.get_actor_rotation()
    return '(%.0f,%.0f,%.0f) yaw=%.0f' % (l.x, l.y, l.z, r.yaw)


for a in eas.get_all_level_actors():
    cn = a.get_class().get_name()
    lbl = a.get_actor_label()
    if cn == 'Repairable':
        try:
            dn = a.get_editor_property('DisplayName')
            mg = a.get_editor_property('MinigameType')
            rt = a.get_editor_property('RequiredTool')
            br = a.get_editor_property('bBroken')
            botch = a.get_editor_property('bAllowBotch')
            dur = a.get_editor_property('RepairDuration')
            out.append('REPAIRABLE %-22s %s | name=%s minigame=%s tool=%s broken=%s botch=%s dur=%.0f' % (
                lbl, loc(a), dn, mg, rt, br, botch, dur))
        except Exception as e:
            out.append('%s err %s' % (lbl, e))
    elif lbl in ('WeldingMachine', 'Tester', 'Fuse', 'Loot_Fuse_N') or cn == 'PlayerStart':
        out.append('%-18s [%s] %s' % (lbl, cn, loc(a)))

out.sort()
open(r'D:\unrealEngine\avariika\Saved\report_repairables.txt', 'w', encoding='utf-8').write('\n'.join(out))
