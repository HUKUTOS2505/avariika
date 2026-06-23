# -*- coding: utf-8 -*-
"""Meshy-safe import: FBX MESH ONLY (no auto-material, no embedded textures ->
no broken meshy normal that crashes UE), import base_color PNG, build a clean
material (base color only), assign mesh+material to the target actor in the
OPEN level, scale, OFPA-safe save.

Run from the OPEN editor (Claudius editor.run_python_script). Do NOT call
load_level here (in-editor it spawns a detached world that never saves).
"""
import os
import unreal

RAW = r'C:\unrealEngine\avariika\RawAssets'
DEST = '/Game/Avariika/Meshes'

# folder -> (actor label, target max-dim cm)
MAPPING = {
    'SM_Breaker': ('Repairable_Breaker', 160.0),
}

out = []
tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}


def find_file(folder, exts):
    for ext in exts:
        for f in sorted(os.listdir(folder)):
            if f.lower().endswith(ext):
                return os.path.join(folder, f)
    return None


def import_one(name, src, is_fbx):
    obj = DEST + '/' + name + '.' + name
    if unreal.EditorAssetLibrary.does_asset_exist(obj):
        unreal.EditorAssetLibrary.delete_asset(obj)
    task = unreal.AssetImportTask()
    task.filename = src
    task.destination_path = DEST
    task.destination_name = name
    task.automated = True
    task.replace_existing = True
    task.save = True
    if is_fbx:
        # Полный импорт даёт геометрию (mesh-only под Interchange ломал её в 0x0x0).
        # Авто-материал с битой нормалью потом перекрываем чистым M_<name>.
        opt = unreal.FbxImportUI()
        opt.import_mesh = True
        opt.import_as_skeletal = False
        opt.import_materials = True
        opt.import_textures = True
        opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
        task.options = opt
    tools.import_asset_tasks([task])
    return unreal.load_asset(obj)


for folder, (actor_label, max_dim) in MAPPING.items():
    fdir = os.path.join(RAW, folder)
    if not os.path.isdir(fdir):
        out.append('SKIP %s: нет папки' % folder)
        continue
    fbx = find_file(fdir, ('.fbx',))
    if not fbx:
        out.append('SKIP %s: нет fbx' % folder)
        continue

    mesh = import_one(folder, fbx, is_fbx=True)
    if not mesh:
        out.append('FAIL %s: меш не импортнулся' % folder)
        continue
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)

    # Nanite для тяжёлых
    try:
        if mesh.get_num_triangles(0) > 20000:
            unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).set_nanite_enabled(mesh, True)
    except Exception:
        pass

    # base color -> material
    png = find_file(fdir, ('_base_color.png', '.png'))
    mat = None
    if png:
        tex_name = folder + '_BaseColor'
        tex = import_one(tex_name, png, is_fbx=False)
        if tex:
            tex.set_editor_property('srgb', True)
            unreal.EditorAssetLibrary.save_loaded_asset(tex)
            mat_name = 'M_' + folder
            mat_obj = DEST + '/' + mat_name + '.' + mat_name
            if unreal.EditorAssetLibrary.does_asset_exist(mat_obj):
                unreal.EditorAssetLibrary.delete_asset(mat_obj)
            mat = tools.create_asset(mat_name, DEST, unreal.Material, unreal.MaterialFactoryNew())
            mel = unreal.MaterialEditingLibrary
            ts = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -350, 0)
            ts.set_editor_property('texture', tex)
            mel.connect_material_property(ts, 'RGB', unreal.MaterialProperty.MP_BASE_COLOR)
            mel.recompile_material(mat)
            unreal.EditorAssetLibrary.save_loaded_asset(mat)
    if mat:
        for i in range(max(mesh.get_num_sections(0), 1)):
            mesh.set_material(i, mat)
        unreal.EditorAssetLibrary.save_loaded_asset(mesh)

    out.append('OK %s: %.0fx%.0fx%.0f, mat=%s' % (folder, size[0], size[1], size[2], bool(mat)))

    # assign to actor in OPEN level
    actor = by_label.get(actor_label)
    if not actor:
        out.append('  WARN: актор %s не найден' % actor_label)
        continue
    actor.get_editor_property('MeshComponent').set_static_mesh(mesh)
    if max_dim > 0 and max(size) > 1.0:
        s = max_dim / max(size)
        actor.set_actor_scale3d(unreal.Vector(s, s, s))
        out.append('  -> %s, scale %.2f' % (actor_label, s))

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('save_dirty_packages=%s' % saved)

with open(r'C:\unrealEngine\avariika\Saved\meshy_import_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
