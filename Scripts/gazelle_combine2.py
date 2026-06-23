# -*- coding: utf-8 -*-
"""ГАЗель combine v2: создать пайплайн-ассет InterchangeGenericAssetsPipeline с
combine_static_meshes=True, сохранить, передать его путём в override_pipelines,
импортировать glb в один меш, назначить на актор. ASCII."""
import os
import unreal

RAW = r'C:\unrealEngine\avariika\RawAssets\SM_Gazelle'
DEST = '/Game/Avariika/Meshes/Gazelle'
PLPATH = '/Game/Avariika/Meshes/PL_GazelleCombine'
out = []

glb = next((os.path.join(RAW, f) for f in sorted(os.listdir(RAW)) if f.lower().endswith('.glb')), None)
tools = unreal.AssetToolsHelpers.get_asset_tools()

if unreal.EditorAssetLibrary.does_asset_exist(PLPATH):
    unreal.EditorAssetLibrary.delete_asset(PLPATH)
try:
    pl = tools.create_asset('PL_GazelleCombine', '/Game/Avariika/Meshes', unreal.InterchangeGenericAssetsPipeline, None)
    pl.get_editor_property('mesh_pipeline').set_editor_property('combine_static_meshes', True)
    pl.get_editor_property('common_meshes_properties').set_editor_property('bake_meshes', True)  # запечь трансформы узлов
    unreal.EditorAssetLibrary.save_loaded_asset(pl)
    out.append('pipeline asset created + combine=True + bake=True')
except Exception as e:
    out.append('pipeline asset FAIL: ' + str(e))

if unreal.EditorAssetLibrary.does_directory_exist(DEST):
    unreal.EditorAssetLibrary.delete_directory(DEST)

try:
    mgr = unreal.InterchangeManager.get_interchange_manager_scripted()
    sd = mgr.create_source_data(glb)
    params = unreal.ImportAssetParameters()
    params.is_automated = True
    params.override_pipelines = [unreal.SoftObjectPath(PLPATH + '.PL_GazelleCombine')]
    ok = mgr.import_asset(DEST, sd, params)
    out.append('import_asset ok=%s' % ok)
except Exception as e:
    out.append('IMPORT FAIL: ' + str(e))

assets = unreal.EditorAssetLibrary.list_assets(DEST, recursive=True)
meshes = [a.split('.')[0] for a in assets if isinstance(unreal.load_asset(a.split('.')[0]), unreal.StaticMesh)]
out.append('static meshes = %d' % len(meshes))
best, bestv = None, -1.0
for m in meshes:
    b = unreal.load_asset(m).get_bounds().box_extent
    v = b.x * b.y * b.z
    if v > bestv:
        bestv, best = v, m
if best:
    mesh = unreal.load_asset(best)
    out.append('combined = %s sections=%d' % (best.split('/')[-1], mesh.get_num_sections(0)))
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        if a.get_actor_label() == 'Gazelle_Mesh':
            a.static_mesh_component.set_static_mesh(mesh)
            bb = mesh.get_bounds().box_extent
            mx = max(bb.x * 2, bb.y * 2, bb.z * 2)
            if mx > 1.0:
                s = 480.0 / mx
                a.set_actor_scale3d(unreal.Vector(s, s, s))
                out.append('reassigned Gazelle_Mesh scale=%.3f size=%.0fx%.0fx%.0f' % (s, bb.x*2, bb.y*2, bb.z*2))
            break
else:
    out.append('no mesh')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('saved')
open(r'C:\unrealEngine\avariika\Saved\gazelle_combine2.txt', 'w', encoding='utf-8').write('\n'.join(out))
