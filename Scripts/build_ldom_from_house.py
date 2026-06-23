import unreal, json, traceback
R={"steps":[],"err":None}
try:
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world=ues.get_editor_world()
    R["cur_before"]=str(world.get_name())
    # save current (LV_House) as our L_Dom
    ok=unreal.EditorLoadingAndSavingUtils.save_map(world, "/Game/Avariika/Maps/L_Dom")
    R["steps"].append("save_map L_Dom: "+str(ok))
    world=ues.get_editor_world(); R["cur_after"]=str(world.get_name())
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    # gamemode -> Avaryo
    gm=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoGameMode")
    try: world.get_world_settings().set_editor_property("default_game_mode", gm.generated_class()); R["steps"].append("gamemode Avaryo")
    except Exception as e: R["steps"].append("gm err "+str(e))
    # PlayerStart (center ground, will settle on floor)
    ps=eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(800,1000,250), unreal.Rotator(0,0,0))
    ps.set_actor_label("PlayerStart_Dom"); R["steps"].append("playerstart @ (800,1000,250)")
    # dark horror light: dim directional + skylight + atmosphere + fog
    def comp_intensity(actor, cls, val):
        c=actor.get_component_by_class(cls)
        if c: c.set_intensity(val); return True
        return False
    dl=eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(800,1000,1500), unreal.Rotator(-50,40,0)); dl.set_actor_label("Moon_Dim")
    R["steps"].append("dir intensity "+str(comp_intensity(dl, unreal.DirectionalLightComponent, 0.6)))
    sky=eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(800,1000,800)); sky.set_actor_label("SkyLight_Dom")
    R["steps"].append("sky intensity "+str(comp_intensity(sky, unreal.SkyLightComponent, 0.3)))
    eas.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(800,1000,0))
    fog=eas.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(800,1000,100)); fog.set_actor_label("Fog_Dom")
    les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem); les.save_current_level()
    R["steps"].append("saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_ldom.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("BUILD_LDOM done")
