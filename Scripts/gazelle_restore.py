# -*- coding: utf-8 -*-
"""Вернуть видимый серый FBX-грузовик (реальная геометрия), почистить glb-эксперименты.
Цветная сборка многоузлового glb скриптовым Interchange не вышла (combine -> нулевые габариты);
делается ручным драг-импортом glb в редактор. ASCII."""
import os
import unreal

RAW = r'C:\unrealEngine\avariika\RawAssets\SM_Gazelle'
DEST = '/Game/Avariika/Meshes'
out = []

# почистить glb-эксперименты
for p in ('/Game/Avariika/Meshes/Gazelle', '/Game/Avariika/Meshes/PL_GazelleCombine'):
    if unreal.EditorAssetLibrary.does_directory_exist(p):
        unreal.EditorAssetLibrary.delete_directory(p)
    elif unreal.EditorAssetLibrary.does_asset_exist(p):
        unreal.EditorAssetLibrary.delete_asset(p)

fbx = next((os.path.join(RAW, f) for f in sorted(os.listdir(RAW)) if f.lower().endswith('.fbx')), None)
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')
obj = DEST + '/SM_Gazelle.SM_Gazelle'
if unreal.EditorAssetLibrary.does_asset_exist(obj):
    unreal.EditorAssetLibrary.delete_asset(obj)
t = unreal.AssetImportTask()
t.filename = fbx
t.destination_path = DEST
t.destination_name = 'SM_Gazelle'
t.automated = True
t.replace_existing = True
t.save = True
opt = unreal.FbxImportUI()
opt.import_mesh = True
opt.import_as_skeletal = False
opt.import_materials = False
opt.import_textures = False
opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
t.options = opt
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
mesh = unreal.load_asset(obj)
if mesh:
    b = mesh.get_bounds().box_extent
    mx = max(b.x * 2, b.y * 2, b.z * 2)
    out.append('fbx gray: tris=%d size=%.1fcm' % (mesh.get_num_triangles(0), mx))
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        if a.get_actor_label() == 'Gazelle_Mesh':
            a.static_mesh_component.set_static_mesh(mesh)
            s = (480.0 / mx) if mx > 1.0 else 1.0
            a.set_actor_scale3d(unreal.Vector(s, s, s))
            a.set_actor_location(unreal.Vector(-300, 0, 100))
            a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=90.0, roll=0.0), False)
            out.append('Gazelle_Mesh assigned scale=%.2f -> ~%.0fcm' % (s, mx * s))
            break

for a in list(unreal.EditorAssetLibrary.list_assets(DEST, recursive=False)):
    nm = a.split('.')[0].split('/')[-1]
    if nm.startswith(('Paint_', 'Aluminum_', 'Emissive_', 'Matte', 'Anodized_', 'Solar_', 'faro')):
        unreal.EditorAssetLibrary.delete_asset(a.split('.')[0])

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('saved')
open(r'C:\unrealEngine\avariika\Saved\gazelle_restore.txt', 'w', encoding='utf-8').write('\n'.join(out))
