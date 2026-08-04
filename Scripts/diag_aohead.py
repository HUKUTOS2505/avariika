import unreal
_OUT=[]
def L(s): unreal.log("AOH| "+str(s)); _OUT.append(str(s))

AO="/Game/Avariika/Anim/Locomotion/AO_Head_Aim"
ao=unreal.load_asset(AO)
L("ao=%s class=%s" % (ao.get_name() if ao else None, type(ao).__name__ if ao else None))

# axis config
try:
    pp=ao.get_blend_parameter(0)
    L("X param: name=%s min=%s max=%s grid=%s" % (pp.display_name, pp.min, pp.max, pp.grid_num))
except Exception as e:
    L("Xparam err %s" % e)
try:
    pp=ao.get_blend_parameter(1)
    L("Y param: name=%s min=%s max=%s grid=%s" % (pp.display_name, pp.min, pp.max, pp.grid_num))
except Exception as e:
    L("Yparam err %s" % e)

# samples
try:
    sd=ao.get_editor_property('sample_data')
    L("samples=%d" % len(sd))
    for i,s in enumerate(sd):
        try:
            anim=s.get_editor_property('animation')
            pos=s.get_editor_property('sample_value')
            an=anim.get_name() if anim else None
            # additive type of the anim
            at=None; bp=None
            if anim:
                try: at=anim.get_editor_property('additive_anim_type')
                except Exception: at='?'
                try:
                    bp=anim.get_editor_property('ref_pose_type')
                except Exception: bp='?'
            L("  [%d] pos=(%.0f,%.0f) anim=%s add=%s refpose=%s" % (i,pos.x,pos.y,an,at,bp))
        except Exception as e2:
            L("  [%d] err %s" % (i,e2))
except Exception as e:
    L("samples err %s" % e)

# preview mesh
try:
    pm=ao.get_editor_property('preview_skeletal_mesh')
    L("preview_mesh=%s" % (pm.get_name() if pm else None))
except Exception as e:
    L("pm err %s" % e)

with open("C:/unrealEngine/avariika/Scripts/diag_aohead_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
