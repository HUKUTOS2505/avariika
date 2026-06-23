# -*- coding: utf-8 -*-
# Чистим пустой остаток moduleHouse + проверяем целостность рабочей карты и сцену воды.
import unreal, json, traceback
R = {"err": None}
try:
    eal = unreal.EditorAssetLibrary
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    # пустой moduleHouse — снести
    if eal.does_directory_exist("/Game/moduleHouse"):
        n = len(ar.get_assets_by_path("/Game/moduleHouse", recursive=True, include_only_on_disk_assets=False))
        if n == 0:
            eal.delete_directory("/Game/moduleHouse"); R["moduleHouse_removed"] = True
        else:
            R["moduleHouse_left"] = n
    # текущая карта
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world"] = w.get_path_name() if w else None
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = eas.get_all_level_actors()
    R["actor_count"] = len(actors)
    names = set(a.get_actor_label() for a in actors if a)
    R["water_scene"] = {
        "Repairable_WaterPipe": "Repairable_WaterPipe" in names,
        "WaterPowerSwitch": "WaterPowerSwitch" in names,
        "RubberBoots": "RubberBoots" in names,
    }
    # флудит ли труба
    for a in actors:
        if a and a.get_actor_label() == "Repairable_WaterPipe":
            try: R["pipe_flooding"] = bool(a.is_flooding())
            except Exception: pass
            try: R["pipe_broken"] = bool(a.is_broken())
            except Exception: pass
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_verify_final.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("VERIFY_FINAL %s" % json.dumps(R, default=str))
