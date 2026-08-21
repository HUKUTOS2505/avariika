# Разведка анимации воркера: какой ABP, какие блендспейсы он реферит.
import unreal
ar = unreal.AssetRegistryHelpers.get_asset_registry()
lines = []

# 1) AnimClass у BP_AvaryoCharacter
bp = unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter")
anim_abp = None
if bp:
    try:
        gc = bp.generated_class()
        cdo = unreal.get_default_object(gc)
        # найти скелетный меш-компонент
        for comp_name in ("CharacterMesh0", "Mesh"):
            try:
                comp = cdo.get_editor_property(comp_name)
            except Exception:
                comp = None
            if comp:
                try:
                    ac = comp.get_editor_property("anim_class")
                    if ac:
                        anim_abp = ac.get_path_name()
                        lines.append("AnimClass via %s: %s" % (comp_name, anim_abp))
                        break
                except Exception as e:
                    lines.append("anim_class err: %s" % str(e)[:60])
    except Exception as e:
        lines.append("cdo err: %s" % str(e)[:80])
else:
    lines.append("BP_AvaryoCharacter не загружен")

# 2) кандидаты ABP + их блендспейс-зависимости
CANDS = [
    "/Game/_Packs/WorkAnimations/Demo/Mannequins/Animations/ABP_Manny",
    "/Game/Avariika/Anim/Locomotion/ABP_Worker",
]
for abp in CANDS:
    if not unreal.EditorAssetLibrary.does_asset_exist(abp):
        lines.append("%s — НЕТ" % abp); continue
    deps = ar.get_dependencies(unreal.Name(abp), unreal.AssetRegistryDependencyOptions(include_hard_package_references=True)) or []
    bss = []
    for d in deps:
        ds = str(d)
        ad = ar.get_assets_by_package_name(unreal.Name(ds))
        for a in ad:
            if "BlendSpace" in str(a.asset_class_path.asset_name):
                bss.append(ds.split("/")[-1])
    lines.append("%s -> blendspaces: %s" % (abp.split("/")[-1], bss))

with open(r"C:/unrealEngine/avariika/Scripts/worker_abp.txt","w",encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")
print("INSPECT_ABP done")
