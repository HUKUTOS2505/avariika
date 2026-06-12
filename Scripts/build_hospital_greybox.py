# -*- coding: utf-8 -*-
"""Карта «Больница» — greybox (проходы 1+2). ТЗ TZ_Hospital_Map_UE5.md §1,2,8.
П1: каркас (перекрытия, наружные стены + проём входа, стены коридора, парапет).
П2: вертикали — проёмы в перекрытиях под ЛК-А/ЛК-Б/лифт, рампы между этажами,
двери-проёмы в стенах коридора (по модулю 6 м), стены шахты лифта, PlayerStart.
Комнаты/пропсы/свет — следующими проходами.

Запуск из ОТКРЫТОГО редактора (Claudius editor.run_python_script). new_level в
commandlet крашит. Идемпотентно: удаляет свои GB_ перед сборкой.
"""
import math
import unreal

LEVEL = '/Game/Hospital/Maps/L_Hospital'
CUBE = unreal.load_asset('/Engine/BasicShapes/Cube')

LEN_X = 6000.0
DEP_Y = 1600.0
EXT = 50.0
COR_S = 600.0     # ось Б (юж. стена коридора)
COR_N = 900.0     # ось В (сев. стена коридора)
ENTR_X = 3000.0
ENTR_W = 200.0
DOOR = 120.0      # ширина дверного проёма
MOD = 600.0       # модуль 6 м

# Вертикали: X-диапазоны проёмов (Y 900..1500, северная половина)
LKA = (2400.0, 3000.0)   # ЛК-А (оси 5-6)
LIFT = (3000.0, 3600.0)  # лифт (оси 6-7)
LKB = (5400.0, 6000.0)   # ЛК-Б (оси 10-11)

FLOORS = [('B', -290.0, 290.0), ('F1', 0.0, 350.0), ('F2', 350.0, 350.0), ('F3', 700.0, 350.0)]
ROOF_Z = 1050.0

# Проёмы в перекрытии (спускаться сквозь пол этого этажа):
SLAB_HOLES = {
    'B': [],                 # подвал — пол сплошной (стоим на нём)
    'F1': [LKB],             # вниз в подвал по ЛК-Б
    'F2': [LKA, LKB],        # вниз по обеим
    'F3': [LKA, LKB],
}

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []

if unreal.EditorAssetLibrary.does_asset_exist(LEVEL):
    les.load_level(LEVEL)
else:
    les.new_level(LEVEL); out.append('создан ' + LEVEL)

n = 0
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label().startswith('GB_'):
        eas.destroy_actor(a); n += 1
out.append('удалено GB_: %d' % n)


def box(name, cx, cy, cz, sx, sy, sz, roll=0.0):
    # from_class + set_mesh работает headless; spawn_actor_from_object падает (EditorFramework AV).
    a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(cx, cy, cz), unreal.Rotator(0.0, 0.0, roll))
    a.set_actor_label(name)
    a.static_mesh_component.set_static_mesh(CUBE)
    a.set_actor_scale3d(unreal.Vector(sx / 100.0, sy / 100.0, sz / 100.0))
    return a


def xwall(prefix, yfixed, z, h, thick, gaps):
    """Стена вдоль X (0..LEN_X) на Y=yfixed, с дверными проёмами gaps=[(x0,x1)...]."""
    edges = [0.0]
    for g0, g1 in sorted(gaps):
        edges += [g0, g1]
    edges += [LEN_X]
    seg = 0
    for i in range(0, len(edges) - 1, 2):
        x0, x1 = edges[i], edges[i + 1]
        if x1 - x0 < 1.0:
            continue
        box('%s_%d' % (prefix, seg), (x0 + x1) / 2, yfixed, z + h / 2, x1 - x0, thick, h)
        seg += 1


def module_gaps():
    """Дверной проём в центре каждого 6м-модуля по X."""
    return [(MOD * i + MOD / 2 - DOOR / 2, MOD * i + MOD / 2 + DOOR / 2) for i in range(int(LEN_X / MOD))]


def slab(nm, z, holes):
    # Южная половина (Y 0..COR_N) — сплошная
    box('GB_SlabS_' + nm, LEN_X / 2, COR_N / 2, z - EXT / 2, LEN_X, COR_N, EXT)
    # Северная половина (Y COR_N..DEP_Y) — сегменты вокруг проёмов
    edges = [0.0]
    for h0, h1 in sorted(holes):
        edges += [h0, h1]
    edges += [LEN_X]
    seg = 0
    for i in range(0, len(edges) - 1, 2):
        x0, x1 = edges[i], edges[i + 1]
        if x1 - x0 < 1.0:
            continue
        box('GB_SlabN_%s_%d' % (nm, seg), (x0 + x1) / 2, (COR_N + DEP_Y) / 2, z - EXT / 2, x1 - x0, DEP_Y - COR_N, EXT)
        seg += 1


# --- Перекрытия ---
for nm, z, h in FLOORS:
    slab(nm, z, SLAB_HOLES[nm])
box('GB_SlabRoof', LEN_X / 2, DEP_Y / 2, ROOF_Z - EXT / 2, LEN_X, DEP_Y, EXT)

# --- Стены по этажам ---
gaps = module_gaps()
for nm, z, h in FLOORS:
    box('GB_WallN_' + nm, LEN_X / 2, DEP_Y - EXT / 2, z + h / 2, LEN_X, EXT, h)
    box('GB_WallW_' + nm, EXT / 2, DEP_Y / 2, z + h / 2, EXT, DEP_Y, h)
    box('GB_WallE_' + nm, LEN_X - EXT / 2, DEP_Y / 2, z + h / 2, EXT, DEP_Y, h)
    if nm == 'F1':
        lw = ENTR_X - ENTR_W / 2
        rw = LEN_X - (ENTR_X + ENTR_W / 2)
        box('GB_WallS_F1a', lw / 2, EXT / 2, z + h / 2, lw, EXT, h)
        box('GB_WallS_F1b', (ENTR_X + ENTR_W / 2) + rw / 2, EXT / 2, z + h / 2, rw, EXT, h)
    else:
        box('GB_WallS_' + nm, LEN_X / 2, EXT / 2, z + h / 2, LEN_X, EXT, h)
    # Стены коридора с проёмами (по модулю)
    xwall('GB_CorS_' + nm, COR_S, z, h, 20.0, gaps)
    xwall('GB_CorN_' + nm, COR_N, z, h, 20.0, gaps)

# --- Рампы (греybox-лестницы) ---
def ramp(prefix, xc, z_lo, z_hi):
    delta = z_hi - z_lo
    run = DEP_Y - COR_N            # 700 по Y (от COR_N до DEP_Y)
    length = math.sqrt(run * run + delta * delta)
    roll = math.degrees(math.atan2(delta, run))
    box(prefix, xc, (COR_N + DEP_Y) / 2, (z_lo + z_hi) / 2, 280.0, length, 20.0, roll=roll)

lka_c = (LKA[0] + LKA[1]) / 2
lkb_c = (LKB[0] + LKB[1]) / 2
ramp('GB_RampB_B_F1', lkb_c, -290.0, 0.0)
ramp('GB_RampB_F1_F2', lkb_c, 0.0, 350.0)
ramp('GB_RampB_F2_F3', lkb_c, 350.0, 700.0)
ramp('GB_RampA_F1_F2', lka_c, 0.0, 350.0)
ramp('GB_RampA_F2_F3', lka_c, 350.0, 700.0)

# --- Шахта лифта (декор, стены вокруг проёма лифта на 1..3) ---
for nm, z, h in FLOORS:
    if nm == 'B':
        continue
    box('GB_LiftW_' + nm, LIFT[0], (COR_N + DEP_Y) / 2, z + h / 2, 20.0, DEP_Y - COR_N, h)
    box('GB_LiftE_' + nm, LIFT[1], (COR_N + DEP_Y) / 2, z + h / 2, 20.0, DEP_Y - COR_N, h)

# --- Парапет ---
pz = ROOF_Z + 45
box('GB_ParN', LEN_X / 2, DEP_Y - EXT / 2, pz, LEN_X, EXT, 90.0)
box('GB_ParS', LEN_X / 2, EXT / 2, pz, LEN_X, EXT, 90.0)
box('GB_ParW', EXT / 2, DEP_Y / 2, pz, EXT, DEP_Y, 90.0)
box('GB_ParE', LEN_X - EXT / 2, DEP_Y / 2, pz, EXT, DEP_Y, 90.0)

# --- PlayerStart у входа ---
ps = eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(ENTR_X, -250.0, 100.0), unreal.Rotator(0.0, 90.0, 0.0))
ps.set_actor_label('GB_PlayerStart')

# Свет — рабочий дневной (видимость greybox; ночь по ТЗ §7 — отдельный проход).
# Movable-солнце 100000 lux + atmosphere sun + realtime SkyLight: без запекания,
# PIE сразу светлый. Значения проверены (иначе чёрный экран). Пересобираем всегда.
MOV = unreal.ComponentMobility.MOVABLE
for a in list(eas.get_all_level_actors()):
    if a.get_actor_label() in ('GB_Sun', 'GB_Sky', 'GB_Atmo', 'GB_Fog'):
        eas.destroy_actor(a)
sun = eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 2200), unreal.Rotator(-45, -45, 0))
sun.set_actor_label('GB_Sun')
sc = sun.get_component_by_class(unreal.DirectionalLightComponent)
sc.set_mobility(MOV)
sc.set_intensity(100000.0)
for p in ('atmosphere_sun_light', 'used_as_atmosphere_sun_light'):
    try:
        sc.set_editor_property(p, True)
    except Exception:
        pass
sky = eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 1800))
sky.set_actor_label('GB_Sky')
skc = sky.get_component_by_class(unreal.SkyLightComponent)
skc.set_mobility(MOV)
try:
    skc.set_editor_property('real_time_capture', True)
except Exception:
    pass
skc.set_intensity(3.0)
eas.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0)).set_actor_label('GB_Atmo')
eas.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0)).set_actor_label('GB_Fog')

les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('greybox П1+П2 готов, сохранён')

with open(r'D:\unrealEngine\avariika\Saved\hospital_greybox_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
