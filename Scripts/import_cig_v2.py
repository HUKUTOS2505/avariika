# -*- coding: utf-8 -*-
"""Импорт новой ПРОСТОЙ пачки сигарет (meshy native) взамен старой сложной (пачка+20).
Чистим старую подпапку, импортируем native в Meshes/Cigarettes/, назначаем на BP_Cigarettes."""
import os
import unreal
tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

SUB = '/Game/Avariika/Meshes/Cigarettes'
RAW = r'D:\unrealEngine\avariika\RawAssets\SM_Cigarettes'
fbx = None
for f in sorted(os.listdir(RAW)):
    if f.lower().endswith('.fbx'):
        fbx = os.path.join(RAW, f); break

if unreal.EditorAssetLibrary.does_directory_exist(SUB):
    unreal.EditorAssetLibrary.delete_directory(SUB)

t = unreal.AssetImportTask()
t.filename = fbx; t.destination_path = SUB; t.destination_name = 'SM_Cigarettes'
t.automated = True; t.replace_existing = True; t.save = True
opt = unreal.FbxImportUI()
opt.import_mesh = True; opt.import_as_skeletal = False
opt.import_materials = True; opt.import_textures = True
opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
t.options = opt
tools.import_asset_tasks([t])
mesh = unreal.load_asset(SUB + '/SM_Cigarettes.SM_Cigarettes')
if not mesh:
    out.append('FAIL import')
else:
    b = mesh.get_bounds().box_extent
    mx = max(b.x*2, b.y*2, b.z*2)
    scale = 10.0 / mx if mx > 0 else 1.0
    out.append('cig mesh %.0fx%.0fx%.0f sect=%d scale=%.3f' % (b.x*2, b.y*2, b.z*2, mesh.get_num_sections(0), scale))
    bp = unreal.EditorAssetLibrary.load_asset('/Game/Avariika/Items/BP_Cigarettes')
    if bp:
        cdo = unreal.get_default_object(bp.generated_class())
        cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
        cdo.get_editor_property('MeshComponent').set_editor_property('override_materials', [])
        bp.modify(); unreal.EditorAssetLibrary.save_loaded_asset(bp)
    n = 0
    for a in eas.get_all_level_actors():
        if a.get_class().get_name() == 'BP_Cigarettes_C':
            c = a.get_editor_property('MeshComponent') or a.get_component_by_class(unreal.StaticMeshComponent)
            c.set_static_mesh(mesh)
            c.set_editor_property('override_materials', [])  # снять старый MI-override
            cnt = max(mesh.get_num_sections(0), 1)
            for i in range(cnt):
                c.set_material(i, mesh.get_material(i))      # явный native материал per-instance
            a.set_actor_scale3d(unreal.Vector(scale, scale, scale))
            a.modify(); n += 1
    out.append('assigned to %d instances' % n)

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\import_cig_v2.txt', 'w', encoding='utf-8').write('\n'.join(out))
