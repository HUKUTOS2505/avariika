# -*- coding: utf-8 -*-
# Точечный тест: почему rename_asset «молча» не двигает остаточные меши. Берём 1 ассет, ловим bool+причину.
import unreal, json, traceback
R = {"err": None, "assets_in_folder": [], "tries": []}
eal = unreal.EditorAssetLibrary
folder = "/Game/KnockedDown"
try:
    assets = list(eal.list_assets(folder, recursive=True, include_folder=False))
    R["assets_in_folder"] = [str(a) for a in assets]
    for a in assets:
        seg = a.rsplit("/", 1); nm = seg[1].split(".")[0]
        src = seg[0] + "/" + nm
        dst = src.replace("/Game/KnockedDown/", "/Game/_Packs/KnockedDown/", 1)
        info = {"src": src, "dst": dst}
        info["src_exists_before"] = eal.does_asset_exist(src)
        info["dst_exists_before"] = eal.does_asset_exist(dst)
        try:
            ret = eal.rename_asset(src, dst)
            info["rename_returned"] = ret
        except Exception as e:
            info["rename_exc"] = str(e)
        info["src_exists_after"] = eal.does_asset_exist(src)
        info["dst_exists_after"] = eal.does_asset_exist(dst)
        # кто ссылается на src (почему держится)
        try:
            ar = unreal.AssetRegistryHelpers.get_asset_registry()
            refs = ar.get_referencers(unreal.Name(src), unreal.AssetRegistryDependencyOptions(
                include_hard_package_references=True, include_soft_package_references=True))
            info["referencers"] = [str(x) for x in (refs or [])][:8]
        except Exception as e:
            info["ref_err"] = str(e)
        R["tries"].append(info)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_test_rename.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("TEST_ONE_RENAME done")
