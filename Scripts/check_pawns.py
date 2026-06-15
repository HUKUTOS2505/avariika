import unreal, json, traceback
R={"placed_pawns":[],"player_starts":[],"err":None}
try:
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        try:
            cn=a.get_class().get_name(); lbl=a.get_actor_label()
            low=(cn+lbl).lower()
            if any(k in low for k in ["character","pawn","firstperson","avaryo"]):
                R["placed_pawns"].append({"label":lbl,"class":cn,"loc":str(a.get_actor_location())})
            if "playerstart" in low or cn=="PlayerStart":
                R["player_starts"].append({"label":lbl,"loc":str(a.get_actor_location())})
        except Exception: pass
    bp=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter")
    cdo=unreal.get_default_object(bp.generated_class())
    R["avaryo_skeletal_components"]=[c.get_name() for c in cdo.get_components_by_class(unreal.SkeletalMeshComponent)]
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_pawns.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("CHECK_PAWNS2 done")
