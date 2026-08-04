import unreal
_OUT=[]
def line(s=""):
    unreal.log("DIAG3| "+str(s)); _OUT.append(str(s))

for p in ("/Game/Avariika/Anim/Locomotion/BS_Locomotion",
          "/Game/Avariika/Anim/Locomotion/BS_Crouch",
          "/Game/Avariika/Anim/Locomotion/Mobility/BS_Loco_Mobility"):
    a=unreal.load_asset(p)
    line("--- %s ---" % p)
    if not a:
        line("  (load fail)"); continue
    line("  class: %s" % a.get_class().get_name())
    try:
        sk=a.get_editor_property("skeleton")
        line("  skeleton: %s" % (sk.get_path_name() if sk else None))
    except Exception as e:
        line("  skel err: %s" % e)
    try:
        sd=a.get_editor_property("sample_data")
        line("  samples: %s" % (len(sd) if sd is not None else None))
    except Exception as e:
        line("  samples err: %s" % e)

with open("C:/unrealEngine/avariika/Scripts/diag_bs_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
