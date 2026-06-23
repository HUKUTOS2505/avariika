import unreal
import traceback

out = "C:/unrealEngine/avariika/Saved/reparent_pc.txt"
lines = []


def flush():
    with open(out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))


try:
    path = "/Game/FirstPerson/Blueprints/BP_FirstPersonPlayerController"
    bp = unreal.load_asset(path)
    if not bp:
        lines.append("BP not found"); flush(); raise SystemExit

    unreal.BlueprintEditorLibrary.reparent_blueprint(bp, unreal.AvaryoPlayerController)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    saved = unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
    lines.append("reparented + compiled + saved=%s" % saved)

    gc_after = unreal.load_object(None, path + ".BP_FirstPersonPlayerController_C")
    cdo = unreal.get_default_object(gc_after) if gc_after else None
    is_child = isinstance(cdo, unreal.AvaryoPlayerController) if cdo else False
    lines.append("is child of AvaryoPlayerController: %s" % is_child)
    lines.append("DONE")
    flush()
except Exception:
    lines.append("EXC:\n" + traceback.format_exc())
    flush()
