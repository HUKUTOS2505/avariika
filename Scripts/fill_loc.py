# -*- coding: utf-8 -*-
# Заполняет переводы в .archive локализации (UTF-16 JSON) по карте key->text для namespace.
# Источник EN-переводов — Scripts/loc_en.json (генерится migrate_loc.py из вторых аргументов
# FAvLoc::T(ru,en); включает меню + HUD). Использование: запустить ПОСЛЕ GatherText
# (он добавил ключи в архивы), затем GatherText ещё раз — перекомпилит .locres с этими переводами.
import json, sys, os

ARCH = "Content/Localization/Game/en/Game.archive"
ENJSON = "Scripts/loc_en.json"
NS = "AvariikaUI"

# key -> английский перевод (RU = источник в CSV). Грузим из loc_en.json.
with open(ENJSON, encoding="utf-8") as f:
    TRANS = json.load(f)

def fill_children(children, trans, stats):
    for e in children:
        k = e.get("Key")
        if k in trans:
            e.setdefault("Translation", {})["Text"] = trans[k]
            stats[0] += 1

def main():
    with open(ARCH, "r", encoding="utf-16") as f:
        data = json.load(f)
    stats = [0]
    # целевой namespace — среди Subnamespaces
    for sub in data.get("Subnamespaces", []):
        if sub.get("Namespace") == NS:
            fill_children(sub.get("Children", []), TRANS, stats)
    # на случай, если namespace окажется корневым
    if data.get("Namespace") == NS:
        fill_children(data.get("Children", []), TRANS, stats)
    with open(ARCH, "w", encoding="utf-16") as f:
        json.dump(data, f, ensure_ascii=False, indent="\t")
    print(f"filled {stats[0]} translations into {ARCH}")

if __name__ == "__main__":
    main()
