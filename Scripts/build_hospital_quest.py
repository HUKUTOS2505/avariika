# -*- coding: utf-8 -*-
"""Карта «Больница» — квест-акторы (маршрут §5 проходим в greybox).

Ставит то, что уже поддержано кодом: 3 ARepairable с типами мини-игр, ExitZone
(ГАЗель), инструменты (сварочник/тестер), рацию, парковочную площадку у входа и
правит PlayerStart. Ключи/двери/канистра топлива — это BP_QuestManager/BP_Door
(не реализовано), будет отдельным проходом; пока двери открыты (greybox).

Координаты больницы (см. build_hospital_greybox.py): ось N = X=(N-1)*600;
юг — Y~300 (полоса 0..600), север — Y~1200 (полоса 900..1500), коридор Y 600..900.
Пол: подвал z=-290, F1 z=0. Объекты — НЕ с префиксом GB_ (чтобы пересборка
каркаса их не сносила; но пересборка rooms/quest нужна после пересборки greybox).

Headless: UnrealEditor-Cmd <uproj> -run=pythonscript -script="<абс путь>".
"""
import unreal

LEVEL = '/Game/Hospital/Maps/L_Hospital'
CUBE = unreal.load_asset('/Engine/BasicShapes/Cube')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les.load_level(LEVEL)
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
out = []


def upsert(cls, label, loc, scale, rot=None):
    a = by_label.get(label)
    if a is None:
        a = eas.spawn_actor_from_class(cls, loc, rot or unreal.Rotator(0, 0, 0))
        a.set_actor_label(label)
    else:
        a.set_actor_location(loc, False, False)
        if rot is not None:
            a.set_actor_rotation(rot, False)
    a.set_actor_scale3d(scale)
    by_label[label] = a
    return a


def repairable(label, name, loc, scale, duration, mtype, tool, leaks=False):
    a = upsert(unreal.Repairable, label, loc, scale)
    a.get_editor_property('MeshComponent').set_static_mesh(CUBE)
    a.set_editor_property('DisplayName', name)
    a.set_editor_property('RepairDuration', duration)
    a.set_editor_property('MinigameType', mtype)
    a.set_editor_property('RequiredTool', tool)
    if leaks:
        a.set_editor_property('bLeaksGasWhenBroken', True)
    out.append('%s @ (%.0f,%.0f,%.0f) %s/%s' % (label, loc.x, loc.y, loc.z, mtype, tool))


def pickup(label, name, loc, scale, size, tool='None', effect=None, charges=-1):
    a = upsert(unreal.PickupItem, label, loc, scale)
    a.get_editor_property('MeshComponent').set_static_mesh(CUBE)
    a.set_editor_property('DisplayName', name)
    a.set_editor_property('ItemSize', size)
    a.set_editor_property('Charges', charges)
    if tool != 'None':
        a.set_editor_property('ToolTag', tool)
    if effect is not None:
        a.set_editor_property('ItemEffect', effect)
    out.append('%s @ (%.0f,%.0f,%.0f)' % (label, loc.x, loc.y, loc.z))


MT = unreal.RepairMinigameType
SZ = unreal.ItemSize

# --- Площадка-парковка у главного входа (ось 6, X=3000, юг). Снаружи пола нет. ---
# Куб 100 см: верх на z=0 (вровень с F1) при center z=-25, scale.z=0.5.
upsert(unreal.StaticMeshActor, 'H_ParkPad', unreal.Vector(3000, -400, -25),
       unreal.Vector(12.0, 8.0, 0.5)).static_mesh_component.set_static_mesh(CUBE)
out.append('H_ParkPad')

# --- PlayerStart: на площадку, лицом в здание (+Y) ---
ps = None
for a in eas.get_all_level_actors():
    if isinstance(a, unreal.PlayerStart):
        ps = a
        break
if ps is None:
    ps = eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(3000, -600, 100), unreal.Rotator(0, 90, 0))
    ps.set_actor_label('H_PlayerStart')
else:
    ps.set_actor_location(unreal.Vector(3000, -600, 100), False, False)
    ps.set_actor_rotation(unreal.Rotator(0, 90, 0), False)
out.append('PlayerStart @ (3000,-600,100) -> %s' % ps.get_actor_label())

# --- ГАЗель/зона выхода на площадке (старт=финиш) ---
upsert(unreal.ExitZone, 'ExitZone_Gazelle', unreal.Vector(3000, -200, 80), unreal.Vector(3.0, 3.0, 1.5))
out.append('ExitZone_Gazelle')

# --- Ремонтируемые (подвал, z=-290 пол) ---
# Генераторная: оси 9-10 север (X 4800-5400), центр 5100
repairable('Repairable_Generator', 'Генератор', unreal.Vector(5100, 1200, -240),
           unreal.Vector(1.5, 1.0, 1.0), 10.0, MT.STARTER, 'Welder')
# ГРЩ: оси 8-9 север (X 4200-4800), центр 4500
repairable('Repairable_Breaker', 'Электрощиток (ГРЩ)', unreal.Vector(4500, 1200, -215),
           unreal.Vector(0.35, 1.0, 1.5), 6.0, MT.CURSOR, 'Tester')
# Газовый узел: оси 4-5 север (X 1800-2400), центр 2100 — травит газ
repairable('Repairable_GasPipe', 'Газовая труба', unreal.Vector(2100, 1200, -180),
           unreal.Vector(0.35, 0.35, 2.2), 8.0, MT.VALVE, 'None', leaks=True)

# --- Инструменты в мастерской (оси 7-8 юг, X 3600-4200, центр 3900) ---
pickup('Welder', 'Сварочник', unreal.Vector(3850, 300, -270), unreal.Vector(0.3, 0.2, 0.4), SZ.HEAVY, tool='Welder')
pickup('Tester', 'Тестер', unreal.Vector(3960, 300, -283), unreal.Vector(0.2, 0.12, 0.05), SZ.LIGHT, tool='Tester')

# --- Рация на площадке у старта ---
pickup('Radio', 'Рация', unreal.Vector(3000, -520, 80), unreal.Vector(0.18, 0.12, 0.3),
       SZ.LIGHT, effect=unreal.ItemEffect.RADIO)

les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('сохранено')

with open(r'D:\unrealEngine\avariika\Saved\hospital_quest_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
