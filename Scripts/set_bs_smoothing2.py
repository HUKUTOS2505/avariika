import unreal
_OUT=[]
def line(s=""):
    unreal.log("BSSMOOTH2| "+str(s)); _OUT.append(str(s))

SPEED = 6.0
for p in ("/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility",
          "/Game/Avariika/Anim/Locomotion/BS_Crouch"):
    bs = unreal.load_asset(p)
    if not bs:
        line("%s: load fail" % p); continue
    bs.set_editor_property("target_weight_interpolation_speed_per_sec", SPEED)
    bs.set_editor_property("target_weight_interpolation_ease_in_out", True)
    pkg = bs.get_outermost()
    try:
        pkg.set_dirty_flag(True)
    except Exception:
        try: pkg.mark_package_dirty()
        except Exception as e: line("dirty err %s"%e)
    line("%s: speed now=%s dirty=%s" % (p.split('/')[-1],
        bs.get_editor_property("target_weight_interpolation_speed_per_sec"),
        pkg.is_dirty() if hasattr(pkg,'is_dirty') else '?'))
    ok1 = False
    try:
        ok1 = unreal.EditorAssetLibrary.save_loaded_asset(bs)
    except Exception as e:
        line("  save_loaded_asset err %s" % e)
    line("  save_loaded_asset -> %s" % ok1)

with open("C:/unrealEngine/avariika/Scripts/set_bs_smoothing2_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
