import unreal
_OUT=[]
def L(s):
    unreal.log("AIMPIE| "+str(s)); _OUT.append(str(s))
ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = ues.get_game_world()
pawn = unreal.GameplayStatics.get_player_pawn(world, 0) if world else None
pc = unreal.GameplayStatics.get_player_controller(world, 0) if world else None
if pawn and pc:
    L("control yaw = %.1f" % pc.get_control_rotation().yaw)
    L("actor yaw   = %.1f" % pawn.get_actor_rotation().yaw)
    try: L("use_controller_rotation_yaw = %s" % pawn.get_editor_property("use_controller_rotation_yaw"))
    except Exception as e: L("ucry err %s"%e)
    body=None
    for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
        if c.get_name()=="CharacterMesh0": body=c
    if body:
        ai = body.get_anim_instance()
        L("anim_instance = %s" % (ai.get_class().get_name() if ai else None))
        for v in ("AimYaw","AimPitch","Speed","Direction"):
            try: L("  %s = %s" % (v, ai.get_editor_property(v)))
            except Exception as e: L("  %s err %s" % (v, str(e)[:60]))
        try:
            hr = body.get_socket_rotation("head"); pr = body.get_socket_rotation("pelvis")
            L("head yaw=%.1f pelvis yaw=%.1f  HEAD-PELVIS diff=%.1f (ждём ~90 если работает)" % (hr.yaw, pr.yaw, hr.yaw-pr.yaw))
        except Exception as e:
            L("socket err %s"%e)
with open("C:/unrealEngine/avariika/Scripts/read_aim_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
