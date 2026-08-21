import unreal
_OUT=[]
def L(s): unreal.log("AOCR| "+str(s)); _OUT.append(str(s))
AIM="/Game/Avariika/Anim/Locomotion/Mobility/Aim/"
DESTDIR="/Game/Avariika/Anim/Locomotion"
tools=unreal.AssetToolsHelpers.get_asset_tools()
ref=unreal.load_asset(AIM+"RT_MOB1_M1_Crouch_Look_Center")
skel=ref.get_editor_property("skeleton") if ref else None
name="AO_Crouch_Head"; prefix="RT_MOB1_M1_Crouch_"
if unreal.EditorAssetLibrary.does_asset_exist(DESTDIR+"/"+name):
    unreal.EditorAssetLibrary.delete_asset(DESTDIR+"/"+name)
fac=unreal.AimOffsetBlendSpaceFactoryNew()
try: fac.set_editor_property("target_skeleton", skel)
except Exception: pass
ao=tools.create_asset(name, DESTDIR, unreal.AimOffsetBlendSpace, fac)
bp=list(ao.get_editor_property("blend_parameters"))
for idx,nm in ((0,"Yaw"),(1,"Pitch")):
    bp[idx].set_editor_property("display_name", nm)
    bp[idx].set_editor_property("min", -90.0); bp[idx].set_editor_property("max", 90.0)
    bp[idx].set_editor_property("grid_num", 2)
ao.set_editor_property("blend_parameters", bp)
grid=[("Look_Center",0,0),("Look_L90",-90,0),("Look_R90",90,0),("Look_U90",0,90),("Look_D90",0,-90),
      ("Look_LU45",-90,90),("Look_RU45",90,90),("Look_LD45",-90,-90),("Look_RD45",90,-90)]
samples=[]
for suf,x,y in grid:
    c=unreal.load_asset(AIM+prefix+suf)
    if c:
        s=unreal.BlendSample(); s.set_editor_property("animation", c)
        s.set_editor_property("sample_value", unreal.Vector(float(x),float(y),0.0)); samples.append(s)
ao.set_editor_property("sample_data", samples)
unreal.EditorLoadingAndSavingUtils.save_packages([ao.get_outermost()], False)
L("AO_Crouch_Head: %d samples, additive base=%s" % (len(samples), ref.get_editor_property("additive_anim_type")))
with open("C:/unrealEngine/avariika/Scripts/build_ao_crouch_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
