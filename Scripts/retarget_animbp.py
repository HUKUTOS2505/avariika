import unreal, json, traceback
OUT = r"C:/unrealEngine/avariika/Scripts/manifests/_retarget_animbp.json"
R = {"steps": [], "created": [], "err": None}
try:
    eal = unreal.EditorAssetLibrary
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    BASE = "/Game/Hospital/Free_Content_Epic_Games/Mannequin"
    rt = unreal.load_asset("/Game/Characters/Operator/Rig/RTG_MCO_to_Operator.RTG_MCO_to_Operator")
    src = unreal.load_asset(BASE + "/Character/Mesh/SK_Mannequin.SK_Mannequin")
    op = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
    R["have"] = {"rt": bool(rt), "src": bool(src), "op": bool(op)}

    picks = []
    for cls in ["AnimBlueprint", "BlendSpace", "BlendSpace1D", "AnimSequence"]:
        for a in ar.get_assets(unreal.ARFilter(class_names=[cls], package_paths=[BASE], recursive_paths=True)):
            picks.append(a)
    R["pick_names"] = sorted(str(p.asset_name) for p in picks)

    if picks and rt and src and op:
        try:
            unreal.IKRetargetBatchOperation.duplicate_and_retarget(picks, src, op, rt, search="", replace="", prefix="Op_", suffix="")
            R["steps"].append("retarget ok")
        except Exception as e:
            R["steps"].append("retarget err " + str(e))

    ar.scan_paths_synchronous(["/Game/Characters/Operator", BASE], True, False)
    DEST = "/Game/Characters/Operator/Locomotion"
    for a in ar.get_assets(unreal.ARFilter(package_paths=["/Game/Characters/Operator", BASE], recursive_paths=True)):
        nm = str(a.asset_name)
        if nm.startswith("Op_ThirdPerson"):
            srcp = str(a.package_name); dstp = DEST + "/" + nm
            try:
                if eal.does_asset_exist(srcp) and srcp != dstp:
                    eal.rename_asset(srcp, dstp)
            except Exception as e:
                R["steps"].append("move err " + nm + " " + str(e))
            try: cls = str(a.asset_class_path.asset_name)
            except Exception: cls = "?"
            R["created"].append({"name": nm, "class": cls})
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(OUT, "w") as f: json.dump(R, f, indent=1)
unreal.log("RETARGET_ANIMBP done")
