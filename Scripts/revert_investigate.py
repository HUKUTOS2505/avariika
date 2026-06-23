import unreal, json, traceback
R={"steps":[],"err":None}
try:
    eal=unreal.EditorAssetLibrary
    # revert anim class to ABP_Unarmed
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    unarmed=unreal.load_asset("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed.ABP_Unarmed")
    gen=bp.generated_class(); cdo=unreal.get_default_object(gen); mesh=cdo.get_editor_property("mesh")
    if unarmed:
        mesh.set_editor_property("anim_class", unarmed.generated_class())
        R["reverted_to"]=str(mesh.get_editor_property("anim_class"))
    try: unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    except Exception as e: R["steps"].append("compile "+str(e))
    eal.save_loaded_asset(bp, False); R["steps"].append("reverted+saved")
    # investigate: which pawn does the game actually use?
    gm=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoGameMode")
    if gm:
        gcdo=unreal.get_default_object(gm.generated_class())
        try: R["avaryo_gm_default_pawn"]=str(gcdo.get_editor_property("default_pawn_class"))
        except Exception as e: R["avaryo_gm_default_pawn"]="err "+str(e)
    fpgm=unreal.load_asset("/Game/FirstPerson/Blueprints/BP_FirstPersonGameMode")
    if fpgm:
        fcdo=unreal.get_default_object(fpgm.generated_class())
        try: R["fp_gm_default_pawn"]=str(fcdo.get_editor_property("default_pawn_class"))
        except Exception as e: R["fp_gm_default_pawn"]="err "+str(e)
    # current level + its worldsettings gamemode override
    try:
        les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        R["current_level"]=str(les.get_current_level()) if hasattr(les,"get_current_level") else "?"
    except Exception as e: R["current_level"]="err "+str(e)
    # project default gamemode
    try:
        R["project_gamemode"]=unreal.SystemLibrary.get_project_setting("/Script/EngineSettings.GameMapsSettings","GlobalDefaultGameMode") if hasattr(unreal.SystemLibrary,"get_project_setting") else "n/a"
    except Exception as e: R["project_gamemode"]="err "+str(e)
    # BP_AvaryoCharacter current mesh
    R["avaryo_char_mesh"]=str(mesh.get_editor_property("skeletal_mesh"))
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_revert_investigate.json","w") as f: json.dump(R,f,indent=1)
unreal.log("REVERT_INVESTIGATE done")
