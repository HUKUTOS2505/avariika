# -*- coding: utf-8 -*-
"""Карта «Больница» — greybox проход 3: перегородки комнат. ТЗ §3.

Делит южную (Y 0..COR_S) и северную (Y COR_N..фасад) полосы на 6-м комнаты
поперечными перегородками (вдоль Y) по каждой оси 2..10. Доступ в комнаты —
через уже существующие дверные проёмы в стенах коридора (module_gaps), поэтому
перегородки сплошные и софтлоков не создают.

Идемпотентно по префиксу GB_Part. Свет и каркас (другие GB_) не трогает.
12-м комнаты ТЗ (морг, котельная, вестибюль, столовая, венткамера, операционная,
реанимация, кабинет главврача) в greybox остаются двумя 6-м ячейками — объединю
в проходе детализации (модульный кит). Внутрикомнатные двери (вестибюль↔гардероб
и т.п.) — тоже на детализацию.

Запуск headless: UnrealEditor-Cmd <uproj> -run=pythonscript -script="<абс путь>".
"""
import unreal

LEVEL = '/Game/Hospital/Maps/L_Hospital'
CUBE = unreal.load_asset('/Engine/BasicShapes/Cube')

LEN_X = 6000.0
DEP_Y = 1600.0
EXT = 50.0
COR_S = 600.0
COR_N = 900.0
MOD = 600.0
PART = 20.0  # толщина внутренней перегородки (ТЗ §1)

FLOORS = [('B', -290.0, 290.0), ('F1', 0.0, 350.0), ('F2', 350.0, 350.0), ('F3', 700.0, 350.0)]

# Лифтовая шахта уже обнесена стенами (GB_LiftW/E) на X=3000/3600 — там север не дублируем.
LIFT_X = (3000.0, 3600.0)

AXES = [MOD * i for i in range(1, 10)]  # 600,1200,...,5400

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []

les.load_level(LEVEL)

n = 0
for a in list(eas.get_all_level_actors()):
    lbl = a.get_actor_label()
    if lbl.startswith('GB_Part') or lbl == 'GB_Probe':
        eas.destroy_actor(a)
        n += 1
out.append('удалено GB_Part/Probe: %d' % n)


def box(name, cx, cy, cz, sx, sy, sz):
    # spawn_actor_from_object падает headless (EditorFramework). from_class + set_mesh — ок.
    a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(cx, cy, cz), unreal.Rotator(0.0, 0.0, 0.0))
    a.set_actor_label(name)
    a.static_mesh_component.set_static_mesh(CUBE)
    a.set_actor_scale3d(unreal.Vector(sx / 100.0, sy / 100.0, sz / 100.0))
    return a


# Южная полоса: Y 0..COR_S ; Северная полоса: Y COR_N..(DEP_Y-EXT)
south_cy = COR_S / 2.0
south_sy = COR_S
north_y0 = COR_N
north_y1 = DEP_Y - EXT
north_cy = (north_y0 + north_y1) / 2.0
north_sy = north_y1 - north_y0

made = 0
for nm, z, h in FLOORS:
    cz = z + h / 2.0
    for x in AXES:
        # Юг — всегда (лестниц/шахт нет, оси В-Г северные)
        box('GB_PartS_%s_%d' % (nm, int(x)), x, south_cy, cz, PART, south_sy, h)
        made += 1
        # Север — кроме осей лифтовой шахты (там уже GB_LiftW/E)
        if x in LIFT_X:
            continue
        box('GB_PartN_%s_%d' % (nm, int(x)), x, north_cy, cz, PART, north_sy, h)
        made += 1

out.append('перегородок добавлено: %d' % made)

les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('сохранено')

with open(r'D:\unrealEngine\avariika\Saved\hospital_rooms_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
