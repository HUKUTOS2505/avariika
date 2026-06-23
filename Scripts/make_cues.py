import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/make_cues_result.json"
tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

CUES = [
    ("SC_ValveRatchet", "/Game/Audio/SFX/Repair", [
        "/Game/Audio/SFX/Repair/Repair_ValveRatchet_1.Repair_ValveRatchet_1",
        "/Game/Audio/SFX/Repair/Repair_ValveRatchet_2.Repair_ValveRatchet_2",
        "/Game/Audio/SFX/Repair/Repair_ValveRatchet_3.Repair_ValveRatchet_3",
    ]),
    ("SC_ValveStrip", "/Game/Audio/SFX/Repair", [
        "/Game/Audio/SFX/Repair/Repair_ValveStrip_1.Repair_ValveStrip_1",
        "/Game/Audio/SFX/Repair/Repair_ValveStrip_2.Repair_ValveStrip_2",
    ]),
    ("SC_Insert", "/Game/Audio/SFX/Repair", [
        "/Game/Audio/SFX/Repair/Repair_Insert_1.Repair_Insert_1",
        "/Game/Audio/SFX/Repair/Repair_Insert_2.Repair_Insert_2",
    ]),
    ("SC_BodyFall", "/Game/Audio/SFX/Foley", [
        "/Game/Audio/SFX/Foley/Foley_BodyFall_1.Foley_BodyFall_1",
        "/Game/Audio/SFX/Foley/Foley_BodyFall_2.Foley_BodyFall_2",
    ]),
    ("SC_MinigameHit", "/Game/Audio/SFX/Cues", [
        "/Game/Survival_SFX/Craft/Anvil_hit_%d.Anvil_hit_%d" % (i, i) for i in range(1, 6)
    ]),
    ("SC_Shove", "/Game/Audio/SFX/Cues", [
        "/Game/Survival_SFX/Survival/Punch_%d.Punch_%d" % (i, i) for i in range(1, 6)
    ]),
]

res = []
for name, folder, wave_paths in CUES:
    rec = {"cue": name, "ok": False}
    pkg = folder + "/" + name
    cue = unreal.load_asset(pkg + "." + name)
    if cue is None:
        cue = tools.create_asset(name, folder, unreal.SoundCue, unreal.SoundCueFactoryNew())
    if cue is None:
        rec["err"] = "create failed"; res.append(rec); continue
    waves = [unreal.load_asset(w) for w in wave_paths]
    waves = [w for w in waves if w is not None]
    rec["waves"] = len(waves)
    if not waves:
        rec["err"] = "no waves"; res.append(rec); continue
    try:
        rnd = unreal.new_object(unreal.SoundNodeRandom, cue)
        players = []
        for w in waves:
            p = unreal.new_object(unreal.SoundNodeWavePlayer, cue)
            p.set_editor_property("sound_wave_asset_ptr", w)
            players.append(p)
        rnd.set_editor_property("child_nodes", players)
        rnd.set_editor_property("weights", [1.0] * len(players))
        try:
            rnd.set_editor_property("randomize_without_replacement", True)
        except Exception:
            pass
        cue.set_editor_property("first_node", rnd)
        eal.save_loaded_asset(cue, False)
        rec["ok"] = True
        rec["children"] = len(rnd.get_editor_property("child_nodes"))
    except Exception as e:
        rec["err"] = str(e)
    res.append(rec)

# cleanup the throwaway test cue
try:
    if eal.does_asset_exist("/Game/Audio/SFX/Repair/SC_Test_Ratchet"):
        eal.delete_asset("/Game/Audio/SFX/Repair/SC_Test_Ratchet")
except Exception:
    pass

ok = sum(1 for r in res if r["ok"])
with open(OUT, "w") as f:
    json.dump({"ok": ok, "total": len(res), "results": res}, f, indent=1)
unreal.log("MAKE CUES: %d/%d" % (ok, len(res)))
