import unreal, math
_OUT=[]
def line(s=""):
    unreal.log("REMEAS| "+str(s)); _OUT.append(str(s))
def dist(a,b):
    return math.sqrt((a.x-b.x)**2+(a.y-b.y)**2+(a.z-b.z)**2)
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = ues.get_game_world()
pawn = unreal.GameplayStatics.get_player_pawn(world, 0) if world else None
if pawn:
    for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
        if c.get_name()=="CharacterMesh0":
            hl=c.get_socket_location("hand_l"); hr=c.get_socket_location("hand_r"); pv=c.get_socket_location("pelvis")
            line("AFTER direct-clip: HAND_SPREAD=%.1f cm ; hand_l height above pelvis=%.1f cm" % (dist(hl,hr), hl.z-pv.z))
            line("(if spread/height now differ from before => the body CAN animate; clips OK, blendspace/skeleton-path was the culprit)")
            break
with open("C:/unrealEngine/avariika/Scripts/remeasure_pie_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
