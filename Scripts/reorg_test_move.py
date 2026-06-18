# -*- coding: utf-8 -*-
# Тест механизма переезда: двигаем ОДИН ненужный пак в /Game/_Packs/ и проверяем результат.
# Не трогает загруженный уровень. Только чтение редактора → перенос ассетов.
import unreal, json, traceback
R = {"err": None}
SRC = "/Game/oldBenchPack"
DST = "/Game/_Packs/oldBenchPack"
try:
    eal = unreal.EditorAssetLibrary
    R["src_exists_before"] = eal.does_directory_exist(SRC)
    R["assets_before"] = len(eal.list_assets(SRC, recursive=True, include_folder=False)) if R["src_exists_before"] else 0
    # перенос каталога (редактор сам чинит ссылки + оставляет редиректоры)
    ok = eal.rename_directory(SRC, DST)
    R["rename_ok"] = bool(ok)
    R["dst_exists_after"] = eal.does_directory_exist(DST)
    R["src_exists_after"] = eal.does_directory_exist(SRC)
    R["assets_after"] = len(eal.list_assets(DST, recursive=True, include_folder=False)) if R["dst_exists_after"] else 0
    # сохранить перенесённые ассеты
    if R["dst_exists_after"]:
        eal.save_directory(DST, only_if_is_dirty=False, recursive=True)
        R["saved"] = True
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_reorg_test.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("REORG_TEST %s" % json.dumps(R, default=str))
