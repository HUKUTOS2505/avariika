import unreal, json, traceback
R={"by_cat":{}, "imported":0, "err":None}
try:
    with open(r"D:/unrealEngine/avariika/Scripts/manifests/_snd_import.json") as f:
        jobs=json.load(f)
    tools=unreal.AssetToolsHelpers.get_asset_tools()
    tasks=[]
    for j in jobs:
        src=j["src"].replace("\\","/")
        cat=j["cat"]
        t=unreal.AssetImportTask()
        t.set_editor_property("filename", src)
        t.set_editor_property("destination_path", "/Game/Audio/Lib/"+cat)
        t.set_editor_property("automated", True)
        t.set_editor_property("save", True)
        t.set_editor_property("replace_existing", True)
        tasks.append(t)
        R["by_cat"][cat]=R["by_cat"].get(cat,0)+1
    tools.import_asset_tasks(tasks)
    # count imported objects
    n=0
    for t in tasks:
        try: n+=len(t.get_editor_property("imported_object_paths"))
        except Exception: pass
    R["imported"]=n
    R["task_count"]=len(tasks)
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_snd_import_result.json","w") as f: json.dump(R,f,indent=1)
unreal.log("IMPORT_SOUNDS done %d" % R["imported"])
