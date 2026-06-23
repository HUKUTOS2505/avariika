import unreal
out=[]
for n in ["Movement/Walk_stone","Movement/Jog_stone","Movement/Run_stone","Movement/Walk_wood"]:
    p="/Game/Survival_SFX/%s"%n
    sw=unreal.load_asset(p)
    if sw:
        try: d=sw.get_editor_property("duration")
        except Exception: d=-1
        try: lp=sw.get_editor_property("looping")
        except Exception: lp="?"
        out.append("%-22s dur=%.2fs looping=%s"%(n.split('/')[-1], d, lp))
    else: out.append("%s NOTFOUND"%n)
open("C:/unrealEngine/avariika/Saved/snd_dur.txt","w").write("\n".join(out))
