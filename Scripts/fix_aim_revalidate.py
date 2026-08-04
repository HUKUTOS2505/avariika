import unreal
_OUT=[]
def L(s): unreal.log("AIMFIX| "+str(s)); _OUT.append(str(s))

AIM="/Game/Avariika/Anim/Locomotion/Mobility/Aim/"
# verify additive flags on a couple poses
for n in ("RT_MOB1_M1_Stand_Relaxed_Look_R90","RT_MOB1_M1_Stand_Relaxed_Look_L90","RT_MOB1_M1_Stand_Relaxed_Look_Center"):
    a=unreal.load_asset(AIM+n)
    if a:
        try:
            L("%s: additive=%s base=%s seq=%s" % (n.split('_')[-1],
                a.get_editor_property("additive_anim_type"),
                a.get_editor_property("ref_pose_type"),
                (a.get_editor_property("ref_pose_seq").get_name() if a.get_editor_property("ref_pose_seq") else None)))
        except Exception as e: L("%s read err %s"%(n,e))

# re-validate AO_Stand_Look (re-set sample_data now that poses are additive)
ao=unreal.load_asset("/Game/Avariika/Anim/Locomotion/AO_Stand_Look")
if ao:
    sd=ao.get_editor_property("sample_data")
    ao.set_editor_property("sample_data", sd)
    L("AO_Stand_Look samples=%d resaved" % len(sd))
    unreal.EditorLoadingAndSavingUtils.save_packages([ao.get_outermost()], False)

with open("C:/unrealEngine/avariika/Scripts/fix_aim_revalidate_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
