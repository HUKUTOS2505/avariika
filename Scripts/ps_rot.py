# -*- coding: utf-8 -*-
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
for a in eas.get_all_level_actors():
    if a.get_class().get_name() == 'PlayerStart':
        l = a.get_actor_location()
        r = a.get_actor_rotation()
        out.append('PlayerStart loc=(%.0f,%.0f,%.0f) rot(pitch=%.0f,yaw=%.0f,roll=%.0f)' % (
            l.x, l.y, l.z, r.pitch, r.yaw, r.roll))
open(r'D:\unrealEngine\avariika\Saved\ps_rot.txt', 'w', encoding='utf-8').write('\n'.join(out) or 'none')
