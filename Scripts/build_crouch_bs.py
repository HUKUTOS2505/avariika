import unreal
_OUT=[]
def line(s=""):
    unreal.log("CRBLD| "+str(s)); _OUT.append(str(s))

M = "/Game/Avariika/Anim/Locomotion/Mobility/"
SRC = M+"BS_Loco_Mobility"
DST = M+"BS_CrouchWalk_Mobility"

# duplicate to inherit WorkAnim skeleton + 2D structure. Use return value (unsaved dup not loadable by path).
if unreal.EditorAssetLibrary.does_asset_exist(DST):
    unreal.EditorAssetLibrary.delete_asset(DST)  # clean any partial/bad duplicate
bs_dup = unreal.EditorAssetLibrary.duplicate_asset(SRC, DST)
line("duplicated -> %s" % (bs_dup is not None))

def clip(*names):
    for n in names:
        a = unreal.load_asset(M+n)
        if a: return a, n
    return None, names[0]

# (label, dir_deg, speed, candidate clip names)
plan = [
    ("idle",  0,    0, ["RT_MOB1_M1_Crouch_Idle_IP"]),
    ("F",     0,  150, ["RT_MOB1_M1_CrouchWalk_F_IP"]),
    ("FR",   45,  150, ["RT_MOB1_M1_CrouchWalk_FR_Loop_IP"]),
    ("R",    90,  150, ["RT_MOB1_M1_CrouchWalk_R_IP","RT_MOB1_M1_CrouchWalk_R_Loop_IP"]),
    ("BR",  135,  150, ["RT_MOB1_M1_CrouchWalk_BR_BkPd_Loop_IP"]),
    ("B",   180,  150, ["RT_MOB1_M1_CrouchWalk_B_IP"]),
    ("Bn", -180,  150, ["RT_MOB1_M1_CrouchWalk_B_IP"]),
    ("BL", -135,  150, ["RT_MOB1_M1_CrouchWalk_BL_BkPd_Loop_IP"]),
    ("L",   -90,  150, ["RT_MOB1_M1_CrouchWalk_L_IP","RT_MOB1_M1_CrouchWalk_L_Loop_IP"]),
    ("FL",  -45,  150, ["RT_MOB1_M1_CrouchWalk_FL_Loop_IP"]),
]

bs = unreal.load_asset(DST)
samples=[]
miss=0
for label, x, y, names in plan:
    c, used = clip(*names)
    if not c:
        line("MISSING %s: %s" % (label, names)); miss+=1; continue
    smp = unreal.BlendSample()
    smp.set_editor_property("animation", c)
    smp.set_editor_property("sample_value", unreal.Vector(float(x), float(y), 0.0))
    try: smp.set_editor_property("rate_scale", 1.0)
    except Exception: pass
    samples.append(smp)
    line("%s -> %s @ (%d,%d)" % (label, used, x, y))

bs.set_editor_property("sample_data", samples)
# triangulate (PostEditChange) + smoothing
bs.set_editor_property("sample_data", bs.get_editor_property("sample_data"))
try: bs.set_editor_property("target_weight_interpolation_speed_per_sec", 6.0)
except Exception: pass
ok = unreal.EditorLoadingAndSavingUtils.save_packages([bs.get_outermost()], False)
line("samples set=%d missing=%d  save=%s" % (len(samples), miss, ok))
line("skeleton: %s" % bs.get_editor_property("skeleton").get_path_name())

with open("C:/unrealEngine/avariika/Scripts/build_crouch_bs_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
