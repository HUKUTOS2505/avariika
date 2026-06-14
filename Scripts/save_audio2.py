import unreal
r=[]
for n in ["RadioComm","FlashClick"]:
    p="/Game/Audio/SFX/%s"%n
    r.append("%s=%s"%(n, unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)))
open("D:/unrealEngine/avariika/Saved/save_audio2.txt","w",encoding="utf-8").write("\n".join(r))
