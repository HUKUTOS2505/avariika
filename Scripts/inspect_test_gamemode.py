# -*- coding: utf-8 -*-
# Смотрим: текущий мир, его GameMode override; и что стоит у BP_GameMode (pawn/controller).
import unreal, json, traceback
R = {"err": None}
def cls(path):
    return unreal.load_object(None, path)
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    w = ues.get_editor_world()
    R["world"] = w.get_name() if w else None
    R["world_path"] = w.get_path_name() if w else None
    ws = w.get_world_settings() if w else None
    if ws:
        try:
            gm = ws.get_editor_property("default_game_mode")
            R["world_gamemode_override"] = gm.get_name() if gm else None
        except Exception as e:
            R["ws_err"] = str(e)
    # BP_GameMode defaults
    base = "/Game/Avariika/blueprinsTest/"
    gen = cls(base + "BP_GameMode.BP_GameMode_C")
    cdo = unreal.get_default_object(gen) if gen else None
    if cdo:
        for p in ("default_pawn_class", "player_controller_class", "hud_class", "game_state_class", "player_state_class"):
            try:
                v = cdo.get_editor_property(p)
                R["gm_"+p] = v.get_name() if v else None
            except Exception as e:
                R["gm_"+p+"_err"] = str(e)
    # существуют ли все три?
    for n in ("BP_GameMode", "BP_Pawn", "BP_PlayerController"):
        R["exists_"+n] = unreal.EditorAssetLibrary.does_asset_exist(base + n)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_test_gamemode.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("TEST_GAMEMODE %s" % json.dumps(R, default=str))
