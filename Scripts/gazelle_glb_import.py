# -*- coding: utf-8 -*-
"""Импорт ГАЗели из .glb через Interchange (glTF) С материалами. Без destination_name
(Interchange создаёт меш+материалы естественно в подпапке), затем меш назначается на
актор Gazelle_Mesh в активном уровне. ASCII-вывод."""
import os
import unreal

RAW = r'D:\unrealEngine\avariika\RawAssets\SM_Gazelle'
DEST = '/Game/Avariika/Meshes/Gazelle'
out = []

glb = next((os.path.join(RAW, f) for f in sorted(os.listdir(RAW)) if f.lower().endswith('.glb')), None)
out.append('glb = ' + str(glb))

if unreal.EditorAssetLibrary.does_directory_exist(DEST):
    unreal.EditorAssetLibrary.delete_directory(DEST)

t = unreal.AssetImportTask()
t.filename = glb
t.destination_path = DEST
t.automated = True
t.replace_existing = True
t.save = True
# .glb -> Interchange glTF pipeline (импортирует материалы по умолчанию); имя не форсируем
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])

try:
    imp = list(t.get_editor_property('imported_object_paths'))
except Exception:
    imp = []
out.append('imported_object_paths = %d' % len(imp))

assets = unreal.EditorAssetLibrary.list_assets(DEST, recursive=True)
meshes, mats, texs = [], 0, 0
for a in assets:
    o = unreal.load_asset(a.split('.')[0])
    if isinstance(o, unreal.StaticMesh):
        meshes.append(a.split('.')[0])
    elif isinstance(o, unreal.MaterialInterface):
        mats += 1
    elif isinstance(o, unreal.Texture):
        texs += 1
out.append('assets=%d meshes=%d materials=%d textures=%d' % (len(assets), len(meshes), mats, texs))
out.append('mesh = ' + (meshes[0] if meshes else 'NONE'))

if meshes:
    mesh = unreal.load_asset(meshes[0])
    # материалы на мешах
    nsec = mesh.get_num_sections(0)
    named = sum(1 for i in range(nsec) if mesh.get_material(i) and 'WorldGrid' not in mesh.get_material(i).get_name())
    out.append('mesh sections=%d, with-real-material=%d' % (nsec, named))
    # назначить на актор Gazelle_Mesh
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    done = False
    for actor in eas.get_all_level_actors():
        if actor.get_actor_label() == 'Gazelle_Mesh':
            actor.static_mesh_component.set_static_mesh(mesh)
            b = mesh.get_bounds().box_extent
            mx = max(b.x * 2, b.y * 2, b.z * 2)
            if mx > 1.0:
                s = 480.0 / mx
                actor.set_actor_scale3d(unreal.Vector(s, s, s))
            out.append('reassigned Gazelle_Mesh actor, scale=%.3f' % (480.0 / mx if mx > 1 else 1))
            done = True
            break
    if not done:
        out.append('Gazelle_Mesh actor not found in active level (%s)' % unreal.EditorLevelLibrary.get_editor_world().get_name() if False else 'not found')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('saved')

open(r'D:\unrealEngine\avariika\Saved\gazelle_glb.txt', 'w', encoding='utf-8').write('\n'.join(out))
