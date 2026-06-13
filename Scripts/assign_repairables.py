# -*- coding: utf-8 -*-
"""Назначить реальные меши размещённым ремонтируемым/тестеру (сейчас кубы/цилиндры):
per-instance override + scale + modify, сохранить уровень и external-акторы."""
import unreal

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

# label -> (mesh asset, target max-dim cm)
JOBS = {
    'Repairable_GasPipe':   ('SM_GasPipe', 220.0),
    'Repairable_Generator': ('SM_Generator', 200.0),
    'Repairable_Breaker':   ('SM_Breaker', 160.0),
    'Tester':               ('SM_Tester', 22.0),
}


def apply(actor, mesh, max_dim):
    comp = actor.get_editor_property('MeshComponent')
    if comp is None:
        comp = actor.get_component_by_class(unreal.StaticMeshComponent)
    if comp is None:
        return 'нет компонента'
    comp.set_static_mesh(mesh)
    b = mesh.get_bounds().box_extent
    size = (b.x * 2, b.y * 2, b.z * 2)
    mx = max(size) if max(size) > 0 else 1.0
    if max_dim > 0 and mx > 1.0:
        s = max_dim / mx
        actor.set_actor_scale3d(unreal.Vector(s, s, s))
    actor.modify()
    return 'mesh=%s %.0fx%.0fx%.0f scale=%.3f' % (mesh.get_name(), size[0], size[1], size[2], (max_dim / mx) if max_dim > 0 else 1.0)


for lbl, (mname, md) in JOBS.items():
    a = by.get(lbl)
    if not a:
        out.append('%s: актор не найден' % lbl)
        continue
    mesh = unreal.load_asset('/Game/Avariika/Meshes/%s.%s' % (mname, mname))
    if not mesh:
        out.append('%s: меш %s не найден' % (lbl, mname))
        continue
    out.append('%s -> %s' % (lbl, apply(a, mesh, md)))

les.save_current_level()
saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('save_dirty=%s' % saved)
open(r'D:\unrealEngine\avariika\Saved\assign_repairables.txt', 'w', encoding='utf-8').write('\n'.join(out))
