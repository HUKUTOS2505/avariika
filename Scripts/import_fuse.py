# -*- coding: utf-8 -*-
"""Импорт Fuse (meshy, современный) с PBR БЕЗ metallic (как фикс остальных предметов).
Назначение: CDO BP_Fuse + per-instance на все BP_Fuse_C инстансы (Fuse, Loot_Fuse_N)."""
import os
import unreal

RAW = r'D:\unrealEngine\avariika\RawAssets\SM_Fuse'
DEST = '/Game/Avariika/Meshes'
NAME = 'SM_Fuse'
BP = '/Game/Avariika/Items/BP_Fuse'
CLS = 'BP_Fuse_C'
MAX_DIM = 12.0

tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')


def classify(folder):
    res = {}
    for f in sorted(os.listdir(folder)):
        lf = f.lower()
        if not lf.endswith('.png'):
            continue
        p = os.path.join(folder, f)
        if '_metallic' in lf:
            continue            # пропускаем metallic и metallic_roughness
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
    t.filename = src
    t.destination_path = DEST
    t.destination_name = name
    t.automated = True
    t.replace_existing = True
    t.save = save
    if fbx:
        opt = unreal.FbxImportUI()
        opt.import_mesh = True
        opt.import_as_skeletal = False
        opt.import_materials = False
        opt.import_textures = False
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


fbx = None
for f in sorted(os.listdir(RAW)):
    if f.lower().endswith('.fbx'):
        fbx = os.path.join(RAW, f)
        break

mesh = imp(fbx, NAME, save=True, fbx=True)
b = mesh.get_bounds().box_extent
size = (b.x * 2, b.y * 2, b.z * 2)
mx = max(size) if max(size) > 0 else 1.0
out.append('mesh %.1fx%.1fx%.1f' % size)

pngs = classify(RAW)
mat_obj = DEST + '/M_' + NAME + '.M_' + NAME
if unreal.EditorAssetLibrary.does_asset_exist(mat_obj):
    unreal.EditorAssetLibrary.delete_asset(mat_obj)
mat = tools.create_asset('M_' + NAME, DEST, unreal.Material, unreal.MaterialFactoryNew())
tb = imptex(pngs['base'], NAME + '_BaseColor', True) if pngs.get('base') else None
if tb:
    ts = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -400, -200)
    ts.set_editor_property('texture', tb)
    ts.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
    mel.connect_material_property(ts, 'RGB', unreal.MaterialProperty.MP_BASE_COLOR)
if pngs.get('normal'):
    tn = imptex(pngs['normal'], NAME + '_Normal', False, normal=True)
    if tn:
        ns = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -400, 50)
        ns.set_editor_property('texture', tn)
        ns.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
        mel.connect_material_property(ns, 'RGB', unreal.MaterialProperty.MP_NORMAL)
if pngs.get('roughness'):
    tr = imptex(pngs['roughness'], NAME + '_Roughness', False, mask=True)
    if tr:
        rs = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -400, 300)
        rs.set_editor_property('texture', tr)
        rs.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_GRAYSCALE)
        mel.connect_material_property(rs, 'R', unreal.MaterialProperty.MP_ROUGHNESS)
mel.recompile_material(mat)
unreal.EditorAssetLibrary.save_loaded_asset(mat)
for i in range(max(mesh.get_num_sections(0), 1)):
    mesh.set_material(i, mat)
unreal.EditorAssetLibrary.save_loaded_asset(mesh)
out.append('material maps=%s' % ','.join(sorted(pngs.keys())))

scale = (MAX_DIM / mx) if mx > 1.0 else 1.0
# CDO
bp = unreal.EditorAssetLibrary.load_asset(BP)
if bp:
    try:
        cdo = unreal.get_default_object(bp.generated_class())
        cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
        bp.modify()
        unreal.EditorAssetLibrary.save_loaded_asset(bp)
    except Exception as e:
        out.append('cdo err %s' % e)
# per-instance
n = 0
for a in eas.get_all_level_actors():
    try:
        if a.get_class().get_name() == CLS:
            comp = a.get_editor_property('MeshComponent') or a.get_component_by_class(unreal.StaticMeshComponent)
            comp.set_static_mesh(mesh)
            a.set_actor_scale3d(unreal.Vector(scale, scale, scale))
            a.modify()
            n += 1
    except Exception:
        pass
out.append('scale=%.3f instances=%d' % (scale, n))

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\import_fuse.txt', 'w', encoding='utf-8').write('\n'.join(out))
