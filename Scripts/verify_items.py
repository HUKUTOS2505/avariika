# -*- coding: utf-8 -*-
"""Прочитать static_mesh у MeshComponent размещённых предметов и записать отчёт."""
import unreal

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
targets = ['Radio', 'Battery', 'Loot_Battery_SE', 'Loot_Battery_NW',
           'FireExtinguisher', 'FirstAidKit', 'Loot_FirstAid_NE', 'WeldingMachine']
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
for lbl in targets:
    a = by_label.get(lbl)
    if not a:
        out.append('%s: НЕ НАЙДЕН' % lbl)
        continue
    comp = a.get_editor_property('MeshComponent')
    if comp is None:
        comp = a.get_component_by_class(unreal.StaticMeshComponent)
    sm = comp.get_editor_property('static_mesh') if comp else None
    sc = a.get_actor_scale3d()
    out.append('%s: mesh=%s scale=(%.2f,%.2f,%.2f)' % (
        lbl, (sm.get_name() if sm else 'NONE'), sc.x, sc.y, sc.z))
open(r'D:\unrealEngine\avariika\Saved\verify_items.txt', 'w', encoding='utf-8').write('\n'.join(out))
