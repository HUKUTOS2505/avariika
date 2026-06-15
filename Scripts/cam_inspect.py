# -*- coding: utf-8 -*-
import unreal, json
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
gw = ues.get_game_world()
R = {}
if gw:
    pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
    if pawn:
        try: R["is_third_person"] = pawn.is_third_person()
        except Exception as e: R["itp_err"] = str(e)
        cams = pawn.get_components_by_class(unreal.CameraComponent)
        R["cameras"] = []
        for c in cams:
            R["cameras"].append({"name": c.get_name(), "active": c.is_active()})
        try: R["bFindCamWhenVT"] = pawn.get_editor_property("find_camera_component_when_view_target")
        except Exception as e: R["fc_err"] = str(e)
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_cam_inspect.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("CAM_INSPECT %s" % json.dumps(R, default=str))
