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
