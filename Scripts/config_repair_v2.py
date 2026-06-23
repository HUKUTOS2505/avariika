# -*- coding: utf-8 -*-
"""Доводка ремонта: отключить колхоз; разнести станции шире; щиток ровно на стене;
добавить КЛЮЧ (ключ для трубы после заварки); расставить инструменты у станций."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
FLOOR = 210.0
PAWN = unreal.CollisionChannel.ECC_PAWN
IGNORE = unreal.CollisionResponseType.ECR_IGNORE
CYL = unreal.load_asset('/Engine/BasicShapes/Cylinder.Cylinder')


def movable(a):
    c = a.get_component_by_class(unreal.StaticMeshComponent)
    if c:
        try: c.set_mobility(unreal.ComponentMobility.MOVABLE)
        except Exception: pass


def place(label, x, y, z, yaw):
    a = by.get(label)
    if not a:
        out.append('%s НЕТ' % label); return None
    movable(a)
    a.set_actor_location(unreal.Vector(x, y, z), False, True)
    a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=yaw, roll=0.0), False)
    a.modify()
    return a


# 1) отключить колхоз на всех ремонтируемых
for a in eas.get_all_level_actors():
    if a.get_class().get_name() == 'Repairable':
        a.set_editor_property('bAllowBotch', False)
        a.modify()
out.append('botch disabled on all repairables')

# 2) станции — широко по дуге (пол 210)
place('Repairable_Generator', 600.0, -550.0, FLOOR + 100.0, 90.0)   # генератор (пивот -100)
place('FuelCanister',        520.0, -560.0, FLOOR + 40.0, 0.0)       # канистра рядом

place('Repairable_GasPipe',  600.0, 40.0, FLOOR, 0.0)               # труба (пивот в основании)
place('WeldingMachine',      520.0, 40.0, FLOOR + 40.0, 0.0)        # сварочник рядом (заварка)

# 3) щиток на стене — стена грунтуется на пол, щиток на её передней грани
wall = by.get('RepairWall')
if wall:
    movable(wall)
    wall.set_actor_location(unreal.Vector(720.0, 650.0, FLOOR + 195.0), False, True)
    wall.set_actor_scale3d(unreal.Vector(0.3, 2.5, 4.1))  # 30 x 250 x 410 -> низ у пола
    wall.modify()
place('Repairable_Breaker', 700.0, 650.0, FLOOR + 95.0, 180.0)      # на передней грани, лицом к игроку
place('Cable',  520.0, 650.0, FLOOR + 40.0, 0.0)
place('Fuse',   560.0, 690.0, FLOOR + 40.0, 0.0)
place('Tester', 600.0, 620.0, FLOOR + 40.0, 0.0)
out.append('stations spread + breaker on wall')

# 4) КЛЮЧ для трубы (искать) + труба требует ключ на этапе вентиля
wr = by.get('Wrench')
if not wr:
    cls = unreal.load_class(None, '/Script/Avaryo.PickupItem')
    wr = eas.spawn_actor_from_class(cls, unreal.Vector(150.0, 950.0, FLOOR + 30.0))
    wr.set_actor_label('Wrench')
comp = wr.get_editor_property('MeshComponent') or wr.get_component_by_class(unreal.StaticMeshComponent)
comp.set_mobility(unreal.ComponentMobility.MOVABLE)
comp.set_static_mesh(CYL)
comp.set_collision_response_to_channel(PAWN, IGNORE)
wr.set_actor_location(unreal.Vector(150.0, 950.0, FLOOR + 30.0), False, True)
wr.set_actor_scale3d(unreal.Vector(0.25, 0.25, 0.25))
wr.set_editor_property('ItemSize', unreal.ItemSize.LIGHT)
wr.set_editor_property('ToolTag', unreal.Name('Wrench'))
wr.set_editor_property('Charges', -1)
wr.set_editor_property('DisplayName', 'Разводной ключ')
wr.set_editor_property('ItemEffect', unreal.ItemEffect.NONE)
wr.modify()
out.append('Wrench created at (150,950) — искать')

pipe = by.get('Repairable_GasPipe')
if pipe:
    pipe.set_editor_property('RequiredTool', unreal.Name('Wrench'))  # вентиль крутится ключом (после заварки)
    pipe.modify()
    out.append('GasPipe RequiredTool=Wrench (валве нужен ключ)')

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'C:\unrealEngine\avariika\Saved\config_repair_v2.txt', 'w', encoding='utf-8').write('\n'.join(out))
