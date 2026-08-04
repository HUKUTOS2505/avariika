import unreal

_OUT = []
def line(s=""):
    unreal.log("FIXCOMPAT| " + str(s)); _OUT.append(str(s))

FBL  = "/Game/FreeAnimationLibrary/Demo/Characters/Mannequins/Meshes/SK_Mannequin"
WORK = "/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin"
QSK  = "/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton"

def get_sk(p):
    return unreal.load_asset(p)

fbl  = get_sk(FBL)
work = get_sk(WORK)
qsk  = get_sk(QSK)
line("loaded: fbl=%s work=%s qsk=%s" % (fbl is not None, work is not None, qsk is not None))

# (owner_skeleton, skeleton_to_mark_compatible) pairs.
# owner.add_compatible_skeleton(other) => anims authored for `other` become usable on/through `owner`.
links = [
    ("Quantum<-FBL",  qsk,  fbl),   # body mesh (Quantum) can be driven by ABP on FBL  -> fixes body T-pose
    ("Quantum<-WORK", qsk,  work),  # body can also use WorkAnim-authored content (work montages)
    ("FBL<-WORK",     fbl,  work),  # ABP_Worker (FBL) can play Mobility clips + work anims (WorkAnim skeleton)
    ("WORK<-FBL",     work, fbl),   # symmetry
]

def compat_paths(sk):
    try:
        cs = sk.get_editor_property("compatible_skeletons")
        return set((c.get_path_name() if hasattr(c, "get_path_name") else str(c)) for c in (cs or []))
    except Exception as e:
        line("  read compat err: %s" % e); return set()

changed = set()
for label, owner, other in links:
    if owner is None or other is None:
        line("SKIP %s (missing skeleton)" % label); continue
    existing = compat_paths(owner)
    op = other.get_path_name()
    if op in existing:
        line("OK   %s already compatible" % label)
        continue
    try:
        owner.add_compatible_skeleton(other)
        line("ADD  %s -> added %s" % (label, op))
        changed.add(owner.get_path_name())
    except Exception as e:
        line("FAIL %s : %s" % (label, e))

# SAVE every skeleton we touched (CRITICAL: restart wipes unsaved compatible_skeletons)
for skpath in changed:
    try:
        ok = unreal.EditorAssetLibrary.save_asset(skpath, only_if_is_dirty=False)
        line("SAVE %s -> %s" % (skpath, ok))
    except Exception as e:
        line("SAVE FAIL %s : %s" % (skpath, e))

# verify
line("--- VERIFY ---")
for label, p in (("FBL", FBL), ("WORK", WORK), ("QSK", QSK)):
    sk = get_sk(p)
    line("%s compatible: %s" % (label, sorted(compat_paths(sk))))

with open("C:/unrealEngine/avariika/Scripts/fix_compat_out.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(_OUT))
