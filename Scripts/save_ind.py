import unreal
for n,loop in [("EngineStart",False),("ElectricZap",False),("DrinkGlug",False),("WeldBuzz",True)]:
    p="/Game/Audio/SFX/%s"%n
    sw=unreal.load_asset(p)
    if sw:
        if loop: sw.set_editor_property("looping", True)
        unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)
open("C:/unrealEngine/avariika/Saved/save_ind.txt","w").write("saved")
