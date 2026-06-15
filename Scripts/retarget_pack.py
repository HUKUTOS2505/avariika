import unreal, json
JOB = r"D:/unrealEngine/avariika/Scripts/retarget_job.json"
OUT = r"D:/unrealEngine/avariika/Scripts/manifests/_retarget_result.json"
with open(JOB) as fp: J = json.load(fp)
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
R = {"job": J.get("label", ""), "steps": [], "picked": [], "retargeted": [], "missing": []}

rt = unreal.load_asset(J["retargeter"])
src = unreal.load_asset(J["source_mesh"])
op = unreal.load_asset(J["operator_mesh"])
dest = J["dest"]; prefix = J.get("prefix", "Op_")
R["have"] = {"rt": bool(rt), "src": bool(src), "op": bool(op)}

want = set(J["wants"])
found = {}
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], package_paths=[J["pack_root"]], recursive_paths=True)):
    nm = str(a.asset_name)
    if nm in want:
        found[nm] = a
picks = list(found.values())
R["picked"] = sorted(found.keys())
R["missing"] = sorted(want - set(found.keys()))

if picks and rt and src and op:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget(
            picks, src, op, rt, search="", replace="", prefix=prefix, suffix="")
        R["steps"].append("retarget ok")
    except Exception as e:
        R["steps"].append("retarget err " + str(e))

picknames = set(prefix + n for n in found.keys())
ar.scan_paths_synchronous(["/Game"], True, False)
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
    nm = str(a.asset_name)
    if nm in picknames:
        srcp = str(a.package_name); dstp = dest + "/" + nm
        try:
            if eal.does_asset_exist(srcp) and srcp != dstp:
                eal.rename_asset(srcp, dstp)
        except Exception: pass
        obj = unreal.load_asset(dstp + "." + nm) or unreal.load_asset(srcp + "." + nm)
        if obj:
            eal.save_loaded_asset(obj, False); R["retargeted"].append(nm)
with open(OUT, "w") as fp: json.dump(R, fp, indent=1)
unreal.log("RETARGET_PACK done: " + J.get("label",""))
