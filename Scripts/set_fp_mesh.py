import unreal, json, traceback
R={"steps":[],"err":None}
try:
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    hz=unreal.load_asset("/Game/hazmat/Mesh/hazmat.hazmat")
    abp=unreal.load_asset("/Game/Hospital/Free_Content_Epic_Games/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP")
    sds=unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    handles=sds.k2_gather_subobject_data_for_blueprint(bp)
    R["components"]=[]
    for h in handles:
        d=sds.k2_find_subobject_data_from_handle(h)
        obj=unreal.SubobjectDataBlueprintFunctionLibrary.get_object(d)
        if not obj: continue
        nm=obj.get_name()
        R["components"].append(nm)
        if "FirstPersonMesh" in nm:
            R["fp_before"]={"mesh":str(obj.get_editor_property("skeletal_mesh")),"anim":str(obj.get_editor_property("anim_class"))}
            obj.set_editor_property("skeletal_mesh", hz)
            obj.set_editor_property("anim_class", abp.generated_class())
            try: obj.set_editor_property("animation_mode", unreal.AnimationMode.ANIMATION_BLUEPRINT)
            except Exception: pass
            R["fp_after"]={"mesh":str(obj.get_editor_property("skeletal_mesh")),"anim":str(obj.get_editor_property("anim_class"))}
            R["steps"].append("FirstPersonMesh -> hazmat")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.EditorAssetLibrary.save_loaded_asset(bp, False)
    R["steps"].append("compiled+saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_fp_mesh.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("SET_FP_MESH done")
