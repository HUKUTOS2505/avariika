import unreal, math
_OUT=[]
def line(s=""):
    unreal.log("MEAS| "+str(s)); _OUT.append(str(s))
def dist(a,b):
    return math.sqrt((a.x-b.x)**2+(a.y-b.y)**2+(a.z-b.z)**2)
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = ues.get_game_world()
pawn = unreal.GameplayStatics.get_player_pawn(world, 0) if world else None
line("pawn: %s  world: %s" % (pawn.get_name() if pawn else None, world.get_name() if world else None))
if pawn:
    for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
        if c.get_name()=="CharacterMesh0":
            ai=c.get_anim_instance()
            line("anim_instance: %s" % (ai.get_class().get_name() if ai else "NONE"))
            hl=c.get_socket_location("hand_l"); hr=c.get_socket_location("hand_r"); pv=c.get_socket_location("pelvis")
            line("HAND_SPREAD=%.1f cm  (95=ref/T ; ~47=posed/animating)" % dist(hl,hr))
            line("hand_l height above pelvis=%.1f cm" % (hl.z-pv.z))
            break
with open("C:/unrealEngine/avariika/Scripts/measure_body_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
