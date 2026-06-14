# Curated audio import for Avariika. Imports staged ASCII WAVs into /Game/Audio,
# trims oversized loop/ambient sources via stdlib wave, sets looping, saves to disk.
# Run from the editor (Claudius editor.run_python_script).
import unreal, wave, os, json

STAGE = r"D:/unrealEngine/avariika/RawAssets/_audio_stage"
RESULT = r"D:/unrealEngine/avariika/Scripts/import_audio_result.json"
tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

SFX = "/Game/Audio/SFX"
AMB = "/Game/Audio/Ambient"

# (staged_filename, dest_folder, asset_name, loop, trim_seconds[0=none])
MAN = [
    ("Repair_ValveRatchet_1.wav",        SFX+"/Repair", "Repair_ValveRatchet_1", False, 0),
    ("Repair_ValveRatchet_2.wav",        SFX+"/Repair", "Repair_ValveRatchet_2", False, 0),
    ("Repair_ValveRatchet_3.wav",        SFX+"/Repair", "Repair_ValveRatchet_3", False, 0),
    ("Repair_ValveStrip_1.wav",          SFX+"/Repair", "Repair_ValveStrip_1",   False, 0),
    ("Repair_ValveStrip_2.wav",          SFX+"/Repair", "Repair_ValveStrip_2",   False, 0),
    ("Repair_GenPull_Loop.wav",          SFX+"/Repair", "Repair_GenPull_Loop",   True,  0),
    ("Repair_Insert_1.wav",              SFX+"/Repair", "Repair_Insert_1",       False, 0),
    ("Repair_Insert_2.wav",              SFX+"/Repair", "Repair_Insert_2",       False, 0),
    ("Repair_EngineIdle_Loop.wav",       SFX+"/Repair", "Repair_EngineIdle_Loop",True,  12),
    ("Repair_JuryRig_Loop.wav",          SFX+"/Repair", "Repair_JuryRig_Loop",   True,  0),
    ("Repair_FuelFill_Loop.wav",         SFX+"/Repair", "Repair_FuelFill_Loop",  True,  0),
    ("Hazard_FireLoop.wav",              SFX+"/Hazard", "Hazard_FireLoop",       True,  0),
    ("Hazard_LampHum_Loop.wav",          SFX+"/Hazard", "Hazard_LampHum_Loop",   True,  0),
    ("Hazard_Overload_1.wav",            SFX+"/Hazard", "Hazard_Overload_1",     False, 0),
    ("Hazard_ExplosionGas_1.wav",        SFX+"/Hazard", "Hazard_ExplosionGas_1", False, 0),
    ("Item_ExtinguisherSpray_Loop.wav",  SFX+"/Item",   "Item_ExtinguisherSpray_Loop", True, 0),
    ("Foley_BodyFall_1.wav",             SFX+"/Foley",  "Foley_BodyFall_1",      False, 0),
    ("Foley_BodyFall_2.wav",             SFX+"/Foley",  "Foley_BodyFall_2",      False, 0),
    ("Amb_Hall_Loop.wav",                AMB,           "Amb_Hall_Loop",         True,  25),
    ("Amb_Corridor_Loop.wav",            AMB,           "Amb_Corridor_Loop",     True,  25),
    ("Amb_WaterDrips_Loop.wav",          AMB,           "Amb_WaterDrips_Loop",   True,  25),
    ("Amb_Rain_Loop.wav",                AMB,           "Amb_Rain_Loop",         True,  20),
    ("Amb_RainThunder_1.wav",            AMB,           "Amb_RainThunder_1",     False, 15),
]


def trim_wav(src, dst, seconds):
    w = wave.open(src, "rb")
    p = w.getparams()
    n = min(w.getnframes(), int(p.framerate * seconds))
    frames = w.readframes(n)
    w.close()
    o = wave.open(dst, "wb")
    o.setparams(p)
    o.writeframes(frames)
    o.close()


results = []
for fname, folder, name, loop, trim in MAN:
    rec = {"name": name, "ok": False, "loop": loop}
    src = os.path.join(STAGE, fname)
    if not os.path.exists(src):
        rec["err"] = "missing staged file"
        results.append(rec); continue
    if trim > 0:
        tsrc = os.path.join(STAGE, "_t_" + fname)
        try:
            trim_wav(src, tsrc, trim); src = tsrc
        except Exception as e:
            rec["err"] = "trim failed: " + str(e)
            # fall back to full file rather than skipping
            src = os.path.join(STAGE, fname)
            rec["trim_fallback"] = True
    pkg = folder + "/" + name
    task = unreal.AssetImportTask()
    task.filename = src
    task.destination_path = folder
    task.destination_name = name
    task.automated = True
    task.replace_existing = True
    task.save = True
    try:
        tools.import_asset_tasks([task])
    except Exception as e:
        rec["err"] = "import exception: " + str(e)
        results.append(rec); continue
    # Grab the imported object directly from the task (load_asset is unreliable
    # in the same tick due to asset-registry timing).
    asset = None
    try:
        objs = task.get_editor_property("result")
        if objs:
            asset = objs[0]
    except Exception:
        pass
    if asset is None:
        asset = eal.load_asset(pkg)
    if asset is None:
        paths = []
        try:
            paths = list(task.get_editor_property("imported_object_paths"))
        except Exception:
            pass
        rec["err"] = "no object returned; imported_paths=" + str(paths)
        results.append(rec); continue
    try:
        if loop and isinstance(asset, unreal.SoundWave):
            asset.set_editor_property("looping", True)
            eal.save_loaded_asset(asset, False)
        rec["ok"] = True
    except Exception as e:
        rec["err"] = "loop/save failed: " + str(e)
    results.append(rec)

ok = sum(1 for r in results if r["ok"])
with open(RESULT, "w") as f:
    json.dump({"ok": ok, "total": len(results), "results": results}, f, indent=1)
unreal.log("AUDIO IMPORT DONE: {}/{} -> {}".format(ok, len(results), RESULT))
