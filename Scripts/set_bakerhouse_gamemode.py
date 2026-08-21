import unreal, json, traceback
rep = {}
try:
    ues = unreal.UnrealEditorSubsystem()
    world = ues.get_editor_world()
    rep["world"] = world.get_name() if world else None
    if not world or world.get_name() != "L_BakerHouse":
        rep["ABORT"] = "wrong world: %r" % (world.get_name() if world else None)
    else:
        ar = unreal.AssetRegistryHelpers.get_asset_registry()
        gm = None; ch = None
        for a in ar.get_assets_by_path(unreal.Name("/Game"), recursive=True):
            n = str(a.package_name).split("/")[-1]
            if n == "BP_AvaryoGameMode":
                gm = str(a.package_name)
            elif n == "BP_AvaryoCharacter":
                ch = str(a.package_name)
        rep["gm_path"] = gm
        rep["char_path"] = ch
        if not gm:
            rep["note"] = "BP_AvaryoGameMode NOT found"
        else:
            gmbp = unreal.load_asset(gm)
            gmclass = gmbp.generated_class()
            ws = world.get_world_settings()
            ws.set_editor_property("default_game_mode", gmclass)
            rep["set_gm"] = str(gmclass)
            unreal.LevelEditorSubsystem().save_current_level()
            rep["saved"] = True
except Exception:
    rep["fatal"] = traceback.format_exc()
open("C:/unrealEngine/avariika/Saved/set_gamemode.json", "w").write(json.dumps(rep, indent=2))
