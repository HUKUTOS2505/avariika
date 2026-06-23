# -*- coding: utf-8 -*-
"""Проверить собранный меш ГАЗели, назначить на актор Gazelle_Mesh с верным масштабом/поворотом."""
import unreal
out = []
mesh = unreal.load_asset('/Game/Avariika/Meshes/Gazelle/uploads_files_6379322_Nissan+Frontier+Pickup+Truck+2004')
if not mesh:
    out.append('combined mesh MISSING')
else:
    b = mesh.get_bounds().box_extent
    sz = (b.x * 2, b.y * 2, b.z * 2)
    n = mesh.get_num_sections(0)
    realmat = sum(1 for i in range(n) if mesh.get_material(i) and 'WorldGrid' not in mesh.get_material(i).get_name())
    out.append('mesh size = %.0f x %.0f x %.0f cm, sections=%d, real materials=%d' % (sz[0], sz[1], sz[2], n, realmat))
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    found = False
    for a in eas.get_all_level_actors():
        if a.get_actor_label() == 'Gazelle_Mesh':
            found = True
            a.static_mesh_component.set_static_mesh(mesh)
            mx = max(sz)
            s = 500.0 / mx if mx > 1.0 else 1.0
            a.set_actor_scale3d(unreal.Vector(s, s, s))
            a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=90.0, roll=0.0), False)
            loc = a.get_actor_location()
            out.append('Gazelle_Mesh assigned: scale=%.3f, final~%.0fcm, loc=(%.0f,%.0f,%.0f)' % (s, mx * s, loc.x, loc.y, loc.z))
            break
    if not found:
        out.append('Gazelle_Mesh actor NOT FOUND')
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'C:\unrealEngine\avariika\Saved\gazelle_verify.txt', 'w', encoding='utf-8').write('\n'.join(out))
