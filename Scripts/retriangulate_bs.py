import unreal
_OUT=[]
def line(s=""):
    unreal.log("RETRI| "+str(s)); _OUT.append(str(s))

for p in ("/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility",):
    bs = unreal.load_asset(p)
    sd = bs.get_editor_property("sample_data")
    line("%s samples=%d" % (p.split('/')[-1], len(sd)))
    # re-set sample_data to fire PostEditChangeProperty -> ValidateSampleData/resample (re-triangulate)
    bs.set_editor_property("sample_data", sd)
    # also re-set blend_parameters to be safe
    try:
        bs.set_editor_property("blend_parameters", bs.get_editor_property("blend_parameters"))
    except Exception as e:
        line("  bp reset err %s" % e)
    pkg = bs.get_outermost()
    ok = unreal.EditorLoadingAndSavingUtils.save_packages([pkg], False)
    line("  re-triangulated + save -> %s" % ok)

with open("C:/unrealEngine/avariika/Scripts/retriangulate_bs_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
