# Asset, Pack, and Optimization Audits

This consolidated document preserves the source material listed below. Originals are archived under Docs/Archive/Originals/.

## Sources
- `ASSETS.md`
- `ASSET_ASSESSMENT.md`
- `PACKS_REFERENCE.md`
- `PLUGINS_REFERENCE.md`
- `RAWASSETS_TRIAGE.md`
- `RAWASSETS_TRIAGE_TABLE.md`
- `CRAWL_FINDINGS.md`
- `OPTIMIZATION_REPORT.md`
- `DISK_C_SCAN.md`
- `FINAL_REPORT.txt`

---

## Source: ASSETS.md

# Установленные ассеты «Аварийки»

Каталог ассет-паков и плагинов в проекте. Обновлён 2026-06-14 (после уборки).
Что из этого как встраивать (особенно погода) — см. [ASSET_ASSESSMENT.md](../Archive/Originals/Audits/ASSET_ASSESSMENT.md).

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
| **Hyper Dynamic Weather & Sky** (обрезан) | `Hyper` | **106M** | локально | 📦 | оставлены ТОЛЬКО погодные Niagara (NS_Rain/ThunderStorm/Blizzard/…) + ночное небо (SkySphere/Moon/Stars/Clouds) + их зависимости. ~6 ГБ природного балласта вычищено (Environments/Tileable/Locomotion/фреймворк). |
| **NiagaraExamples** | `NiagaraExamples` | 1.2G | локально | 📦 | движковые примеры VFX (бесплатно): взрывы (FX_Explosions), огонь, дым, искры, туман |
| **Fire_EXP Vol.01 (Free)** | `Fire_EXP_Vol01_Free` | 47M | локально | 📦 | огонь (циклы)/взрыв/искры/обломки (бесплатный) |

**VFX-покрытие (вывод 2026-06-14): на разработку/прототип бесплатных ХВАТАЕТ.**
- взрыв газа → NiagaraExamples FX_Explosions / Fire_EXP NS_Sub_EXP_* · огонь → Fire_EXP циклы · дым/облако → NiagaraExamples · искры/замыкание → NiagaraExamples+Fire_EXP · обломки → Fire_EXP · дождь/гроза → Hyper. Foam огнетушителя — белой частицей.
- **Докупить ПОТОМ (опц., для лоска):** 1) Niagara Realistic Starter VFX Pack **2** (огонь/дым/искры/взрыв, Niagara) — лучший один пак; 2) All Explosion Pack (1 том) — кинематографичный бабах, если захочется; 3) Blood Starter VFX — только если решим видимую кровь.
- **НЕ покупать:** Niagara Sci-Fi, Rocket, Plane Crash, Water Starter; Realistic Starter v1/v2 на **Cascade** (брать Niagara-версию).

---

## Системы и UI

| Пак | Папка | Размер | git | Исп. | Что это |
|---|---|---|---|---|---|
| **Easy Options Menu** | `EasyOptionsMenu` | 40M | git | ✅ | экран настроек (нужен плагин JsonBlueprintUtilities) |
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

### Итого (≈15 ГБ Content — было ~50)
- **В git** (клонируется): наш код+контент, HospitalCombo, Construction_VOL2, EasyOptionsMenu, FootstepSystem, ResourcePack, Content/Audio/SFX.
- **Локально** (тяжёлое, не в git): Hospital, PostApocalypticHouse, Hyper (обрезан до 106М), Construction_Pit, NiagaraExamples, oldBenchPack, Fire_EXP, Survival_SFX + Plugins + RawAssets.
- **Удалено в уборку** (−~47 ГБ суммарно): AmericanDrive, LightHouse, Madrid_Street, Shoothouse, Backrooms_TheLobby, YI_Luggage, FencesVOL2, Adventure_Pack(Sarah), Citizens_Pack, 3 лишних больницы (Leartes/Horror/Abandoned), **FoggyStreet, Decal_Forge, OGMainMenu**, плагин EOSCore (платный), + ~6 ГБ балласта внутри Hyper.
- **Лицензий к докупке** (паки в билде): ~по одному на категорию — больница(City+Combo), дом, стройка, Hyper, ResourcePack, EasyOptions, AudioToolkitPro + звук-библиотеки (Ghosthack, Asylum, Survival). Бесплатные: Fire_EXP, NiagaraExamples, EOS (движок). _Туман/декали — теперь из Hyper-погоды / твоих VFX._

---

## Source: ASSET_ASSESSMENT.md

# Разбор импортированных паков — что брать, что нет

Параллельный аудит (2026-06-14): отдельные агенты прочитали структуру/зависимости каждого пака и оценили применимость к Аварийке. Каталог самих паков — см. [ASSETS.md](../Archive/Originals/Assets/ASSETS.md).

## ГЛАВНОЕ: динамическая погода (HyperDynamicWeatherSky)

**Вердикт: фреймворк целиком — SKIP. Cherry-pick только VFX грозы/дождя + статичное ночное небо.**

### Стоит ли брать саму «динамическую погоду»?
Нет. Это система для open-world/survival: погода, биомы, сезоны, часовые пояса. В закрытой больнице/доме всё это почти не видно, а нашу ночь+туман+свет↔паника мы УЖЕ сделали. Из 6.3 ГБ реально полезно ~45–50 МБ. Полный движок погоды только продублирует и поломает нашу архитектуру.

### Что именно мешает (компонентность на GameState/Controller)
Погода живёт как компоненты на GameState и PlayerController, со своим ReplicationSubSystem и интерфейсами (BPI_WeatherGameState/BPI_Biome). Если внедрить:
- получаем **второй параллельный мир-стейт** со своей репликацией поверх нашего C++ ARunState/диспетчера/фаз забега;
- **авторитет хоста на погоду конкурирует** с авторитетом на фазы забега — конфликт коопной репликации;
- тянет BML_*-макробиблиотеки и весь слой енумов/структур (DayStates/WeatherStates/Biomes/Seasons);
- UI пака требует CommonUI+CommonInput (не включены), а у нас Canvas-HUD.
Поэтому НЕ навешивать AC_GameState_Time / AC_GameState_DayNightCycle / AC_Biome_WeatherManager на наш ARunState и AC_PlayerController_WeatherManager на наш PC.

### Как сделать нашу ночь/дождь/грозу без 5.7 ГБ природного мусора
1. **VFX (нужен только Niagara, уже включён).** Самодостаточны, без менеджера, из `ResourcePack/Effects/Weather`:
   - `NS_ThunderStormWithRain`, `NS_Rain_Heavy`, `NS_Rain_Light` — дождь/гроза за окнами палат;
   - `NS_Heavy_Wind`, `NS_Blizzard` — по желанию.
   - Спавнить из ARunState как ambient FX или ставить вручную за окнами. Молнию позже подвязать к шумовой системе (раскат грома = маскировка шагов / всплеск для будущего монстра-слухача).
2. **Ночное небо — статично, без динамики:**
   - положить `BP_DayNight_SunMoon` и зафиксировать ночь (НЕ запускать day-night менеджер) — он несёт SkyAtmosphere+SkyLight+DirectionalLight+VolumetricCloud+ExpHeightFog+PostProcess готовым ригом;
   - либо чище: скопировать эти компоненты в расстановку света уровня + `SM_SkySphere`/`M_SkySphere`/`T_Moon`/`T_Stars`/`MI_SimpleVolumetricClouds` как ночной купол.

### Порядок действий (Hyper)
1. При закрытом редакторе вычистить балласт (~6 ГБ): удалить `ResourcePack/Environments` (3.1 ГБ), `ResourcePack/Tileable_Materials` (2.3 ГБ), `Locomotion` (390 МБ), `Food/Props/Epic`. Оставить `ResourcePack/Effects/Weather`, `Icons/Weather_States`, `ResourcePack/Post_Process_Effects`.
2. **Перед удалением Locomotion** обнулить хард-ссылку `BP_DayNight_SunMoon` → `/Game/Hyper/Locomotion/Chars` (и на Effects), иначе ошибка загрузки/кука.
3. Учесть `*_BuiltData` и ссылки перед чисткой.
4. Поставить ночной небо-риг статично в тест-уровень, проверить связку с туманом/паникой.
5. Подключить 1–2 грозовых NS_* за окнами, проверить производительность в коопе.

**Усилия на полезную часть — низкие. На полное внедрение — высокие и не оправданы.**

---

## Остальные паки (краткие вердикты)

- **City Modular Hospital v.2** — **USE (cherry-pick), флагман-окружение.** Собрать СВОЙ уровень из модульного кита; прямое попадание ремонт-целей: щиток `SM_Electrical_Shield`, генератор `SM_Generator`. Шаг: перевести двери-BP на наше взаимодействие, под «трубу» искать меш отдельно, текстуры ужать до 2K.
- **Modular Haunted House** — **USE (cherry-pick), карта «дом» для прототипа 0.1.** Взять `LV_House` + кит и особенно `LV_Horror_Light` (готовый ночь+туман+PostProcess+LUT, ложится на нашу панику-от-света). Шаг: дублировать LV_House с `__ExternalActors__`, поставить наш GameMode; для швыряемого дебриса снять Nanite/дать fallback.
- **Citizens_Pack** — **cherry-pick, мирные NPC (посетители/пациенты).** Шаг: реассайн скелета на наш Mannequin → бесплатная анимация. Медперсонала (халаты) НЕТ — форму искать отдельно; текстуры до 1–2K.
- **Easy Options Menu** — **USE, готовый экран настроек на чистом UMG.** JsonBlueprintUtilities уже включён. Шаг: открывать `WBP_EasyOptionsMenuMain`, привязать ремап к нашим Avaryo IMC/IA, выкинуть Demo/Benchmark/Maps.
- **OG Main Menu** — **SKIP (как фронт).** Тяжёлый CommonUI-фреймворк со своим GI/GM/PC/HUD/Sessions, конфликтует с нашим стеком. Своё меню уже сделали.

---

## Что делать дальше (по приоритету)

1. **Флагман-уровень больницы (City Hospital):** playable-коробка из модулей под ARunState, ремонт-цели через set_static_mesh на `SM_Electrical_Shield`/`SM_Generator`, меш под «трубу», динамический ночной свет (Lumen/movable). Ядро игры.
2. **Хоррор-свет даром:** перенести светориг `LV_Horror_Light` (DirLight+SkyLight+ExpHeightFog+PPV+LUT+RectLights) в геймплейный уровень — самый дешёвый способ закрыть «ночь+туман+свет↔паника».
3. **Hyper — мини-задача cherry-pick:** статичное ночное небо в окнах + 1–2 грозовых NS_* за окнами; затем вычистить 6 ГБ балласта (при закрытом редакторе, обнулив хард-ссылки BP_DayNight_SunMoon). Фреймворк погоды не трогать.
4. **Меню настроек (EOM):** вкрутить `WBP_EasyOptionsMenuMain` в паузу, ремап → наши Enhanced Input ассеты, аудио-слайдеры → наши SoundClass/SoundMix (согласовать с AudioToolkitPro).
5. **NPC из Citizens_Pack:** реассайн скелета на Mannequin, базовый NPC-BP (пациенты — спящая поза, посетители — MoveTo), даунскейл текстур. После того, как уровень и цикл починок стоят.

---

## Source: PACKS_REFERENCE.md

# Каталог паков дома — справочник для ручной сборки

Снято из проекта (asset registry + bounds). Габариты = X×Y×Z в см (полный размер меша).

Высота этажа в проекте: **1–2 эт = 300 см**, чердак 220–240, подвал 220. Снап-сетка 100 см.

Базовый путь мешей: `/Game/ResidentialHouses/Meshes/...`


---
## RESIDENTIALHOUSES — строительный набор (Architecture)


### `Meshes/Architecture/Walls` (90)
- SM_Coving_Cap — 25×15×15 см
- SM_Inside_1m — 100×15×315 см
- SM_Inside_2m — 200×15×315 см
- SM_Inside_3m — 300×15×315 см
- SM_Inside_Corner_1m — 100×100×315 см
- SM_Inside_Corner_1m_2 — 50×100×315 см
- SM_Inside_Corner_1m_3 — 50×100×315 см
- SM_Inside_Corner_1m_b — 100×100×665 см
- SM_Inside_Corner_2m — 200×200×315 см
- SM_Inside_Corner_3m — 300×300×315 см
- SM_Inside_Corner_S — 150×200×315 см
- SM_Inside_Corner_S_2 — 150×115×315 см
- SM_Inside_Corner_S_2_X — 150×115×315 см
- SM_Inside_Corner_S_X — 150×200×315 см
- SM_Inside_Detail_3m — 315×65×315 см
- SM_Inside_Door_2_3m — 300×15×315 см
- SM_Inside_Door_2_4m — 400×15×315 см
- SM_Inside_Door_2m — 200×15×315 см
- SM_Inside_Door_3m — 300×15×315 см
- SM_Inside_FirePlace_1m — 100×15×165 см
- SM_Inside_FirePlace_2m — 200×15×315 см
- SM_Inside_FirePlace_2m_b — 200×15×665 см
- SM_Inside_IntCorner_1m — 115×115×315 см
- SM_Inside_IntCorner_1m_2 — 65×115×315 см
- SM_Inside_IntCorner_1m_2_2 — 65×115×315 см
- SM_Inside_IntCorner_1m_b — 115×115×665 см
- SM_Inside_IntCorner_1m_b_Coving — 115×36×15 см
- SM_Inside_Wall_1m_b — 100×15×665 см
- SM_Inside_Wall_2m_b — 200×15×665 см
- SM_Inside_Window_2_3m — 300×15×315 см
- SM_Inside_Window_2_4m — 400×15×315 см
- SM_Inside_Window_2m — 200×15×315 см
- SM_Inside_Window_2m_b — 200×15×665 см
- SM_Inside_Window_3_3m — 300×15×315 см
- SM_Inside_Window_3_4m — 400×15×315 см
- SM_Inside_Window_3m — 300×15×315 см
- SM_Inside_Window_4m — 400×15×315 см
- SM_Int_Wall_45 — 400×215×315 см
- SM_Int_Wall_Staircase — 490×600×665 см
- SM_Outside_1m — 100×4×350 см
- SM_Outside_2m — 200×4×350 см
- SM_Outside_3m — 300×4×350 см
- SM_Outside_Corner_1m — 125×126×350 см
- SM_Outside_Corner_1m_2 — 75×126×350 см
- SM_Outside_Corner_1m_3 — 75×126×350 см
- SM_Outside_Corner_2m — 226×226×350 см
- SM_Outside_Corner_3m — 325×326×350 см
- SM_Outside_Corner_S — 175×225×350 см
- SM_Outside_Corner_S_2 — 150×106×350 см
- SM_Outside_Corner_S_2_X — 150×105×350 см
- SM_Outside_Corner_S_X — 175×226×350 см
- SM_Outside_Door_2_3m — 300×4×350 см
- SM_Outside_Door_2_4m — 400×4×350 см
- SM_Outside_Door_2m — 200×4×350 см
- SM_Outside_Door_3m — 300×4×350 см
- SM_Outside_GarageDoor_8m — 800×4×120 см
- SM_Outside_IntCorner_1m — 80×80×350 см
- SM_Outside_IntCorner_1m_2 — 30×80×350 см
- SM_Outside_IntCorner_1m_2_2 — 30×80×350 см
- SM_Outside_Roof01_0-5_25 — 4×50×144 см
- SM_Outside_Roof01_0-5_25_Cap — 4×50×36 см
- SM_Outside_Roof01_1m_25 — 4×100×144 см
- SM_Outside_Roof01_1m_25_Cap — 4×100×36 см
- SM_Outside_Roof01_2-5m_25 — 4×250×144 см
- SM_Outside_Roof01_2-5m_25_Cap — 4×250×36 см
- SM_Outside_Roof01_2m_025 — 4×200×144 см
- SM_Outside_Roof01_2m_25_Cap — 4×200×36 см
- SM_Outside_Roof01_3m_25 — 4×300×144 см
- SM_Outside_Roof01_3m_25_Cap — 4×300×36 см
- SM_Outside_Roof02_0-5_25 — 4×50×27 см
- SM_Outside_Roof02_0-5_25_2 — 4×50×27 см
- SM_Outside_Roof02_1-5m_25 — 4×147×69 см
- SM_Outside_Roof02_1-5m_25_2 — 4×147×69 см
- SM_Outside_Roof02_1m_25 — 4×100×51 см
- SM_Outside_Roof02_1m_25_2 — 4×100×51 см
- SM_Outside_Roof02_2-5m_25 — 4×256×119 см
- SM_Outside_Roof02_2-5m_25_2 — 4×256×119 см
- SM_Outside_Roof02_2m_025_2 — 4×200×97 см
- SM_Outside_Roof02_2m_25 — 4×200×97 см
- SM_Outside_Roof02_3m_25 — 4×300×144 см
- SM_Outside_Roof02_3m_25_2 — 4×300×144 см
- SM_Outside_Window_2_3m — 300×4×350 см
- SM_Outside_Window_2_4m — 400×4×350 см
- SM_Outside_Window_2m — 200×4×350 см
- SM_Outside_Window_3_3m — 300×4×350 см
- SM_Outside_Window_3_4m — 400×4×350 см
- SM_Outside_Window_3m — 300×4×350 см
- SM_Outside_Window_4m — 400×4×350 см
- SM_Outside_Window_Garage_3m — 300×4×350 см
- SM_Outside_Window_Garage_4m — 400×4×350 см

### `Meshes/Architecture/Walls/Garage` (8)
- SM_Garage_Inside_1m — 100×3×355 см
- SM_Garage_Inside_2m — 200×3×355 см
- SM_Garage_Inside_3m — 300×3×355 см
- SM_Garage_Inside_3m_Window — 300×3×355 см
- SM_Garage_Inside_4m_Window — 400×3×355 см
- SM_Garage_Inside_Corner_1m — 100×100×355 см
- SM_Garage_Inside_Corner_2m — 200×200×355 см
- SM_Garage_Inside_Corner_3m — 300×300×355 см

### `Meshes/Architecture/Walls/Garage/PartitionWalls` (6)
- SM_Garage_Int_Corner_1m — 90×90×355 см
- SM_Garage_Int_Wall_1m — 100×3×355 см
- SM_Garage_Int_Wall_2m — 200×3×355 см
- SM_Garage_Int_Wall_2m_Door — 200×3×355 см
- SM_Garage_Int_Wall_3m — 300×3×355 см
- SM_Garage_Int_Wall_3m_Door — 300×3×355 см

### `Meshes/Architecture/Walls/Garage/PartitionWalls/Stairs` (1)
- SM_Garage_Stairs — 189×142×96 см

### `Meshes/Architecture/Walls/PartitionWalls` (42)
- SM_Door_2m — 200×15×315 см
- SM_Door_3m — 300×15×315 см
- SM_Door_4m — 400×15×315 см
- SM_Door_4m_2 — 400×15×315 см
- SM_Door_Bifold — 300×15×315 см
- SM_Ext_Corner_1m — 125×125×315 см
- SM_Ext_Corner_1m_Staircase — 125×25×315 см
- SM_Ext_Corner_2_1m — 125×125×315 см
- SM_Ext_Corner_2_2m — 225×225×315 см
- SM_Ext_Corner_2_3m — 325×325×315 см
- SM_Ext_Corner_2m — 225×225×315 см
- SM_Ext_Corner_3m — 325×325×315 см
- SM_Ext_Door_Corner_1m — 325×125×315 см
- SM_Ext_Door_Corner_1m_2 — 325×125×315 см
- SM_Ext_Door_Corner_2_3m — 325×350×315 см
- SM_Ext_Door_Corner_2_3m_2 — 325×350×315 см
- SM_Ext_Door_Corner_3m — 325×325×315 см
- SM_Ext_Door_Corner_3m_2 — 325×325×315 см
- SM_Floor_3m_Cut — 300×300×0 см
- SM_Floor_Triangle — 50×50×0 см
- SM_Int_Corner_1m — 90×90×315 см
- SM_Int_Corner_2_1m — 90×90×315 см
- SM_Int_Corner_2_2m — 190×190×315 см
- SM_Int_Corner_2_3m — 290×290×315 см
- SM_Int_Corner_2m — 190×190×315 см
- SM_Int_Corner_3m — 290×290×315 см
- SM_Int_Door_Corner_1m — 290×90×315 см
- SM_Int_Door_Corner_1m_2 — 290×90×315 см
- SM_Int_Door_Corner_2_3m — 290×280×315 см
- SM_Int_Door_Corner_2_3m_2 — 290×280×315 см
- SM_Int_Door_Corner_3m — 290×290×315 см
- SM_Int_Door_Corner_3m_2 — 290×290×315 см
- SM_Int_Wall_1m — 100×15×315 см
- SM_Int_Wall_2m — 200×15×315 см
- SM_Int_Wall_3m — 300×15×315 см
- SM_Int_Wall_Bifold — 280×80×315 см
- SM_Int_Wall_Cap — 25×50×315 см
- SM_Int_Wall_Pillar — 55×55×315 см
- SM_Int_Wall_Staircase_1m — 100×15×665 см
- SM_Int_Wall_Staircase_2m — 200×15×665 см
- SM_Int_Wall_Staircase_3m — 300×15×665 см
- SM_Int_Wall_Staircase_Window_2m — 200×15×665 см

### `Meshes/Architecture/Walls/Tiles` (24)
- SM_Inside_1m_Tiles — 100×15×315 см
- SM_Inside_1m_Tiles02 — 100×15×315 см
- SM_Inside_2m_Tiles — 200×15×315 см
- SM_Inside_2m_Tiles02 — 200×15×315 см
- SM_Inside_3m_Tiles — 300×15×315 см
- SM_Inside_3m_Tiles02 — 300×15×315 см
- SM_Inside_Corner_1m_Tiles — 100×100×315 см
- SM_Inside_Corner_1m_Tiles02 — 100×100×315 см
- SM_Inside_Corner_2m_Tiles — 200×200×315 см
- SM_Inside_Corner_2m_Tiles02 — 200×200×315 см
- SM_Inside_Window_2_3m_Tiles — 300×15×315 см
- SM_Inside_Window_2_3m_Tiles02 — 300×15×315 см
- SM_Inside_Window_2_4m_Tiles — 400×15×315 см
- SM_Inside_Window_2_4m_Tiles02 — 400×15×315 см
- SM_Inside_Window_2m_Tiles — 200×15×315 см
- SM_Inside_Window_2m_Tiles02 — 200×15×315 см
- SM_Inside_Window_3_3m_Tiles — 300×15×315 см
- SM_Inside_Window_3_3m_Tiles02 — 300×15×315 см
- SM_Inside_Window_3_4m_Tiles — 400×15×315 см
- SM_Inside_Window_3_4m_Tiles02 — 400×15×315 см
- SM_Inside_Window_3m_Tiles — 300×15×315 см
- SM_Inside_Window_3m_Tiles02 — 300×15×315 см
- SM_Inside_Window_4m_Tiles — 400×15×315 см
- SM_Inside_Window_4m_Tiles02 — 400×15×315 см

### `Meshes/Architecture/Walls/Tiles/PartionWalls` (51)
- SM_Bathtub_Wall — 111×22×72 см
- SM_Door_2m_Tiles — 200×15×315 см
- SM_Door_2m_Tiles02 — 200×15×315 см
- SM_Door_2m_Tiles_Half — 200×15×315 см
- SM_Door_3m_Bath_Half — 300×15×315 см
- SM_Door_3m_Tiles — 300×15×315 см
- SM_Door_3m_Tiles02 — 300×15×315 см
- SM_Door_4m_Tiles — 400×15×315 см
- SM_Door_4m_Tiles02 — 400×15×315 см
- SM_Ext_Corner_1m_Tiles — 125×125×315 см
- SM_Ext_Corner_1m_Tiles02 — 125×125×315 см
- SM_Ext_Corner_2m_Tiles — 225×225×315 см
- SM_Ext_Corner_2m_Tiles02 — 225×225×315 см
- SM_Ext_Corner_3m_Tiles — 325×325×315 см
- SM_Ext_Corner_3m_Tiles02 — 325×325×315 см
- SM_Ext_Door_Corner_1m_2_Tiles — 325×125×315 см
- SM_Ext_Door_Corner_1m_2_Tiles02 — 325×125×315 см
- SM_Ext_Door_Corner_1m_Tiles — 325×125×315 см
- SM_Ext_Door_Corner_1m_Tiles02 — 325×125×315 см
- SM_Ext_Door_Corner_2_3m_2_Tiles — 325×350×315 см
- SM_Ext_Door_Corner_2_3m_2_Tiles02 — 325×350×315 см
- SM_Ext_Door_Corner_2_3m_Tiles — 325×350×315 см
- SM_Ext_Door_Corner_2_3m_Tiles02 — 325×350×315 см
- SM_Ext_Door_Corner_3m_2_Tiles — 325×325×315 см
- SM_Ext_Door_Corner_3m_2_Tiles02 — 325×325×315 см
- SM_Ext_Door_Corner_3m_Tiles — 325×325×315 см
- SM_Ext_Door_Corner_3m_Tiles02 — 325×325×315 см
- SM_Int_Corner_1m_Tiles — 90×90×315 см
- SM_Int_Corner_1m_Tiles02 — 90×90×315 см
- SM_Int_Corner_2m_Tiles — 190×190×315 см
- SM_Int_Corner_2m_Tiles02 — 190×190×315 см
- SM_Int_Corner_3m_Tiles — 290×290×315 см
- SM_Int_Corner_3m_Tiles02 — 290×290×315 см
- SM_Int_Door_Corner_1m_2_Tiles — 290×90×315 см
- SM_Int_Door_Corner_1m_2_Tiles02 — 290×90×315 см
- SM_Int_Door_Corner_1m_Tiles — 290×90×315 см
- SM_Int_Door_Corner_1m_Tiles02 — 290×90×315 см
- SM_Int_Door_Corner_2_3m_2_Bath_02 — 290×280×315 см
- SM_Int_Door_Corner_2_3m_2_Tiles — 290×280×315 см
- SM_Int_Door_Corner_2_3m_Tiles — 290×280×315 см
- SM_Int_Door_Corner_2_3m_Tiles02 — 290×280×315 см
- SM_Int_Door_Corner_3m_2_Tiles — 290×290×315 см
- SM_Int_Door_Corner_3m_2_Tiles02 — 290×290×315 см
- SM_Int_Door_Corner_3m_Tiles — 290×290×315 см
- SM_Int_Door_Corner_3m_Tiles02 — 290×290×315 см
- SM_Int_Wall_1m_Bath_02 — 100×15×315 см
- SM_Int_Wall_1m_Tiles — 100×15×315 см
- SM_Int_Wall_2m_Tiles — 200×15×315 см
- SM_Int_Wall_2m_Tiles02 — 200×15×315 см
- SM_Int_Wall_3m_Tiles — 300×15×315 см
- SM_Int_Wall_3m_Tiles02 — 300×15×315 см

### `Meshes/Architecture/Floors` (22)
- SM_Floor_1m_a — 300×100×0 см
- SM_Floor_1m_b — 100×300×0 см
- SM_Floor_1m_c — 300×100×0 см
- SM_Floor_1m_d — 100×300×0 см
- SM_Floor_1m_d2 — 100×200×0 см
- SM_Floor_1x1m — 100×100×0 см
- SM_Floor_2m_a — 300×200×0 см
- SM_Floor_2m_b — 200×300×0 см
- SM_Floor_2m_c — 300×200×0 см
- SM_Floor_2m_d — 200×300×0 см
- SM_Floor_3m — 300×300×0 см
- SM_Floor_Bay_a — 500×100×0 см
- SM_Floor_Bay_c — 500×100×0 см
- SM_Floor_a1 — 100×100×0 см
- SM_Floor_a2 — 100×100×0 см
- SM_Floor_a3 — 100×100×0 см
- SM_Floor_b1 — 100×100×0 см
- SM_Floor_b2 — 100×100×0 см
- SM_Floor_b3 — 100×100×0 см
- SM_Floor_c1 — 100×100×0 см
- SM_Floor_c2 — 100×100×0 см
- SM_Floor_c3 — 100×100×0 см

### `Meshes/Architecture/Ceiling` (22)
- SM_Ceiling_1m_a — 300×100×0 см
- SM_Ceiling_1m_b — 100×300×0 см
- SM_Ceiling_1m_b2 — 100×200×0 см
- SM_Ceiling_1m_c — 300×100×0 см
- SM_Ceiling_1m_d — 100×300×0 см
- SM_Ceiling_1m_d2 — 100×200×0 см
- SM_Ceiling_2m_a — 300×200×0 см
- SM_Ceiling_2m_b — 200×300×0 см
- SM_Ceiling_2m_c — 300×200×0 см
- SM_Ceiling_2m_d — 200×300×0 см
- SM_Ceiling_3m — 300×300×0 см
- SM_Ceiling_Bay_a — 500×100×0 см
- SM_Ceiling_Bay_c — 500×100×0 см
- SM_Ceiling_a1 — 100×100×0 см
- SM_Ceiling_a2 — 100×100×0 см
- SM_Ceiling_a3 — 100×100×0 см
- SM_Ceiling_b1 — 100×100×0 см
- SM_Ceiling_b2 — 100×100×0 см
- SM_Ceiling_b3 — 100×100×0 см
- SM_Ceiling_c1 — 100×100×0 см
- SM_Ceiling_c2 — 100×100×0 см
- SM_Ceiling_c3 — 100×100×0 см

### `Meshes/Architecture/Stairs` (11)
- SM_StairRailing_1m — 30×100×148 см
- SM_StairRailing_2m — 30×203×148 см
- SM_StairRailing_Corner — 203×201×160 см
- SM_StairRailing_Corner_Wall_Trim — 203×401×160 см
- SM_StairRailing_Post — 15×15×123 см
- SM_Staircase — 498×526×490 см
- SM_Staircase_Carpet — 498×525×490 см
- SM_Staircase_Straight — 188×606×366 см
- SM_Staircase_Straight_Carpet — 188×605×366 см
- SM_Staircase_U — 400×516×476 см
- SM_Staircase_U_Carpet — 400×516×476 см

### `Meshes/Architecture/Doors` (12)
- SM_DoorFrame — 164×28×247 см
- SM_DoorFrame_4m_2 — 384×28×249 см
- SM_DoorFrame_Bifold — 264×28×247 см
- SM_DoorFrame_Entrance — 280×39×255 см
- SM_DoorFrame_Entrance_Back — 178×39×255 см
- SM_DoorFrame_Garage — 801×30×364 см
- SM_Door_Bifold_L_01 — 58×8×230 см
- SM_Door_Bifold_L_02 — 59×13×230 см
- SM_Door_Bifold_R_01 — 58×8×230 см
- SM_Door_Bifold_R_02 — 59×13×230 см
- SM_EntranceDoor — 131×22×228 см
- SM_InsideDoor — 133×28×230 см

### `Meshes/Architecture/Window` (4)
- SM_Window_1m — 136×40×195 см
- SM_Window_1m_Short — 136×40×172 см
- SM_Window_2m — 266×40×195 см
- SM_Window_2m_Garage — 207×29×102 см

### `Meshes/Architecture/Window/Shutters` (11)
- SM_Window_Hinge_1m — 107×2×167 см
- SM_Window_Hinge_2m — 228×2×167 см
- SM_Window_Short_Hinge_1m — 107×2×144 см
- SM_Window_Short_Shutter_1m_L — 53×4×146 см
- SM_Window_Short_Shutter_1m_R — 53×4×146 см
- SM_Window_Shutter_1m_L — 53×4×169 см
- SM_Window_Shutter_1m_R — 53×4×169 см
- SM_Window_Shutter_2m_L — 54×4×170 см
- SM_Window_Shutter_2m_L_2 — 50×3×169 см
- SM_Window_Shutter_2m_R — 53×4×169 см
- SM_Window_Shutter_2m_R_2 — 50×3×170 см

### `Meshes/Architecture/Foundation` (15)
- SM_Foundation_1m — 100×6×100 см
- SM_Foundation_2m — 200×6×100 см
- SM_Foundation_3m — 300×6×100 см
- SM_Foundation_Corner_1m — 125×126×100 см
- SM_Foundation_Corner_1m_2 — 75×126×100 см
- SM_Foundation_Corner_1m_3 — 75×126×100 см
- SM_Foundation_Corner_2m — 226×226×100 см
- SM_Foundation_Corner_3m — 326×326×100 см
- SM_Foundation_Corner_S — 175×225×101 см
- SM_Foundation_Corner_S_2 — 150×111×101 см
- SM_Foundation_Corner_S_2_X — 150×111×101 см
- SM_Foundation_Corner_S_X — 175×226×101 см
- SM_Foundation_IntCorner_1m_2 — 35×85×100 см
- SM_Foundation_IntCorner_1m_2_2 — 35×85×100 см
- SM_Foundation_Int_Corner_1m — 85×85×100 см

### `Meshes/Architecture/Porch` (29)
- SM_Porch_2m_End_L — 207×326×512 см
- SM_Porch_2m_End_R — 206×326×512 см
- SM_Porch_2m_End_R_2 — 206×342×512 см
- SM_Porch_3m — 315×326×512 см
- SM_Porch_3m_2 — 304×326×512 см
- SM_Porch_3m_Corner — 354×354×512 см
- SM_Porch_3m_End_L — 323×326×512 см
- SM_Porch_3m_End_L_Foundation — 316×292×103 см
- SM_Porch_3m_End_R — 323×326×512 см
- SM_Porch_3m_Foundation — 304×292×103 см
- SM_Porch_3m_Roof — 302×326×132 см
- SM_Porch_3m_Roof_2 — 302×325×132 см
- SM_Porch_3m_Roof_3 — 302×416×158 см
- SM_Porch_B_2m_End_R — 220×304×450 см
- SM_Porch_B_3m_2 — 304×304×450 см
- SM_Porch_Railing — 300×11×88 см
- SM_Porch_Railing_2 — 150×11×88 см
- SM_Porch_Railing_3 — 105×11×88 см
- SM_Porch_Railing_4 — 53×11×88 см
- SM_Porch_Railing_B — 300×8×56 см
- SM_Porch_Railing_B_2 — 150×8×56 см
- SM_Porch_Railing_Column — 29×29×278 см
- SM_Porch_Railing_Column_B — 29×29×350 см
- SM_Porch_Railing_Post — 16×16×112 см
- SM_Porch_Stairs — 280×127×215 см
- SM_Porch_Stairs_2 — 227×127×215 см
- SM_Porch_Stairs_B — 326×214×215 см
- SM_Porch_Stairs_B_2 — 326×214×215 см
- SM_StormPorch_Garage — 1005×181×190 см

### `Meshes/Architecture/Roof` (46)
- SM_Roof_1x0-5_25 — 100×50×24 см
- SM_Roof_1x1-5_25 — 100×150×70 см
- SM_Roof_1x1_25 — 100×100×48 см
- SM_Roof_1x2-5_25 — 100×250×118 см
- SM_Roof_1x2_25 — 100×200×94 см
- SM_Roof_1x3_25 — 100×300×140 см
- SM_Roof_2x0-5_25 — 200×50×24 см
- SM_Roof_2x1-5_25 — 200×150×70 см
- SM_Roof_2x1_25 — 200×100×48 см
- SM_Roof_2x2-5_25 — 200×250×118 см
- SM_Roof_2x2_25 — 200×200×94 см
- SM_Roof_2x3_25 — 200×300×140 см
- SM_Roof_3x0-5_25 — 300×50×24 см
- SM_Roof_3x1-5_25 — 300×150×70 см
- SM_Roof_3x1_25 — 300×100×48 см
- SM_Roof_3x2-5_25 — 300×250×118 см
- SM_Roof_3x2_25 — 300×200×94 см
- SM_Roof_3x3_25 — 300×300×140 см
- SM_Roof_Cap_1_25 — 100×50×12 см
- SM_Roof_Cap_2_25 — 200×50×12 см
- SM_Roof_Cap_3_25 — 300×50×12 см
- SM_Roof_Cap_End_L_25 — 89×50×14 см
- SM_Roof_Cap_End_R_25 — 89×50×14 см
- SM_Roof_Overhang_1_25 — 100×90×66 см
- SM_Roof_Overhang_2_25 — 200×90×66 см
- SM_Roof_Overhang_3_25 — 300×91×66 см
- SM_Roof_Overhang_End_L_0-5_25 — 89×50×54 см
- SM_Roof_Overhang_End_L_1-5_25 — 89×150×100 см
- SM_Roof_Overhang_End_L_1_25 — 89×100×79 см
- SM_Roof_Overhang_End_L_2-5_25 — 89×251×149 см
- SM_Roof_Overhang_End_L_25 — 89×89×66 см
- SM_Roof_Overhang_End_L_25_02 — 189×89×66 см
- SM_Roof_Overhang_End_L_2_25 — 89×201×124 см
- SM_Roof_Overhang_End_L_3_25 — 89×300×170 см
- SM_Roof_Overhang_End_R_0-5_25 — 89×50×54 см
- SM_Roof_Overhang_End_R_1-5_25 — 89×150×100 см
- SM_Roof_Overhang_End_R_1_25 — 89×100×79 см
- SM_Roof_Overhang_End_R_2-5_25 — 89×251×149 см
- SM_Roof_Overhang_End_R_25 — 89×89×66 см
- SM_Roof_Overhang_End_R_25_02 — 189×89×66 см
- SM_Roof_Overhang_End_R_2_25 — 89×201×124 см
- SM_Roof_Overhang_End_R_3_25 — 89×300×170 см
- SM_Roof_Overhang_IntCorner — 100×100×66 см
- SM_Roof_Overhang_IntCorner_2 — 89×89×66 см
- SM_Roof_Overhang_IntCorner_3 — 100×89×66 см
- SM_Roof_Overhang_IntCorner_3_2 — 100×89×66 см

### `Meshes/Architecture/Edging` (4)
- SM_FloorEdging_1
- SM_FloorEdging_1m
- SM_FloorEdging_1m_Corner
- SM_FloorEdging_2

### `Meshes/Architecture/FalseRooms` (1)
- SM_FrontDoorFalseRoom

### `Meshes/Architecture/Gutter` (15)
- SM_Downspout
- SM_Downspout_End
- SM_Downspout_Top
- SM_Downspout_Top_Porch
- SM_Gutter_1m
- SM_Gutter_2m
- SM_Gutter_3m
- SM_Gutter_Cap
- SM_Gutter_Cap_2
- SM_Gutter_Corner_1m
- SM_Gutter_Corner_1m_2
- SM_Gutter_Corner_L
- SM_Gutter_Corner_L_2
- SM_Gutter_Overhang
- SM_Gutter_Porch_Downspout

---
## RESIDENTIALHOUSES — мебель и пропы (по папкам)


### `HDRI_Backdrop/Meshes` (1)
  SM_EnviroDome_NoBase

### `Meshes/ArmChair` (1)
  SM_Armchair_02

### `Meshes/Backdrop` (1)
  Backdrop

### `Meshes/Ball` (2)
  SM_BasketBall, SM_SoccerBall

### `Meshes/BarStool` (1)
  SM_BarStool

### `Meshes/Bathroom` (36)
  SM_Aftershave, SM_BathTub, SM_Bleach, SM_Cabinet, SM_CabinetDoor_L, SM_CabinetDoor_R, SM_CleaningSpray, SM_Cup, SM_Cupboard, SM_CupboardDoor, SM_Mouthwash, SM_Razor, SM_Shampoo01, SM_Shampoo02, SM_ShampooBottle1, SM_ShampooBottle2, SM_ShampooBottle3, SM_ShowerCurtain, SM_ShowerHead, SM_ShowerTap, SM_ShowerValve, SM_Sink, SM_SoapBar, SM_SoapDish, SM_SoapDispenser, SM_SoapHolder, SM_Toilet, SM_ToiletBrush, SM_ToiletPaper, SM_ToiletRollHolder, SM_ToiletSeat, SM_ToiletSeat_Cover, SM_Toothbrush, SM_Toothpaste, SM_TowelRack, SM_TowelRail

### `Meshes/BathroomMats` (5)
  SM_BathMat_01, SM_BathMat_02, SM_TowelStack_01, SM_Towel_01, SM_Towel_02

### `Meshes/Bed` (13)
  SM_Bed, SM_BedCushion_01, SM_BedCushion_02, SM_Cover_01, SM_Cover_02, SM_Cover_03, SM_Duvet01, SM_Matress, SM_MatressCover, SM_Pillow01, SM_Pillow02, SM_QueenBed, SM_QueenBed_Matress

### `Meshes/BedroomFurniture` (9)
  SM_BedsideTable, SM_ChestDrawers, SM_Door01_L, SM_Door01_R, SM_Door02_L, SM_Door02_R, SM_Drawer01, SM_Drawer02, SM_Wardrobe

### `Meshes/Books` (11)
  SM_Book01, SM_Book02, SM_Book03, SM_Book04, SM_Book05, SM_Book06, SM_BooksCollection01, SM_BooksCollection02, SM_BooksCollection03, SM_BooksCollection04, SM_BooksCollection05

### `Meshes/Bookshelf` (12)
  SM_Bookshelf01, SM_Bookshelf02, SM_Bookshelf03, SM_Bookshelf04, SM_Bookshelf05, SM_Cupboard01, SM_Door01_Left, SM_Door01_Right, SM_Door02_Left, SM_Door02_Left_Glass, SM_Door02_Right, SM_Door02_Right_Glass

### `Meshes/Bottles` (4)
  SM_Bottle_01, SM_Bottle_02, SM_Bottle_03, SM_Bottle_04

### `Meshes/Cable` (1)
  SM_Cable

### `Meshes/Cable/MergedCables` (8)
  SM_MERGED_BP_CableSpline_1, SM_MERGED_BP_CableSpline_2, SM_MERGED_BP_CableSpline_3, SM_MERGED_BP_CableSpline_4, SM_MERGED_BP_CableSpline_5, SM_MERGED_BP_CableSpline_6, SM_MERGED_BP_CableSpline_7, SM_MERGED_BP_CableSpline_8

### `Meshes/Candle` (7)
  SM_CandleGlass, SM_CandlePlateCage_01, SM_CandlePlate_01, SM_CandlePlate_02, SM_Candle_01, SM_Candle_02, SM_Candle_03

### `Meshes/Cardboard` (5)
  SM_Cardboard_Closed_1, SM_Cardboard_Closed_2, SM_Cardboard_Closed_3, SM_Cardboard_Open_2, SM_Cardboard_Open_3

### `Meshes/Carpets` (4)
  SM_Carpet_01, SM_Carpet_Doormat_01, SM_Carpet_Runner_01, SM_Carpet_Runner_02

### `Meshes/CeilingLamp` (2)
  SM_CeilingLamp, SM_CeilingLamp_On

### `Meshes/Chimney` (3)
  SM_ChimneyTop, SM_Chimney_3m, SM_Chimney_Trim

### `Meshes/CoatRack` (4)
  SM_CoatRack, SM_CoatRackSmall, SM_CoatRack_DecoWood, SM_ShoeRack

### `Meshes/CoffeeMachine` (2)
  SM_CoffeeMachine, SM_CoffeeMachineJug

### `Meshes/Cookware` (5)
  SM_FryingPan, SM_Pot1, SM_Pot1Lid, SM_Pot2, SM_Pot2Lid

### `Meshes/Couch` (4)
  SM_Couch, SM_CouchCushion_01, SM_CouchCushion_02, SM_Couch_ArmChair

### `Meshes/Curtain` (5)
  SM_Curtain_01, SM_Curtain_01_01, SM_Curtain_02, SM_Curtain_02_01, SM_Curtain_Kitchen

### `Meshes/CurtainPlane` (2)
  SM_CurtainPlane_1m, SM_CurtainPlane_2m

### `Meshes/DVDPlayer` (2)
  SM_DVDPlayer, SM_TVRemote

### `Meshes/DeskLamp` (1)
  SM_DeskLamp_01

### `Meshes/Dining` (3)
  SM_DiningChair, SM_DiningTable, SM_RoundDiningTable

### `Meshes/DrainingBoard` (1)
  SM_DrainingBoard

### `Meshes/ElectricBox` (1)
  SM_ElectricBox

### `Meshes/ElectricMeter` (4)
  SM_ElectricMeter, SM_WeatherHead, SM_WeatherHeadPole_1m, SM_WeatherHeadPole_3m

### `Meshes/EntertainmentUnit` (2)
  SM_Shelves, SM_TVUnit

### `Meshes/FirePlace` (1)
  SM_FirePlace

### `Meshes/FlagPole` (1)
  SM_FlagPoleStatic

### `Meshes/Foliage/Bushes/Hedge` (7)
  SM_Hedge_4m, SM_Hedge_4m_180, SM_Hedge_4m_90, SM_Hedge_4m_Half, SM_Hedge_4m_Half_180, SM_Hedge_4m_Half_90, SM_Hedge_Sphere

### `Meshes/Foliage/Bushes/Shrub` (1)
  SM_Shrub01

### `Meshes/Foliage/Debris/MapleDeadLeaves` (7)
  SM_FallingLeaf01, SM_MapleDeadLeaves_01, SM_MapleDeadLeaves_02, SM_MapleDeadLeaves_03, SM_MapleDeadLeaves_Curb_01, SM_MapleDeadLeaves_Curb_02, SM_MapleDeadLeaves_Curb_03

### `Meshes/Foliage/Flowers/GardenDaisy` (2)
  SM_GardenDaisy01, SM_GardenDaisy02

### `Meshes/Foliage/Grass` (1)
  SM_GrassClump01

### `Meshes/Foliage/Grass/BarleyGrass` (5)
  SM_BarleyGrass01, SM_BarleyGrass02, SM_BarleyGrass03, SM_BarleyGrass04, SM_PavementGrass01

### `Meshes/Foliage/Trees` (1)
  SM_MapleTree_Pivot

### `Meshes/FridgeMagnets` (15)
  SM_FridgeMagnet01, SM_FridgeMagnet02, SM_FridgeMagnet03, SM_FridgeMagnet04, SM_FridgeMagnet05, SM_FridgeMagnet06, SM_FridgeMagnet07, SM_FridgeMagnet08, SM_Paper01, SM_Paper02, SM_Paper03, SM_Paper04, SM_Polaroid01, SM_Polaroid02, SM_Polaroid03

### `Meshes/FruitComputer` (2)
  SM_FruitComputer01, SM_FruitComputer02

### `Meshes/Furniture` (2)
  SM_Lamp_Stranding, SM_Lamp_Table

### `Meshes/GarageDoor` (3)
  SM_GarageDoor_Closed, SM_GarageDoor_Open, SM_GarageDoor_Track

### `Meshes/Gravel` (3)
  SM_Gravel_01, SM_Gravel_02, SM_Gravel_03

### `Meshes/Groceries/Drinks` (31)
  SM_BeerBottle01, SM_BeerBottle02, SM_BeerBottle03, SM_BeerBottle04, SM_BeerBottle05, SM_BeerBottle06, SM_BeerBottle07, SM_BeerBottle08, SM_Carton1, SM_Carton2, SM_Carton3, SM_DrinkCanBig_1, SM_DrinkCanBig_2, SM_DrinkCanBig_3, SM_DrinkCanBig_4, SM_DrinkCanBig_5, SM_DrinkCanSmall_1, SM_DrinkCanSmall_2, SM_DrinkCanSmall_3, SM_DrinkCanSmall_4, SM_DrinkCanSmall_5, SM_Soda01, SM_Soda02, SM_Soda03, SM_SodaSmall01, SM_SodaSmall02, SM_SodaSmall03, SM_Water01, SM_Water02, SM_WaterSmall01, SM_WaterSmall02

### `Meshes/HallwayCabinet` (3)
  SM_HallwayCabientDrawer01, SM_HallwayCabientDrawer02, SM_HallwayCabinet

### `Meshes/Hydrant` (1)
  SM_Hydrant

### `Meshes/Kitchen` (38)
  SM_CounterCorner, SM_CoverFull, SM_CupboardDoor1_L, SM_CupboardDoor1_R, SM_CupboardDoor2_L, SM_CupboardDoor2_R, SM_CupboardDoor2b_L, SM_CupboardDoor2b_R, SM_CupboardDoor3_L, SM_CupboardDoor3_R, SM_CupboardDoorSink_L, SM_CupboardDoorSink_R, SM_Cupboard_120, SM_Cupboard_120_CounterOnly, SM_Cupboard_120_Dishwasher, SM_Cupboard_120_DishwasherR, SM_Cupboard_120_L, SM_Cupboard_120_R, SM_Cupboard_60, SM_Cupboard_60_02, SM_Cupboard_60_L, SM_Drawer, SM_FridgeCupboard_R, SM_Island01, SM_Island02, SM_KitchenSink, SM_KitchenSinkPipes, SM_Pantry, SM_PantryFridgeCombo, SM_Sink_Cupboard_120, SM_TopCabinetCorner, SM_TopCabinetShelves_60, SM_TopCabinetSmall_120, SM_TopCabinet_120, SM_TopCabinet_120_EndL, SM_TopCabinet_120_EndR, SM_TopCabinet_60, SM_TopCabinet_60_L

### `Meshes/Kitchen/Dishwasher` (1)
  SM_Dishwasher

### `Meshes/Kitchen/Fridge` (5)
  SM_Fridge, SM_FridgeClosed, SM_FridgeDoorBottom, SM_FridgeDoorTop, SM_FridgeDrawer

### `Meshes/Kitchen/Stove` (2)
  SM_Stove, SM_StoveHood

### `Meshes/KitchenCeramics` (6)
  SM_Bowl1, SM_Bowl2, SM_Glass, SM_Mug, SM_Plate1, SM_Plate2

### `Meshes/KitchenClutter` (16)
  SM_KitchenRoll01, SM_KitchenRoll02, SM_KitchenRollHolder01, SM_KitchenRollHolder02, SM_Knife01, SM_Knife02, SM_Knife03, SM_Knife04, SM_Knife05, SM_KnifeBlock01, SM_KnifeBlock02, SM_OliveOil, SM_SpoonCup, SM_WashingLiquid, SM_WoodenSpatula, SM_WoodenSpoon

### `Meshes/Lamps` (3)
  SM_PendantCeilingLamp, SM_PendantCeilingLamp02, SM_WallLamp

### `Meshes/LargeTV` (2)
  SM_LargeTV, SM_LargeTV_WallMounted

### `Meshes/Laundry` (13)
  SM_ClothesPole, SM_CoatHanger, SM_Hoodie, SM_Hoodie_Hanger, SM_IroningBoard, SM_IroningBoard02, SM_LaundryHamper, SM_Laundry_01, SM_Pullover, SM_Pullover_Hanger, SM_TShirt, SM_TShirt_Hanger, SM_WashingBasket

### `Meshes/Magazines` (7)
  SM_Magazine00, SM_Magazine01, SM_Magazine02, SM_Magazine03, SM_Magazine04, SM_Magazine05, SM_Magazine06

### `Meshes/MailBox` (6)
  SM_MailBox, SM_MailBox02, SM_MailBox_Door, SM_MailBox_Flag, SM_MailBox_Main, SM_MailBox_Open

### `Meshes/ManholeCover` (1)
  SM_ManholeCover

### `Meshes/MetalTrashCan` (2)
  SM_MetalTrashCan, SM_MetalTrashCanLid

### `Meshes/Microwave` (1)
  SM_Microwave

### `Meshes/Mirror` (2)
  SM_Mirror1, SM_Mirror2

### `Meshes/Monitor` (2)
  SM_Monitor1, SM_Monitor2

### `Meshes/OfficeCarpet` (1)
  SM_Carpet

### `Meshes/OfficeDesk` (6)
  SM_Drawer01, SM_Drawer02, SM_OfficeDesk01, SM_OfficeDesk02, SM_OfficeDesk02_Shelves, SM_OfficeDesk_Full

### `Meshes/OfficeProps` (10)
  SM_FolderBox, SM_OfficeBox1, SM_OfficeBox1Closed, SM_OfficeBox1Lid, SM_OfficeBox2, SM_OfficeBox2Closed, SM_OfficeBox2Lid, SM_Paper, SM_PaperStack, SM_PaperStack2

### `Meshes/OldTV` (1)
  SM_OldTV

### `Meshes/OutdoorBench` (4)
  SM_OutdoorBench, SM_OutdoorBench_02, SM_OutdoorBench_03, SM_OutdoorBench_04

### `Meshes/OutdoorStorageBox` (1)
  SM_OutdoorStorageBox

### `Meshes/OutsideChair` (1)
  SM_OutsideChair

### `Meshes/OutsideLamp` (1)
  SM_OutsideLamp

### `Meshes/OutsideTable` (1)
  SM_OutsideTable

### `Meshes/PaintCans` (10)
  SM_PaintBrush, SM_PaintCanLarge01, SM_PaintCanLarge02, SM_PaintCanLargeLid, SM_PaintCanMedium01, SM_PaintCanMedium02, SM_PaintCanMediumLid, SM_PaintCanSmall01, SM_PaintCanSmall02, SM_PaintCanSmallLid

### `Meshes/Paths` (2)
  SM_PathBase, SM_Patio_4m

### `Meshes/Pictures` (10)
  SM_PictureFrame01, SM_PictureFrame02, SM_PictureFrame03, SM_PictureFrame04, SM_Picture_01, SM_Picture_02, SM_Picture_03, SM_Picture_04, SM_Picture_05, SM_Picture_06

### `Meshes/Plants` (3)
  SM_AglaonemaPlant, SM_CastIronPlant, SM_SnakePlant

### `Meshes/PlasticChair` (1)
  SM_PlasticChair

### `Meshes/Pots` (5)
  SM_Pot01, SM_Pot02, SM_Pot03, SM_Pot04, SM_Pot05

### `Meshes/Roads` (10)
  SM_CrossRoad, SM_DoubleDriveWay, SM_DriveWay, SM_DriveWayPath, SM_GrassGround, SM_Junction, SM_RoadCorner, SM_RoadEnd, SM_RoadStraight, SM_RoadStraight_Drain

### `Meshes/SatelliteDish` (5)
  SM_SatelliteDish01, SM_SatelliteDish02, SM_SatelliteDish03, SM_SatelliteDish_Arm, SM_SatelliteDish_Head

### `Meshes/Shelves` (2)
  SM_LadderShelf, SM_WallShelf

### `Meshes/Shoes` (2)
  SM_Boots_L, SM_Boots_R

### `Meshes/Sockets` (3)
  SM_LightSwitchOff, SM_LightSwitchOn, SM_PowerSocket

### `Meshes/SpeedBump` (5)
  SM_SpeedBump_10m, SM_SpeedBump_End01, SM_SpeedBump_End02, SM_SpeedBump_Mid01, SM_SpeedBump_Mid02

### `Meshes/Stone` (2)
  SM_Stone_01, SM_Stone_02

### `Meshes/StorageContainers` (8)
  SM_FabricOrganiser, SM_RoundStorage01, SM_RoundStorage02, SM_RoundStorage03, SM_RoundStorageLid01, SM_RoundStorageLid02, SM_RoundStorageLid03, SM_StorageBasket

### `Meshes/StorageShelf` (1)
  SM_StorageShelf

### `Meshes/StreetSigns` (20)
  SM_RoadClosedSign01, SM_RoadClosedSign02, SM_Sandbag, SM_Sign_Bike, SM_Sign_CrossRoad, SM_Sign_DoNotEnter, SM_Sign_NoCenterLine, SM_Sign_NoParking01, SM_Sign_NoParking02, SM_Sign_OneWay01, SM_Sign_OneWay02, SM_Sign_Residential, SM_Sign_Sign01, SM_Sign_Sign02, SM_Sign_Sign03, SM_Sign_SpeedLimit25, SM_Sign_SpeedLimit30, SM_Sign_SpeedLimit40, SM_Sign_StopSign, SM_Sign_TJunction

### `Meshes/TableFan` (5)
  SM_TableFan, SM_TableFan_SM_TableFan, SM_TableFan_SM_TableFan_Fan, SM_TableFan_SM_TableFan_Holder, SM_TableFan_SM_TableFan_Top

### `Meshes/TableSet01` (3)
  SM_CoffeeTable01, SM_EndTable01, SM_Table01

### `Meshes/Telephone` (3)
  SM_Handset, SM_Telephone, SM_TelephoneBase

### `Meshes/TrashBag` (4)
  SM_Trashbag_01, SM_Trashbag_02, SM_Trashbag_03, SM_Trashbag_04

### `Meshes/TrashBin` (1)
  SM_TrashBin

### `Meshes/Tv` (1)
  SM_TV

### `Meshes/Umbrella` (2)
  SM_Umbrella, SM_Umbrella_20

### `Meshes/UtilityPole` (3)
  SM_UtilityPole01, SM_UtilityPole02, SM_UtilityPole03

### `Meshes/Vases` (6)
  SM_Vase1, SM_Vase2, SM_Vase3, SM_Vase4, SM_Vase5, SM_Vase6

### `Meshes/VinylFence` (9)
  SM_VinylFenceGate, SM_VinylFenceGate_4m, SM_VinylFencePole, SM_VinylFence_1m, SM_VinylFence_1m_02, SM_VinylFence_2m, SM_VinylFence_2m_02, SM_VinylFence_4m, SM_VinylFence_4m_02

### `Meshes/WasherDryer` (2)
  SM_WasherFront, SM_WasherTop

### `Meshes/Wheeliebin` (2)
  SM_Wheeliebin, SM_WheeliebinLid

### `Meshes/WoodenFence` (4)
  SM_WoodenFence, SM_WoodenFence02, SM_WoodenFence03, SM_WoodenFence_Pole

---
## ATMOSPHERICHOUSE (подвал, двери, доп.модули, пропы)

Базовый путь: `/Game/AtmosphericHouse/...`


### `Meshes/Meshes_building` (157)
  SM_Ceiling_1x3, SM_Ceiling_2x3, SM_Ceiling_3x3, SM_Ceiling_3x3_broken_A, SM_Ceiling_3x3_broken_B, SM_Ceiling_3x3_broken_C, SM_Ceiling_4x3, SM_Ceiling_4x3_broken_A, SM_Ceiling_6x3, SM_Ceiling_6x3_broken_A, SM_Ceiling_6x3_stairs_L, SM_Ceiling_6x3_stairs_R, SM_Ceiling_6x6, SM_Ceiling_6x6_broken_A, SM_Ceiling_6x6_broken_B, SM_Doorframe_in_double, SM_Doorframe_in_single, SM_Doorframe_out_double, SM_Doorframe_out_single, SM_Doorframe_out_single_windows, SM_Floor_1x3, SM_Floor_2x3, SM_Floor_3x3, SM_Floor_3x3_broken_A, SM_Floor_3x3_broken_B, SM_Floor_4x3, SM_Floor_6x3, SM_Floor_6x3_broken_A, SM_Floor_6x3_stairs_L, SM_Floor_6x3_stairs_R, SM_Floor_6x6, SM_Foundation_1x, SM_Foundation_2x, SM_Foundation_3x, SM_Foundation_6x, SM_Gutter_pipe_1story, SM_Gutter_pipe_2story, SM_Gutter_pipe_abovePorch, SM_Porch_A_3x, SM_Porch_A_3x_broken_A, SM_Porch_A_3x_broken_B, SM_Porch_A_3x_broken_C, SM_Porch_A_3x_broken_D, SM_Porch_A_3x_corner, SM_Porch_A_3x_corner_in, SM_Porch_A_3x_end_L, SM_Porch_A_3x_end_R, SM_Porch_A_3x_end_entrance_L, SM_Porch_A_3x_end_entrance_R, SM_Porch_A_3x_entrance, SM_Porch_A_3x_entrance_broken_A, SM_Porch_A_3x_noColumn, SM_Porch_A_6x_entrance_separate, SM_Roof_A_12x, SM_Roof_A_12x_corner, SM_Roof_A_12x_corner_T, SM_Roof_A_12x_gable, SM_Roof_A_6x, SM_Roof_A_6x_corner, SM_Roof_A_6x_corner_T, SM_Roof_A_6x_gable, SM_Roof_A_9x, SM_Roof_A_9x_corner, SM_Roof_A_9x_corner_T, SM_Roof_A_9x_gable, SM_WallIn_A_1x, SM_WallIn_A_2x, SM_WallIn_A_2x_bent_A, SM_WallIn_A_3x, SM_WallIn_A_3x_bent_A, SM_WallIn_A_3x_door, SM_WallIn_A_3x_door_bent_A, SM_WallIn_A_3x_doordouble, SM_WallIn_A_3x_doordouble_bent_A, SM_WallIn_A_3x_recessed, SM_WallIn_A_3x_window, SM_WallIn_A_3x_window_bent_A, SM_WallIn_A_3x_windowdouble, SM_WallIn_A_3x_windowdouble_bent_A, SM_WallIn_A_3x_windowsmall, SM_WallIn_A_6x, SM_WallIn_A_6x_bent_A, SM_WallIn_A_6x_windowtriple, SM_WallIn_A_6x_windowtriple_bent_A, SM_WallIn_A_column_medium, SM_WallIn_A_column_small, SM_WallIn_A_columnhalf_large, SM_WallIn_A_columnhalf_medium, SM_WallIn_A_columnhalf_small, SM_WallIn_A_corner_outLarge, SM_WallIn_A_corner_outPatch, SM_WallIn_A_corner_outSmall, SM_WallIn_B_1x, SM_WallIn_B_2x, SM_WallIn_B_2x_bent_A, SM_WallIn_B_3x, SM_WallIn_B_3x_bent_A, SM_WallIn_B_3x_door, SM_WallIn_B_3x_door_bent_A, SM_WallIn_B_3x_doordouble, SM_WallIn_B_3x_doordouble_bent_A, SM_WallIn_B_3x_recessed, SM_WallIn_B_3x_window, SM_WallIn_B_3x_window_bent_A, SM_WallIn_B_3x_windowdouble, SM_WallIn_B_3x_windowdouble_bent_A, SM_WallIn_B_3x_windowsmall, SM_WallIn_B_6x, SM_WallIn_B_6x_bent_A, SM_WallIn_B_6x_windowtriple, SM_WallIn_B_6x_windowtriple_bent_A, SM_WallIn_B_column_medium, SM_WallIn_B_column_small, SM_WallIn_B_columnhalf_large, SM_WallIn_B_columnhalf_medium, SM_WallIn_B_columnhalf_small, SM_WallIn_B_corner_outLarge, SM_WallIn_B_corner_outPatch, SM_WallIn_B_corner_outSmall, SM_WallIn_C_1x, SM_WallIn_C_2x, SM_WallIn_C_3x, SM_WallIn_C_3x_door, SM_WallIn_C_3x_doordouble, SM_WallIn_C_6x, SM_WallIn_C_column_medium, SM_WallIn_C_column_small, SM_WallIn_C_columnhalf_large, SM_WallIn_C_columnhalf_medium, SM_WallIn_C_columnhalf_small, SM_WallIn_C_corner_outLarge, SM_WallIn_C_corner_outPatch, SM_WallIn_C_corner_outSmall, SM_WallOut_1x, SM_WallOut_2x, SM_WallOut_3x, SM_WallOut_3x_door, SM_WallOut_3x_doordouble, SM_WallOut_3x_window, SM_WallOut_3x_windowdouble, SM_WallOut_3x_windowsmall, SM_WallOut_6x, SM_WallOut_6x_windowtriple, SM_WallOut_corner, SM_Window_Emissive, SM_Window_double_static, SM_Window_double_vinyl, SM_Window_single_static, SM_Window_single_vinyl, SM_Window_single_vinyl_openable, SM_Window_singlesmall_vinyl, SM_Window_singlesmall_vinyl_openable, SM_Windowglass_doorframe_out_A, SM_Windowglass_doorframe_out_B, SM_Windowglass_single_static, SM_Windowglass_single_vinyl, SM_Windowglass_window_openable

### `Meshes/Meshes_building/Meshes_stairs` (26)
  SM_Basement_stairs_ceiling_wood, SM_Ceiling_stairs_basement_A, SM_Ceiling_stairs_basement_B, SM_Stairs_basement_steps, SM_Stairs_main_L_railing, SM_Stairs_main_L_railing_extra, SM_Stairs_main_R_railing, SM_Stairs_main_railing_R_extra, SM_Stairs_straight_L_railing, SM_Stairs_straight_L_railing_extra, SM_Stairs_straight_L_steps, SM_Stairs_straight_R_railing, SM_Stairs_straight_R_railing_extra, SM_Stairs_straight_R_steps, SM_WallIn_A_stairs_main_L_A, SM_WallIn_A_stairs_main_L_B, SM_WallIn_A_stairs_main_R_A, SM_WallIn_A_stairs_main_R_B, SM_WallIn_A_stairs_straight_L_A, SM_WallIn_A_stairs_straight_L_B, SM_WallIn_A_stairs_straight_R_A, SM_WallIn_A_stairs_straight_R_B, SM_WallIn_C_stairs_basement_A, SM_WallIn_C_stairs_basement_B, SM_stairs_main_L_steps, SM_stairs_main_R_steps

### `Meshes/Meshes_decals` (28)
  SM_Decal_damage_concrete_A, SM_Decal_damage_concrete_B, SM_Decal_damage_concrete_C, SM_Decal_damage_concrete_D, SM_Decal_damage_concrete_corner, SM_Decal_damage_crack_A, SM_Decal_damage_crack_B, SM_Decal_damage_crack_C, SM_Decal_damage_tiles, SM_Decal_damage_wood, SM_Decal_leaking_A, SM_Decal_leaking_B, SM_Decal_papers_A, SM_Decal_papers_B, SM_Decal_papers_C, SM_Decal_papers_D, SM_Decal_papers_E, SM_Decal_papers_F, SM_Decal_papers_G, SM_Decal_papers_H, SM_Decal_plaster_floor_A, SM_Decal_plaster_floor_B, SM_Decal_plaster_floor_C, SM_Decal_plaster_floor_D, SM_Decal_plaster_floor_E, SM_Decal_plaster_floor_F, SM_Decal_plaster_floor_G, SM_Decal_plaster_floor_H

### `Meshes/Meshes_decals/Decals_corner` (27)
  SM_Decal_wallIn_A_3x_door, SM_Decal_wallIn_A_3x_doordouble, SM_Decal_wallIn_A_column_medium, SM_Decal_wallIn_A_column_small, SM_Decal_wallIn_A_columnhalf_large, SM_Decal_wallIn_A_columnhalf_medium, SM_Decal_wallIn_A_columnhalf_small, SM_Decal_wallIn_A_corner_outLarge, SM_Decal_wallIn_A_corner_outPatch, SM_Decal_wallIn_A_corner_outSmall, SM_Decal_wallIn_B_3x_door, SM_Decal_wallIn_B_3x_doordouble, SM_Decal_wallIn_B_column_medium, SM_Decal_wallIn_B_column_small, SM_Decal_wallIn_B_columnhalf_large, SM_Decal_wallIn_B_columnhalf_medium, SM_Decal_wallIn_B_columnhalf_small, SM_Decal_wallIn_B_corner_outLarge, SM_Decal_wallIn_B_corner_outPatch, SM_Decal_wallIn_B_corner_outSmall, SM_Decal_wallIn_C_column_medium, SM_Decal_wallIn_C_column_small, SM_Decal_wallIn_C_columnhalf_large, SM_Decal_wallIn_C_columnhalf_medium, SM_Decal_wallIn_C_columnhalf_small, SM_Decal_wallIn_C_corner_outLarge, SM_Decal_wallIn_C_corner_outSmall

### `Meshes/Meshes_doors` (10)
  SM_Door_mesh_A_L, SM_Door_mesh_A_R, SM_Door_mesh_B_L, SM_Door_mesh_B_R, SM_Door_mesh_C_L, SM_Door_mesh_C_R, SM_Door_mesh_sliding, SM_Windowglass_door_B, SM_Windowglass_door_C, SM_Windowglass_door_sliding

### `Meshes/Meshes_props/Basement_props` (24)
  SM_Barrel, SM_Box_container_A, SM_Box_container_B, SM_Box_container_C, SM_Cardboard_box_A, SM_Cardboard_box_B, SM_Cardboard_box_C, SM_Cardboard_box_D, SM_Cardboard_box_E, SM_Metal_box, SM_Paint_bucket_A, SM_Paint_bucket_B, SM_Paint_bucket_C, SM_Paint_bucket_D, SM_Paint_set_A, SM_Paint_set_B, SM_Paint_spray_A, SM_Paint_spray_B, SM_Pallet, SM_Plank_set_A, SM_Plank_set_B, SM_Shelf_wooden_standing, SM_Table_wooden, SM_Wooden_crate

### `Meshes/Meshes_props/Bathroom_props` (19)
  SM_Bath_soap_holder, SM_Bathroom_cabinet, SM_Bathroom_cabinet_door, SM_Bathroom_mirror, SM_Bathroom_sink, SM_Bathroom_sink_door_L, SM_Bathroom_sink_door_R, SM_Bathroom_sink_double, SM_Bathtub, SM_Hand_towel, SM_Shower_curtain, SM_Shower_curtain_rail, SM_Toilet, SM_Toilet_lid, SM_Toiletpaper, SM_Toiletpaper_holder, SM_Toiletpaper_set, SM_Towel_holder, SM_Trashcan

### `Meshes/Meshes_props/Books` (32)
  SM_Book_A, SM_Book_B, SM_Book_C, SM_Book_D, SM_Book_E, SM_Book_F, SM_Book_G, SM_Book_H, SM_Book_I, SM_Book_J, SM_Book_K, SM_Book_L, SM_Book_M, SM_Book_N, SM_Book_O, SM_Book_Open, SM_Book_set_A, SM_Book_set_B, SM_Book_set_C, SM_Book_set_D, SM_Book_set_E, SM_Book_set_F, SM_Book_set_G, SM_Book_set_H, SM_Book_set_I, SM_Book_set_J, SM_Book_set_bookshelf_A, SM_Book_set_bookshelf_B, SM_Book_set_bookshelf_C, SM_Book_set_bookshelf_D, SM_Book_set_floor_A, SM_Book_set_floor_B

### `Meshes/Meshes_props/Carpets` (7)
  SM_Carpet_A, SM_Carpet_B, SM_Carpet_C, SM_Carpet_D, SM_Carpet_E, SM_Carpet_persian_A, SM_Carpet_persian_B

### `Meshes/Meshes_props/Clothes` (18)
  SM_Cloth_hanger, SM_Clothes_rack, SM_Collared_shirt_A, SM_Collared_shirt_B, SM_Drying_rod, SM_Jeans_folded, SM_Laundry_basket_full_A, SM_Laundry_basket_full_B, SM_Rag_A, SM_Rag_B, SM_T-shirt, SM_T-shirt_folded, SM_T-shirt_folded_pile, SM_Towel_folded_A, SM_Towel_folded_A_pile, SM_Towel_folded_B, SM_Towel_folded_B_pile, SM_Towel_hanging

### `Meshes/Meshes_props/Curtains` (7)
  SM_Curtain_long_A, SM_Curtain_long_B, SM_Curtain_rod_doublewindow, SM_Curtain_rod_singlewindow, SM_Curtain_rod_triplewindow, SM_Curtain_short_A, SM_Curtain_short_B

### `Meshes/Meshes_props/Electronics` (17)
  SM_Computer_central_unit, SM_Computer_keyboard, SM_Computer_mouse, SM_Computer_mousepad, SM_Computer_screen, SM_Dryer, SM_Fridge_Body, SM_Fridge_door_L, SM_Fridge_door_R, SM_Fridge_door_freezer, SM_Oven_door, SM_Stove, SM_Stove_range_hood, SM_TV_off, SM_TV_on, SM_Tv_remote, SM_Washing_machine

### `Meshes/Meshes_props/Extras` (23)
  SM_Ceiling_3x_trim, SM_Ceiling_wood_3x3, SM_Ceiling_wood_3x_single, SM_Ceiling_wood_9x3, SM_Ceiling_wood_pilar, SM_Fireplace, SM_Floor_trim_door, SM_Floor_trim_doordouble, SM_Light_switch, SM_Pipe_1x, SM_Pipe_3x, SM_Pipe_3x_valve, SM_Pipe_corner_L, SM_Pipe_corner_R, SM_Pipe_corner_T, SM_Pipe_end, SM_Pipe_vertical, SM_Power_socket, SM_Roof_corner_overlap_fix, SM_Windowblinds_closed, SM_Windowblinds_open_A, SM_Windowblinds_open_B, SM_Windowblinds_open_C

### `Meshes/Meshes_props/Furniture` (69)
  SM_Bed_king, SM_Bed_twin, SM_Bookcase_A, SM_Bookcase_B, SM_Bookshelf, SM_Bookshelf_6x, SM_Bookshelf_Door_L, SM_Bookshelf_Door_R, SM_Bookshelf_fireplace, SM_Bookshelf_mounted, SM_Cabinet_A, SM_Cabinet_B, SM_Cabinet_B_door_L, SM_Cabinet_B_door_R, SM_Cabinet_C, SM_Cabinet_C_door, SM_Cabinet_D, SM_Cabinet_D_door_L, SM_Cabinet_D_door_L_windowglass, SM_Cabinet_D_door_R, SM_Cabinet_D_door_R_windowglass, SM_Cabinet_D_windowglass, SM_Chair_barstool, SM_Chair_kitchen, SM_Chair_wooden, SM_Dresser_A, SM_Dresser_A_drawer_A, SM_Dresser_A_drawer_B, SM_Dresser_B, SM_Dresser_B_drawer, SM_Dresser_C, SM_Dresser_C_drawer, SM_Pillow_A, SM_Pillow_B, SM_Pillow_C, SM_Shelf, SM_Shelf_closet, SM_Shelf_metal_single, SM_Shelf_metal_standing, SM_Shelf_short, SM_Shelf_standing, SM_Side_table_A, SM_Side_table_A_drawer, SM_Side_table_B, SM_Side_table_B_drawer, SM_Side_table_C, SM_Side_table_C_drawer, SM_Side_table_D, SM_Side_table_D_Door_L, SM_Side_table_D_Door_R, SM_Side_table_E, SM_Side_table_F, SM_Side_table_G, SM_Sofa_1seat, SM_Sofa_2seat, SM_Sofa_3seat, SM_Table_A, SM_Table_A_tablecloth, SM_Table_B, SM_Table_coffee, SM_Wardrobe, SM_Wardrobe_door, SM_Wardrobe_drawer, SM_Workdesk, SM_Workdesk_drawer_A, SM_Workdesk_drawer_B, SM_Workdesk_extension, SM_Workdesk_extension_shelf, SM_Workdesk_shelf_A

### `Meshes/Meshes_props/Girl_room_props` (22)
  SM_Girl_box_A, SM_Girl_box_B_closed, SM_Girl_box_B_lid, SM_Girl_box_B_open, SM_Girl_poster_A, SM_Girl_poster_B, SM_Girl_poster_C, SM_Girl_poster_D, SM_Girl_poster_E, SM_Girl_poster_F, SM_Girl_poster_G, SM_Girl_poster_H, SM_Girl_poster_I, SM_Girl_poster_J, SM_Girl_poster_K, SM_Girl_poster_L, SM_Girl_poster_M, SM_Girl_poster_N, SM_Magazine_animals, SM_Post_it_set_A, SM_Post_it_set_B, SM_Stool_dressing

### `Meshes/Meshes_props/Interior_plants` (3)
  SM_Interior_plant_A, SM_Interior_plant_B, SM_Interior_plant_C

### `Meshes/Meshes_props/Kitchen_cabinets` (22)
  SM_Kitchen_cabinet_A, SM_Kitchen_cabinet_C, SM_Kitchen_cabinet_corner, SM_Kitchen_cabinet_door_A_L, SM_Kitchen_cabinet_door_A_R, SM_Kitchen_cabinet_door_B_L, SM_Kitchen_cabinet_door_B_R, SM_Kitchen_cabinet_door_C_L, SM_Kitchen_cabinet_door_C_R, SM_Kitchen_cabinet_end_L, SM_Kitchen_cabinet_end_R, SM_Kitchen_cabinet_shelf_L, SM_Kitchen_cabinet_shelf_R, SM_Kitchen_counter_A, SM_Kitchen_counter_B, SM_Kitchen_counter_corner, SM_Kitchen_counter_dishwasher, SM_Kitchen_counter_door_L, SM_Kitchen_counter_door_R, SM_Kitchen_counter_drawer, SM_Kitchen_counter_separate, SM_Kitchen_counter_sink

### `Meshes/Meshes_props/Kitchen_supplies` (35)
  SM_Beer_bottle_closed, SM_Beer_bottle_open, SM_Beer_can, SM_Beer_set_A, SM_Bottle, SM_Bottle_plastic, SM_Bowl, SM_Bowl_set, SM_Coffee_mug, SM_Cutlery_fork, SM_Cutlery_holder, SM_Cutlery_hooks, SM_Cutlery_knife, SM_Cutting_board, SM_Dish_soap, SM_Drying_cloth, SM_Food_box, SM_Frying_pan, SM_Jar_A, SM_Jar_B, SM_Jar_C, SM_Jar_D, SM_Jar_E, SM_Jar_F, SM_Jar_G, SM_Pizza_box, SM_Plate, SM_Plate_dirty, SM_Plate_set, SM_Plate_spaghetti, SM_Pot, SM_Pot_lid, SM_Spatula, SM_Spoon, SM_Trashcan_large

### `Meshes/Meshes_props/Lamps` (33)
  SM_CeilingFan_Base, SM_CeilingFan_Blades, SM_CeilingFan_Lamps, SM_Lamp_bathroom_wall, SM_Lamp_ceiling_ball_ball, SM_Lamp_ceiling_ball_base, SM_Lamp_ceiling_circle, SM_Lamp_ceiling_cone_A, SM_Lamp_ceiling_cone_B, SM_Lamp_ceiling_fluorescent, SM_Lamp_ceiling_round, SM_Lamp_ceiling_spot_base, SM_Lamp_ceiling_spot_spot_A, SM_Lamp_ceiling_spot_spot_B, SM_Lamp_ceiling_spot_spot_C, SM_Lamp_cellar_wall, SM_Lamp_chandelier_body, SM_Lamp_chandelier_shade_A, SM_Lamp_chandelier_shade_B, SM_Lamp_chandelier_shade_C, SM_Lamp_chandelier_shade_D, SM_Lamp_chandelier_shade_E, SM_Lamp_exterior_wall_Bulb, SM_Lamp_exterior_wall_Glass, SM_Lamp_exterior_wall_base, SM_Lamp_hanging_fluorescent, SM_Lamp_standing, SM_Lamp_table_shade_body, SM_Lamp_table_shade_shade, SM_Lamp_table_work, SM_Lamp_wall_fluorescent, SM_Lamp_wall_shade_body, SM_Lamp_wall_shade_shade

### `Meshes/Meshes_props/Office_props` (21)
  SM_Diary, SM_Folder_A, SM_Folder_B, SM_Folder_C, SM_Folder_D, SM_Folder_set_A, SM_Folder_set_B, SM_Folder_set_C, SM_Folder_set_D, SM_Office_box_A, SM_Office_box_B, SM_Office_cardboard_box_A, SM_Office_cardboard_box_B, SM_Papers_A, SM_Papers_B, SM_Papers_pile_A, SM_Papers_pile_B, SM_Post_it_A, SM_Post_it_B, SM_Post_it_C, SM_Post_it_D

### `Meshes/Meshes_props/Paintings` (19)
  SM_Painting_A, SM_Painting_B, SM_Painting_C, SM_Painting_D, SM_Painting_E, SM_Painting_F, SM_Painting_G, SM_Painting_H, SM_Painting_I, SM_Painting_J, SM_Painting_K, SM_Painting_L, SM_Painting_M, SM_Painting_N, SM_Painting_O, SM_Painting_P, SM_Painting_Q, SM_Painting_R, SM_Painting_S

### `Meshes/Meshes_props/Small_table_props` (35)
  SM_Candelabra, SM_China_bowl, SM_China_plate, SM_China_set_A, SM_China_set_B, SM_China_set_C, SM_China_vase, SM_Hair_brush, SM_Jar_ceramic, SM_Lotion_set_A, SM_Lotionbottle_A, SM_Lotionbottle_B, SM_Lotionbottle_C, SM_Lotionbottle_D, SM_Lotionbottle_E, SM_Lotionbottle_F, SM_Lotiontube_A, SM_Lotiontube_B, SM_Magazine_A, SM_Magazine_B, SM_Makeup_bag, SM_Notebook, SM_Painkillers_A, SM_Painkillers_B, SM_Painting_table_A, SM_Painting_table_B, SM_Painting_table_C, SM_Painting_table_D, SM_Painting_table_E, SM_Painting_table_F, SM_Painting_table_G, SM_Pencil_holder, SM_Pencil_post_it, SM_Pencils, SM_Tissue_box

### `Meshes/Meshes_props/Storage_props` (24)
  SM_Bag_plastic_A, SM_Bag_plastic_B, SM_Bucket, SM_Can_plastic_A, SM_Can_plastic_B, SM_Can_plastic_C, SM_Can_powder, SM_Laundry_basket_A, SM_Laundry_basket_B, SM_Laundry_basket_C, SM_Laundry_basket_hanging, SM_Laundry_basket_holder, SM_Spray, SM_Storage_box_A, SM_Storage_box_A_lid, SM_Storage_box_B_closed, SM_Storage_box_B_open, SM_Storage_box_C, SM_Storage_box_cardboard_A, SM_Storage_box_cardboard_B, SM_Storage_box_wicker, SM_Storage_plastic_box_A, SM_Storage_prop_set_A, SM_Storage_prop_set_B

---

## Source: PLUGINS_REFERENCE.md

# PLUGINS_REFERENCE — плагины проекта «Аварийка»: что за что отвечает

> Составлено 2026-06-21 по `avariika.uproject` + манифестам `Plugins/*.uplugin` + движковым маркетплейс-плагинам.
> Engine 5.7.4. `Plugins/` — в `.gitignore` (локально, re-copy из RawAssets/FAB при свежем клоне).

---

## 1. Включённые в `avariika.uproject` (12)

### Встроенные UE (engine), просто включены
| Плагин | Зачем в проекте |
|---|---|
| **ModelingToolsEditorMode** | режим Modeling Tools в редакторе (правка мешей, blockout). Editor-only |
| **GameplayStateTree** | StateTree — логика поведения (AI/гейм-стейты); под будущего монстра/NPC |
| **OnlineSubsystem** + **OnlineSubsystemNull** + **OnlineSubsystemUtils** | каркас сетевого кооп; **Null** = локальный/LAN-бэкенд (когда EOS не активен) |
| **JsonBlueprintUtilities** | парсинг/сборка JSON в блупринтах |

### Сторонние / маркетплейс (ядро рабочего процесса)
| Плагин | Версия / автор | Зачем | Гочи / память |
|---|---|---|---|
| **Claudius** | v3.0/3.1 · Claudius Code | **AI-мост**: правлю UE из Claude JSON-командами (HTTP `127.0.0.1:8080`, ~230 команд, 26 категорий, кураторский индекс). Ядро автономной работы | ⚠️ порт **8080**, не 8765 (память `claudius-helper-script`); null-byte воркэраунд парс-бага. **ДУБЛЬ в движке** — см. §3 |
| **meshy** | v0.2.0 · wyn | **генерация 3D-моделей** (мебель/пропсы) через meshy.ai по тексту/картинке. «Генерация мебели» из твоего вопроса = это | Editor-only; модели генерит пользователь по промптам Claude (память `avariika-game-concept`) |
| **EOSCore** | FAB · af157f77 | **Epic Online Services** бэкенд для кооп (лобби/сессии/друзья) | ключи в `DefaultEngine.ini`; `DefaultPlatformService` пока Null; **ротировать Client Secret если репо откроют** (память `eos-plugins-batch`) |
| **WorldBLD** | v1.3.9 · WorldBLD LLC | генерация **города/дорог/окружения** (карта Завод/улица); 3 C++-модуля | ODR-фикс `GetCreditsValueFont` для сборки на 5.7.4 (память `worldbld-plugin`); локальный |
| **TickOptToolkit** | v1.5 · loonyware | **оптимизация тиков** актёров/компонентов по дистанции/видимости | установлен, **ещё не применён** — кандидаты в `OPTIMIZATION_REPORT.md` §5 |
| **Imperfecter** | v1.3.1 · Hubert Mika | пост-процесс «несовершенства камеры» (зерно/аберрация/виньетка) — **хоррор-атмосфера** | установлен, пресет ещё не настроен |
| **AudioToolkitPro** | v1.2 · UMonster | проф. редактирование аудио в редакторе | Editor-only |
| **UnrealMonsterAssetInstaller** | Editor · UMonster | инсталлер ассетов **монстра** | gated — **монстра не делать без отмашки** (память `project-status`) |

---

## 2. Лежат в `Plugins/`, но НЕ в списке `uproject` (AI-мосты — проверить статус)

| Плагин | Что | Замечание |
|---|---|---|
| **UnrealMCPServer57** | v2.0.2 · StraySpark · MCP-сервер UE5.7, **305 тулзов** по JSON-RPC/HTTP | `mcp__unreal__*` — живой канал (память `mcp-servers`). `EnabledByDefault:true` в манифесте → работает без записи в uproject |
| **NwiroIntegrationKit** | v1.0.3 · Nwiro (support Leartes) · **MCP-мост** Claude Code/Codex/Cursor/Windsurf ↔ UE5 | ещё один AI-мост; в uproject не прописан |

⚠️ **Итого 4 AI-моста**: Claudius (проектный) + Claudius (движковый дубль) + UnrealMCPServer + NwiroIntegrationKit.
Избыточно — стоит оставить 1–2 рабочих (Claudius на 8080 + UnrealMCP), остальные выключить, чтобы не плодить
порты/тики редактора. На решение пользователя.

---

## 3. ⚠️ Дубль Claudius в движке

В `C:\Program Files\Epic Games\UE_5.7\Engine\Plugins\Marketplace\Untitledca7e22d4545aV1\` установлен
**второй Claudius** (`Claudius.uplugin`, v3.1.0, «External Automation Framework»). То есть Claudius стоит
дважды: проектный `Plugins/Claudius` + движковый. Два экземпляра одного editor-плагина → возможный конфликт
(двойная регистрация команд/портов). Рекомендация: оставить ОДИН (проектный), движковый отключить/удалить.
Папка с безымянным GUID (`Untitledca7e22d4545aV1`) — артефакт установки с FAB без имени.

Движковые маркетплейс-плагины (для справки): `FabLauncher` (магазин FAB) + этот безымянный Claudius.

---

## 4. Скачано в `RawAssets/`, но НЕ установлено в проект

Тулкиты/паки на полке (вердикты — память `obshchee-asset-catalog`, WORKLOG):

| В RawAssets | Что | Под что |
|---|---|---|
| **Level Design Tools — DrCG Assistant** | Editor-виджет: Align/Distribute/Scatter/Array/Randomize | **расстановка мебели/объектов** (твоя ручная стройка) — НЕ установлен |
| **Hyper Procedural Spline Toolkit v4** | кабели/трубы по сплайну (5 ГБ) | провода/трубы дома (`ATMOSPHERE_PLAN.md` §3) |
| **Next Gen Destruction Toolkit** | разрушение мешей (Geometry Collection) | взрыв газа рушит стену (gated по EffectsQuality) |
| **Flashlight** | фонарь-в-руке | старт-инструмент → каска-апгрейд |
| **InteractionToolkit** | взаимодействие (референс) | образец для нашей E-механики |
| **Boss AI Toolkit 5.3** + **слухач** | AI босса + монстр-слухач | **монстр — отложен/gated** |
| **Luoss Particle Toolkit Vol 1** | VFX-партиклы | эффекты аварий (резерв) |
| **Multiplayer FPS Procedural Animation Toolkit 5.0** | проц-анимация рук/оружия FPS | апгрейд от 1-го лица (нужен ребилд, 5.0) |
| **Pro HUD Pack V2 / Survival_UI_Kit / Easy Game UI Ultimate** | HUD/UI-наборы | переход HUD на UMG (задумка WORKLOG) |
| **TranslationToolkit** | editor-перевод (4.27) | НЕ рантайм-локализация |
| **TOT57 / Imperfecter57 / WorldBLD_1.3.9_UE57** | исходники уже установленных | бэкап под re-copy |
| **[SCANS] Warehouse** | исходник пака Warehouse | re-copy фабрики |
| **звуки / anims_incoming** | сырые WAV / FBX-анимы | курация под механики |

> «Interior Toolkit» (спец. меблировка интерьеров ⭐) — в списках `СКАЧАТЬ/DOWNLOADS/SHOPPING.md`, но **ещё не скачан**.

---

## 5. Карта «что чем делаю»

- **Править UE из Claude** → Claudius (8080) / UnrealMCP (`mcp__unreal__*`).
- **Генерить мебель/пропсы** → meshy (по промпту) → импорт скриптом.
- **Расставлять объекты** → вручную DrCG-виджетом (после установки) ИЛИ скриптом `level.spawn_actor` (Claude).
- **Кооп-сеть** → EOSCore (+ OnlineSubsystem Null как фолбэк).
- **Окружение Завод/улица** → WorldBLD.
- **Атмосфера-камера** → Imperfecter; **перф** → TickOptToolkit.
- **Монстр** → UnrealMonsterAssetInstaller + Boss AI + слухач — **только по отмашке**.

_Связано: `mcp-servers`, `claudius-helper-script`, `eos-plugins-batch`, `worldbld-plugin`, `obshchee-asset-catalog`._

---

## Source: RAWASSETS_TRIAGE.md

# RAWASSETS — ТРИАЖ (отчёт за ночь 2026-06-24)

> Разобраны **все 60 папок** в `RawAssets/` (один инспектор на категорию, осмотр на диске + сверка с `СКАЧАТЬ.md`, уже установленным и движком 5.7.4).
> Суммарно на диске: **92.5 ГБ**. Из них «реальный вес импорта» в разы меньше — почти всё это сырой звук (37 ГБ), демо-проекты и `Intermediate/` (см. §⚠️).
>
> **Я НИЧЕГО не импортировал и не коммитил** — это осознанно: плагины требуют ребилда под 5.7.4 (риск сломать сборку, пока ты спишь), контент 5.0–5.6 требует resave в редакторе, паки в основном пиратские-плейсхолдеры, и 8 паков требуют твоего решения. Всё разложено по группам — скажешь «гони группу N», и я выполню за минуты. Зипы (Fuse Box, Stairs Set) уже **распаковал** — это безвредно.

## Итог одним взглядом

| Вердикт | Кол-во | Что значит |
|---|---:|---|
| ⬇️ **IMPORT_NOW** | 21 | прямое попадание в потребность, тащим |
| ✂️ **CHERRY_PICK** | 18 | брать срез, не весь пак |
| ❓ **РЕШЕНИЕ ЗА ТОБОЙ** | 8 | спорные — мой совет внутри |
| ❌ **SKIP** | 6 | дубль/против направления/не наш жанр |
| ✅ **УЖЕ ЕСТЬ** | 4 | бэкапы установленного, не переставлять |
| 👹 **МОНСТР — ПОТОМ** | 3 | заблокировано правилом «монстра не делать» |

---

## ⬇️ IMPORT_NOW — тащим (21)

### Плагины-ядро
| Пак | Что даёт | Версия | Установка | Риск |
|---|---|---|---|---|
| **NVIDIA DLSS 4** (`2025.12.09_UE5.7_DLSS4Plugin`) | ⭐апскейл/перф (Завод 21 ГБ будет лагать без него) | 5.7.0 native, **prebuilt** | копировать только под-плагины `DLSS/NIS/Streamline*` в `Plugins/`, выкинуть Samples/WinGDK/Docs | низкий; Frame Gen только RTX 40+ |
| **Cross-Platform Voice Chat** (`CrossPla…`) | ⭐бэкбон рации-кооп (`SPEC_Radio_Haggle`) | 5.7.0, но есть `Source/` | `Plugins/UniversalVoiceChatPro` → **ребилд** | средний (Build.bat) |
| **Narrative 3** (`Narrative57`) | ⭐диспетчер квестов/целей + диалоги (каркас миссии) | 5.7.0, `Source/` 3 модуля | `Plugins/Narrative` → **ребилд** (выкинуть Binaries/Intermediate) | средний (Build.bat) |
| **Runtime Audio Importer** | ⭐основа офлайн-голоса (рация-торг) | 5.7.0 **prebuilt** | `Plugins/RuntimeAudioImporter` | низкий |
| **Runtime Speech Recognizer** (Whisper) | ⭐STT — интенты игрока по рации | 5.7.0 **prebuilt** | копировать **вложенную** `…57/Plugins/RuntimeSpeechRecognizer` | низкий; зависит от RAI |
| **Runtime Text To Speech** (Piper) | ⭐голос диспетчера | 5.7.0 **prebuilt** | копировать вложенную `…57/Plugins/RuntimeTextToSpeech` | низкий; зависит от RAI |
| **Ultimate Level Art Tool** (`ULAT_5.7`) | ⭐рабочая лошадь дрессинга всех 3 карт (модули/сплайны/скаттер/материалы) | 5.7.0 **prebuilt** | `Plugins/UltimateLevelArtTool` | низкий |
| **Blockout Tools** | ⭐грейбокс Завода/Больницы до арт-пасса | 5.7.0 **prebuilt** | `Plugins/BlockoutToolsPlugin` | низкий |
| **Triangle Count View Mode** | дев: тепловая карта плотности треугольников (под разруху) | 5.7.0 **prebuilt** | `Plugins/TriangleCountViewMode` (переименовать из обфусц. имени) | низкий |

> Офлайн-голос ставить **тройкой** (RAI+STT+TTS) — STT/TTS зависят от Audio Importer.

### Контент (robocopy / FBX-импорт)
| Пак | Что даёт | Версия | Куда |
|---|---|---|---|
| **DR7 = Decals Rusty Metal Vol.7** | ⭐ржавчина/коррозия — топ-дрессинг разрухи на всех картах | content-only | `Content/Decals_Rusty_metal_Vol_7` |
| **Horror and Decay Vol.5** (`HorrorAndDecay_VOL5`) | ⭐гнилые/битые меши+материалы под разруху | 5.3 content | `Content/DEKO_HorrorAndDecay5` (только `Content/`) |
| **Industrial Fuse Box** ✅распакован | ⭐щиток — ключевой объект починки (электро-станция/дуга) | свежий content | `Content/FuseBoxPack` |
| **Electrical set / lamps / wires** | вторичные щитки/розетки/провода/лампы — дрессинг электрики | ≤5.4 content | `Content/ElectricPack` |
| **Bags, Backpack & Survival Kit** | ⭐снаряжение бригады (рюкзаки/сумки) + клаттер | 4.27 content | `Content/Bags_Backpack_Survival_Kit` |
| **Modular Sewers & Tunnels** | ⭐подвал Дома/тоннели Завода + куча trash-пропсов (разруха) | 5.3 content | `Content/Sewer` (выкинуть Demo/ThirdPerson/External) |
| **Sundries — Icon Pack** | ⭐190 иконок слотов инвентаря (готовые текстуры) | текстуры | `Content/Sundries` (набор 512) |
| **Ultimate Fire Pack Vol.1** | ⭐пожар на статусе «Горит» (Niagara-вариант) | content | `Content/UltimateFirePackVol1` (только Niagara, не Cascade) |
| **Sonomar: Abandoned Asylum** | ⭐хоррор-эмбиент (Больница/заброшенный Дом) | сырой WAV 15 ГБ | `Content/SFX/Sonomar_Asylum` (**курир. подмножество**, не все 112) |
| **Foley Props Sound FX** | ⭐фоли предметов: Tools/Containers/Keys/Craft/спрей → починка | 4.27 `.uasset` 280 | `Content/SFX/FoleyProps` |
| **anims_incoming** (FBX) | Injured&Exhausted (диегетич. HP/выносл.), Getting-Up (нокдаун), Game Anim Sample (локомоция Quantum), эмоции | FBX (версионно-агностич.) | **импорт** в `Content/Avariika/Anim` на Quantum-скелет |

> `Smart Measure` (дев-линейка, BP, 3 МБ) — тоже IMPORT_NOW, но дев-инструмент → `Content/SmartMeasure`.

---

## ✂️ CHERRY_PICK — брать срез (18)

| Пак | Брать | Выбросить | Приоритет |
|---|---|---|---|
| **Flashlight** | Meshes/FX/Audios/Blueprints/Materials/Animations — **фонарь обязателен хоррору** | весь `Demo/` (StarterContent, UE4-маннекен) | 🔥высокий |
| **Poison Magic Niagara** | `NS_Posion_Magic_Area/Aura/Flow` → зелёное газ-облако | проджектайлы/лучи/орбы, демо-персонажи, ThirdPerson | высокий (закрывает газ-VFX) |
| **Survival UI Kit** | текстуры HUD + рамки инвентаря (под диегетику) | оружейная кастомизация (нет пушек), полоски | высокий (скин HUD) |
| **Ghosthack x Boom** (22 ГБ!) | Explosives (газ/взрыв), Sci-Fi Horror FX&Foley, Fighting Foley (импакты) | magic/medieval/steampunk/sci-fi-guns | средний |
| **Hyper Mesh to Icon Creator** | только модуль `Icon_Creator`(+Core) — дев-печь иконок, прогнать раз | фреймворк Hyper (Locomotion/MainMenu) | средний (дев) |
| **Lightning & Electric Alembics** (1.4 ГБ) | 2–3 болта Straight/Cylinder — **только под скриптовую дугу/катсцену** | остальные 37 (alembic = не драйвится как Niagara) | средний |
| **Easy Game UI Ultimate** | `EasyInputPrompts` (глифы клавиш), м.б. `EasySaveGameUI` | `EasyOptionsMenu` (УЖЕ стоит — не дублировать!), PhotoMode/Credits | средний |
| **Horror Amplifier** | отличительные стинги/дроны (папка `5.3`) | дубли с Sonomar | средний |
| **GothicTexture** | пара материалов под стены Больницы/декей | демо-карта | низкий |
| **Hitreact Pro** | компонент+directional (богаче) — **ИЛИ оставить JKMotion (уже стоит)** | ThirdPerson/демо | низкий — **выбрать одно**, склоняюсь оставить JKMotion |
| **FlexPath** | только подпапку `5.7` — сглаживание AI-нав | папки 5.5/5.6 | низкий (отложить до AI) |
| **PT5.7 ProInstanceTools** | instanced-клаттер разрухи | — | низкий (дубль скаттера ULAT) |
| **Hyper Procedural Spline** (5 ГБ) | `Core`+`Environment_Building` (PCG-дороги/заборы) | 5 ГБ `ResourcePack` | низкий (дубль ULAT AutoSpline) |
| **Luoss Particle Toolkit** | только если найдётся уникальный луп пара/искр | иначе SKIP (полка VFX переполнена) | низкий |
| **Stairs Set** ✅распакован | аним лестничной локомоции (UE5-mann ≈ Quantum) | — | низкий (отложить до блендспейса) |
| **Modular Staircase BP** | параметрич. лестница, если не хватит AtmosphericHouse | демо-карта/тесты | низкий |
| **Funeral Animations** | kneel/mourn под скриптовую сцену | демо-маннекен/карта | низкий |
| **звуки** (Rope Creak) | 1 семпл скрипа верёвки/конструкции | — | тривиально (можно SKIP) |

---

## ❓ РЕШЕНИЕ ЗА ТОБОЙ (8) — мой совет внутри

| Пак | Суть | Мой совет |
|---|---|---|
| **Stage Zero** ⚠️ | НЕ блокаут-тул (как в списке), а **Preload Suite** — оптимизатор подгрузки/стрим-хитчей | **Брать** как перф-инструмент (5.7 prebuilt, низкий риск), но для блокаута юзать Blockout Tools+ULAT. Подтверди, что перф-тул нужен. |
| **NPC Creator** (13 ГБ) | риг жильцов/жертв + ClothingMegaPack | **Осторожно**: 13 ГБ, UE5.5→миграция, СВОЙ `SK_Human_Skeleton` (≠ Quantum игрока → отдельный гардероб+ретаргет анимов). Брать только если жильцы реально нужны; тогда мигрировать срез ClothingMegaPack. |
| **Next Gen Destruction** (1.6 ГБ) | Chaos: ломкие стены/стекло/колонны (разруха!) | **Соблазнительно** под взрыв газа рушит стену, но это целая система (5.6→миграция BP/GeometryCollection). Дай отмашку — заведу отдельным заходом. |
| **Easy Multi Save** (`EMS57`) | надёжный сейв-фреймворк | **Скорее нет**: у нас свой сейв (флаги снаряжения уже там). Смысл только при росте до DRG-масштаба персистентности. |
| **Black Eye** (`BEC57`, 3.4 ГБ) | приоритет-стек камер/катсцены | **Нет сейчас**: для true-FPS не нужен, 93 cpp ребилд. Только если захочешь кинематик-камеру. |
| **Screen Space Fog Scattering** | рассеивание света в тумане (атмосфера) | **Можно**: легит-маркетплейс, сильный атмосферный выигрыш с EasyFog/UDS, но рендер-плагин + ребилд. Не в списке — реши. |
| **n00dEmotes** | система эмоций (кооп-социалка) | **Опц.**: приятно для кооп/DRG, но code-плагин + ребилд, не ядро. Брать `5.7`, удалить `5.6`. |
| **TranslationToolkit** | авто-перевод строк в редакторе (4.27) | **Скорее нет**: 4.27 editor-only, онлайн googletrans/gTTS, ребилд-риск. Наш .locres-конвейер уже работает (`LOCALIZATION.md`). |

---

## ❌ SKIP — не тащим (6)

| Пак | Почему |
|---|---|
| **Dialogue Plugin** (`Dialogue Plugin57`) | дубль диалогов из Narrative 3 (выбран он); второй code-плагин = фрагментация + ChatGPT-API флаг |
| **Narrative Tutorial (Finished Project)** | целый UE5.3 .uproject (4 ГБ MetaHuman+ThirdPerson) — только как **референс**, не импорт |
| **Interaction Toolkit** | дубль `InteractionKitVol1` (уже стоит, с репликацией) |
| **Pro HUD Pack V2** | миникарта/компас/вейпоинты — **против** диегетик-HUD (сердцебиение/одышка) |
| **CLASSIC STAIRS 3** | сырой 3dsMax/OBJ (0 uasset), вычурная классика ≠ разруха; лестницы уже из AtmosphericHouse |
| **Multiplayer FPS Procedural Anim** | sway/recoil оружия — у нас нет стрельбы; 5.0 ребилд ради неиспользуемого |

---

## ✅ УЖЕ ЕСТЬ — не переставлять (4)

| Пак в RawAssets | Где стоит | Замечание |
|---|---|---|
| `Imperfecter57` | `Plugins/Imperfecter` | бэкап-источник 5.7 |
| `TOT57` (TickOptToolkit) | `Plugins/TickOptToolkit` | бэкап-источник |
| `WorldBLD_1.3.9_UE57` | `Plugins/WorldBLD` | ⚠️ **не переставлять** — установленный пропатчен (ODR-фикс `GetCreditsValueFont` под 5.7.4), перезапись затрёт фикс |
| `Level Design Tools — DrCG` | `Content/DrCGLevelDesignTools` | бэкап-источник (BP, 5.1) |

---

## 👹 МОНСТР — ОТЛОЖЕНО (3) — не трогать без отмашки

`Mutant Rat Mouse`, `слухач` (Boss Animations + Plant Monster + Spiders), `Boss AI Toolkit` — всё это враг/босс-AI, заблокировано правилом «монстра-слухача не делать». Припарковано, мигрировать при открытии монстр-фазы.

---

## 🔧 ПЛАН ИСПОЛНЕНИЯ — скажи «гони группу N»

Порядок от безопасного к рискованному. Каждая группа — отдельная команда.

- **Группа A — контент robocopy (нулевой ребилд):** DR7, HorrorAndDecay, FuseBox, ElectricPack, Bags, Sewer(срез), Sundries, UltimateFire(Niagara), Smart Measure, Flashlight(срез), Poison(срез), Survival_UI(срез). → копирую в `Content/`, открываю редактор на resave. ⚠️ контент 5.3/4.27 апгрейдится при первой загрузке; пиратские-плейсхолдеры — **коммитить решишь ты**.
- **Группа B — FBX-анимы:** `anims_incoming` → импорт на Quantum (сначала Injured&Exhausted + Getting-Up, потом срез Game Anim Sample под блендспейс).
- **Группа C — звук:** курир. подмножество Sonomar + Foley Props + выбранные суб-паки Ghosthack → SoundWave/SoundCue.
- **Группа D — плагины prebuilt (копия в `Plugins/` + правка `.uproject` + рестарт редактора):** DLSS, RAI+STT+TTS, ULAT, Blockout, TriangleCount. ⚠️ риск: проверка бинарника 5.7.0 vs 5.7.4 — если плагин не пройдёт, редактор может не стартовать (поэтому делаю при тебе).
- **Группа E — плагины с ребилдом (закрыть редактор → Build.bat → смоук):** Cross-Platform Voice Chat, Narrative 3. ⚠️ риск сборки.

---

## ⚠️ ВАЖНЫЕ НАХОДКИ И КОРРЕКЦИИ

1. **Stage Zero ≠ блокаут.** В `СКАЧАТЬ.md` он помечен «блокаут/дрессинг» — на деле это **«Preload Suite»** (оптимизатор подгрузки). Реальный блокаут даёт **Blockout Tools + ULAT** (оба тащим). Поправил ожидание.
2. **Ghosthack-бандл ≠ «двери/шаги фоли».** Это **SFX-бандл** (взрывы/хоррор/импакты, 22 ГБ, 3737 WAV). Под аварии годится, но потребность «дверь/шаги фоли» из списка им **не закрыта** — это другой пак.
3. **Лицензии.** Множество контент-паков — пиратские-плейсхолдеры (маркеры `unrealengine.monster` и т.п.). До релиза: один пак на категорию + лицензировать. (Память `asset-licensing`.)
4. **Диск.** 92.5 ГБ в RawAssets, но «вес импорта» в разы меньше: 37 ГБ — сырой звук (берём подмножество), 13 ГБ NPC Creator (под вопросом), демо-проекты/`Intermediate` (NarrativeTutorial 4.7, BEC 3.4) — не импортируются. 4 пака — бэкапы установленного: после стабилизации можно удалить из RawAssets (~2.7 ГБ), т.к. `Plugins/` в gitignore и re-copy не из них.
5. **Дубли-комплименты:** `Industrial Fuse Box` (главный щиток) и `ElectricBox` из `Electrical set` — **разный стиль, держим оба**. `Hitreact Pro` vs установленный `JKMotion_HitReaction` — **выбрать одно**.

---

## 🗺️ Привязка к картам

- **Дом:** Modular Sewers (подвал), Bags/clutter, DR7+HorrorAndDecay (разруха), Flashlight, Rope Creak.
- **Завод:** ElectricPack, FuseBox, DR7-ржавчина, Modular Sewers (тоннели), DLSS (тяжёлая карта).
- **Больница:** GothicTexture (стены), Sonomar (эмбиент асайлума), HorrorAndDecay.
- **Все:** Sundries-иконки, Foley, UltimateFire, Poison-газ, Lightning-дуга, Survival UI-скин, Narrative-каркас, голос-стек (рация).

---
_Полная таблица по 60 пакам — `RAWASSETS_TRIAGE_TABLE.md` (приложение). Источник вердиктов: воркфлоу-триаж, 11 инспекторов, 2026-06-24._

---

## Source: RAWASSETS_TRIAGE_TABLE.md

# RAWASSETS — полная таблица (приложение к RAWASSETS_TRIAGE.md)

Все 60 папок. Колонки: вердикт · тип · версия UE · размер (МБ) · назначение · короткая заметка.
Вердикты: ⬇️=IMPORT_NOW · ✂️=CHERRY_PICK · ❓=NEEDS_USER_CALL · ❌=SKIP · ✅=ALREADY_HAVE · 👹=MONSTER_LATER

| # | Папка | Что это | Вердикт | Тип | UE | МБ | Куда | Заметка |
|---|---|---|---|---|---|---:|---|---|
| 1 | `2025.12.09_UE5.7_DLSS4Plugin` | NVIDIA DLSS 4 (Streamline/NIS/Reflex) | ⬇️ | plugin-cpp | 5.7.0 | 2560 | Plugins/* | native prebuilt; копировать только под-плагины, выкинуть Samples/WinGDK/Docs |
| 2 | `Triangle Count View Mode 5.7` | вьюпорт-режим плотности тришек | ⬇️ | plugin-cpp | 5.7.0 | 63 | Plugins/TriangleCountViewMode | дев; переименовать из обфусц. имени |
| 3 | `Smart Measure` | линейка измерений (BP) | ⬇️ | plugin-content | ~5.x | 3 | Content/SmartMeasure | дев; копир. `MetaCode/SmartMeasure` |
| 4 | `Imperfecter57` | шейдер несовершенств | ✅ | plugin-cpp | 5.7.0 | 629 | — | стоит в `Plugins/Imperfecter` |
| 5 | `TOT57` | Tick Optimization Toolkit | ✅ | plugin-cpp | 5.7.0 | 211 | — | стоит в `Plugins/TickOptToolkit` |
| 6 | `CrossPla…` | Universal Voice Chat Pro | ⬇️ | plugin-cpp | 5.7.0 | 137 | Plugins/UniversalVoiceChatPro | ⭐рация-кооп; есть Source → **ребилд** |
| 7 | `EMS57` | Easy Multi Save | ❓ | plugin-cpp | 5.7.0 | 232 | Plugins/EasyMultiSave | у нас свой сейв; брать только при DRG-персистентности |
| 8 | `BEC57` | Black Eye (камеры/катсцены) | ❓ | plugin-cpp | 5.7.0 | 3400 | Plugins/Black_Eye | не нужен true-FPS; 93cpp ребилд; 3.4 ГБ |
| 9 | `FlexPath - Smooth Navigation` | сглаживание AI-нав | ✂️ | plugin-cpp | 5.7.0 | 347 | Plugins/FlexPath | только подпапка 5.7; отложить до AI |
| 10 | `Narrative57` | Narrative 3 (квесты+диалоги) | ⬇️ | plugin-cpp | 5.7.0 | 397 | Plugins/Narrative | ⭐каркас миссии; Source → **ребилд** |
| 11 | `Dialogue Plugin57` | node-диалоги (CodeSpartan) | ❌ | plugin-cpp | 5.7.0 | 184 | — | дубль диалогов Narrative; ChatGPT-API флаг |
| 12 | `NarrativeTutorialFinishedProject` | демо-проект Narrative | ❌ | mixed | 5.3 | 4710 | — | целый .uproject (4 ГБ MetaHuman) — только референс |
| 13 | `RuntimeAudioImporter` | импорт/захват аудио рантайм | ⬇️ | plugin-cpp | 5.7.0 | 348 | Plugins/RuntimeAudioImporter | ⭐основа голос-стека; prebuilt |
| 14 | `RuntimeSpeechRecognizer57` | Whisper STT (офлайн) | ⬇️ | plugin-cpp | 5.7.0 | 430 | Plugins/RuntimeSpeechRecognizer | ⭐копир. вложенную; зависит от RAI |
| 15 | `RuntimeTextToSpeech57` | Piper TTS (офлайн) | ⬇️ | plugin-cpp | 5.7.0 | 449 | Plugins/RuntimeTextToSpeech | ⭐голос диспетчера; зависит от RAI |
| 16 | `TranslationToolkit` | авто-перевод строк (редактор) | ❓ | plugin-cpp | 4.27 | 70 | Plugins/TranslationToolkit | 4.27 editor-only, онлайн-либы, ребилд-риск; .locres уже работает |
| 17 | `stage zero-3.3.1 (5.7)` | **Preload Suite** (не блокаут!) | ❓ | plugin-cpp | 5.7.0 | 525 | Plugins/StageZero | перф-оптимизатор подгрузки; prebuilt |
| 18 | `UltimateLevelArtTool_5.7` | ULAT — дрессинг/сплайны/скаттер | ⬇️ | plugin-cpp | 5.7.0 | 206 | Plugins/UltimateLevelArtTool | ⭐лошадь дрессинга 3 карт; prebuilt |
| 19 | `Blockout Tools Plugin` | грейбокс (боксы/лестницы/снап) | ⬇️ | plugin-cpp | 5.7.0 | 170 | Plugins/BlockoutToolsPlugin | прибилт; разметка Завода/Больницы |
| 20 | `PT5.7` | ProInstance Tools (ISM-скаттер) | ✂️ | plugin-cpp | 5.7.0 | 199 | Plugins/ProInstanceToolsPlugin | дубль скаттера ULAT; только если мало |
| 21 | `WorldBLD_1.3.9_UE57` | город/дороги | ✅ | plugin-cpp | 5.7.0 | 1900 | — | стоит в `Plugins/WorldBLD` (пропатчен — не трогать) |
| 22 | `Hyper Procedural Spline Toolkit v4` | PCG-сплайны (дороги/заборы) | ✂️ | plugin-content | 5.5 | 5100 | Content/Hyper | брать Core+Environment_Building, выкинуть 5 ГБ ResourcePack |
| 23 | `Level Design Tools - DrCG` | хелпер расстановки (BP) | ✅ | plugin-content | 5.1 | 21 | — | стоит в `Content/DrCGLevelDesignTools` |
| 24 | `InteractionToolkit` | фреймворк взаимодействия (BP) | ❌ | content-bp | ? | 84 | — | дубль `InteractionKitVol1` |
| 25 | `Easy Game UI Ultimate` | модульный UI-бандл | ✂️ | content-ui | ? | 85 | Content/EasyGameUI | брать только InputPrompts/SaveGameUI; EasyOptionsMenu УЖЕ стоит |
| 26 | `Pro HUD Pack V2` | миникарта/компас/вейпоинты | ❌ | content-ui | 5.0 | 153 | — | против диегетик-HUD |
| 27 | `Survival_UI_Kit` | хоррор HUD/инвентарь скин | ✂️ | content-ui | ? | 230 | Content/Survival_UI_Kit | брать текстуры/рамки, не полоски; оружие — мимо |
| 28 | `Hyper Mesh to Icon Creator v3.15` | печь иконок из мешей | ✂️ | tool | ~5.x | 358 | Content/Hyper | дев; брать только Icon_Creator+Core, прогнать раз |
| 29 | `Sundries - Icon Pack` | 190 иконок предметов | ⬇️ | content-ui | текстуры | 268 | Content/Sundries | ⭐слоты инвентаря; набор 512 |
| 30 | `NPC Creator` | риг жильцов + одежда | ❓ | content-char | 5.5 | 13000 | Content/NPCCreator | 13 ГБ, СВОЙ скелет (≠Quantum), миграция; брать срез если жильцы нужны |
| 31 | `UltimateFirePackVol1` | огонь/дым/взрыв VFX | ⬇️ | content-vfx | ? | 466 | Content/UltimateFirePackVol1 | ⭐«Горит»; Niagara, не Cascade |
| 32 | `Lightning___Electric…Alembics` | 40 baked-болтов (alembic) | ✂️ | content-vfx | ? | 1400 | Content/LightningAlembics | 2–3 болта, только скрипт-дуга; не драйвится |
| 33 | `Poison Magic Niagara` | 32 зелёных Niagara-системы | ✂️ | content-vfx | ? | 559 | Content/PoisonMagicNiagara | Area/Aura/Flow → газ-облако; выкинуть магию/демо |
| 34 | `Luoss Particle Toolkit Vol 1` | общий VFX-граббэг (Cascade) | ✂️ | content-vfx | ? | 720 | (или SKIP) | полка VFX переполнена; брать только уникальный луп |
| 35 | `ScreenSpaceFogScattering5.7` | рассеивание в тумане (рендер) | ❓ | plugin-cpp | 5.7.0 | 107 | Plugins/ScreenSpaceFogScattering | легит, атмосфера+, но ребилд; не в списке |
| 36 | `Next Gen Destruction Toolkit` | Chaos: ломкие стены/стекло | ❓ | content-bp | 5.6 | 1600 | Content/NextGenDestruction | разруха!, но целая система, 5.6→миграция |
| 37 | `DR7` | Decals Rusty Metal Vol.7 | ⬇️ | content-mat | ? | 875 | Content/Decals_Rusty_metal_Vol_7 | ⭐ржавчина на всех картах |
| 38 | `GothicTexture_Meshingun` | готические поверхности | ✂️ | content-mat | ? | 382 | Content/GothicTexture_Meshingun | пара материалов под Больницу |
| 39 | `HorrorAndDecay_VOL5_5.3` | гнилые меши+материалы | ⬇️ | content-meshes | 5.3 | 409 | Content/DEKO_HorrorAndDecay5 | ⭐разруха; копир. только Content/ |
| 40 | `Industrial Fuse Box Pack` ✅распак. | 14 щитков | ⬇️ | content-meshes | свежий | 39 | Content/FuseBoxPack | ⭐объект починки; дёшево |
| 41 | `Electrical set lamps wires` | щитки/розетки/провода/лампы | ⬇️ | content-meshes | ≤5.4 | 453 | Content/ElectricPack | дрессинг электрики; комплимент FuseBox |
| 42 | `Bags, Backpack & Survival Kit` | рюкзаки/сумки | ⬇️ | content-meshes | 4.27 | 942 | Content/Bags_Backpack_Survival_Kit | ⭐снаряга бригады + клаттер |
| 43 | `Modular Sewers & Tunnels` | модульный кит + trash | ⬇️ | content-meshes | 5.3 | 3200 | Content/Sewer | ⭐подвал/тоннели; выкинуть Demo/External |
| 44 | `CLASSIC STAIRS 3` | сырой 3dsMax/OBJ лестница | ❌ | content-meshes | DCC | 397 | — | 0 uasset, вычурно ≠ разруха |
| 45 | `Stairs Set (5.0+)` ✅распак. | аним лестничной локомоции | ✂️ | content-anim | 5.0+ | 311 | Content/StairsSet | UE5-mann ≈ Quantum; отложить |
| 46 | `Modular Staircase Blueprint` | проц-генератор лестниц | ✂️ | content-bp | 5.5 | 312 | Content/Staircase_BP | если не хватит AtmosphericHouse |
| 47 | `Flashlight` | фонарь/фонарь-лампа (геймплей) | ✂️ | content-bp | ~5.x | 761 | Content/Flashlight | 🔥обязателен хоррору; брать продукт, выкинуть Demo |
| 48 | `Ghosthack x Boom - Bundle` | 13 SFX-суб-паков, 3737 WAV | ✂️ | content-sound | сырой | 22528 | Content/SFX/Ghosthack | 22 ГБ! брать Explosives/Horror/Fighting |
| 49 | `Sonomar: Abandoned Asylum` | 112 WAV асайлума | ⬇️ | content-sound | сырой | 15360 | Content/SFX/Sonomar_Asylum | ⭐хоррор-эмбиент; курир. подмножество |
| 50 | `Foley Props Sound FX Pack 4.27` | 280 фоли предметов | ⬇️ | content-sound | 4.27 | 108 | Content/SFX/FoleyProps | ⭐Tools/Keys/Craft/спрей → починка |
| 51 | `Horror Amplifier` | 93 стинга/эмбиент | ✂️ | content-sound | 5.3 | 238 | Content/SFX/HorrorAmplifier | папка 5.3; отличит. стинги, не дубли Sonomar |
| 52 | `звуки` | Rope Stress & Creak (1 WAV) | ✂️ | content-sound | сырой | 2 | Content/SFX/Misc | тривиально; можно SKIP |
| 53 | `Mutant Rat Mouse 4.20+` | риг крыса-мутант | 👹 | content-char | 4.20+ | 86 | — | враг → монстр-отложено |
| 54 | `FuneralAnimations` | аним похорон (маннекен) | ✂️ | content-anim | ? | 427 | Content/FuneralAnimations | kneel/mourn под сцену; низкий приоритет |
| 55 | `Hitreact Pro` | directional hit-реакции (компонент) | ✂️ | content-bp | 5.x | 208 | Content/HitreactPro | **выбрать**: он ИЛИ установленный JKMotion |
| 56 | `Multiplayer FPS Procedural Anim 5.0` | sway/recoil оружия | ❌ | plugin-cpp | 5.0.0 | 566 | — | нет стрельбы; 5.0 ребилд впустую |
| 57 | `n00dEmotes - Unreal Engine` | система эмоций | ❓ | plugin-cpp | 5.7.0 | 494 | Plugins/n00dEmotes | кооп-приятность; брать 5.7, удалить 5.6; ребилд |
| 58 | `anims_incoming` | FBX: Injured/GetUp/GameAnimSample | ⬇️ | content-anim | FBX | 2355 | Content/Avariika/Anim | **импорт** на Quantum; начать с Injured&Exhausted+GetUp |
| 59 | `слухач` | Boss Anims+Plant Monster+Spiders | 👹 | content-char | 5.0-5.2 | 868 | — | монстр-бандл — отложено |
| 60 | `Boss AI Toolkit 5.3` | босс-AI (BT/фазы) | 👹 | content-bp | 5.3 | 583 | — | монстр-AI — отложено |

---

## Source: CRAWL_FINDINGS.md

# КРАУЛ UE-АССЕТОВ — НОВЫЕ находки (вне unrealmonster)

Скан: **scrydy.ru** (496 стр) + **cgdownload.net** (48 стр). Дедуп против `DOWNLOADS.md` (unrealmonster) — здесь **только то, чего на монстре не было**. Источник: *(scrydy)* / *(cgdownload)* / *(оба)*.
`moreassets.ru` — пропущен (Beget JS-гейт обойдён, но DLE-разметка + 8 стр + полное пересечение со scrydy/cgdownload).

> ⚠️ Паки-плейсхолдеры (пиратские) — лицензии к релизу. UE 5.7.

---

## 🏆 Жемчужины (лучшее новое)
- **Decay: Oil Drums District** *(оба)* — индастриал-двор завода: ржавые бочки/грязь — наш сеттинг
- **Abandoned Hospital (Modular)** *(оба)* — готовая больница с интерьерами — локация №3
- **Industrial Factory (Modular)** *(cgdownload)* — модульный завод — локация №2
- **Fire Hydrant Package** *(scrydy)* — аварийное спецоборудование (гидранты)
- **First Aid Set** *(scrydy)* — меш аптечки/мед-снаряжения (наш предмет!)
- **Bags, Backpack & Survival Kit** *(cgdownload)* — снаряжение спасателей
- **Radio System and Interaction** *(scrydy)* — рация для коопа (наш вайб «орут в рацию»)
- **Toxic Gas Pack** *(scrydy)* — токсичный газ VFX (наша газовая авария!)
- **Cinemotion 2 — Handheld Camera Kit** *(cgdownload)* — дрожащая камера (реализм/комедия)
- **Dark Russian Voices** *(scrydy)* — русские голоса (диспетчер/жильцы) — на-тему
- **Soviet Apartment Megapack / Post-Soviet Flat** *(оба)* — гритти-жильё под дом

## 🖥 UI
- **Grunge / Military / Horror Dark GUI Kit** *(scrydy)* — тёмный хоррор-UI (меню/иконки)
- **1000 Profession & Craft Icons** *(scrydy)* — иконки профессий/инструментов (под слоты)
- **3D Text Generator / 3D Volume Text** *(scrydy)* — диегетический 3D-текст (подсказки в мире)
- **Delayed Healthbar System** *(scrydy)* — HUD здоровья с задержкой
- **PSD2UMG** *(scrydy)* — импорт UI из PSD (пригодится с хоррор-артом)

## 🔊 Звук
- **Extreme Horror Sound Effects** *(scrydy)* — хоррор-эмбиент/эффекты
- **Horror Amplifier** *(cgdownload)* — атмосферные хоррор-эффекты
- **Ambient & Environment SFX Pro** *(scrydy)* — эмбиент завода/дома/больницы + фоли
- **Impacts, Hits & Whooshes** *(cgdownload)* — удары/действия
- **Thunder & Rain SFX** *(scrydy)* — гроза/дождь/гром
- **Bloodlust Gore SFX / Big Monster Sounds** *(scrydy)* — гор/монстры (часть под монстра)

## 🔌 Плагины
- **Fire Chat PRO** *(scrydy)* — proximity voice-chat (альт Voice Chat Pro)
- **Host Migration System V2** *(scrydy)* — хост-миграция (стабильность кооп-сессии)
- **Basic Ping System** *(scrydy)* — пинги-маркеры для команды
- **Elevator Kit (Blueprint)** *(cgdownload)* — интерактивные лифты (§10 лифт в доме/больница)
- **Dynamic Real Water / UIWS** *(scrydy)* — симуляция воды (затопление/протечки — наш потоп)
- **Advanced AI Spawn System** *(scrydy)* — спавн/менеджмент AI (под монстра позже)

## 🧰 Пропсы (предметы/декор)
- **First Aid Set** *(scrydy)* — аптечка ⭐
- **Bags, Backpack & Survival Kit** *(cgdownload)* — снаряжение бригады ⭐
- **Fire Hydrant Package** *(scrydy)* — гидранты/аварийное
- **Toolset Collection Vol 1 / AAA Carpenter's Workshop** *(scrydy)* — инструменты/мастерская
- **Operating Room Pack** *(scrydy)* — мед-оборудование (больница)
- **Electrical set / lamps / wires** *(cgdownload)* — электрика/провода (щиток/индастриал)
- **101+ Everyday Props / Soviet Living Room** *(оба)* — бытовуха для домов

## 🎨 Материалы / декали
- **1000+ Mega Decal Package** *(scrydy)* — ржавчина/грязь/кровь, 1000+ декалей
- **12 Burning Wood Materials (Animated)** *(scrydy)* — горящее дерево (под «Горит»)
- **400 Noise Texture Mega Pack** *(scrydy)* — грязь/ржавчина/износ поверхностей
- **Concrete / Brick / Asphalt Material Packs** *(scrydy)* — индастриал-поверхности
- **Destruction Decal BP** *(cgdownload)* — декали разрушений
- **Advanced Glass / Broken Glass Shader** *(scrydy)* — стекло/разбитые окна

## ✨ VFX (хазарды/атмосфера)
- **Toxic Gas Pack** *(scrydy)* — токсичный газ (газовая авария!) ⭐
- **Radiation VFX** *(scrydy)* — радиация (опасные зоны)
- **100 Explosion Pack** *(scrydy)* — взрывы/искры
- **Concrete Debris VFX** *(cgdownload)* — обломки при разрушении
- **Advanced CRT/VHS Effects** *(scrydy)* — старые мониторы (хоррор-антураж)
- **Elias Wick Advanced Fog / EasyFog / Ultra Volumetrics** *(оба)* — туман/объём
- **Advanced Dissolve FX** *(scrydy)* — растворение (хоррор)

## 🏗 Локации (твоя зона — стройка)
- **Abandoned Hospital · Asylum · Industrial Factory · Chemical Plant & Refinery** *(оба)* — дом/завод/больница
- **Soviet Apartment Megapack · Post-Soviet Flat · Modular Neighborhood · American Suburban House** *(оба)* — жильё
- **Concrete Hallway · Brutalist Office · Industrial City Mega Pack · 1940s Office** *(оба)* — индастриал-интерьеры
- **Underground Caves & Bunker · Underground Facility · Abandoned Hotel/School** *(оба)* — альт-локации
- **[SCANS] Abandoned Factory/Warehouse/Manor** *(scrydy)* — фотограмметрия (реализм)

## 🧍 Персонажи (гражданские/анимы)
- **Modular Human Skeleton / Assembly Modular Character Creator** *(scrydy)* — модульные реалистичные люди
- **Pedestrians Vol 2** *(cgdownload)* — гражданские NPC (толпа/соседи)
- **1950s Characters Bundle / Bar Counter People / Agents Pack** *(scrydy)* — обычные люди
- **MoCap-анимы: Blacksmith/Preacher NPC · 50 Female · Vendors & Customer · Village Life** *(оба)* — анимации рабочих/быта/интеракций

## 👹 МОНСТР — ОТЛОЖЕНО (не качать без отмашки)
- **105 Zombie Anims Mocap + 281 Zombie Audio** *(scrydy)* — анимы+звук зомби (большой)
- **Gruesome Zombie AnimSet · 26 Zombie Anims+BP · 60 Creatures Mocap** *(scrydy)* — анимы существ
- **Wendigo · Mutant Monster 10 · BigBlob Slime · Parasite/Urban/Super Zombie** *(оба)* — враги
- **Zombie - Hazmat / Police / Cheerleader (Starved)** *(оба)* — тематические зомби (хоррор-комедия)
- **Realistic Decomposition · Bloodthirsty Beetle · Infected Collection** *(scrydy)* — гор/жуть

---
**Итог:** ~1405 уникальных позиций из 2 сайтов; здесь — отфильтрованное НОВОЕ (вне unrealmonster), макс ~7 на корзину. Дубли с монстром и «есть-своё» (EOS/сейв/инвентарь/камера/Dialogue/GAS) убраны.

---

## Source: OPTIMIZATION_REPORT.md

# OPTIMIZATION_REPORT — оптимизация проекта «Аварийка»

> Автономный read-only аудит 2026-06-21. Ничего не удалял/не менял — только анализ + рекомендации.
> Узкое звено железа: **RTX 5060, 8 ГБ VRAM** (редактор виснет при переполнении VRAM — Lumen+Nanite+тяжёлые паки).
> ⚠️ Паки 3 карт (Дом/Завод/Больница) НЕ удалять без сверки (`three-maps-plan`, `asset-deletion-method`).

---

## TL;DR — три самых ценных действия

1. **Добавить Nanite/Lumen scalability-тиры** в `DefaultScalability.ini` (сейчас Lumen ВСЕГДА на полном качестве,
   даже на Low) — это прямое лечение VRAM-зависаний. Самый важный пункт, безопасно.
2. **Безопасная чистка ~6–7 ГБ** без риска: `Intermediate` (3 ГБ), `Saved` (0.6 ГБ), `DerivedDataCache` (0.26 ГБ)
   регенерируются; demo-`*_BuiltData` непродакшн-карт (~2–3 ГБ).
3. **Разобрать дубли** Hospital/HospitalCombo и `_Packs`-дубликаты (~1.7 ГБ) — но СНАЧАЛА сверить, какой том в игре.

---

## 1. Куда уходит диск (Content ≈ 93 ГБ + служебное)

| Объект | ГБ | Статус |
|---|---|---|
| `Content/IndustrialFactory` | 21.2 | пак карты **Завод** — нужен (cherry-pick в дом, см. `FACTORY_PACK_AUDIT.md`) |
| `Content/Warehouse` | 16.0 | 2-й фабричный — донор декея; демо-карты не нужны |
| `RawAssets/` | 15.5 | исходники тулкитов (Hyper Procedural 5 ГБ, мокап 2.3 ГБ, WorldBLD 1.9 ГБ) — локально, gitignore |
| `Content/ResidentialHouses` | 11.4 | каркас **Дома** — нужен |
| `Content/_Packs` | 10.3 | **сборная солянка**: 43 демо-карты + 29 BuiltData (вендорские демки, не геймплей) |
| `Content/AtmosphericHouse` | 9.25 | меблировка/атмосфера **Дома** — нужен (4 карты-демо можно облегчить) |
| `*_BuiltData.uasset` (77 файлов) | 7.4 | запечённый свет/GI — крупные: Hospital Demonstration_2 (~2 ГБ), FactoryDocks day/overcast (1.6 ГБ ×2) |
| `Content/Hospital` | 3.0 | пак **Больницы** — нужен; ⚠️ дублируется с HospitalCombo |
| `Intermediate/` | 3.02 | шейдер-кэш — регенерируется |
| `Content/HospitalCombo` | 1.16 | ⚠️ дубль Hospital (восстановлен из git bd76ef7) |
| `Content/NiagaraExamples` | 1.19 | демо-галерея (~109 систем) — не геймплей |
| `Content/CitizenNPC` | 1.14 | бригада/жильцы — нужен (демо-overview можно убрать) |
| `Saved/` | 0.61 | логи/темп — чистимо |
| `DerivedDataCache/` | 0.26 | DDC — регенерируется |

---

## 2. Безопасная чистка СЕЙЧАС (регенерируемое, ноль риска для игры)

> Чистить при **закрытом** редакторе. Всё ниже движок пересоберёт при следующем открытии/готовке.

- `Intermediate/` (3.02 ГБ) — шейдеры/материалы. ⚠️ Первое открытие после чистки = долгая пересборка (10–15 мин на 8 ГБ) — это норма.
- `Saved/` (0.61 ГБ) — логи, автосейвы, темп (кроме нужных тебе `Saved/*.json` от наших скриптов — их оставить).
- `DerivedDataCache/` (0.26 ГБ) — локальный DDC.
- **Demo-`*_BuiltData`** непродакшн-карт (FactoryDocks, Warehouse_01/02, NiagaraExamples GalleryLevel, FluidFlux/M5VFXVOL2 демки) — ~2–3 ГБ; сами `.umap` оставить, свет перепечётся при надобности.

**Итого безопасно: ~6–7 ГБ.** Не трогает ни один геймплейный ассет.

---

## 3. Дубли и лишнее (на твоё решение — СНАЧАЛА сверить)

- **Hospital (3 ГБ) + HospitalCombo (1.16 ГБ) = 4.16 ГБ** — оба про Больницу. Решить, какой том (VOL1–6) реально в карте Больницы, второй — в архив. ⚠️ память: HospitalCombo восстановлен намеренно — не сноси вслепую.
- **`_Packs/` дубли**: `HospitalCombo` и `Construction_VOL2` лежат И в `Content/`, И в `Content/_Packs/` (~1.7 ГБ дублирования). Оставить одну копию.
- **`_Packs/` = 43 демо/showcase-карты** (Garage_Tools showcase, Survival_Horror gallery, ZombieAnimationPack showcase…) + 29 BuiltData — вендорские демки, не продакшн. Кандидат на прореживание (оставить только нужные меши, демо-карты убрать).
- **`_Packs/oldBenchPack`** — легаси-бенчмарк, без ссылок в геймплее.
- **Warehouse demo-карты** (`Warehouse_01/02_P`) — reference, не в шиппинге.
- **VFX-демки**: NiagaraExamples (1.19 ГБ), FluidFlux (0.43 ГБ), M5VFXVOL2 (0.56 ГБ), NiagaraExplosion01 (0.84 ГБ) — учебные/showcase. Нужны конкретные системы (см. `EFFECTS_INVENTORY.md`/`INCIDENT_FX_MAP.md`), не демо-карты.

> ⚠️ Все эти паки уже в `.gitignore` (локально). Удаление влияет только на локальный диск, но **меши могут быть references** в картах — перед сносом `get_referencers` (память `asset-deletion-method`).

---

## 4. ⭐ Настройки рендера — главная находка (VRAM 8 ГБ)

**Что уже хорошо** (`DefaultScalability.ini` / `DefaultEngine.ini`):
- TSR-апскейл настроен по тирам (Low 50% → Epic 100%) — правильно для 8 ГБ.
- `EffectsQuality` гейтит партиклы (0.5–0.75× на Low/Med); FluidFlux завязан на EffectsQuality≥High.
- `r.RayTracing=False`, `r.Lumen.HardwareRayTracing=False` — софт-Lumen, без аппаратного RT (бережёт 8 ГБ).
- `r.Streaming.PoolSize=2500` — поднят (был «TEXTURE STREAMING POOL OVER» от тяжёлых текстур).
- VSM (`r.Shadow.Virtual.Enable=1`), Substrate, статик-лайтинг off (только Lumen).

**🟢 ПОПРАВКА (перепроверено по движковому `BaseScalability.ini`): Lumen/текстуры УЖЕ масштабируются — добавлять тиры НЕ нужно.**
Первичная находка аудита «нет Lumen/Nanite тиров, Lumen всегда full» оказалась **ложной** (агент читал только проектный ini, не движковый).
Движок (`Engine/Config/BaseScalability.ini`) уже даёт: `[GlobalIlluminationQuality@0]` → `r.Lumen.DiffuseIndirect.Allow=0` (Lumen GI
ВЫКЛ на Low, DFAO вместо), `@1` тоже без Lumen, `@2` Lumen с урезанным бюджетом, `@3` полный; `[ReflectionQuality@N]` гейтит
`r.Lumen.Reflections.Allow`; `[TextureQuality@0]` = `r.Streaming.MipBias=16` + `PoolSize=400` + `LimitPoolSizeToVRAM=1` (агрессивно под VRAM);
Nanite гейтится своими движковыми путями. Проект это наследует (переопределяет лишь ResolutionQuality + EffectsQuality).
→ **Кастомные `[LumenQuality@N]`/`[NaniteQuality@N]` добавлять НЕ нужно и ВРЕДНО** — затрут разумные движковые значения. (Правка НЕ внесена.)

**🔴 Что РЕАЛЬНО осталось (рычаги VRAM):**
- **Нет авто-детекта качества при первом запуске** — игрок на слабом железе попадёт на Epic → VRAM overflow → плохой отзыв.
  В `EasyOptionsMenu` уже есть `DemoBenchmarkLevel` — повесить авто-бенчмарк, ставящий scalability на старте.
- **Нет per-map override** — тяжёлая карта (Завод с FluidFlux) может требовать ниже глобала. Добавить map-specific конфиг при VRAM-давлении.

---

## 5. TickOptToolkit (установлен, ещё не применён) — куда натравить

TOT снижает частоту/выключает тики по дистанции и видимости. Кандидаты:
- **`Repairable_*` акторы** (аварии): эффект/звук-тики только когда игрок близко/видит (а не каждый кадр по всему дому).
- **Ambient-эмиттеры** (скрипы, room-tone, капель) — тик по дистанции.
- **CitizenNPC** (жильцы/бригада): `ActorTickInterval`-пулинг, выключать тик пешек вне зоны игрока, упрощённый AI для фоновых.
- **FluidFlux-солверы** — пауза/пул когда вода не на экране; ниже разрешение сетки для невидимых симуляций.
- **TOD-актёры UDS** в фоновых сублевелах — гасить тик солнца/времени когда уровень не активен.
- **Destruction (Next Gen)** — пул актёров разрушения, лимит одновременных симуляций, статик-меш для неинтерактивного щебня.

---

## 6. Git / воспроизводимость (важно для бэкапа)

- Все тяжёлые паки в `.gitignore` (стратегия «локально, re-copy из RawAssets»). Плюс: репо не пухнет. **Минус: свежий клон получит пустой `Content/`** — придётся переимпортить всё вручную.
- У части паков источник = ТОЛЬКО git-история (Hospital/Construction — их нет в RawAssets). Если репо потеряется — паки не восстановить.
- **Рекомендация:** в `WORKLOG.md` вести список «какой пак откуда переимпортить» (FAB-ссылка / RawAssets / git-история). Для ядра (IndustrialFactory, ResidentialHouses) рассмотреть **Git LFS**, чтобы не качать заново.

---

## 7. Приоритеты (что я бы сделал по порядку)

| # | Действие | Риск | Выигрыш |
|---|---|---|---|
| 1 | Nanite/Lumen scalability-тиры + авто-бенчмарк | низкий | **прямое лечение зависаний** |
| 2 | Безопасная чистка Intermediate/Saved/DDC/demo-BuiltData (закрыть редактор) | нет | ~6–7 ГБ |
| 3 | TOT на Repairable/ambient/NPC | низкий | FPS/стабильность в игре |
| 4 | Разобрать Hospital/HospitalCombo + `_Packs`-дубли (сверив тома) | средний | ~1.7–4 ГБ |
| 5 | Demo-карты паков → локальный архив | низкий | ~6 ГБ BuiltData |
| 6 | Отрезать дальнее вост. крыло L_Dom1 (см. `FLOOR1_DESIGN.md` §7) | средний | VRAM + плотность хоррора |

_Связано: `FACTORY_PACK_AUDIT.md`, `FLOOR1_DESIGN.md`, `three-maps-plan`, `water-tech-decision`, `ue-build-workflow`._

---

## Source: DISK_C_SCAN.md

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

---

## Source: FINAL_REPORT.txt

НОВЫЕ НАХОДКИ ДЛЯ КООП-ХОРРОР-КОМЕДИИ (Аварийная бригада)
═══════════════════════════════════════════════════════════════

📦 PROPS (Игровые пропсы)
━━━━━━━━━━━━━━━━━━━━━━━━━━
▸ Hospital – COMBO Prop Pack (VOL 1-6) — Полный набор больничного оборудования и мебели
▸ R2 – REALISTIC HOUSE PACK — Реалистичные домашние предметы для жилых сцен
▸ Modern Hospital Props VOL.2 – Medications and Tools — Медикаменты и инструменты для спасательных сцен  
▸ Hospital Props VOL.1 – Interior & Equipment — Крупное больничное оборудование (каталки, мониторы)
▸ Factory Interior + Warehouse Props Vol 1 – BUNDLE — Промышленное оборудование для заводских уровней
▸ Industrial Fuse Box Pack — Электрические щиты для индастриальных сценариев
▸ Garage Tools Props — Слесарные инструменты для аварийной бригады
▸ [SCANS] Industrial Abandoned Buildings – Modular — Модульные части заброшенных зданий

🎨 МАТЕРИАЛЫ (Хоррор-декали и текстуры)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
▸ Horror and Decay VOL.3 – Quarantine/Hospital — Загрязнение и ржавчина для больничных стен
▸ Industrial Rust & Corrosion Vol. 7 — 53 фотосканированных декаля ржавчины
▸ Horror and Decay VOL.5 – Urban Surfaces — Урбанские поверхности с загрязнением
▸ 40 Pcs Professional Blood Decal Bundle — Кровь и травмы для комедийных сцен
▸ Realistic Blood Decals — Реалистичные кровяные пятна
▸ Horror Collection: Blood — Дополнительный набор кровяных эффектов
▸ Gothic Texture Pack — Тёмные индастриальные текстуры
▸ Industrial Decal Pack — Промышленные декали (масло, грязь, потеки)

👥 ПЕРСОНАЖИ (Гражданские NPC)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━
▸ Survival NPC – Maria Estrada — Реалистичная женская фигура для спасательной сцены
▸ Survival NPC – Hank Murphy — Мужской рабочий характер для команды
▸ Agents Characters Pack — Пакет агентов/специалистов
▸ Male Mega Realistic Character Pack 01 — Множество реалистичных мужских моделей
▸ Modular Characters MegaPack — Кастомизируемые гражданские персонажи
▸ Construction NPC Animations — Анимации рабочих для интерьеров

🛠️ ЛЕВЕЛ-ТУЛЗЫ (Конструкция уровней)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
▸ Interior Toolkit — Основной инструмент для построения интерьеров домов/больниц/заводов
▸ Residential Houses – Modular Pack — Модульные части жилых домов
▸ Modular Sewers & Tunnels — Подземные ходы и канализация для заводов
▸ Modular abandoned factory — Готовые детали заброшенной фабрики
▸ KDath: Modular Buildings 01 — Модульные здания под снос
▸ Spline Mesh Tool – Modular Path Builder — Спланевые инструменты для построения путей

🎵 АУДИО (Звук и атмосфера)
━━━━━━━━━━━━━━━━━━━━━━━━━
▸ Video-Presets 3100 Cinematic Sound Effects — 3100 звуковых эффектов для атмосферы
▸ Boom Library – Armageddon — Взрывы и разрушительные звуки
▸ Chains Sound Pack — Звуки цепей для индастриальных сцен
▸ Metal Core Alliance — Металлические и механические звуки
▸ Rope Stress and Creak Sound Pack — Звуки натяжения и скрипа

✨ VFX (Визуальные эффекты)
━━━━━━━━━━━━━━━━━━━━━━━
▸ Stylized Water and Liquid VFX — Вода, утечки, затопления
▸ Ultimate Fire Pack Vol 1 — Огонь и взрывы
▸ Lightning & Electric Effect Alembics Pack — Электрические разряды
▸ Smoke and Fire Spline – Niagara Fluids — Дым через помещения

🎮 UI (Интерфейс)
━━━━━━━━━━━━━━
▸ Radial Menu (Wheel) — Круговое меню для аварийной команды
▸ Journeyman's Minimap 1.2 — Мини-карта для навигации в комплексе
▸ Basic Interaction, Widgets & Notes — Базовые интерактивные виджеты
▸ Inventory Examine Interface — Интерфейс экзаминации снаряжения

⚙️ ПЛАГИНЫ (Игровые механики)
━━━━━━━━━━━━━━━━━━━━━━━━━━
▸ Dynamic AI System (5.7) — Система ИИ для гражданских NPC и противников
▸ Hyper Scalable Interaction System v3 — Интерактивные объекты (двери, переключатели)
▸ Inventory Framework Plugin — Фреймворк инвентаря для оборудования
▸ Plug and Play AI Bundle — Готовое поведение ИИ для товарищей
▸ Mission & Objectives 1.5 — Система миссий и задач для кооп
▸ Advanced Jigsaw Inventory system — Продвинутый инвентарь по сетке


🏆 ЖЕМЧУЖИНЫ (ТОП-10 САМЫХ ЦЕННЫХ НАХОДОК)
═══════════════════════════════════════════════════════════════

1. 🏥 Hospital – COMBO Prop Pack (VOL 1-6)
   └─ КРИТИЧНО для сеттинга: полный больничный комплекс с реалистичным оборудованием

2. 🏭 Factory Interior + Warehouse Props Vol 1 – BUNDLE  
   └─ Один из трёх основных сеттингов: полный набор фабричного оборудования и структур

3. 🪜 Industrial Rust & Corrosion Vol. 7 (53 Photoscanned Decals)
   └─ ЖЕМЧУЖИНА материалов: фотосканированная ржавчина для индастриального реализма

4. 👥 Survival NPC – Hank Murphy / Maria Estrada
   └─ Реалистичные гражданские персонажи для построения команды аварийной бригады

5. 🛠️ Interior Toolkit
   └─ Основной инструмент для быстрого построения интерьеров дома/заводов/больниц

6. 🗺️ [SCANS] Industrial Abandoned Buildings – Modular Environment
   └─ Модульная разрушенная индастриальная среда — готовая база для заводских уровней

7. 🩸 40 Pcs Professional Blood Decal Bundle + Horror and Decay VOL.3
   └─ ЖЕМЧУЖИНА хоррора: комбо для создания мрачной атмосферы в разрушенных локациях

8. 🎯 Dynamic AI System (5.7) + Hyper Scalable Interaction System v3
   └─ Комбо плагинов для живого мира: ИИ NPC + интерактивные объекты

9. 🎵 Video-Presets 3100 Cinematic Sound Effects
   └─ ОГРОМНЫЙ набор звуков для создания атмосферы во всех локациях

10. 🌊 Stylized Water and Liquid VFX
    └─ Утечки, затопления, лужи — критично для реалистичных аварийных сценариев


📋 РЕКОМЕНДАЦИЯ ПРИОРИТЕТА
════════════════════════════

НЕМЕДЛЕННО:
  • Hospital COMBO (основной сеттинг)
  • Factory Interior Bundle (второй сеттинг)
  • Industrial Rust & Corrosion Vol.7 (материалы)
  • Interior Toolkit (левел-дизайн)

ВЫСОКИЙ:
  • NPC Survival пакеты (персонажи)
  • Horror & Decay VOL.3/5 (атмосфера)
  • Industrial Abandoned Buildings (модули)
  • Dynamic AI System (ИИ)

СРЕДНИЙ:
  • Звуковые пакеты
  • VFX пакеты
  • UI системы
  • Дополнительные пропсы


