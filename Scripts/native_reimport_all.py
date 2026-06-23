# -*- coding: utf-8 -*-
"""Переимпорт ВСЕХ meshy-моделей с РОДНЫМИ материалами+текстурами (import_materials/textures=True)
в подпапки Meshes/<Name>/. Мой самодельный материал одноцветил модели; нативный импорт meshy
даёт верный многоцветный вид (доказано A/B на огнетушителе; унитаз уже так и сделан).
Назначение на акторы/BP + масштаб под реальный размер. Старые root-ассеты чистим отдельно."""
import os
import unreal

RAWROOT = r'C:\unrealEngine\avariika\RawAssets'
MESHES = '/Game/Avariika/Meshes'
tools = unreal.AssetToolsHelpers.get_asset_tools()
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

# folder, kind('actor:<label>' | 'bp:<path>:<class>'), target_cm
CFG = [
    ('SM_FireExtinguisher', 'bp:/Game/Avariika/Items/BP_FireExtinguisher:BP_FireExtinguisher_C', 55.0),
    ('SM_FirstAidKit', 'bp:/Game/Avariika/Items/BP_FirstAidKit:BP_FirstAidKit_C', 28.0),
    ('SM_Battery', 'bp:/Game/Avariika/Items/BP_Battery:BP_Battery_C', 18.0),
    ('SM_Fuse', 'bp:/Game/Avariika/Items/BP_Fuse:BP_Fuse_C', 12.0),
    ('SM_WeldingMachine', 'bp:/Game/Avariika/Items/BP_WeldingMachine:BP_WeldingMachine_C', 45.0),
    ('SM_MotionSensor', 'bp:/Game/Avariika/Items/BP_MotionSensor:BP_MotionSensor_C', 16.0),
    ('SM_Radio', 'actor:Radio', 22.0),
    ('SM_Tester', 'actor:Tester', 20.0),
    ('SM_Thermos', 'actor:Thermos', 25.0),
    ('SM_LightKit', 'actor:LightKit', 28.0),
    ('SM_TrapKit', 'actor:TrapKit', 18.0),
    ('SM_GasPipe', 'actor:Repairable_GasPipe', 220.0),
    ('SM_Generator', 'actor:Repairable_Generator', 200.0),
    ('SM_Breaker', 'actor:Repairable_Breaker', 160.0),
]


def comp_of(a):
    try:
        c = a.get_editor_property('MeshComponent')
        if c:
            return c
    except Exception:
        pass
    return a.get_component_by_class(unreal.StaticMeshComponent)


def find_fbx(folder):
    d = os.path.join(RAWROOT, folder)
    if not os.path.isdir(d):
        return None
    for f in sorted(os.listdir(d)):
        if f.lower().endswith('.fbx'):
            return os.path.join(d, f)
    return None


for folder, kind, target in CFG:
    try:
        fbx = find_fbx(folder)
        if not fbx:
            out.append('%s: нет fbx' % folder); continue
        sub = '%s/%s' % (MESHES, folder)
        obj = '%s/%s.%s' % (sub, folder, folder)
        if unreal.EditorAssetLibrary.does_asset_exist(obj):
            unreal.EditorAssetLibrary.delete_asset(obj)
        t = unreal.AssetImportTask()
        t.filename = fbx; t.destination_path = sub; t.destination_name = folder
        t.automated = True; t.replace_existing = True; t.save = True
        opt = unreal.FbxImportUI()
        opt.import_mesh = True; opt.import_as_skeletal = False
        opt.import_materials = True; opt.import_textures = True
        opt.static_mesh_import_data.set_editor_property('combine_meshes', True)
        t.options = opt
        tools.import_asset_tasks([t])
        mesh = unreal.load_asset(obj)
        if not mesh:
            out.append('%s: FAIL import' % folder); continue
        b = mesh.get_bounds().box_extent
        mx = max(b.x * 2, b.y * 2, b.z * 2)
        try:
            if mesh.get_num_triangles(0) > 20000:
                ns = mesh.get_editor_property('nanite_settings'); ns.enabled = True
                mesh.set_editor_property('nanite_settings', ns)
                unreal.EditorAssetLibrary.save_loaded_asset(mesh)
        except Exception:
            pass
        scale = target / mx if mx > 0 else 1.0
        msg = '%s: %.0fсм sect=%d scale=%.3f' % (folder, mx, mesh.get_num_sections(0), scale)

        k, _, rest = kind.partition(':')
        if k == 'bp':
            bppath, _, cls = rest.partition(':')
            bp = unreal.EditorAssetLibrary.load_asset(bppath)
            if bp:
                try:
                    cdo = unreal.get_default_object(bp.generated_class())
                    cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
                    bp.modify(); unreal.EditorAssetLibrary.save_loaded_asset(bp)
                except Exception as e:
                    msg += ' cdo_err=%s' % e
            n = 0
            for a in eas.get_all_level_actors():
                if a.get_class().get_name() == cls:
                    c = comp_of(a)
                    if c:
                        c.set_static_mesh(mesh); a.set_actor_scale3d(unreal.Vector(scale, scale, scale)); a.modify(); n += 1
            msg += ' -> CDO+%d' % n
        else:
            lbl = rest
            done = 0
            for a in eas.get_all_level_actors():
                if a.get_actor_label() == lbl:
                    c = comp_of(a)
                    if c:
                        c.set_static_mesh(mesh); a.set_actor_scale3d(unreal.Vector(scale, scale, scale)); a.modify(); done += 1
            msg += ' -> actor %s (%d)' % (lbl, done)
        out.append(msg)
    except Exception as e:
        out.append('%s: EXC %s' % (folder, e))

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'C:\unrealEngine\avariika\Saved\native_reimport_all.txt', 'w', encoding='utf-8').write('\n'.join(out))
