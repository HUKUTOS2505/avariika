# -*- coding: utf-8 -*-
"""Импорт моделей из meshy.ai и назначение на акторы/предметы.

Пайплайн: пользователь кладёт FBX в D:\\unrealEngine\\avariika\\RawAssets\\<SM_Имя>\\модель.fbx
(имя папки — из таблицы MAPPING ниже). Запуск при закрытом редакторе:
  UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="Scripts/import_models.py"

Скрипт: импортирует в /Game/Avariika/Meshes/, назначает меш по метке актора или
в CDO Blueprint-предмета, масштабирует level-акторы под целевой габарит. Идемпотентен.
"""
import os
import unreal

RAW_DIR = r'D:\unrealEngine\avariika\RawAssets'
DEST = '/Game/Avariika/Meshes'
out = []

# имя папки -> (куда ставить: 'actor:<label>' | 'bp:<путь BP>', целевой max-габарит см для level-актора или 0)
MAPPING = {
    'SM_Toilet':           ('actor:Toilet', 75.0),   # обычный унитаз (~75 см с бачком), НЕ кабинка
    'SM_Breaker':          ('actor:Repairable_Breaker', 160.0),
    'SM_GasPipe':          ('actor:Repairable_GasPipe', 240.0),
    'SM_Generator':        ('actor:Repairable_Generator', 160.0),
    'SM_Tester':           ('actor:Tester', 22.0),
    'SM_Radio':            ('actor:Radio', 25.0),
    'SM_Gazelle':          ('spawn:Gazelle_Mesh', 480.0),  # декор в зоне выхода
    'SM_FirstAidKit':      ('bp:/Game/Avariika/Items/BP_FirstAidKit', 0.0),
    'SM_Cigarettes':       ('bp:/Game/Avariika/Items/BP_Cigarettes', 0.0),
    'SM_WeldingMachine':   ('bp:/Game/Avariika/Items/BP_WeldingMachine', 0.0),
    'SM_Battery':          ('bp:/Game/Avariika/Items/BP_Battery', 0.0),
    'SM_Fuse':             ('bp:/Game/Avariika/Items/BP_Fuse', 0.0),
    'SM_FireExtinguisher': ('bp:/Game/Avariika/Items/BP_FireExtinguisher', 0.0),
}


def find_source(folder):
    """Первый FBX/OBJ/GLB в папке."""
    for ext in ('.fbx', '.obj', '.glb'):
        for f in sorted(os.listdir(folder)):
            if f.lower().endswith(ext):
                return os.path.join(folder, f)
    return None


def import_mesh(name, source):
    asset_path = DEST + '/' + name
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path + '.' + name):
        unreal.EditorAssetLibrary.delete_asset(asset_path + '.' + name)  # переимпорт свежей версии

    task = unreal.AssetImportTask()
    task.filename = source
    task.destination_path = DEST
    task.destination_name = name
    task.automated = True
    task.replace_existing = True
    task.save = True

    options = unreal.FbxImportUI()
    options.import_mesh = True
    options.import_as_skeletal = False
    options.import_animations = False
    options.import_materials = True
    options.import_textures = True
    options.static_mesh_import_data.set_editor_property('combine_meshes', True)
    task.options = options

    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    mesh = unreal.EditorAssetLibrary.load_asset(asset_path + '.' + name)
    return mesh


def enable_nanite(mesh):
    """Тяжёлые меши из meshy (сотни тысяч треугольников) включаем в Nanite —
    UE5 рендерит их эффективно без ручного ремеша. Возвращает True, если включил."""
    try:
        tris = mesh.get_num_triangles(0)
    except Exception:
        tris = 0
    if tris < 20000:
        return False  # лёгкий меш — Nanite не нужен
    try:
        sub = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
        sub.set_nanite_enabled(mesh, True)  # сам перестроит
        return True
    except Exception:
        try:
            ns = mesh.get_editor_property('nanite_settings')
            ns.set_editor_property('enabled', True)
            mesh.set_editor_property('nanite_settings', ns)
            unreal.EditorAssetLibrary.save_loaded_asset(mesh)
            return True
        except Exception as e:
            return 'err:' + str(e)


les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not les.load_level('/Game/FirstPerson/Lvl_FirstPerson'):
    raise RuntimeError('Не удалось загрузить уровень')
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
level_dirty = False

if not os.path.isdir(RAW_DIR):
    os.makedirs(RAW_DIR)
    out.append('Создал ' + RAW_DIR + ' — клади туда папки с моделями')

for name, (target, max_dim) in MAPPING.items():
    folder = os.path.join(RAW_DIR, name)
    if not os.path.isdir(folder):
        continue
    source = find_source(folder)
    if not source:
        out.append('SKIP %s: в папке нет fbx/obj/glb' % name)
        continue

    mesh = import_mesh(name, source)
    if not mesh:
        out.append('FAIL %s: импорт не удался' % name)
        continue
    bounds = mesh.get_bounds().box_extent
    size = (bounds.x * 2, bounds.y * 2, bounds.z * 2)
    try:
        tris = mesh.get_num_triangles(0)
    except Exception:
        tris = -1
    out.append('OK %s: %.0fx%.0fx%.0f см, %d тр из %s' % (name, size[0], size[1], size[2], tris, os.path.basename(source)))

    nanite = enable_nanite(mesh)
    if nanite is True:
        out.append('  Nanite ВКЛ (тяжёлый меш)')
    elif isinstance(nanite, str):
        out.append('  WARN Nanite не включился: ' + nanite)

    kind, _, key = target.partition(':')
    if kind == 'actor':
        actor = by_label.get(key)
        if not actor:
            out.append('  WARN: актор %s не найден' % key)
            continue
        actor.get_editor_property('MeshComponent').set_static_mesh(mesh)
        if max_dim > 0 and max(size) > 1.0:
            scale = max_dim / max(size)
            actor.set_actor_scale3d(unreal.Vector(scale, scale, scale))
            out.append('  назначен на %s, scale %.2f' % (key, scale))
        level_dirty = True
    elif kind == 'spawn':
        if key in by_label:
            actor = by_label[key]
        else:
            zone = by_label.get('ExitZone_Gazelle')
            loc = zone.get_actor_location() if zone else unreal.Vector(-250.0, 0.0, 300.0)
            actor = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(loc.x, loc.y, 210.0), unreal.Rotator(0.0, 90.0, 0.0))
            actor.set_actor_label(key)
            by_label[key] = actor
        comp = actor.static_mesh_component
        comp.set_editor_property('static_mesh', mesh)
        if max_dim > 0 and max(size) > 1.0:
            scale = max_dim / max(size)
            actor.set_actor_scale3d(unreal.Vector(scale, scale, scale))
        out.append('  декор %s готов' % key)
        level_dirty = True
    elif kind == 'bp':
        bp = unreal.EditorAssetLibrary.load_asset(key)
        if not bp:
            out.append('  WARN: BP %s не найден' % key)
            continue
        cdo = unreal.get_default_object(bp.generated_class())
        cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
        unreal.EditorAssetLibrary.save_loaded_asset(bp)
        out.append('  назначен в CDO %s (масштаб предмета проверить руками в игре)' % key.split('/')[-1])

# Нормал-карты от meshy импортируются как sRGB-цвет и роняют рендер
# («Texture not valid! NormalMap»). Чиним КАЖДЫЙ импорт: sRGB off + TC_NORMALMAP.
for asset_path in unreal.EditorAssetLibrary.list_assets(DEST, recursive=True):
    nm = asset_path.split('.')[0]
    base = nm.split('/')[-1].lower()
    if 'normal' in base:
        tex = unreal.load_asset(nm)
        if isinstance(tex, unreal.Texture2D) and tex.get_editor_property('srgb'):
            tex.set_editor_property('srgb', False)
            tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_NORMALMAP)
            tex.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_WORLD_NORMAL_MAP)
            unreal.EditorAssetLibrary.save_loaded_asset(tex)
            out.append('нормаль поправлена: ' + base)

if level_dirty:
    # Уровень — World Partition (OFPA): акторы лежат во внешних пакетах, и
    # save_current_level их НЕ пишет. Сохраняем все dirty-пакеты (карта + внешние акторы).
    saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    les.save_current_level()
    out.append('сохранено dirty-пакетов: ' + str(saved))

with open(r'D:\unrealEngine\avariika\Saved\import_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out) if out else 'нечего импортировать')
