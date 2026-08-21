# Диагностика чейнов IK_Motifect (source) vs IK_Mannequin (target) + чистка мусорных RT_ из корня /Game.
# Итог -> Scripts/rig_chains.txt
import unreal
eal = unreal.EditorAssetLibrary
lines = []

def chains_of(path):
    rig = unreal.load_asset(path)
    if not rig: return None
    try:
        c = unreal.IKRigController.get_controller(rig)
        return [str(x.chain_name) for x in c.get_retarget_chains()]
    except Exception as e:
        return ["ERR " + str(e)]

mot = chains_of("/Game/Avariika/Anim/Rig/IK_Motifect.IK_Motifect")
crew = chains_of("/Game/Avariika/Anim/Rig/IK_CrewUE4.IK_CrewUE4")
mann = chains_of("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
lines.append("IK_Motifect chains (%d): %s" % (len(mot or []), mot))
lines.append("IK_CrewUE4 chains (%d): %s" % (len(crew or []), crew))
lines.append("IK_Mannequin chains (%d): %s" % (len(mann or []), mann))

# чистка мусорных RT_ ассетов в корне /Game
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game"], True, False)
killed = []
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
    nm = str(a.asset_name); pkg = str(a.package_name)
    # мусор: RT_ прямо в /Game/ (корень), не в Injured
    if nm.startswith("RT_") and pkg.count("/") == 2:
        try:
            if eal.delete_asset(pkg): killed.append(pkg)
        except Exception: pass
lines.append("killed junk RT_: " + str(killed))

with open(r"C:/unrealEngine/avariika/Scripts/rig_chains.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("DIAG done")
