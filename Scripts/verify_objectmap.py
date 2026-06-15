# -*- coding: utf-8 -*-
import unreal, json
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
w = ues.get_editor_world()
R = {"map": w.get_path_name() if w else "?"}
cnt = {}
labels_exit = []
for a in eas.get_all_level_actors():
    c = a.get_class().get_name()
    cnt[c] = cnt.get(c, 0) + 1
    if c == "ExitZone":
        labels_exit.append(a.get_actor_label())
R["ExitZone"] = cnt.get("ExitZone", 0)
R["Repairable"] = cnt.get("Repairable", 0)
R["exit_labels"] = labels_exit
R["interesting"] = {k: v for k, v in cnt.items()
                    if k in ("ExitZone", "Repairable", "CallBoard", "ToolCase", "PlayerStart")
                    or "azelle" in k or "Truck" in k}
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_verify_obj.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("VERIFY_OBJ %s" % json.dumps(R, default=str))
