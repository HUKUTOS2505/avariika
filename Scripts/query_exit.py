# -*- coding: utf-8 -*-
import unreal, json
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
R = {"exit": None, "playerstarts": [], "repairables": []}
def s(v): return [round(v.x), round(v.y), round(v.z)]
for a in eas.get_all_level_actors():
    c = a.get_class().get_name()
    if c == "ExitZone":
        info = {"label": a.get_actor_label(), "loc": s(a.get_actor_location())}
        try:
            box = a.get_editor_property("Zone")
            if box:
                ext = box.get_unscaled_box_extent()
                info["box_extent"] = s(ext)
        except Exception as e:
            info["box_err"] = str(e)
        R["exit"] = info
    elif c == "PlayerStart":
        R["playerstarts"].append({"label": a.get_actor_label(), "loc": s(a.get_actor_location())})
    elif c == "Repairable":
        R["repairables"].append({"label": a.get_actor_label(), "loc": s(a.get_actor_location())})
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_query_exit.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("QUERY_EXIT %s" % json.dumps(R, default=str))
