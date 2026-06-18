# -*- coding: utf-8 -*-
# Отодвигаем и поднимаем TP-камеру на ЖИВОЙ пружине, чтобы оператор гарантированно попал в кадр,
# и снимаем игровой вьюпорт. Диагностика (рисуется ли тело вообще) + вероятная правка рамки.
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        if pawn:
            arm = None
            for c in pawn.get_components_by_class(unreal.SpringArmComponent):
                arm = c; break
            if arm:
                arm.set_editor_property("target_arm_length", 650.0)
                arm.set_editor_property("socket_offset", unreal.Vector(0.0, 0.0, 160.0))
                arm.set_editor_property("do_collision_test", False)  # на тесте пол не должен схлопывать
                R["arm_set"] = True
                R["new_len"] = float(arm.get_editor_property("target_arm_length"))
            # дать кадр на пересчёт и снять
            try:
                unreal.AutomationLibrary.take_high_res_screenshot(1280, 720, "tp_wide_shot")
                R["shot"] = "tp_wide_shot requested"
            except Exception as e:
                R["shot_err"] = str(e)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_set_tp_wide.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SET_TP_WIDE %s" % json.dumps(R, default=str))
