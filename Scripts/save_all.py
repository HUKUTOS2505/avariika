# -*- coding: utf-8 -*-
import unreal
try:
    unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
except Exception:
    pass
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'D:\unrealEngine\avariika\Saved\save_all.txt', 'w').write('saved')
