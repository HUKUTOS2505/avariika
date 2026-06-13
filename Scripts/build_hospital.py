# -*- coding: utf-8 -*-
"""L_Hospital — Этап 1 (greybox вертикального среза) по TZ_Hospital_Map_UE5_v2.md.
Строит: центральный корпус + АТРИУМ (3 света, парадная лестница, балконы, колонны,
световой фонарь), внутренний двор (ограда/ворота/фонтан/сторожка/биотуалеты/траншея),
западное крыло 1 этаж (кухня+газовый стояк, столовая, ЛК-З) и западный ПОДВАЛ-маршрут
(газовый узел, котельная, генераторная+приямок, ГРЩ, мастерская). Дневной свет для осмотра
(ночь/атмосфера — Этап 5). Координаты в UU (1 см), угол здания в (0,0,0).

Запуск: Claudius editor.run_python_script (открытый редактор). Создаёт уровень сам
через new_level (активный, без load_level-detach). Идемпотентность: при повторе чистит
актеры с тегом 'hosp' в активном L_Hospital. Отчёт -> Saved/build_hospital.txt.
"""
import unreal

LEVEL = '/Game/Hospital/L_Hospital'
out = []

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
CUBE = unreal.load_asset('/Engine/BasicShapes/Cube.Cube')
HOSP_TAG = 'hosp'

# --- создать/открыть уровень: всегда строим в СВЕЖИЙ new_level (надёжно активный,
# сохраняется; load_level существующего в открытом редакторе даёт detached-мир) ---
if unreal.EditorAssetLibrary.does_asset_exist(LEVEL):
    les.load_level('/Game/FirstPerson/Lvl_FirstPerson')  # увести активный с L_Hospital
    unreal.EditorAssetLibrary.delete_asset(LEVEL)
les.new_level(LEVEL)
out.append('level created fresh: ' + LEVEL)

# ---------- хелперы ----------
def tag(a):
    if a:
        a.tags = [HOSP_TAG]
    return a

def box(name, x0, y0, z0, x1, y1, z1):
    """Коробка по двум углам (min..max). Cube=100uu, пивот в центре."""
    cx, cy, cz = (x0 + x1) * 0.5, (y0 + y1) * 0.5, (z0 + z1) * 0.5
    a = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(cx, cy, cz))
    a.static_mesh_component.set_static_mesh(CUBE)
    a.set_actor_scale3d(unreal.Vector(max(abs(x1 - x0), 1) / 100.0,
                                      max(abs(y1 - y0), 1) / 100.0,
                                      max(abs(z1 - z0), 1) / 100.0))
    a.set_actor_label(name)
    return tag(a)

DOOR_H = 210
def wall_x(name, y, t, x0, x1, zb, zt, doors=None):
    """Стена вдоль X (тонкая по Y, центр по y, толщина t), с проёмами doors=[(dx0,dx1),...]."""
    doors = sorted(doors or [])
    segs = []
    cx = x0
    for dx0, dx1 in doors:
        if dx0 > cx:
            segs.append((cx, dx0))
        cx = max(cx, dx1)
    if cx < x1:
        segs.append((cx, x1))
    i = 0
    for sx0, sx1 in segs:
        box('%s_%d' % (name, i), sx0, y - t * 0.5, zb, sx1, y + t * 0.5, zt)
        i += 1
    # перемычки над проёмами
    for dx0, dx1 in doors:
        if zt > zb + DOOR_H:
            box('%s_lnt%d' % (name, i), dx0, y - t * 0.5, zb + DOOR_H, dx1, y + t * 0.5, zt)
            i += 1

def wall_y(name, x, t, y0, y1, zb, zt, doors=None):
    """Стена вдоль Y (тонкая по X), с проёмами doors=[(dy0,dy1),...]."""
    doors = sorted(doors or [])
    segs = []
    cy = y0
    for dy0, dy1 in doors:
        if dy0 > cy:
            segs.append((cy, dy0))
        cy = max(cy, dy1)
    if cy < y1:
        segs.append((cy, y1))
    i = 0
    for sy0, sy1 in segs:
        box('%s_%d' % (name, i), x - t * 0.5, sy0, zb, x + t * 0.5, sy1, zt)
        i += 1
    for dy0, dy1 in doors:
        if zt > zb + DOOR_H:
            box('%s_lnt%d' % (name, i), x - t * 0.5, dy0, zb + DOOR_H, x + t * 0.5, dy1, zt)
            i += 1

def slab(name, x0, y0, x1, y1, ztop, th=50):
    box(name, x0, y0, ztop - th, x1, y1, ztop)

def stair_y(name, x0, x1, y_lo, y_hi, z_lo, z_hi, n=12, up_to_north=True):
    """Лестница: поднимается по Y от (y_lo,z_lo) к (y_hi,z_hi). Ступени-коробки."""
    dy = (y_hi - y_lo) / n
    dz = (z_hi - z_lo) / n
    for i in range(n):
        sy0 = y_lo + dy * i
        if up_to_north:
            box('%s_%d' % (name, i), x0, sy0, z_lo, x1, sy0 + dy + 2, z_lo + dz * (i + 1))
        else:
            box('%s_%d' % (name, i), x0, sy0, z_lo, x1, sy0 + dy + 2, z_hi - dz * i)

def column(name, x, y, zb, zt, r=35):
    box(name, x - r, y - r, zb, x + r, y + r, zt)

# уровни Z
BZ = -290   # пол подвала
B_CEIL = -50
F1 = 0
F2 = 350
F3 = 700
ATR_TOP = 1050

# ================= ЦЕНТРАЛЬНЫЙ КОРПУС  X0..5600  Y0..1400 =================
# Наземная плита портика/парковки у южного фасада (старт: ГАЗель + PlayerStart стоят НА ней)
slab('Portico_Ground', 800, -1400, 4800, 0, F1)
# Пол 1 этажа (полный)
slab('C_FloorB', 0, 0, 5600, 1400, F1)          # верх=0
slab('C_FloorBase', 0, 0, 5600, 1400, BZ)        # пол подвала под центром
# Атриум-пустота: дыра в перекрытиях 2/3 этажей. Void = X2350..3250, Y250..1150.
VX0, VX1, VY0, VY1 = 2350, 3250, 250, 1150
for nm, zt in (('C_Floor2', F2), ('C_Floor3', F3)):
    slab(nm + '_S', 0, 0, 5600, VY0, zt)            # юг от пустоты
    slab(nm + '_N', 0, VY1, 5600, 1400, zt)         # север
    slab(nm + '_W', 0, VY0, VX0, VY1, zt)           # запад
    slab(nm + '_E', VX1, VY0, 5600, VY1, zt)        # восток
# Крыша центрального корпуса с ПРОЁМОМ светового фонаря над пустотой атриума (свет внутрь)
slab('C_Roof_S', 0, 0, 5600, VY0, ATR_TOP)
slab('C_Roof_N', 0, VY1, 5600, 1400, ATR_TOP)
slab('C_Roof_W', 0, VY0, VX0, VY1, ATR_TOP)
slab('C_Roof_E', VX1, VY0, 5600, VY1, ATR_TOP)

# Наружные стены центрального корпуса (3 этажа). Гл.вход — проём в южной стене у атриума.
ENT = (2600, 3000)   # парадный вход (юг)
CY_DOORS = [(2150, 2300), (3200, 3350)]  # двери в двор за лестницей (север)
for (zb, zt) in ((F1, F2), (F2, F3), (F3, ATR_TOP)):
    wall_x('C_SWall_%d' % zb, 0, 50, 0, 5600, zb, zt, doors=[ENT] if zb == F1 else None)
    wall_x('C_NWall_%d' % zb, 1400, 50, 0, 5600, zb, zt, doors=CY_DOORS if zb == F1 else None)
    wall_y('C_WWall_%d' % zb, 0, 50, 0, 1400, zb, zt)
    wall_y('C_EWall_%d' % zb, 5600, 50, 0, 1400, zb, zt)

# ---------- АТРИУМ  X2100..3500 / Y0..1400 (§3.1) ----------
# Шахматный пол атриума — просто слэб (материал позже). Колонны 2 ряда по 3 (сетка 7м).
for cx in (2450, 2800, 3150):
    for cy in (450, 1050):
        column('Atr_Col_%d_%d' % (cx, cy), cx, cy, F1, ATR_TOP - 50)
# Парадная лестница: центральный марш F1->площадка, затем балкон 2 эт.
# Центральный марш (север, у Y1400) спускается на юг к площадке Z175 @ Y~1000
stair_y('Atr_StairMain', 2600, 3000, 1000, 1380, F1, 175, n=10)
slab('Atr_Landing', 2550, 950, 3050, 1050, 175)                 # площадка
# Два боковых марша с площадки к балкону F2 (по краям атриума)
stair_y('Atr_StairW', 2150, 2340, 600, 1000, 175, F2, n=10)
stair_y('Atr_StairE', 3260, 3450, 600, 1000, 175, F2, n=10)
# Балюстрады балконов (низкие парапеты по краю пустоты F2/F3)
for zt in (F2, F3):
    box('Atr_BalS_%d' % zt, VX0, VY0 - 10, zt, VX1, VY0 + 10, zt + 100)
    box('Atr_BalN_%d' % zt, VX0, VY1 - 10, zt, VX1, VY1 + 10, zt + 100)
    box('Atr_BalW_%d' % zt, VX0 - 10, VY0, zt, VX0 + 10, VY1, zt + 100)
    box('Atr_BalE_%d' % zt, VX1 - 10, VY0, zt, VX1 + 10, VY1, zt + 100)
# Световой фонарь = открытый проём в крыше (стекло/рамы — модульный кит, Этап 3).
# Тонкая рамка по краю проёма, чтобы читался как фонарь, но свет проходит.
box('Atr_SkylightFrameW', VX0 - 20, VY0, ATR_TOP, VX0 + 20, VY1, ATR_TOP + 30)
box('Atr_SkylightFrameE', VX1 - 20, VY0, ATR_TOP, VX1 + 20, VY1, ATR_TOP + 30)
# Стойка регистратуры (слева от входа)
box('Atr_Reception', 2150, 200, F1, 2500, 320, F1 + 110)

# ---------- Перегородки 1 этажа центрального корпуса + галерея ----------
# Галерея-коридор вдоль севера Y1100..1400 (двери комнат в галерею, двери в двор — выше)
wall_x('C_GalWall', 1100, 20, 0, 2100, F1, F2, doors=[(300, 450), (1000, 1150), (1700, 1850)])
wall_x('C_GalWallE', 1100, 20, 3500, 5600, F1, F2, doors=[(3800, 3950), (4500, 4650), (5200, 5350)])
# Поперечные перегородки комнат 1 этажа (физио|аптека|рег | атриум | гардероб|охрана|приёмный)
for px in (700, 1400, 2100, 3500, 4200, 4900):
    wall_y('C_P1_%d' % px, px, 20, 0, 1100, F1, F2, doors=[(500, 650)])

# ================= ВНУТРЕННИЙ ДВОР  X1400..4200 / Y1400..4200 (§3.2) =================
slab('Yard_Ground', 1400, 1400, 4200, 4200, F1)
# Ограда по северу (2 м) с воротами
wall_x('Yard_NFence', 4200, 40, 1400, 4200, F1, F1 + 200, doors=[(2700, 3100)])
wall_y('Yard_WFence', 1400, 40, 1400, 4200, F1, F1 + 200)   # запад двора (стена крыла снаружи)
wall_y('Yard_EFence', 4200, 40, 1400, 4200, F1, F1 + 200)
# Сухой фонтан (центр двора)
box('Yard_Fountain', 2700, 2700, F1, 2900, 2900, F1 + 60)
# Сторожка 3x4 у ворот (внутри ключ ПОДВАЛ)
wall_x('Booth_S', 3300, 20, 3300, 3700, F1, F1 + 250, doors=[(3450, 3550)])
wall_x('Booth_N', 3700, 20, 3300, 3700, F1, F1 + 250)
wall_y('Booth_W', 3300, 20, 3300, 3700, F1, F1 + 250)
wall_y('Booth_E', 3700, 20, 3300, 3700, F1, F1 + 250)
box('Booth_KeyPODVAL', 3500, 3600, F1 + 90, 3560, 3660, F1 + 130)  # ключ-заглушка
# Траншея «ремонт труб» у западной стены двора (лента) + 2 биотуалета
box('Yard_Trench', 1500, 2000, F1 - 60, 1700, 3000, F1)
# (биотуалеты SM_Toilet ставятся ниже как пропсы)

# ================= ЗАПАДНОЕ КРЫЛО 1 ЭТАЖ  X0..1400 / Y1400..4200 (§4.1) =================
slab('W1_Floor', 0, 1400, 1400, 4200, F1)
# наружные стены крыла (запад, север-торец; восток граничит с двором — стена Yard_WFence)
wall_y('W1_WWall', 0, 50, 1400, 4200, F1, F2)
wall_x('W1_NWall', 4200, 50, 0, 1400, F1, F2)
# Кухня Y1400..2100 (во всю ширину) — газовый стояк; проход из центр.коридора (юг, Y1400)
wall_x('W1_KitchenS', 1400, 30, 0, 1400, F1, F2, doors=[(600, 750)])  # связь с центр.корпусом
box('W1_GasRiser', 200, 1500, F1, 320, 1620, F1 + 260)               # газовый стояк кухни (целый)
# Столовая Y2100..3150 (окна в двор — проёмы в восточной стене крыла = Yard_WFence уже с дверью? добавим окна)
wall_x('W1_DiningS', 2100, 20, 0, 1400, F1, F2, doors=[(550, 850)])  # кухня->столовая (двойная)
wall_x('W1_PantryS', 3150, 20, 0, 1400, F1, F2, doors=[(600, 750)])  # столовая->кладовая
wall_x('W1_LKZ_S', 3500, 20, 0, 700, F1, F2, doors=[(250, 400)])     # вход в ЛК-З
# ЛК-З (X0..700 / Y3500..4200): лестница 1 этаж -> подвал (маршрут квеста)
stair_y('LKZ_Down', 100, 600, 3550, 4150, F1, BZ + 50, n=12, up_to_north=False)
box('LKZ_HoleMark', 100, 3550, F1 - 5, 600, 4150, F1)  # проём в полу под лестницу (визуально)

# ================= ЗАПАДНЫЙ ПОДВАЛ  X0..1400 / Y1400..4200 (§4.4) =================
slab('B_W_Floor', 0, 1400, 1400, 4200, BZ)
wall_y('B_W_WWall', 0, 50, 1400, 4200, BZ, B_CEIL)
wall_y('B_W_EWall', 1400, 50, 1400, 4200, BZ, B_CEIL)
wall_x('B_W_NWall', 4200, 50, 0, 1400, BZ, B_CEIL)
wall_x('B_W_SWall', 1400, 50, 0, 1400, BZ, B_CEIL, doors=[(600, 750)])  # связь к центр.подвалу
slab('B_W_Ceil', 0, 1400, 1400, 4200, B_CEIL + 50)                      # потолок подвала (низкий)
# комнаты подвала (перегородки поперёк, двери)
#   газовый узел Y1400-2100 | котельная 2100-2800 | генераторная 2800-3500 | ГРЩ 3500-3850 | мастерская 3850-4200
wall_x('B_GasN', 2100, 20, 0, 1400, BZ, B_CEIL, doors=[(300, 450)])
wall_x('B_BoilerN', 2800, 20, 0, 1400, BZ, B_CEIL, doors=[(300, 450)])
wall_x('B_GenN', 3500, 20, 0, 1400, BZ, B_CEIL, doors=[(300, 450)])
wall_x('B_GRShN', 3850, 20, 0, 1400, BZ, B_CEIL, doors=[(300, 450)])    # ГРЩ-мастерская
# дверь генераторной в приямок (запад. фасад) — проём в наружной стене подвала
wall_y('B_W_WWall_priyamok', 0, 50, 2900, 3100, BZ, BZ + 210)          # (заглушка-проём отдельно не нужен)
box('B_Priyamok', -300, 2850, BZ, 0, 3150, F1)                          # приямок-колодец на улицу
box('B_CanisterSolyarka', 700, 2400, BZ, 820, 2520, BZ + 90)            # канистра солярки (котельная)
box('B_GRSh_Glass', 700, 3500, BZ, 720, 3850, BZ + 210)                 # армированное стекло двери ГРЩ (видно щиток)
box('B_Workbench', 200, 3950, BZ, 1200, 4080, BZ + 100)                 # верстак мастерской

# ---------- дневной свет для осмотра (атмосфера — Этап 5) ----------
try:
    sun = eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(2800, 2000, 2500))
    sun.set_actor_rotation(unreal.Rotator(-45.0, 35.0, 0.0), False)
    sunc = sun.get_component_by_class(unreal.DirectionalLightComponent)
    sunc.set_mobility(unreal.ComponentMobility.MOVABLE)
    sunc.set_intensity(100000.0)
    try:
        sunc.set_editor_property('atmosphere_sun_light', True)
    except Exception as e:
        out.append('  sun atmosphere_sun_light? %s' % e)
    sun.set_actor_label('Hosp_Sun'); tag(sun)
    sky = eas.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(2800, 2000, 0))
    sky.set_actor_label('Hosp_SkyAtmo'); tag(sky)
    skl = eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(2800, 2000, 1500))
    sklc = skl.get_component_by_class(unreal.SkyLightComponent)
    sklc.set_mobility(unreal.ComponentMobility.MOVABLE)
    try:
        sklc.set_editor_property('real_time_capture', True)
    except Exception as e:
        out.append('  skylight real_time_capture? %s' % e)
    skl.set_actor_label('Hosp_SkyLight'); tag(skl)
    out.append('daylight ok')
except Exception as e:
    out.append('LIGHT FAIL: %s' % e)

# ---------- PostProcessVolume: зажать экспозицию (иначе авто-экспозиция уводит в чёрное) ----------
try:
    ppv = eas.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(2800, 2000, 200))
    ppv.set_editor_property('unbound', True)
    pset = ppv.get_editor_property('settings')
    pset.set_editor_property('b_override_auto_exposure_min_brightness', True)
    pset.set_editor_property('auto_exposure_min_brightness', 1.0)
    pset.set_editor_property('b_override_auto_exposure_max_brightness', True)
    pset.set_editor_property('auto_exposure_max_brightness', 1.0)
    ppv.set_editor_property('settings', pset)
    ppv.set_actor_label('Hosp_PPV'); tag(ppv)
    out.append('ppv ok')
except Exception as e:
    out.append('PPV FAIL: %s' % e)

# ---------- PlayerStart (под портиком, лицом в атриум, +Y) ----------
try:
    ps = eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(2800, -250, F1 + 100))
    ps.set_actor_rotation(unreal.Rotator(0.0, 90.0, 0.0), False)
    ps.set_actor_label('PlayerStart'); tag(ps)
    out.append('playerstart ok')
except Exception as e:
    out.append('PLAYERSTART FAIL: %s' % e)

# ---------- ГАЗель под портиком (старт/финиш) + ExitZone ----------
try:
    gz = unreal.load_asset('/Game/Avariika/Meshes/SM_Gazelle.SM_Gazelle')
    if gz:
        g = eas.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(2800, -550, F1))
        g.static_mesh_component.set_static_mesh(gz)
        g.set_actor_scale3d(unreal.Vector(90.8, 90.8, 90.8))
        g.set_actor_rotation(unreal.Rotator(0.0, 90.0, 0.0), False)
        g.set_actor_label('Gazelle_Mesh'); tag(g)
    ExitZoneCls = unreal.load_class(None, '/Script/Avaryo.ExitZone')
    if ExitZoneCls:
        ez = eas.spawn_actor_from_class(ExitZoneCls, unreal.Vector(2800, -350, F1 + 100))
        ez.set_actor_label('ExitZone_Gazelle'); tag(ez)
    out.append('gazelle/exitzone ok (gz=%s ez=%s)' % (bool(gz), bool(ExitZoneCls)))
except Exception as e:
    out.append('GAZELLE/EXIT FAIL: %s' % e)

# ---------- ключевые ремонтируемые ----------
RepCls = unreal.load_class(None, '/Script/Avaryo.Repairable')
def mg(name):
    try:
        return getattr(unreal.RepairMinigameType, name)
    except Exception as e:
        out.append('  enum RepairMinigameType.%s? %s' % (name, e))
        return None
def repairable(label, x, y, z, mgname, tool, leaks):
    if not RepCls:
        out.append('  Repairable class not found'); return
    try:
        r = eas.spawn_actor_from_class(RepCls, unreal.Vector(x, y, z))
        r.set_actor_label(label); tag(r)
        m = mg(mgname)
        if m is not None:
            r.set_editor_property('MinigameType', m)
        if tool:
            r.set_editor_property('RequiredTool', tool)
        r.set_editor_property('bLeaksGasWhenBroken', leaks)
        r.set_editor_property('DisplayName', label)
        out.append('  repairable %s ok' % label)
    except Exception as e:
        out.append('  repairable %s? %s' % (label, e))
repairable('Repairable_Generator', 700, 3150, BZ + 60, 'STARTER', 'Welder', False)
repairable('Repairable_Breaker',  1100, 3680, BZ + 60, 'CURSOR', 'Tester', False)
repairable('Repairable_GasPipe',   300, 1750, BZ + 60, 'VALVE', '', True)

# ---------- сохранить ----------
les.save_current_level()
saved = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('save_dirty_packages = ' + str(saved))
out.append('total actors now = ' + str(len(eas.get_all_level_actors())))

open(r'D:\unrealEngine\avariika\Saved\build_hospital.txt', 'w', encoding='utf-8').write('\n'.join(out))
print('\n'.join(out))
