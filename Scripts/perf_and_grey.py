import unreal, json, traceback
R={"applied":[],"err":None}
try:
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world=ues.get_editor_world()
    def cmd(c):
        unreal.SystemLibrary.execute_console_command(world, c); R["applied"].append(c)
    # reversible perf cvars (test GPU bottleneck)
    cmd("r.VolumetricCloud 0")
    cmd("sg.GlobalIlluminationQuality 1")
    cmd("sg.ShadowQuality 2")
    cmd("sg.EffectsQuality 2")
    cmd("sg.PostProcessQuality 2")
    cmd("r.VolumetricFog 0")
    # gamemode override of current level (to know if grey spawns)
    try:
        wset=world.get_world_settings()
        gm=wset.get_editor_property("default_game_mode")
        R["level_gamemode_override"]=str(gm)
    except Exception as e:
        R["gm_err"]=str(e)
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_perf_grey.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("PERF_GREY done")
