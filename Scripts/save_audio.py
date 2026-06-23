import unreal
saved=[]
for n in ["Explosion","RepairDone","RadioBlip","Ambient_Boiler","Heartbeat"]:
    p="/Game/Audio/SFX/%s"%n
    ok=unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)
    saved.append("%s=%s"%(n,ok))
open("C:/unrealEngine/avariika/Saved/save_audio.txt","w",encoding="utf-8").write("\n".join(saved))
