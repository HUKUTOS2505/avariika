import unreal
_OUT=[]
def L(s): unreal.log("SPD| "+str(s)); _OUT.append(str(s))

for path in ("/Game/Avariika/Blueprints/BP_AvaryoCharacter",):
    bp=unreal.load_asset(path)
    if not bp:
        L("%s -> not found" % path); continue
    gc=bp.generated_class() if hasattr(bp,'generated_class') else None
    cdo=unreal.get_default_object(gc) if gc else None
    L("bp=%s gc=%s cdo=%s" % (bp.get_name(), gc.get_name() if gc else None, cdo.get_name() if cdo else None))
    if cdo:
        for prop in ("BaseWalkSpeed","SprintSpeed","CrawlSpeed","HeavyCarryMultiplier"):
            try:
                L("  %s = %s" % (prop, cdo.get_editor_property(prop)))
            except Exception as e:
                L("  %s ERR %s" % (prop, e))
with open("C:/unrealEngine/avariika/Scripts/check_speeds_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
