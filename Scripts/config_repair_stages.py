# -*- coding: utf-8 -*-
"""Настроить многоэтапный ремонт: создать Кабель+Канистру (тяжёлые), задать ToolTags,
выставить PrereqStages на трубе/генераторе/щитке. Меши пока заглушки (модели позже)."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
PICKUP_CLS = unreal.load_class(None, '/Script/Avaryo.PickupItem')
CUBE = unreal.load_asset('/Engine/BasicShapes/Cube.Cube')
CYL = unreal.load_asset('/Engine/BasicShapes/Cylinder.Cylinder')
PAWN = unreal.CollisionChannel.ECC_PAWN
IGNORE = unreal.CollisionResponseType.ECR_IGNORE


def mkitem(label, loc, mesh, scale, tooltag, display):
    a = by.get(label)
    if not a:
        a = eas.spawn_actor_from_class(PICKUP_CLS, unreal.Vector(loc[0], loc[1], loc[2]))
        a.set_actor_label(label)
    comp = a.get_editor_property('MeshComponent') or a.get_component_by_class(unreal.StaticMeshComponent)
    comp.set_mobility(unreal.ComponentMobility.MOVABLE)
    comp.set_static_mesh(mesh)
    comp.set_collision_response_to_channel(PAWN, IGNORE)
    a.set_actor_location(unreal.Vector(loc[0], loc[1], loc[2]), False, True)
    a.set_actor_scale3d(unreal.Vector(scale, scale, scale))
    a.set_editor_property('ItemSize', unreal.ItemSize.HEAVY)
    a.set_editor_property('ToolTag', tooltag)
    a.set_editor_property('Charges', 1)
    a.set_editor_property('DisplayName', display)
    a.set_editor_property('ItemEffect', unreal.ItemEffect.NONE)
    a.modify()
    out.append('item %s tooltag=%s heavy' % (label, tooltag))


def stage(kind, item_tag, duration, label):
    s = unreal.RepairStage()
    s.set_editor_property('kind', kind)
    s.set_editor_property('item_tag', unreal.Name(item_tag) if item_tag else unreal.Name('None'))
    s.set_editor_property('duration', duration)
    try:
        s.set_editor_property('label', label)
    except Exception:
        pass
    return s


HH = unreal.RepairStageKind.HOLD_HAND
HT = unreal.RepairStageKind.HOLD_TOOL
II = unreal.RepairStageKind.INSERT_ITEM

# --- ToolTags расходников/инструментов ---
# Сварочник
w = by.get('WeldingMachine')
if w:
    try:
        w.set_editor_property('ToolTag', unreal.Name('Welder')); w.modify()
        out.append('WeldingMachine ToolTag=Welder')
    except Exception as e:
        out.append('welder tag err %s' % e)
# Предохранитель: CDO + инстансы -> ToolTag "Fuse"
bp = unreal.EditorAssetLibrary.load_asset('/Game/Avariika/Items/BP_Fuse')
if bp:
    try:
        cdo = unreal.get_default_object(bp.generated_class())
        cdo.set_editor_property('ToolTag', unreal.Name('Fuse'))
        bp.modify(); unreal.EditorAssetLibrary.save_loaded_asset(bp)
    except Exception as e:
        out.append('fuse cdo err %s' % e)
for a in eas.get_all_level_actors():
    if a.get_class().get_name() == 'BP_Fuse_C':
        try:
            a.set_editor_property('ToolTag', unreal.Name('Fuse')); a.modify()
        except Exception:
            pass

# --- новые тяжёлые предметы ---
mkitem('Cable', (470.0, 250.0, 250.0), CYL, 0.4, 'Cable', 'Кабель')
mkitem('FuelCanister', (470.0, -230.0, 250.0), CUBE, 0.3, 'Fuel', 'Канистра с бензином')

# --- этапы на ремонтируемых ---
pipe = by.get('Repairable_GasPipe')
if pipe:
    pipe.set_editor_property('PrereqStages', [stage(HT, 'Welder', 4.0, 'Заварить трубу сваркой')])
    pipe.modify(); out.append('GasPipe: weld -> valve')

gen = by.get('Repairable_Generator')
if gen:
    gen.set_editor_property('RequiredTool', unreal.Name('None'))  # стартер руками после этапов
    gen.set_editor_property('PrereqStages', [
        stage(HH, '', 4.0, 'Починить корпус руками'),
        stage(II, 'Fuel', 0.0, 'Залить бензин (канистра)')])
    gen.modify(); out.append('Generator: hand -> fuel -> starter (RequiredTool=None)')

brk = by.get('Repairable_Breaker')
if brk:
    brk.set_editor_property('PrereqStages', [
        stage(II, 'Cable', 0.0, 'Подключить кабель'),
        stage(II, 'Fuse', 0.0, 'Вставить предохранитель')])
    brk.modify(); out.append('Breaker: cable -> fuse -> tester(Cursor)')

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'C:\unrealEngine\avariika\Saved\config_repair_stages.txt', 'w', encoding='utf-8').write('\n'.join(out))
