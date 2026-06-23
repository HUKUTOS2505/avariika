# -*- coding: utf-8 -*-
"""Размещает (или поправляет) ремонтируемые объекты и зону выхода в Lvl_FirstPerson.

Запуск (редактор пользователя должен быть закрыт):
  UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="Scripts/place_run_objects.py"

Идемпотентен: актор ищется по метке; есть — двигаем, нет — спавним.
Координаты подобраны по фактической геометрии карты (пол z=-15, платформа спавна z=205).
"""
import unreal

LEVEL = '/Game/FirstPerson/Lvl_FirstPerson'

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not les.load_level(LEVEL):
    raise RuntimeError('Не удалось загрузить уровень ' + LEVEL)

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by_label = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
cube = unreal.load_asset('/Engine/BasicShapes/Cube')


def upsert(cls, label, loc, scale):
    actor = by_label.get(label)
    if actor is None:
        actor = eas.spawn_actor_from_class(cls, loc, unreal.Rotator(0.0, 0.0, 0.0))
        actor.set_actor_label(label)
    else:
        actor.set_actor_location(loc, False, False)
    actor.set_actor_scale3d(scale)
    return actor


def upsert_repairable(label, display_name, loc, scale, duration):
    actor = upsert(unreal.Repairable, label, loc, scale)
    actor.get_editor_property('MeshComponent').set_static_mesh(cube)
    actor.set_editor_property('DisplayName', display_name)
    actor.set_editor_property('RepairDuration', duration)
    unreal.log('OK %s (%.0f, %.0f, %.0f)' % (label, loc.x, loc.y, loc.z))


# Куб 100 см: z = пол(-15) + 50 * scale.z. Точки разнесены по карте,
# чтобы бригада разделялась (источник кооп-историй).
upsert_repairable('Repairable_Breaker', 'Электрощиток',
                  unreal.Vector(1200.0, -1320.0, 60.0), unreal.Vector(0.35, 1.0, 1.5), 6.0)
upsert_repairable('Repairable_GasPipe', 'Газовая труба',
                  unreal.Vector(1700.0, 150.0, 95.0), unreal.Vector(0.35, 0.35, 2.2), 8.0)
upsert_repairable('Repairable_Generator', 'Генератор',
                  unreal.Vector(-1200.0, 1250.0, 35.0), unreal.Vector(1.5, 1.0, 1.0), 10.0)

# ГАЗель — зона выхода на платформе спавна (игроки стоят на z~210)
zone = upsert(unreal.ExitZone, 'ExitZone_Gazelle',
              unreal.Vector(-250.0, 0.0, 300.0), unreal.Vector(1.0, 1.0, 1.0))
unreal.log('OK ExitZone_Gazelle')

if not les.save_current_level():
    raise RuntimeError('Уровень не сохранился')

with open(r'C:\unrealEngine\avariika\Saved\place_result.txt', 'w', encoding='utf-8') as f:
    f.write('saved ok\n')
    for a in eas.get_all_level_actors():
        if isinstance(a, (unreal.Repairable, unreal.ExitZone)):
            loc = a.get_actor_location()
            f.write('%s (%.0f, %.0f, %.0f)\n' % (a.get_actor_label(), loc.x, loc.y, loc.z))
