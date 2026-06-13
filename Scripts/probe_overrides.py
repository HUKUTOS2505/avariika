# -*- coding: utf-8 -*-
"""Сравнить материалы КОМПОНЕНТА размещённых BP-предметов с материалами их МЕША.
Если у компонента override (отличается от меш-материала) — вот причина одноцветности."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
CLASSES = ['BP_FireExtinguisher_C', 'BP_FirstAidKit_C', 'BP_Fuse_C', 'BP_MotionSensor_C', 'BP_WeldingMachine_C', 'BP_Battery_C']


def comp_of(a):
    try:
        c = a.get_editor_property('MeshComponent')
        if c:
            return c
    except Exception:
        pass
    return a.get_component_by_class(unreal.StaticMeshComponent)


seen = set()
for a in eas.get_all_level_actors():
    cn = a.get_class().get_name()
    if cn not in CLASSES or cn in seen:
        continue
    seen.add(cn)
    c = comp_of(a)
    if not c:
        out.append('%s: нет компонента' % cn); continue
    sm = c.get_editor_property('static_mesh')
    mesh_mat = sm.get_material(0).get_name() if (sm and sm.get_material(0)) else 'None'
    # материалы, которые реально отрисует компонент
    comp_mats = []
    try:
        n = c.get_num_materials()
    except Exception:
        n = 1
    for i in range(max(n, 1)):
        m = c.get_material(i)
        comp_mats.append(m.get_name() if m else 'None')
    # есть ли override-элементы
    try:
        overrides = c.get_editor_property('override_materials')
        ov = [(o.get_name() if o else 'None') for o in overrides]
    except Exception as e:
        ov = 'err %s' % e
    out.append('%s [%s]: mesh=%s mesh_mat=%s comp_mats=%s override_materials=%s' % (
        cn, a.get_actor_label(), sm.get_name() if sm else 'None', mesh_mat, comp_mats, ov))
open(r'D:\unrealEngine\avariika\Saved\probe_overrides.txt', 'w', encoding='utf-8').write('\n'.join(out))
