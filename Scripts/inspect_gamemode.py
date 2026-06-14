import unreal
import traceback

out = "D:/unrealEngine/avariika/Saved/inspect_gamemode.txt"
lines = []


def flush():
    with open(out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))


def cls_name(c):
    return c.get_name() if c else "None"


try:
    gm_path = "/Game/Avariika/Blueprints/BP_AvaryoGameMode"
    name = gm_path.split('/')[-1]
    gc = unreal.load_object(None, gm_path + "." + name + "_C")
    if not gc:
        bp = unreal.load_asset(gm_path)
        gc = bp.generated_class() if bp else None
    lines.append("GameMode class: %s" % cls_name(gc))
    if gc:
        cdo = unreal.get_default_object(gc)
        for p in ["player_controller_class", "default_pawn_class", "hud_class", "game_state_class", "use_seamless_travel"]:
            try:
                v = cdo.get_editor_property(p)
                lines.append("  %s = %s" % (p, cls_name(v) if hasattr(v, 'get_name') else v))
            except Exception as e:
                lines.append("  %s ERR %s" % (p, e))
    flush()
except Exception:
    lines.append("EXC:\n" + traceback.format_exc())
    flush()
