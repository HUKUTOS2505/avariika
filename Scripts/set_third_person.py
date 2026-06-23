# -*- coding: utf-8 -*-
# Переключает камеру игрока в вид от 3-го лица (если идёт PIE и пешка — AvaryoCharacter).
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    R["pie_active"] = gw is not None
    if not gw:
        R["note"] = "PIE не запущен — нет игрока, чтобы переключить камеру. Нажми Play."
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        R["pawn"] = pawn.get_class().get_name() if pawn else None
        if pawn and hasattr(pawn, "toggle_camera_mode"):
            if not pawn.is_third_person():
                pawn.toggle_camera_mode()
            R["third_person"] = bool(pawn.is_third_person())
        else:
            R["note"] = "Пешка не AvaryoCharacter — тумблер камеры (V) тут недоступен. Нужна карта/режим Avaryo."
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_set_tp.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SET_TP %s" % json.dumps(R, default=str))
