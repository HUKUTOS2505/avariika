import unreal
_OUT=[]
def L(s): unreal.log("WEAR| "+str(s)); _OUT.append(str(s))
ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world=ues.get_game_world()
pawn=unreal.GameplayStatics.get_player_pawn(world,0) if world else None
comp = pawn.get_component_by_class(unreal.WorkerAppearanceComponent) if pawn else None
L("comp: %s" % (comp.get_name() if comp else None))
if comp:
    S = unreal.WorkerSlot
    comp.apply_default_preset()
    L("hair options: %s" % comp.get_options_for_slot(S.HAIR))
    L("torso options: %s" % comp.get_options_for_slot(S.TORSO)[:6])
    L("headgear options: %s" % comp.get_options_for_slot(S.HEADGEAR)[:8])
    # swaps
    L("set hair Long -> %s" % comp.set_slot_by_key(S.HAIR, "Long"))
    L("set torso Jacket_Worker_Blue -> %s" % comp.set_slot_by_key(S.TORSO, "Jacket_Worker_Blue"))
    L("set legs Jumpsuit -> %s" % comp.set_slot_by_key(S.LEGS, "Jumpsuit"))
    L("set headgear Yellow(helmet) -> %s" % comp.set_slot_by_key(S.HEADGEAR, "Helmet_Worker_Yellow"))
    L("set glasses aviator -> %s" % comp.set_slot_by_key(S.GLASSES, "Aviator"))
    # inspect resulting parts
    parts=pawn.get_components_by_class(unreal.SkeletalMeshComponent)
    L("--- skeletal components (%d) ---" % len(parts))
    for c in parts:
        sm=None
        try: sm=c.get_skinned_asset()
        except Exception: pass
        if sm and sm.get_name() not in ("SKM_Worker_Male_1","hazmat"):
            L("  %s" % sm.get_name())
with open("C:/unrealEngine/avariika/Scripts/test_wear_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
