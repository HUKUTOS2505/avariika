import unreal, json, traceback
R={"steps":[],"err":None}
try:
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    sds=unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    for h in sds.k2_gather_subobject_data_for_blueprint(bp):
        obj=unreal.SubobjectDataBlueprintFunctionLibrary.get_object(sds.k2_find_subobject_data_from_handle(h))
        if obj and "FirstPersonMesh" in obj.get_name():
            obj.set_editor_property("visible", False)
            try: obj.set_editor_property("hidden_in_game", True)
            except Exception: pass
            R["steps"].append("FirstPersonMesh hidden")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp); unreal.EditorAssetLibrary.save_loaded_asset(bp, False)
    R["steps"].append("saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_hide_fp.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("HIDE_FP done")
