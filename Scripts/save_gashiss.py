import unreal
p="/Game/Audio/SFX/GasHiss"
sw=unreal.load_asset(p)
if sw:
    sw.set_editor_property("looping", True)
    unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)
open("C:/unrealEngine/avariika/Saved/gashiss.txt","w").write("GasHiss looping=%s"%(sw.get_editor_property("looping") if sw else "NOFIND"))
