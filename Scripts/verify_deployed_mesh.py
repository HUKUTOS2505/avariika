# -*- coding: utf-8 -*-
"""Заспавнить рантайм-акторы и проверить, что FObjectFinder разрешил реальные меши."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
JOBS = [
    ('/Script/Avaryo.Floodlight', 'SM_Floodlight'),
    ('/Script/Avaryo.Trap', 'SM_Trap'),
    ('/Script/Avaryo.BioProjectile', 'SM_BioBlob'),
    ('/Script/Avaryo.BioPickup', 'SM_BioBlob'),
]
for cls_path, expect in JOBS:
    cls = unreal.load_class(None, cls_path)
    if not cls:
        out.append('%s: класс не найден' % cls_path); continue
    a = eas.spawn_actor_from_class(cls, unreal.Vector(0, 0, -5000))
    comp = a.get_editor_property('MeshComponent') if a.get_editor_property('MeshComponent') else a.get_component_by_class(unreal.StaticMeshComponent)
    sm = comp.get_editor_property('static_mesh') if comp else None
    sc = a.get_actor_scale3d()
    out.append('%s -> mesh=%s scale=(%.2f,%.2f,%.2f) [ожидался %s]' % (
        cls_path.split('.')[-1], sm.get_name() if sm else 'NONE', sc.x, sc.y, sc.z, expect))
    eas.destroy_actor(a)
open(r'D:\unrealEngine\avariika\Saved\verify_deployed_mesh.txt', 'w', encoding='utf-8').write('\n'.join(out))
