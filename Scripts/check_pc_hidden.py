# -*- coding: utf-8 -*-
# Тело кидает тень, но не рисуется и НЕ через owner_no_see => скорее всего скрыто через
# список скрытых примитивов/актёров на PlayerController. Проверяем + чиним cast_shadow обратно.
import unreal, json, traceback
R = {"err": None}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pc = unreal.GameplayStatics.get_player_controller(gw, 0)
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        R["pc"] = pc.get_class().get_name() if pc else None
        # вернуть тень телу
        for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
            if c.get_name() == "CharacterMesh0":
                try: c.set_cast_shadow(True)
                except Exception: pass
        # читаем возможные списки скрытия на PC
        for prop in ("hidden_actors", "hidden_primitive_components"):
            try:
                v = pc.get_editor_property(prop)
                R[prop] = [str(x) for x in v] if v else []
            except Exception as e:
                R[prop+"_err"] = str(e)
        # актёр-пешка скрыт?
        try: R["pawn_hidden"] = bool(pawn.get_editor_property("hidden"))
        except Exception:
            try: R["pawn_hidden"] = bool(pawn.is_hidden_ed())
            except Exception: pass
        # есть ли SceneCapture, который мог утянуть тело в hidden? просто перечислим вид
        try:
            R["view_target"] = pc.get_view_target().get_name() if pc.get_view_target() else None
        except Exception as e: R["vt_err"] = str(e)
        # local player / debug camera?
        try:
            R["is_in_debug_cam"] = bool(pc.get_editor_property("cheat_manager") is not None)
        except Exception: pass
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_pc_hidden.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("PC_HIDDEN %s" % json.dumps(R, default=str))
