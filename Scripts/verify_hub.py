# -*- coding: utf-8 -*-
import unreal, json
R = {}
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
w = ues.get_editor_world()
R["map"] = w.get_path_name() if w else "?"
actors = eas.get_all_level_actors()
R["total"] = len(actors)
boards, lights, ps = [], 0, 0
for a in actors:
    cls = a.get_class().get_name()
    if cls == "CallBoard":
        info = {"label": a.get_actor_label()}
        try: info["hub_override"] = a.get_editor_property("hub_map_override")
        except Exception as e: info["hub_override_err"] = str(e)
        try: info["calls"] = len(a.get_editor_property("calls"))
        except Exception as e: info["calls_err"] = str(e)
        boards.append(info)
    elif cls in ("DirectionalLight", "SkyLight", "SkyAtmosphere", "ExponentialHeightFog"):
        lights += 1
    elif cls == "PlayerStart":
        ps += 1
R["call_boards"] = boards
R["light_actors"] = lights
R["player_starts"] = ps
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_verify_hub.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("VERIFY_HUB %s" % json.dumps(R, default=str))
