# -*- coding: utf-8 -*-
"""Импорт 3 рантайм-моделей (BioBlob/Floodlight/Trap) нативно в подпапки. Без привязки к
акторам — их меш задаётся в C++-конструкторе (FObjectFinder). Репортим размер для масштаба."""
import os
import unreal
tools = unreal.AssetToolsHelpers.get_asset_tools()
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')
MESHES = '/Game/Avariika/Meshes'
JOBS = ['SM_BioBlob', 'SM_Floodlight', 'SM_Trap']
for folder in JOBS:
    d = os.path.join(r'C:\unrealEngine\avariika\RawAssets', folder)
    fbx = None
    for f in sorted(os.listdir(d)):
        if f.lower().endswith('.fbx'):
            fbx = os.path.join(d, f); break
    if not fbx:
        out.append('%s: нет fbx' % folder); continue
    sub = '%s/%s' % (MESHES, folder)
    obj = '%s/%s.%s' % (sub, folder, folder)
    if unreal.EditorAssetLibrary.does_asset_exist(obj):
        unreal.EditorAssetLibrary.delete_asset(obj)
    t = unreal.AssetImportTask()
    t.filename = fbx; t.destination_path = sub; t.destination_name = folder
    t.automated = True; t.replace_existing = True; t.save = True
    opt = unreal.FbxImportUI()
    opt.import_mesh = True; opt.import_as_skeletal = False
    opt.import_materials = True; opt.import_textures = True
    opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
    t.options = opt
    tools.import_asset_tasks([t])
    m = unreal.load_asset(obj)
    if not m:
        out.append('%s: FAIL' % folder); continue
    b = m.get_bounds().box_extent
    mx = max(b.x * 2, b.y * 2, b.z * 2)
    try:
        if m.get_num_triangles(0) > 20000:
            ns = m.get_editor_property('nanite_settings'); ns.enabled = True
            m.set_editor_property('nanite_settings', ns)
            unreal.EditorAssetLibrary.save_loaded_asset(m)
    except Exception:
        pass
    out.append('%s: maxdim=%.1fсм sect=%d path=%s' % (folder, mx, m.get_num_sections(0), obj))
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'C:\unrealEngine\avariika\Saved\import_deployed_native.txt', 'w', encoding='utf-8').write('\n'.join(out))
