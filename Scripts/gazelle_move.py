# -*- coding: utf-8 -*-
"""Сделать грузовик (nisan_scene_0 + потомки) Movable и сдвинуть на место ГАЗели; удалить серую."""
import unreal

def log(m):
    unreal.log_warning('GZMV: ' + m)

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
acts = eas.get_all_level_actors()
root = next((a for a in acts if a.get_actor_label() == 'nisan_scene_0'), None)
log('root=%s' % bool(root))

if root:
    def walk(a, fn):
        fn(a)
        for c in a.get_attached_actors():
            walk(c, fn)
    def mkmov(a):
        try:
            for c in a.get_components_by_class(unreal.SceneComponent):
                c.set_mobility(unreal.ComponentMobility.MOVABLE)
        except Exception:
            pass
    walk(root, mkmov)
    log('mobility movable set')
    try:
        root.set_actor_location(unreal.Vector(-300.0, 0.0, 40.0), False, True)
    except Exception as e:
        log('move exc: ' + str(e))
    l = root.get_actor_location()
    log('after move loc=(%.0f,%.0f,%.0f) scale=%.0f' % (l.x, l.y, l.z, root.get_actor_scale3d().x))

killed = 0
for g in [a for a in acts if a.get_actor_label() == 'Gazelle_Mesh']:
    eas.destroy_actor(g)
    killed += 1
log('deleted gray=%d' % killed)

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
log('done')
