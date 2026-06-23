import unreal, json, traceback
R={"skeletal":[],"cameras":[],"static":[],"err":None}
try:
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    bpc=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter").generated_class()
    a=eas.spawn_actor_from_class(bpc, unreal.Vector(0,0,5000), unreal.Rotator(0,0,0))
    for c in a.get_components_by_class(unreal.SkeletalMeshComponent):
        m=c.get_editor_property("skeletal_mesh")
        R["skeletal"].append({"comp":c.get_name(),"mesh":str(m),"owner_no_see":c.get_editor_property("only_owner_see") if False else c.get_editor_property("bOwnerNoSee") if False else None,
                              "ono":c.get_editor_property("only_owner_see"),"ons":c.get_editor_property("owner_no_see")})
    for c in a.get_components_by_class(unreal.CameraComponent):
        R["cameras"].append({"comp":c.get_name(),"loc":str(c.get_relative_location())})
    for c in a.get_components_by_class(unreal.StaticMeshComponent):
        R["static"].append(c.get_name())
    eas.destroy_actor(a)
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_char_runtime.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("SPAWN_INSPECT done")
