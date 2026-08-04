import unreal
_OUT=[]
def line(s=""):
    unreal.log("CRBS| "+str(s)); _OUT.append(str(s))

bs = unreal.load_asset("/Game/Avariika/Anim/Locomotion/BS_Crouch")
if bs:
    line("BS_Crouch class=%s" % bs.get_class().get_name())
    sk = bs.get_editor_property("skeleton")
    line("  skeleton: %s" % (sk.get_path_name() if sk else None))
    sd = bs.get_editor_property("sample_data")
    line("  samples: %d" % len(sd))
    for s in sd[:12]:
        a=s.get_editor_property("animation"); v=s.get_editor_property("sample_value")
        line("    %s @ (%.0f,%.0f)" % (a.get_name() if a else None, v.x, v.y))

# crouch clips available?
ar = unreal.AssetRegistryHelpers.get_asset_registry()
cw = [str(h.asset_name) for h in ar.get_assets_by_path("/Game/Avariika/Anim/Locomotion/Mobility", recursive=False) if "CrouchWalk" in str(h.asset_name) or "Crouch_Idle" in str(h.asset_name)]
line("crouch clips found: %d" % len(cw))
for n in sorted(cw)[:14]:
    line("  %s" % n)

# WorkAnim skeleton of a crouch clip
c = unreal.load_asset("/Game/Avariika/Anim/Locomotion/Mobility/RT_MOB1_M1_CrouchWalk_F_IP")
if c:
    line("CrouchWalk_F skeleton: %s" % c.get_editor_property("skeleton").get_path_name())

with open("C:/unrealEngine/avariika/Scripts/inspect_crouch_bs_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
