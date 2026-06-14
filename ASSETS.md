# Установленные ассеты «Аварийки»

Каталог ассет-паков и плагинов в проекте. Обновлён 2026-06-14 (после уборки).
Что из этого как встраивать (особенно погода) — см. [ASSET_ASSESSMENT.md](ASSET_ASSESSMENT.md).

> ⚠️ **ЛИЦЕНЗИИ:** большинство паков — БЕЗ лицензии (плейсхолдеры на разработку). План до релиза:
> 1) один пак на категорию (минимум лицензий); 2) hero-ассеты (предметы в руках) — свои в meshy;
> 3) в финальном билде каждый ассет = куплен ИЛИ свой/бесплатный (этот файл = список на докупку);
> 4) «чуть изменить чужую модель ради обхода» — НЕ защита; надёжно только купить или заменить.

**Статус git:** `git` — в репозитории · `локально` — только на диске (тяжёлое/gitignore, перекачиваемо).
**Используется:** ✅ подключено в игре · 📦 лежит готовое (под карты/будущее) · 🔧 инструмент/референс/источник.

---

## Плагины (`Plugins/`, локально)

| Плагин | Размер | Исп. | Назначение |
|---|---|---|---|
| **AudioToolkitPro** 1.2 | 75M | 🔧 | редактор-инструмент обработки звука (фикс `bUseUnity=false`) |
| Claudius | — | 🔧 | мост Claude↔редактор |
| meshy | 71M | 🔧 | генерация 3D-моделей |

**Движковые плагины (в `.uproject`):** OnlineSubsystem (+Null +Utils) ✅ кооп · JsonBlueprintUtilities ✅ (нужен Easy Options) · EnhancedInput (дефолт).
**EOS:** платный EOSCore **убран** → онлайн-кооп пойдёт через БЕСПЛАТНЫЙ движковый `OnlineSubsystemEOS` + `EOSShared` (включить + креды Epic при интернет-коопе; сейчас Null/LAN).

---

## Окружения / уровни

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **City Modular Hospital v.2** | `Hospital` | 3.1G | локально | 📦 | **флагман-больница** (есть меши под цели: щиток `SM_Electrical_Shield`, генератор `SM_Generator`) |
| **Hospital COMBO Prop Pack (VOL 1-6)** | `HospitalCombo` | 1.2G | git | 📦 | мебель/пропсы больницы |
| **Modular Haunted House** | `PostApocalypticHouse` | 4.8G | локально | 📦 | дом-прототип 0.1 + готовый хоррор-свет `LV_Horror_Light` |
| **Construction Pit** | `Construction_Pit` | 2.1G | локально | 📦 | стройплощадка (мультилокация — объект для бригады) |
| **Construction VOL.2** | `Construction_VOL2` | 536M | git | 📦 | стройка, том 2 |
| **Old Bench Pack** | `oldBenchPack` | 160M | локально | 📦 | скамейки (приёмная больницы) |

> Мультилокация: диспетчер шлёт бригаду на разные объекты (больница / дом / стройка). **Бэклог:** «Abandoned Factory» от автора City Hospital (тот же арт-стиль).

---

## Погода / VFX / атмосфера

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **Hyper Dynamic Weather & Sky** | `Hyper` | 6.1G | локально | 📦 | cherry-pick: ночное небо + грозовые NS_* (фреймворк не берём, ~6G мусора вычистить — см. ASSET_ASSESSMENT) |
| **Foggy Street** | `FoggyStreet` | 3.6G | локально | 📦 | туман/пар/дождь VFX (оставлен ради них) |
| **Decal Forge** | `Decal_Forge` | 2.8G | локально | 📦 | декали (грязь/кровь/потёки — хоррор) |
| **NiagaraExamples** | `NiagaraExamples` | 1.2G | локально | 🔧 | движковые примеры VFX (бесплатно, вернётся одной кнопкой) |
| **Fire_EXP Vol.01 (Free)** | `Fire_EXP_Vol01_Free` | 47M | локально | 📦 | огонь-VFX (бесплатный) |

---

## Системы и UI

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **Easy Options Menu** | `EasyOptionsMenu` | 40M | git | ✅ | экран настроек (нужен плагин JsonBlueprintUtilities) |
| **OG Main Menu System** | `OGMainMenu` | 462M | локально | 🔧 | как фронт НЕ используем (своё меню сделали) — склад шрифтов/иконок/звуков |
| **Footstep System** | `FootstepSystem` | 8.6M | git | 📦 | система шагов по поверхностям |
| **Resource Pack** | `ResourcePack` | 1.5G | git | 📦 | тайловые материалы/текстуры (общий ресурс) |

> Наше меню — собственное (`Content/Avariika/Maps/L_MainMenu` + C++ `AMenuHUD`).

---

## Звук

| Что | Где | git | Исп. | Назначение |
|---|---|---|---|---|
| **Наши вырезки** | `Content/Audio/SFX` | git | ✅ | подключены в игре: Explosion (взрыв), RepairDone (починка), RadioComm (диспетчер-рация), Ambient_Boiler (эмбиент-луп), Heartbeat (паника). _RadioBlip/FlashClick — заменены, не используются._ |
| **Survival SFX** | `Survival_SFX` | локально | ✅ | библиотека: Button_press (фонарь), Metal_item_pick_up (подбор), Anvil_hit (тычок мини-игры) — подключены; ещё много UI/craft/foley |

**Источники-библиотеки** (`RawAssets/звуки`, 52G локально, лицензии): **Ghosthack x Boom** (взрыв/рация/сердцебиение), **Abandoned Asylum / Sonomar** (эмбиент бойлерной), **Survival** (UI/craft/foley). Прочие в папке (Monster/Jumpscares/Earthquake/Rope/Dogs) — точечно по мере озвучки.

---

## Наше / шаблонное (не «паки»)

`Avariika` (наш код-контент: BP/карты/материалы) · `FirstPerson` (`Lvl_FirstPerson`) · `Characters` (манекены UE) · `Audio` (наши звуки) · `Input` `LevelPrototyping` `Movies` `Localization` `Splash` (шаблон/мелочь) · `__ExternalActors__`/`__ExternalObjects__` (OFPA) · `Collections`/`Developers` (пусто).

---

### Итого (≈29 ГБ Content)
- **В git** (клонируется): наш код+контент, HospitalCombo, Construction_VOL2, EasyOptionsMenu, FootstepSystem, ResourcePack, Content/Audio/SFX.
- **Локально** (тяжёлое, не в git): Hospital, PostApocalypticHouse, Hyper, FoggyStreet, Decal_Forge, Construction_Pit, NiagaraExamples, OGMainMenu, oldBenchPack, Fire_EXP, Survival_SFX + Plugins + RawAssets.
- **Удалено в уборку** (−~34 ГБ): AmericanDrive, LightHouse, Madrid_Street, Shoothouse, Backrooms_TheLobby, YI_Luggage, FencesVOL2, Adventure_Pack(Sarah), Citizens_Pack, 3 лишних больницы (Leartes/Horror/Abandoned), плагин EOSCore (платный).
- **Лицензий к докупке** (паки в билде): ~по одному на категорию — больница(City+Combo), дом, стройка, Hyper, Decal_Forge, FoggyStreet, ResourcePack, EasyOptions, AudioToolkitPro + звук-библиотеки (Ghosthack, Asylum, Survival). Бесплатные: Fire_EXP, NiagaraExamples, EOS (движок).
