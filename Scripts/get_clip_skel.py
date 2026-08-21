import unreal
a = unreal.load_asset("/Game/Avariika/Anim/Locomotion/Mobility/RT_MOB1_M1_Walk_F_IP.RT_MOB1_M1_Walk_F_IP")
sk = a.get_editor_property("skeleton") if a else None
res = "skeleton path: %s" % (sk.get_path_name() if sk else "-")
unreal.log(res)
with open(r"C:/unrealEngine/avariika/Scripts/clip_skel.txt","w",encoding="utf-8") as f:
    f.write(res + "\n")
print(res)
