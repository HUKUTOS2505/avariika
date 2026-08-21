import unreal
try:
    ok = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(save_map_packages=True, save_content_packages=True)
    unreal.log("SAVEALL| save_dirty_packages -> %s" % ok)
except Exception as e:
    unreal.log_error("SAVEALL| err %s" % e)
with open("C:/unrealEngine/avariika/Scripts/save_all_now_out.txt","w",encoding="utf-8") as f:
    f.write("done")
