# Planning, Shopping, and Download Notes

This consolidated document preserves the source material listed below. Originals are archived under Docs/Archive/Originals/.

## Sources
- `DOWNLOADS.md`
- `SHOPPING.md`
- `СКАЧАТЬ.md`
- `BUILD_FORMAT.md`
- `test.txt`

---

## Source: DOWNLOADS.md

 # АВАРИЙКА — список ассетов на скачку

Итог сканирования каталога unrealmonster (UI/Audio/Plugins/Characters постранично + весь фид /ue-assets/ 287 стр) + точечный поиск.
Отмечай галочками по мере скачивания. Кидать в `Content/` (плагины — в `Plugins/`).

> ⚠️ Паки в основном плейсхолдеры (пиратские) — **докупить лицензии к релизу**. Версия движка: **5.7** (старые паки → проход совместимости).

---

## 🖥 UI
- [ ] **Easy Game UI Ultimate** — меню/пауза/НАСТРОЙКИ (виджеты). ⭐ закрывает реальную дыру
- [ ] **Editable Survival Horror UI / Survival UI Kit** — PSD-арт (скин нашего C++-HUD; виджеты не нужны)
- [ ] **Hyper Mesh to Icon Creator v3** — генерит иконки слотов ИЗ мешей предметов ⭐
- [ ] Input Prompts Pack 1500+ — графика подсказок `[E]`/клавиш *(опц.)*
- [ ] Pop-up & Notification System — реплики диспетчера/алерты *(опц.)*
- [ ] Radial Menu (Wheel) — колесо быстрых слотов *(опц.)*
- [ ] Journeyman's Minimap — карта оператора *(опц.)*

## 🔊 Звук
- [ ] **Ghosthack × Boom Library — Sound FX & Foley Bundle** — двери/шаги/фоли (чинит «звуки не в тему») ⭐
- [ ] **Sonomar Collection: Abandoned Asylum** — хоррор-эмбиент ⭐
- [ ] **Blastwave FX — Horror Vol. 3** — хоррор-атмосферы
- [ ] SFXtools — Jumpscares (FREE) — стингеры
- [ ] Ocular Sounds — Ultimate Sound FX Bundle 4000+ — всё-в-одном
- [ ] **ВОКАЛЫ ИГРОКА** (под хуки крик/дыхание/кашель) — взять ОДИН:
  - Human Vocalizations (1034: грунты боли/эффорты/крики) — самый полный
  - Character Attack, Pain & Labor Voice Pack — урон/breath/struggle, бродкаст
  - Human Vocalization Sounds Pack Vol 1 — дыхание/кашель/грунты/дрожь *(точно под нас)*
- [ ] *(индастриал-фоли, опц.)* Metal Core Alliance · Chains · Rope Stress & Creak · Earthquake

## 🔌 Плагины
- [ ] **Stage Zero** — загрузка + прекомпил шейдеров (под 5060) ⭐
- [ ] **NVIDIA DLSS** — апскейл-перф под видеокарту
- [ ] **AudioToolkit Pro** — менеджмент/окклюзия звука
- [ ] **Narrative 3 (Node Quests & Dialogue)** — диспетчер/цели/сюжет
- [ ] **Cross-Platform Voice Chat Pro** — кооп-рация (голос)
- [ ] Mission & Objectives 1.5 — усилить наши цели *(опц.)*
- [ ] Hyper Scalable Interaction System v3 — интеракт *(опц.)*
- ❌ НЕ брать (есть своё): EOS→EOSCore · сейв→AvariikaSaveGame · инвентарь→C++ · камера→своя

## 🧰 Меши предметов (замена КУБОВ-плейсхолдеров)
- [ ] **Industrial Fuse Box Pack** — меш для ЩИТКА (breaker) ⭐
- [ ] **Garage Tools Props** — инструменты (ключ/тестер/…)
- [ ] *(спецснаряжение: противогаз/сварочная маска/огнетушитель/газоанализатор/аптечка/нашатырь — искать «safety equipment / first responder props»)*

## 🎨 Материалы / декали (стройка + хоррор)
- [ ] **Industrial Rust & Corrosion Vol. 7** (53 фотоскан-декаля ржавчины) ⭐
- [ ] Horror and Decay VOL.3 (Quarantine/Hospital) / VOL.5 — загрязнение стен
- [ ] **Blood Decal Bundle (40 шт)** — под огонь/гор/санинциденты/монстра
- [ ] Gothic Texture Pack / Industrial Decal Pack — масло/грязь/потёки

## ✨ VFX (прямо наши хазарды)
- [ ] **Stylized Water & Liquid VFX** — утечки/потоп
- [ ] **Ultimate Fire Pack Vol. 1** — огонь/взрыв (апгрейд статуса «Горит»)
- [ ] **Lightning & Electric Effects** — ток/живой провод

## 🏗 Стройка-тулзы (твоя зона)
- [ ] **Interior Toolkit** — быстрая обстановка интерьеров ⭐
- [ ] Ultimate Level Art Tool (ULAT) · Blockout Tools · DrCG Assistant · Simple Spline Mesh
- [ ] Modular Sewers & Tunnels · Modular Abandoned Factory · Post-Soviet World (окружение)

## 🌍 Окружение / «жизнь вокруг дома»
- [ ] **Procedural World Generator v1.4** (unrealmonster, **UE 5.7 ✓**) — ⭐ окружение вокруг дома: 217 моделей растительности + 350 сканов, деревья/кусты/камни, дома/дороги/ЖД/ЛЭП/заборы, landscape-слои, voxelized Nanite-листва. https://unrealmonster.com/procedural-world-generator-v1-4/
  - ⚠️ Ставить **локально + в .gitignore** (как все паки) — НЕ коммитить в git (раздувает историю, см. чистку 2026-06-22). Лицензия — докупить к релизу (unrealmonster = плейсхолдер).
  - Под «разруху» (юзеру нравится мусор/битые стены) — заросший/запущенный двор: бурьян, сухие деревья, поваленные заборы.

## 🧍 Персонажи (гражданские/жильцы — сейчас)
- [ ] **Rigged NPC Creator (5.5+)** — генератор модульных NPC ⭐
- [ ] Public Service NPC — Metahuman (бригада/коммунальщики)
- [ ] Survival NPC Hank Murphy / Maria Estrada · Modular Characters MegaPack (соседи)
- [ ] Construction NPC Animations — анимации рабочих
- ℹ️ свериться: **CitizenNPC** уже в проекте

## 🩸 МОНСТР — ОТЛОЖЕНО (не качать без отмашки)
Закладка на потом: Morbid Horror Pack · Horror Villains Pack · INFECTED Modular · Zombie/Mutant bundles · Horror Clown · Ghost Grandma · Wendigo · Animations For Monsters · Monster Library (звук) · NPC Eyes Sight PRO / Enhanced AI Movement / Dynamic AI · EnGore Dismemberment / Modular Limb Damage.

---

## ✅ УЖЕ ЕСТЬ — НЕ качать (подтверждение под 3 карты)
Hospital COMBO Props · Factory / Warehouse Props · Residential Houses Modular — это паки локаций дом/завод/больница, **в проекте**.

---

## Source: SHOPPING.md

# АВАРИЙКА — ИТОГ: что качаю и ставлю (ПО САЙТАМ)

**Приоритет источника:** ① **unrealmonster.com** (наш основной) → ② scrydy.ru → ③ cgdownload.net → ④ Marketplace/Fab.
**Правило:** есть на unrealmonster — берём ТАМ; scrydy/cgdownload только для того, чего на монстре нет.
**Куда:** ассет-паки → `Content/<Pack>/` · плагины → `Plugins/<Plugin>/`. ⭐ = ядро (качать первым).
⚠️ Плейсхолдеры — лицензии к релизу. UE 5.7.

---

## ① unrealmonster.com — ПРИОРИТЕТ (основное отсюда)

**UI**
- [ ] ⭐ Easy Game UI Ultimate — меню/пауза/настройки
- [ ] ⭐ Survival Horror UI Kit (PSD) — скин HUD
- [ ] ⭐ Hyper Mesh to Icon Creator — иконки слотов из мешей
- [ ] Input Prompts Pack 1500+ · Pop-up & Notification · Nine Slice Magic *(опц.)*

**Звук**
- [ ] ⭐ Ghosthack × Boom Foley — двери/шаги/фоли
- [ ] ⭐ Sonomar: Abandoned Asylum — хоррор-эмбиент
- [ ] Blastwave FX Horror Vol.3 · SFXtools Jumpscares (free) · Ocular 4000+

**Плагины → `Plugins/`**
- [ ] ⭐ Stage Zero · ⭐ NVIDIA DLSS
- [ ] ⭐ Narrative 3 (диспетчер/цели)
- [ ] Cross-Platform Voice Chat *(или Fire Chat PRO со scrydy)*

**Меши предметов**
- [ ] ⭐ Industrial Fuse Box — щиток · ⭐ Garage Tools — инструменты

**VFX**
- [ ] ⭐ Stylized Water & Liquid — потоп · ⭐ Ultimate Fire Pack — «Горит» · ⭐ Lightning & Electric — ток

**Материалы**
- [ ] ⭐ Industrial Rust & Corrosion Vol.7 · Blood Decal Bundle · Horror and Decay · Gothic Texture

**Стройка** *(твоя зона)*
- [ ] ULAT · Blockout Tools · DrCG Assistant · Simple Spline Mesh
- [ ] Modular Sewers/Tunnels · Modular Abandoned Factory · Post-Soviet World

**Персонажи**
- [ ] ⭐ Rigged NPC Creator · Public Service NPC · Construction NPC Animations · Survival NPC Hank/Maria · Modular Characters MegaPack

---

## ② scrydy.ru — только чего НЕТ на монстре

**Звук**
- [ ] ⭐ Dark Russian Voices — голоса диспетчер/жильцы
- [ ] Extreme Horror SFX · Ambient & Environment SFX Pro · Thunder & Rain

**Плагины → `Plugins/`**
- [ ] Fire Chat PRO — proximity voice-chat
- [ ] Host Migration V2 · Basic Ping · Dynamic Real Water / UIWS · Advanced AI Spawn

**Меши предметов**
- [ ] ⭐ First Aid Set — аптечка · ⭐ Radio System & Interaction — рация · Fire Hydrant Package · Toolset / AAA Carpenter's Workshop · Operating Room Pack · 101+ Everyday Props

**VFX**
- [ ] ⭐ Toxic Gas Pack — газ · Radiation VFX · 100 Explosion Pack · Advanced CRT/VHS · Advanced Dissolve

**Материалы**
- [ ] ⭐ 1000+ Mega Decal · ⭐ 12 Burning Wood (анимир.) · 400 Noise Mega Pack · Concrete/Brick/Asphalt · Advanced Glass

**Персонажи / Локации / UI**
- [ ] Modular Human Skeleton · 1950s/Bar/Agents · MoCap-анимы (Blacksmith/50 Female)
- [ ] Grunge/Horror Dark GUI Kit · 1000 Profession Icons · 3D Text Generator · PSD2UMG · Delayed Healthbar
- [ ] Локации: Soviet Apartment Megapack · Brutalist Office · Industrial City · Abandoned Hotel · [SCANS] Factory/Warehouse

---

## ③ cgdownload.net — только чего НЕТ на монстре

- [ ] ⭐ Bags & Survival Kit — снаряжение бригады *(пропс)*
- [ ] Pedestrians Vol 2 — гражданские NPC *(персонажи)*
- [ ] Elevator Kit (Blueprint) — лифты *(плагин)*
- [ ] Electrical set / lamps / wires — электрика *(пропс)*
- [ ] Horror Amplifier · Impacts/Hits/Whooshes *(звук)*
- [ ] Destruction Decal BP · Concrete Debris VFX *(материалы/vfx)*
- [ ] Локации: Industrial Factory · Asylum · Chemical Plant · Modular Neighborhood

**На обоих (scrydy + cgdownload)** — бери на любом:
- [ ] ⭐ Interior Toolkit — обстановка интерьеров *(стройка)*
- [ ] Abandoned Hospital · Post-Soviet Flat *(локации)*
- [ ] Elias Wick Advanced Fog / EasyFog / Ultra Volumetrics *(vfx)*

---

## ④ Marketplace / Fab
- [ ] ⭐ Human Vocalizations *(или Character Pain & Labor / Vol 1)* — вокалы игрока (крик/дыхание)

---

## 👹 МОНСТР — ПОТОМ (не качать без отмашки)
- unrealmonster: Animations For Monsters · Monster Library (звук) · NPC Eyes Sight PRO · Enhanced AI Movement · EnGore Dismemberment
- scrydy: 105 Zombie Anims + 281 Audio · Wendigo · BigBlob Slime · Gruesome Zombie AnimSet
- cgdownload: Mutant Monster · Parasite/Urban/Super Zombie · Zombie Hazmat/Police/Cheerleader

## ❌ НЕ качать (есть своё / уже стоит)
**EOS** → у нас **EOSCore** (стоит+настроен; Null до онлайн-теста) · **AudioToolkit Pro** → уже в `Plugins/` · **Сейв** → AvariikaSaveGame · **Инвентарь** → свой C++ · **Камера** → своя (V).

---
Детали с описаниями: `DOWNLOADS.md` (unrealmonster) · `CRAWL_FINDINGS.md` (scrydy/cgdownload).

---

## Source: СКАЧАТЬ.md

# АВАРИЙКА — ЧТО СКАЧАТЬ (сверено с тем, что уже есть)

Свод от 2026-06-19. Сверено с `Content/`, `Content/_Packs/`, `RawAssets/`, `Plugins/`.
Колонка **«Что даёт»** — конкретно под нашу игру (кооп-хоррор-бригада, карты Дом/Завод/Больница).
Приоритет источника: ① unrealmonster → ② scrydy → ③ cgdownload → ④ Fab. ⭐ = ядро.
Детали-описания: `DOWNLOADS.md` · `CRAWL_FINDINGS.md`. Полный исходный список: `SHOPPING.md`.

---

## ✅ УЖЕ ЕСТЬ — НЕ КАЧАТЬ

| Пункт | Где у нас | Что уже закрывает |
|---|---|---|
| AudioToolkit Pro | `Plugins/` (установлен) | аудио-движок (звук-менеджмент) |
| Easy Game UI Ultimate | `RawAssets/` (скачан, не импортнут) | меню/пауза/настройки |
| Garage Tools | `Content/_Packs/Garage_Tools_Props` | инструменты бригады (меши) |
| DrCG Assistant | `RawAssets/` | хелпер левел-дизайна (твоя стройка) |
| Modular Abandoned Factory + [SCANS] Factory/Warehouse | `IndustrialFactory` + `Warehouse` + `RawAssets/[SCANS]…` | окружение карты «Завод» |
| Abandoned Hospital | `Content/Hospital` + `HospitalCombo` | окружение карты «Больница» |
| EasyFog / Volumetrics | `Content/EasyFog` | объёмный туман (атмосфера хоррора) |
| Вода (Stylized/UIWS/Real Water) | `FluidFlux` + `FluidNinjaLive` | потоп/протечки/лужи |
| WorldBLD | `Plugins/` + `RawAssets/` | генерация города/дорог (окружение) |

## 🟡 ВОЗМОЖНО ЕСТЬ — ПРОВЕРИТЬ В РЕДАКТОРЕ ПЕРЕД ПОКУПКОЙ

| Пункт | Что у нас похожее | Решение |
|---|---|---|
| Survival Horror UI Kit | `RawAssets/[Survival_UI_Kit]` (HUD/Inv/Lobby/Settings) | глянуть, хватает ли скина |
| Hyper Mesh to Icon Creator | `Content/Hyper`, `Content/_Packs/Hyper` | проверить, он ли это |
| Simple Spline Mesh | `RawAssets/[Hyper Procedural Spline Toolkit v4]` | сплайны уже покрыты |
| Destruction Decal / Concrete Debris | `RawAssets/[Next Gen Destruction Toolkit]` | разрушение уже покрыто |

---

## ⬇️ СКАЧАТЬ

### ① unrealmonster.com — ПРИОРИТЕТ

**Плагины → `Plugins/`**
- [ ] ⭐ **Stage Zero** — быстрый блокаут/набивка уровней ассетами; ускоряет твою стройку карт.
- [ ] ⭐ **NVIDIA DLSS** — апскейл/производительность; тяжёлые паки (Завод 21 ГБ) будут лагать без него.
- [ ] ⭐ **Narrative 3** — диспетчер квестов/целей: «почини щиток → доберись до рации → эвакуируйся». Каркас миссии.
- [ ] ⭐ **Cross-Platform Voice Chat** — общий радио-канал между игроками (кооп). **Бэкбон рации-торга** (`SPEC_Radio_Haggle.md`) — без него диспетчер-подслушка не работает. Альтернатива — Fire Chat PRO (scrydy).

**Офлайн-голос (georgy.dev) — стек под рацию-торг (`SPEC_Radio_Haggle.md`)** — всё офлайн, в релиз чисто:
- [ ] ⭐ **Runtime Speech Recognizer** (Whisper) — распознавание речи игрока (интенты по рации, рус, GPU).
- [ ] ⭐ **Runtime Text To Speech** (Piper/Kokoro) — голос диспетчера (рус, 2800+ голосов, динамические реплики).
- [ ] ⭐ **Runtime Audio Importer** — компаньон (проигрывает синтез; нужен обоим + липсинку).
- [ ] **Runtime MetaHuman Lip Sync** *(опц., позже)* — губы NPC под голос, если диспетчер/жильцы станут видимы. ❌ НЕ брать AI Chatbot Integrator (облако, платно за запрос).

**Звук**
- [ ] ⭐ **Ghosthack × Boom Foley** — двери/шаги/фоли: каждое действие бригады звучит (иммерсия).
- [ ] ⭐ **Sonomar: Abandoned Asylum** — хоррор-эмбиент: фон напряжения на всех картах.

**UI**
- [ ] **Survival Horror UI Kit** *(если 🟡 не подойдёт)* — скин HUD под хоррор.
- [ ] **Hyper Mesh to Icon Creator** *(если 🟡 не он)* — авто-иконки предметов для слотов инвентаря из мешей.

**Меши**
- [ ] ⭐ **Industrial Fuse Box** — щиток: ключевой объект починки (заварка/замыкание/ток).

**VFX**
- [ ] ⭐ **Ultimate Fire Pack** — пожар на статусе «Горит» (у нас пока только Fire_EXP free).
- [ ] ⭐ **Lightning & Electric** — искры/дуга при замыкании щитка, удар током.

**Материалы**
- [ ] ⭐ **Industrial Rust & Corrosion Vol.7** — ржавчина/коррозия: вид заброшки на трубах/металле.
- [ ] **Blood Decal Bundle · Horror and Decay · Gothic Texture** — кровь/грязь/потёки (хоррор-детализация).

**Стройка** *(твоя зона)*
- [ ] **ULAT · Blockout Tools** — инструменты быстрой разметки уровней.
- [ ] **Modular Sewers/Tunnels** — подвалы/коллекторы (хоррор-секции).
- [ ] **Post-Soviet World** — постсоветский антураж (Дом/двор).

**Персонажи**
- [ ] ⭐ **Rigged NPC Creator** — генератор NPC (жильцы/пострадавшие).
- [ ] **Public Service NPC · Survival NPC Hank/Maria · Modular Characters MegaPack** — типажи гражданских/бригады.

### ② scrydy.ru — чего нет на монстре

**Звук**
- [ ] ⭐ **Dark Russian Voices** — голоса диспетчера/жильцов на русском (наш сеттинг). голос не подходит для диспетчера голос орка
- [ ] **Extreme Horror SFX · Ambient & Environment Pro · Thunder & Rain** — хоррор-эффекты, погода.

**Плагины → `Plugins/`**
- [ ] **Fire Chat PRO** — proximity voice-chat (слышно ближнего громче) — атмосфернее обычного.
- [ ] **Host Migration V2 · Basic Ping** — стабильность кооп-сессии (хост ушёл — не развал).
- [ ] **Advanced AI Spawn** — спавн-менеджер (пригодится под монстра/NPC позже).

**Меши**
- [ ] ⭐ **First Aid Set** — аптечка: лечение/стадии ранения.
- [ ] ⭐ **Radio System & Interaction** — рация: механика вызова/эвакуации.
- [ ] **Fire Hydrant · Carpenter's Workshop · Operating Room · 101+ Everyday Props** — реквизит Завод/Дом/Больница.

**VFX**
- [ ] ⭐ **Toxic Gas Pack** — газ: авария «утечка», зона урона/паники.
- [ ] **Radiation VFX · CRT/VHS · Advanced Dissolve** — радиация, ретро-экраны, растворение.

**Материалы**
- [ ] ⭐ **1000+ Mega Decal** — наклейки (грязь/знаки/потёки) — дёшево детализируют уровни.
- [ ] ⭐ **12 Burning Wood (анимир.)** — горящее дерево под пожар.
- [ ] **400 Noise · Concrete/Brick/Asphalt · Advanced Glass** — базовые поверхности/стекло.

**Локации / UI**
- [ ] **Soviet Apartment Megapack · Brutalist Office · Industrial City** — антураж Дом/Завод.
- [ ] **Grunge/Horror GUI Kit · 1000 Profession Icons** — иконки профессий/инструментов для UI.

### ③ cgdownload.net — чего нет на монстре

- [ ] ⭐ **Bags & Survival Kit** — снаряжение бригады (рюкзаки/сумки на поясе).
- [ ] **Pedestrians Vol 2** — гражданские NPC (толпа/жильцы).
- [ ] **Elevator Kit (Blueprint)** — рабочие лифты (вертикальные секции уровней).
- [ ] **Electrical set / lamps / wires** — электрика: провода/лампы/щитки (детализация починок).
- [ ] **Horror Amplifier · Impacts/Hits/Whooshes** — стингеры/удары (звук напряжения).
- [ ] **Interior Toolkit** — обстановка интерьеров (мебель/быт Дома).

**📦 Free Asset Bundles (cgdownload)** — ссылки:
- May 2026: https://cgdownload.net/catalog/11154-fab-unreal-engine-assets-bundle-2-may-2026/
- June 2026: https://cgdownload.net/catalog/11357-unreal-engine-assets-bundle-2-iyun-2026/

Из бандлов **БРАТЬ** (отфильтровано под наш проект):
- [ ] ⭐ **Operating Room Pack** — операционная для карты **Больница** (пропсы).
- [ ] ⭐ **Foley Props Sound FX Pack** — фоли предметов/инструментов (к нашей звук-базе `SOUND_MAP`).
- [ ] ⭐ **Poison Magic Niagara** — кандидат закрыть эффект-пробел «газ-облако» (токсично-зелёное) без ручного авторинга.
- [ ] ⭐ **Sundries — Icon Pack** — иконки слотов инвентаря (HUD-задумка: иконки вместо текста).
- [ ] **Hitreact Pro — Directional Body Parts Component** — направленные реакции на урон (улучшит `M_Hit`).

🟡 **По ситуации:** Mutant Rat/Mouse (крыса-атмосфера подвала) · n00dEmotes (кооп-эмоции) · Survival/Shopkeeper NPC Anim Set (жильцы/бригада) · Chairs / Rose Bedroom (мебель дома) · Triangle Count View Mode / Smart Measure (дев-тулзы перф/замеры) · Funeral Animations · FlexPath Smooth Navigation (AI-нав на будущее).

❌ Скип из бандлов: Feudal Japanese · Medieval Weapons · Ship Frigate · OceanShaper · все Sci-Fi · MonoWheel Bike · Stylized Soap Bubble · Toony Zombies · HEX TREE/Skill VFX · Procedural Weapon/Recoil · Necklaces for MetaHumans · Knight/Wyvern/Orcs · Plane.

### ④ Fab
- [ ] ⭐ **Human Vocalizations** *(или Character Pain & Labor)* — вокалы игрока: крик/боль/одышка (HUD без полосок — состояние через звук).

---

## 👹 МОНСТР — ПОТОМ (не качать без отмашки)
- Уже в `RawAssets/[слухач]`: **Boss Animations · Plant Monster · Spiders** — заново НЕ качать.
- Докупать по списку из `SHOPPING.md` (§👹) только после отмашки.

---

## Source: BUILD_FORMAT.md

# BUILD_FORMAT — как юзер задаёт планировку, а Claude строит

> Цель: убрать ручную расстановку 200 стен. Юзер задаёт **комнаты клеточками**,
> Claude автоматически выводит стены/углы/проёмы/пол/потолок/фундамент и строит в UE.
> Дом: Main House из RE7 (см. `HOUSE_RE7_PLAN.md`), 4 уровня: подвал(огромный)+1эт+2эт+чердак.

## Формат входа (предельно простой)

**Тетрадь в клеточку. 1 клетка = 1 метр.** Север — вверху, улица — внизу. Origin (0,0) = ЮЗ-угол (фиксирован, поэтому куски стыкуются между сообщениями).

- **Буква (A–Z) = клетка комнаты.** Одинаковые буквы = одна комната. Форма ЛЮБАЯ (Г-образная, кривая — просто закрашиваешь её клетки).
- **Точка `.` = снаружи / пусто.**
- **Двери / лестницы / вход — словами под картинкой**, не значками. Напр.: «дверь между кухней и холлом», «лестница наверх — в холле», «вход с юга», «ворота 8м на южном фасаде».

Пример (гараж 8×6 м):
```
         X→ 0 1 2 3 4 5 6 7
  Y=5      G G G G G G G G
  Y=4      G G G G G G G G
  Y=3      G G G G G G G G
  Y=2      G G G G G G G G
  Y=1      G G G G G G G G
  Y=0      G G G G G G G G
```
+ словами: ворота 8м — юг (Y=0); дверь в дом 2м — север по центру; лестница в подвал — у вост. стены; генератор — у зап. стены.

## Что Claude выводит сам (юзеру не указывать)

наружные стены по периметру + углы · внутренние перегородки на границах комнат · проёмы (двери/арки) где сказано · заливка пола · потолок · **фундамент под весь след дома** · высоты из модуля пака.

## Реальные куски пака (всё кратно 1 м)

- Наружные стены: `SM_Outside_1m/2m/3m` (выс. **350**, толстые) + углы `SM_Outside_Corner_1m/2m/3m` + внутр.углы `SM_Outside_IntCorner_*`
- Внутр. перегородки: `SM_Inside_Wall_1m_b/2m_b` (выс. **315**, тонк.) + диагональ `SM_Int_Wall_45`
- Проёмы наруж.: `SM_Outside_Door_2m/2_3m/3m`, гараж `SM_Outside_GarageDoor_8m`
- Проёмы внутр.: `SM_Inside_Door_2m/2_3m/2_4m/3m`; полотно `SM_InsideDoor` → вешать наш `ADoor`
- Пол: `SM_Floor_1x1m` (+ 2м/3м). Потолок: `SM_Ceiling_1m/2m/3m`. Фундамент: `SM_Foundation_1m/2m/3m` + углы.
- Лестницы (для 3м этажей): `AtmosphericHouse/.../SM_Stairs_straight_L_steps` (не великанские Residential).
- Подвал: `AtmosphericHouse` `BP_WallIn_Basement`, basement-лестница, worn-материалы `MI_Basement_woods_pipes_worn` (гниль).

## Высоты (диктует модуль, не свободная цифра)

Этаж в свету ≈ 3.15 м, межэтаж 3.5 м. «Давит» (подвал/чердак) — за счёт basement-модуля и скоса крыши, а не подгонки высоты.

## Конвейер

1. Юзер даёт кусок (клеточки + слова) → 2. Claude → данные → идемпотентный скрипт-строитель в `Scripts/` (world-guard на `L_Dom`) → 3. собирает в `L_Dom` → 4. ASCII-контроль занятости (дыры/наезды) + **скрин юзеру на сверку атмосферы** → 5. правки → следующий кусок.

Старт: гараж → центральный холл+лестница → крылья 1 этажа → вниз огромный подвал → 2 этаж → чердак.

_Связано: HOUSE_RE7_PLAN.md, house-build-cellgrid-method, house-floor-heights, house-asset-packs._

---

## Source: test.txt

Вот финальный список запросов + что из уже найденного брать обязательно.
📋 ЗАПРОСЫ ДЛЯ ПОИСКА (вбивай, делай скрин, кидай мне)
🥇 Скрин 1 — Кооп (САМОЕ ВАЖНОЕ, без этого нет игры):
multiplayer
EOS
steam sessions
voice chat
lobby
🥈 Скрин 2 — Окружение (локации):
hospital
apartment
factory
ship interior
basement
🥉 Скрин 3 — Звук (игра построена на звуке):
horror sound
ambient
footstep
monster sound
(или просто зайди в категорию «Звуковые эффекты»)
Скрин 4 — VFX:
niagara
smoke fire
fog
explosion
(или категория «Эффекты Niagara»)
Скрин 5 — Персонажи и монстры:
worker character
civilian
creature
monster
(или категория «Персонажи»)
Скрин 6 — Системы:
inventory
shop
save game
weather

✅ ЧТО ОБЯЗАТЕЛЬНО БРАТЬ (из уже найденного)
Окружение — больница (твой флагман):

✅ City MODULAR HOSPITAL v.2 — главный, конструктор больницы (стены, лестницы, мебель, парковка, мусор)
✅ Modern Hospital Environment (229 моделей) — добор
✅ Hospital — COMBO Prop Pack (VOL 1-6) — вся мебель больницы

Окружение — дом (для прототипа 0.1):

✅ Modular Haunted House — модульный дом, на нём проще всего собрать ПЕРВЫЙ прототип

Системы/плагины (то что реально нужно):

✅ EOSCore (Epic Online Services) — фундамент коопа 🟢🟢 КРИТИЧНО
✅ AudioToolkit Pro — обработка звука (вся игра на звуке)
✅ Hyper Dynamic Weather & Sky — ночь/дождь/гроза
✅ Easy Options Menu — меню настроек
✅ OG Main Menu System — главное меню

Мелочёвка:

✅ POS System (FREE) — переделать под магазин снаряжения в ГАЗели


⭐ ТОП-5 ПРИОРИТЕТ (если брать только самое нужное)

EOSCore — без онлайна нет коопа (фундамент)
Modular Haunted House ИЛИ City MODULAR HOSPITAL — нужна хоть ОДНА карта
AudioToolkit Pro — звук = половина хоррора
OG Main Menu + Easy Options Menu — меню (нужно для любой игры)
Hyper Dynamic Weather — атмосфера ночи/дождя

