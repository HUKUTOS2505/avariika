# -*- coding: utf-8 -*-
"""Переключить этапы на новые режимы + опустить щиток ниже.
Генератор: Minigame(починка руками)->InsertItem(бензин)->Starter.
Труба: Minigame(заварка сваркой)->Valve. Щиток: AutoFill(кабель)->InsertItem(предохранитель)->Cursor."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

K = unreal.RepairStageKind


def stage(kind, item_tag, dur, label):
    s = unreal.RepairStage()
    s.set_editor_property('kind', kind)
    s.set_editor_property('item_tag', unreal.Name(item_tag) if item_tag else unreal.Name('None'))
    s.set_editor_property('duration', dur)
    try:
        s.set_editor_property('label', label)
    except Exception:
        pass
    return s


gen = by.get('Repairable_Generator')
if gen:
    gen.set_editor_property('PrereqStages', [
        stage(K.MINIGAME, '', 0.0, 'Починить корпус'),
        stage(K.INSERT_ITEM, 'Fuel', 0.0, 'Залить бензин')])
    gen.modify(); out.append('Generator: Minigame -> Fuel -> Starter')

pipe = by.get('Repairable_GasPipe')
if pipe:
    pipe.set_editor_property('PrereqStages', [stage(K.MINIGAME, 'Welder', 0.0, 'Заварить трубу')])
    pipe.modify(); out.append('GasPipe: Minigame(weld, Welder) -> Valve')

brk = by.get('Repairable_Breaker')
if brk:
    brk.set_editor_property('PrereqStages', [
        stage(K.AUTO_FILL, 'Cable', 3.0, 'Установить кабель'),
        stage(K.INSERT_ITEM, 'Fuse', 0.0, 'Вставить предохранитель')])
    # опустить ниже — удобнее чинить
    brk.set_actor_location(unreal.Vector(700.0, 650.0, 210.0 + 55.0), False, True)
    brk.modify(); out.append('Breaker: AutoFill(cable) -> Fuse -> Cursor; опущен на z=265')

# проверка
for lbl in ['Repairable_Generator', 'Repairable_GasPipe', 'Repairable_Breaker']:
    a = by.get(lbl)
    if a:
        st = a.get_editor_property('PrereqStages')
        parts = ['%s/%s' % (str(s.get_editor_property('kind')).split('.')[-1].rstrip('>').split(':')[0], s.get_editor_property('item_tag')) for s in st]
        out.append('  %s stages=[%s]' % (lbl, ' | '.join(parts)))

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'C:\unrealEngine\avariika\Saved\config_stage_modes.txt', 'w', encoding='utf-8').write('\n'.join(out))
