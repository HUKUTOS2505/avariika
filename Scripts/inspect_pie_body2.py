import unreal, math
_OUT=[]
def line(s=""):
    unreal.log("PIEINS2| "+str(s)); _OUT.append(str(s))
def dist(a,b):
    return math.sqrt((a.x-b.x)**2+(a.y-b.y)**2+(a.z-b.z)**2)

ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = ues.get_game_world()
pawn = unreal.GameplayStatics.get_player_pawn(world, 0) if world else None
line("pawn: %s" % (pawn.get_name() if pawn else None))
if pawn:
    body = None
    for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
        if c.get_name()=="CharacterMesh0":
            body=c; break
    if body:
        ai = body.get_anim_instance()
        line("body anim_instance: %s" % (ai.get_class().get_name() if ai else "NONE"))
        hl = body.get_socket_location("hand_l")
        hr = body.get_socket_location("hand_r")
        pelvis = body.get_socket_location("pelvis")
        head = body.get_socket_location("head")
        actor_loc = pawn.get_actor_location()
        line("hand_l=(%.1f,%.1f,%.1f)" % (hl.x,hl.y,hl.z))
        line("hand_r=(%.1f,%.1f,%.1f)" % (hr.x,hr.y,hr.z))
        line("pelvis=(%.1f,%.1f,%.1f)" % (pelvis.x,pelvis.y,pelvis.z))
        line("head=(%.1f,%.1f,%.1f)" % (head.x,head.y,head.z))
        line("HAND_SPREAD=%.1f cm   (>120 => arms horizontal = T/ref ; <70 => arms down = posed)" % dist(hl,hr))
        line("hand_l height above pelvis=%.1f cm  (near 0 => arms horizontal = T-pose)" % (hl.z-pelvis.z))

        # DIRECT-CLIP TEST: force a single known retargeted clip onto the body, bypass the AnimBP+blendspace.
        clip = unreal.load_asset("/Game/Avariika/Anim/Locomotion/Mobility/RT_MOB1_M1_Jog_F_IP")
        if clip:
            body.set_animation_mode(unreal.AnimationMode.ANIMATION_SINGLE_NODE)
            body.play_animation(clip, True)
            line("DIRECT-CLIP: set single-node + play RT_MOB1_M1_Jog_F_IP (check viewport: does body move now?)")
        else:
            line("DIRECT-CLIP: clip load fail")

with open("C:/unrealEngine/avariika/Scripts/inspect_pie_body2_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
