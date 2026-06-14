import unreal
for p in ["/Game/Survival_SFX/Survival/First_aid_1","/Game/Audio/SFX/DrinkGlug"]:
    sw=unreal.load_asset(p)
    if sw:
        sw.set_editor_property("looping", True)
        unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)
open("D:/unrealEngine/avariika/Saved/loop_cast.txt","w").write("First_aid + DrinkGlug looping set")
