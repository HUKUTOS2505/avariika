import unreal
_OUT=[]
def L(s): unreal.log("WRK2| "+str(s)); _OUT.append(str(s))
ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world=ues.get_game_world()
pawn=unreal.GameplayStatics.get_player_pawn(world,0) if world else None
L("pawn: %s" % (pawn.get_name() if pawn else None))
if pawn:
    comp=None
    try:
        comp=pawn.get_component_by_class(unreal.WorkerAppearanceComponent)
    except Exception as e:
        L("get comp err: %s" % e)
    L("WorkerAppearance comp: %s" % (comp.get_name() if comp else None))
    if comp:
        try:
            comp.apply_default_preset()
            comp.apply_equipment_flags(True, True, True)
            L("called apply_default_preset + equipment flags")
        except Exception as e:
            L("call err: %s" % e)
    comps=pawn.get_components_by_class(unreal.SkeletalMeshComponent)
    L("skeletal mesh components now: %d" % len(comps))
    for c in comps:
        sm=None
        try: sm=c.get_skinned_asset()
        except Exception: pass
        L("  %s -> %s" % (c.get_name(), sm.get_name() if sm else None))
with open("C:/unrealEngine/avariika/Scripts/test_worker_build2_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
