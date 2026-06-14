import unreal
p="/Game/Survival_SFX/Craft/Crafting_wood_item_1"
sw=unreal.load_asset(p)
ok="no"
if sw:
    sw.set_editor_property("looping", True)
    unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)
    ok=str(sw.get_editor_property("looping"))
open("D:/unrealEngine/avariika/Saved/loop_fill.txt","w").write("Crafting_wood_item_1 looping=%s"%ok)
