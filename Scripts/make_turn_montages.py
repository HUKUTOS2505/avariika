import unreal
eal = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
DEST = "/Game"
clips = ['RT_MOB1_M1_Stand_Relaxed_R_45','RT_MOB1_M1_Stand_Relaxed_R_90',
         'RT_MOB1_M1_Stand_Relaxed_R_135','RT_MOB1_M1_Stand_Relaxed_R_180']

for c in clips:
    seq = unreal.load_asset("%s/%s.%s" % (DEST, c, c))
    if seq is None:
        print("MISS seq", c); continue
    mname = c.replace('RT_MOB1_M1_Stand_Relaxed_', 'M_Turn_')  # M_Turn_R_90 etc
    mpath = "%s/%s" % (DEST, mname)
    if eal.does_asset_exist(mpath):
        eal.delete_asset(mpath)
    factory = unreal.AnimMontageFactory()
    try:
        factory.set_editor_property('source_animation', seq)
    except Exception as e:
        print("  factory.source_animation err:", str(e)[:80])
    montage = None
    try:
        montage = tools.create_asset(mname, DEST, unreal.AnimMontage, factory)
    except Exception as e:
        print("  create_asset err %s: %s" % (mname, str(e)[:120]))
    if montage is None:
        print("  %s create FAIL" % mname); continue
    saved = eal.save_asset(mpath, only_if_is_dirty=False)
    exists = eal.does_asset_exist(mpath)
    print("    saved=%s exists=%s" % (saved, exists))
    hrm = '?'
    try:
        hrm = montage.has_root_motion()
    except Exception:
        try: hrm = montage.get_editor_property('extract_root_motion')
        except Exception: pass
    ln = '?'
    try: ln = round(montage.get_play_length(), 2)
    except Exception: pass
    print("  %s  len=%s  hasRootMotion=%s" % (mname, ln, hrm))
print("DONE_MONTAGES")
