# -*- coding: utf-8 -*-
# Тело в TP не рисуется (видна только тень). Гипотеза: камера ВНУТРИ меша (бэкфейс-куллинг).
# Снимаем мировые позиции камеры/меша, длину пружины, FOV, габариты меша — считаем расстояние.
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        R["pawn"] = pawn.get_class().get_name() if pawn else None
        if pawn:
            ploc = pawn.get_actor_location()
            R["pawn_loc"] = [round(ploc.x), round(ploc.y), round(ploc.z)]
            # Камеры
            cams = pawn.get_components_by_class(unreal.CameraComponent)
            R["cameras"] = []
            for c in cams:
                wl = c.get_world_location()
                R["cameras"].append({
                    "name": c.get_name(),
                    "active": bool(c.is_active()),
                    "world": [round(wl.x), round(wl.y), round(wl.z)],
                    "fov": round(float(c.get_editor_property("field_of_view")), 1),
                })
            # Пружина
            arms = pawn.get_components_by_class(unreal.SpringArmComponent)
            R["springs"] = []
            for a in arms:
                try:
                    sl = a.get_socket_location("SpringEndpoint")
                except Exception:
                    sl = None
                R["springs"].append({
                    "name": a.get_name(),
                    "target_len": round(float(a.get_editor_property("target_arm_length")), 1),
                    "do_collision": bool(a.get_editor_property("do_collision_test")),
                    "endpoint_world": ([round(sl.x), round(sl.y), round(sl.z)] if sl else None),
                })
            # Тело (CharacterMesh0): мировые габариты + расстояние от активной камеры
            body = pawn.get_editor_property("mesh") if hasattr(pawn, "mesh") else None
            try:
                body = pawn.get_components_by_class(unreal.SkeletalMeshComponent)[0]
            except Exception:
                pass
            for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
                if c.get_name() == "CharacterMesh0":
                    body = c; break
            if body:
                bl = body.get_world_location()
                R["body_name"] = body.get_name()
                R["body_world"] = [round(bl.x), round(bl.y), round(bl.z)]
                R["body_only_owner_see"] = bool(body.get_editor_property("only_owner_see"))
                R["body_render_main"] = bool(body.get_editor_property("render_in_main_pass"))
                R["body_owner_no_see"] = bool(body.get_editor_property("owner_no_see"))
                try:
                    bounds = body.bounds  # FBoxSphereBounds
                    o = bounds.origin; ext = bounds.box_extent
                    R["body_bounds_origin"] = [round(o.x), round(o.y), round(o.z)]
                    R["body_bounds_extent"] = [round(ext.x), round(ext.y), round(ext.z)]
                except Exception as e:
                    R["bounds_err"] = str(e)
                # расстояние активной камеры до центра тела
                act = [c for c in cams if c.is_active()]
                if act and "body_bounds_origin" in R:
                    cw = act[0].get_world_location()
                    o = R["body_bounds_origin"]
                    import math
                    d = math.sqrt((cw.x-o[0])**2 + (cw.y-o[1])**2 + (cw.z-o[2])**2)
                    R["cam_to_body_center"] = round(d)
                    R["active_cam"] = act[0].get_name()
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_diag_tp.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("DIAG_TP %s" % json.dumps(R, default=str))
