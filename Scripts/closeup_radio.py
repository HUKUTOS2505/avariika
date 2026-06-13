# -*- coding: utf-8 -*-
"""Рацию вплотную к камере PIE (cam -13.5,0,377.6, +X) для чистого close-up; остальные PROD_ — вдаль."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in eas.get_all_level_actors():
    lbl = a.get_actor_label()
    if not lbl.startswith('PROD_'):
        continue
    if lbl == 'PROD_SM_Radio':
        c = a.static_mesh_component
        sm = c.get_editor_property('static_mesh')
        b = sm.get_bounds().box_extent
        mx = max(b.x, b.y, b.z) * 2.0
        s = 45.0 / mx if mx > 0 else 1.0
        a.set_actor_scale3d(unreal.Vector(s, s, s))
        a.set_actor_location(unreal.Vector(72.0, 0.0, 372.0), False, True)
        a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=180.0, roll=0.0), False)
    else:
        a.set_actor_location(unreal.Vector(72.0, 4000.0, 372.0), False, True)
open(r'D:\unrealEngine\avariika\Saved\closeup_radio.txt', 'w', encoding='utf-8').write('ok')
