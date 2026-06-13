# -*- coding: utf-8 -*-
"""Аудит реальных размеров всех моделей на уровне: базовый габарит меша × масштаб актора =
мировой размер; флаг неравномерного масштаба (искажение). Реальные эталоны для сверки."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []

# эталонные реальные max-габариты (см) для сверки
REAL = {
    'Toilet': 75, 'Repairable_Breaker': '60-160', 'Repairable_GasPipe': '200-240',
    'Repairable_Generator': '90-200', 'FireExtinguisher': 55, 'FirstAidKit': 30,
    'Radio': 22, 'Battery': 18, 'Tester': 20, 'WeldingMachine': 45, 'Fuse': 12,
    'Cigarettes': 11, 'Thermos': 25, 'LightKit': 28, 'TrapKit': 18, 'MotionSensor': 16,
    'Gazelle_Mesh': 533,
    'Loot_Battery_NW': 18, 'Loot_Battery_SE': 18, 'Loot_FirstAid_NE': 30,
    'Loot_Fuse_N': 12, 'Loot_Cigarettes_SW': 11,
}


def comp_of(a):
    try:
        c = a.get_editor_property('MeshComponent')
        if c:
            return c
    except Exception:
        pass
    return a.get_component_by_class(unreal.StaticMeshComponent)


for a in eas.get_all_level_actors():
    lbl = a.get_actor_label()
    if lbl not in REAL:
        continue
    c = comp_of(a)
    sm = c.get_editor_property('static_mesh') if c else None
    if not sm:
        out.append('%-22s НЕТ МЕША' % lbl); continue
    b = sm.get_bounds().box_extent
    base = (b.x * 2, b.y * 2, b.z * 2)
    s = a.get_actor_scale3d()
    world = (base[0] * s.x, base[1] * s.y, base[2] * s.z)
    uniform = (abs(s.x - s.y) < 0.01 and abs(s.y - s.z) < 0.01)
    flag = '' if uniform else '  <-- НЕРАВНОМЕРНЫЙ масштаб!'
    out.append('%-22s world=%5.0fx%5.0fx%5.0f см (max %3.0f) scale=(%.3f,%.3f,%.3f) эталон~%s%s' % (
        lbl, world[0], world[1], world[2], max(world), s.x, s.y, s.z, REAL[lbl], flag))

out.sort()
open(r'D:\unrealEngine\avariika\Saved\audit_sizes.txt', 'w', encoding='utf-8').write('\n'.join(out))
