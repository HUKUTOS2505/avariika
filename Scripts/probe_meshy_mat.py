# -*- coding: utf-8 -*-
"""Импорт огнетушителя С РОДНЫМИ материалами+текстурами meshy в тест-папку и инспекция:
какие карты (base/normal/metallic/roughness/specular) meshy реально подключает."""
import os
import unreal
mel = unreal.MaterialEditingLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

FBX = None
RAW = r'C:\unrealEngine\avariika\RawAssets\SM_FireExtinguisher'
for f in sorted(os.listdir(RAW)):
    if f.lower().endswith('.fbx'):
        FBX = os.path.join(RAW, f); break

DEST = '/Game/Avariika/Meshes/_test'
if unreal.EditorAssetLibrary.does_directory_exist(DEST):
    unreal.EditorAssetLibrary.delete_directory(DEST)
t = unreal.AssetImportTask()
t.filename = FBX; t.destination_path = DEST; t.destination_name = 'SM_FE_meshy'
t.automated = True; t.replace_existing = True; t.save = True
opt = unreal.FbxImportUI()
opt.import_mesh = True; opt.import_as_skeletal = False
opt.import_materials = True; opt.import_textures = True
opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
t.options = opt
tools.import_asset_tasks([t])

mesh = unreal.load_asset(DEST + '/SM_FE_meshy.SM_FE_meshy')
if not mesh:
    out.append('FAIL import')
else:
    out.append('mesh sections=%d' % mesh.get_num_sections(0))
    m = mesh.get_material(0)
    out.append('mat0=%s' % (m.get_name() if m else 'None'))
    if m:
        props = [('BaseColor', unreal.MaterialProperty.MP_BASE_COLOR),
                 ('Metallic', unreal.MaterialProperty.MP_METALLIC),
                 ('Roughness', unreal.MaterialProperty.MP_ROUGHNESS),
                 ('Normal', unreal.MaterialProperty.MP_NORMAL),
                 ('Specular', unreal.MaterialProperty.MP_SPECULAR)]
        for nm, p in props:
            try:
                node = mel.get_material_property_input_node(m, p)
            except Exception as e:
                node = 'ERR'
            if node is None or node == 'ERR':
                out.append('  %-10s: -' % nm)
            elif isinstance(node, unreal.MaterialExpressionTextureSample):
                tx = node.get_editor_property('texture')
                out.append('  %-10s: TEX %s' % (nm, tx.get_name() if tx else '?'))
            else:
                out.append('  %-10s: %s' % (nm, node.get_class().get_name()))

# что за ассеты создались
out.append('--- assets in _test ---')
for a in unreal.EditorAssetLibrary.list_assets(DEST, recursive=False):
    out.append('  ' + a.split('/')[-1])

open(r'C:\unrealEngine\avariika\Saved\probe_meshy_mat.txt', 'w', encoding='utf-8').write('\n'.join(out))
