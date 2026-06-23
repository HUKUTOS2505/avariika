import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/scan_mc_actions.json"
R = {}
ar = unreal.AssetRegistryHelpers.get_asset_registry()
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/MC_Sample"], recursive_paths=True)
names = sorted(str(a.asset_name) for a in ar.get_assets(f))
R["total_mc"] = len(names)
# bucket by action keyword
KW = ["vend", "drill", "grab", "pick", "push", "drink", "throw", "kick", "punch",
      "lift", "carry", "drag", "kneel", "crouch", "reach", "wrench", "valve",
      "lever", "switch", "button", "press", "pull", "open", "interact", "use",
      "search", "inspect", "look", "point", "wave", "talk", "conv", "sit", "stand_up",
      "getup", "scared", "fear", "panic", "shock", "electro", "stumble", "trip",
      "fall", "land", "jump", "climb", "vault", "cower", "hide", "hammer", "tool",
      "work", "build", "fix", "repair", "screw", "bolt", "knock", "react", "emotion",
      "idle", "fidget", "breath", "cough"]
buckets = {}
for nm in names:
    low = nm.lower()
    for k in KW:
        if k in low:
            buckets.setdefault(k, []).append(nm)
            break
R["buckets"] = {k: v for k, v in sorted(buckets.items())}
# anything not bucketed
hit = set()
for v in buckets.values(): hit.update(v)
R["unbucketed"] = [n for n in names if n not in hit]
with open(OUT, "w") as fp:
    json.dump(R, fp, indent=1)
unreal.log("SCAN_MC_ACTIONS done")
