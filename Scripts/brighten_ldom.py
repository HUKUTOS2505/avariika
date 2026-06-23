import unreal, json, traceback
R={"steps":[],"err":None}
try:
    les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if "L_Dom" not in str(ues.get_editor_world().get_name()):
        les.load_level("/Game/Avariika/Maps/L_Dom")
    for a in eas.get_all_level_actors():
        lbl=a.get_actor_label(); cn=a.get_class().get_name()
        if cn=="DirectionalLight":
            c=a.get_component_by_class(unreal.DirectionalLightComponent)
            if c:
                c.set_intensity(50000.0)
                try: c.set_editor_property("atmosphere_sun_light", True)
                except Exception as e: R["steps"].append("atmsun "+str(e))
                R["steps"].append("dir 50000 ("+lbl+")")
        elif cn=="SkyLight":
            c=a.get_component_by_class(unreal.SkyLightComponent)
            if c:
                try: c.set_editor_property("real_time_capture", True)
                except Exception: pass
                c.set_intensity(1.0)
                try: c.recapture_sky()
                except Exception: pass
                R["steps"].append("skylight rtc+1 ("+lbl+")")
        elif cn=="ExponentialHeightFog":
            c=a.get_component_by_class(unreal.ExponentialHeightFogComponent)
            if c:
                try: c.set_editor_property("fog_density", 0.02)
                except Exception: pass
                R["steps"].append("fog low")
    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_brighten.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("BRIGHTEN done")
