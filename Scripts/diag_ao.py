import unreal
_OUT=[]
def L(s): unreal.log("AODIAG| "+str(s)); _OUT.append(str(s))
ao=unreal.load_asset("/Game/Avariika/Anim/Locomotion/AO_Stand_Look")
L("class: %s" % ao.get_class().get_name())
bp=ao.get_editor_property("blend_parameters")
L("blend_parameters count: %s" % len(bp))
for i,p in enumerate(bp):
    try:
        L("  axis %d: name=%s min=%s max=%s grid=%s" % (i,
            p.get_editor_property("display_name"),
            p.get_editor_property("min"), p.get_editor_property("max"),
            p.get_editor_property("grid_num")))
    except Exception as e:
        L("  axis %d read err %s" % (i, e))
sd=ao.get_editor_property("sample_data")
L("samples: %d" % len(sd))
for s in sd:
    a=s.get_editor_property("animation"); v=s.get_editor_property("sample_value")
    L("  %s @ (%.0f,%.0f)" % (a.get_name().split('_')[-1] if a else None, v.x, v.y))
# is it flagged as additive-compatible? check a sample anim additive
with open("C:/unrealEngine/avariika/Scripts/diag_ao_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
