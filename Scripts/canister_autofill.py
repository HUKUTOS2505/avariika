import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
K = unreal.RepairStageKind
out = []
def stage(kind, tag, dur, label):
    s = unreal.RepairStage()
    s.set_editor_property('kind', kind)
    s.set_editor_property('item_tag', unreal.Name(tag) if tag else unreal.Name('None'))
    s.set_editor_property('duration', dur)
    try: s.set_editor_property('label', label)
    except Exception: pass
    return s
for a in eas.get_all_level_actors():
    if a.get_actor_label() == 'Repairable_Generator':
        a.set_editor_property('PrereqStages', [
            stage(K.MINIGAME, '', 0.0, 'Починить корпус'),
            stage(K.AUTO_FILL, 'Fuel', 3.0, 'Залить бензин')])  # канистра: полоска заполняется
        a.modify()
        st = a.get_editor_property('PrereqStages')
        out.append('Generator stages=[%s]' % ' | '.join('%s/%s' % (str(s.get_editor_property('kind')).split('.')[-1].split(':')[0], s.get_editor_property('item_tag')) for s in st))
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('saved')
open(r'D:\unrealEngine\avariika\Saved\canister_autofill.txt','w',encoding='utf-8').write('\n'.join(out))
