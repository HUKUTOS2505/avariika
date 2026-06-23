# -*- coding: utf-8 -*-
# Телепортируем PIE-игрока к щитку и поворачиваем лицом к нему — чтобы СНЯТЬ скрин подсказки.
import unreal, json, math, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        pc = unreal.GameplayStatics.get_player_controller(gw, 0)
        # Щиток (Breaker) в [740,290,265]. Встаём в ~2.2м перед ним.
        bx, by, bz = 740.0, 290.0, 300.0
        px, py, pz = 560.0, 250.0, 330.0
        if pawn:
            pawn.set_actor_location(unreal.Vector(px, py, pz), False, False)
            R["moved_to"] = [px, py, pz]
        if pc:
            # цельемся в ЦЕНТР щитка (он ниже уровня глаз ~395) — смотрим вниз
            eye = pz + 65.0
            hdist = math.hypot(bx - px, by - py)
            yaw = math.degrees(math.atan2(by - py, bx - px))
            pitch = math.degrees(math.atan2(bz - eye, hdist))
            pc.set_control_rotation(unreal.Rotator(pitch, yaw, 0.0))
            R["yaw"] = round(yaw, 1); R["pitch"] = round(pitch, 1)
        # подсказка-фокус считается локально в Tick игрока — дадим кадр
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_tp_breaker.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("TP_BREAKER %s" % json.dumps(R, default=str))
