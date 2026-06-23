import unreal, json, traceback
R={"steps":[],"err":None}
try:
    eal=unreal.EditorAssetLibrary
    hz=unreal.load_asset("/Game/hazmat/Mesh/hazmat.hazmat")
    abp=unreal.load_asset("/Game/Hospital/Free_Content_Epic_Games/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP")
    hosp_skel=unreal.load_asset("/Game/Hospital/Free_Content_Epic_Games/Mannequin/Character/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton")
    R["have"]={"hz":bool(hz),"abp":bool(abp),"hosp_skel":bool(hosp_skel)}
    R["hz_skel_before"]=str(hz.get_editor_property("skeleton"))
    # force hazmat mesh onto the Hospital UE4 skeleton (identical bones -> AnimBP compatible)
    ok=False
    try:
        if hasattr(hz,"set_skeleton"): hz.set_skeleton(hosp_skel); ok=True; R["steps"].append("set_skeleton()")
    except Exception as e: R["steps"].append("set_skeleton err "+str(e))
    if not ok:
        try: hz.set_editor_property("skeleton", hosp_skel); R["steps"].append("set prop skeleton")
        except Exception as e: R["steps"].append("set prop err "+str(e))
    eal.save_loaded_asset(hz, False)
    R["hz_skel_after"]=str(hz.get_editor_property("skeleton"))
    # assign to player
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    cdo=unreal.get_default_object(bp.generated_class()); mesh=cdo.get_editor_property("mesh")
    mesh.set_editor_property("skeletal_mesh", hz)
    mesh.set_editor_property("anim_class", abp.generated_class())
    try: mesh.set_editor_property("animation_mode", unreal.AnimationMode.ANIMATION_BLUEPRINT)
    except Exception: pass
    R["after"]={"mesh":str(mesh.get_editor_property("skeletal_mesh")),"anim":str(mesh.get_editor_property("anim_class"))}
    unreal.BlueprintEditorLibrary.compile_blueprint(bp); eal.save_loaded_asset(bp, False)
    R["steps"].append("player set + compiled + saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_hazmat_player.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("SET_HAZMAT_PLAYER done")
