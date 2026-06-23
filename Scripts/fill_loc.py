# -*- coding: utf-8 -*-
# Заполняет переводы в .archive локализации (UTF-16 JSON) по карте key->text для namespace.
# Использование: редактируем TRANS ниже, запускаем; затем GatherText перекомпилит .locres.
import json, sys, os

ARCH = "Content/Localization/Game/en/Game.archive"
NS = "AvariikaUI"

# key -> английский перевод (RU = источник в CSV; тут только EN)
TRANS = {
    "menu_title": "AVARIYKA",
    "menu_subtitle": "emergency repair crew",
    "menu_host": "Host Game",
    "menu_find": "Find Game",
    "menu_settings": "Settings",
    "menu_quit": "Quit",
    "menu_browse_header": "Find Network Games",
    "menu_searching": "Searching...",
    "menu_no_games": "No games found",
    "menu_game_prefix": "Game ",
    "menu_join_suffix": "   [Join]",
    "menu_refresh": "Refresh",
    "menu_back": "Back",
    "menu_hint": "v0.1 — co-op LAN/EOS",
}

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
