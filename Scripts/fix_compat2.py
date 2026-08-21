import unreal
_OUT=[]
def line(s=""):
    unreal.log("FIX2| "+str(s)); _OUT.append(str(s))

FBL  = unreal.load_asset("/Game/FreeAnimationLibrary/Demo/Characters/Mannequins/Meshes/SK_Mannequin")
WORK = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin")
QSK  = unreal.load_asset("/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton")

def compat_names(sk):
    try:
        return set(c.get_path_name() for c in (sk.get_editor_property("compatible_skeletons") or []))
    except Exception:
        return set()

# belt-and-suspenders: make every pair mutually compatible (cover both check directions)
pairs = [("FBL+Q", FBL, QSK), ("WORK+Q", WORK, QSK), ("Q+FBL", QSK, FBL), ("Q+WORK", QSK, WORK),
         ("FBL+WORK", FBL, WORK), ("WORK+FBL", WORK, FBL)]
touched=set()
for label, owner, other in pairs:
    if not owner or not other:
        line("skip %s"%label); continue
    if other.get_path_name() in compat_names(owner):
        line("ok %s"%label); continue
    try:
        owner.add_compatible_skeleton(other); touched.add(owner.get_path_name())
        line("add %s"%label)
    except Exception as e:
        line("fail %s: %s"%(label,e))

for p in touched:
    try: line("save skel %s -> %s" % (p, unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)))
    except Exception as e: line("save fail %s: %s"%(p,e))

# recompile the anim BP and the character BP so the mesh<->anim binding re-resolves
for bp_path in ("/Game/Avariika/Anim/Locomotion/ABP_Worker",
                "/Game/Avariika/Blueprints/BP_AvaryoCharacter"):
    bp = unreal.load_asset(bp_path)
    if not bp:
        line("BP load fail %s"%bp_path); continue
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        line("compiled %s"%bp_path)
    except Exception as e:
        line("compile err %s: %s"%(bp_path,e))
    try:
        line("save bp %s -> %s"%(bp_path, unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)))
    except Exception as e:
        line("save bp fail %s: %s"%(bp_path,e))

line("--- final compat ---")
for label, sk in (("FBL",FBL),("WORK",WORK),("QSK",QSK)):
    line("%s: %s" % (label, sorted(c.split('.')[-1]+"@"+c.split('/')[2] for c in compat_names(sk))))

with open("C:/unrealEngine/avariika/Scripts/fix_compat2_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
