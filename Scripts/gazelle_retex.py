# -*- coding: utf-8 -*-
"""Переимпорт SM_Gazelle из имеющегося .fbx С материалами/текстурами (вдруг вшиты).
Если вшитых текстур нет — материалов с текстурами не будет, тогда нужен .glb. ASCII-вывод."""
import os
import unreal

RAW = r'D:\unrealEngine\avariika\RawAssets\SM_Gazelle'
DEST = '/Game/Avariika/Meshes'
out = []

src = None
for f in sorted(os.listdir(RAW)):
    if f.lower().endswith(('.glb', '.gltf')):  # предпочесть glb/gltf, если пользователь докачал
        src = os.path.join(RAW, f); break
if not src:
    for f in sorted(os.listdir(RAW)):
        if f.lower().endswith('.fbx'):
            src = os.path.join(RAW, f); break
out.append('source = ' + str(src))

if src:
    if src.lower().endswith(('.glb', '.gltf')):
        unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 1')
    else:
        unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')
    obj = DEST + '/SM_Gazelle.SM_Gazelle'
    if unreal.EditorAssetLibrary.does_asset_exist(obj):
        unreal.EditorAssetLibrary.delete_asset(obj)
    before = set(unreal.EditorAssetLibrary.list_assets(DEST, recursive=False))
    t = unreal.AssetImportTask()
    t.filename = src
    t.destination_path = DEST
    t.destination_name = 'SM_Gazelle'
    t.automated = True
    t.replace_existing = True
    t.save = True
    if src.lower().endswith('.fbx'):
        opt = unreal.FbxImportUI()
        opt.import_mesh = True
        opt.import_as_skeletal = False
        opt.import_materials = True
        opt.import_textures = True
        opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
        t.options = opt
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
    mesh = unreal.load_asset(obj)
    if mesh:
        try:
            if mesh.get_num_triangles(0) > 20000:
                unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).set_nanite_enabled(mesh, True)
        except Exception as e:
            out.append('nanite? ' + str(e))
        n = mesh.get_num_sections(0)
        out.append('sections = %d' % n)
        tex_count = 0
        for i in range(n):
            m = mesh.get_material(i)
            out.append('  mat[%d] = %s' % (i, m.get_name() if m else 'NONE'))
        after = set(unreal.EditorAssetLibrary.list_assets(DEST, recursive=False))
        newassets = sorted(after - before)
        for a in newassets:
            asset = unreal.load_asset(a.split('.')[0])
            if isinstance(asset, unreal.Texture2D):
                tex_count += 1
        out.append('new assets = %d, of them Texture2D = %d' % (len(newassets), tex_count))
        out.append('NEW: ' + ', '.join(x.split('/')[-1].split('.')[0] for x in newassets[:25]))
    else:
        out.append('mesh import FAILED')
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'D:\unrealEngine\avariika\Saved\gazelle_retex.txt', 'w', encoding='utf-8').write('\n'.join(out))
