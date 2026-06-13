# -*- coding: utf-8 -*-
"""Пакетный импорт meshy-моделей (Thermos/LightKit/MotionSensor/TrapKit) без metallic,
назначение на актор-заглушку + масштаб под реальный габарит."""
import os
import unreal

RAWROOT = r'D:\unrealEngine\avariika\RawAssets'
DEST = '/Game/Avariika/Meshes'
tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

# folder, mesh-name, actor label, bp-path(or None), bp-class(or None), target max-dim cm
CONFIGS = [
    ('SM_Thermos', 'SM_Thermos', 'Thermos', None, None, 25.0),
    ('SM_LightKit', 'SM_LightKit', 'LightKit', None, None, 28.0),
    ('SM_TrapKit', 'SM_TrapKit', 'TrapKit', None, None, 18.0),
    ('SM_MotionSensor', 'SM_MotionSensor', 'MotionSensor',
     '/Game/Avariika/Items/BP_MotionSensor', 'BP_MotionSensor_C', 16.0),
]


def classify(folder):
    res = {}
    for f in sorted(os.listdir(folder)):
        lf = f.lower()
        if not lf.endswith('.png'):
            continue
        p = os.path.join(folder, f)
        if '_metallic' in lf:
            continue
        elif '_roughness' in lf:
            res['roughness'] = p
        elif '_normal' in lf:
            res['normal'] = p
        elif '_emit' in lf or '_emission' in lf:
            continue
        else:
            res.setdefault('base', p)
    return res


def imp(src, name, save=False, fbx=False):
    obj = DEST + '/' + name + '.' + name
    if unreal.EditorAssetLibrary.does_asset_exist(obj):
        unreal.EditorAssetLibrary.delete_asset(obj)
    t = unreal.AssetImportTask()
    t.filename = src; t.destination_path = DEST; t.destination_name = name
    t.automated = True; t.replace_existing = True; t.save = save
    if fbx:
        opt = unreal.FbxImportUI()
        opt.import_mesh = True; opt.import_as_skeletal = False
        opt.import_materials = False; opt.import_textures = False
        opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
        t.options = opt
    tools.import_asset_tasks([t])
    return unreal.load_asset(obj)


def imptex(src, name, srgb, normal=False, mask=False):
    tex = imp(src, name)
    if not tex:
        return None
    tex.set_editor_property('srgb', srgb)
    if normal:
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_NORMALMAP)
    elif mask:
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_MASKS)
    unreal.EditorAssetLibrary.save_loaded_asset(tex)
    return tex


def build_mat(name, pngs):
    mo = DEST + '/M_' + name + '.M_' + name
    if unreal.EditorAssetLibrary.does_asset_exist(mo):
        unreal.EditorAssetLibrary.delete_asset(mo)
    m = tools.create_asset('M_' + name, DEST, unreal.Material, unreal.MaterialFactoryNew())
    tb = imptex(pngs['base'], name + '_BaseColor', True) if pngs.get('base') else None
    if tb:
        ts = mel.create_material_expression(m, unreal.MaterialExpressionTextureSample, -400, -200)
        ts.set_editor_property('texture', tb)
        ts.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
        mel.connect_material_property(ts, 'RGB', unreal.MaterialProperty.MP_BASE_COLOR)
    if pngs.get('normal'):
        tn = imptex(pngs['normal'], name + '_Normal', False, normal=True)
        if tn:
            ns = mel.create_material_expression(m, unreal.MaterialExpressionTextureSample, -400, 50)
            ns.set_editor_property('texture', tn)
            ns.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
            mel.connect_material_property(ns, 'RGB', unreal.MaterialProperty.MP_NORMAL)
    if pngs.get('roughness'):
        tr = imptex(pngs['roughness'], name + '_Roughness', False, mask=True)
        if tr:
            rs = mel.create_material_expression(m, unreal.MaterialExpressionTextureSample, -400, 300)
            rs.set_editor_property('texture', tr)
            rs.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_GRAYSCALE)
            mel.connect_material_property(rs, 'R', unreal.MaterialProperty.MP_ROUGHNESS)
    mel.recompile_material(m)
    unreal.EditorAssetLibrary.save_loaded_asset(m)
    return m


def comp_of(a):
    c = a.get_editor_property('MeshComponent')
    return c if c else a.get_component_by_class(unreal.StaticMeshComponent)


by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
for folder, name, label, bp, cls, target in CONFIGS:
    fdir = os.path.join(RAWROOT, folder)
    fbx = None
    for f in sorted(os.listdir(fdir)):
        if f.lower().endswith('.fbx'):
            fbx = os.path.join(fdir, f); break
    if not fbx:
        out.append('%s: нет fbx' % folder); continue
    mesh = imp(fbx, name, save=True, fbx=True)
    if not mesh:
        out.append('%s: FAIL import' % folder); continue
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)
    mx = max(size) if max(size) > 0 else 1.0
    mat = build_mat(name, classify(fdir))
    if mat:
        for i in range(max(mesh.get_num_sections(0), 1)):
            mesh.set_material(i, mat)
        unreal.EditorAssetLibrary.save_loaded_asset(mesh)
    scale = target / mx
    msg = '%s: %.0fx%.0fx%.0f -> %.0fсм (scale %.3f)' % (name, size[0], size[1], size[2], target, scale)
    # назначение
    if cls and bp:
        b_ = unreal.EditorAssetLibrary.load_asset(bp)
        if b_:
            try:
                cdo = unreal.get_default_object(b_.generated_class())
                cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
                b_.modify(); unreal.EditorAssetLibrary.save_loaded_asset(b_)
            except Exception as e:
                msg += ' cdo_err=%s' % e
        n = 0
        for a in eas.get_all_level_actors():
            if a.get_class().get_name() == cls:
                c = comp_of(a)
                if c:
                    c.set_static_mesh(mesh); a.set_actor_scale3d(unreal.Vector(scale, scale, scale)); a.modify(); n += 1
        msg += ' -> CDO + %d инст.' % n
    else:
        a = by.get(label)
        if a:
            c = comp_of(a)
            c.set_static_mesh(mesh); a.set_actor_scale3d(unreal.Vector(scale, scale, scale)); a.modify()
            msg += ' -> actor %s' % label
        else:
            msg += ' (актор %s не найден)' % label
    out.append(msg)

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\import_meshy_batch.txt', 'w', encoding='utf-8').write('\n'.join(out))
