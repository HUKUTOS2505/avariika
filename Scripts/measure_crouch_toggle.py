# -*- coding: utf-8 -*-
# Меряем высоты FP-камеры/мешей, затем переключаем присед. Запустить ДВАЖДЫ:
# 1-й прогон пишет «стоя» и приседает; 2-й пишет «сидя» и встаёт. Сравниваем FP cam Z.
import unreal, json, traceback
R = {"err": None}
def relz(c):
    try: return round(float(c.get_editor_property("relative_location").z),1)
    except Exception:
        try: return round(float(c.relative_location.z),1)
        except Exception: return None
def worldz(c):
    try: return round(float(c.get_world_location().z),1)
    except Exception: return None
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        try: crouched = bool(pawn.bis_crouched)
        except Exception: crouched = bool(pawn.get_editor_property("is_crouched"))
        R["bIsCrouched"] = crouched
        cap = pawn.get_component_by_class(unreal.CapsuleComponent)
        R["capsule_half"] = round(float(cap.get_scaled_capsule_half_height()),1) if cap else None
        for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
            R[c.get_name()] = {"world_z": worldz(c), "rel_z": relz(c)}
        for c in pawn.get_components_by_class(unreal.CameraComponent):
            R["cam_"+c.get_name()] = {"world_z": worldz(c), "active": bool(c.is_active())}
        if crouched:
            pawn.un_crouch(); R["did"] = "un_crouch"
        else:
            pawn.crouch(); R["did"] = "crouch"
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
tag = "crouched" if R.get("bIsCrouched") else "standing"
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_crouch_%s.json" % tag, "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("MEASURE_CROUCH %s" % json.dumps(R, default=str))
