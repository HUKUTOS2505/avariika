# Безопасный пробинг BlendSpace API (всё в try, всегда пишет результат).
import unreal
lines = []
def safe(label, fn):
    try: lines.append("%s = %s" % (label, fn()))
    except Exception as e: lines.append("%s ERR: %s" % (label, str(e)[:120]))

safe("BlendSpaceFactory exists", lambda: hasattr(unreal, "BlendSpaceFactory"))
safe("BlendSpaceFactory1D exists", lambda: hasattr(unreal, "BlendSpaceFactory1D"))
safe("AnimationBlendSpaceSampleLibrary exists", lambda: hasattr(unreal, "AnimationBlendSpaceSampleLibrary"))
safe("AnimationBlendSpaceLibrary exists", lambda: hasattr(unreal, "AnimationBlendSpaceLibrary"))
# методы хелперов
for cand in ("AnimationBlendSpaceSampleLibrary","AnimationBlendSpaceLibrary"):
    if hasattr(unreal, cand):
        lines.append("%s methods: %s" % (cand, [m for m in dir(getattr(unreal, cand)) if not m.startswith("_")]))
# методы самого BlendSpace
lines.append("BlendSpace sample-ish methods: " + str([m for m in dir(unreal.BlendSpace) if ('ample' in m.lower() or 'axis' in m.lower() or 'add' in m.lower())]))
# попробовать создать пустой BS через каждую фабрику
tools = unreal.AssetToolsHelpers.get_asset_tools()
clip = unreal.load_asset("/Game/Avariika/Anim/Locomotion/Mobility/RT_MOB1_M1_Walk_F_IP.RT_MOB1_M1_Walk_F_IP")
skel = clip.get_editor_property("skeleton") if clip else None
lines.append("test clip+skel: %s / %s" % (bool(clip), skel.get_name() if skel else "-"))
for facname in ("BlendSpaceFactory1D","BlendSpaceFactory"):
    if not hasattr(unreal, facname): continue
    try:
        fac = getattr(unreal, facname)()
        try: fac.set_editor_property("target_skeleton", skel)
        except Exception as e: lines.append("  %s set skel err: %s" % (facname, str(e)[:80]))
        cls = unreal.BlendSpace1D if facname.endswith("1D") else unreal.BlendSpace
        bs = tools.create_asset("BS_PROBE_" + facname, "/Game/Avariika/Anim/Locomotion", cls, fac)
        lines.append("  %s -> create %s" % (facname, bool(bs)))
        if bs:
            unreal.EditorAssetLibrary.delete_asset("/Game/Avariika/Anim/Locomotion/BS_PROBE_" + facname)
    except Exception as e:
        lines.append("  %s create ERR: %s" % (facname, str(e)[:120]))
with open(r"C:/unrealEngine/avariika/Scripts/probe_bs.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("PROBE done")
