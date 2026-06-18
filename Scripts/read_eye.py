# -*- coding: utf-8 -*-
# Только чтение: прижились ли ручные смещения FP-меша/камеры (или что-то сбросило в ноль).
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
            if "FirstPerson" in c.get_name():
                rl = c.get_editor_property("relative_location")
                R["fpmesh_rel_z"] = round(float(rl.z),1)
                R["fpmesh_world_z"] = round(float(c.get_world_location().z),1)
        for c in pawn.get_components_by_class(unreal.CameraComponent):
            if c.is_active() and c.get_name() != "ThirdPersonCamera":
                rl = c.get_editor_property("relative_location")
                R["cam_rel_z"] = round(float(rl.z),1)
                R["cam_world_z"] = round(float(c.get_world_location().z),1)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_read_eye.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("READ_EYE %s" % json.dumps(R, default=str))
