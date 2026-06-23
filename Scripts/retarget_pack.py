import unreal, json
JOB = r"C:/unrealEngine/avariika/Scripts/retarget_job.json"
OUT = r"C:/unrealEngine/avariika/Scripts/manifests/_retarget_result.json"
with open(JOB) as fp: SPEC = json.load(fp)
JOBS = SPEC if isinstance(SPEC, list) else [SPEC]
eal = unreal.EditorAssetLibrary
ar = unreal.AssetRegistryHelpers.get_asset_registry()
RES = []

def run_job(J):
    R = {"label": J.get("label", ""), "steps": [], "picked": [], "retargeted": [], "missing": []}
    rt = unreal.load_asset(J["retargeter"]); src = unreal.load_asset(J["source_mesh"]); op = unreal.load_asset(J["operator_mesh"])
    dest = J["dest"]; prefix = J.get("prefix", "Op_")
    R["have"] = {"rt": bool(rt), "src": bool(src), "op": bool(op)}
    want = set(J["wants"]); found = {}
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], package_paths=[J["pack_root"]], recursive_paths=True)):
        nm = str(a.asset_name)
        if nm in want: found[nm] = a
    picks = list(found.values())
    R["picked"] = sorted(found.keys()); R["missing"] = sorted(want - set(found.keys()))
    # delete existing dest Op_<name> so re-retargets overwrite cleanly
    for n in found.keys():
        dp = dest + "/" + prefix + n
        try:
            if eal.does_asset_exist(dp): eal.delete_asset(dp)
        except Exception: pass
    if picks and rt and src and op:
        try:
            unreal.IKRetargetBatchOperation.duplicate_and_retarget(picks, src, op, rt, search="", replace="", prefix=prefix, suffix="")
            R["steps"].append("retarget ok")
        except Exception as e:
            R["steps"].append("retarget err " + str(e))
    R["_picknames"] = set(prefix + n for n in found.keys())
    R["_dest"] = dest
    return R

for J in JOBS:
    RES.append(run_job(J))

# one scan, then move+save all
ar.scan_paths_synchronous(["/Game"], True, False)
allpick = {}
for R in RES:
    for n in R["_picknames"]: allpick[n] = R["_dest"]
for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
    nm = str(a.asset_name)
    if nm in allpick:
        dest = allpick[nm]; srcp = str(a.package_name); dstp = dest + "/" + nm
        try:
            if eal.does_asset_exist(srcp) and srcp != dstp:
                eal.rename_asset(srcp, dstp)
        except Exception: pass
        obj = unreal.load_asset(dstp + "." + nm) or unreal.load_asset(srcp + "." + nm)
        if obj:
            eal.save_loaded_asset(obj, False)
            for R in RES:
                if nm in R["_picknames"]: R["retargeted"].append(nm)
for R in RES:
    R.pop("_picknames", None); R.pop("_dest", None)
with open(OUT, "w") as fp: json.dump(RES, fp, indent=1)
unreal.log("RETARGET_PACK multi done: %d jobs" % len(JOBS))
