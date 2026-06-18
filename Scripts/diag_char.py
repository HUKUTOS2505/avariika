import unreal, json, traceback
R={"components":[],"err":None}
try:
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    cdo=unreal.get_default_object(bp.generated_class())
    # mesh visibility flags
    mesh=cdo.get_editor_property("mesh")
    R["mesh"]={
        "skeletal_mesh": str(mesh.get_editor_property("skeletal_mesh")),
        "owner_no_see": mesh.get_editor_property("only_owner_see") if False else None,
    }
    for prop in ["bOwnerNoSee","owner_no_see","bOnlyOwnerSee","only_owner_see","hidden_in_game","visible"]:
        try: R["mesh"][prop]=mesh.get_editor_property(prop)
        except Exception: pass
    R["mesh"]["rel_location"]=str(mesh.get_editor_property("relative_location"))
    # list all components via the BP CDO
    comps=[]
    try:
        for c in cdo.get_components_by_class(unreal.SceneComponent):
            comps.append({"name":c.get_name(),"class":c.get_class().get_name(),"loc":str(c.get_editor_property("relative_location"))})
    except Exception as e: R["comp_err"]=str(e)
    R["components"]=comps
    # camera / springarm presence
    R["has_camera"]=any("Camera" in c["class"] for c in comps)
    R["has_springarm"]=any("SpringArm" in c["class"] for c in comps)
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_diag_char.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("DIAG_CHAR done")
