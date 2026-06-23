# -*- coding: utf-8 -*-
"""LightKit нативно из переименованной папки SM_LightKit_Folded -> actor LightKit, 28см."""
import os
import unreal
tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

RAW = r'C:\unrealEngine\avariika\RawAssets\SM_LightKit_Folded'
fbx = None
for f in sorted(os.listdir(RAW)):
    if f.lower().endswith('.fbx'):
        fbx = os.path.join(RAW, f); break
sub = '/Game/Avariika/Meshes/SM_LightKit'
obj = sub + '/SM_LightKit.SM_LightKit'
if unreal.EditorAssetLibrary.does_asset_exist(obj):
    unreal.EditorAssetLibrary.delete_asset(obj)
t = unreal.AssetImportTask()
t.filename = fbx; t.destination_path = sub; t.destination_name = 'SM_LightKit'
t.automated = True; t.replace_existing = True; t.save = True
opt = unreal.FbxImportUI()
opt.import_mesh = True; opt.import_as_skeletal = False
opt.import_materials = True; opt.import_textures = True
opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
t.options = opt
tools.import_asset_tasks([t])
mesh = unreal.load_asset(obj)
if mesh:
    b = mesh.get_bounds().box_extent
    mx = max(b.x * 2, b.y * 2, b.z * 2)
    scale = 28.0 / mx if mx > 0 else 1.0
    done = 0
    for a in eas.get_all_level_actors():
        if a.get_actor_label() == 'LightKit':
            c = a.get_editor_property('MeshComponent') or a.get_component_by_class(unreal.StaticMeshComponent)
            c.set_static_mesh(mesh); a.set_actor_scale3d(unreal.Vector(scale, scale, scale)); a.modify(); done += 1
    out.append('LightKit native: %.0fсм scale=%.3f -> actor(%d)' % (mx, scale, done))
else:
    out.append('FAIL')
les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'C:\unrealEngine\avariika\Saved\import_lightkit_native.txt', 'w', encoding='utf-8').write('\n'.join(out))
