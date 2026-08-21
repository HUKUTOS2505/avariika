import unreal
_OUT=[]
def line(s=""):
    unreal.log("BSINT| "+str(s)); _OUT.append(str(s))

bs = unreal.load_asset("/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility")
line("bs: %s class=%s" % (bs.get_path_name() if bs else None, bs.get_class().get_name() if bs else None))

# dump interpolation/smoothing-related editor properties
cands = ["target_weight_interpolation_speed_per_sec",
         "target_weight_interpolation_ease_in_out",
         "per_bone_blend",
         "interpolation_param",
         "blend_parameters",
         "axis_to_scale_animation_input",
         "notify_trigger_mode"]
for c in cands:
    try:
        v = bs.get_editor_property(c)
        line("  %s = %s" % (c, v))
    except Exception as e:
        line("  %s : <no prop> (%s)" % (c, str(e)[:60]))

# blend_parameters detail (per-axis: name/min/max/grid + maybe interpolation)
try:
    bp = bs.get_editor_property("blend_parameters")
    line("blend_parameters: %s" % bp)
except Exception as e:
    line("blend_parameters err %s" % e)

with open("C:/unrealEngine/avariika/Scripts/probe_bs_interp_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
