# -*- coding: utf-8 -*-
# Состояние дома в РЕЕСТРЕ (взялся ли rename): сколько ассетов числится по обоим путям.
import unreal, json, traceback
R = {"err": None}
try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    eal = unreal.EditorAssetLibrary
    for tag, path in [("module", "/Game/moduleHouse"), ("post", "/Game/PostApocalypticHouse")]:
        datas = ar.get_assets_by_path(path, recursive=True, include_only_on_disk_assets=False)
        redir = sum(1 for d in datas if str(getattr(d.asset_class_path, "asset_name", "")) == "ObjectRedirector")
        R[tag+"_total"] = len(datas)
        R[tag+"_redirectors"] = redir
        R[tag+"_real"] = len(datas) - redir
        R[tag+"_dir_exists"] = eal.does_directory_exist(path)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_house_state.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("CHECK_HOUSE_STATE %s" % json.dumps(R, default=str))
