import unreal
_OUT=[]
def L(s): unreal.log("LBB| "+str(s)); _OUT.append(str(s))

bp = unreal.load_asset("/Game/Avariika/Anim/Locomotion/ABP_Worker")
graphs = unreal.BlueprintEditorLibrary.get_blueprint_animation_graphs(bp) if hasattr(unreal.BlueprintEditorLibrary,'get_blueprint_animation_graphs') else None
L("bp=%s" % (bp.get_name() if bp else None))

# Walk all nodes in the BP graphs to find the LayeredBoneBlend node
found=False
try:
    import unreal as u
    # Use the K2 graph traversal
    for g in unreal.BlueprintEditorLibrary.__dict__:
        pass
except Exception as e:
    L("e0 %s" % e)

# Generic: iterate the AnimBlueprint's generated class default object anim node properties is hard.
# Instead inspect via the graph nodes through the editor utility.
try:
    nodes = unreal.AnimationBlueprintLibrary
except Exception as e:
    L("no ABL")

# Try the EdGraph approach
def find_lbb(obj):
    pass

# Direct: load all objects in package, find AnimGraphNode_LayeredBoneBlend
pkg = bp.get_outermost()
for o in unreal.find_objects(unreal.Object, None) if False else []:
    pass

# Use get_objects_with_outer-like: iterate inner objects
objs = []
try:
    objs = unreal.EditorAssetLibrary  # placeholder
except Exception:
    pass

# Best effort: use the K2Node listing via Blueprint API isn't exposed; report node class properties by reflection
L("--- reflection on LayeredBoneBlend node type ---")
try:
    cls = unreal.AnimGraphNode_LayeredBoneBlend
    L("has type: AnimGraphNode_LayeredBoneBlend")
except Exception as e:
    L("no node type: %s" % e)

with open("C:/unrealEngine/avariika/Scripts/inspect_layered_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
