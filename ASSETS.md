# Установленные ассеты «Аварийки»

Каталог всех ассет-паков и плагинов, поставленных в проект. Обновлён 2026-06-14.

> 🗑 **УБОРКА 2026-06-14** (освободить место + минимизировать лицензии). Удалено (не наш сеттинг / лишнее / делаем своё):
> AmericanDrive, LightHouse, Madrid_Street, Shoothouse, Backrooms_TheLobby, YI_Luggage, FencesVOL2, **Adventure_Pack (Sarah)**, **Citizens_Pack** — ~25 ГБ. Все локальные, перекачиваемы при надобности.
> **EOSCore (платный) УБРАН** → онлайн-кооп пойдёт через БЕСПЛАТНЫЙ движковый `OnlineSubsystemEOS` (включим при интернет-коопе). −3 ГБ, −1 лицензия.
> **Больницы → 1 флагман:** удалены Leartes / Horror_Hospital / AbandonedHospital (−6.7 ГБ, −3 лицензии). Остался **City Modular Hospital + Combo**. Foggy Street оставлен ради тумана/пара. Construction оставлен (мультилокация). «Abandoned Factory» (автор City Hospital) — в бэклог.
> **Звук:** `Survival_SFX` импортирован (распакованная копия; локально/gitignore). Источники-библиотеки звука: Ghosthack, Abandoned Asylum (Sonomar), Survival.

> Что из этого брать/как встраивать (особенно динамическая погода) — см. [ASSET_ASSESSMENT.md](ASSET_ASSESSMENT.md).

> ⚠️ **ЛИЦЕНЗИИ:** большинство паков — БЕЗ лицензии (плейсхолдеры на время разработки). План легализации до релиза:
> 1) минимизировать до ОДНОГО пака на категорию (окружение/пропсы/...), чтобы докупить минимум лицензий;
> 2) hero-ассеты (предметы в руках) — генерить свои в meshy (лицензия не нужна);
> 3) перед релизом каждый ассет в билде = куплен ИЛИ свой/бесплатный (этот файл = список на докупку);
> 4) «чуть изменить чужую модель ради обхода» — НЕ защита (производное произведение = то же нарушение); надёжно только купить или заменить на своё.

**Легенда статуса:**
- `git` — лежит в репозитории (можно клонировать).
- `локально` — только на диске (тяжёлый, есть файлы >100 МБ / не влезает в лимит пуша). В git НЕ держим до решения по LFS. См. `.gitignore` и WORKLOG.

**Легенда «используется»:**
- ✅ — уже подключено в игре.
- 📦 — импортировано, лежит готовое (под карты/будущее), пока не размещено.
- 🔧 — инструмент/референс (не идёт в билд игрой напрямую).

---

## Плагины (код) — папка `Plugins/`, все локальные (gitignore)

| Плагин | Размер | Исп. | Назначение |
|---|---|---|---|
| **EOSCore** v1.9.8.2 | 3.0 ГБ | ✅ | Epic Online Services — фундамент онлайн-коопа (за NAT). Включён в `.uproject`. Для интернет-игры нужны креды из Epic Dev Portal. |
| **AudioToolkitPro** 1.2 | 75 МБ | 🔧 | Редактор-инструмент обработки звука. Чинен под VS2026 (`bUseUnity=false`). |
| Claudius | — | 🔧 | Мост Claude↔редактор (наш рабочий инструмент). |
| meshy | 71 МБ | 🔧 | Генерация 3D-моделей (наш инструмент). |

**Движковые плагины (включены в `.uproject`):** OnlineSubsystem (+Null +Utils) ✅ кооп · JsonBlueprintUtilities ✅ (нужен Easy Options) · EnhancedInput (дефолт движка).

---

## Окружения / уровни — папка `Content/`

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **City Modular Hospital v.2** | `Hospital` | 3.1 ГБ | локально | 📦 | Флагман — модульный кит больницы (стены/лестницы/мебель/парковка/мусор). Карты Demonstration/Overview. |
| **Modern Hospital (Leartes)** | `Modern_Hospital_Leartes` | 4.1 ГБ | локально | 📦 | Больница 229 мешей (добор к флагману). |
| **Horror Hospital** | `Horror_Hospital` | 1.7 ГБ | git | 📦 | Окружение «страшной больницы». |
| **Abandoned Hospital** | `AbandonedHospital` | 911 МБ | git | 📦 | Заброшенная больница. |
| **Modular Haunted House** | `PostApocalypticHouse` | 4.8 ГБ | локально | 📦 | Модульный дом — под прототип 0.1. Карты LV_House/LV_Main/LV_Horror_Light. |
| **Backrooms: The Lobby** | `Backrooms_TheLobby` | 990 МБ | локально | 📦 | Бэкрумс-локация. |
| **Light House** | `LightHouse` | 3.3 ГБ | локально | 📦 | Маяк/окружение. |
| **Madrid Street** | `Madrid_Street` | 2.3 ГБ | локально | 📦 | Городская улица. |
| **Shoothouse** | `Shoothouse` | 2.1 ГБ | локально | 📦 | Помещения/шутхаус. |
| **American Drive** | `AmericanDrive` | 8.4 ГБ | локально | 📦 | Большое уличное окружение (тяжёлый). |
| **Construction Pit** | `Construction_Pit` | 2.1 ГБ | локально | 📦 | Стройплощадка/котлован. |
| **Construction VOL.2** | `Construction_VOL2` | 536 МБ | git | 📦 | Стройка, том 2. |

---

## Погода / VFX / атмосфера

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **Hyper Dynamic Weather & Sky** | `Hyper` | 6.1 ГБ | локально | 📦 | Динамическая погода/небо/день-ночь. ⚠️ компонентная (на GameState/PlayerController), ~5.7 ГБ внутри — демо-природа (можно выкинуть). Разбор интеграции — см. отчёт. |
| **Foggy Street** | `FoggyStreet` | 3.6 ГБ | локально | 📦 | VFX: туман/дождь/пар (атмосфера). |
| **Decal Forge** | `Decal_Forge` | 2.8 ГБ | локально | 📦 | Декали (грязь/потёки/следы). |

---

## Системы и UI

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **Easy Options Menu** | `EasyOptionsMenu` | 40 МБ | git | ✅ | Экран настроек (открывается из нашего меню). Нужен плагин JsonBlueprintUtilities (включён). |
| **OG Main Menu System** | `OGMainMenu` | 462 МБ | локально | 🔧 | Целый фреймворк меню — как фронт НЕ используем (свой меню сделали). Остаётся складом шрифтов/иконок/звуков. |
| **Footstep System** | `FootstepSystem` | 8.6 МБ | git | 📦 | Система шагов по поверхностям. |
| **Resource Pack** | `ResourcePack` | 1.5 ГБ | git | 📦 | Тайловые материалы/текстуры (общий ресурс). |

> Наше главное меню — собственное (`Content/Avariika/Maps/L_MainMenu` + C++ `AMenuHUD`), не из пака.

---

## Пропсы / декор / мебель

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **Hospital COMBO Prop Pack (VOL 1-6)** | `HospitalCombo` | 1.2 ГБ | git | 📦 | Вся мебель/пропсы больницы. |
| **YI Luggage** | `YI_Luggage` | 327 МБ | локально | 📦 | Чемоданы/багаж (пропсы). |
| **Old Bench Pack** | `oldBenchPack` | 160 МБ | локально | 📦 | Старые скамейки. |
| **Fences VOL.2** | `FencesVOL2` | 154 МБ | локально | 📦 | Заборы/ограждения. |

---

## Персонажи / NPC

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **Adventure Pack (механик Sarah)** | `Adventure_Pack` | 3.0 ГБ | локально | 📦 | Персонаж-рабочий Sarah (SK_Sarah) + ещё. Кандидат на модель монтёра. |
| **Citizens Pack** | `Citizens_Pack` | 768 МБ | локально | 📦 | NPC-горожане — под пациентов/персонал больницы. |

---

## Звук (библиотеки) — `RawAssets/звуки`, 52 ГБ / ~7700 WAV, локально

⚠️ **Не импортировать пачкой!** Это SFX-библиотеки-исходники. Берём ТОЧЕЧНО нужные WAV под конкретное событие игры (шаг, починка, газ, взрыв, скрип, рация, джампскейр) в `Content/Audio/` по мере озвучки. Лицензии — тот же план (плейсхолдеры → докупить/заменить до релиза).

| Библиотека | Подо что у нас |
|---|---|
| **Sonomar — Abandoned Asylum** | амбиент больницы/коридоров (профильное попадание) |
| **HAZMAT SUIT** | фоли бригады (костюм/дыхание/шуршание) |
| **Rope Stress and Creak** | скрипы (наша механика «фоновая жуть»/скрипы) |
| **Earthquake Sounds** | рокот/тряска — взрыв газа, обвал |
| **Survival Sound Effects** | починка/инвентарь/UI-щелчки |
| **Ghosthack x Boom (Bundle)** | хоррор/кинематик-стингеры, эфир рации |
| **SFXtools Jumpscares** | джампскейры (под монстра — ОТЛОЖЕН) |
| **Monster Sound FX Pack 1**, **SoundMorph — Monster Within** | голос/звуки монстра (под монстра — ОТЛОЖЕН) |
| **BOOM.Library DOGS** | собаки (если будет уличная сцена/двор) |
| **FootstepSoundComponent** | ⚠️ дублирует наш `FootstepSystem` — выбрать один |

## Наше / шаблонное (для полноты, не «установленные паки»)

`Avariika` (наш код-контент: BP, карты, материалы) · `FirstPerson` (наш игровой уровень `Lvl_FirstPerson`) · `Characters` (манекены UE) · `Input` `LevelPrototyping` `Movies` `Localization` `Splash` (шаблон/мелочь) · `__ExternalActors__`/`__ExternalObjects__` (OFPA уровней) · `Collections`/`Developers` (пусто).

---

### Итого
- **В git** (клонируется): наш код+контент, EasyOptionsMenu, FootstepSystem, ResourcePack, HospitalCombo, Horror_Hospital, AbandonedHospital, Construction_VOL2.
- **Локально** (тяжёлые, не в git): EOSCore, AudioToolkitPro, Hospital, Modern_Hospital_Leartes, PostApocalypticHouse, Hyper, OGMainMenu, Citizens_Pack, Adventure_Pack, AmericanDrive, FoggyStreet, LightHouse, Madrid_Street, Shoothouse, Construction_Pit, Decal_Forge, Backrooms_TheLobby, YI_Luggage, oldBenchPack, FencesVOL2.
- **Git-стратегия для локальных** (~60 ГБ): не решена (LFS / чанки). Пока — один рабочий компьютер.
