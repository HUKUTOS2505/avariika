# -*- coding: utf-8 -*-
"""Узнать высоту пола (трейс вниз) и пивоты мешей ремонтируемых, чтобы ставить на пол."""
import unreal
out = []
world = unreal.EditorLevelLibrary.get_editor_world()

def floor_at(x, y):
    start = unreal.Vector(x, y, 800.0)
    end = unreal.Vector(x, y, -300.0)
    hit = unreal.SystemLibrary.line_trace_single(
        world, start, end, unreal.TraceTypeQuery.TRACE_TYPE_QUERY1, False, [],
        unreal.DrawDebugTrace.NONE, True)
    if hit:
        try:
            return hit.impact_point.z
        except Exception:
            try:
                return hit.location.z
            except Exception:
                return None
    return None

for (x, y) in [(0, 0), (480, -350), (480, 60), (480, 380)]:
    z = floor_at(x, y)
    out.append('floor at (%d,%d) = %s' % (x, y, ('%.0f' % z) if z is not None else 'MISS'))

# пивоты мешей (minZ относительно пивота)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in eas.get_all_level_actors():
    if a.get_class().get_name() == 'Repairable':
        c = a.get_component_by_class(unreal.StaticMeshComponent)
        sm = c.get_editor_property('static_mesh') if c else None
        if sm:
            b = sm.get_bounds()
            sc = a.get_actor_scale3d()
            out.append('%s mesh=%s pivotMinZ=%.1f extZ=%.1f scaleZ=%.2f worldH=%.0f' % (
                a.get_actor_label(), sm.get_name(), b.origin.z - b.box_extent.z, b.box_extent.z, sc.z, b.box_extent.z*2*sc.z))

open(r'D:\unrealEngine\avariika\Saved\get_layout_info.txt', 'w', encoding='utf-8').write('\n'.join(out))
