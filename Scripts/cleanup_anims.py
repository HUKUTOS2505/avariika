import unreal, json
eal = unreal.EditorAssetLibrary
D = "/Game/Characters/Operator/Anims_Retarget/"
# close all asset editors so deletes succeed
try: unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).close_all_asset_editors()
except Exception as e: unreal.log("close err "+str(e))
remove = ["Op_AS_Fixing","Op_AS_FixingSomething",
          "Op_Emote105_UE5Anim","Op_Emote106_UE5Anim","Op_Emote105_UE4Anim",
          "Op_Emote101_UE5Anim","Op_Emote102_UE5Anim","Op_Emote103_UE5Anim","Op_Emote104_UE5Anim",
          "Op_Emote107_UE5Anim","Op_Emote108_UE5Anim","Op_Emote109_UE5Anim","Op_Emote110_UE5Anim",
          "Op_Emote111_UE5Anim","Op_Emote112_UE5Anim",
          "Op_AS_Drilling","Op_AS_DrivingNails","Op_AS_TighteningWithSocketWrench","Op_AS_CarryBag","Op_AS_HangingSomethingHeavy"]
gone=[]
for n in remove:
    p=D+n
    try:
        if eal.does_asset_exist(p): eal.delete_asset(p); gone.append(n)
    except Exception as e: unreal.log("del err "+n+" "+str(e))
# fresh list
import unreal as u
ar = u.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous([D[:-1]], True, False)
names = sorted(str(a.asset_name) for a in ar.get_assets(u.ARFilter(class_names=["AnimSequence"], package_paths=[D[:-1]], recursive_paths=True)))
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_op_list.txt","w") as f: f.write("\n".join(names))
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_cleanup.json","w") as f: json.dump({"removed":gone,"count_now":len(names)}, f, indent=1)
u.log("CLEANUP removed %d, now %d" % (len(gone), len(names)))
