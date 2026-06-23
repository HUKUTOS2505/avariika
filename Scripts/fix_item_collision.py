# -*- coding: utf-8 -*-
"""Предметы мешают ходьбе: меш блокирует Pawn. Pawn=Ignore на меше носимых предметов.
Без isinstance — по имени класса. Полный перехват ошибок."""
import unreal
out = []
try:
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    ITEM_BP_PATHS = {
        'BP_Battery_C': '/Game/Avariika/Items/BP_Battery',
        'BP_Cigarettes_C': '/Game/Avariika/Items/BP_Cigarettes',
        'BP_FireExtinguisher_C': '/Game/Avariika/Items/BP_FireExtinguisher',
        'BP_FirstAidKit_C': '/Game/Avariika/Items/BP_FirstAidKit',
        'BP_Fuse_C': '/Game/Avariika/Items/BP_Fuse',
        'BP_MotionSensor_C': '/Game/Avariika/Items/BP_MotionSensor',
        'BP_WeldingMachine_C': '/Game/Avariika/Items/BP_WeldingMachine',
    }
    ITEM_CLASSES = set(ITEM_BP_PATHS.keys()) | {'PickupItem'}
    PAWN = unreal.CollisionChannel.ECC_PAWN
    IGNORE = unreal.CollisionResponseType.ECR_IGNORE

    def comp_of(a):
        try:
            c = a.get_editor_property('MeshComponent')
            if c:
                return c
        except Exception:
            pass
        return a.get_component_by_class(unreal.StaticMeshComponent)

    # CDO
    for cls, path in ITEM_BP_PATHS.items():
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if not bp:
            continue
        try:
            cdo = unreal.get_default_object(bp.generated_class())
            c = cdo.get_editor_property('MeshComponent')
            if c:
                c.set_collision_response_to_channel(PAWN, IGNORE)
                bp.modify(); unreal.EditorAssetLibrary.save_loaded_asset(bp)
                out.append('CDO %s ok' % cls)
        except Exception as e:
            out.append('CDO %s err %s' % (cls, e))

    n = 0
    for a in eas.get_all_level_actors():
        if a.get_class().get_name() not in ITEM_CLASSES:
            continue
        c = comp_of(a)
        if c:
            try:
                c.set_collision_response_to_channel(PAWN, IGNORE)
                a.modify(); n += 1
                out.append('%s[%s]: Pawn=Ignore' % (a.get_class().get_name(), a.get_actor_label()))
            except Exception as e:
                out.append('inst %s err %s' % (a.get_actor_label(), e))

    les.save_current_level()
    out.append('instances=%d save_dirty=%s' % (n, unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)))
except Exception as e:
    out.append('FATAL %s' % e)
open(r'C:\unrealEngine\avariika\Saved\fix_item_collision.txt', 'w', encoding='utf-8').write('\n'.join(out))
