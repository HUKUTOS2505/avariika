# -*- coding: utf-8 -*-
"""ГАЗель: переимпорт glb через Interchange с combine_static_meshes=True → один
текстурный меш, назначить на актор Gazelle_Mesh. ASCII-вывод."""
import os
import unreal

RAW = r'C:\unrealEngine\avariika\RawAssets\SM_Gazelle'
DEST = '/Game/Avariika/Meshes/Gazelle'
out = []

glb = next((os.path.join(RAW, f) for f in sorted(os.listdir(RAW)) if f.lower().endswith('.glb')), None)
out.append('glb = ' + str(glb))

if unreal.EditorAssetLibrary.does_directory_exist(DEST):
    unreal.EditorAssetLibrary.delete_directory(DEST)

try:
    mgr = unreal.InterchangeManager.get_interchange_manager_scripted()
    sd = mgr.create_source_data(glb)
    pipe = unreal.InterchangeGenericAssetsPipeline()
    mp = pipe.get_editor_property('mesh_pipeline')
    mp.set_editor_property('combine_static_meshes', True)
    params = unreal.ImportAssetParameters()
    params.is_automated = True
    params.override_pipelines = [pipe]
    ok = mgr.import_asset(DEST, sd, params)
    out.append('import_asset ok=%s' % ok)
except Exception as e:
    out.append('IMPORT FAIL: ' + str(e))

assets = unreal.EditorAssetLibrary.list_assets(DEST, recursive=True)
meshes = [a.split('.')[0] for a in assets if isinstance(unreal.load_asset(a.split('.')[0]), unreal.StaticMesh)]
out.append('total assets=%d, static meshes=%d' % (len(assets), len(meshes)))

best, bestv = None, -1.0
for m in meshes:
    mesh = unreal.load_asset(m)
    b = mesh.get_bounds().box_extent
    v = b.x * b.y * b.z
    if v > bestv:
        bestv, best = v, m
if best:
    mesh = unreal.load_asset(best)
    out.append('combined mesh = %s (sections=%d)' % (best.split('/')[-1], mesh.get_num_sections(0)))
    try:
        if mesh.get_num_triangles(0) > 20000:
            ns = mesh.get_editor_property('nanite_settings'); ns.enabled = True
            mesh.set_editor_property('nanite_settings', ns)
            unreal.EditorAssetLibrary.save_loaded_asset(mesh)
    except Exception as e:
        out.append('nanite? ' + str(e))
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        if a.get_actor_label() == 'Gazelle_Mesh':
            a.static_mesh_component.set_static_mesh(mesh)
            bb = mesh.get_bounds().box_extent
            mx = max(bb.x * 2, bb.y * 2, bb.z * 2)
            if mx > 1.0:
                s = 480.0 / mx
                a.set_actor_scale3d(unreal.Vector(s, s, s))
                out.append('reassigned Gazelle_Mesh, scale=%.3f' % s)
            break
else:
    out.append('no static mesh found')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('saved')
open(r'C:\unrealEngine\avariika\Saved\gazelle_combine.txt', 'w', encoding='utf-8').write('\n'.join(out))
