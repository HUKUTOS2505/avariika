import unreal
_OUT=[]
def L(s): unreal.log("BS| "+str(s)); _OUT.append(str(s))

bs=unreal.load_asset("/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility")
L("bs=%s class=%s" % (bs.get_name() if bs else None, type(bs).__name__ if bs else None))
try:
    L("skeleton=%s" % bs.get_editor_property('target_skeleton').get_name())
except Exception as e:
    L("skel err %s" % e)

# Try to read samples
for prop in ('sample_data',):
    try:
        sd=bs.get_editor_property(prop)
        L("%s count=%d" % (prop, len(sd)))
        for i,s in enumerate(sd):
            try:
                anim=s.get_editor_property('animation')
                pos=s.get_editor_property('sample_value')
                L("  [%d] pos=(%.1f,%.1f) anim=%s" % (i, pos.x, pos.y, anim.get_name() if anim else None))
            except Exception as e2:
                L("  [%d] err %s" % (i,e2))
    except Exception as e:
        L("%s err %s" % (prop,e))

with open("C:/unrealEngine/avariika/Scripts/dump_bs_samples_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
