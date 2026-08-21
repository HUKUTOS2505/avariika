import unreal
_OUT=[]
def L(s): unreal.log("CUST| "+str(s)); _OUT.append(str(s))
ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world=ues.get_game_world()
pc=unreal.GameplayStatics.get_player_controller(world,0) if world else None
pawn=unreal.GameplayStatics.get_player_pawn(world,0) if world else None
L("pc: %s  pawn: %s" % (pc.get_class().get_name() if pc else None, pawn.get_name() if pawn else None))
if pc:
    try:
        pc.toggle_customize()
        L("toggle_customize() called; is_customize_open=%s" % pc.is_customize_open())
    except Exception as e:
        L("toggle err: %s" % e)
# after a draw frame the bum should auto-build
if pawn:
    comp=pawn.get_component_by_class(unreal.WorkerAppearanceComponent)
    if comp:
        ap=comp.get_appearance()
        L("appearance slots: %d" % len(ap.slots))
    parts=[c for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent) if (c.get_skinned_asset() and c.get_skinned_asset().get_name() not in ("SKM_Worker_Male_1","hazmat"))]
    L("modular parts built: %d" % len(parts))
L("PIE alive (no crash) — pawn still valid: %s" % (pawn is not None))
with open("C:/unrealEngine/avariika/Scripts/test_customize_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
