# -*- coding: utf-8 -*-
"""Замедлить тайминговые мини-игры на 50%. MinigameType по строке (надёжно)."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
n = 0
for a in eas.get_all_level_actors():
    if a.get_class().get_name() != 'Repairable':
        continue
    n += 1
    lbl = a.get_actor_label()
    s = str(a.get_editor_property('MinigameType')).upper()
    try:
        if 'CURSOR' in s:
            old = a.get_editor_property('MinigameCursorSpeed')
            a.set_editor_property('MinigameCursorSpeed', old * 0.5)
            gw = a.get_editor_property('MinigameGreenHalfWidth')
            a.set_editor_property('MinigameGreenHalfWidth', gw * 1.3)
            a.modify()
            out.append('%s Cursor: speed %.2f->%.2f green %.3f->%.3f' % (lbl, old, old*0.5, gw, gw*1.3))
        elif 'STARTER' in s:
            old = a.get_editor_property('StarterChargeTime')
            a.set_editor_property('StarterChargeTime', old * 2.0)
            a.modify()
            out.append('%s Starter: chargeTime %.2f->%.2f' % (lbl, old, old*2.0))
        elif 'VALVE' in s:
            out.append('%s Valve (ритм) — не трогаю' % lbl)
        else:
            out.append('%s MinigameType=%s' % (lbl, s))
    except Exception as e:
        out.append('%s set err %s' % (lbl, e))
out.append('repairables found=%d' % n)
les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\slow_minigames.txt', 'w', encoding='utf-8').write('\n'.join(out))
