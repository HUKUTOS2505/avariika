# -*- coding: utf-8 -*-
# Смотрим скелетные меши пешки: у кого назначен меш, кто скрыт/owner-no-see — чтобы понять, почему в TP не видно тело.
import unreal, json, traceback
R = {"err": None, "skeletals": []}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        R["pawn"] = pawn.get_class().get_name() if pawn else None
        if pawn:
            try: R["is_third_person"] = pawn.is_third_person()
            except Exception: pass
            sks = pawn.get_components_by_class(unreal.SkeletalMeshComponent)
            for c in sks:
                info = {"name": c.get_name()}
                try:
                    sm = c.get_editor_property("skeletal_mesh_asset")
                    info["mesh"] = sm.get_name() if sm else None
                except Exception:
                    try:
                        sm = c.get_skeletal_mesh_asset()
                        info["mesh"] = sm.get_name() if sm else None
                    except Exception as e: info["mesh_err"] = str(e)
                try: info["owner_no_see"] = bool(c.get_editor_property("owner_no_see"))
                except Exception as e: info["ons_err"] = str(e)
                try: info["hidden_in_game"] = bool(c.get_editor_property("hidden_in_game"))
                except Exception: pass
                try: info["visible"] = bool(c.is_visible())
                except Exception: pass
                try:
                    loc = c.get_relative_location(); info["rel_z"] = round(loc.z)
                except Exception: pass
                R["skeletals"].append(info)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_inspect_meshes.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("INSPECT_MESHES %s" % json.dumps(R, default=str))
