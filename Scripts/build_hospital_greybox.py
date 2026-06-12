# -*- coding: utf-8 -*-
"""Карта «Больница» — greybox, ПРОХОД 1: каркас здания.
Перекрытия (подвал/1/2/3/крыша), наружные стены, стены центрального коридора,
парапет крыши, проём главного входа, PlayerStart. По ТЗ TZ_Hospital_Map_UE5.md §1,8.
Лестницы/лифт/комнаты/проёмы — следующими проходами.

Запуск (редактор ЗАКРЫТ):
  UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="D:\\unrealEngine\\avariika\\Scripts\\build_hospital_greybox.py"
Идемпотентен: удаляет свои актёры (префикс GB_) перед сборкой.
"""
import unreal

LEVEL = '/Game/Hospital/Maps/L_Hospital'
CUBE = unreal.load_asset('/Engine/BasicShapes/Cube')

# Габариты (UU = см). Угол ось1/А в (0,0,0). Здание X 0..6000, Y 0..1600.
LEN_X = 6000.0
DEP_Y = 1600.0
EXT = 50.0      # наружная стена / перекрытие
COR_S = 600.0   # южная стена коридора (ось Б)
COR_N = 900.0   # северная стена коридора (ось В)
ENTR_X = 3000.0 # главный вход (ось 6)
ENTR_W = 200.0  # ширина проёма входа

# Уровни: (имя, Z пола, высота этажа)
FLOORS = [
    ('B', -290.0, 290.0),
    ('F1', 0.0, 350.0),
    ('F2', 350.0, 350.0),
    ('F3', 700.0, 350.0),
]
ROOF_Z = 1050.0

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

out = []
if unreal.EditorAssetLibrary.does_asset_exist(LEVEL):
    les.load_level(LEVEL)
else:
    les.new_level(LEVEL)
    out.append('создан уровень ' + LEVEL)

# Чистим прошлый greybox
removed = 0
for a in eas.get_all_level_actors():
    if a.get_actor_label().startswith('GB_'):
        eas.destroy_actor(a)
        removed += 1
out.append('удалено старых GB_: %d' % removed)


def box(name, cx, cy, cz, sx, sy, sz):
    a = eas.spawn_actor_from_object(CUBE, unreal.Vector(cx, cy, cz))
    a.set_actor_label(name)
    a.set_actor_scale3d(unreal.Vector(sx / 100.0, sy / 100.0, sz / 100.0))
    return a


# --- Перекрытия (верх плиты = Z пола) ---
for nm, z, h in FLOORS:
    box('GB_Slab_' + nm, LEN_X / 2, DEP_Y / 2, z - EXT / 2, LEN_X, DEP_Y, EXT)
box('GB_Slab_Roof', LEN_X / 2, DEP_Y / 2, ROOF_Z - EXT / 2, LEN_X, DEP_Y, EXT)

# --- Наружные стены + стены коридора по этажам ---
for nm, z, h in FLOORS:
    cz = z + h / 2
    # Север/Запад/Восток — сплошные
    box('GB_WallN_' + nm, LEN_X / 2, DEP_Y - EXT / 2, cz, LEN_X, EXT, h)
    box('GB_WallW_' + nm, EXT / 2, DEP_Y / 2, cz, EXT, DEP_Y, h)
    box('GB_WallE_' + nm, LEN_X - EXT / 2, DEP_Y / 2, cz, EXT, DEP_Y, h)
    # Юг: на 1 этаже — проём главного входа (две секции)
    if nm == 'F1':
        left_w = (ENTR_X - ENTR_W / 2)
        right_w = LEN_X - (ENTR_X + ENTR_W / 2)
        box('GB_WallS_F1a', left_w / 2, EXT / 2, cz, left_w, EXT, h)
        box('GB_WallS_F1b', (ENTR_X + ENTR_W / 2) + right_w / 2, EXT / 2, cz, right_w, EXT, h)
    else:
        box('GB_WallS_' + nm, LEN_X / 2, EXT / 2, cz, LEN_X, EXT, h)
    # Стены центрального коридора (вдоль X), толщина 20, проёмы добавим в проходе 2
    box('GB_CorS_' + nm, LEN_X / 2, COR_S, cz, LEN_X, 20.0, h)
    box('GB_CorN_' + nm, LEN_X / 2, COR_N, cz, LEN_X, 20.0, h)

# --- Парапет крыши (высота 90) ---
pz = ROOF_Z + 45
box('GB_ParN', LEN_X / 2, DEP_Y - EXT / 2, pz, LEN_X, EXT, 90.0)
box('GB_ParS', LEN_X / 2, EXT / 2, pz, LEN_X, EXT, 90.0)
box('GB_ParW', EXT / 2, DEP_Y / 2, pz, EXT, DEP_Y, 90.0)
box('GB_ParE', LEN_X - EXT / 2, DEP_Y / 2, pz, EXT, DEP_Y, 90.0)

# --- PlayerStart снаружи у главного входа ---
for a in eas.get_all_level_actors():
    if a.get_actor_label() == 'GB_PlayerStart':
        eas.destroy_actor(a)
ps = eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(ENTR_X, -250.0, 100.0), unreal.Rotator(0.0, 90.0, 0.0))
ps.set_actor_label('GB_PlayerStart')

les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('greybox проход 1 готов, уровень сохранён')

with open(r'D:\unrealEngine\avariika\Saved\hospital_greybox_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
