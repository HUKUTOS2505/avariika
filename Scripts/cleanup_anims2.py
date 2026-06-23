import unreal, json
eal = unreal.EditorAssetLibrary
D = "/Game/Characters/Operator/Anims_Retarget/"
try: unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).close_all_asset_editors()
except Exception as e: unreal.log("close err "+str(e))
remove = [
 "Op_am_StandDrillLow_01_Drill","Op_am_StandDrillLow_Trans_Stand","Op_am_Stand_Trans_StandDrillLow",
 "Op_Worker_FixLandingGear_Loop","Op_Worker_FixPanelOverhead_Loop","Op_Worker_FixWiresOverhead_Loop","Op_Worker_HammerPanelStepUp_Loop",
 "Op_Jump_From_Stand","Op_Jump_From_Jog",
 "Op_Loot_Locker_GrabItem","Op_Loot_Fridge_GrabItem","Op_Loot_FloorPickUp_Kneel_SmallBox",
 "Op_am_Moonshine_01_Drink_Loop","Op_am_Moonshine_Trans_Stand",
]
gone=[]
for n in remove:
    p=D+n
    try:
        if eal.does_asset_exist(p): eal.delete_asset(p); gone.append(n)
    except Exception as e: unreal.log("del err "+n+" "+str(e))
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous([D[:-1]], True, False)
names = sorted(str(a.asset_name) for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], package_paths=[D[:-1]], recursive_paths=True)))
# check meshy jump options on operator (Anims folder, no retarget)
jumps=[]
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Characters/Operator/Anims"], recursive_paths=True)):
    nm=str(a.asset_name)
    if "jump" in nm.lower() or "fall" in nm.lower(): jumps.append(nm)
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_cleanup2.json","w") as f:
    json.dump({"removed":gone,"count_now":len(names),"meshy_jumps":jumps}, f, indent=1)
unreal.log("CLEANUP2 removed %d now %d" % (len(gone), len(names)))
