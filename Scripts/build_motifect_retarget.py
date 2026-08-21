# Строит ретаргетер Motifect(AI-stickman) -> SK_Mannequin(UE5) на базе СУЩЕСТВУЮЩИХ ригов.
# source = IK_Motifect, target = WorkAnimations IK_Mannequin. Создаёт ОДИН ассет (не bulk).
# Итог -> Scripts/motifect_retarget_result.txt
import unreal

eal = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
F = "/Game/Avariika/Anim/Rig"
lines = []

src = unreal.load_asset(F + "/IK_Motifect.IK_Motifect")
tgt = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin")
lines.append("source IK_Motifect: %s" % bool(src))
lines.append("target IK_Mannequin(WorkAnim): %s" % bool(tgt))

if not src or not tgt:
    with open(r"C:/unrealEngine/avariika/Scripts/motifect_retarget_result.txt","w",encoding="utf-8") as f:
        f.write("\n".join(lines) + "\nABORT: рига(ов) нет\n")
    print("ABORT")
else:
    rt = unreal.load_asset(F + "/RTG_Motifect_to_Mannequin.RTG_Motifect_to_Mannequin")
    if rt is None:
        rt = tools.create_asset("RTG_Motifect_to_Mannequin", F, unreal.IKRetargeter, unreal.IKRetargetFactory())
    rtc = unreal.IKRetargeterController.get_controller(rt)
    rt.set_editor_property("source_ik_rig_asset", src)
    rt.set_editor_property("target_ik_rig_asset", tgt)
    # очистить старые ops, добавить дефолтные
    try:
        for i in range(rtc.get_num_retarget_ops() - 1, -1, -1):
            try: rtc.remove_retarget_op(i)
            except Exception: pass
    except Exception: pass
    if rtc.get_num_retarget_ops() == 0:
        try: rtc.add_default_ops(); lines.append("ops added")
        except Exception as e: lines.append("ops err " + str(e))
    try:
        rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.TARGET, tgt)
        rtc.assign_ik_rig_to_all_ops(unreal.RetargetSourceOrTarget.SOURCE, src)
        lines.append("assign ok")
    except Exception as e: lines.append("assign err " + str(e))
    try: rtc.auto_map_chains(unreal.AutoMapChainType.FUZZY, True); lines.append("map fuzzy ok")
    except Exception as e: lines.append("map err " + str(e))
    for side, lbl in [(unreal.RetargetSourceOrTarget.SOURCE,"src"),(unreal.RetargetSourceOrTarget.TARGET,"tgt")]:
        try: rtc.auto_align_all_bones(side); lines.append("align "+lbl+" ok")
        except Exception as e: lines.append("align "+lbl+" err "+str(e))
    eal.save_loaded_asset(rt, False)
    lines.append("has_source=%s has_target=%s" % (rt.has_source_ik_rig(), rt.has_target_ik_rig()))
    # показать смапленные чейны
    try:
        mappings = rtc.get_chain_mappings() if hasattr(rtc, 'get_chain_mappings') else []
        lines.append("chain mappings: %d" % len(mappings))
    except Exception: pass
    with open(r"C:/unrealEngine/avariika/Scripts/motifect_retarget_result.txt","w",encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")
    print("BUILD_MOTIFECT_RETARGET done")
