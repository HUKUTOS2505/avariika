# Полный скан диска C: — что можно удалить

> Снято автоном 2026-06-23. Один физический SSD **Samsung 980 1TB**, два раздела:
> **C: 749 ГБ (своб. ~386)** · **D: 181 ГБ (своб. ~52)**. Игра-проект лежит на D:.
> Цель по плану переезда: **C: = Windows + дев-проект**, **D: = установленные игры**.

---

## 🎯 БЫСТРЫЕ ПОБЕДЫ (по убыванию выгоды)

| Что | Размер | Действие | Безопасность |
|---|---|---:|---|
| **`C:\Games\Resident Evil 7`** | **37.8 ГБ** | переместить на **D:** (игры → D:) | ✅ отдельная установка, переносится папкой |
| **Steam-игра `Marathon`** | **28.4 ГБ** | перенести библиотеку на D: или удалить, если не играешь | ✅ через Steam → «Переместить» |
| **`C:\_avariika_backup`** | **25.7 ГБ** | удалить — мой одноразовый git-бандл, проект цел | ✅ `rmdir /s /q C:\_avariika_backup` |
| **`C:\ProgramData\Epic\EpicGamesLauncher` (VaultCache)** | **16.9 ГБ** | кэш скачанного из Fab/Marketplace — чистить ПОСЛЕ установки нужного | ⚠️ сюда же падают текущие загрузки плагинов — чистить, когда докачаешь |
| **`Desktop\data.bin`** | **12.5 ГБ** | ❓ **ОПОЗНАЙ САМ** — один файл на рабочем столе. Если хлам — разом 12.5 ГБ | ❓ только ты знаешь, что это |
| **`AppData\Local\UnrealEngine`** | **10.9 ГБ** | глобальный кэш UE (общий DDC/логи/автосейвы) — отстроится | ✅ чистить при закрытом редакторе |

**Итого быстрых побед: ~130 ГБ** (из них ~66 ГБ — перенос игр на D:, ~64 ГБ — удаление хлама/кэша).

---

## 📊 ВЕРХНИЙ УРОВЕНЬ C:\

| Папка | ГБ | Комментарий |
|---|---:|---|
| `Program Files` | 57.6 | см. ниже (движок UE + софт) |
| `Windows` | 44.8 | система — **не трогать** |
| `Program Files (x86)` | 43.0 | Steam (29.9, вкл. игру Marathon) + VS + прочее |
| `Games` | 37.8 | **RE7 → на D:** |
| `ProgramData` | 28.2 | Epic VaultCache 16.9 + MS 6 + NVIDIA 3.7 |
| `_avariika_backup` | 25.7 | **мой git-бэкап → удалить** |
| `Users\admin` | ~55 | AppData 37.9 + Desktop 13.8 + прочее |
| `Android` 0.4 · `python` 0.34 · `rabota` 0.3 · `sounpad` 0.18 | <1 | по мелочи |

---

## 📦 Program Files (57.6 ГБ)

| | ГБ | |
|---|---:|---|
| `Epic Games` (движок **UE_5.7**) | 28.3 | **нужен — не трогать** |
| `WindowsApps` | 7.7 | UWP-приложения (через «Параметры» → Приложения) |
| `Microsoft Visual Studio` | 7.0 | IDE — нужен для сборки |
| `Microsoft Office` | 3.2 | софт |
| `JetBrains` (Rider) | 2.8 | IDE |
| `ZennoLab` | 1.85 | автоматизация — твоё, реши сам |
| NVIDIA 0.85 · dotnet 0.78 · Yandex 0.55 · Unity Hub 0.49 · Google 0.46 · Git 0.40 · Wargaming 0.37 · Java 0.36 | | софт |

## 📦 Program Files (x86) (43.0 ГБ)

| | ГБ | |
|---|---:|---|
| `Steam` | 29.9 | клиент + **игра Marathon 28.4 → на D:** |
| `Microsoft Visual Studio` | 3.65 | IDE |
| `Microsoft` | 3.18 | софт |
| `Windows Kits` | 2.12 | SDK (нужен для сборки C++) |
| Epic Games 0.79 · Lesta 0.29 · прочее | | по мелочи |

## 📦 ProgramData (28.2 ГБ)

| | ГБ | |
|---|---:|---|
| `Epic\EpicGamesLauncher` (**VaultCache**) | 16.9 | **кэш скачанного с Fab/Marketplace** — чистить после установки нужного |
| `Microsoft` | 6.0 | Defender/системное |
| `NVIDIA Corporation` | 3.7 | драйверы/кэш шейдеров |
| `Package Cache` 0.85 · прочее | | инсталляторы (для починки софта — лучше не трогать) |

## 📦 Users\admin\AppData\Local (37.9 ГБ всего)

| | ГБ | Чистка |
|---|---:|---|
| `UnrealEngine` | 10.9 | ✅ кэш UE (DDC) — снести при закрытом редакторе |
| `JetBrains` | 3.9 | ⚠️ кэши/индексы Rider — чистятся, но переиндексирует |
| `Programs` | 3.8 | установленные per-user программы — не мусор |
| `Microsoft` | 1.75 | кэши MS |
| `npm-cache` | 1.21 | ✅ `npm cache clean --force` |
| `Mozilla` | 1.04 | ✅ кэш браузера |
| `NVIDIA` | 1.04 | кэш шейдеров (регенерируется) |
| `Discord` | 0.99 | ✅ кэш Discord |
| `Directive8020` | 0.95 | игра? — реши сам |
| `CrashDumps` | 0.46 | ✅ дампы сбоев — удалить |
| `Steam`/`Yandex`/`Postman`/`Packages` | ~2.5 | данные приложений |

**Roaming:** `Telegram Desktop` 2.5 ГБ (✅ кэш медиа — чистить в ТГ), Mozilla 0.85, npm 0.66, UnityHub 0.36, Lesta 0.34.

**Desktop (13.8 ГБ):** `data.bin` **12.5** (❓опознай), `Unigine_Superposition-1.1.exe` 1.25 (✅ бенчмарк-инсталлятор, удалить).

---

## 🔝 Самые крупные ФАЙЛЫ на C: (>0.8 ГБ)

```
20.13  Games\Resident Evil 7 Biohazard\re_chunk_000.pak      ─┐
 6.52  ...RE7\530611\re_dlc_000.pak                            │ RE7 целиком ~37.8 ГБ
 3.90  ...RE7\564190\re_dlc_000.pak                            │ → на D:
 2.44  ...RE7\re_chunk_000.pak.patch_001.pak                   │
 1.92  ...RE7\530610\re_dlc_000.pak  · 1.15  ...529930...     ─┘
12.51  Desktop\data.bin                                        ❓ ОПОЗНАЙ
 ~1.9×6  Steam\...\Marathon\packages\*.pkg                     Marathon ~28.4 ГБ → на D:
 1.69  Program Files\Epic Games\UE_5.7\...\Compressed.ddp      движковый DDC (не трогать вручную)
 1.36  ProgramData\Epic\...\VaultCache\GameAnimationSample...  кэш Fab-пака
 1.25  Desktop\Unigine_Superposition-1.1.exe                   ✅ удалить
```

---

## ⛔ НЕ ТРОГАТЬ
- `C:\Windows` (кроме явных кэшей), `C:\Windows\Installer` (1.64 — нужен для починки/удаления программ).
- `pagefile.sys` (38 ГБ) — файл подкачки; `hiberfil.sys` (12.4 ГБ) — гибернация (можно отключить `powercfg /h off`, если не пользуешься — вернёт 12 ГБ).
- `Program Files\Epic Games\UE_5.7` — движок.
- Visual Studio / Windows Kits — нужны для C++-сборки.

---

## ✅ ПЛАН ДЕЙСТВИЙ (рекомендуемый порядок)
1. **Опознать `Desktop\data.bin`** (12.5 ГБ) — если хлам, удалить.
2. **Удалить** `C:\_avariika_backup` (25.7), `Desktop\Unigine_...exe` (1.25), `AppData\Local\CrashDumps` (0.46).
3. **Перенести игры на D:**: RE7 (папкой), Marathon (через Steam «Переместить установку»). −66 ГБ с C:.
4. **Почистить кэши** (когда докачаешь плагины): VaultCache Epic, `AppData\Local\UnrealEngine`, npm/Discord/Telegram/Mozilla. До ~40 ГБ.
5. После — на C: будет своб. **~250+ ГБ** под Windows + дев-проект (его перенесём отдельно).
