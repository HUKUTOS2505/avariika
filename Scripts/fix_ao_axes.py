import unreal
_OUT=[]
def L(s): unreal.log("AOAXES| "+str(s)); _OUT.append(str(s))

def fix(path, xname, yname):
    ao=unreal.load_asset(path)
    if not ao:
        L("%s not found"%path); return
    bp=list(ao.get_editor_property("blend_parameters"))
    # axis 0 = X (Yaw), axis 1 = Y (Pitch)
    for idx,nm in ((0,xname),(1,yname)):
        bp[idx].set_editor_property("display_name", nm)
        bp[idx].set_editor_property("min", -90.0)
        bp[idx].set_editor_property("max", 90.0)
        bp[idx].set_editor_property("grid_num", 2)
    ao.set_editor_property("blend_parameters", bp)
    # revalidate samples against new axes
    ao.set_editor_property("sample_data", ao.get_editor_property("sample_data"))
    unreal.EditorLoadingAndSavingUtils.save_packages([ao.get_outermost()], False)
    # readback
    b2=ao.get_editor_property("blend_parameters")
    L("%s: X[min=%s max=%s] Y[min=%s max=%s]" % (path.split('/')[-1],
        b2[0].get_editor_property("min"), b2[0].get_editor_property("max"),
        b2[1].get_editor_property("min"), b2[1].get_editor_property("max")))

fix("/Game/Avariika/Anim/Locomotion/AO_Stand_Look", "Yaw", "Pitch")
fix("/Game/Avariika/Anim/Locomotion/AO_Crouch_Look", "Yaw", "Pitch")

with open("C:/unrealEngine/avariika/Scripts/fix_ao_axes_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
