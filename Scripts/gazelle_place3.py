# -*- coding: utf-8 -*-
"""Минимально: nisan_scene_0 -> позиция ГАЗели, фикс-масштаб, удалить серую заглушку. Лог в варнинги."""
import unreal

def log(m):
    unreal.log_warning('GZ3: ' + m)

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
acts = eas.get_all_level_actors()
root = next((a for a in acts if a.get_actor_label() == 'nisan_scene_0'), None)
log('root=%s' % bool(root))
if root:
    root.set_actor_scale3d(unreal.Vector(100.0, 100.0, 100.0))
    root.set_actor_location(unreal.Vector(-300.0, 0.0, 40.0))
    root.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=0.0, roll=0.0), False)
    log('set scale100 loc(-300,0,40)')
killed = 0
for g in [a for a in acts if a.get_actor_label() == 'Gazelle_Mesh']:
    eas.destroy_actor(g)
    killed += 1
log('deleted gray=%d' % killed)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
log('done')
