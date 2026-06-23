# -*- coding: utf-8 -*-
# Замер геометрии TP + скрин ИГРОВОГО вьюпорта в один проход, чтобы они соответствовали друг другу.
import unreal, json, math, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        if pawn:
            try: R["is_third_person"] = bool(pawn.is_third_person())
            except Exception: pass
            act = None
            for c in pawn.get_components_by_class(unreal.CameraComponent):
                if c.is_active(): act = c; break
            if act:
                cw = act.get_world_location(); cr = act.get_world_rotation()
                R["active_cam"] = act.get_name()
                R["cam_world"] = [round(cw.x),round(cw.y),round(cw.z)]
                R["cam_pitch"] = round(cr.pitch,1); R["cam_yaw"] = round(cr.yaw,1)
            body = None
            for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
                if c.get_name() == "CharacterMesh0": body = c; break
            if body:
                bw = body.get_world_location()
                R["body_world"] = [round(bw.x),round(bw.y),round(bw.z)]
                R["body_visible"] = bool(body.is_visible())
                R["body_owner_no_see"] = bool(body.get_editor_property("owner_no_see"))
                R["body_bounds_radius"] = round(float(body.get_editor_property("bounds_scale")),2) if False else None
            # игровой скрин (асинхронно, следующий кадр) — пишет в Saved/Screenshots/
            try:
                unreal.AutomationLibrary.take_high_res_screenshot(1280, 720, "corr_tp_shot")
                R["shot"] = "corr_tp_shot requested"
            except Exception as e:
                R["shot_err"] = str(e)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_correlate_tp.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("CORRELATE_TP %s" % json.dumps(R, default=str))
