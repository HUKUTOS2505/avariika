# -*- coding: utf-8 -*-
"""Сигарета засорила корень Meshes/ (~120 ассетов из 20 сигарет в FBX). Чистим корень
и переимпортируем в подпапку Meshes/Cigarettes/ (вся каша материалов/текстур там же)."""
import os
import unreal

ROOT = '/Game/Avariika/Meshes'
SUB = '/Game/Avariika/Meshes/Cigarettes'
RAW = r'D:\unrealEngine\avariika\RawAssets\SM_Cigarettes\source'
BP = '/Game/Avariika/Items/BP_Cigarettes'
CLS = 'BP_Cigarettes_C'
MAX_DIM = 11.0

tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []

# 1) удалить cig-мусор из корня по характерным основам имён
STEMS = ['6408', '9926', '681928', 'Case_base', 'Cig_base', 'Cig_filter', 'tobaco',
         'Sticker', 'elastic', 'plastic', 'Inside_white', 'Screen-Shot', 'SM_Cigarettes']
killed = 0
for a in unreal.EditorAssetLibrary.list_assets(ROOT, recursive=False):
    base = a.split('.')[0]
    nm = base.split('/')[-1]
    if any(s in nm for s in STEMS):
        try:
            unreal.EditorAssetLibrary.delete_asset(base)
            killed += 1
        except Exception:
            pass
out.append('killed root cig assets = %d' % killed)

# 2) переимпорт в подпапку
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')
fbx = os.path.join(RAW, 'cig.fbx')
t = unreal.AssetImportTask()
t.filename = fbx
t.destination_path = SUB
t.destination_name = 'SM_Cigarettes'
t.automated = True
t.replace_existing = True
t.save = True
opt = unreal.FbxImportUI()
opt.import_mesh = True
opt.import_as_skeletal = False
opt.import_materials = True
opt.import_textures = True
opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
t.options = opt
tools.import_asset_tasks([t])
mesh = unreal.load_asset(SUB + '/SM_Cigarettes.SM_Cigarettes')
if not mesh:
    out.append('FAIL reimport')
else:
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)
    mx = max(size) if max(size) > 0 else 1.0
    scale = (MAX_DIM / mx) if mx > 1.0 else 1.0
    out.append('mesh %.1fx%.1fx%.1f sections=%d scale=%.3f' % (size[0], size[1], size[2], mesh.get_num_sections(0), scale))
    bp = unreal.EditorAssetLibrary.load_asset(BP)
    if bp:
        try:
            cdo = unreal.get_default_object(bp.generated_class())
            cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
            bp.modify()
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
        except Exception as e:
            out.append('cdo err %s' % e)
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
    out.append('instances=%d' % n)

# контроль: что осталось в корне с cig-основами
leftover = [x for x in unreal.EditorAssetLibrary.list_assets(ROOT, recursive=False)
            if any(s in x.split('/')[-1] for s in STEMS)]
out.append('root cig leftover = %d' % len(leftover))

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\reimport_cig_clean.txt', 'w', encoding='utf-8').write('\n'.join(out))
