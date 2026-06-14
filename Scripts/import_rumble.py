import unreal, wave, os, json
STAGE = r"D:/unrealEngine/avariika/RawAssets/_audio_stage"
OUT = r"D:/unrealEngine/avariika/Scripts/import_rumble_result.json"
tools = unreal.AssetToolsHelpers.get_asset_tools()

def trim_wav(src, dst, seconds):
    w = wave.open(src, "rb"); p = w.getparams()
    n = min(w.getnframes(), int(p.framerate * seconds)); frames = w.readframes(n); w.close()
    o = wave.open(dst, "wb"); o.setparams(p); o.writeframes(frames); o.close()

src = os.path.join(STAGE, "Hazard_Rumble_1.wav")
tsrc = os.path.join(STAGE, "_t_Hazard_Rumble_1.wav")
rec = {"ok": False}
try:
    trim_wav(src, tsrc, 6); src = tsrc
except Exception as e:
    rec["trim_err"] = str(e)
folder = "/Game/Audio/SFX/Hazard"; name = "Hazard_Rumble_1"
task = unreal.AssetImportTask()
task.filename = src; task.destination_path = folder; task.destination_name = name
task.automated = True; task.replace_existing = True; task.save = True
tools.import_asset_tasks([task])
a = unreal.load_asset(folder + "/" + name + "." + name)
rec["ok"] = a is not None
rec["class"] = a.get_class().get_name() if a else None
with open(OUT, "w") as f:
    json.dump(rec, f, indent=1)
unreal.log("RUMBLE IMPORT: " + str(rec))
