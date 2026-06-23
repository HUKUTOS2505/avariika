# -*- coding: utf-8 -*-
"""Найти пол: актор с самым большим XY-следом рядом со спавном; вернуть верх его поверхности.
Плюс полувысота капсулы игрока (PlayerStart)."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
best = None
best_area = 0.0
for a in eas.get_all_level_actors():
    c = a.get_component_by_class(unreal.StaticMeshComponent)
    if not c or not c.get_editor_property('static_mesh'):
        continue
    try:
        org, ext = a.get_actor_bounds(False)
    except Exception:
        continue
    area = ext.x * ext.y
    # большой плоский: большая площадь, малая высота, близко к спавну по Z
    if area > best_area and ext.z < 200 and abs(org.z) < 600:
        best_area = area
        best = (a.get_actor_label(), org.z + ext.z, org.z, ext.z, ext.x*2, ext.y*2)
if best:
    out.append('FLOOR candidate: %s topZ=%.0f (origZ=%.0f extZ=%.0f size=%.0fx%.0f)' % best)
else:
    out.append('no floor found')

# капсула игрока
for a in eas.get_all_level_actors():
    if a.get_class().get_name() == 'PlayerStart':
        l = a.get_actor_location()
        cap = a.get_component_by_class(unreal.CapsuleComponent)
        hh = cap.get_unscaled_capsule_half_height() if cap else -1
        out.append('PlayerStart z=%.0f capsuleHalfH=%.1f -> feet~%.0f' % (l.z, hh, l.z - (hh if hh > 0 else 0)))
open(r'C:\unrealEngine\avariika\Saved\find_floor.txt', 'w', encoding='utf-8').write('\n'.join(out))
