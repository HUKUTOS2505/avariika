# -*- coding: utf-8 -*-
# Тест гипотезы «застрявший рендер-прокси»: жёстко гоним owner_no_see false + пересоздаём
# рендер-стейт тела (toggle visibility), затем снимаем игровой кадр.
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
            # форсим изменение, чтобы точно дёрнуть MarkRenderStateDirty
            try: body.set_owner_no_see(True)
            except Exception as e: R["e1"] = str(e)
            try: body.set_owner_no_see(False)
            except Exception as e: R["e2"] = str(e)
            try: body.set_only_owner_see(False)
            except Exception as e: R["e3"] = str(e)
            # пересоздать прокси через toggle видимости
            try:
                body.set_visibility(False, True)
                body.set_visibility(True, True)
            except Exception as e: R["e4"] = str(e)
            R["after_owner_no_see"] = bool(body.get_editor_property("owner_no_see"))
            R["after_visible"] = bool(body.is_visible())
            try:
                unreal.AutomationLibrary.take_high_res_screenshot(1280, 720, "force_body_shot")
                R["shot"] = "force_body_shot requested"
            except Exception as e:
                R["shot_err"] = str(e)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_force_body.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("FORCE_BODY %s" % json.dumps(R, default=str))
