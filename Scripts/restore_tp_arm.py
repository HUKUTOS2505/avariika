# -*- coding: utf-8 -*-
# Возвращаем TP-пружину к проектным значениям (диагностика их меняла) + финальный кадр подтверждения.
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        for c in pawn.get_components_by_class(unreal.SpringArmComponent):
            c.set_editor_property("target_arm_length", 300.0)
            c.set_editor_property("socket_offset", unreal.Vector(0.0, 40.0, 60.0))
            c.set_editor_property("do_collision_test", True)
            R["arm_restored"] = True
            break
        # тело точно в None (на случай если до live-patch)
        for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
            if c.get_name() == "CharacterMesh0":
                c.set_editor_property("first_person_primitive_type", unreal.FirstPersonPrimitiveType.NONE)
                c.set_owner_no_see(False)
                break
        try:
            unreal.AutomationLibrary.take_high_res_screenshot(1280, 720, "tp_final_shot")
            R["shot"] = "tp_final_shot requested"
        except Exception as e:
            R["shot_err"] = str(e)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_restore_tp_arm.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("RESTORE_TP_ARM %s" % json.dumps(R, default=str))
