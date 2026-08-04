# Свап клипов в BS_Locomotion (что ABP_Worker уже использует) на Mobility-mocap по тем же координатам.
# Граф ABP не трогается. Итог -> Scripts/swap_loco_result.txt
import unreal
eal = unreal.EditorAssetLibrary
M = "/Game/Avariika/Anim/Locomotion/Mobility/"
lines = []
bs = unreal.load_asset("/Game/Avariika/Anim/Locomotion/BS_Locomotion.BS_Locomotion")

def clip(n):
    return unreal.load_asset(M + "RT_MOB1_M1_" + n + ".RT_MOB1_M1_" + n)

# (Direction X, Speed Y) -> Mobility-клип
MAP = {
    (0,0): "Stand_Relaxed_Idle_IP",
    (0,150): "Walk_F_IP", (180,150): "Walk_B_IP", (-180,150): "Walk_B_IP",
    (-90,150): "Walk_L_IP", (90,150): "Walk_R_IP",
    (0,400): "Jog_F_IP", (180,400): "Jog_B_IP", (-180,400): "Jog_B_IP",
    (-90,400): "Jog_L_IP", (90,400): "Jog_R_IP",
}
new_samples = []
for (x, y), nm in MAP.items():
    a = clip(nm)
    if not a:
        lines.append("НЕТ клипа " + nm); continue
    s = unreal.BlendSample()
    s.set_editor_property("animation", a)
    s.set_editor_property("sample_value", unreal.Vector(x, y, 0))
    new_samples.append(s)
lines.append("новых сэмплов: %d" % len(new_samples))
try:
    bs.set_editor_property("sample_data", new_samples)
    lines.append("sample_data заменён")
except Exception as e:
    lines.append("set err: " + str(e)[:100])
eal.save_loaded_asset(bs, False)
# проверка
sd = bs.get_editor_property("sample_data")
lines.append("итог %d сэмплов:" % len(sd))
for s in sd:
    a = s.get_editor_property("animation"); v = s.get_editor_property("sample_value")
    lines.append("  (%.0f,%.0f) <- %s" % (v.x, v.y, a.get_name() if a else "None"))
with open(r"C:/unrealEngine/avariika/Scripts/swap_loco_result.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("SWAP done")
