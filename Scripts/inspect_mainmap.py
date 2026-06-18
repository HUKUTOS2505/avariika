# -*- coding: utf-8 -*-
# Базовый замер ПЕРЕД переносом главной карты: грузим Lvl_FirstPerson, считаем акторов, проверяем WP.
import unreal, json, traceback
R = {"err": None}
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.load_level("/Game/FirstPerson/Lvl_FirstPerson")
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world_path"] = w.get_path_name() if w else None
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = eas.get_all_level_actors()
    R["actor_count"] = len(actors)
    # классы акторов (сводка по типам — чтобы потом сверить)
    from collections import Counter
    cc = Counter(a.get_class().get_name() for a in actors if a)
    R["by_class_top"] = dict(sorted(cc.items(), key=lambda kv: -kv[1])[:25])
    # World Partition?
    try:
        wp = w.get_world_partition() if hasattr(w, "get_world_partition") else None
        R["is_world_partition"] = wp is not None
    except Exception as e:
        R["wp_err"] = str(e)
    # наши ключевые акторы по именам (репейрблы/зоны)
    names = [a.get_actor_label() for a in actors if a]
    R["sample_labels"] = names[:40]
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_mainmap_before.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("MAINMAP_BEFORE %s" % json.dumps(R, default=str))
