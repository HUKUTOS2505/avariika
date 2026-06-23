# -*- coding: utf-8 -*-
# Переименовываем «дом»: /Game/PostApocalypticHouse -> /Game/moduleHouse (WP, ~5ГБ, краш-риск).
# Пак самодостаточный (игрой не используется) → внешних ссылок нет, чинятся только внутренние.
import unreal, json, traceback
R = {"err": None}
eal = unreal.EditorAssetLibrary
SRC = "/Game/PostApocalypticHouse"; DST = "/Game/moduleHouse"
try:
    R["src_before"] = eal.does_directory_exist(SRC)
    R["dst_before"] = eal.does_directory_exist(DST)
    if R["src_before"]:
        ok = eal.rename_directory(SRC, DST)
        R["rename_ok"] = bool(ok)
    R["dst_after"] = eal.does_directory_exist(DST)
    R["src_after"] = eal.does_directory_exist(SRC)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_rename_house.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, ensure_ascii=False)
unreal.log("RENAME_HOUSE done")
