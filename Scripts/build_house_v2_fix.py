# -*- coding: utf-8 -*-
# Фикс L_Dom: реальные габариты дома считаем по ВСЕМ акторам-кускам (House01 = ~600 child-акторов,
# родитель давал крошечную коробку). Переставляем землю и PlayerStart по реальному центру/полу,
# выставляем GameMode = BP_AvaryoGameMode через world.get_world_settings(). ГАРД на L_Dom.
import unreal, json, traceback
R = {"err": None, "steps": []}
def log(s): R["steps"].append(s); unreal.log("HOUSEFIX %s" % s)
try:
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les.load_level("/Game/Avariika/Maps/L_Dom")
    w = ues.get_editor_world()
    if not (w and "L_Dom" in w.get_name()):
        R["err"] = "GUARD: не на L_Dom"; raise RuntimeError(R["err"])
    SKIP = (unreal.DirectionalLight, unreal.SkyLight, unreal.SkyAtmosphere, unreal.ExponentialHeightFog)
    INF = 1e9
    mnx = mny = mnz = INF; mxx = mxy = mxz = -INF
    ground = None; pstart = None; n = 0
    for a in eas.get_all_level_actors():
        if not a: continue
        if a.get_actor_label() == "Ground": ground = a; continue
        if isinstance(a, unreal.PlayerStart): pstart = a; continue
        if isinstance(a, SKIP): continue
        try: o, e = a.get_actor_bounds(False)
        except Exception: continue
        if e.x <= 1 and e.y <= 1 and e.z <= 1: continue
        mnx = min(mnx, o.x - e.x); mny = min(mny, o.y - e.y); mnz = min(mnz, o.z - e.z)
        mxx = max(mxx, o.x + e.x); mxy = max(mxy, o.y + e.y); mxz = max(mxz, o.z + e.z); n += 1
    R["measured_actors"] = n
    if n == 0 or mnx >= INF:
        R["err"] = "no house bounds measured"; raise RuntimeError(R["err"])
    cx = (mnx + mxx) / 2.0; cy = (mny + mxy) / 2.0; floor = mnz
    R["envelope_min"] = [round(mnx), round(mny), round(mnz)]
    R["envelope_max"] = [round(mxx), round(mxy), round(mxz)]
    R["size_m"] = [round((mxx - mnx) / 100.0, 1), round((mxy - mny) / 100.0, 1), round((mxz - mnz) / 100.0, 1)]
    R["center_floor"] = [round(cx), round(cy), round(floor)]

    if ground:
        ground.set_actor_location(unreal.Vector(cx, cy, floor - 2.0), False, False)
        span = max(mxx - mnx, mxy - mny)
        s = (span / 100.0) * 1.6 + 6.0  # plane=1м, с запасом
        ground.set_actor_scale3d(unreal.Vector(s, s, 1.0))
        log("ground @center floor-2, scale=%.1f (span=%.0f)" % (s, span))
    if pstart:
        pstart.set_actor_location(unreal.Vector(cx, cy, floor + 100.0), False, False)
        log("PlayerStart @center floor+100")

    gm = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoGameMode.BP_AvaryoGameMode_C")
    ws = None
    try: ws = w.get_world_settings()
    except Exception as we: R["ws_err"] = str(we)
    if ws and gm:
        ws.set_editor_property("default_game_mode", gm); log("GameMode set")
    else:
        log("GameMode NOT set (ws=%s gm=%s)" % (bool(ws), bool(gm)))

    les.save_current_level(); R["saved"] = True
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_house_fix.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("HOUSEFIX DONE %s" % json.dumps(R, default=str))
