# -*- coding: utf-8 -*-
"""Persist the welder mesh: the import set it only on the in-memory BP CDO (native
component default does not serialize). Set a per-instance override on the placed
'WeldingMachine' actor (saved in its external actor package) + best-effort BP CDO. ASCII."""
import unreal

out = []
mesh = unreal.load_asset('/Game/Avariika/Meshes/SM_WeldingMachine.SM_WeldingMachine')

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
target = None
for a in eas.get_all_level_actors():
    if a.get_actor_label() == 'WeldingMachine':
        target = a
        break

if target and mesh:
    comp = target.get_editor_property('MeshComponent')
    if comp is None:
        comp = target.get_component_by_class(unreal.StaticMeshComponent)
    comp.set_static_mesh(mesh)
    target.modify()
    sm = comp.get_editor_property('static_mesh')
    out.append('instance WeldingMachine mesh = ' + (sm.get_path_name() if sm else 'NONE'))
else:
    out.append('target_found=%s mesh_loaded=%s' % (bool(target), bool(mesh)))

bp = unreal.load_asset('/Game/Avariika/Items/BP_WeldingMachine')
if bp and mesh:
    cdo = unreal.get_default_object(bp.generated_class())
    cdo.get_editor_property('MeshComponent').set_static_mesh(mesh)
    bp.modify()
    unreal.EditorAssetLibrary.save_loaded_asset(bp)
    out.append('BP CDO set + saved')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('save_dirty_packages = ' + str(saved))

open(r'D:\unrealEngine\avariika\Saved\fix_welder.txt', 'w', encoding='utf-8').write('\n'.join(out))
