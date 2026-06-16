# -*- coding: utf-8 -*-
# PIE-проверка газоанализатора: ломаем газовую трубу (утечка), телепортируем игрока
# в облако, читаем GetGasReading() — должно быть > 0.
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        cls = unreal.load_class(None, "/Script/Avaryo.Repairable")
        reps = unreal.GameplayStatics.get_all_actors_of_class(gw, cls)
        # газовая труба — ближайшая к [600,40,210]
        pipe = None; best = 1e18
        target = unreal.Vector(600.0, 40.0, 210.0)
        for a in reps:
            d = (a.get_actor_location() - target).length()
            if d < best:
                best = d; pipe = a
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        R["pawn_class"] = pawn.get_class().get_name() if pawn else None
        if not pawn or not hasattr(pawn, "get_gas_reading"):
            R["err"] = "пешка не оператор (%s) — PIE не готов" % R.get("pawn_class")
            raise RuntimeError(R["err"])
        R["pipe_loc"] = None
        if pipe:
            pipe.set_broken(True)  # утечка пошла
            loc = pipe.get_actor_location()
            R["pipe_loc"] = [round(loc.x), round(loc.y), round(loc.z)]
            R["pipe_leaking"] = bool(pipe.is_leaking_gas())
            R["pipe_gas_radius"] = pipe.get_current_gas_radius()
            pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
            if pawn:
                pawn.set_actor_location(unreal.Vector(loc.x + 80.0, loc.y, loc.z + 30.0), False, False)
                R["reading_near"] = round(pawn.get_gas_reading(), 3)
                pawn.set_actor_location(unreal.Vector(loc.x + 2000.0, loc.y, loc.z + 30.0), False, False)
                R["reading_far"] = round(pawn.get_gas_reading(), 3)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_verify_gas.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("VERIFY_GAS %s" % json.dumps(R, default=str))
