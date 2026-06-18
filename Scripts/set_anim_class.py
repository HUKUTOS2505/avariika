import unreal, json, traceback
R={"steps":[],"err":None}
try:
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    abp=unreal.load_asset("/Game/Characters/Operator/Locomotion/ABP_Operator.ABP_Operator")
    R["have"]={"bp":bool(bp),"abp":bool(abp)}
    abp_cls=abp.generated_class() if abp else None
    gen=bp.generated_class()
    cdo=unreal.get_default_object(gen)
    mesh=cdo.get_editor_property("mesh")  # inherited ACharacter Mesh (SkeletalMeshComponent)
    R["before"]={
        "skeletal_mesh": str(mesh.get_editor_property("skeletal_mesh")) if mesh else None,
        "anim_class": str(mesh.get_editor_property("anim_class")) if mesh else None,
    }
    # set anim class + mode
    mesh.set_editor_property("anim_class", abp_cls)
    try: mesh.set_editor_property("animation_mode", unreal.AnimationMode.ANIMATION_BLUEPRINT)
    except Exception as e: R["steps"].append("animmode "+str(e))
    R["after_anim_class"]=str(mesh.get_editor_property("anim_class"))
    # compile + save
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp); R["steps"].append("compiled")
    except Exception as e: R["steps"].append("compile err "+str(e))
    unreal.EditorAssetLibrary.save_loaded_asset(bp, False); R["steps"].append("saved bp")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_set_anim_class.json","w") as f: json.dump(R,f,indent=1)
unreal.log("SET_ANIM_CLASS done")
