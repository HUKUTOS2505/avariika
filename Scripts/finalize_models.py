# -*- coding: utf-8 -*-
"""Verify the welder CDO mesh got assigned and save all dirty packages
(captures the Gazelle rotation fix + any actor/material/mesh edits). ASCII output."""
import unreal

out = []

bp = unreal.EditorAssetLibrary.load_asset('/Game/Avariika/Items/BP_WeldingMachine')
if bp:
    cdo = unreal.get_default_object(bp.generated_class())
    mc = cdo.get_editor_property('MeshComponent')
    sm = mc.get_editor_property('static_mesh') if mc else None
    out.append('welder mesh = ' + (sm.get_path_name() if sm else 'NONE'))
else:
    out.append('welder BP not found')

# Material sanity on the imported meshes
for name in ('SM_GasPipe', 'SM_Generator', 'SM_WeldingMachine', 'SM_Gazelle'):
    m = unreal.load_asset('/Game/Avariika/Meshes/%s.%s' % (name, name))
    if m:
        mat = m.get_material(0)
        try:
            tris = m.get_num_triangles(0)
        except Exception:
            tris = -1
        out.append('%s: mat=%s tris=%d' % (name, (mat.get_name() if mat else 'NONE'), tris))
    else:
        out.append('%s: mesh asset MISSING' % name)

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('save_dirty_packages = ' + str(saved))

open(r'C:\unrealEngine\avariika\Saved\finalize_models.txt', 'w', encoding='utf-8').write('\n'.join(out))
