# -*- coding: utf-8 -*-
# ВОССТАНОВЛЕНИЕ: build_hub.py по ошибке заспавнил хаб-актёры в ТЕКУЩИЙ уровень
# (Lvl_FirstPerson), т.к. new_level в живом редакторе не переключил мир.
# Этот скрипт удаляет ТОЛЬКО хаб-актёры из текущего уровня и сохраняет его.
# Критерии «мой хаб-актёр» (любой из):
#   - tag == "HUB" / "VAN"
#   - label начинается с "Hub_" или == "CallBoard_Hub"/"PlayerStart_Hub"
#   - класс ACallBoard
#   - SkyAtmosphere/ExponentialHeightFog/DirectionalLight/SkyLight рядом с (800,550) — мои источники
import unreal, json, traceback

R = {"deleted": [], "kept_classes": {}, "count": 0, "err": None}
try:
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = eas.get_all_level_actors()

    HUB_TAGS = {"HUB", "VAN"}
    HUB_LABELS_EXACT = {"CallBoard_Hub", "PlayerStart_Hub"}
    NEAR_X, NEAR_Y, TOL = 800.0, 550.0, 60.0

    def is_mine(a):
        try:
            label = a.get_actor_label()
        except Exception:
            label = ""
        # тег
        try:
            tags = [str(t) for t in a.tags]
        except Exception:
            tags = []
        if any(t in HUB_TAGS for t in tags):
            return True
        if label.startswith("Hub_") or label in HUB_LABELS_EXACT:
            return True
        cls = a.get_class().get_name()
        if cls == "CallBoard":
            return True
        # незалейбленные атмосфера/туман/свет, которые я заспавнил у (800,550)
        if cls in ("SkyAtmosphere", "ExponentialHeightFog", "DirectionalLight", "SkyLight"):
            loc = a.get_actor_location()
            if abs(loc.x - NEAR_X) < TOL and abs(loc.y - NEAR_Y) < TOL:
                return True
        return False

    to_delete = [a for a in actors if is_mine(a)]
    for a in to_delete:
        try:
            R["deleted"].append("%s | %s" % (a.get_actor_label(), a.get_class().get_name()))
            eas.destroy_actor(a)
            R["count"] += 1
        except Exception as e:
            R["deleted"].append("FAIL %s: %s" % (a.get_actor_label(), e))

    # сохранить текущий уровень (удаление внешних актёров запишется на диск)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.save_current_level()
    R["saved"] = True
    R["remaining_total"] = len(eas.get_all_level_actors())
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()

with open(r"C:/unrealEngine/avariika/Scripts/manifests/_clean_fp.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("CLEAN_FP done deleted=%d err=%s" % (R["count"], R["err"]))
