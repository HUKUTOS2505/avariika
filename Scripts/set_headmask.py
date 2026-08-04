import unreal
_OUT=[]
def L(s): unreal.log("HMASK| "+str(s)); _OUT.append(str(s))

PATH="/Game/Avariika/Anim/Locomotion/ABP_Worker"
abp=unreal.load_asset(PATH)
L("abp=%s" % (abp.get_name() if abp else None))

# collect graphs
graphs=[]
for prop in ('function_graphs','ubergraph_pages','macro_graphs','delegate_signature_graphs','animation_graphs'):
    try:
        g=abp.get_editor_property(prop)
        if g:
            graphs += list(g)
            L("prop %s -> %d graphs" % (prop, len(list(g))))
    except Exception as e:
        pass

L("total graphs: %d" % len(graphs))
for g in graphs:
    try:
        L("  graph: %s" % g.get_name())
    except Exception as e:
        L("  graph name err %s" % e)

# find LayeredBoneBlend node
target=None
for g in graphs:
    try:
        nodes=g.get_editor_property('nodes')
    except Exception as e:
        nodes=None
    if not nodes: continue
    for n in nodes:
        cn=n.get_class().get_name()
        if cn=="AnimGraphNode_LayeredBoneBlend":
            target=n
            L("FOUND LayeredBoneBlend in graph %s" % g.get_name())
            break
    if target: break

if not target:
    L("NODE NOT FOUND via graph walk")
else:
    try:
        animnode=target.get_editor_property('node')
        L("animnode type: %s" % type(animnode).__name__)
        ls=animnode.get_editor_property('layer_setup')
        L("layer_setup count: %d" % len(ls))
        # report current branch filters
        for i,ibp in enumerate(ls):
            bfs=ibp.get_editor_property('branch_filters')
            L("  setup[%d] branch_filters=%d" % (i, len(bfs)))
            for bf in bfs:
                L("     bone=%s depth=%s" % (bf.get_editor_property('bone_name'), bf.get_editor_property('blend_depth')))
        try:
            L("mesh_space_rotation_blend(before)=%s" % animnode.get_editor_property('mesh_space_rotation_blend'))
        except Exception as e:
            L("no msr prop: %s" % e)
        try:
            L("blend_mode(before)=%s" % animnode.get_editor_property('blend_mode'))
        except Exception as e:
            L("no blend_mode: %s" % e)

        # build new branch filter
        bf=unreal.BranchFilter()
        bf.set_editor_property('bone_name', unreal.Name('neck_01'))
        bf.set_editor_property('blend_depth', 1)
        new_ls=[]
        for i,ibp in enumerate(ls):
            ibp.set_editor_property('branch_filters', [bf])
            new_ls.append(ibp)
        animnode.set_editor_property('layer_setup', new_ls)
        try: animnode.set_editor_property('mesh_space_rotation_blend', True)
        except Exception as e: L("set msr err %s" % e)
        target.set_editor_property('node', animnode)
        try: target.reconstruct_node()
        except Exception as e: L("reconstruct err %s" % e)
        L("APPLIED branch filter neck_01 depth1 + mesh_space_rotation_blend")

        # verify
        an2=target.get_editor_property('node')
        ls2=an2.get_editor_property('layer_setup')
        for i,ibp in enumerate(ls2):
            bfs=ibp.get_editor_property('branch_filters')
            L("  VERIFY setup[%d] bf=%d" % (i,len(bfs)))
            for bf2 in bfs:
                L("     bone=%s depth=%s" % (bf2.get_editor_property('bone_name'), bf2.get_editor_property('blend_depth')))
    except Exception as e:
        L("EDIT ERR: %s" % e)

    # compile + save
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(abp)
        L("compiled")
        unreal.EditorLoadingAndSavingUtils.save_packages([abp.get_outermost()], False)
        L("saved")
    except Exception as e:
        L("compile/save err: %s" % e)

with open("C:/unrealEngine/avariika/Scripts/set_headmask_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
