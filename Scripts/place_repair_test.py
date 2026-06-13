# -*- coding: utf-8 -*-
"""Удобная тест-расстановка ремонта: 3 станции по дуге перед спавном (пол z=210).
Генератор+сварочник | труба | щиток на стене+тестер+предохранитель. Мини-игры уже настроены."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
FLOOR = 210.0
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}


def movable(a):
    c = a.get_component_by_class(unreal.StaticMeshComponent)
    if c:
        try:
            c.set_mobility(unreal.ComponentMobility.MOVABLE)
        except Exception:
            pass


def place(label, x, y, z, yaw):
    a = by.get(label)
    if not a:
        out.append('%s НЕТ' % label); return
    movable(a)
    a.set_actor_location(unreal.Vector(x, y, z), False, True)
    a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=yaw, roll=0.0), False)
    a.modify()
    out.append('%s -> (%.0f,%.0f,%.0f) yaw=%.0f' % (label, x, y, z, yaw))


# --- станция ГЕНЕРАТОР (нужен сварочник) ---
place('Repairable_Generator', 470.0, -320.0, FLOOR + 100.0, 90.0)  # база на пол (пивот -100)
place('WeldingMachine', 470.0, -200.0, FLOOR + 30.0, 0.0)          # сварочник рядом

# --- станция ТРУБА (руками) ---
place('Repairable_GasPipe', 470.0, 30.0, FLOOR, 0.0)               # пивот в основании

# --- станция ЩИТОК на стене (нужен тестер + предохранители) ---
# стена за щитком
wall = by.get('RepairWall')
if not wall:
    wall = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, 0))
    wall.set_actor_label('RepairWall')
    cube = unreal.load_asset('/Engine/BasicShapes/Cube.Cube')
    wall.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    wall.static_mesh_component.set_static_mesh(cube)
wall.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
wall.set_actor_location(unreal.Vector(560.0, 360.0, FLOOR + 200.0), False, True)
wall.set_actor_scale3d(unreal.Vector(0.3, 3.0, 4.0))  # 30x300x400 см
out.append('RepairWall at (560,360) 30x300x400')

place('Repairable_Breaker', 535.0, 360.0, FLOOR + 95.0, 180.0)     # на стене, лицом к игроку (-X)
place('Tester', 440.0, 330.0, FLOOR + 40.0, 0.0)                   # тестер рядом
place('Fuse', 470.0, 400.0, FLOOR + 40.0, 0.0)                     # предохранитель рядом

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\place_repair_test.txt', 'w', encoding='utf-8').write('\n'.join(out))
