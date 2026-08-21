import unreal
abp = unreal.load_asset("/Game/Avariika/Anim/Locomotion/ABP_Worker")
ok=False
try:
    unreal.BlueprintEditorLibrary.compile_blueprint(abp); ok=True
except Exception as e:
    unreal.log_error("compile err %s"%e)
try:
    unreal.EditorAssetLibrary.save_loaded_asset(abp, False)
except Exception as e:
    unreal.log_error("save err %s"%e)
with open("C:/unrealEngine/avariika/Scripts/compile_abp_out.txt","w",encoding="utf-8") as f:
    f.write("compiled=%s saved" % ok)
