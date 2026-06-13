# -*- coding: utf-8 -*-
"""Спозиционировать собранный scene-import грузовик (корень nisan_scene_0) на место ГАЗели,
смасштабировать ~5 м, удалить серую заглушку Gazelle_Mesh. ASCII."""
import unreal

out = []
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
allactors = eas.get_all_level_actors()

root = next((a for a in allactors if a.get_actor_label() == 'nisan_scene_0'), None)
old = next((a for a in allactors if a.get_actor_label() == 'Gazelle_Mesh'), None)
target = old.get_actor_location() if old else unreal.Vector(-300, 0, 100)
out.append('root=%s old_gazelle=%s target=(%.0f,%.0f,%.0f)' % (bool(root), bool(old), target.x, target.y, target.z))

if root:
    # собрать всех потомков
    desc = []
    def collect(a):
        for c in a.get_attached_actors():
            desc.append(c)
            collect(c)
    collect(root)
    out.append('descendants=%d' % len(desc))

    # сброс корня в (0,0,0) scale 1, чтобы посчитать натуральные габариты
    root.set_actor_location(unreal.Vector(0, 0, 0))
    root.set_actor_scale3d(unreal.Vector(1, 1, 1))

    INF = 1e18
    mn = [INF, INF, INF]
    mx = [-INF, -INF, -INF]
    n = 0
    for a in [root] + desc:
        o, e = a.get_actor_bounds(False)
        if e.x <= 0 and e.y <= 0 and e.z <= 0:
            continue
        comp = [o.x, o.y, o.z]
        ext = [e.x, e.y, e.z]
        for i in range(3):
            mn[i] = min(mn[i], comp[i] - ext[i])
            mx[i] = max(mx[i], comp[i] + ext[i])
        n += 1
    out.append('mesh actors with bounds=%d' % n)
    if n > 0:
        size = [mx[i] - mn[i] for i in range(3)]
        center = [(mx[i] + mn[i]) / 2 for i in range(3)]
        out.append('natural size=%.1f x %.1f x %.1f, center=(%.1f,%.1f,%.1f)' % (size[0], size[1], size[2], center[0], center[1], center[2]))
        maxdim = max(size) if max(size) > 0 else 1.0
        scale = 500.0 / maxdim
        root.set_actor_scale3d(unreal.Vector(scale, scale, scale))
        # центр после масштаба (корень в 0): center*scale; ставим так, чтобы центр попал в target (низ чуть выше пола)
        newloc = unreal.Vector(target.x - center[0] * scale, target.y - center[1] * scale, target.z - center[2] * scale + size[2] * scale * 0.5)
        root.set_actor_location(newloc)
        out.append('scale=%.4f, final size~%.0fcm, root loc=(%.0f,%.0f,%.0f)' % (scale, maxdim * scale, newloc.x, newloc.y, newloc.z))

if old:
    eas.destroy_actor(old)
    out.append('deleted gray Gazelle_Mesh')

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
out.append('saved')
open(r'D:\unrealEngine\avariika\Saved\gazelle_place.txt', 'w', encoding='utf-8').write('\n'.join(out))
