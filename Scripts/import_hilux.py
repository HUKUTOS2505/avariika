# -*- coding: utf-8 -*-
"""Импорт Toyota Hilux (скачанный мульти-материальный FBX, 38МБ) в подпапку Meshes/Hilux/,
с материалами+текстурами (лежат рядом с fbx). Заменить меш актора Gazelle_Mesh, масштаб
под длину пикапа (~530см), положить флэтом у ExitZone_Gazelle. Nanite для тяжёлого меша."""
import unreal

FBX = r'C:\unrealEngine\avariika\RawAssets\SM_Gazelle\source\hilux\hilux\hilux.fbx'
SUB = '/Game/Avariika/Meshes/Hilux'
NAME = 'SM_Hilux'
LENGTH = 530.0

tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

obj = SUB + '/' + NAME + '.' + NAME
if unreal.EditorAssetLibrary.does_asset_exist(obj):
    unreal.EditorAssetLibrary.delete_asset(obj)

t = unreal.AssetImportTask()
t.filename = FBX
t.destination_path = SUB
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
    out.append('FAIL import')
else:
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)
    mx = max(size) if max(size) > 0 else 1.0
    out.append('mesh %.0fx%.0fx%.0f sections=%d tris=%d' % (
        size[0], size[1], size[2], mesh.get_num_sections(0), mesh.get_num_triangles(0)))
    try:
        if mesh.get_num_triangles(0) > 20000:
            ns = mesh.get_editor_property('nanite_settings')
            ns.enabled = True
            mesh.set_editor_property('nanite_settings', ns)
            unreal.EditorAssetLibrary.save_loaded_asset(mesh)
            out.append('nanite on')
    except Exception as e:
        out.append('nanite? %s' % e)

    scale = LENGTH / mx
    # найти актор Gazelle_Mesh (или создать у ExitZone)
    by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
    actor = by.get('Gazelle_Mesh')
    if not actor:
        zone = by.get('ExitZone_Gazelle')
        loc = zone.get_actor_location() if zone else unreal.Vector(-300.0, 0.0, 0.0)
        actor = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(loc.x, loc.y, 0.0))
        actor.set_actor_label('Gazelle_Mesh')
    comp = actor.static_mesh_component
    comp.set_mobility(unreal.ComponentMobility.MOVABLE)
    comp.set_static_mesh(mesh)
    actor.set_actor_scale3d(unreal.Vector(scale, scale, scale))
    actor.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=90.0, roll=0.0), False)
    actor.modify()
    loc = actor.get_actor_location()
    out.append('scale=%.4f (len~%.0f) actor=Gazelle_Mesh loc=(%.0f,%.0f,%.0f)' % (scale, mx * scale, loc.x, loc.y, loc.z))

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'C:\unrealEngine\avariika\Saved\import_hilux.txt', 'w', encoding='utf-8').write('\n'.join(out))
