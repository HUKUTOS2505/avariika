# -*- coding: utf-8 -*-
"""Назначить унитаз на актор Toilet ПРЯМО в открытом уровне (без load_level,
который в редакторе плодит отдельный мир и не сохраняется). Запускать из
открытого редактора (Claudius editor.run_python_script) на загруженном Lvl_FirstPerson.
"""
import unreal

out = []
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
mesh = unreal.load_asset('/Game/Avariika/Meshes/SM_Toilet')
done = False
for a in eas.get_all_level_actors():
    if a.get_actor_label() == 'Toilet':
        comp = a.get_editor_property('MeshComponent')
        comp.set_static_mesh(mesh)
        a.set_actor_scale3d(unreal.Vector(1.0, 1.0, 1.0))
        b = mesh.get_bounds().box_extent
        out.append('Toilet: mesh=SM_Toilet, scale=1.0, габарит %.0fx%.0fx%.0f' % (b.x * 2, b.y * 2, b.z * 2))
        done = True
        break
if not done:
    out.append('актор Toilet не найден в открытом уровне')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('save_current_level + save_dirty_packages=%s' % saved)

with open(r'C:\unrealEngine\avariika\Saved\assign_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
