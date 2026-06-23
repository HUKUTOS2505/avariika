import unreal, json, traceback
R={"steps":[],"err":None}
try:
    eal=unreal.EditorAssetLibrary
    tools=unreal.AssetToolsHelpers.get_asset_tools()
    F="/Game/Characters/Operator/Locomotion"
    skel=unreal.load_asset("/Game/Characters/Operator/SK_Operator_Skeleton.SK_Operator_Skeleton")
    # remove test bs
    if eal.does_asset_exist(F+"/BS_TEST_Op"): eal.delete_asset(F+"/BS_TEST_Op"); R["steps"].append("removed test bs")
    # BlendSpace1D shell
    if not eal.does_asset_exist(F+"/BS_Op_Locomotion"):
        f1=unreal.BlendSpaceFactory1D(); 
        try: f1.set_editor_property("target_skeleton",skel)
        except Exception as e: R["steps"].append("bs skel "+str(e))
        bs=tools.create_asset("BS_Op_Locomotion",F,unreal.BlendSpace1D,f1)
        # set horizontal axis = Speed 0..500
        try:
            bp=unreal.BlendParameter(); bp.set_editor_property("display_name","Speed"); bp.set_editor_property("min",0.0); bp.set_editor_property("max",500.0); bp.set_editor_property("grid_num",4)
            bs.set_editor_property("blend_parameters",bp)
            R["steps"].append("bs axis Speed 0-500 set")
        except Exception as e: R["steps"].append("bs axis err "+str(e))
        eal.save_loaded_asset(bs,False); R["steps"].append("BS_Op_Locomotion created")
    else: R["steps"].append("BS exists")
    # AnimBP shell
    if not eal.does_asset_exist(F+"/ABP_Operator"):
        fa=unreal.AnimBlueprintFactory()
        try: fa.set_editor_property("target_skeleton",skel)
        except Exception as e: R["steps"].append("abp skel "+str(e))
        abp=tools.create_asset("ABP_Operator",F,unreal.AnimBlueprint,fa)
        eal.save_loaded_asset(abp,False); R["steps"].append("ABP_Operator created "+str(bool(abp)))
    else: R["steps"].append("ABP exists")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_shells.json","w") as f: json.dump(R,f,indent=1)
unreal.log("BUILD_SHELLS done")
