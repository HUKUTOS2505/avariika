# -*- coding: utf-8 -*-
"""Удалить все временные PROD_-акторы и СОХРАНИТЬ уровень (чтобы не попали в коммит)."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
killed = 0
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label().startswith('PROD_'):
        eas.destroy_actor(a); killed += 1
left = [x.get_actor_label() for x in eas.get_all_level_actors() if x.get_actor_label().startswith('PROD_')]
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'D:\unrealEngine\avariika\Saved\cleanup_prod_save.txt', 'w', encoding='utf-8').write('killed=%d left=%s' % (killed, left))
