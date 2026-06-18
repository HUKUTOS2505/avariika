# -*- coding: utf-8 -*-
# Тест: вручную опускаем FP-меш и FP-камеру на -50 по Z. Потом read_eye проверит, что прижилось.
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
                c.set_editor_property("relative_location", unreal.Vector(rl.x, rl.y, -50.0))
                R["fpmesh_set_z"] = -50.0
        for c in pawn.get_components_by_class(unreal.CameraComponent):
            if c.is_active() and c.get_name() != "ThirdPersonCamera":
                rl = c.get_editor_property("relative_location")
                R["cam_before"] = [round(rl.x,1), round(rl.y,1), round(rl.z,1)]
                c.set_editor_property("relative_location", unreal.Vector(rl.x, rl.y, rl.z - 50.0))
                R["cam_set_z"] = round(rl.z - 50.0, 1)
                R["cam_world_z_after"] = round(float(c.get_world_location().z),1)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_set_eye.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SET_EYE %s" % json.dumps(R, default=str))
