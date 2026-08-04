import unreal
_OUT=[]
def L(s): unreal.log("ABPC| "+str(s)); _OUT.append(str(s))

bp = unreal.load_asset("/Game/Avariika/Anim/Locomotion/ABP_Worker")
L("loaded: %s" % (bp.get_name() if bp else None))
try:
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    L("compiled OK")
except Exception as e:
    L("compile err: %s" % e)
try:
    pkg = bp.get_outermost()
    ok = unreal.EditorLoadingAndSavingUtils.save_packages([pkg], False)
    L("saved: %s" % ok)
except Exception as e:
    L("save err: %s" % e)

with open("C:/unrealEngine/avariika/Scripts/compile_abp_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
