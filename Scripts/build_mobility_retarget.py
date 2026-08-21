# UE4-mann (Mobility Pro) -> SK_Mannequin (UE5) ретаргетер + тест 1 клипа.
# Учтён урок floor-sink: Root-чейн + retarget root = pelvis. Итог -> Scripts/mobility_retarget_result.txt
import unreal, re
eal = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
F = "/Game/Avariika/Anim/Rig"
lines = []

src_mesh = unreal.load_asset("/Game/Mobility_01/Character/Mesh/SK_Mannequin.SK_Mannequin")
lines.append("src_mesh(UE4)=%s cls=%s" % (bool(src_mesh), src_mesh.get_class().get_name() if src_mesh else "-"))

# 1) IK-риг источника (UE4-mann)
rig = unreal.load_asset(F + "/IK_MobilityUE4.IK_MobilityUE4")
if rig is None:
    rig = tools.create_asset("IK_MobilityUE4", F, unreal.IKRigDefinition, unreal.IKRigDefinitionFactory())
sc = unreal.IKRigController.get_controller(rig)
sc.set_skeletal_mesh(src_mesh)
sc.set_retarget_root("pelvis")
for c in list(sc.get_retarget_chains()):
    sc.remove_retarget_chain(c.chain_name)
def names(): return [str(c.chain_name) for c in sc.get_retarget_chains()]
def add(name, s, ends):
    for e in (ends if isinstance(ends, list) else [ends]):
        try: sc.add_retarget_chain(name, s, e, "None")
        except Exception: pass
        if name in names(): return e
        try: sc.remove_retarget_chain(name)
        except Exception: pass
    lines.append("chain %s FAILED" % name); return None
add("Root", "root", ["root"])                 # <-- floor-sink fix
add("Spine", "spine_01", ["spine_03","spine_02"])
add("Neck", "neck_01", ["neck_01"])
add("Head", "head", ["head"])
add("LeftClavicle", "clavicle_l", ["clavicle_l"])
add("LeftArm", "upperarm_l", ["hand_l"])
add("RightClavicle", "clavicle_r", ["clavicle_r"])
add("RightArm", "upperarm_r", ["hand_r"])
add("LeftLeg", "thigh_l", ["ball_l","foot_l"])
add("RightLeg", "thigh_r", ["ball_r","foot_r"])
eal.save_loaded_asset(rig, False)
lines.append("IK_MobilityUE4 chains: " + str(names()))

# 2) ретаргетер -> IK_Mannequin (UE5)
tgt_rig = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
rt = unreal.load_asset(F + "/RTG_MobilityUE4_to_Mannequin.RTG_MobilityUE4_to_Mannequin")
if rt is None:
    rt = tools.create_asset("RTG_MobilityUE4_to_Mannequin", F, unreal.IKRetargeter, unreal.IKRetargetFactory())
rtc = unreal.IKRetargeterController.get_controller(rt)
rt.set_editor_property("source_ik_rig_asset", rig)
rt.set_editor_property("target_ik_rig_asset", tgt_rig)
for i in range(rtc.get_num_retarget_ops()-1, -1, -1):
    try: rtc.remove_retarget_op(i)
    except Exception: pass
try: rtc.add_default_ops()
except Exception as e: lines.append("ops err "+str(e))
try:
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.TARGET, tgt_rig)
    rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.SOURCE, rig)
except Exception as e: lines.append("assign err "+str(e))
mapped = False
for mt in (unreal.AutoMapChainType.EXACT, unreal.AutoMapChainType.FUZZY):
    try:
        rtc.auto_map_chains(mt, True); mapped = True; lines.append("map %s ok" % mt); break
    except Exception as e: lines.append("map err "+str(e))
for side in (unreal.RetargetSourceOrTarget.SOURCE, unreal.RetargetSourceOrTarget.TARGET):
    try: rtc.auto_align_all_bones(side)
    except Exception: pass
KEEP={"Pelvis Motion","FK Chains","Root Motion","Remap Curves"}; seen=set()
for i in range(rtc.get_num_retarget_ops()):
    nm=str(rtc.get_op_name(i)); base=re.sub(r"_\d+$","",nm); en=(base in KEEP) and (base not in seen)
    if en: seen.add(base)
    try: rtc.set_retarget_op_enabled(i, en)
    except Exception: pass
eal.save_loaded_asset(rt, False)

# 3) тест 1 клип
ar = unreal.AssetRegistryHelpers.get_asset_registry()
tgt_mesh = unreal.IKRigController.get_controller(tgt_rig).get_skeletal_mesh()
f = unreal.ARFilter(class_names=["AnimSequence"], package_paths=["/Game/Mobility_01/Animation/In-Place"], recursive_paths=True)
pick = {str(a.asset_name): a for a in ar.get_assets(f)}.get("MOB1_M1_Walk_F_IP")
lines.append("pick Walk_F=%s tgt_mesh=%s" % (bool(pick), bool(tgt_mesh)))
if pick and src_mesh and tgt_mesh:
    try:
        unreal.IKRetargetBatchOperation.duplicate_and_retarget([pick], src_mesh, tgt_mesh, rt, search="", replace="", prefix="RT_", suffix="")
    except Exception as e:
        lines.append("retarget err: "+str(e)[:200])
    ar.scan_paths_synchronous(["/Game"], True, False)
    for a in ar.get_assets(unreal.ARFilter(class_names=["AnimSequence"], recursive_paths=True)):
        if str(a.asset_name)=="RT_MOB1_M1_Walk_F_IP":
            srcp=str(a.package_name); dstp="/Game/Avariika/Anim/Locomotion/RT_MOB1_M1_Walk_F_IP"
            try:
                if srcp!=dstp: eal.rename_asset(srcp, dstp)
            except Exception: pass
            obj=unreal.load_asset(dstp+".RT_MOB1_M1_Walk_F_IP") or unreal.load_asset(srcp+".RT_MOB1_M1_Walk_F_IP")
            if obj:
                eal.save_loaded_asset(obj, False)
                sk=obj.get_editor_property("skeleton")
                lines.append("RESULT skeleton=%s frames=%s" % (sk.get_name() if sk else "-", obj.get_editor_property("number_of_sampled_frames")))
with open(r"C:/unrealEngine/avariika/Scripts/mobility_retarget_result.txt","w",encoding="utf-8") as fo:
    fo.write("\n".join(lines)+"\n")
print("BUILD_MOBILITY done")
