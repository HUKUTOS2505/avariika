# -*- coding: utf-8 -*-
# Полная иерархия компонентов живой пешки: имя, класс, родитель крепления, относительная позиция.
# Нужно для починки и TP-камеры, и приседа (куда привязана FirstPersonCamera, следует ли за капсулой).
import unreal, json, traceback
R = {"err": None, "comps": []}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        R["pawn"] = pawn.get_class().get_name() if pawn else None
        if pawn:
            move = pawn.get_component_by_class(unreal.CharacterMovementComponent)
            if move:
                R["can_crouch"] = bool(move.get_editor_property("nav_agent_props").get_editor_property("can_crouch")) if False else None
                try: R["is_crouching"] = bool(pawn.get_editor_property("is_crouched")) if False else None
                except Exception: pass
            try: R["bIsCrouched"] = bool(pawn.bis_crouched)
            except Exception:
                try: R["bIsCrouched"] = bool(pawn.get_editor_property("is_crouched"))
                except Exception as e: R["crouch_err"] = str(e)
            cap = pawn.get_component_by_class(unreal.CapsuleComponent)
            if cap:
                try: R["capsule_half_height"] = round(float(cap.get_scaled_capsule_half_height()),1)
                except Exception: pass
            comps = pawn.get_components_by_class(unreal.SceneComponent)
            for c in comps:
                info = {"name": c.get_name(), "class": c.get_class().get_name()}
                try:
                    par = c.get_attach_parent()
                    info["parent"] = par.get_name() if par else None
                except Exception: pass
                try:
                    sock = c.get_attach_socket_name()
                    info["socket"] = str(sock) if sock and str(sock) != "None" else None
                except Exception: pass
                try:
                    rl = c.get_relative_location(); info["rel"] = [round(rl.x,1), round(rl.y,1), round(rl.z,1)]
                except Exception: pass
                R["comps"].append(info)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_comp_tree.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("COMP_TREE %s" % json.dumps(R, default=str))
