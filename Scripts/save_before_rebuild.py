# save_before_rebuild.py — сохранить работу юзера ПЕРЕД закрытием редактора под ребилд.
import unreal, json
res = {"level": False, "dirty": False}
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    res["level"] = bool(les.save_current_level())
except Exception as e:
    res["level_err"] = str(e)
try:
    res["dirty"] = bool(unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
except Exception as e:
    res["dirty_err"] = str(e)
open(r"C:\unrealEngine\avariika\Saved\save_before_rebuild.json", "w").write(json.dumps(res))
