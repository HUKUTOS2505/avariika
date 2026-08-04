import unreal
_OUT=[]
def line(s=""):
    unreal.log("PIEINS| "+str(s)); _OUT.append(str(s))

ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = ues.get_game_world()
line("game world: %s" % (world.get_name() if world else None))
if world:
    pawn = unreal.GameplayStatics.get_player_pawn(world, 0)
    line("player pawn: %s (%s)" % (pawn.get_name() if pawn else None, pawn.get_class().get_name() if pawn else None))
    if pawn:
        comps = pawn.get_components_by_class(unreal.SkeletalMeshComponent)
        line("skel comps: %d" % len(comps))
        for c in comps:
            line("--- comp %s ---" % c.get_name())
            try:
                sm = c.get_skinned_asset(); line("  mesh: %s" % (sm.get_name() if sm else None))
            except Exception as e: line("  mesh err %s"%e)
            try:
                ai = c.get_anim_instance(); line("  ANIM_INSTANCE: %s" % (ai.get_class().get_name() if ai else "NONE!!"))
            except Exception as e: line("  ai err %s"%e)
            try: line("  animation_mode: %s" % c.get_editor_property("animation_mode"))
            except Exception: pass
            try: line("  visible=%s owner_no_see=%s" % (c.get_editor_property("visible"), c.get_editor_property("owner_no_see")))
            except Exception: pass
            try: line("  using_leader_pose: %s" % c.is_using_leader_pose())
            except Exception as e: line("  lp err %s"%e)
            try:
                ll = c.get_socket_location("hand_l"); lr = c.get_socket_location("hand_r")
                d = (ll - lr).size()
                line("  hand_l=%s" % ll); line("  hand_r=%s" % lr)
                line("  HAND_SPREAD=%.1f cm  (>120 => T-pose/ref ; <70 => posed/idle)" % d)
            except Exception as e:
                line("  socket err %s"%e)

with open("C:/unrealEngine/avariika/Scripts/inspect_pie_body_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
