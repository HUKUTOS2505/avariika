# -*- coding: utf-8 -*-
"""Придвинуть Hilux к игроку (3/4 спереди-сбоку) для проверки стёкол в дневном свете."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
t = by.get('Gazelle_Mesh')
if t:
    t.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    t.set_actor_location(unreal.Vector(820.0, -134.0, 300.0), False, True)
    t.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=210.0, roll=0.0), False)
    t.modify()
open(r'C:\unrealEngine\avariika\Saved\hilux_glass_shot.txt', 'w', encoding='utf-8').write('moved')
