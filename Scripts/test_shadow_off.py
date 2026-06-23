# -*- coding: utf-8 -*-
# Решаем: тёмное пятно — это ТЕНЬ (тело не рисуется) или само ТЕЛО (рисуется тёмным)?
# Выключаем отбрасывание тени у CharacterMesh0 и снимаем. Пятно исчезло => это была тень.
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        body = None
        for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
            if c.get_name() == "CharacterMesh0": body = c; break
        if body:
            try: body.set_cast_shadow(False)
            except Exception as e: R["shadow_err"] = str(e)
            R["cast_shadow_now"] = bool(body.get_editor_property("cast_shadow"))
            try:
                unreal.AutomationLibrary.take_high_res_screenshot(1280, 720, "shadow_off_shot")
                R["shot"] = "shadow_off_shot requested"
            except Exception as e:
                R["shot_err"] = str(e)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_shadow_off.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("SHADOW_OFF %s" % json.dumps(R, default=str))
