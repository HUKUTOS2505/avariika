# Инвентаризация анимаций: все AnimSequence/Montage/BlendSpace/AnimBP по /Game-папке и скелету.
# Итог -> Scripts/anim_inventory.txt
import unreal

areg = unreal.AssetRegistryHelpers.get_asset_registry()

CLASSES = ["AnimSequence", "AnimMontage", "BlendSpace", "BlendSpace1D", "AimOffsetBlendSpace", "AnimBlueprint", "AnimComposite"]

# Собрать ассеты по классам
all_assets = []
for cls in CLASSES:
    f = unreal.ARFilter(class_names=[cls], recursive_classes=True)
    try:
        ad = areg.get_assets(f)
    except Exception:
        ad = []
    for a in ad:
        all_assets.append((cls, a))

def top_folder(path):
    # /Game/Foo/Bar -> Foo
    p = str(path)
    parts = p.split("/")
    return parts[2] if len(parts) > 2 else "?"

def skel_of(a):
    try:
        s = a.get_tag_value("Skeleton")
        if s:
            s = str(s).split(".")[-1]
            return s
    except Exception:
        pass
    return "-"

# Группировки
by_folder = {}        # folder -> count
by_folder_skel = {}   # folder -> {skel: count}
by_skel = {}          # skel -> count
class_counts = {}

for cls, a in all_assets:
    pkg = str(a.package_name)
    fold = top_folder(pkg)
    sk = skel_of(a) if cls in ("AnimSequence","AnimMontage","BlendSpace","BlendSpace1D","AimOffsetBlendSpace","AnimComposite") else "(bp)"
    by_folder[fold] = by_folder.get(fold, 0) + 1
    by_folder_skel.setdefault(fold, {})
    by_folder_skel[fold][sk] = by_folder_skel[fold].get(sk, 0) + 1
    by_skel[sk] = by_skel.get(sk, 0) + 1
    class_counts[cls] = class_counts.get(cls, 0) + 1

lines = []
lines.append("=== АНИМ-АССЕТЫ: всего %d ===" % len(all_assets))
lines.append("по классам: " + ", ".join("%s=%d" % (k, v) for k, v in sorted(class_counts.items())))
lines.append("")
lines.append("=== ПО СКЕЛЕТАМ ===")
for sk, n in sorted(by_skel.items(), key=lambda x: -x[1]):
    lines.append("  %-45s %d" % (sk, n))
lines.append("")
lines.append("=== ПО ПАПКАМ /Game/<folder> (скелеты внутри) ===")
for fold, n in sorted(by_folder.items(), key=lambda x: -x[1]):
    sk_str = ", ".join("%s:%d" % (s, c) for s, c in sorted(by_folder_skel[fold].items(), key=lambda x: -x[1]))
    lines.append("  %-32s total=%-4d  [%s]" % (fold, n, sk_str))

text = "\n".join(lines)
for ln in lines:
    unreal.log(ln)
with open(r"C:/unrealEngine/avariika/Scripts/anim_inventory.txt", "w", encoding="utf-8") as f:
    f.write(text + "\n")
print("ANIM_INVENTORY_DONE total=%d" % len(all_assets))
