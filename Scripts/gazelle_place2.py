# -*- coding: utf-8 -*-
"""Позиционирование собранного грузовика (nisan_scene_0): робастно (try на каждый актор),
лог через unreal.log_warning('GAZPLACE: ...'). Низ грузовика на месте серой ГАЗели."""
import unreal

def log(m):
    unreal.log_warning('GAZPLACE: ' + m)

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
allactors = eas.get_all_level_actors()
root = next((a for a in allactors if a.get_actor_label() == 'nisan_scene_0'), None)
grays = [a for a in allactors if a.get_actor_label() == 'Gazelle_Mesh']
target = grays[0].get_actor_location() if grays else unreal.Vector(-300, 0, 100)
log('root=%s grays=%d target=(%.0f,%.0f,%.0f)' % (bool(root), len(grays), target.x, target.y, target.z))

if root:
    desc = [root]
    def collect(a):
        for c in a.get_attached_actors():
            desc.append(c)
            collect(c)
    collect(root)
    log('descendants=%d' % len(desc))
    root.set_actor_location(unreal.Vector(0, 0, 0))
    root.set_actor_scale3d(unreal.Vector(1, 1, 1))
    INF = 1e18
    mn = [INF, INF, INF]
    mx = [-INF, -INF, -INF]
    n = 0
    for a in desc:
        try:
            o, e = a.get_actor_bounds(False)
        except Exception:
            continue
        if e.x <= 1 and e.y <= 1 and e.z <= 1:
            continue
        cc = [o.x, o.y, o.z]
        ee = [e.x, e.y, e.z]
        for i in range(3):
            mn[i] = min(mn[i], cc[i] - ee[i])
            mx[i] = max(mx[i], cc[i] + ee[i])
        n += 1
    log('mesh actors with bounds=%d' % n)
    if n > 0:
        size = [mx[i] - mn[i] for i in range(3)]
        center = [(mx[i] + mn[i]) / 2 for i in range(3)]
        log('natural size=%.1f x %.1f x %.1f center=(%.1f,%.1f,%.1f)' % (size[0], size[1], size[2], center[0], center[1], center[2]))
        md = max(size) if max(size) > 0 else 1.0
        scale = 500.0 / md
        root.set_actor_scale3d(unreal.Vector(scale, scale, scale))
        nl = unreal.Vector(target.x - center[0] * scale, target.y - center[1] * scale, target.z - mn[2] * scale)
        root.set_actor_location(nl)
        log('scale=%.4f finalsize~%.0fcm loc=(%.0f,%.0f,%.0f)' % (scale, md * scale, nl.x, nl.y, nl.z))

for g in grays:
    eas.destroy_actor(g)
    log('deleted gray Gazelle_Mesh')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
log('saved DONE')
