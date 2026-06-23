import unreal, json, traceback
R={"steps":[],"err":None}
try:
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    sds=unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    for h in sds.k2_gather_subobject_data_for_blueprint(bp):
        obj=unreal.SubobjectDataBlueprintFunctionLibrary.get_object(sds.k2_find_subobject_data_from_handle(h))
        if not obj: continue
        nm=obj.get_name()
        if obj.get_class().get_name()=="CameraComponent":
            obj.set_editor_property("relative_location", unreal.Vector(-300,0,90))
            obj.set_editor_property("relative_rotation", unreal.Rotator(-8,0,0))
            try: obj.set_editor_property("use_pawn_control_rotation", True)
            except Exception: pass
            R["steps"].append("camera -> behind (TP)")
        if nm=="CharacterMesh0":
            obj.set_editor_property("owner_no_see", False)
            R["steps"].append("CharacterMesh0 visible to owner")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp); unreal.EditorAssetLibrary.save_loaded_asset(bp, False)
    R["steps"].append("saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_tp.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("TP_CAMERA done")
