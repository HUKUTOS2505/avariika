# -*- coding: utf-8 -*-
"""Вернуть Hilux к выходу + задать РЕАЛЬНЫЕ размеры заглушкам. Полный перехват ошибок."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []


def report():
    open(r'C:\unrealEngine\avariika\Saved\restore_and_sizes.txt', 'w', encoding='utf-8').write('\n'.join(out))


try:
    by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

    t = by.get('Gazelle_Mesh')
    if t:
        try:
            t.set_actor_location(unreal.Vector(-300.0, 0.0, 210.0), False, True)
            t.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=90.0, roll=0.0), False)
            t.set_actor_scale3d(unreal.Vector(0.9952, 0.9952, 0.9952))
            t.static_mesh_component.set_mobility(unreal.ComponentMobility.STATIC)
            t.modify()
            out.append('Hilux restored')
        except Exception as e:
            out.append('Hilux FAIL: %s' % e)

    SIZES = {'Thermos': 25.0, 'TrapKit': 18.0, 'LightKit': 28.0, 'MotionSensor': 16.0}
    for lbl, target in SIZES.items():
        a = by.get(lbl)
        if not a:
            out.append('%s: НЕТ актора' % lbl)
            continue
        try:
            comp = a.get_component_by_class(unreal.StaticMeshComponent)
            sm = comp.get_editor_property('static_mesh') if comp else None
            if not sm:
                out.append('%s: нет static mesh' % lbl)
                continue
            b = sm.get_bounds().box_extent
            base = max(b.x, b.y, b.z) * 2.0
            s = target / base if base > 0 else 1.0
            a.set_actor_scale3d(unreal.Vector(s, s, s))
            a.modify()
            out.append('%s -> %.0fсм (scale %.3f, base %.0f)' % (lbl, target, s, base))
        except Exception as e:
            out.append('%s FAIL: %s' % (lbl, e))

    les.save_current_level()
    out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
except Exception as e:
    out.append('FATAL: %s' % e)

report()
