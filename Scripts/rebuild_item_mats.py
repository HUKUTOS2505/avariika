# -*- coding: utf-8 -*-
"""Пересобрать материалы 4 предметов БЕЗ metallic (meshy завышает металличность →
крашенный металл/пластик становится зеркалом и белеет). Текстуры уже импортированы —
переиспользуем. Оставляем base color + normal + roughness; metallic=0 (диэлектрик)."""
import unreal

DEST = '/Game/Avariika/Meshes'
tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
out = []


def tex(name):
    p = '%s/%s.%s' % (DEST, name, name)
    return unreal.load_asset(p) if unreal.EditorAssetLibrary.does_asset_exist(p) else None


for folder in ['SM_FireExtinguisher', 'SM_Battery', 'SM_Radio', 'SM_FirstAidKit']:
    mat_obj = '%s/M_%s.M_%s' % (DEST, folder, folder)
    if unreal.EditorAssetLibrary.does_asset_exist(mat_obj):
        unreal.EditorAssetLibrary.delete_asset(mat_obj)
    mat = tools.create_asset('M_' + folder, DEST, unreal.Material, unreal.MaterialFactoryNew())

    tb = tex(folder + '_BaseColor')
    if tb:
        ts = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -400, -200)
        ts.set_editor_property('texture', tb)
        ts.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
        mel.connect_material_property(ts, 'RGB', unreal.MaterialProperty.MP_BASE_COLOR)
    tn = tex(folder + '_Normal')
    if tn:
        ns = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -400, 50)
        ns.set_editor_property('texture', tn)
        ns.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
        mel.connect_material_property(ns, 'RGB', unreal.MaterialProperty.MP_NORMAL)
    tr = tex(folder + '_Roughness')
    if tr:
        rs = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -400, 300)
        rs.set_editor_property('texture', tr)
        rs.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_GRAYSCALE)
        mel.connect_material_property(rs, 'R', unreal.MaterialProperty.MP_ROUGHNESS)
    # metallic НЕ подключаем -> по умолчанию 0 (диэлектрик)

    mel.recompile_material(mat)
    unreal.EditorAssetLibrary.save_loaded_asset(mat)

    mesh = unreal.load_asset('%s/%s.%s' % (DEST, folder, folder))
    if mesh:
        for i in range(max(mesh.get_num_sections(0), 1)):
            mesh.set_material(i, mat)
        unreal.EditorAssetLibrary.save_loaded_asset(mesh)
    out.append('%s rebuilt (base+normal+rough, no metallic)' % folder)

open(r'D:\unrealEngine\avariika\Saved\rebuild_item_mats.txt', 'w', encoding='utf-8').write('\n'.join(out))
