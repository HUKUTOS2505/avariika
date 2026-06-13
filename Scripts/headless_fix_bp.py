# -*- coding: utf-8 -*-
"""HEADLESS (UnrealEditor-Cmd -run=pythonscript): починить меши 4 BP-предметов без импорта FBX.
Меши уже на диске в подпапках. Ставим per-instance override на ВСЕ размещённые инстансы +
CDO + bp.modify (как проверенный фикс сварочника). Диагностика до/после."""
import unreal

out = []
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# гарантированно загрузить уровень
try:
    les.load_level('/Game/FirstPerson/Lvl_FirstPerson')
    out.append('level loaded')
except Exception as e:
    out.append('load_level: %s' % e)

# bp-path, class, subfolder mesh path, target cm
CFG = [
    ('/Game/Avariika/Items/BP_FireExtinguisher', 'BP_FireExtinguisher_C', '/Game/Avariika/Meshes/SM_FireExtinguisher/SM_FireExtinguisher.SM_FireExtinguisher', 55.0),
    ('/Game/Avariika/Items/BP_FirstAidKit', 'BP_FirstAidKit_C', '/Game/Avariika/Meshes/SM_FirstAidKit/SM_FirstAidKit.SM_FirstAidKit', 28.0),
    ('/Game/Avariika/Items/BP_Battery', 'BP_Battery_C', '/Game/Avariika/Meshes/SM_Battery/SM_Battery.SM_Battery', 18.0),
    ('/Game/Avariika/Items/BP_WeldingMachine', 'BP_WeldingMachine_C', '/Game/Avariika/Meshes/SM_WeldingMachine/SM_WeldingMachine.SM_WeldingMachine', 45.0),
]


def comp_of(a):
    try:
        c = a.get_editor_property('MeshComponent')
        if c:
            return c
    except Exception:
        pass
    return a.get_component_by_class(unreal.StaticMeshComponent)


actors = eas.get_all_level_actors()
out.append('total actors: %d' % len(actors))

for bppath, cls, meshpath, target in CFG:
    mesh = unreal.load_asset(meshpath)
    if not mesh:
        out.append('%s: МЕШ НЕ НАЙДЕН %s' % (cls, meshpath)); continue
    b = mesh.get_bounds().box_extent
    mx = max(b.x * 2, b.y * 2, b.z * 2)
    scale = target / mx if mx > 0 else 1.0

    # CDO best-effort
    bp = unreal.EditorAssetLibrary.load_asset(bppath)
    if bp:
        try:
            cdo = unreal.get_default_object(bp.generated_class())
            cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
            bp.modify()
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
        except Exception as e:
            out.append('  %s cdo_err %s' % (cls, e))

    n = 0
    for a in actors:
        if a.get_class().get_name() == cls:
            c = comp_of(a)
            before = c.get_editor_property('static_mesh') if c else None
            bname = before.get_name() if before else 'NONE'
            if c:
                c.set_static_mesh(mesh)
                a.set_actor_scale3d(unreal.Vector(scale, scale, scale))
                a.modify()
                n += 1
            out.append('  %s[%s]: %s -> %s' % (cls, a.get_actor_label(), bname, mesh.get_name()))
    out.append('%s: fixed %d instances, scale=%.3f' % (cls, n, scale))

les.save_current_level()
saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('save_dirty=%s' % saved)
open(r'D:\unrealEngine\avariika\Saved\headless_fix_bp.txt', 'w', encoding='utf-8').write('\n'.join(out))
print('HEADLESS_FIX_BP_DONE')
