# -*- coding: utf-8 -*-
# Перенос через AssetTools.rename_assets (как Content Browser) — корректно тащит внешние акторы OFPA.
import unreal, json, traceback
R = {"err": None, "expected_actors": 94}
SRC = "/Game/FirstPerson/Lvl_FirstPerson"
DST = "/Game/Avariika/Maps/Lvl_FirstPerson"
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.load_level("/Game/Avariika/Maps/Test")  # уйти с переносимой карты
    at = unreal.AssetToolsHelpers.get_asset_tools()
    asset = unreal.load_asset(SRC)
    R["asset"] = asset.get_name() if asset else None
    try:
        rd = unreal.AssetRenameData(asset=asset, new_package_path="/Game/Avariika/Maps", new_name="Lvl_FirstPerson")
    except Exception as e1:
        R["ctor_kwargs_err"] = str(e1)
        rd = unreal.AssetRenameData(asset, "/Game/Avariika/Maps", "Lvl_FirstPerson")
    at.rename_assets([rd])
    R["dst_exists"] = unreal.EditorAssetLibrary.does_asset_exist(DST)
    R["src_exists"] = unreal.EditorAssetLibrary.does_asset_exist(SRC)
    if R["dst_exists"]:
        les.load_level(DST)
        ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        R["world_after"] = ues.get_editor_world().get_path_name()
        eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        R["actor_count_after"] = len(eas.get_all_level_actors())
        R["match"] = (R["actor_count_after"] == R["expected_actors"])
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_move_mainmap2.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("MOVE_MAINMAP2 %s" % json.dumps(R, default=str))
