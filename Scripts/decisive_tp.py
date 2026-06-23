# -*- coding: utf-8 -*-
# Решающий замер: тело в TP скрыто (owner_no_see) ИЛИ просто не в кадре (рамка)?
# Считаем угол между forward активной камеры и вектором камера->тело. Если > FOV/2 — тело вне кадра.
import unreal, json, math, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен — нажми Play и встань в 3-е лицо (V)"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        R["pawn"] = pawn.get_class().get_name() if pawn else None
        if pawn:
            try: R["is_third_person"] = bool(pawn.is_third_person())
            except Exception: pass
            # видимость обоих мешей
            R["meshes"] = []
            for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
                m = {"name": c.get_name()}
                for p in ("owner_no_see","only_owner_see","visible","hidden_in_game","render_in_main_pass"):
                    try: m[p] = bool(c.get_editor_property(p))
                    except Exception: pass
                wl = c.get_world_location(); m["world"] = [round(wl.x),round(wl.y),round(wl.z)]
                R["meshes"].append(m)
            # активная камера
            act = None
            for c in pawn.get_components_by_class(unreal.CameraComponent):
                if c.is_active(): act = c; break
            if act:
                cw = act.get_world_location()
                cr = act.get_world_rotation()
                fwd = cr.get_forward_vector()
                fov = float(act.get_editor_property("field_of_view"))
                R["active_cam"] = act.get_name()
                R["cam_world"] = [round(cw.x),round(cw.y),round(cw.z)]
                R["cam_forward"] = [round(fwd.x,2),round(fwd.y,2),round(fwd.z,2)]
                R["cam_pitch"] = round(cr.pitch,1)
                R["fov"] = round(fov,1)
                # вектор камера -> центр тела (берём CharacterMesh0 + ~90см вверх к торсу)
                body = None
                for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
                    if c.get_name() == "CharacterMesh0": body = c; break
                if body:
                    bw = body.get_world_location()
                    torso = unreal.Vector(bw.x, bw.y, bw.z + 90.0)
                    to = unreal.Vector(torso.x-cw.x, torso.y-cw.y, torso.z-cw.z)
                    dist = math.sqrt(to.x**2+to.y**2+to.z**2)
                    if dist > 0:
                        nd = unreal.Vector(to.x/dist, to.y/dist, to.z/dist)
                        dot = fwd.x*nd.x + fwd.y*nd.y + fwd.z*nd.z
                        dot = max(-1.0, min(1.0, dot))
                        ang = math.degrees(math.acos(dot))
                        R["cam_to_body_dist"] = round(dist)
                        R["angle_off_center_deg"] = round(ang,1)
                        R["in_frame_estimate"] = ang < (fov*0.5)  # грубо: половина горизонт. FOV
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_decisive_tp.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("DECISIVE_TP %s" % json.dumps(R, default=str))
