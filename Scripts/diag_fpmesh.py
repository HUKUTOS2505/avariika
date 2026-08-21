import unreal
_OUT=[]
def line(s=""):
    unreal.log("FPM| "+str(s)); _OUT.append(str(s))

bp = unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
sds = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
handles = sds.k2_gather_subobject_data_for_blueprint(bp)
line("subobject handles: %d" % len(handles))

def prop(obj, name):
    try:
        if obj.has_editor_property(name):
            return obj.get_editor_property(name)
    except Exception:
        pass
    try:
        return obj.get_editor_property(name)
    except Exception as e:
        return "<err:%s>" % e

for h in handles:
    data = sds.k2_find_subobject_data_from_handle(h)
    obj = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
    if not obj:
        continue
    cls = obj.get_class().get_name()
    if "SkeletalMeshComponent" not in cls:
        continue
    nm = obj.get_name()
    line("=== %s (%s) ===" % (nm, cls))
    sm = prop(obj, "skeletal_mesh_asset")
    line("  mesh: %s" % (sm.get_path_name() if hasattr(sm,'get_path_name') else sm))
    if hasattr(sm,'get_editor_property'):
        try:
            sk = sm.get_editor_property("skeleton"); line("  skeleton: %s" % (sk.get_path_name() if sk else None))
        except Exception as e: line("  skel err %s"%e)
    ac = prop(obj, "anim_class")
    line("  anim_class: %s" % (ac.get_path_name() if hasattr(ac,'get_path_name') else ac))
    line("  animation_mode: %s" % prop(obj, "animation_mode"))
    line("  visible: %s" % prop(obj, "visible"))
    line("  only_owner_see: %s" % prop(obj, "only_owner_see"))
    line("  owner_no_see: %s" % prop(obj, "owner_no_see"))
    line("  leader_pose_component: %s" % prop(obj, "leader_pose_component"))

with open("C:/unrealEngine/avariika/Scripts/diag_fpmesh_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
