# -*- coding: utf-8 -*-
"""Снять override-материалы (MI_*) с компонентов BP-предметов, чтобы рендерился родной
многоцветный Material_001 меша. На CDO + на ВСЕХ размещённых инстансах. Сохранить."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []

BP = {
    'BP_FireExtinguisher_C': '/Game/Avariika/Items/BP_FireExtinguisher',
    'BP_FirstAidKit_C': '/Game/Avariika/Items/BP_FirstAidKit',
    'BP_Battery_C': '/Game/Avariika/Items/BP_Battery',
    'BP_MotionSensor_C': '/Game/Avariika/Items/BP_MotionSensor',
    'BP_WeldingMachine_C': '/Game/Avariika/Items/BP_WeldingMachine',
    'BP_Fuse_C': '/Game/Avariika/Items/BP_Fuse',
    'BP_Cigarettes_C': '/Game/Avariika/Items/BP_Cigarettes',
}


def comp_of(a):
    try:
        c = a.get_editor_property('MeshComponent')
        if c:
            return c
    except Exception:
        pass
    return a.get_component_by_class(unreal.StaticMeshComponent)


def clear(comp):
    try:
        comp.set_editor_property('override_materials', [])
        return True
    except Exception:
        return False


# 1) CDO каждого BP
for cls, path in BP.items():
    bp = unreal.EditorAssetLibrary.load_asset(path)
    if not bp:
        continue
    try:
        cdo = unreal.get_default_object(bp.generated_class())
        c = cdo.get_editor_property('MeshComponent')
        if c and clear(c):
            bp.modify()
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
            out.append('CDO %s: override cleared' % cls)
    except Exception as e:
        out.append('CDO %s err %s' % (cls, e))

# 2) все размещённые инстансы
n = 0
for a in eas.get_all_level_actors():
    if a.get_class().get_name() in BP:
        c = comp_of(a)
        if c and clear(c):
            a.modify(); n += 1
            m0 = c.get_material(0)
            out.append('  %s[%s]: comp_mat now=%s' % (a.get_class().get_name(), a.get_actor_label(), m0.get_name() if m0 else 'None'))
out.append('instances cleared = %d' % n)

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\fix_clear_overrides.txt', 'w', encoding='utf-8').write('\n'.join(out))
