import unreal, json, traceback
p = "C:/unrealEngine/avariika/Saved/stair_fix.json"
out = {}
try:
    path = "/Game/ResidentialHouses/Meshes/Architecture/Stairs/SM_Staircase1"
    m = unreal.load_asset(path)
    bs = m.get_editor_property("body_setup")
    out["before_trace_flag"] = str(bs.get_editor_property("collision_trace_flag"))
    bs.set_editor_property("collision_trace_flag", unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
    out["after_trace_flag"] = str(bs.get_editor_property("collision_trace_flag"))
    m.modify()
    bs.modify()
    saved = unreal.EditorAssetLibrary.save_loaded_asset(m, only_if_is_dirty=False)
    out["saved"] = bool(saved)
except Exception:
    out["fatal"] = traceback.format_exc()
f = open(p, "w"); f.write(json.dumps(out, indent=2)); f.close()
