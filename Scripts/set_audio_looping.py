import unreal, json
eal = unreal.EditorAssetLibrary
OUT = r"C:/unrealEngine/avariika/Scripts/set_audio_looping_result.json"

# The editor's asset registry doesn't auto-pick up Python-written .uasset files;
# force a synchronous rescan so load_asset can find them.
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(["/Game/Audio"], force_rescan=True, ignore_deny_list_scan_filters=False)

LOOPS = [
    "/Game/Audio/SFX/Repair/Repair_GenPull_Loop",
    "/Game/Audio/SFX/Repair/Repair_EngineIdle_Loop",
    "/Game/Audio/SFX/Repair/Repair_JuryRig_Loop",
    "/Game/Audio/SFX/Repair/Repair_FuelFill_Loop",
    "/Game/Audio/SFX/Hazard/Hazard_FireLoop",
    "/Game/Audio/SFX/Hazard/Hazard_LampHum_Loop",
    "/Game/Audio/SFX/Item/Item_ExtinguisherSpray_Loop",
    "/Game/Audio/Ambient/Amb_Hall_Loop",
    "/Game/Audio/Ambient/Amb_Corridor_Loop",
    "/Game/Audio/Ambient/Amb_WaterDrips_Loop",
    "/Game/Audio/Ambient/Amb_Rain_Loop",
]

res = []
for pkg in LOOPS:
    rec = {"pkg": pkg, "ok": False}
    objp = pkg + "." + pkg.split("/")[-1]
    a = unreal.load_asset(objp)
    if a is None:
        rec["err"] = "load None"; res.append(rec); continue
    if not isinstance(a, unreal.SoundWave):
        rec["err"] = "not SoundWave: " + a.get_class().get_name(); res.append(rec); continue
    try:
        a.set_editor_property("looping", True)
        eal.save_loaded_asset(a, False)
        rec["ok"] = True
        rec["looping"] = a.get_editor_property("looping")
    except Exception as e:
        rec["err"] = str(e)
    res.append(rec)

ok = sum(1 for r in res if r["ok"])
with open(OUT, "w") as f:
    json.dump({"ok": ok, "total": len(res), "results": res}, f, indent=1)
unreal.log("SET LOOPING {}/{}".format(ok, len(res)))
