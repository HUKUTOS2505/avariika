# -*- coding: utf-8 -*-
"""Импорт тестера (SM_Tester) — БЕЗ битой нормали.

Отключаем Interchange-FBX (cvar) → работает legacy-импортёр, который уважает
import_textures=False → меш без авто-текстур и без проблемной meshy-нормали.
Затем чистый материал из base color, назначение на актор Tester в ОТКРЫТОМ уровне.

Запуск через Claudius в открытом редакторе (FBX-импорт требует Slate).
"""
import os
import unreal

RAW = r'C:\unrealEngine\avariika\RawAssets\SM_Tester'
DEST = '/Game/Avariika/Meshes'
NAME = 'SM_Tester'
ACTOR = 'Tester'
MAX_DIM = 22.0

out = []
tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)


def find(exts):
    for ext in exts:
        for f in sorted(os.listdir(RAW)):
            if f.lower().endswith(ext):
                return os.path.join(RAW, f)
    return None


# Снимок ассетов ДО импорта — чтобы поймать любые авто-орфаны и снести.
before = set(unreal.EditorAssetLibrary.list_assets(DEST, recursive=False))

# Отключаем Interchange для FBX → legacy importer уважает FbxImportUI флаги.
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

fbx = find(('.fbx',))
mesh_obj = DEST + '/' + NAME + '.' + NAME
if unreal.EditorAssetLibrary.does_asset_exist(mesh_obj):
    unreal.EditorAssetLibrary.delete_asset(mesh_obj)

task = unreal.AssetImportTask()
task.filename = fbx
task.destination_path = DEST
task.destination_name = NAME
task.automated = True
task.replace_existing = True
task.save = True
opt = unreal.FbxImportUI()
opt.import_mesh = True
opt.import_as_skeletal = False
opt.import_materials = False
opt.import_textures = False
opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
task.options = opt
tools.import_asset_tasks([task])

mesh = unreal.load_asset(mesh_obj)
if not mesh:
    out.append('FAIL: меш не импортнулся')
else:
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)
    out.append('mesh %.1fx%.1fx%.1f tris=%d' % (size[0], size[1], size[2], mesh.get_num_triangles(0)))

    # base color -> чистый материал
    png = find(('_base_color.png', '.png'))
    mat = None
    if png:
        tex_obj = DEST + '/' + NAME + '_BaseColor.' + NAME + '_BaseColor'
        if unreal.EditorAssetLibrary.does_asset_exist(tex_obj):
            unreal.EditorAssetLibrary.delete_asset(tex_obj)
        t = unreal.AssetImportTask()
        t.filename = png
        t.destination_path = DEST
        t.destination_name = NAME + '_BaseColor'
        t.automated = True
        t.replace_existing = True
        t.save = True
        tools.import_asset_tasks([t])
        tex = unreal.load_asset(tex_obj)
        if tex:
            tex.set_editor_property('srgb', True)
            unreal.EditorAssetLibrary.save_loaded_asset(tex)
            mat_obj = DEST + '/M_' + NAME + '.M_' + NAME
            if unreal.EditorAssetLibrary.does_asset_exist(mat_obj):
                unreal.EditorAssetLibrary.delete_asset(mat_obj)
            mat = tools.create_asset('M_' + NAME, DEST, unreal.Material, unreal.MaterialFactoryNew())
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
    out.append('material=%s' % bool(mat))

    # Назначить на актор Tester (PickupItem) в открытом уровне
    by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
    actor = by_label.get(ACTOR)
    if actor:
        actor.get_editor_property('MeshComponent').set_static_mesh(mesh)
        m = max(size)
        if m > 1.0:
            s = MAX_DIM / m
            actor.set_actor_scale3d(unreal.Vector(s, s, s))
            out.append('-> %s scale %.3f' % (ACTOR, s))
    else:
        out.append('WARN: актор %s не найден' % ACTOR)

# Снести любые авто-орфаны, появившиеся при импорте (normal/Image_*/Material_* и т.п.)
after = set(unreal.EditorAssetLibrary.list_assets(DEST, recursive=False))
keep = {mesh_obj.split('.')[0], DEST + '/' + NAME + '_BaseColor', DEST + '/M_' + NAME}
killed = []
for a in after - before:
    base = a.split('.')[0]
    if base not in keep:
        unreal.EditorAssetLibrary.delete_asset(base)
        killed.append(base)
out.append('орфанов снесено: %d %s' % (len(killed), killed))

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('сохранено')

open(r'C:\unrealEngine\avariika\Saved\import_tester.txt', 'w', encoding='utf-8').write('\n'.join(out))
