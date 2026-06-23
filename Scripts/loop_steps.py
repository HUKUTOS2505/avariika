import unreal
r=[]
for n in ["Walk_stone","Jog_stone"]:
    p="/Game/Survival_SFX/Movement/%s"%n
    sw=unreal.load_asset(p)
    if sw:
        sw.set_editor_property("looping", True)
        unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)
        r.append("%s looping=%s"%(n, sw.get_editor_property("looping")))
open("C:/unrealEngine/avariika/Saved/loop_steps.txt","w").write("\n".join(r))
