import unreal
_OUT=[]
def line(s=""):
    unreal.log("BSS| "+str(s)); _OUT.append(str(s))

for BSP in ("/Game/Avariika/Anim/Locomotion/BS_Locomotion",
            "/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility"):
    bs = unreal.load_asset(BSP)
    line("=== %s ===" % BSP)
    if not bs:
        line("  load fail"); continue
    sd = bs.get_editor_property("sample_data")
    line("  samples: %d" % len(sd))
    nulls=0
    for i,s in enumerate(sd):
        anim = s.get_editor_property("animation")
        pos = s.get_editor_property("sample_value")
        if anim is None:
            nulls+=1
            line("  [%d] anim=NULL pos=%s" % (i, pos)); continue
        try:
            nf = unreal.AnimationLibrary.get_num_frames(anim)
            tn = len(unreal.AnimationLibrary.get_animation_track_names(anim))
            sk = anim.get_editor_property("skeleton")
            line("  [%d] %s frames=%s tracks=%s skel=%s pos=(%.0f,%.0f)" % (
                i, anim.get_name(), nf, tn, sk.get_name() if sk else None,
                pos.x, pos.y))
        except Exception as e:
            line("  [%d] %s ERR %s" % (i, anim.get_name(), e))
    line("  NULL samples: %d" % nulls)

with open("C:/unrealEngine/avariika/Scripts/diag_bs_samples_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
