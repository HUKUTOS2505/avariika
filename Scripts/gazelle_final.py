# -*- coding: utf-8 -*-
import unreal
out = []
m = unreal.load_asset('/Game/Avariika/Meshes/Gazelle/uploads_files_6379322_Nissan+Frontier+Pickup+Truck+2004')
if not m:
    out.append('mesh MISSING')
else:
    tris = m.get_num_triangles(0)
    b = m.get_bounds()
    o = b.origin
    e = b.box_extent
    out.append('tris=%d origin=(%.0f,%.0f,%.0f) extent=(%.1f,%.1f,%.1f)' % (tris, o.x, o.y, o.z, e.x, e.y, e.z))
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        if a.get_actor_label() == 'Gazelle_Mesh':
            a.static_mesh_component.set_static_mesh(m)
            mx = max(e.x * 2, e.y * 2, e.z * 2)
            s = (500.0 / mx) if mx > 1.0 else 100.0
            a.set_actor_scale3d(unreal.Vector(s, s, s))
            a.set_actor_location(unreal.Vector(-300, 0, 100))
            out.append('assigned scale=%.2f' % s)
            break
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.save_current_level()
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'D:\unrealEngine\avariika\Saved\gazelle_final.txt', 'w', encoding='utf-8').write('\n'.join(out))
