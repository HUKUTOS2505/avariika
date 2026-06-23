import unreal, json, traceback
R={"err":None}
try:
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world=ues.get_editor_world()
    R["level"]=str(world.get_name())
    ws=unreal.GameplayStatics.get_game_mode(world) if False else None
    # world settings gamemode override
    try:
        wset=world.get_world_settings() if hasattr(world,"get_world_settings") else None
    except Exception: wset=None
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors=eas.get_all_level_actors()
    R["actor_count"]=len(actors)
    # tally by class
    from collections import Counter
    cnt=Counter()
    lights=0; dyn_lights=0; niagara=0; skel=0; heavy=[]
    for a in actors:
        cn=a.get_class().get_name()
        cnt[cn]+=1
        if "Light" in cn: lights+=1
        if "Niagara" in cn or "Emitter" in cn: niagara+=1
        if "SkeletalMesh" in cn: skel+=1
        lbl=a.get_actor_label()
        low=(cn+lbl).lower()
        if any(k in low for k in ["weather","sky","hyper","cloud","volumetric","fog"]): heavy.append(lbl+" ["+cn+"]")
    R["top_classes"]=cnt.most_common(15)
    R["lights"]=lights; R["niagara"]=niagara; R["skeletal_actors"]=skel
    R["weather_sky_actors"]=heavy[:20]
    # rendering features
    try:
        R["lumen_gi"]=unreal.SystemLibrary.get_console_variable_float_value("r.DynamicGlobalIlluminationMethod")
    except Exception as e: R["lumen_gi"]="?"
    try:
        R["sg_overall"]=unreal.SystemLibrary.get_console_variable_int_value("sg.ResolutionQuality")
    except Exception: pass
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_diag_level.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("DIAG_LEVEL done")
