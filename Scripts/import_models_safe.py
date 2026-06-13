# -*- coding: utf-8 -*-
"""Безопасный импорт meshy-моделей (legacy FBX, без Interchange → без битой нормали).

Импортирует ИЗ RawAssets все модели из MAPPING, у которых уже есть .fbx и которых
ещё нет в /Game/Avariika/Meshes. Строит чистый материал из base color, Nanite для
тяжёлых, чистит авто-орфаны, при наличии метки — назначает меш на актора уровня.

ВАЖНО: FBX-импорт требует ОТКРЫТОГО редактора (Slate). Headless падает на
assert CurrentApplication.IsValid() (проверено — даже legacy-импортёр без Interchange).
Гнать ТОЛЬКО через Claudius editor.run_python_script с уже открытым Lvl_FirstPerson
(load_level в открытом редакторе плодит detached-мир — поэтому тут НЕ грузим уровень).
"""
import os
import unreal

RAW = r'D:\unrealEngine\avariika\RawAssets'
DEST = '/Game/Avariika/Meshes'

# folder -> (target, target max-dim cm). target:
#   'actor:<label>'  — назначить меш на актора уровня (+ масштаб под max-dim)
#   'spawn:<label>'  — декор: найти/создать StaticMeshActor у ExitZone_Gazelle
#   'bp:<path>'      — назначить меш в CDO компонента MeshComponent блюпринта-предмета
#   None             — только импорт, без привязки
MAPPING = {
    'SM_Toilet':          ('actor:Toilet', 75.0),
    'SM_Breaker':         ('actor:Repairable_Breaker', 160.0),
    'SM_Tester':          ('actor:Tester', 22.0),
    'SM_GasPipe':         ('actor:Repairable_GasPipe', 220.0),
    'SM_Generator':       ('actor:Repairable_Generator', 200.0),
    'SM_Radio':           ('actor:Radio', 20.0),
    'SM_Gazelle':         ('spawn:Gazelle_Mesh', 480.0),
    'SM_WeldingMachine':  ('bp:/Game/Avariika/Items/BP_WeldingMachine', 0.0),
    'SM_FireExtinguisher':('bp:/Game/Avariika/Items/BP_FireExtinguisher', 0.0),
    'SM_FirstAidKit':     ('bp:/Game/Avariika/Items/BP_FirstAidKit', 0.0),
    'SM_Battery':         ('bp:/Game/Avariika/Items/BP_Battery', 0.0),
    'SM_Cigarettes':      ('bp:/Game/Avariika/Items/BP_Cigarettes', 0.0),
    'SM_Fuse':            ('bp:/Game/Avariika/Items/BP_Fuse', 0.0),
}

out = []
tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
# Уровень уже открыт в редакторе (load_level тут плодил бы detached-мир).
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

# legacy FBX (Interchange уважает import_textures=False → нет битой нормали)
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')


def find(folder, exts):
    for ext in exts:
        for f in sorted(os.listdir(folder)):
            if f.lower().endswith(ext):
                return os.path.join(folder, f)
    return None


def do_task(src, name):
    obj = DEST + '/' + name + '.' + name
    if unreal.EditorAssetLibrary.does_asset_exist(obj):
        unreal.EditorAssetLibrary.delete_asset(obj)
    t = unreal.AssetImportTask()
    t.filename = src
    t.destination_path = DEST
    t.destination_name = name
    t.automated = True
    t.replace_existing = True
    t.save = True
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


def import_model(folder, target, max_dim):
    fdir = os.path.join(RAW, folder)
    if not os.path.isdir(fdir):
        return None
    fbx = find(fdir, ('.fbx',))
    if not fbx:
        out.append('SKIP %s: нет fbx' % folder)
        return None
    if unreal.EditorAssetLibrary.does_asset_exist(DEST + '/' + folder + '.' + folder):
        out.append('SKIP %s: уже импортирован' % folder)
        return None

    before = set(unreal.EditorAssetLibrary.list_assets(DEST, recursive=False))
    mesh = do_task(fbx, folder)
    if not mesh:
        out.append('FAIL %s: меш не импортнулся' % folder)
        return None
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)

    try:
        if mesh.get_num_triangles(0) > 20000:
            ns = mesh.get_editor_property('nanite_settings')
            ns.enabled = True
            mesh.set_editor_property('nanite_settings', ns)
    except Exception as e:
        out.append('  nanite? ' + str(e))

    png = find(fdir, ('_base_color.png', '.png'))
    mat = None
    if png:
        tex = do_task(png, folder + '_BaseColor')
        if tex:
            tex.set_editor_property('srgb', True)
            unreal.EditorAssetLibrary.save_loaded_asset(tex)
            mat_obj = DEST + '/M_' + folder + '.M_' + folder
            if unreal.EditorAssetLibrary.does_asset_exist(mat_obj):
                unreal.EditorAssetLibrary.delete_asset(mat_obj)
            mat = tools.create_asset('M_' + folder, DEST, unreal.Material, unreal.MaterialFactoryNew())
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

    # снести авто-орфаны
    keep = {DEST + '/' + folder, DEST + '/' + folder + '_BaseColor', DEST + '/M_' + folder}
    after = set(unreal.EditorAssetLibrary.list_assets(DEST, recursive=False))
    killed = 0
    for a in after - before:
        base = a.split('.')[0]
        if base not in keep:
            unreal.EditorAssetLibrary.delete_asset(base)
            killed += 1

    msg = 'OK %s: %.0fx%.0fx%.0f mat=%s orphans_killed=%d' % (folder, size[0], size[1], size[2], bool(mat), killed)

    kind, _, key = (target or '').partition(':')
    if kind == 'actor':
        actor = by_label.get(key)
        if actor:
            actor.get_editor_property('MeshComponent').set_static_mesh(mesh)
            if max_dim > 0 and max(size) > 1.0:
                s = max_dim / max(size)
                actor.set_actor_scale3d(unreal.Vector(s, s, s))
                msg += ' -> %s scale %.3f' % (key, s)
        else:
            msg += ' (актор %s не найден)' % key
    elif kind == 'spawn':
        actor = by_label.get(key)
        if not actor:
            zone = by_label.get('ExitZone_Gazelle')
            loc = zone.get_actor_location() if zone else unreal.Vector(-250.0, 0.0, 300.0)
            actor = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(loc.x, loc.y, 210.0), unreal.Rotator(0.0, 90.0, 0.0))
            actor.set_actor_label(key)
            by_label[key] = actor
        actor.static_mesh_component.set_editor_property('static_mesh', mesh)
        if max_dim > 0 and max(size) > 1.0:
            s = max_dim / max(size)
            actor.set_actor_scale3d(unreal.Vector(s, s, s))
        msg += ' -> декор %s' % key
    elif kind == 'bp':
        bp = unreal.EditorAssetLibrary.load_asset(key)
        if bp:
            cdo = unreal.get_default_object(bp.generated_class())
            cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
            msg += ' -> CDO %s' % key.split('/')[-1]
        else:
            msg += ' (BP %s не найден)' % key
    out.append(msg)
    return mesh


for folder, (target, max_dim) in MAPPING.items():
    import_model(folder, target, max_dim)

les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('сохранено')

open(r'D:\unrealEngine\avariika\Saved\import_models_safe.txt', 'w', encoding='utf-8').write('\n'.join(out))
