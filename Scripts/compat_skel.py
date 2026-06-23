import unreal, json, traceback
R={"steps":[],"methods":[],"err":None}
try:
    hz=unreal.load_asset("/Game/hazmat/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton")
    hosp=unreal.load_asset("/Game/Hospital/Free_Content_Epic_Games/Mannequin/Character/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton")
    R["methods"]=[m for m in dir(hz) if 'compat' in m.lower()]
    # try API method first
    done=False
    for meth in ["add_compatible_skeleton"]:
        if hasattr(hz, meth):
            try:
                getattr(hz, meth)(hosp); getattr(hosp, meth)(hz); R["steps"].append(meth+" ok"); done=True
            except Exception as e: R["steps"].append(meth+" err "+str(e))
    if not done:
        # try editor property (array of compatible skeletons)
        for prop in ["compatible_skeletons"]:
            try:
                cur=list(hz.get_editor_property(prop)); cur.append(hosp); hz.set_editor_property(prop,cur)
                cur2=list(hosp.get_editor_property(prop)); cur2.append(hz); hosp.set_editor_property(prop,cur2)
                R["steps"].append("prop "+prop+" set"); done=True
            except Exception as e: R["steps"].append("prop "+prop+" err "+str(e))
    eal=unreal.EditorAssetLibrary
    eal.save_loaded_asset(hz, False); eal.save_loaded_asset(hosp, False)
    R["done"]=done
    try: R["hz_compat_now"]=str(hz.get_editor_property("compatible_skeletons"))
    except Exception: pass
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_compat.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("COMPAT_SKEL done")
