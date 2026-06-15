# -*- coding: utf-8 -*-
# Инспекция РАНТАЙМА в PIE: проверяем фиксы вживую (присед, зона выхода, аптечка у щитка).
import unreal, json, traceback
R = {"err": None}
def s(v):
    try: return [round(v.x), round(v.y), round(v.z)]
    except Exception: return str(v)
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()  # PIE-мир, если идёт игра
    R["game_world"] = gw.get_path_name() if gw else None
    if not gw:
        R["err"] = "PIE не активен (game world нет)"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        R["pawn"] = pawn.get_class().get_name() if pawn else None
        if pawn:
            cm = pawn.get_component_by_class(unreal.CharacterMovementComponent)
            if cm:
                try:
                    nav = cm.get_editor_property("nav_agent_props")
                    R["can_crouch"] = bool(nav.get_editor_property("can_crouch"))
                except Exception as e:
                    R["can_crouch_err"] = str(e)
                try: R["crouched_half_height"] = cm.get_editor_property("crouched_half_height")
                except Exception as e: R["chh_err"] = str(e)
                try: R["max_walk_crouched"] = cm.get_editor_property("max_walk_speed_crouched")
                except Exception as e: pass
            try: R["pawn_loc"] = s(pawn.get_actor_location())
            except Exception: pass
        # Актёры в PIE-мире
        def count_cls(path):
            c = unreal.load_class(None, path)
            return unreal.GameplayStatics.get_all_actors_of_class(gw, c) if c else []
        ez = count_cls("/Script/Avaryo.ExitZone")
        R["exit_zones"] = [{"label": a.get_actor_label(), "loc": s(a.get_actor_location())} for a in ez]
        try:
            reps = count_cls("/Script/Avaryo.Repairable")
            R["repairables"] = [{"loc": s(a.get_actor_location()), "broken": bool(a.is_broken())} for a in reps]
        except Exception as e:
            R["rep_err"] = str(e)
        try:
            items = count_cls("/Script/Avaryo.PickupItem")
            R["pickups"] = [{"name": a.get_actor_label(), "loc": s(a.get_actor_location())} for a in items]
        except Exception as e:
            R["pick_err"] = str(e)
        try:
            runs = count_cls("/Script/Avaryo.RunState")
            R["runstate_count"] = len(runs)
            if runs:
                R["total_objectives"] = runs[0].get_total_objectives()
                R["repaired_count"] = runs[0].get_repaired_count()
                R["is_hub"] = runs[0].is_hub_mode()
                try: R["has_exit_zone"] = runs[0].has_exit_zone()
                except Exception as e: R["hez_err"] = str(e)
        except Exception as e:
            R["run_err"] = str(e)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_inspect_pie.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("INSPECT_PIE %s" % json.dumps(R, default=str))
