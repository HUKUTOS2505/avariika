import unreal, json, traceback
R={"steps":[],"err":None}
try:
    les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    les.load_level("/Game/Avariika/Maps/L_Dom")
    world=ues.get_editor_world(); R["cur"]=str(world.get_name())
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    R["actor_count"]=len(eas.get_all_level_actors())
    # if L_Dom didn't load (still LV_House), bail
    if "L_Dom" not in R["cur"]:
        R["steps"].append("WARN current is not L_Dom: "+R["cur"])
    # gamemode
    gm=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoGameMode")
    world.get_world_settings().set_editor_property("default_game_mode", gm.generated_class()); R["steps"].append("gamemode Avaryo")
    # remove any leftover gameplay actors from prior run (idempotent)
    for a in list(eas.get_all_level_actors()):
        try:
            if a.get_actor_label() in ("PlayerStart_Dom","Moon_Dim","SkyLight_Dom","Fog_Dom"): eas.destroy_actor(a)
        except Exception: pass
    ps=eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(800,1000,250), unreal.Rotator(0,0,0)); ps.set_actor_label("PlayerStart_Dom")
    def ci(a,cls,v):
        c=a.get_component_by_class(cls)
        if c: c.set_intensity(v)
    dl=eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(800,1000,1500), unreal.Rotator(-50,40,0)); dl.set_actor_label("Moon_Dim"); ci(dl,unreal.DirectionalLightComponent,0.6)
    sky=eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(800,1000,800)); sky.set_actor_label("SkyLight_Dom"); ci(sky,unreal.SkyLightComponent,0.3)
    if not any(a.get_class().get_name()=="SkyAtmosphere" for a in eas.get_all_level_actors()):
        eas.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(800,1000,0))
    fog=eas.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(800,1000,100)); fog.set_actor_label("Fog_Dom")
    R["steps"].append("spawn+lights added")
    les.save_current_level(); R["steps"].append("saved L_Dom")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_setup_ldom.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("SETUP_LDOM done")
