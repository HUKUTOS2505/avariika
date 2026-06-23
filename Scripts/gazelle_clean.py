# -*- coding: utf-8 -*-
"""Откат неудачного materials=True импорта ГАЗели: переимпорт чистым (1 секция,
дефолт-материал) + удаление 107 мусорных материалов (Paint_/Aluminum_/...). ASCII."""
import os
import unreal

RAW = r'C:\unrealEngine\avariika\RawAssets\SM_Gazelle'
DEST = '/Game/Avariika/Meshes'
out = []

fbx = None
for f in sorted(os.listdir(RAW)):
    if f.lower().endswith('.fbx'):
        fbx = os.path.join(RAW, f); break

unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')
obj = DEST + '/SM_Gazelle.SM_Gazelle'
if unreal.EditorAssetLibrary.does_asset_exist(obj):
    unreal.EditorAssetLibrary.delete_asset(obj)
t = unreal.AssetImportTask()
t.filename = fbx
t.destination_path = DEST
t.destination_name = 'SM_Gazelle'
t.automated = True
t.replace_existing = True
t.save = True
opt = unreal.FbxImportUI()
opt.import_mesh = True
opt.import_as_skeletal = False
opt.import_materials = False
opt.import_textures = False
opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
t.options = opt
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
mesh = unreal.load_asset(obj)
out.append('reimport clean: ' + ('ok' if mesh else 'FAIL'))

junk_pref = ('Paint_', 'Aluminum_', 'Emissive_', 'Matte', 'Anodized_', 'Solar_', 'faro')
deleted = 0
for a in list(unreal.EditorAssetLibrary.list_assets(DEST, recursive=False)):
    nm = a.split('.')[0].split('/')[-1]
    if nm.startswith(junk_pref):
        try:
            unreal.EditorAssetLibrary.delete_asset(a.split('.')[0]); deleted += 1
        except Exception as e:
            out.append('del? ' + nm + ' ' + str(e))
out.append('deleted junk materials = %d' % deleted)
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'C:\unrealEngine\avariika\Saved\gazelle_clean.txt', 'w', encoding='utf-8').write('\n'.join(out))
