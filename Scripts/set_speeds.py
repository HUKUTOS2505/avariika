import unreal
_OUT=[]
def line(s=""):
    unreal.log("SPEED| "+str(s)); _OUT.append(str(s))

gc = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
cdo = unreal.get_default_object(gc)
for prop, val in (("base_walk_speed", 150.0), ("sprint_speed", 450.0)):
    try:
        before = cdo.get_editor_property(prop)
        cdo.set_editor_property(prop, val)
        line("%s: %s -> %s" % (prop, before, cdo.get_editor_property(prop)))
    except Exception as e:
        line("%s err: %s" % (prop, e))

# save BP
pkg = cdo.get_outermost()
try:
    ok = unreal.EditorLoadingAndSavingUtils.save_packages([pkg], False)
    line("save BP -> %s" % ok)
except Exception as e:
    line("save err %s" % e)

with open("C:/unrealEngine/avariika/Scripts/set_speeds_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
