import unreal
_OUT=[]
def line(s=""):
    unreal.log("ROTYAW| "+str(s)); _OUT.append(str(s))

gc = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
cdo = unreal.get_default_object(gc)
for p in ("use_controller_rotation_yaw","use_controller_rotation_pitch","use_controller_rotation_roll"):
    try: line("%s = %s" % (p, cdo.get_editor_property(p)))
    except Exception as e: line("%s err %s" % (p, e))
# CharacterMovement orient to movement?
try:
    mv = cdo.get_editor_property("character_movement")
    line("bOrientRotationToMovement = %s" % mv.get_editor_property("orient_rotation_to_movement"))
except Exception as e:
    line("cmc err %s" % e)

# turn OFF body auto-yaw so head aim-offset becomes visible
cdo.set_editor_property("use_controller_rotation_yaw", False)
line("-> set use_controller_rotation_yaw = %s" % cdo.get_editor_property("use_controller_rotation_yaw"))
unreal.EditorLoadingAndSavingUtils.save_packages([cdo.get_outermost()], False)
line("saved BP")

with open("C:/unrealEngine/avariika/Scripts/set_rotyaw_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
