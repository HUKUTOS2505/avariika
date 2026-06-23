import unreal, json, traceback
R={"steps":[],"err":None}
try:
    # --- BP: revert camera to FP + hide own body ---
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    sds=unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    for h in sds.k2_gather_subobject_data_for_blueprint(bp):
        obj=unreal.SubobjectDataBlueprintFunctionLibrary.get_object(sds.k2_find_subobject_data_from_handle(h))
        if not obj: continue
        if obj.get_class().get_name()=="CameraComponent":
            obj.set_editor_property("relative_location", unreal.Vector(0,0,75))
            obj.set_editor_property("relative_rotation", unreal.Rotator(0,0,0))
            try: obj.set_editor_property("use_pawn_control_rotation", True)
            except Exception: pass
            R["steps"].append("camera -> FP head")
        if obj.get_name()=="CharacterMesh0":
            obj.set_editor_property("owner_no_see", True)
            R["steps"].append("body hidden from self (FP)")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp); unreal.EditorAssetLibrary.save_loaded_asset(bp, False)
    # --- LEVEL: fix spawn + solid floors ---
    les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    if "L_Dom" not in str(ues.get_editor_world().get_name()): les.load_level("/Game/Avariika/Maps/L_Dom")
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        lbl=a.get_actor_label()
        if lbl=="PlayerStart_Dom":
            a.set_actor_location(unreal.Vector(800,200,250), False, False); R["steps"].append("spawn z=250")
        if lbl in ("House_Floor","Garage_Floor"):
            a.set_actor_scale3d(unreal.Vector(a.get_actor_scale3d().x, a.get_actor_scale3d().y, 0.4))
            L=a.get_actor_location(); a.set_actor_location(unreal.Vector(L.x,L.y,-20),False,False)
        if lbl=="Ground":
            a.set_actor_scale3d(unreal.Vector(80,80,1.0)); a.set_actor_location(unreal.Vector(800,600,-50),False,False)
    R["steps"].append("floors thickened")
    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_fix_spawn.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("FIX_SPAWN_FP done")
