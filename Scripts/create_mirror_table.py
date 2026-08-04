import unreal
_OUT=[]
def L(s): unreal.log("MIRROR| "+str(s)); _OUT.append(str(s))

skel = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin")
L("skeleton: %s" % (skel.get_name() if skel else None))
tools = unreal.AssetToolsHelpers.get_asset_tools()
path = "/Game/Avariika/Anim/Rig"
name = "MDT_Mannequin"
if unreal.EditorAssetLibrary.does_asset_exist(path+"/"+name):
    L("MDT_Mannequin already exists");
else:
    fac = None
    for fn in ("MirrorDataTableFactory","MirrorDataTableFactoryNew"):
        cls = getattr(unreal, fn, None)
        if cls:
            try:
                fac = cls()
                try: fac.set_editor_property("target_skeleton", skel)
                except Exception:
                    try: fac.set_editor_property("skeleton", skel)
                    except Exception: pass
                L("factory: %s" % fn); break
            except Exception as e: L("fac %s err %s"%(fn,e))
    if not fac:
        L("NO MirrorDataTable factory available")
    else:
        try:
            mdt = tools.create_asset(name, path, unreal.MirrorDataTable, fac)
            L("created: %s" % (mdt is not None))
            if mdt:
                try:
                    if mdt.get_editor_property("skeleton") is None and skel:
                        mdt.set_editor_property("skeleton", skel)
                except Exception: pass
                # standard l<->r suffix/prefix rules
                try:
                    M = unreal.MirrorRowAxis  # probe
                except Exception: pass
                exprs=[]
                def expr(find, repl):
                    e = unreal.MirrorFindReplaceExpression()
                    e.set_editor_property("find_string", find)
                    e.set_editor_property("replace_string", repl)
                    try: e.set_editor_property("find_replace_method", unreal.MirrorFindReplaceMethod.SUFFIX)
                    except Exception: pass
                    return e
                for a,b in (("_l","_r"),("_r","_l"),("_L","_R"),("_R","_L")):
                    exprs.append(expr(a,b))
                try:
                    mdt.set_editor_property("mirror_find_replace_expressions", exprs)
                except Exception as e:
                    try: mdt.set_editor_property("find_replace_expressions", exprs)
                    except Exception as e2: L("exprs set err %s / %s"%(e,e2))
                unreal.EditorLoadingAndSavingUtils.save_packages([mdt.get_outermost()], False)
                L("MDT saved with %d rules" % len(exprs))
        except Exception as e:
            L("create err %s" % str(e)[:160])

with open("C:/unrealEngine/avariika/Scripts/create_mirror_table_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
