# -*- coding: utf-8 -*-
"""Бронебойно: на КАЖДОМ размещённом BP-инстансе явно выставить материал = материал меша
(per-instance override, хранится в external-акторе → переживает рекомпиляцию BP).
Это перекрывает любой MI_*-override с CDO."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
CLASSES = ['BP_FireExtinguisher_C', 'BP_FirstAidKit_C', 'BP_Battery_C',
           'BP_MotionSensor_C', 'BP_WeldingMachine_C', 'BP_Fuse_C']


def comp_of(a):
    try:
        c = a.get_editor_property('MeshComponent')
        if c:
            return c
    except Exception:
        pass
    return a.get_component_by_class(unreal.StaticMeshComponent)


n = 0
for a in eas.get_all_level_actors():
    if a.get_class().get_name() not in CLASSES:
        continue
    c = comp_of(a)
    if not c:
        continue
    sm = c.get_editor_property('static_mesh')
    if not sm:
        out.append('%s: нет меша' % a.get_actor_label()); continue
    cnt = max(sm.get_num_sections(0), 1)
    for i in range(cnt):
        mm = sm.get_material(i)
        c.set_material(i, mm)   # явный per-instance override = материал меша
    a.modify(); n += 1
    m0 = c.get_material(0)
    out.append('%s[%s]: forced %d slots, mat0=%s' % (a.get_class().get_name(), a.get_actor_label(), cnt, m0.get_name() if m0 else 'None'))

les.save_current_level()
out.append('instances=%d save_dirty=%s' % (n, unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)))
open(r'C:\unrealEngine\avariika\Saved\fix_force_instance_mat.txt', 'w', encoding='utf-8').write('\n'.join(out))
