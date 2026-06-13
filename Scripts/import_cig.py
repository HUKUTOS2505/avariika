# -*- coding: utf-8 -*-
"""Импорт сигарет (скачанный мульти-материальный FBX) с материалами+текстурами.
Текстуры лежат рядом с fbx (скопированы). Назначение: CDO BP_Cigarettes + per-instance."""
import os
import unreal

RAW = r'D:\unrealEngine\avariika\RawAssets\SM_Cigarettes\source'
DEST = '/Game/Avariika/Meshes'
NAME = 'SM_Cigarettes'
BP = '/Game/Avariika/Items/BP_Cigarettes'
CLS = 'BP_Cigarettes_C'
MAX_DIM = 11.0

tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

fbx = os.path.join(RAW, 'cig.fbx')
obj = DEST + '/' + NAME + '.' + NAME
if unreal.EditorAssetLibrary.does_asset_exist(obj):
    unreal.EditorAssetLibrary.delete_asset(obj)

t = unreal.AssetImportTask()
t.filename = fbx
t.destination_path = DEST
t.destination_name = NAME
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
mesh = unreal.load_asset(obj)
if not mesh:
    out.append('FAIL no mesh')
else:
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)
    mx = max(size) if max(size) > 0 else 1.0
    out.append('mesh %.1fx%.1fx%.1f sections=%d' % (size[0], size[1], size[2], mesh.get_num_sections(0)))
    # отчёт по материалам/текстурам
    for i in range(max(mesh.get_num_sections(0), 1)):
        m = mesh.get_material(i)
        out.append('  slot %d: mat=%s' % (i, m.get_name() if m else 'None'))

    scale = (MAX_DIM / mx) if mx > 1.0 else 1.0
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
    out.append('scale=%.3f instances=%d' % (scale, n))

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\import_cig.txt', 'w', encoding='utf-8').write('\n'.join(out))
