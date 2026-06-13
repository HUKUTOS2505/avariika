# -*- coding: utf-8 -*-
"""Импорт новых предметов (Battery, FireExtinguisher, FirstAidKit, Radio) с ПОЛНЫМ PBR
(base color + normal + roughness + metallic) и надёжной привязкой к уровню.

Почему так:
* legacy FBX (Interchange.FeatureFlags.Import.FBX 0) — нет битой нормали на meshy-моделях.
* materials/textures импортёра НЕ трогаем (import_materials/textures=False), строим материал сами
  из png-карт рядом с fbx — иначе meshy кладёт мусорные дубль-текстуры.
* МЕШ нативного компонента APickupItem.MeshComponent НЕ сериализуется через CDO
  (нужен InheritableComponentHandler). Поэтому ставим per-instance override на КАЖДЫЙ
  размещённый инстанс класса (их видно в PIE — подбор аттачит тот же актор) + CDO best-effort.

Гнать ТОЛЬКО через Claudius editor.run_python_script с уже открытым Lvl_FirstPerson.
"""
import os
import unreal

RAW = r'D:\unrealEngine\avariika\RawAssets'
DEST = '/Game/Avariika/Meshes'

# folder -> (target, target max-dim cm)
#   'actor:<label>'   — назначить меш на конкретный актор уровня
#   'bpclass:<path>'  — назначить в CDO блюпринта + per-instance на ВСЕ инстансы этого класса
MAPPING = {
    'SM_Radio':            ('actor:Radio', 20.0),
    'SM_Battery':          ('bpclass:/Game/Avariika/Items/BP_Battery', 18.0),
    'SM_FireExtinguisher': ('bpclass:/Game/Avariika/Items/BP_FireExtinguisher', 55.0),
    'SM_FirstAidKit':      ('bpclass:/Game/Avariika/Items/BP_FirstAidKit', 28.0),
}

out = []
tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
all_actors = eas.get_all_level_actors()
by_label = {a.get_actor_label(): a for a in all_actors}

unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')


def find_fbx(folder):
    for f in sorted(os.listdir(folder)):
        if f.lower().endswith('.fbx'):
            return os.path.join(folder, f)
    return None


def classify_pngs(folder):
    """Вернуть {role: path} по суффиксам meshy."""
    res = {}
    for f in sorted(os.listdir(folder)):
        lf = f.lower()
        if not lf.endswith('.png'):
            continue
        p = os.path.join(folder, f)
        if '_normal' in lf:
            res['normal'] = p
        elif '_roughness' in lf:
            res['roughness'] = p
        elif '_metallic' in lf:
            res['metallic'] = p
        elif '_emission' in lf:
            pass  # meshy emission обычно чёрная — не подключаем
        else:
            res.setdefault('base', p)  # ..._texture.png (без доп.суффикса) -> base color
    return res


def import_one(src, name, save=True):
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
    if src.lower().endswith('.fbx'):
        opt = unreal.FbxImportUI()
        opt.import_mesh = True
        opt.import_as_skeletal = False
        opt.import_materials = False
        opt.import_textures = False
        opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
        t.options = opt
    tools.import_asset_tasks([t])
    return unreal.load_asset(obj)


def import_tex(src, name, srgb, normal=False, mask=False):
    tex = import_one(src, name, save=False)
    if not tex:
        return None
    tex.set_editor_property('srgb', srgb)
    if normal:
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_NORMALMAP)
    elif mask:
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_MASKS)
    unreal.EditorAssetLibrary.save_loaded_asset(tex)
    return tex


def build_material(folder, pngs):
    """Собрать M_<folder> с base/normal/roughness/metallic."""
    base = pngs.get('base')
    if not base:
        return None
    mel = unreal.MaterialEditingLibrary
    mat_obj = DEST + '/M_' + folder + '.M_' + folder
    if unreal.EditorAssetLibrary.does_asset_exist(mat_obj):
        unreal.EditorAssetLibrary.delete_asset(mat_obj)
    mat = tools.create_asset('M_' + folder, DEST, unreal.Material, unreal.MaterialFactoryNew())

    def sampler(tex, y, stype, prop):
        ts = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -380, y)
        ts.set_editor_property('texture', tex)
        ts.set_editor_property('sampler_type', stype)
        mel.connect_material_property(ts, 'RGB', prop)
        return ts

    tb = import_tex(base, folder + '_BaseColor', True)
    if tb:
        sampler(tb, -200, unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, unreal.MaterialProperty.MP_BASE_COLOR)
    if pngs.get('normal'):
        tn = import_tex(pngs['normal'], folder + '_Normal', False, normal=True)
        if tn:
            sampler(tn, 0, unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, unreal.MaterialProperty.MP_NORMAL)
    if pngs.get('roughness'):
        tr = import_tex(pngs['roughness'], folder + '_Roughness', False, mask=True)
        if tr:
            ts = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -380, 200)
            ts.set_editor_property('texture', tr)
            ts.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_GRAYSCALE)
            mel.connect_material_property(ts, 'R', unreal.MaterialProperty.MP_ROUGHNESS)
    if pngs.get('metallic'):
        tm = import_tex(pngs['metallic'], folder + '_Metallic', False, mask=True)
        if tm:
            ts = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -380, 400)
            ts.set_editor_property('texture', tm)
            ts.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_GRAYSCALE)
            mel.connect_material_property(ts, 'R', unreal.MaterialProperty.MP_METALLIC)
    mel.recompile_material(mat)
    unreal.EditorAssetLibrary.save_loaded_asset(mat)
    return mat


def apply_instance(actor, mesh, scale):
    comp = actor.get_editor_property('MeshComponent')
    if comp is None:
        comp = actor.get_component_by_class(unreal.StaticMeshComponent)
    if comp is None:
        return False
    comp.set_static_mesh(mesh)
    if scale and scale > 0:
        actor.set_actor_scale3d(unreal.Vector(scale, scale, scale))
    actor.modify()
    return True


def process(folder, target, max_dim):
    fdir = os.path.join(RAW, folder)
    if not os.path.isdir(fdir):
        out.append('SKIP %s: нет папки' % folder)
        return
    fbx = find_fbx(fdir)
    if not fbx:
        out.append('SKIP %s: нет fbx' % folder)
        return

    mesh = import_one(fbx, folder)
    if not mesh:
        out.append('FAIL %s: меш не импортнулся' % folder)
        return
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)
    maxs = max(size) if max(size) > 0 else 1.0

    try:
        if mesh.get_num_triangles(0) > 20000:
            ns = mesh.get_editor_property('nanite_settings')
            ns.enabled = True
            mesh.set_editor_property('nanite_settings', ns)
    except Exception as e:
        out.append('  nanite? ' + str(e))

    pngs = classify_pngs(fdir)
    mat = build_material(folder, pngs)
    if mat:
        for i in range(max(mesh.get_num_sections(0), 1)):
            mesh.set_material(i, mat)
        unreal.EditorAssetLibrary.save_loaded_asset(mesh)

    scale = (max_dim / maxs) if (max_dim > 0 and maxs > 1.0) else 0.0
    msg = 'OK %s: %.0fx%.0fx%.0f maps=%s mat=%s scale=%.4f' % (
        folder, size[0], size[1], size[2], ','.join(sorted(pngs.keys())), bool(mat), scale)

    kind, _, key = target.partition(':')
    if kind == 'actor':
        a = by_label.get(key)
        if a and apply_instance(a, mesh, scale):
            msg += ' -> actor %s' % key
        else:
            msg += ' (актор %s не найден)' % key
    elif kind == 'bpclass':
        # 1) CDO best-effort
        bp = unreal.EditorAssetLibrary.load_asset(key)
        cls_name = key.split('/')[-1] + '_C'
        if bp:
            try:
                cdo = unreal.get_default_object(bp.generated_class())
                cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
                bp.modify()
                unreal.EditorAssetLibrary.save_loaded_asset(bp)
            except Exception as e:
                msg += ' cdo_err=%s' % e
        # 2) per-instance на ВСЕ инстансы этого класса
        n = 0
        for a in all_actors:
            try:
                if a.get_class().get_name() == cls_name:
                    if apply_instance(a, mesh, scale):
                        n += 1
            except Exception:
                pass
        msg += ' -> CDO + %d инстансов(%s)' % (n, cls_name)
    out.append(msg)


for folder, (target, max_dim) in MAPPING.items():
    process(folder, target, max_dim)

les.save_current_level()
saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('save_dirty=%s' % saved)
open(r'D:\unrealEngine\avariika\Saved\import_items_realistic.txt', 'w', encoding='utf-8').write('\n'.join(out))
