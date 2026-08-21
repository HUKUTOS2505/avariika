import unreal
_OUT=[]
def line(s=""):
    unreal.log("AIMOFF| "+str(s)); _OUT.append(str(s))

AIM = "/Game/Avariika/Anim/Locomotion/Mobility/Aim/"
DESTDIR = "/Game/Avariika/Anim/Locomotion"
tools = unreal.AssetToolsHelpers.get_asset_tools()

# get WorkAnim skeleton from a retargeted look pose
ref = unreal.load_asset(AIM+"RT_MOB1_M1_Stand_Relaxed_Look_Center")
skel = ref.get_editor_property("skeleton") if ref else None
line("skeleton: %s" % (skel.get_path_name() if skel else None))

def make_factory():
    for fn in ("AimOffsetBlendSpaceFactory", "AimOffsetBlendSpaceFactoryNew", "AimOffsetBlendSpaceFactory1D"):
        cls = getattr(unreal, fn, None)
        if cls:
            try:
                fac = cls()
                try: fac.set_editor_property("target_skeleton", skel)
                except Exception: pass
                return fac, fn
            except Exception as e:
                line("  factory %s err %s" % (fn, e))
    return None, None

def build(name, prefix):
    fac, fn = make_factory()
    if not fac:
        line("%s: NO aim-offset factory available" % name); return False
    line("%s: factory=%s" % (name, fn))
    if unreal.EditorAssetLibrary.does_asset_exist(DESTDIR+"/"+name):
        unreal.EditorAssetLibrary.delete_asset(DESTDIR+"/"+name)
    try:
        ao = tools.create_asset(name, DESTDIR, unreal.AimOffsetBlendSpace, fac)
    except Exception as e:
        line("%s: create_asset err %s" % (name, str(e)[:160])); return False
    if not ao:
        line("%s: create returned None" % name); return False
    try:
        if ao.get_editor_property("skeleton") is None and skel:
            ao.set_editor_property("skeleton", skel)
    except Exception: pass
    # 9 poses on Yaw(X -90..90) x Pitch(Y -90..90) grid
    grid = [("Look_Center",0,0),("Look_L90",-90,0),("Look_R90",90,0),
            ("Look_U90",0,90),("Look_D90",0,-90),
            ("Look_LU45",-90,90),("Look_RU45",90,90),("Look_LD45",-90,-90),("Look_RD45",90,-90)]
    samples=[]
    for suf,x,y in grid:
        c = unreal.load_asset(AIM+prefix+suf)
        if not c:
            line("  MISS %s" % (prefix+suf)); continue
        s = unreal.BlendSample()
        s.set_editor_property("animation", c)
        s.set_editor_property("sample_value", unreal.Vector(float(x),float(y),0.0))
        samples.append(s)
    try:
        ao.set_editor_property("sample_data", samples)
        ao.set_editor_property("sample_data", ao.get_editor_property("sample_data"))  # triangulate
    except Exception as e:
        line("  sample set err %s" % e)
    unreal.EditorLoadingAndSavingUtils.save_packages([ao.get_outermost()], False)
    line("%s: built with %d samples" % (name, len(samples)))
    return True

build("AO_Stand_Look", "RT_MOB1_M1_Stand_Relaxed_")
build("AO_Crouch_Look", "RT_MOB1_M1_Crouch_")

with open("C:/unrealEngine/avariika/Scripts/build_aimoffset_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
