# -*- coding: utf-8 -*-
"""Build a safe base-color material for the toilet and assign it to SM_Toilet.
Avoids the meshy normal map entirely (it crashes texture build/render). Uses the
already-imported base color Image_0. Headless-safe: creates a material asset and
assigns it, no FBX import (Slate sync) and no rendering.
Run: UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="...build_toilet_material.py"
"""
import unreal

out = []
DEST = '/Game/Avariika/Meshes'
base = unreal.load_asset(DEST + '/Image_0')  # base color из первого импорта
mesh = unreal.load_asset(DEST + '/SM_Toilet')

if not base:
    out.append('нет Image_0 (base color) — нечего класть в материал')
elif not mesh:
    out.append('нет SM_Toilet')
else:
    base.set_editor_property('srgb', True)  # base color = sRGB
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    mat_name = 'M_Toilet'
    mat_obj = DEST + '/' + mat_name + '.' + mat_name
    if unreal.EditorAssetLibrary.does_asset_exist(mat_obj):
        unreal.EditorAssetLibrary.delete_asset(mat_obj)
    mat = tools.create_asset(mat_name, DEST, unreal.Material, unreal.MaterialFactoryNew())

    mel = unreal.MaterialEditingLibrary
    ts = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -350, 0)
    ts.set_editor_property('texture', base)
    mel.connect_material_property(ts, 'RGB', unreal.MaterialProperty.MP_BASE_COLOR)

    # Керамика: глянцевее дефолта
    rough = mel.create_material_expression(mat, unreal.MaterialExpressionConstant, -350, 250)
    rough.set_editor_property('r', 0.25)
    mel.connect_material_property(rough, '', unreal.MaterialProperty.MP_ROUGHNESS)

    mel.recompile_material(mat)
    unreal.EditorAssetLibrary.save_loaded_asset(mat)

    n = max(mesh.get_num_sections(0), 1)
    for i in range(n):
        mesh.set_material(i, mat)
    unreal.EditorAssetLibrary.save_loaded_asset(mesh)
    out.append('M_Toilet создан (base color Image_0, roughness 0.25), назначен на SM_Toilet x%d' % n)

with open(r'D:\unrealEngine\avariika\Saved\build_mat_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
