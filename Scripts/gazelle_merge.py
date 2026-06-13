# -*- coding: utf-8 -*-
"""Слить 121 собранный актор грузовика в ОДИН статик-меш (запекает мировые трансформы),
поставить один чистый актор Gazelle_Mesh, удалить scene-актёры и папку GazelleScene. Лог в варнинги."""
import unreal

def log(m):
    unreal.log_warning('GZMRG: ' + m)

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
acts = eas.get_all_level_actors()
root = next((a for a in acts if a.get_actor_label() == 'nisan_scene_0'), None)
log('root=%s' % bool(root))

if root:
    desc = []
    def walk(a):
        for c in a.get_attached_actors():
            desc.append(c)
            walk(c)
    walk(root)
    comps = []
    for a in [root] + desc:
        for c in a.get_components_by_class(unreal.StaticMeshComponent):
            if c.get_static_mesh():
                comps.append(c)
    log('static mesh comps=%d' % len(comps))

    out_pkg = '/Game/Avariika/Meshes/SM_GazelleTruck'
    if unreal.EditorAssetLibrary.does_asset_exist(out_pkg):
        unreal.EditorAssetLibrary.delete_asset(out_pkg)
    world = root.get_world()
    settings = unreal.MeshMergingSettings()
    merged = None
    try:
        unreal.MeshMergeFunctionLibrary.merge_static_mesh_components(comps, world, settings, out_pkg)
        merged = unreal.load_asset(out_pkg)
        log('merge done, mesh=%s' % bool(merged))
    except Exception as e:
        log('MERGE FAIL: ' + str(e))

    if merged:
        b = merged.get_bounds().box_extent
        log('merged size=%.0fx%.0fx%.0f' % (b.x * 2, b.y * 2, b.z * 2))
        # merge baked world transforms -> spawn at origin, geometry уже в мировых координатах
        a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, 0))
        a.static_mesh_component.set_static_mesh(merged)
        a.set_actor_label('Gazelle_Mesh')
        log('spawned clean Gazelle_Mesh')
        for x in [root] + desc:
            try:
                eas.destroy_actor(x)
            except Exception:
                pass
        log('deleted %d scene actors' % (len(desc) + 1))
        if unreal.EditorAssetLibrary.does_directory_exist('/Game/Avariika/Meshes/GazelleScene'):
            unreal.EditorAssetLibrary.delete_directory('/Game/Avariika/Meshes/GazelleScene')
            log('deleted GazelleScene folder')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
log('done')
