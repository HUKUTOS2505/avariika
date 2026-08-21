# Audio, VFX, and Incident Feedback Knowledge Base

This consolidated document preserves the source material listed below. Originals are archived under Docs/Archive/Originals/.

## Sources
- `AUDIO_INVENTORY.md`
- `SOUND_MAP.md`
- `SOUND_VFX_TZ.md`
- `INCIDENT_FX_MAP.md`
- `EFFECTS_INVENTORY.md`

---

## Source: AUDIO_INVENTORY.md

# Аудио: что есть / кандидат / чего не хватает

Легенда статуса: ✅ ЕСТЬ — уже импортировано в `Content/` · 🟡 КАНДИДАТ — лежит в `RawAssets/`, нужен импорт · ❌ НЕТ — нет годного источника у нас.

Опорные импортированные наборы: `Content/Audio/SFX/` (12 кураторских стемов), `Content/Survival_SFX/{Movement,Survival,Craft,User_Interface}/`.

> Сводка по нашим пакам в `RawAssets/звуки/`: Ghosthack×BOOM Bundle (огромный — Garage-инструменты, Explosives, Fire/Water/Lightning foley, Whoosh/Body Fall, Hyperion-импакты, Weather), Sonomar Abandoned Asylum (эмбиент лечебницы — под больницу), FootstepSoundComponent (система шагов + бетон + Niagara-пыль), Survival SFX (уже импортирован), Rope Creak / Earthquake / HAZMAT (мелочь), и монстр-паки (SoundMorph Monster Within, Monster Sound FX, SFXtools Jumpscares, BOOM Dogs) — на будущее.

---

## 1. Движение (MOVEMENT)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Шаги ходьба по поверхности | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Movement/Walk_{grass,gravel,wood,stone}` (×4) | Есть grass/gravel/wood/stone. `stone`≈бетон. Нет **tile/metal** — добрать из FootstepSoundComponent. |
| Шаги бег | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Movement/Run_{grass,gravel,wood,stone}` (×4) | Есть и `Jog_` (средний темп). | иногда лагает звук накладывается
| Шаги краулинг/присед | ❌ НЕТ | — | Отдельного crouch-стема нет. Временно — тихий `Walk_` со сдвигом питча. |
| Прыжок (отталкивание) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Movement/Jump_{grass,gravel,stone,wood}` (×4) | — |
| Приземление (тяжёлый удар) | 🟡 КАНДИДАТ | Ghosthack `Ultimate Fighting Foley/Body Fall` (15–39) | Бетон/кожа thud, прямое попадание. Альт — `Hyperion/Impacts`. |
| Скольжение по пене | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Movement/Slide_{stone,wood}` (×4) | Сухой слип-стем. Настоящий «мокрый» squelch пены — в шоппинг-лист. |
| Падение (споткнулся, тело) | 🟡 КАНДИДАТ | Ghosthack `Ultimate Fighting Foley/Body Fall` (15) | Тот же набор, что и приземление; импортировать один раз. |
| Волочение раненого (скрежет+кряхт) | ❌ НЕТ | скрежет 🟡 Ghosthack `Ultimate Fighting Foley/Movement` (13, кожа/трение) | Скрежет покрываем, **кряхтение = вокал** → шоппинг-лист (мужской vocal foley). |

---

## 2. Витал (VITALS)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Сердцебиение (loop) | ✅ ЕСТЬ | `Content/Audio/SFX/Heartbeat` | — |
| Паник-вздох (gasp) | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Крик боли (ранение) | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Тяжёлое дыхание при ползании | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Бинтование/лечение | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/First_aid_*` (10) | Чистый foley бинта/аптечки. | работает странно аптечка применяется секунд 10 а бинтование 2.5 секунды что то менять  надо
| Кряхтение при подъёме напарника | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Кашель | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Икота | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Запыхался (winded) | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Туалетный инцидент (комедийно, тихо) | ❌ НЕТ | жидкостной слой 🟡 Ghosthack `Magic Elements/Magic Water/Foleys` (glug) | Жидкость покрываема, реакция/«пшик» = вокал. |

> Витал — это в основном **дыра по мужскому вокалу** (подтверждается каталогами: ни в одном паке нет настоящих человеческих эффортов).

---

## 3. Предметы (ITEMS)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Подбор предмета (металл/пластик) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Metal_item_pick_up` | Пластик-вариант не критичен. |
| Бросок/дроп предмета (thud) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Metal_item_drop` | — |
| Тик переключения слота | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Button_hover` | — |
| Фонарик вкл/выкл (клик) | ✅ ЕСТЬ | `Content/Audio/SFX/FlashClick` | — |
| Фонарик сел: клик + испуг-вздох | ❌ НЕТ | клик ✅ `Content/Audio/SFX/FlashClick` | Сам клик есть; **испуг-вздох = вокал** → шоппинг-лист. |
| Рация вкл (клик + static-блип) | ✅ ЕСТЬ | `Content/Audio/SFX/RadioBlip` | — |
| Рация: фон статика (loop) | ❌ НЕТ | — | Нужен loop радиошума → шоппинг-лист (comms-пак). `RadioBlip` — только one-shot. |
| Зажигалка: щелчок+затяжка (каст) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/Lighter_*` (5) | Щелчок+поджиг в стеме. |
| Кофе/термос: глоток+вздох (каст) | ✅ ЕСТЬ | `Content/Audio/SFX/DrinkGlug` | Глоток есть; финальный вздох — вокал (мелочь). |
| Вставка батарейки (клик, каст) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/User_Interface/Metal_item_pick_up` | Альт точнее — Ghosthack `Garage/Nut Driver Clatter`. |
| Огнетушитель: спрей под давлением (loop) | 🟡 КАНДИДАТ | Ghosthack `Steampunk Foley/Air Burst/Pneumatic Pump` (27); альт `Garage/Tire Pressure Control` (4) | Нет настоящего CO2-пшика → импорт пневматики или докупка (шоппинг-лист). |
| Бросок: зарядка (charge) | 🟡 КАНДИДАТ | Ghosthack `Ultimate Fighting Foley/Whoosh` (30) | Замах/нарастание. |
| Бросок: свист (whoosh) | 🟡 КАНДИДАТ | Ghosthack `Ultimate Fighting Foley/Whoosh` (30) | Тот же набор. |
| Бросок: удар (impact) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Craft/Anvil_hit_*` / `User_Interface/Metal_item_drop` | Альт — Ghosthack `Hyperion/Impacts`. |
| Ловушка: установка (клик) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/Trap_place_*` (5) | — |
| Ловушка: взвод (beep) | ❌ НЕТ | — | Электронный бип → синтез/UI-пак. Времянка — `Button_press`. |
| Ловушка: срабатывание (snap) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/Trap_snap_*` (5) | — |
| Прожектор-тренога: клац + гул лампы | 🟡 КАНДИДАТ | гул: Ghosthack `Magic Lightning/Foleys` (Fluorescent Lamp/Low Hum); клац: импорт. `Metal_item_drop` | Гул лампы — импортировать electric foley. |
| Био-блоб: бросок + шлепок | 🟡 КАНДИДАТ | splat: Ghosthack `Magic Water/Foleys` (Impact Splash); throw: `Whoosh` | Будущая механика; собрать слоями. |

---

## 4. Починка (REPAIR)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Вставка расходника (метал. клик) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Metal_item_pick_up` | Альт — Ghosthack `Garage/Nut Driver Clatter`. |
| Заправка топлива: глюк/налив (loop) | 🟡 КАНДИДАТ | Ghosthack `Magic Elements/Magic Water/Foleys` (Bubble Bottle Fill Glug, 15) | Времянка — `Content/Audio/SFX/DrinkGlug` (one-shot). Нужен именно loop-залив. |
| Кабель: разматывание (loop) | ❌ НЕТ | прибл. Survival_SFX `Content/Survival_SFX/Craft/Crafting_cloth_item_*` (шуршание) | Настоящего unspool нет → шоппинг-лист. |
| Тестер: контакт (beep, hit) | ❌ НЕТ | прибл. `Content/Survival_SFX/User_Interface/Button_press` | Электронный бип → синтез/UI-пак. |
| Тестер: ошибка (buzz, miss) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Error` | Прямое попадание. |
| Вентиль: трещотка (на оборот) | 🟡 КАНДИДАТ | Ghosthack `Steampunk Foley/Garage/Impact Wrench Screw` (15) | Годного импортированного нет. |
| Вентиль: срыв резьбы + шипение | 🟡 КАНДИДАТ | Ghosthack `Garage/Tire Pressure Control` (deflate, 4) | Слой шипения можно добрать из `Content/Audio/SFX/GasHiss`. |
| Генератор: натяг шнура (loop) | 🟡 КАНДИДАТ | Ghosthack `Steampunk Foley/Garage/Hydraulic Lift` (8, мех. натяг) | Идеала (pull-cord) нет → шоппинг-лист. |
| Генератор: захват шнура (snap) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Survival/Trap_snap_*` | Резкий щелчок-защёлка. |
| Обратная вспышка (dull thud) | 🟡 КАНДИДАТ | Ghosthack `Explosives/Explosion Indoor` (12, тихий) / `Body Fall` | Глухой хлоп. |
| Двигатель: запуск (1x) | ✅ ЕСТЬ | `Content/Audio/SFX/EngineStart` | — |
| Двигатель: холостой ход (loop, весь забег) | ❌ НЕТ | прибл. Ghosthack `Garage/Electric Drill Idle` (9, мотор-гул) | **EngineStart — только запуск.** Нужен idle-loop двигателя → шоппинг-лист. |
| Сварка: дуга, гул/треск (loop) | ✅ ЕСТЬ | `Content/Audio/SFX/WeldBuzz` | — |
| Починка завершена (chime) | ✅ ЕСТЬ | `Content/Audio/SFX/RepairDone` | Альт — UI `Save`. |
| Костыль-сборка: возня (loop) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Craft/Building_item_remove_*` / `Crafting_wood_item_*` | Альт точнее — Ghosthack `Garage/Tire Changing Machine` (12). |
| Костыль: провал (clank) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Craft/Anvil_hit_*` / `User_Interface/Metal_item_drop` | — |
| Электро-замыкание/zap | ✅ ЕСТЬ | `Content/Audio/SFX/ElectricZap` | — |

---

## 5. Опасности (HAZARDS)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Утечка газа: ровное шипение (loop, 3D) | ✅ ЕСТЬ | `Content/Audio/SFX/GasHiss` | ⚠️ Вероятно слоёный плейсхолдер (чистого газ-хисса в RawAssets нет) — проверить качество, при релизе докупить настоящий. |
| Газовое облако: усиленное шипение | ✅ ЕСТЬ (прибл.) | `Content/Audio/SFX/GasHiss` (питч/слой) | Альт — Ghosthack `Garage/Tire Pressure Control`. |
| Газовый ВЗРЫВ (boom+rumble+debris) | ✅ ЕСТЬ | `Content/Audio/SFX/Explosion` | Для разнообразия — Ghosthack `Explosives/Explosion Gas` (40). |
| Пожар после взрыва (crackle, loop) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/Campfire_*` (5) | Альт — Ghosthack `Medieval.../Fire` (9). |
| Перегруз сети: хлопок реле + спад гула | 🟡 КАНДИДАТ | `Content/Audio/SFX/ElectricZap` + Ghosthack `Magic Lightning/Foleys` (Low Hum) | Собрать слоями; хлопок реле — из металл-клика. |
| Восстановление питания: гул + свет | 🟡 КАНДИДАТ | Ghosthack `Magic Lightning/Foleys` (Low Hum Sequence, Fluorescent Lamp) | Импортировать electric foley. |
| Хоррор-скрипы/стуки/капли (рандом) | 🟡 КАНДИДАТ | Sonomar `Abandoned Asylum/Ambience - Room Tone/...Basement_Water_Drips` (3) | Скрипы — Ghosthack `Fire`/дерево; импорт Sonomar. |
| Капель (ambient drip) | 🟡 КАНДИДАТ | Sonomar `...Basement_Water_Drips_SC-AA_026` (3) | Альт — Ghosthack `Magic Water` drip. |

---

## 6. Диспетчер / соц (DISPATCHER/SOCIAL)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Одиночный бип рации | ✅ ЕСТЬ | `Content/Audio/SFX/RadioBlip` | — |
| Паник-скрим игрока | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Толчок напарника: thud + кряхт | ❌ НЕТ | thud ✅ Survival_SFX `Content/Survival_SFX/Survival/Punch_*` (5) | Удар есть; **кряхтение = вокал**. |
| Оператор: тумблер монитора/экран | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/User_Interface/Button_press` | Альт — `FlashClick`. |

---

## 7. UI

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Наведение (hover tick) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Button_hover` | — |
| Клик кнопки | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Button_press_*` (2) | — |
| Открытие/закрытие меню (swoosh) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/{Open,Close}_menu_*` | — |
| Покупка (ka-ching) | ❌ НЕТ | прибл. `Content/Survival_SFX/User_Interface/Save` | Кассы нет → UI/casino-пак. |
| Не хватает денег (error buzz) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Error` | — |
| Старт забега (sting) | 🟡 КАНДИДАТ | Ghosthack `Hyperion - Trailer Hits & Atmospheres/Impacts` (72) | Импорт или короткий муз-стинг-пак. |
| Победа (sting) | 🟡 КАНДИДАТ | Ghosthack `Hyperion/Impacts` | Времянка — `RepairDone`. |
| Поражение (sting) | 🟡 КАНДИДАТ | Ghosthack `Hyperion/Drones & Atmospheres` (20) | — |
| Акт о работе (шуршание бумаги) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Craft/Crafting_cloth_item_*` | Ткань вместо бумаги; настоящий paper rustle — мелочь в шоппинг-лист. |

---

## 8. Эмбиент (AMBIENT)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Фоновый дрон уровня (больница/котельная, вент) | ✅ ЕСТЬ | `Content/Audio/SFX/Ambient_Boiler` | Для палат/коридоров добрать Sonomar `Large Hall` / `Long Corridor` (🟡). |
| Ночной ветер | 🟡 КАНДИДАТ | Ghosthack `Medieval.../Wind` (9) | — |
| Дождь + гром | 🟡 КАНДИДАТ | Ghosthack `Medieval.../Weather` (35–44) | Medium/Strong Rain, Thunder Distant/Close. |
| Электрический гул | 🟡 КАНДИДАТ | Ghosthack `Magic Lightning/Foleys` (Low Hum, Fluorescent Lamp) | Частично покрывает `Ambient_Boiler`. |

---

## 9. Монстр — будущее (MONSTER, только каталог)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Тяжёлые шаги | 🟡 КАНДИДАТ | SoundMorph `Monster Within/_(WAVs)_Footsteps` (272) | Импорт только при старте работ по монстру. |
| Дыхание/рык | 🟡 КАНДИДАТ | SoundMorph `Monster Within/_(WAVs)_Breathing` (55) | Альт-орг. — BOOM `DOGS.WAV/Growl` (обработать). |
| Стинг «заметил» (alerted) | 🟡 КАНДИДАТ | SoundMorph `Monster Within/_(WAVs)_Reaction` (156) | — |
| Джампскейр-скрим | 🟡 КАНДИДАТ | `SFXtools Jumpscares` (41, JS_Jumpscare_*) | Готовые стинги. |
| Музыка погони | 🟡 КАНДИДАТ | SoundMorph `Monster Within/_(WAVs)_Charge` (34) | Передаёт преследование, но это **не муз-трек** → отдельная музыка позже. |

---

## ❌ Чего не хватает (купить/сгенерить)

**A. Мужской вокальный foley (самая большая дыра — у нас нет ни одного человеческого эффорта):**
паник-вздох, крик боли, тяжёлое дыхание при ползании, кряхтение при подъёме, кашель, икота, «запыхался», туалетная реакция, испуг-вздох (севший фонарик), вздох после кофе, кряхтение при волочении тела и толчке напарника, паник-скрим игрока.
→ Купить **male vocal efforts / breaths / pain pack** (напр. «Male Character Voices: Efforts, Pain, Breathing» на Fab/asset-store) **или записать своего актёра** — самый дешёвый и качественный путь для коопа.

**B. Настоящий газ/давление/двигатель (индустриальные loop'ы):**
огнетушитель CO2-спрей (loop), **проверить/заменить `GasHiss`** на настоящее газовое шипение под давлением, **холостой ход двигателя (loop на весь забег)**, разматывание кабеля (loop), трещотка вентиля.
→ Купить **automotive/garage + pressurized gas & steam pack** и **engine idle loop pack** (часть закрывается импортом Ghosthack Garage — см. ниже, но idle двигателя и CO2 лучше докупить отдельно).

**C. Электроника / UI-стинги / радио:**
покупка (ka-ching), взвод ловушки (beep), контакт тестера (beep), фон радиошума (loop), стинги старт/победа/поражение.
→ **UI/casino SFX pack** (ka-ching, бипы) + **comms/radio static pack** (loop рации) + короткий **musical sting pack** (старт/победа/поражение можно временно собрать из Ghosthack Hyperion).

**D. Мелочь:**
crouch-шаги (можно из Walk питчем), «мокрый» squelch скольжения по пене, шуршание бумаги (акт).
→ Низкий приоритет, закрыть импровизацией из имеющихся стемов.

---

## ✅ Импортировано курировано (2026-06-14) — готово к развеске

23 SoundWave в `Content/Audio/` (по семейству на событие, не целые папки — лицензии+диск). Гиганты Sonomar и большие лупы обрезаны до 12–25 c. Лупы (★) уже `looping=True`.

**Статус врезки (`2f3930f` + `5160a47`):** 🔌 ВЖАТО в код — Repair_ValveRatchet/ValveStrip, Repair_GenPull_Loop, Repair_Insert, Repair_EngineIdle_Loop, Repair_FuelFill_Loop, Repair_JuryRig_Loop (колхоз), Hazard_ExplosionGas, Hazard_Rumble_1 (рокот-хвост взрыва), Hazard_LampHum_Loop (гул прожектора AFloodlight), Item_ExtinguisherSpray_Loop, Foley_BodyFall; + из Survival: Metal_item_drop (бросок), Button_hover (смена слота). ⏳ Импортировано, ждёт механики/левел-дизайна — Hazard_FireLoop (нет персистентного огня), Hazard_Overload_1 (есть ElectricZap), Amb_* (ставить 3D AmbientSound в реальных уровнях — не в тест-боксе). Rope creak (хоррор-эмбиент) — нужна система случайных скрипов.

| Ассет (`/Game/Audio/...`) | Событие (куда вешать) | Источник |
|---|---|---|
| `SFX/Repair/Repair_ValveRatchet_1..3` | Труба: докрутка вентиля (на тык, 3 варианта) | Garage Impact Wrench Screw |
| `SFX/Repair/Repair_ValveStrip_1..2` | Труба: срыв резьбы + шипение | Garage Tire Pressure Deflate |
| `SFX/Repair/Repair_GenPull_Loop` ★ | Генератор: натяг шнура (пока тянешь) | Garage Winch Spin |
| `SFX/Repair/Repair_Insert_1..2` | Вставка расходника (метал. клик) | Garage Nut Driver Clatter |
| `SFX/Repair/Repair_EngineIdle_Loop` ★ | Генератор: холостой ход (весь забег) — **времянка** | Garage Flue Gas Installation Running |
| `SFX/Repair/Repair_JuryRig_Loop` ★ | Колхоз-сборка: возня | Garage Tire Changing Machine |
| `SFX/Repair/Repair_FuelFill_Loop` ★ | Заправка топлива: налив | Magic Water Bubble Fill Glug |
| `SFX/Hazard/Hazard_FireLoop` ★ | Пожар после взрыва / газ-горелка | Magic Fire Gas Flame Burn Loop |
| `SFX/Hazard/Hazard_LampHum_Loop` ★ | Гул лампы (прожектор) / восстановление света | Magic Lightning Fluorescent Lamp |
| `SFX/Hazard/Hazard_Overload_1` | Перегруз/дефект генератора (электро) | Magic Lightning Defect Generator |
| `SFX/Hazard/Hazard_ExplosionGas_1` | Взрыв газа (вариант, лучше плейсхолдера) | Explosives Explosion Gas Debris |
| `SFX/Item/Item_ExtinguisherSpray_Loop` ★ | Огнетушитель: спрей под давлением | Air Burst Pneumatic Pump |
| `SFX/Foley/Foley_BodyFall_1..2` | Приземление / падение тела | Ultimate Fighting Body Fall Concrete |
| `Ambient/Amb_Hall_Loop` ★ | Эмбиент: большой зал/палата | Sonomar Large Hall |
| `Ambient/Amb_Corridor_Loop` ★ | Эмбиент: коридор | Sonomar Long Corridor |
| `Ambient/Amb_WaterDrips_Loop` ★ | Эмбиент: капель/подвал | Sonomar Basement Water Drips |
| `Ambient/Amb_Rain_Loop` ★ | Эмбиент: дождь (если погода) | Medieval Weather Medium Rain |
| `Ambient/Amb_RainThunder_1` | Раскат грома (1×) | Medieval Weather Rain Thunder |

> Развеска (репоинт `FObjectFinder` + новые лупы idle/огнетушитель/эмбиент в коде) — в окно сборки. Скрипты импорта: `Scripts/import_audio_batch.py` + `Scripts/set_audio_looping.py`.

## 📦 Импортировать в проект (приоритет)

Конвенция: разовые SFX → `Content/Audio/SFX/<Группа>/`, эмбиенты → `Content/Audio/Ambient/<Группа>/`. Импорт = robocopy дерева пака в целевую папку (см. memory: asset import method), либо headless `UnrealEditor-Cmd`.

1. **Sonomar Abandoned Asylum** (эмбиент + капель + котельная + подвал-шаги) → `Content/Audio/Ambient/Asylum/`.
   Закрывает: фоновый дрон палат/коридоров, капель, хоррор-фон, опасный газ/мех-фон. Высший приоритет — атмосфера уровня.
2. **FootstepSoundComponent** (бетон + Blueprint-логика поверхностей + Niagara-пыль) → `Content/FootstepSoundComponent/` (сохранить пути `/Game/FootstepSoundComponent/`).
   Закрывает: бетонные шаги + переиспользуемая детекция поверхностей под недостающие **tile/metal**, и пыль-VFX.
3. **Ghosthack Steampunk Garage** (Impact Wrench, Tire Pressure Control, Hydraulic Lift, Pneumatic Pump, Electric Drill Idle, Tire Changing Machine, Nut Driver Clatter) → `Content/Audio/SFX/Garage/`.
   Закрывает: трещотка/срыв вентиля, шнур генератора, пневмо-спрей огнетушителя, возня костыля, мотор-гул (времянка под idle).
4. **Ghosthack Magic Fire/Water/Lightning Foleys** (реалистичные слои, игнор спелл-вушей) → `Content/Audio/SFX/Elemental/`.
   Закрывает: огонь после взрыва, налив топлива (glug-loop), капель, электро-гул/zap/флуоресцент, восстановление питания.
5. **Ghosthack Whoosh + Body Fall + Hyperion Impacts** → `Content/Audio/SFX/Foley/`.
   Закрывает: бросок (charge/whoosh/impact), приземление/падение, UI-стинги старт/победа/поражение.
6. **Ghosthack Medieval Weather + Wind** → `Content/Audio/Ambient/Weather/`.
   Закрывает: дождь+гром, ночной ветер.
7. **(Будущее, по отмашке)** SoundMorph Monster Within + SFXtools Jumpscares → `Content/Audio/Monster/`.
   Только когда стартуют работы по монстру (см. project-status: монстра не делать без отмашки).

---

## Source: SOUND_MAP.md

# SOUND_MAP — звук под механики «Аварийки»

_Курация 2026-06-19. База `Content/Audio/Lib/` (Ghosthack + Abandoned Asylum + Monster Within), импорт `.uasset`, локально (gitignore)._
_Пути = `/Game/Audio/Lib/<кат>/<файл>`. Пара к визуалу — см. `EFFECTS_INVENTORY.md`._

---

## Лучшее под каждую механику

| Механика | ⭐ Лучший файл (`Lib/…`) | Альтернатива | Как использовать |
|---|---|---|---|
| **Дверь — открыть** | `door_impact/Ghosthack-MFW_Gate_Wood_Large_Open` | `door_impact/Ghosthack-SF_Household_Cabinet_Cupboard_Slide_Door_Close_01` (лёгкая) | `ADoor` OpenSound; большие деревянные двери дома |
| **Дверь — закрыть/хлопок** | `door_impact/Ghosthack-SH_Hit_Metal_Door_Slam` | `door_impact/Ghosthack-MFW_Gate_Wood_Large_Close` | CloseSound; металл = жёстче, дерево = мягче |
| **Гаражные ворота** | `door_impact/Ghosthack-SH_Hit_Metal_Rattling_Gate` | `door_impact/Ghosthack-MFW_Gate_Stone_Large_Close` | громкий лязг ворот |
| **Вентиль (газ/вода) — крутить** | `repair_tool/Ghosthack-SF_Garage_Car_Fuel_Tank_Cap_Screw_Open_01` | `repair_tool/Ghosthack-SF_Garage_Workbench_Vise_Handle_01` (колесо-вентиль) | перекрытие трубы; `_Close_01` на обратный ход |
| **Щиток/рубильник — щёлк** | `repair_tool/Ghosthack-H_Impact_Metal_Klonk` | `repair_tool/Ghosthack-H_Impact_Metal_Strike` | `APowerSwitch` toggle (плейсхолдер — чистого брейкера в базе нет) |
| **Питание под напряжением (гул)** | `electrical/Ghosthack-M_Ambience_Power_Line` | — | ЛУП пока щиток живой (зона под током) |
| **Искры/КЗ провода** | `electrical/Ghosthack-ME_Magic_Fire_Weld_Machine_Inventor_Multiple_Short_Electrode_Sparkling_Hits` | — | живой провод/замыкание; пара к `NS_Spark_Continuous`/`NS_TeslaCoil` |
| **Сварка (дуга)** | `repair_tool/Ghosthack-ME_Magic_Fire_Weld_Machine_Inventor_Slide_Electrode_On_Metal_Sparkles_Soft` | `electrical/…_Electrode_Sparkling_Hits` | заварка трубы; пара к искрам Niagara |
| **Утечка газа (шипение)** | `gas/Ghosthack-SF_Garage_Tire_Pressure_Control_Deflate_01` | `gas/Ghosthack-SF_Air_Burst_Train_Pressure_Release_Squeak` | ЛУП у `Repairable_GasPipe`; пара к `NS_Smoke_7_acid` |
| **Газ поджёгся (горит)** | `gas/Ghosthack-ME_Magic_Fire_Gas_Flame_On_01` (поджиг) + `ambience_house/Ghosthack-ME_Magic_Fire_Gas_Flame_Burn_Loop` (горение) | `gas/Ghosthack-ME_Magic_Fire_Gas_Flame_Burn_Strong_Long_01` | one-shot поджиг → переход в луп |
| **Взрыв газа** | `door_impact/Ghosthack-E_Explosion_Gas_Real_Gas_Explosion_Close_Large_01` | `door_impact/…_Close_Small_Indoor_01` (в помещении) | пара к `NS_Explosion`; indoor-вариант для комнат |
| **Прорыв трубы (струя)** | `water/Ghosthack-SF_Garage_Mechanic_Sink_Running_Long_01` | `…_Long_02` | ЛУП у `Repairable_WaterPipe`; пара к `NS_WaterHose_SingleProjection` |
| **Капель (потолок/труба)** | `water/Abandoned_Asylum_Room_Tone_Basement_Water_Drips_SC-AA_026_A` | `…_026_B` (вариант для рандома) | пара к `Dripping_Static_Mesh`; подвал/мокрые зоны |
| **Рация** | `radio/Ghosthack-H_Transition_Old_Radio` | — | вызов диспетчеру / приём задания |
| **Ремонт — ключ/гайковёрт** | `repair_tool/Ghosthack-SF_Garage_Impact_Wrench_Use_01` | `…_Impact_Wrench_Screw_Single_01` | Hold-этап ремонта (откручивание) |
| **Ремонт — удар молотком** | `repair_tool/Ghosthack-X_Hit_Metal_Workshop` | `repair_tool/Ghosthack-H_Impact_Metal_Strike` | забивка/правка |
| **Дрель** | `repair_tool/Ghosthack-SF_Garage_Electric_Drill_Idle_Short_01` | `…_Idle_Very_Short_01` | сверление крепежа |

## Атмосфера / фон (амбиент-аварийка)

| Зона/событие | Файлы (`Lib/…`) |
|---|---|
| **Комнатный тон (дом/коридор/зал)** | `ambience_house/Abandoned_Asylum_Room_Tone_*` (Large_Hall / Long_Corridor / Large_Room / Public_Bathroom — луп под комнату) |
| **Подвал/котельная** | `ambience_house/Abandoned_Asylum_Room_Tone_Boiler_Room_Basement_SC-AA_024/025` |
| **Скрипы конструкции (напряжение)** | `creak_struct/Ghosthack-SF_Destruction_High_Pitch_Creak_01/02`, `…_Wood_Slide_Creak_01/02`, `…_Floorboard_Crack_Long_Single` — рандом-триггеры |
| **Обвал/разрушение** | `door_impact/Ghosthack-E_Demolition_Collapsing_Building_*`, `creak_struct/Ghosthack-X_Boom_Dark_Metal_Bones` |
| **Джампскейр-стингеры** (под монстра, gated) | `jumpscare/JS_Jumpscare_*`, `jumpscare/Ghosthack-SH_Stinger_*`, `jumpscare/Monster_Within_*` |

---

## Что собрать дальше (SoundCue)

Для механик с несколькими дублями — обернуть в **SoundCue с Random + Modulation** (чтобы не повторялось):
- **Дверь** (open/close × дерево/металл) — Random-нода.
- **Скрипы** (`creak_struct`, 13 файлов) — Random + питч-модуляция, рандом-таймер в амбиенте.
- **Ремонт-удары** (wrench/hammer/strike) — Random на каждый тик Hold-этапа.
- **Капель** (026_A/B) — Random + случайный интервал.

Лупы (газ-шип, вода-струя, power-line гул, газ-горение) — отдельные Cue с `Looping`.

## Привязка в коде (требует ребилда — когда закроешь редактор)
Свойства уже есть в C++ (напр. `ADoor::OpenSound`); проставить дефолты Cue на CDO/в `BeginPlay` через `LoadObject`. Аварийные лупы — `UAudioComponent` на `ARepairable`, старт/стоп по состоянию (сломан→играет). Я распишу точечно, когда дойдём до запекания.

## Пробелы базы
- **Чистого «рубильник/брейкер-щёлк» нет** — сейчас металл-klonk-плейсхолдер. Кандидат на докачку (Boom Foley / Ghosthack switch).
- **Чистого «вентиль-колесо скрип» нет** — fuel-cap-screw близко, но не идеально.
- **Голоса диспетчера/жильцов** — в базе нет (план: *Dark Russian Voices*, scrydy — см. `СКАЧАТЬ.md`).

---

## Source: SOUND_VFX_TZ.md

# ТЗ: Звук и эффекты «Аварийки»

Полная карта аудио + VFX. Для КАЖДОГО действия/события — нужный звук (или явно «без звука»), тип, поведение, и эффект. По этому документу подключаем пак чисто и сразу правильно. Цель — «шедевр»: ничего случайного, всё к месту.

Статусы: ✅ нужен · 🔇 без звука (осознанно) · 🎚 фоновый/эмбиент · 🟡 опц.
Тип звука: **1×** разовый · **loop** зацикленный (живёт по состоянию) · **cast** привязан к длительности действия (старт→стоп, гаснет при отмене).

---

## 0. Принципы (важно для всех звуков)

1. **Сеттинг — современный, реалистичный.** Никакого sci-fi/фэнтези/мульта. Грубый честный foley: металл, пластик, электрика, дизель, ткань, бетон.
2. **Мировые звуки — 3D с затуханием по расстоянию.** Вблизи слышно, дальше тише, очень далеко — почти тишина (как в жизни). Радиус подбираем под событие (мелочь ~5–10 м, взрыв — на всю карту).
3. **Личные звуки игрока (сердцебиение, одышка) — 2D, только сам слышишь.** Не пространственные.
4. **Кооп: всё мировое — через мультикаст/репликацию**, чтобы слышали все. Личное — локально.
5. **Длительные действия (lecture/cast) — звук живёт ПО СОСТОЯНИЮ:** старт при начале, **стоп при завершении И при отмене**. Никаких «звук доиграл после отмены».
6. **Лупы — это реально зацикленные файлы** (не одношаговый клип, который дёргается). Шаги/заливка/дуга/огонь — длинные бесшовные лупы.
7. **Вариативность:** где звук частый (шаги, тычки, удары) — 3–5 вариантов клипа, выбор случайный, чтобы не «долбило одно и то же».
8. **Громкости — единая шкала** (мастер-микс): шаги тихие, взрыв громкий; настраивается в `EditAnywhere`.
9. **Плейсхолдеры → к релизу заменить/лицензировать** (см. ASSETS.md).

---

## 1. Движение / локомоция (звук на каждом персонаже, по его скорости; 3D)

| Событие | Звук | Тип | VFX |
|---|---|---|---|
| Ходьба | шаги по поверхности (бетон/плитка/дерево/металл — по полу) | loop (цикл шага) ✅ | мелкая пыль под ногой 🟡 |
| Бег | шаги быстрее/тяжелее | loop ✅ | пыль 🟡 |
| Присяд-ходьба | тихие крадущиеся шаги | loop ✅ | — |
| Прыжок (отрыв) | толчок/выдох | 1× ✅ | — |
| Приземление | удар стоп о пол (мягче в присяде) | 1× ✅ | пыль/брызги 🟡 |
| Скольжение по пене | скрип-скольжение | loop, пока скользит ✅ | след пены 🟡 |
| Споткнулся/упал | вскрик + удар тела | 1× ✅ | — |
| Тащит раненого (волочёт) | волочение по полу + кряхтение | loop ✅ | след 🟡 |

> **Поверхности:** идеально — определять пол (бетон/плитка/дерево/металл/вода) и подбирать набор. Минимум на старт — бетон/плитка (больница). Пак шагов нужен с разбивкой по поверхностям.

---

## 2. Витал / состояние игрока (личные — 2D, кроме оговорённых)

| Событие | Звук | Тип | VFX |
|---|---|---|---|
| Паника растёт | сердцебиение + сбитое дыхание, чем выше — тем чаще/громче | loop, 2D личный ✅ | пульсация виньетки экрана (есть) |
| Паническая «отключка»/пик | резкий вдох, звон в ушах | 1×, 2D ✅ | вспышка/блюр 🟡 |
| Ранение (падение в «down») | вскрик боли + падение | 1× ✅ | — |
| Ползёт раненый | тяжёлое дыхание + волочение | loop, тихо ✅ | капли крови (декаль) 🟡 |
| Лечение/перевязка (аптечка) | разматывание бинта/пшик спрея | **cast** (всю перевязку), 3D ✅ | — |
| Подъём напарника | «оп», помощь встать | 1× ✅ | — |
| Адреналин (низкий HP) | глухой пульс/тиннитус | loop тихий, 2D 🟡 | лёгкий красный край 🟡 |
| Кашель (провонял газом/химией) | кашель | 1× периодически ✅ | — |
| Икота (полный пузырь) | «ик» | 1× периодически ✅ | — |
| Одышка (выносливость в 0) | громкое дыхание | loop ~3 c ✅ | — |
| Санитарный инцидент | звук… (комедийно, негромко) | 1× 🟡 | — |
| «Воняет»/амбре | — | 🔇 (можно жужжание мух 🟡) | рой мух/зелёная дымка 🟡 |

---

## 3. Предметы: подбор/смена/применение

| Событие | Звук | Тип | VFX |
|---|---|---|---|
| Подобрал предмет | взял/щёлк | 1× ✅ | — |
| Бросил/уронил | падение предмета о пол | 1× ✅ | — |
| Смена слота (1–5) | тихий тук/шорох | 1× 🟡 | — |
| Передача напарнику (ПКМ) | «на, держи» шорох | 1× 🟡 | — |
| Фонарь вкл/выкл (F) | щелчок тумблера | 1× ✅ | луч появляется (есть) |
| Фонарь сел | гаснет + щелчок + (испуг-вдох) | 1× ✅ | свет гаснет (есть) |
| Рация вкл/выкл (ЛКМ) | щелчок + короткий шум эфира | 1× ✅ | — |
| Рация шипит (вкл, в кармане) | тихий эфир/помехи | loop тихий 🟡 | — |
| Сигарета (закурить, Calm) | чирк зажигалки + затяжка | **cast** ✅ | дымок у лица 🟡 |
| Кофе/термос (Drink) | глоток + выдох «ах» | **cast** ✅ (нужен глоток!) | пар от кружки 🟡 |
| Батарея в фонарь (Recharge) | вставка/щелчок отсека | **cast** ✅ | — |
| Огнетушитель (распыление) | шипение порошка под давлением | loop, пока жмёшь ✅ | **струя/облако порошка (VFX!)** |
| Бросок (G): зарядка | нарастающее усилие/замах 🟡 | loop пока заряжаешь 🟡 | индикатор заряда (HUD есть) |
| Бросок: отпустил | вжух броска | 1× ✅ | — |
| Бросок: попал/упал | удар предмета | 1× ✅ | — |
| Растяжка: поставил | щёлк установки | 1× ✅ | индикатор 🟡 |
| Растяжка: взвелась | тихий «бип» взвода | 1× ✅ | мигает лампа (есть) |
| Растяжка: сработала | громкий хлопок/трещотка | 1× ✅ | вспышка (есть) |
| Прожектор: поставил | стук треноги + гудёж лампы | 1× + loop тихий ✅ | свет (есть) |
| Биоснаряд: бросок/шлепок | вжух + «шлёп» | 1× ✅ | брызги 🟡 |

---

## 4. Починка (ядро игры) — каждый этап озвучен

| Событие | Звук | Тип | VFX |
|---|---|---|---|
| Начал взаимодействие (E) | тихий «беру в работу» 🟡 | 1× 🟡 | — |
| Вставка расходника (кабель/предохранитель) | металлический клик «вставлено» (НЕ «уронил») | 1× ✅ | — |
| Заливка бензина (AutoFill, генератор) | бульканье льющегося топлива | loop по полоске ✅ | струйка топлива 🟡 |
| Прокладка кабеля (AutoFill, щиток) | разматывание/возня с кабелем | loop по полоске ✅ | — |
| Щиток — тык тестером (Cursor, попал) | короткий «пик»/контакт | 1× ✅ | искорка 🟡 |
| Щиток — промах | злой «вз-з-з»/неверный тон | 1× ✅ | — |
| Труба — докрутка вентиля (Valve) | скрип/щелчок храповика на каждый тык | 1× ✅ | — |
| Труба — «сорвал резьбу» (часто) | металлический срыв + шипение | 1× ✅ | — |
| Генератор — натяжение стартера (держишь) | нарастающее «вж-ж» троса | loop пока тянешь ✅ | — |
| Генератор — рывок в зелёной зоне | щелчок/хлопок шнура | 1× ✅ | — |
| Генератор — «обратный удар» (рано/перетянул) | глухой стук/осечка | 1× ✅ | — |
| Генератор — ЗАВЁЛСЯ (починен) | двигатель схватывает → ровный холостой | 1× «завёлся» + **loop холостого хода на весь забег** ✅ | дымок из выхлопа 🟡 |
| Сварка (держишь, с инструментом) | электро-дуга шипит/трещит | loop, пока варишь ✅ | **искры дуги + свечение (VFX!)** + анимация |
| Любая починка завершена | «готово»/щелчок успеха | 1× ✅ | — |
| Колхоз (без инструмента) | возня/стук подручным | loop ✅ | — |
| Колхоз-косяк | звяк-облом + ругань 🟡 | 1× ✅ | искра/дымок 🟡 |
| Замыкание щитка (3 промаха) | мощный электро-разряд/хлопок | 1× ✅ | **дуга/искры по площади (VFX!)** + красный свет |

> **Сварка и стартер — связаны с анимацией.** Финальный звук завязываем на нормальную анимацию (как держит/варит/дёргает). Сейчас механика сварки на переделке (см. бэклог).

---

## 5. Опасности / мир

| Событие | Звук | Тип | VFX |
|---|---|---|---|
| Утечка газа | постоянное лёгкое шипение, **сильно затухает по расстоянию** (вблизи слышно, вдали еле-еле) | loop, 3D ✅ | **струя газа/пар (VFX!)** — лёгкий, НЕ густой чёрный дым |
| Облако газа разрослось | шипение чуть гуще 🟡 | loop ✅ | облако больше |
| Взрыв газа | мощный бабах + раскат + звон | 1×, 3D на всю карту ✅ | вспышка + ударная волна + **обломки + огонь** |
| После взрыва: горит | потрескивание огня | loop, пока горит ✅ | **огонь + дым (VFX!)** |
| Перегрузка сети (вырубило щиток) | хлоп реле + гул пропал | 1× ✅ | свет мигнул/погас |
| Восстановление света (починили) | гул сети + лампы зажглись | 1× ✅ | свет включился |
| Фоновая жуть (скрипы) | случайный скрип/стук/капля | 1× редко, 3D ✅ | — |
| Лужа пены (после огнетушителя) | — (звук при шаге по ней) | 🔇 | декаль пены |
| Капли воды / трубы (эмбиент) | кап-кап | loop редкий 🎚 | — |

---

## 6. Диспетчер / соц / кооп

| Событие | Звук | Тип | VFX |
|---|---|---|---|
| Реплика диспетчера (плашка) | **по умолчанию 🔇** (per-message бип раздражает). Опц. — ОДИН тихий короткий «бип эфира» 🟡 | 1× 🟡 | плашка (есть) |
| Голос диспетчера (озвучка реплик) | 🟡 будущее: реальная озвучка/TTS | — | — |
| Паническая «реплика» игрока (крик) | крик/вскрик | 1× ✅ | — |
| Толчок напарника (Q) | глухой удар/«эй!» | 1× ✅ | — |
| Совместный перекур | — | 🔇 | дымок 🟡 |
| Монитор оператора (Tab) вкл/выкл | щелчок/помеха экрана | 1× ✅ | — |

---

## 7. UI / меню

| Событие | Звук | Тип |
|---|---|---|
| Наведение на кнопку | тихий тик | 1× ✅ |
| Нажатие кнопки | клик | 1× ✅ |
| Открыть/закрыть меню/магазин | свуш/щелчок | 1× ✅ |
| Покупка апгрейда | «касса»/успех | 1× ✅ |
| Не хватает денег / ошибка | низкий «бз-з» | 1× ✅ |
| Старт забега | короткий стинг/гудок диспетчерской | 1× 🟡 |
| Победа (все починили, в ГАЗель) | позитивный стинг | 1× ✅ |
| Поражение (все ранены / провал квоты) | мрачный стинг | 1× ✅ |
| «Акт выполненных работ» (экран) | перелистывание/печать 🟡 | 1× 🟡 |

---

## 8. Эмбиент (фон уровня)

| Слой | Звук | Тип |
|---|---|---|
| Бед уровня (больница/подвал) | низкий гул, вентиляция, дальние звуки | loop 🎚 ✅ (есть бойлерная) |
| Ночь/ветер за окнами | ветер | loop тихий 🎚 🟡 |
| Дождь/гроза (если погода) | дождь + раскаты | loop 🎚 🟡 (Hyper VFX есть) |
| Электрический гул (пока свет есть) | трансформаторный гул 🟡 | loop тихий 🟡 |

---

## 9. Монстр-слухач (ОТЛОЖЕН — спека на будущее)

| Событие | Звук |
|---|---|
| Шаги монстра | тяжёлые шаги, 3D ✅ |
| Дыхание/рык рядом | дыхание/рык ✅ |
| Услышал шум → охота | «насторожился» стинг ✅ |
| Атака/джампскейр | резкий скример ✅ |
| Музыка преследования | нарастающее напряжение loop 🟡 |

> Вся шумовая база под монстра уже в коде (MakeNoise на всех событиях). Монстр — по отмашке.

---

## 10. VFX — сводка (что точно нужно сделать)

**Niagara/частицы:** ✅ взрыв (вспышка+ударная волна), ✅ огонь (горение, loop), ✅ дым (от огня), ✅ **газ-утечка (лёгкий пар/струя, НЕ чёрный дым)**, ✅ искры (щиток-замыкание, сварка), ✅ **струя огнетушителя (порошок/пена)**, ✅ обломки взрыва, 🟡 пыль под ногами, 🟡 брызги/капли, 🟡 пар от кофе/выхлопа.
**Декали:** ✅ копоть после взрыва, ✅ пятна пены, 🟡 грязь/потёки/кровь, 🟡 следы.
**Свет:** ✅ красная пульсация на сломанном (есть), ✅ луч фонаря (есть), ✅ вспышка взрыва/растяжки (есть), 🟡 искрение-свет при замыкании.
**Пост-эффекты/HUD:** ✅ виньетка паники (есть), 🟡 блюр/звон при пике паники, 🟡 хроматика при ранении.
**Небо/погода:** 🟡 ночное небо + гроза за окнами (Hyper, по желанию).

---

## 11. Технические правила реализации (как вкручиваем)

- **Разовый (1×):** `PlaySoundAtLocation` (3D) или мультикаст для коопа. Личное — `PlaySound2D` локально.
- **Loop по состоянию:** `UAudioComponent` на акторе, гоняется в Tick по **реплицируемому** флагу (utечка, заливка, сварка, распыление, спринт). Старт/стоп по состоянию → само гаснет при отмене.
- **Cast (применение):** компонент по `UseCastRemaining>0` (реплицируется) → играет всю длительность, **гаснет при отмене** (уже сделано так для аптечки).
- **3D-затухание:** у мировых звуков выставляем attenuation (радиус полной громкости + дистанция затухания). Газ/мелочь — малый радиус; машины/взрыв — большой.
- **Личное (2D):** сердцебиение/одышка — `bAllowSpatialization=false`, только локальный игрок.
- **Кооп:** мировые события — `NetMulticast`; шаги/лупы — per-character Tick по реплиц. скорости/флагам (каждый клиент сам).
- **Вариативность:** для частых — `USoundCue` со случайным выбором из 3–5 волн (или массив + random в коде).
- **Громкость/радиус — `EditAnywhere`** на каждом звуке, крутим без пересборки.

---

## 12. Приоритеты подключения (когда будет пак)

1. **Ядро забега:** шаги (по поверхностям), починка (вставка/заливка/тык/вентиль/стартер-завёлся/сварка-дуга), завершение починки, взрыв, утечка-газ (звук+VFX), замыкание.
2. **Предметы:** аптечка(cast), сигарета, кофе(глоток), фонарь, рация, огнетушитель(loop+VFX), бросок.
3. **Витал:** сердцебиение(паника), кашель/икота/одышка, ранение.
4. **UI/меню**, старт/победа/поражение.
5. **Эмбиент**, фоновая жуть.
6. **Монстр** — по отмашке.

> Что какой звук — на твоём паке подберём по этим описаниям. Пришлёшь пак — пройдёмся по таблицам и навесим (по тех-правилам §11), чтобы сразу было ровно и в кооп-корректно.

---

## Source: INCIDENT_FX_MAP.md

# INCIDENT_FX_MAP — авария → эффект + звук (единый лукап)

_2026-06-19. Связывает `EFFECTS_INVENTORY.md` (Niagara) + `SOUND_MAP.md` + 16 Cue в `/Game/Audio/SFX/Cues/`._
_Назначение: привязка к `ARepairable`/механикам — лукап, а не раскопки. Niagara = `/Game/…`; SFX = `SC_*` (Cue)._

---

## Аварии (опасности)

| Авария | Niagara (эффект) | SFX луп | SFX ваншот | Заметка |
|---|---|---|---|---|
| **Газ — утечка** | `Realistic_…/Smoke/NS_Smoke_7_acid` | `SC_GasLeak_Loop` | — | облако токсичное; перекрасить под бытовой газ — gap |
| **Газ — поджёгся** | `M5VFXVOL2/…/Reference/Fireloop/3_Nblowingfire_fwd_pt` (струйный факел) | `SC_Gas_Burn_Loop` | `SC_Gas_Ignite` | поджиг → переход в луп горения |
| **Газ — взрыв** | `NiagaraExamples/FX_Explosions/NS_Explosion` | — | `SC_Explosion_Gas` | indoor-вариант звука для комнат |
| **Вода — прорыв трубы (струя)** | `FluidNinjaLive/…/NS_WaterHose_SingleProjection` | `SC_Water_Jet_Loop` | — | направленная напорная струя |
| **Вода — разлив/потоп** | `FluidFlux/…/NS_FluxDefault` + `NS_InfiniteSurfaceMesh` | (амбиент воды) | — | требует настройки плагина FluidFlux |
| **Вода — капель** | `UltraDynamicSky/Particles/Standalone/Dripping_Static_Mesh` | — | `SC_Drip` | с потолка/трубы; подвал |
| **Электро — искры/КЗ** | `NiagaraExamples/FX_Sparks/NS_Spark_Continuous` | — | `SC_Sparks` | коротящий провод |
| **Электро — дуга (живой провод)** | `NiagaraExamples/FX_Ribbons/NS_TeslaCoil` | `SC_Power_Hum_Loop` (пока под напряжением) | `SC_Sparks` | оголённая проводка |
| **Электро — разряд по воде** | `NiagaraExamples/FX_Player/NS_Player_Electricity_Looping` (растянуть по луже) | `SC_Power_Hum_Loop` | — | **gap** — честный эффект = авторинг с глазами; кандидат Poison/доп. |
| **Сварка** | `Realistic_…/Sparks/NS_Sparks_1` | — | `SC_Weld` | дуга при заварке трубы |
| **Пар (утечка)** | `IndustrialFactory/Effects/Smoke_01/ns_SteamFast_01_01` | — *(steam-loop Cue нет)* | — | высоконапорный свисток |
| **Огонь (поверхность)** | `IndustrialFactory/Effects/Fire_01/ns_Fire_01_01` / `Realistic_…/Fire/NS_Fire_Wall` | — | — | пожар на статусе «Горит» |
| **Пыль / обвал** | `Realistic_…/Destruction/NS_Destruction_1_concrete` | — | (обвал — `Lib/door_impact/E_Demolition_*`) | крошка по поверхности |

## Интерактивные объекты / действия

| Объект/действие | Niagara | SFX | Заметка |
|---|---|---|---|
| **Щиток / рубильник** | (искры при КЗ — см. выше) | `SC_Power_Hum_Loop` (живой) + `SC_Panel_Switch` (щелчок) | механика свет↔питание уже в коде (тег `PoweredLight`) |
| **Дверь** | — | `SC_Door_Open` / `SC_Door_Close` | `ADoor` |
| **Вентиль (газ/вода)** | — | `SC_Valve_Turn` | перекрытие трубы |
| **Рация** | — | `SC_Radio` | вызов диспетчеру (см. `SPEC_Radio_Haggle.md`) |
| **Ремонт (Hold-этап)** | (искры/пар по типу) | `SC_Repair_Hit` (рандом) | каждый тик откручивания/удара |
| **Скрипы (ambient)** | — | `SC_Creak` (5-вар. рандом) | эмиттер с рандом-таймером по дому |
| **Комнатный тон** | — | `SC_RoomTone_*` (Hall/Corridor/Room/Bathroom/Basement/Stairs) | AmbientSound на комнату |

## Привязка (когда закроем редактор — ребилд)
- Аварийные лупы → `UAudioComponent` на `ARepairable`, старт/стоп по `bBroken`/состоянию.
- Эффект-спавн → `UNiagaraComponent` на `ARepairable`, активировать при поломке (gated по `EffectsQuality`).
- Ваншоты (взрыв/поджиг/щёлк) → `UGameplayStatics::PlaySoundAtLocation` в точке события.
- Дверь/вентиль/рация → Cue в соответствующие свойства/вызовы.

## Дыры (нужен заход с глазами / докупка)
- **Электро-по-воде** — нет готовой системы (растяжка `NS_Player_Electricity` — временно; кандидат: **Poison Magic Niagara** из `СКАЧАТЬ.md`).
- **Газ-облако бытовое** (без зелёного) — перекраска дыма (материал/цвет-параметр).
- **Steam-loop Cue** — не собран (есть визуал `ns_SteamFast`, нет звук-лупа; в базе пневмо-burst `Lib/gas/Air_Burst_*` подойдёт — собрать Cue).

---

## Source: EFFECTS_INVENTORY.md

# EFFECTS_INVENTORY — каталог Niagara/VFX под механики «Аварийки»

_Авто-рескан 2026-06-18 (multi-agent workflow, 8 паков → 280 spawnable-систем). Пути = /Game/...; источники паков локальны (gitignore)._

---

## Сводка

**Итого пригодных Niagara-систем: 280** (только spawnable NS_/ns_/N_ системы; Cascade-ParticleSystems, эмиттеры NE_/ne_, модули NM_/NMS_, материалы/текстуры/меши исключены авторами каталогов).

| Пак | Систем |
|---|---|
| NiagaraExamples + UltraDynamicSky/Particles | 76 |
| Realistic_Starter_VFX_Pack_Niagara_Vol2 | 60 |
| FluidFlux + FluidNinjaLive | 41 |
| IndustrialFactory | 32 |
| NiagaraExplosion01 + Fire_EXP_Vol01_Free | 30 |
| Warehouse Effects (/Game/Warehouse/Effects) | 18 |
| M5VFXVOL2 | 12 |
| Hyper/Interaction/ResourcePack (только Hyper) | 11 |

Распределение по категориям: explosion 34 · fire 32 · water_splash 48 · smoke_gas 22 · dust_debris 28 · rain 16 · sparks 17 · electric_arc 12 · mist_fog 11 · steam 10 · flood 2 · magic_misc 48.

---

## По категориям

### explosion (34)
- `/Game/NiagaraExamples/FX_Explosions/NS_Explosion` — чистый огненный взрыв, готов под взрыв газа (+`NS_Explosion_Medium`, `NS_Explosion_Small`)
- `/Game/NiagaraExamples/FX_Explosions/NS_Dirt_Explosion` — взрыв с землёй/обломками (+Medium, Small)
- `/Game/_Packs/Fire_EXP_Vol01_Free/Niagara/EXP/NS_Sub_EXP_Large_001_01` — крупный бласт, явно «gas-leak fit» (+Mid, Small)
- `/Game/M5VFXVOL2/Niagara/Explosion/NFire_Exp_00` — огне/газовый burst (+`NFire_Exp_01`, `NFire_Exp_03`)
- `/Game/NiagaraExplosion01/Niagaras/Air/N_ExplosionAir_001` — галерея воздушных бластов (+9 more)
- `/Game/NiagaraExplosion01/Niagaras/Ground/N_ExplosionGround_001` — галерея наземных бластов (+9 more)
- `/Game/NiagaraExamples/Utilities/SpriteGeneration/ExplosionRoil/NS_ExplosionRoil` — клубящийся roil-источник; `/Game/NiagaraExamples/FX_Misc/NS_FireworkBurst` — салютный burst

### fire (32)
- `/Game/IndustrialFactory/Effects/Fire_01/ns_Fire_01_01` — основной промышленный огонь (+Small/Torch/TorchSmoke/Candle, `ns_FireGuide_01_01` лента-трейл)
- `/Game/M5VFXVOL2/Niagara/Fire_for_BP/NFire_BP_00` — луп-пламя, BP-ready (+15 вариантов; `NFire_Grd_BP_00` напольный +2; Candle/Torch)
- `/Game/M5VFXVOL2/Niagara/Reference/Fireloop/3_Nblowingfire_fwd_pt` — направленный струйный факел = поджжённая газовая труба (+`4_Nbigfire_pt` крупный)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Fire/NS_Fire_Wall` — огонь по стене (+Ceiling/Floor/Window×2/Camp×2/Torch — набор под поверхности)
- `/Game/_Packs/Fire_EXP_Vol01_Free/Niagara/Fire/Loop/NS_Sub_FireLarge_Loop_004` — крупный луп-огонь (+Mid/Small Loop&Rate, Torch = 7)
- `/Game/NiagaraExamples/FX_Misc/NS_Fire` · `/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_Bonfire` · `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Environment/NS_Environment_5_flare` (сигнальный фаер)

### smoke_gas (22)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_7_acid` — **единственная штатная токсично-газовая туча**
- `/Game/NiagaraExamples/FX_Smoke/NS_Smoke_Plume` — плотный плюм, «toxic-gas ready» (+`NS_Chimney_Smoke`, `NS_SmokePuffLight`, `NS_RocketTrail`)
- `/Game/IndustrialFactory/Effects/Smoke_01/ns_ChimneySmoke_01_03_Large` — крупный пром-плюм, плейсхолдер газа (+S/M; Warehouse дублирует те же 3)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_1` — дженерик-дым (+`_2`/`_5`/`_8`, `_4_tornado`, `NS_Environment_6_burnout`)
- `/Game/M5VFXVOL2/Niagara/Reference/Smoke/5_6_Nsmoke6_pt` — тяжёлый дым (+тонкий `0_6_Nsmoke_pt`); `/Game/FluidNinjaLive/.../NS_BonfireSmoke` (+Smoke2, MagicCauldron vapor)

### mist_fog (11)
- `/Game/IndustrialFactory/Effects/Mist_01/ns_Mist_01_03_UnlitLarge` — крупная дешёвая unlit-мгла (+Lit Large/Small, Unlit Small; Warehouse дублирует ещё 4)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Environment/NS_Environment_7_volume` — объёмный fog-volume
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_3_coldsmoke` — низовой стелющийся туман, хоррор (+`NS_Environment_2_coldsmoke`)

### water_splash (48)
- `/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_WaterHose_SingleProjection` — напорная струя = прорыв трубы (+DoubleProjection)
- `/Game/UltraDynamicSky/Particles/Standalone/Dripping_Static_Mesh` — капель с меша/потолка (+`Dripping_Mesh`, `Dripping_Skeletal_Mesh`); `/Game/FluidNinjaLive/.../NS_LiquidDrops_WorldSpace` (+local, `NS_Demo_WateryDrizzleDown`)
- `/Game/FluidFlux/Interaction/Splash/Effects/NS_SimpleSplash` — брызги от взаимодействия (+`NS_FluxSplashArea`, `NS_FluxFoam`, `NS_Bubbles`)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Water/NS_Water_1` — спрей/всплеск галерея (+7 more)
- `/Game/IndustrialFactory/Effects/Waterfall_01/ns_Waterfall_Splash_01_04_Single` — водопад-набор (+4 части) + `ns_FallingSludge_Green_03_02_Large` токсичная капель-жижа (+чистая/грязная ×размеры)
- `/Game/FluidNinjaLive/.../NS_Acid_DoubleProjection` — кислотный спрей (токсичная жидкость); `/Game/UltraDynamicSky/Particles/Puddle_Splash` (+Ripple, FluidGrid); footstep-splashes ×4 (+N more)

### flood (2)
- `/Game/FluidFlux/Environment/Niagara/Default/NS_FluxDefault` — драйвер симуляции поверхности воды (реальный разлив/подъём уровня)
- `/Game/FluidFlux/Surface/Meshes/Niagara/NS_InfiniteSurfaceMesh` — рендер бесконечной водной поверхности

### rain (16)
- `/Game/_Packs/Hyper/ResourcePack/Effects/Weather/System/NS_Rain_Heavy` — сильный дождь (+Light, ThunderStormWithRain, SnowImproved, Blizzard, Hailstorm = 6)
- `/Game/UltraDynamicSky/Particles/Rain` — UDS-осадки (+Snow, Radial_Storm, Weather_Particles, GPU_WeatherParticles = 5)
- `/Game/FluidNinjaLive/.../NS_Rain_NonFluidDriven_Translucent` (+`NS_FluxRain`, снег/буран); `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Environment/NS_Environment_9_storm`

### sparks (17)
- `/Game/NiagaraExamples/FX_Sparks/NS_Spark_Continuous` — непрерывный поток искр = искрящий провод (+`NS_Spark_Burst`, `NS_Spark_Impact_Looping`)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Sparks/NS_Sparks_1` — сварка/электро-искры (+10 more)
- `/Game/NiagaraExamples/FX_Weapons/Impacts/NS_Impact_Metal` — искры удара по металлу (+`NS_MuzzleFlash`, `NS_NDC_Impacts`)

### electric_arc (12)
- `/Game/NiagaraExamples/FX_Ribbons/NS_TeslaCoil` — ветвящиеся дуги = оголённая проводка
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Lightning/NS_Lightning_1` — дуга/молния (+7 more)
- `/Game/NiagaraExamples/FX_Player/NS_Player_Electricity_Looping` — электро-аура (зона удара током); `/Game/UltraDynamicSky/Particles/Lightning_Strike` (+`Obscured_Lightning`)

### steam (10)
- `/Game/IndustrialFactory/Effects/Smoke_01/ns_SteamFast_01_01` — напорная струя пара = утечка из трубы/вентиля (+`ns_Steam_01_01` медленный, `ns_ManholeSteam` S/L; Warehouse дублирует все 4)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_6_steam` — плюм пара
- `/Game/UltraDynamicSky/Particles/Standalone/FreezingBreath` — пар-прокси (холодное дыхание)

### dust_debris (28)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Destruction/NS_Destruction_1_concrete` — пыль/обломки разрушения (набор 8 по поверхностям: бетон/плитка/стекло/дерево×3/камень)
- `/Game/NiagaraExamples/FX_Weapons/Impacts/NS_Impact_Concrete` — локальная крошка (+Glass, Wood, footsteps)
- `/Game/UltraDynamicSky/Particles/Dust` — воздушная пыль на всю комнату (+`Wind_Debris`)
- `/Game/_Packs/Hyper/ResourcePack/Effects/Particles/Environment/NS_Dusty` — пылинки (+Sandstorm, Heavy_Wind); `/Game/FluidFlux/Demo/Environment/Dust/NS_FluxDust` (+песок/обломки в воде, +N more)

### magic_misc (48) — амбиент/декор/SDK-демо, не опасности
- `/Game/IndustrialFactory/Effects/Birds_01/ns_Birds_01_01` — птицы (+насекомые, листва, интерактивная флора; Warehouse дублирует)
- `/Game/NiagaraExamples/FX_Markers/NS_Marker_Location` — маркер цели/задачи (+`Marker_Target`, `Pickup_*` свечение ×4)
- `/Game/M5VFXVOL2/Niagara/Reference/distortion/9_Ndistort_pt` — тепловое марево над огнём (полезно поверх пламени)
- декор воды FluidFlux (рыбы/ряска/планктон), телепорт/бафф-ауры, `NS_Aurora`, SkeletalMesh/Dino-демо (+N more — большинство это SDK-демо и геймплейные не-опасные эффекты)

---

## Лучшее под наши аварии

| Авария | Топ-пики (`/Game` пути) | Почему |
|---|---|---|
| **ГАЗ-облако (toxic)** | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_7_acid` | Единственная система, изначально сделанная как кислотно-токсичная туча — без перекраски |
| | `/Game/NiagaraExamples/FX_Smoke/NS_Smoke_Plume` | Плотный плюм, помеченный «toxic-gas ready»; перекрасить в жёлто-зелёный |
| | `/Game/IndustrialFactory/Effects/Smoke_01/ns_ChimneySmoke_01_03_Large` | Крупный промышленный плюм как утечка из стояка/оборудования |
| **ВОДА — струя из трубы** | `/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_WaterHose_SingleProjection` | Направленная напорная струя — ровно прорыв трубы под давлением |
| **ВОДА — разлив-поверхность** | `/Game/FluidFlux/Environment/Niagara/Default/NS_FluxDefault` (+`/Game/FluidFlux/Surface/Meshes/Niagara/NS_InfiniteSurfaceMesh`) | Реальная симуляция растекающейся воды = лужа/подтоп на полу |
| **ВОДА — капли** | `/Game/UltraDynamicSky/Particles/Standalone/Dripping_Static_Mesh` | Капель прямо со статик-меша (труба/потолок); альтернатива — `/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_LiquidDrops_WorldSpace` |
| **ЭЛЕКТРО — искры** | `/Game/NiagaraExamples/FX_Sparks/NS_Spark_Continuous` | Непрерывный поток искр из коротящего провода; альт `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Sparks/NS_Sparks_1` |
| **ЭЛЕКТРО — дуга-провод** | `/Game/NiagaraExamples/FX_Ribbons/NS_TeslaCoil` | Ветвящиеся дуги — идеально для оголённой/искрящей проводки; альт `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Lightning/NS_Lightning_1` |
| **ЭЛЕКТРО — разряд по воде** | `/Game/NiagaraExamples/FX_Player/NS_Player_Electricity_Looping` | Луп-аура электричества, растягивается на зону мокрого пола (точного «разряд по луже» нет — это ближайшее) |
| **ВЗРЫВ газа** | `/Game/NiagaraExamples/FX_Explosions/NS_Explosion` | Чистый огненный бласт, 3 размера, geared под взрыв газа |
| | `/Game/_Packs/Fire_EXP_Vol01_Free/Niagara/EXP/NS_Sub_EXP_Large_001_01` | Крупный бласт, помеченный «gas-leak fit» |
| | `/Game/M5VFXVOL2/Niagara/Explosion/NFire_Exp_00` | Огне/газовый burst-вариант для разнообразия |
| **ПАР** | `/Game/IndustrialFactory/Effects/Smoke_01/ns_SteamFast_01_01` | Напорная струя пара = утечка из вентиля/трубы |
| | `/Game/IndustrialFactory/Effects/Smoke_01/ns_Steam_01_01` | Медленный плюм для слабой утечки (есть и в Warehouse-паке) |
| | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_6_steam` | Альтернативный паровой плюм |
| **ПЫЛЬ/мусор** | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Destruction/NS_Destruction_1_concrete` | Пыль+обломки разрушения; есть весь набор по поверхностям (стекло/дерево/камень/плитка) |
| | `/Game/NiagaraExamples/FX_Weapons/Impacts/NS_Impact_Concrete` | Локальная крошка под удар/обвал в конкретной точке |
| | `/Game/UltraDynamicSky/Particles/Dust` | Висящая пыль на весь объём комнаты (атмосфера заброшки) |
| **Фон/амбиент (мгла/туман)** | `/Game/IndustrialFactory/Effects/Mist_01/ns_Mist_01_03_UnlitLarge` | Крупная дешёвая unlit-мгла для коридоров/подвала |
| | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_3_coldsmoke` | Низовой стелющийся туман — хоррор-настроение |
| | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Environment/NS_Environment_7_volume` | Объёмный fog-volume для крупных пространств |

---

## Пробелы

Опасности с **готовым** ассетом покрыты практически все — взрыв, огонь, пар, искры, дуга, пыль, туман, капель и струя воды есть «из коробки». Реальные дефициты:

- **Разряд тока по воде (электро+лужа)** — нет ни одной специализированной системы. Решается без редактора (растянуть `NS_Player_Electricity_Looping` / `NS_Lightning_*` по площади лужи), но «честный» эффект потребует **авторинга в Niagara**.
- **Подъём/разлив воды как готовый дроп-ин** — отдельной «flood»-системы нет; есть только сим-драйверы FluidFlux (`NS_FluxDefault` + `NS_InfiniteSurfaceMesh`), которые **требуют настройки плагина FluidFlux**, а не простого спавна. Niagara-редактор не нужен, но нужна интеграция плагина.
- **Чистое «газовое облако» тонко представлено** — по сути один штатный ассет (`NS_Smoke_7_acid`) + перекраска дымов. Если нужен отдельный вид именно бытового газа (метан, без цвета) — это материал/перекраска, либо лёгкий **авторинг в Niagara**.
- **Пар достаточного «промышленного» качества** есть (`ns_SteamFast_01_01`), но как высоконапорный белый свисток он один; вариативность — за счёт перекраски/масштаба, без редактора.

Категории `rain` и `magic_misc` для нашего жанра избыточны (погода/декор/SDK-демо) — в gameplay не нужны, кроме маркеров задач (`NS_Marker_Location`) и теплового марева над огнём (`9_Ndistort_pt`).

