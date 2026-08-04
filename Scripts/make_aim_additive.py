import unreal
_OUT=[]
def line(s=""):
    unreal.log("ADDITIVE| "+str(s)); _OUT.append(str(s))

AIM = "/Game/Avariika/Anim/Locomotion/Mobility/Aim/"
def setup(prefix, center_name):
    base = unreal.load_asset(AIM+center_name)
    if not base:
        line("NO base %s" % center_name); return
    poses = ["Look_Center","Look_L90","Look_R90","Look_U90","Look_D90",
             "Look_LU45","Look_RU45","Look_LD45","Look_RD45"]
    done=0
    for suf in poses:
        a = unreal.load_asset(AIM+prefix+suf)
        if not a:
            line("  miss %s"%(prefix+suf)); continue
        try:
            a.set_editor_property("additive_anim_type", unreal.AdditiveAnimationType.AAT_ROTATION_OFFSET_MESH_SPACE)
            a.set_editor_property("ref_pose_type", unreal.AdditiveBasePoseType.ABPT_ANIM_FRAME)
            a.set_editor_property("ref_pose_seq", base)
            a.set_editor_property("ref_frame_index", 0)
            done+=1
        except Exception as e:
            line("  err %s: %s"%(suf, str(e)[:100]))
    # save all
    pkgs=[unreal.load_asset(AIM+prefix+s).get_outermost() for s in poses if unreal.load_asset(AIM+prefix+s)]
    unreal.EditorLoadingAndSavingUtils.save_packages(list(set(pkgs)), False)
    line("%s: additive set on %d poses (base=%s)" % (prefix, done, center_name))

setup("RT_MOB1_M1_Stand_Relaxed_", "RT_MOB1_M1_Stand_Relaxed_Look_Center")
setup("RT_MOB1_M1_Crouch_",        "RT_MOB1_M1_Crouch_Look_Center")

with open("C:/unrealEngine/avariika/Scripts/make_aim_additive_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
