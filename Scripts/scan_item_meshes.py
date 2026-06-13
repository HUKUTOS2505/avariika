# -*- coding: utf-8 -*-
"""Пройтись по всем размещённым предметам/объектам и сказать, у кого реальный меш,
а у кого заглушка (basic shape) или пусто. Чтобы понять, что ещё нужно смоделить."""
import unreal

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
PLACEHOLDER = ('Cube', 'Sphere', 'Cylinder', 'Cone', 'Plane', '1M_Cube', 'BasicShapes')


def mesh_of(a):
    comp = None
    try:
        comp = a.get_editor_property('MeshComponent')
    except Exception:
        pass
    if comp is None:
        comp = a.get_component_by_class(unreal.StaticMeshComponent)
    if comp is None:
        comp = a.get_component_by_class(unreal.SkeletalMeshComponent)
    if comp is None:
        return None
    try:
        return comp.get_editor_property('static_mesh') or comp.get_editor_property('skeletal_mesh')
    except Exception:
        try:
            return comp.get_editor_property('static_mesh')
        except Exception:
            return None


seen = {}
for a in eas.get_all_level_actors():
    cn = a.get_class().get_name()
    # интересуют предметы/мир, не свет/объёмы/системные
    if not (cn.startswith('BP_') or cn in ('PickupItem', 'Repairable', 'Toilet', 'Trap', 'Floodlight')):
        continue
    m = mesh_of(a)
    nm = m.get_name() if m else 'NONE'
    status = 'OK'
    if not m:
        status = 'НЕТ МЕША'
    elif any(p in nm for p in PLACEHOLDER):
        status = 'ЗАГЛУШКА(' + nm + ')'
    key = a.get_actor_label()
    out.append('%-22s %-26s mesh=%-22s %s' % (key, cn, nm, status))

out.sort()
open(r'D:\unrealEngine\avariika\Saved\scan_item_meshes.txt', 'w', encoding='utf-8').write('\n'.join(out))
