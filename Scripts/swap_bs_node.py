import unreal
_OUT=[]
def line(s=""):
    unreal.log("SWAPBS| "+str(s)); _OUT.append(str(s))

abp = unreal.load_asset("/Game/Avariika/Anim/Locomotion/ABP_Worker")
newbs = unreal.load_asset("/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility")
line("abp=%s newbs=%s" % (abp is not None, newbs is not None))

graphs=[]
for p in ("function_graphs","uber_graph_pages","macro_graphs","delegate_signature_graphs"):
    try:
        g = abp.get_editor_property(p)
        if g:
            graphs += list(g)
            line("prop %s -> %d graphs: %s" % (p, len(list(g)), [x.get_name() for x in g]))
    except Exception as e:
        line("no prop %s (%s)" % (p, e))

done=False
for g in graphs:
    try:
        nodes = g.get_editor_property("nodes")
    except Exception as e:
        line("graph %s nodes err %s" % (g.get_name(), e)); continue
    for n in (nodes or []):
        cn = n.get_class().get_name()
        if "BlendSpacePlayer" in cn:
            line("FOUND node %s (%s) in graph %s" % (n.get_name(), cn, g.get_name()))
            # approach A: inner FAnimNode struct
            for inner_prop in ("node",):
                try:
                    an = n.get_editor_property(inner_prop)
                    an.set_editor_property("blend_space", newbs)
                    n.set_editor_property(inner_prop, an)
                    line("  set %s.blend_space -> BS_Loco_Mobility OK" % inner_prop)
                    done=True
                except Exception as e:
                    line("  inner '%s' set err: %s" % (inner_prop, e))
            # approach B: direct property
            if not done:
                for dp in ("blend_space","blendspace"):
                    try:
                        n.set_editor_property(dp, newbs); line("  set node.%s OK"%dp); done=True
                    except Exception as e:
                        line("  direct '%s' err: %s"%(dp,e))

line("DONE=%s" % done)
if done:
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(abp)
        unreal.EditorAssetLibrary.save_asset("/Game/Avariika/Anim/Locomotion/ABP_Worker", only_if_is_dirty=False)
        line("recompiled+saved ABP_Worker")
    except Exception as e:
        line("compile/save err %s"%e)

with open("C:/unrealEngine/avariika/Scripts/swap_bs_node_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
