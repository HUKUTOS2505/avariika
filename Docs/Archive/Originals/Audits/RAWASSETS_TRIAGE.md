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
