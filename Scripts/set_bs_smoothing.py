import unreal
_OUT=[]
def line(s=""):
    unreal.log("BSSMOOTH| "+str(s)); _OUT.append(str(s))

SPEED = 6.0  # weight interp speed/sec; ~1/6s to switch sample. lower=smoother/laggier, higher=snappier
for p in ("/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility",
          "/Game/Avariika/Anim/Locomotion/BS_Crouch"):
    bs = unreal.load_asset(p)
    if not bs:
        line("%s: load fail" % p); continue
    try:
        before = bs.get_editor_property("target_weight_interpolation_speed_per_sec")
        bs.set_editor_property("target_weight_interpolation_speed_per_sec", SPEED)
        bs.set_editor_property("target_weight_interpolation_ease_in_out", True)
        after = bs.get_editor_property("target_weight_interpolation_speed_per_sec")
        line("%s: speed %s -> %s, ease_in_out=True" % (p.split('/')[-1], before, after))
    except Exception as e:
        line("%s: set err %s" % (p, e))
    try:
        line("  save -> %s" % unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False))
    except Exception as e:
        line("  save err %s" % e)

with open("C:/unrealEngine/avariika/Scripts/set_bs_smoothing_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
