import unreal
_OUT=[]
def line(s=""):
    unreal.log("BSS3| "+str(s)); _OUT.append(str(s))

SPEED = 6.0
paths = ["/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility",
         "/Game/Avariika/Anim/Locomotion/BS_Crouch"]
pkgs = []
for p in paths:
    bs = unreal.load_asset(p)
    if not bs:
        line("%s load fail"%p); continue
    bs.set_editor_property("target_weight_interpolation_speed_per_sec", SPEED)
    bs.set_editor_property("target_weight_interpolation_ease_in_out", True)
    pkgs.append(bs.get_outermost())
    line("%s speed=%s" % (p.split('/')[-1], bs.get_editor_property("target_weight_interpolation_speed_per_sec")))

try:
    ok = unreal.EditorLoadingAndSavingUtils.save_packages(pkgs, False)  # only_dirty=False
    line("save_packages(only_dirty=False) -> %s" % ok)
except Exception as e:
    line("save_packages err %s" % e)
    # fallback: save by filename
    try:
        ok2 = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(False, True)
        line("save_dirty_packages -> %s" % ok2)
    except Exception as e2:
        line("save_dirty err %s" % e2)

with open("C:/unrealEngine/avariika/Scripts/set_bs_smoothing3_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
