# -*- coding: utf-8 -*-
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
K = unreal.RepairStageKind
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
out.append('repairables: ' + ', '.join(sorted(l for l in by if l.startswith('Repairable_'))))


def stage(kind, tag, dur, label):
    s = unreal.RepairStage()
    s.set_editor_property('kind', kind)
    s.set_editor_property('item_tag', unreal.Name(tag) if tag else unreal.Name('None'))
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
        stage(K.AUTO_FILL, 'Fuel', 3.0, 'Залить бензин')])
    gen.modify()
    st = gen.get_editor_property('PrereqStages')
    out.append('Generator -> [%s]' % ' | '.join('%s/%s' % (str(s.get_editor_property('kind')).split('.')[-1].split(':')[0], s.get_editor_property('item_tag')) for s in st))
else:
    out.append('GENERATOR NOT FOUND')
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'C:\unrealEngine\avariika\Saved\canister_autofill2.txt', 'w', encoding='utf-8').write('\n'.join(out))
