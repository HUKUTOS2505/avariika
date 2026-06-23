# -*- coding: utf-8 -*-
"""Сварочник был крошечным (14см) и с НЕРАВНОМЕРНЫМ масштабом (искажён). Поставить
равномерный масштаб под реальный ~45см на всех WeldingMachine."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
out = []
TARGET = 45.0


def comp_of(a):
    try:
        c = a.get_editor_property('MeshComponent')
        if c:
            return c
    except Exception:
        pass
    return a.get_component_by_class(unreal.StaticMeshComponent)


n = 0
for a in eas.get_all_level_actors():
    if a.get_actor_label() == 'WeldingMachine' or a.get_class().get_name() == 'BP_WeldingMachine_C':
        c = comp_of(a)
        sm = c.get_editor_property('static_mesh') if c else None
        if not sm:
            out.append('%s: нет меша' % a.get_actor_label()); continue
        b = sm.get_bounds().box_extent
        base = max(b.x, b.y, b.z) * 2.0
        s = TARGET / base if base > 0 else 1.0
        a.set_actor_scale3d(unreal.Vector(s, s, s))
        a.modify()
        out.append('%s: base %.0f -> uniform scale %.3f (%.0fсм)' % (a.get_actor_label(), base, s, base * s))
        n += 1
out.append('fixed=%d' % n)
les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'C:\unrealEngine\avariika\Saved\fix_welder_size.txt', 'w', encoding='utf-8').write('\n'.join(out))
