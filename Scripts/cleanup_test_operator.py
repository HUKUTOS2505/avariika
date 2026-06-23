import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/cleanup_test_operator.json"
R = {"removed": [], "level": None, "err": None}
try:
    R["level"] = str(unreal.EditorLevelLibrary.get_editor_world().get_name())
    sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in list(sub.get_all_level_actors()):
        lbl = a.get_actor_label()
        if lbl in ("TestOperator", "TestOperatorLight"):
            sub.destroy_actor(a)
            R["removed"].append(lbl)
    unreal.EditorLoadingAndSavingUtils.save_current_level()
except Exception as e:
    R["err"] = str(e)
with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("CLEANUP_TEST_OP: " + json.dumps(R))
