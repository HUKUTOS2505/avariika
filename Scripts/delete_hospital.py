# -*- coding: utf-8 -*-
# Удаляем больничную карту (не подошла, делается заново по новому промту).
# Запуск через Claudius в ОТКРЫТОМ редакторе.
import unreal

out = []
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
# Уйти с L_Hospital, чтобы можно было удалить
les.load_level('/Game/FirstPerson/Lvl_FirstPerson')
out.append('switched to Lvl_FirstPerson')

if unreal.EditorAssetLibrary.does_directory_exist('/Game/Hospital'):
    ok = unreal.EditorAssetLibrary.delete_directory('/Game/Hospital')
    out.append('delete /Game/Hospital = %s' % ok)
else:
    out.append('/Game/Hospital уже нет')

open(r'D:\unrealEngine\avariika\Saved\delete_hospital.txt', 'w', encoding='utf-8').write('\n'.join(out))
