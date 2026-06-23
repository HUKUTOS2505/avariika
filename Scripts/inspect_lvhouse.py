import unreal, json, traceback
R={"err":None}
try:
    les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.load_level("/Game/PostApocalypticHouse/Maps/LV_House")
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    acts=eas.get_all_level_actors()
    R["level"]="LV_House"; R["actor_count"]=len(acts)
    from collections import Counter
    c=Counter(); minv=[1e9,1e9,1e9]; maxv=[-1e9,-1e9,-1e9]; sm=0
    for a in acts:
        cn=a.get_class().get_name(); c[cn]+=1
        if cn=="StaticMeshActor":
            sm+=1; L=a.get_actor_location()
            minv=[min(minv[0],L.x),min(minv[1],L.y),min(minv[2],L.z)]
            maxv=[max(maxv[0],L.x),max(maxv[1],L.y),max(maxv[2],L.z)]
    R["top"]=c.most_common(10); R["staticmesh"]=sm
    R["bounds_m"]={"min":[round(v/100,1) for v in minv],"max":[round(v/100,1) for v in maxv],
                   "size":[round((maxv[0]-minv[0])/100,1),round((maxv[1]-minv[1])/100,1),round((maxv[2]-minv[2])/100,1)]}
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_lvhouse.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("INSPECT_LVHOUSE done")
