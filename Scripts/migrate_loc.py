# -*- coding: utf-8 -*-
# Миграция FAvLoc::T(TEXT("ru"),TEXT("en")) -> FAvLoc::S(TEXT("key")) в AvaryoHUD.cpp.
# Извлекает пары прямо из кода (нет ручной транскрипции), присваивает стабильные ключи ui_NNN
# (dedupe по ru), переписывает .cpp, дописывает ключи+ru в CSV, пишет EN-карту в JSON.
import re, json, os

SRC = "Source/Avaryo/Private/UI/AvaryoHUD.cpp"
CSV = "Content/Avariika/Localization/AvariikaUI.csv"
ENJSON = "Scripts/loc_en.json"

# EN-переводы строк меню (уже мигрированы ранее) — чтобы loc_en.json покрывал и меню.
MENU_EN = {
    "menu_title": "AVARIYKA", "menu_subtitle": "emergency repair crew",
    "menu_host": "Host Game", "menu_find": "Find Game", "menu_settings": "Settings",
    "menu_quit": "Quit", "menu_browse_header": "Find Network Games",
    "menu_searching": "Searching...", "menu_no_games": "No games found",
    "menu_game_prefix": "Game ", "menu_join_suffix": "   [Join]",
    "menu_refresh": "Refresh", "menu_back": "Back", "menu_hint": "v0.1 — co-op LAN/EOS",
}

pat = re.compile(r'FAvLoc::T\(\s*TEXT\("([^"]*)"\)\s*,\s*TEXT\("([^"]*)"\)\s*\)')

with open(SRC, encoding="utf-8") as f:
    code = f.read()

ru2key = {}
order = []   # (key, ru, en) в порядке первого появления
n = [0]

def repl(m):
    ru, en = m.group(1), m.group(2)
    if ru not in ru2key:
        n[0] += 1
        ru2key[ru] = "ui_%03d" % n[0]
        order.append((ru2key[ru], ru, en))
    return 'FAvLoc::S(TEXT("%s"))' % ru2key[ru]

newcode, cnt = pat.subn(repl, code)
with open(SRC, "w", encoding="utf-8") as f:
    f.write(newcode)

def csvval(s):
    if ('"' in s) or (',' in s) or (s != s.strip()):
        return '"' + s.replace('"', '""') + '"'
    return s

with open(CSV, "a", encoding="utf-8") as f:
    for k, ru, en in order:
        f.write("%s,%s\n" % (k, csvval(ru)))

en_map = {k: en for k, ru, en in order}
en_map.update(MENU_EN)
with open(ENJSON, "w", encoding="utf-8") as f:
    json.dump(en_map, f, ensure_ascii=False, indent=1)

print("replaced %d call(s); %d unique keys; en_map total %d" % (cnt, len(order), len(en_map)))
print("first 3:", order[:3])
