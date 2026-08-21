import unreal
_OUT=[]
def line(s=""):
    unreal.log("COMP| "+str(s)); _OUT.append(str(s))

gc = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
cdo = unreal.get_default_object(gc)
line("CDO: %s" % (cdo.get_name() if cdo else None))

# Enumerate all components on the CDO
def dump_skelcomp(name, comp):
    if comp is None:
        line("%s : <none>" % name); return
    cls = comp.get_class().get_name()
    line("%s : comp class=%s" % (name, cls))
    sm = None
    for getter in ("get_skeletal_mesh_asset",):
        if hasattr(comp, getter):
            try:
                sm = getattr(comp, getter)()
            except Exception:
                pass
    if sm is None:
        try: sm = comp.get_editor_property("skeletal_mesh_asset")
        except Exception: pass
    line("   mesh: %s" % (sm.get_path_name() if sm else None))
    if sm:
        try:
            sk = sm.get_editor_property("skeleton")
            line("   mesh.skeleton: %s" % (sk.get_path_name() if sk else None))
        except Exception as e: line("   skel err %s" % e)
    try:
        ac = comp.get_editor_property("anim_class")
        line("   anim_class: %s" % (ac.get_path_name() if ac else None))
    except Exception as e: line("   ac err %s" % e)
    try:
        lp = comp.get_editor_property("leader_pose_component")
        line("   leader_pose: %s" % (lp if lp else None))
    except Exception:
        pass
    try:
        vis = comp.get_editor_property("visible")
        own = comp.get_editor_property("only_owner_see") if comp.has_editor_property("only_owner_see") else "?"
        oos = comp.get_editor_property("owner_no_see") if comp.has_editor_property("owner_no_see") else "?"
        line("   visible=%s only_owner_see=%s owner_no_see=%s" % (vis, own, oos))
    except Exception:
        pass

# Known named components
for nm in ("Mesh", "CharacterMesh0", "FirstPersonMesh", "FP_Mesh", "Mesh1P", "FirstPersonArms"):
    try:
        c = cdo.get_editor_property(nm)
        dump_skelcomp(nm, c)
    except Exception:
        pass

# Brute: iterate all object properties that are SkeletalMeshComponents
line("--- all skeletal mesh components found via components list ---")
try:
    comps = cdo.get_components_by_class(unreal.SkeletalMeshComponent)
    line("count via get_components_by_class: %s" % (len(comps) if comps is not None else None))
    for c in (comps or []):
        dump_skelcomp(c.get_name(), c)
except Exception as e:
    line("get_components_by_class err: %s" % e)

with open("C:/unrealEngine/avariika/Scripts/diag_components_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
