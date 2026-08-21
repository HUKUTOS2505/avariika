import unreal
_OUT=[]
def line(s=""):
    unreal.log("LOCOST| "+str(s)); _OUT.append(str(s))

# which ABP is on the body?
gc = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
cdo = unreal.get_default_object(gc)
for c in cdo.get_components_by_class(unreal.SkeletalMeshComponent):
    if c.get_name()=="CharacterMesh0":
        ac = c.get_editor_property("anim_class")
        line("BODY anim_class: %s" % (ac.get_path_name() if ac else None))

# find BS_Op_Locomotion
ar = unreal.AssetRegistryHelpers.get_asset_registry()
for hit in ar.get_assets_by_path("/Game/Avariika/Anim/Locomotion", recursive=True):
    n = str(hit.asset_name)
    if "Op_Locomotion" in n or n.startswith("BS_Op") or "BS_Loco" in n:
        line("blendspace asset: %s (%s)" % (hit.get_full_name(), hit.asset_class_path.asset_name if hasattr(hit,'asset_class_path') else '?'))

def bsinfo(path):
    bs = unreal.load_asset(path)
    if not bs:
        line("  %s: NOT FOUND" % path); return
    line("  %s" % path)
    line("    class: %s" % bs.get_class().get_name())
    try:
        sk = bs.get_editor_property("skeleton"); line("    skeleton: %s" % (sk.get_name() if sk else None))
    except Exception as e: line("    skel err %s"%e)
    try:
        bp = bs.get_editor_property("blend_parameters"); line("    blend_parameters(axes): %s" % len(bp) if bp is not None else None)
    except Exception: pass
    try:
        sd = bs.get_editor_property("sample_data"); line("    samples: %s" % len(sd))
        names=[]
        for s in sd[:4]:
            a=s.get_editor_property("animation"); names.append(a.get_name() if a else None)
        line("    first samples: %s" % names)
    except Exception as e: line("    sample err %s"%e)

for p in ("/Game/Avariika/Anim/Locomotion/BS_Op_Locomotion",
          "/Game/Avariika/Anim/Locomotion/Mobility/BS_Op_Locomotion",
          "/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility"):
    bsinfo(p)

# Op_MOB1 clips: where + skeleton
line("--- Op_MOB1 clips ---")
found=0
for hit in ar.get_assets_by_path("/Game/Avariika/Anim", recursive=True):
    n=str(hit.asset_name)
    if n.startswith("Op_MOB1"):
        found+=1
        if found<=2:
            a=unreal.load_asset(hit.get_full_name().split(' ')[-1]) if False else unreal.load_asset(str(hit.package_name))
        if found<=3:
            line("  %s @ %s" % (n, str(hit.package_path)))
line("Op_MOB1 count: %d" % found)

with open("C:/unrealEngine/avariika/Scripts/inspect_loco_state_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
